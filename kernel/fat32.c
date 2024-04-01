
/*
 * Create by lm. 3.31 
 *对于文件系统，保留了xv6的disk、buf和file descriptor这几层的实现，而大体上做了以下修改：
+ FAT32不支持日志系统，我们去掉了xv6文件系统的log层（log.c）；
+ FAT32没有inode，文件的元数据直接存放在目录项中，因此我们去掉了`struct inode`，替换为目录项`struct dirent`（directory entry）；
+ FAT32没有link，因此删除了相关的系统调用；
+ 重新实现xv6文件系统（fs.c）中的各个函数，将函数接口中的inode替换成了entry，函数命名上保留原函数的特征但也做了修改以示区分，如`ilock`变为`elock`、`writei`变为`ewrite`等等；
+ 关于buf层，由于FAT32的一个簇的大小较大，并且依不同的存储设备而变，因此我们目前以扇区为单位作缓存。

*/

#include "param.h"
#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "buf.h"
#include "proc.h"
#include "stat.h"
#include "fat32.h"
#include "defs.h"    //其中包含 打印、字符串等



typedef struct short_name_entry {
    char        name[CHAR_SHORT_NAME];
    uint8       attr;  //文件属性

    uint16      fst_clus_hi;  //文件起始簇的高位
    uint16      fst_clus_lo;  //起始簇低位
    uint32      file_size;    //文件大小
    // uint8       _nt_res;
    // uint8       _crt_time_tenth;
    // uint16      _crt_time;
    // uint16      _crt_date;
    // uint16      _lst_acce_date;
    // uint16      _lst_wrt_time;
    // uint16      _lst_wrt_date;
} __attribute__((packed, aligned(4))) short_name_entry_t;

typedef struct long_name_entry {
    
    unsigned short     name1[5];
    unsigned short     name2[6];
    unsigned short     name3[2];
    uint8       order;        //长文件名的顺序
    uint8       attr;         //属性
    uint8       checksum;     //校验和，校验文件名的完整性
    // uint16      _fst_clus_lo;
    // uint8       _type;
    
} __attribute__((packed, aligned(4))) long_name_entry_t;

union dentry {
    short_name_entry_t  short_name;
    long_name_entry_t   long_name;
};

static struct {  //描述fat（文件分配表）文件系统的参数
    uint32  first_data_sec;       /* 数据区*/
    uint32  data_sec_cnt;
    uint32  data_clus_cnt;
    uint32  byts_per_clus;        /* 每簇的字节数 */

    struct {
        uint16  byts_per_sec;     /* 每扇区的字节数*/
        uint8   sec_per_clus;     /* 每簇的扇区数*/
        uint16  rsvd_sec_cnt;     /* 保留扇区数*/
        uint8   fat_cnt;          /* FAT区域的数量 */
        uint32  hidd_sec;         /* 隐藏扇区的数量 */
        uint32  tot_sec;          /* 包括所有区域的总扇区数 */
        uint32  fat_sz;           /* 每个FAT区域的扇区数 */
        uint32  root_clus;        /* 根目录的起始簇*/
    } bpb;

} fat;

static struct entry_cache {  // 文件 || 目录缓存
    struct spinlock lock;
    struct dirent entries[ENTRY_CACHE_NUM];
} ecache;

static struct dirent root; // 根目录


/*
 * fat32 初始化 读取boot扇区的数据
 * if SUCCESS ? 0 : -1
*/
int fat32_init(){
    /* test */
    printf("now i'm in fat32init!\n");
    struct buf *b = bread(0, 0);
    if (strncmp((char const*)(b->data + 82), "FAT32", 5))  /* 检查是否为 FAT 文件系统 */
        panic("not FAT32 volume");
    /*使用memmove函数从b->data的第12个字节（因为数组索引从0开始，所以11+1=12）开始，
    复制2个字节到fat.bpb.byts_per_sec。这通常是为了避免在某些平台上可能出现的未对齐访问问题。*/
    memmove(&fat.bpb.byts_per_sec, b->data + 11, 2);   

    fat.bpb.sec_per_clus = *(b->data + 13);                  /*每个簇包含的扇区数。*/
    fat.bpb.rsvd_sec_cnt = *(uint16 *)(b->data + 14);        /*保留扇区的数量*/
    fat.bpb.fat_cnt = *(b->data + 16);                       /*FAT表的数量*/
    fat.bpb.hidd_sec = *(uint32 *)(b->data + 28);            /*表示隐藏的扇区数*/
    fat.bpb.tot_sec = *(uint32 *)(b->data + 32);
    fat.bpb.fat_sz = *(uint32 *)(b->data + 36);              /*每个FAT表的大小（以扇区为单位）*/
    fat.bpb.root_clus = *(uint32 *)(b->data + 44);           /*根目录的起始簇号（对于FAT32文件系统，这个值通常是0，因为根目录是存储在FAT表中而不是在特定的簇中*/


    fat.first_data_sec = fat.bpb.rsvd_sec_cnt + fat.bpb.fat_cnt * fat.bpb.fat_sz; /*计算数据区的起始扇区号(保留扇区数加上FAT表的总大小)*/
    fat.data_sec_cnt = fat.bpb.tot_sec - fat.first_data_sec;                      /*数据区的扇区数(总扇区数减去数据区的起始扇区号)*/
    fat.data_clus_cnt = fat.data_sec_cnt / fat.bpb.sec_per_clus;                  /*数据区的簇数(数据区的扇区数除以每簇的扇区数)*/
    fat.byts_per_clus = fat.bpb.sec_per_clus * fat.bpb.byts_per_sec;              /*每个簇的字节数(每簇的扇区数乘以每扇区的字节数)*/
    brelse(b);
    /* test */
    printf("[FAT32 init]byts_per_sec: %d\n", fat.bpb.byts_per_sec);
    printf("[FAT32 init]root_clus: %d\n", fat.bpb.root_clus);
    printf("[FAT32 init]sec_per_clus: %d\n", fat.bpb.sec_per_clus);
    printf("[FAT32 init]fat_cnt: %d\n", fat.bpb.fat_cnt);
    printf("[FAT32 init]fat_sz: %d\n", fat.bpb.fat_sz);
    printf("[FAT32 init]first_data_sec: %d\n", fat.first_data_sec);

    // 检查 BSIZE 
    if (BSIZE != fat.bpb.byts_per_sec) 
        panic("byts_per_sec != BSIZE");
    initlock(&ecache.lock, "ecache");
    memset(&root, 0, sizeof(root));
    initsleeplock(&root.lock, "entry");

    root.attribute = (ATTR_DIRECTORY | ATTR_SYSTEM);        /*root目录项的属性，这里它被设置为目录（ATTR_DIRECTORY）和系统（ATTR_SYSTEM）属性。*/
    root.first_clus = root.cur_clus = fat.bpb.root_clus;    /*目录项的起始簇和当前簇为FAT BPB中读取的根目录簇号*/
    root.valid = 1;
    root.prev = &root;
    root.next = &root;
    /* 双向链表目录缓冲区 */
    for(struct dirent *de = ecache.entries; de < ecache.entries + ENTRY_CACHE_NUM; de++) {
        de->dev = 0;
        de->valid = 0;
        de->ref = 0;
        de->dirty = 0;
        de->parent = 0;
        de->next = root.next;
        de->prev = &root;
        initsleeplock(&de->lock, "entry");
        root.next->prev = de;
        root.next = de;
    }
    return 0;    
}

/*
 * 以下是一些参数的计算
 * 
*/
/*第一个簇的起始扇区号 簇号从 2 开始*/
static inline uint32 first_sec_of_clus(uint32 cluster){
    return ((cluster - 2) * fat.bpb.sec_per_clus) + fat.first_data_sec;
}
/*函数返回的是FAT表中对应簇号的扇区号*/
static inline uint32 fat_sec_of_clus(uint32 cluster, uint8 fat_num) /* 数据簇号、fat表编号*/
{
    return fat.bpb.rsvd_sec_cnt + (cluster << 2) / fat.bpb.byts_per_sec + fat.bpb.fat_sz * (fat_num - 1);
}
/*返回的是FAT表中对应簇号的扇区内的偏移量*/
static inline uint32 fat_offset_of_clus(uint32 cluster)
{
    return (cluster << 2) % fat.bpb.byts_per_sec;
}

/*
 * 读 fat 表中给定簇号对应的内容
 * if end ? current_cluster : next_cluster
*/
static uint32 read_fat(uint32 cluster)
{
    if (cluster >= FAT32_EOC) {  // 表示文件或目录结束
        return cluster;
    }

    if (cluster > fat.data_clus_cnt + 1) {     // because cluster number starts at 2, not 0
        return 0;                              // 0 表示无效
    }
    uint32 fat_sec = fat_sec_of_clus(cluster, 1);
    // here should be a cache layer for FAT table, but not implemented yet.
    struct buf *b = bread(0, fat_sec);

    /*使用 fat_offset_of_clus 函数计算簇在FAT表扇区内的偏移量，并从扇区数据中读取相应的4字节（uint32）内容，这代表簇的下一个簇号。*/
    uint32 next_clus = *(uint32 *)(b->data + fat_offset_of_clus(cluster)); 
    brelse(b);
    return next_clus;
}
/*
 * 在 FAT 表中写入指定簇号对应的内容(更新FAT表中某个簇的“下一个簇号”字段或者设置结束簇标记)
 * error   -1
 * success  0
*/
static int write_fat(uint32 cluster, uint32 content)
{
    if (cluster > fat.data_clus_cnt + 1) {
        return -1;
    }
    uint32 fat_sec = fat_sec_of_clus(cluster, 1);  /*计算给定簇号在FAT表中的扇区号*/
    struct buf *b = bread(0, fat_sec);
    uint off = fat_offset_of_clus(cluster);        /*计算簇在FAT表扇区内的偏移量。*/
    *(uint32 *)(b->data + off) = content;
    bwrite(b);                                     /* pay attion 没有判断写是否成功*/
    brelse(b);
    return 0;
}

/* 
 * 以下是对 fat 系统的管理
 * zero_clus、alloc_clus
*/
/*将指定簇的内容全部清零*/
static void zero_clus(uint32 cluster)
{
    uint32 sec = first_sec_of_clus(cluster);
    struct buf *b;
    for (int i = 0; i < fat.bpb.sec_per_clus; i++) {
        b = bread(0, sec++);
        memset(b->data, 0, BSIZE);
        bwrite(b);
        brelse(b);
    }
}
/*分配一个未使用的簇，并将其内容清零*/
static uint32 alloc_clus(uint8 dev)
{
    
    struct buf *b;
    uint32 sec = fat.bpb.rsvd_sec_cnt;  /*FAT表的起始扇区号*/
    uint32 const ent_per_sec = fat.bpb.byts_per_sec / sizeof(uint32); /*计算每扇区可以容纳的FAT项数量（ent_per_sec）*/
    for (uint32 i = 0; i < fat.bpb.fat_sz; i++, sec++) { /*外层循环遍历FAT表的每个扇区*/
        b = bread(dev, sec);
        for (uint32 j = 0; j < ent_per_sec; j++) {       /*内层循环遍历每个扇区中的FAT项*/
            if (((uint32 *)(b->data))[j] == 0) {         /*每个FAT项，检查其值是否为0*/
                ((uint32 *)(b->data))[j] = FAT32_EOC + 7;/*文件结束*/
                bwrite(b);
                brelse(b);
                uint32 clus = i * ent_per_sec + j;
                zero_clus(clus);
                return clus;
            }
        }
        brelse(b);
    }
    panic("no clusters");
}
/*释放某个簇*/
static void free_clus(uint32 cluster)
{
    write_fat(cluster, 0);
}
/*簇的读写
 * cluster        要操作的簇号
 * write          0(read)1(write)
 * user           区分数据来源 用户空间 || 内核空间
 * data           数据起始地址
 * off            簇内的偏移量
 * n              要读取写入的字节数
 * return tot     已处理的数据字节
*/
static uint rw_clus(uint32 cluster, int write, int user, uint64 data, uint off, uint n)
{
    if (off + n > fat.byts_per_clus)
        panic("offset out of range");
    uint tot, m;
    struct buf *bp;
    uint sec = first_sec_of_clus(cluster) + off / fat.bpb.byts_per_sec;
    off = off % fat.bpb.byts_per_sec;

    int bad = 0;
    for (tot = 0; tot < n; tot += m, off += m, data += m, sec++) {
        bp = bread(0, sec);
        m = BSIZE - off % BSIZE;
        if (n - tot < m) {
            m = n - tot;
        }
        if (write) {
            if ((bad = either_copyin(bp->data + (off % BSIZE), user, data, m)) != -1) {
                bwrite(bp);
            }
        } else {
            bad = either_copyout(user, data, bp->data + (off % BSIZE), m);
        }
        brelse(bp);
        if (bad == -1) {
            break;
        }
    }
    return tot;
}

/**
 * for the given entry, relocate the cur_clus field based on the off
 * @param   entry       modify its cur_clus field
 * @param   off         the offset from the beginning of the relative file
 * @param   alloc       whether alloc new cluster when meeting end of FAT chains
 * @return              the offset from the new cur_clus
 */
static int reloc_clus(struct dirent *entry, uint off, int alloc)
{
    int clus_num = off / fat.byts_per_clus;
    while (clus_num > entry->clus_cnt) {
        int clus = read_fat(entry->cur_clus);
        if (clus >= FAT32_EOC) {
            if (alloc) {
                clus = alloc_clus(entry->dev);
                write_fat(entry->cur_clus, clus);
            } else {
                entry->cur_clus = entry->first_clus;
                entry->clus_cnt = 0;
                return -1;
            }
        }
        entry->cur_clus = clus;
        entry->clus_cnt++;
    }
    if (clus_num < entry->clus_cnt) {
        entry->cur_clus = entry->first_clus;
        entry->clus_cnt = 0;
        while (entry->clus_cnt < clus_num) {
            entry->cur_clus = read_fat(entry->cur_clus);
            if (entry->cur_clus >= FAT32_EOC) {
                panic("reloc_clus");
            }
            entry->clus_cnt++;
        }
    }
    return off % fat.byts_per_clus;
}

/* like the original readi, but "reade" is odd, let alone "writee" */
// Caller must hold entry->lock.
int eread(struct dirent *entry, int user_dst, uint64 dst, uint off, uint n)
{
    if (off > entry->file_size || off + n < off || (entry->attribute & ATTR_DIRECTORY)) {
        return 0;
    }
    if (off + n > entry->file_size) {
        n = entry->file_size - off;
    }

    uint tot, m;
    for (tot = 0; entry->cur_clus < FAT32_EOC && tot < n; tot += m, off += m, dst += m) {
        reloc_clus(entry, off, 0);
        m = fat.byts_per_clus - off % fat.byts_per_clus;
        if (n - tot < m) {
            m = n - tot;
        }
        if (rw_clus(entry->cur_clus, 0, user_dst, dst, off % fat.byts_per_clus, m) != m) {
            break;
        }
    }
    return tot;
}

// Caller must hold entry->lock.
int ewrite(struct dirent *entry, int user_src, uint64 src, uint off, uint n)
{
    if (off > entry->file_size || off + n < off || (uint64)off + n > 0xffffffff
        || (entry->attribute & ATTR_READ_ONLY)) {
        return -1;
    }
    if (entry->first_clus == 0) {   // so file_size if 0 too, which requests off == 0
        entry->cur_clus = entry->first_clus = alloc_clus(entry->dev);
        entry->clus_cnt = 0;
        entry->dirty = 1;
    }
    uint tot, m;
    for (tot = 0; tot < n; tot += m, off += m, src += m) {
        reloc_clus(entry, off, 1);
        m = fat.byts_per_clus - off % fat.byts_per_clus;
        if (n - tot < m) {
            m = n - tot;
        }
        if (rw_clus(entry->cur_clus, 1, user_src, src, off % fat.byts_per_clus, m) != m) {
            break;
        }
    }
    if(n > 0) {
        if(off > entry->file_size) {
            entry->file_size = off;
            entry->dirty = 1;
        }
    }
    return tot;
}

// Returns a dirent struct. If name is given, check ecache. It is difficult to cache entries
// by their whole path. But when parsing a path, we open all the directories through it, 
// which forms a linked list from the final file to the root. Thus, we use the "parent" pointer 
// to recognize whether an entry with the "name" as given is really the file we want in the right path.
// Should never get root by eget, it's easy to understand.
static struct dirent *eget(struct dirent *parent, char *name)
{
    struct dirent *ep;
    acquire(&ecache.lock);
    if (name) {
        for (ep = root.next; ep != &root; ep = ep->next) {          // LRU algo
            if (ep->valid == 1 && ep->parent == parent
                && strncmp(ep->filename, name, FAT32_MAX_FILENAME) == 0) {
                if (ep->ref++ == 0) {
                    ep->parent->ref++;
                }
                release(&ecache.lock);
                // edup(ep->parent);
                return ep;
            }
        }
    }
    for (ep = root.prev; ep != &root; ep = ep->prev) {              // LRU algo
        if (ep->ref == 0) {
            ep->ref = 1;
            ep->dev = parent->dev;
            ep->off = 0;
            ep->valid = 0;
            ep->dirty = 0;
            release(&ecache.lock);
            return ep;
        }
    }
    panic("eget: insufficient ecache");
    return 0;
}

// trim ' ' in the head and tail, '.' in head, and test legality
char *formatname(char *name)
{
    static char illegal[] = { '\"', '*', '/', ':', '<', '>', '?', '\\', '|', 0 };
    char *p;
    while (*name == ' ' || *name == '.') { name++; }
    for (p = name; *p; p++) {
        char c = *p;
        if (c < 0x20 || strchr(illegal, c)) {
            return 0;
        }
    }
    while (p-- > name) {
        if (*p != ' ') {
            p[1] = '\0';
            break;
        }
    }
    return name;
}

static void generate_shortname(char *shortname, char *name)
{
    static char illegal[] = { '+', ',', ';', '=', '[', ']', 0 };   // these are legal in l-n-e but not s-n-e
    int i = 0;
    char c, *p = name;
    for (int j = strlen(name) - 1; j >= 0; j--) {
        if (name[j] == '.') {
            p = name + j;
            break;
        }
    }
    while (i < CHAR_SHORT_NAME && (c = *name++)) {
        if (i == 8 && p) {
            if (p + 1 < name) { break; }            // no '.'
            else {
                name = p + 1, p = 0;
                continue;
            }
        }
        if (c == ' ') { continue; }
        if (c == '.') {
            if (name > p) {                    // last '.'
                memset(shortname + i, ' ', 8 - i);
                i = 8, p = 0;
            }
            continue;
        }
        if (c >= 'a' && c <= 'z') {
            c += 'A' - 'a';
        } else {
            if (strchr(illegal, c) != NULL) {
                c = '_';
            }
        }
        shortname[i++] = c;
    }
    while (i < CHAR_SHORT_NAME) {
        shortname[i++] = ' ';
    }
}

uint8 cal_checksum(uchar* shortname)
{
    uint8 sum = 0;
    for (int i = CHAR_SHORT_NAME; i != 0; i--) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + *shortname++;
    }
    return sum;
}

/**
 * Generate an on disk format entry and write to the disk. Caller must hold dp->lock
 * @param   dp          the directory
 * @param   ep          entry to write on disk
 * @param   off         offset int the dp, should be calculated via dirlookup before calling this
 */
void emake(struct dirent *dp, struct dirent *ep, uint off)
{
    if (!(dp->attribute & ATTR_DIRECTORY))
        panic("emake: not dir");
    if (off % sizeof(union dentry))
        panic("emake: not aligned");
    
    union dentry de;
    memset(&de, 0, sizeof(de));
    if (off <= 32) {
        if (off == 0) {
            strncpy(de.short_name.name, ".          ", sizeof(de.short_name.name));
        } else {
            strncpy(de.short_name.name, "..         ", sizeof(de.short_name.name));
        }
        de.short_name.attr = ATTR_DIRECTORY;
        de.short_name.fst_clus_hi = (uint16)(ep->first_clus >> 16);        // first clus high 16 bits
        de.short_name.fst_clus_lo = (uint16)(ep->first_clus & 0xffff);       // low 16 bits
        de.short_name.file_size = 0;                                       // filesize is updated in eupdate()
        off = reloc_clus(dp, off, 1);
        rw_clus(dp->cur_clus, 1, 0, (uint64)&de, off, sizeof(de));
    } else {
        int entcnt = (strlen(ep->filename) + CHAR_LONG_NAME - 1) / CHAR_LONG_NAME;   // count of l-n-entries, rounds up
        char shortname[CHAR_SHORT_NAME + 1];
        memset(shortname, 0, sizeof(shortname));
        generate_shortname(shortname, ep->filename);
        de.long_name.checksum = cal_checksum((uchar *)shortname);
        de.long_name.attr = ATTR_LONG_NAME;
        for (int i = entcnt; i > 0; i--) {
            if ((de.long_name.order = i) == entcnt) {
                de.long_name.order |= LAST_LONG_ENTRY;
            }
            char *p = ep->filename + (i - 1) * CHAR_LONG_NAME;
            uint8 *w = (uint8 *)de.long_name.name1;
            int end = 0;
            for (int j = 1; j <= CHAR_LONG_NAME; j++) {
                if (end) {
                    *w++ = 0xff;            // on k210, unaligned reading is illegal
                    *w++ = 0xff;
                } else { 
                    if ((*w++ = *p++) == 0) {
                        end = 1;
                    }
                    *w++ = 0;
                }
                switch (j) {
                    case 5:     w = (uint8 *)de.long_name.name2; break;
                    case 11:    w = (uint8 *)de.long_name.name3; break;
                }
            }
            uint off2 = reloc_clus(dp, off, 1);
            rw_clus(dp->cur_clus, 1, 0, (uint64)&de, off2, sizeof(de));
            off += sizeof(de);
        }
        memset(&de, 0, sizeof(de));
        strncpy(de.short_name.name, shortname, sizeof(de.short_name.name));
        de.short_name.attr = ep->attribute;
        de.short_name.fst_clus_hi = (uint16)(ep->first_clus >> 16);      // first clus high 16 bits
        de.short_name.fst_clus_lo = (uint16)(ep->first_clus & 0xffff);     // low 16 bits
        de.short_name.file_size = ep->file_size;                         // filesize is updated in eupdate()
        off = reloc_clus(dp, off, 1);
        rw_clus(dp->cur_clus, 1, 0, (uint64)&de, off, sizeof(de));
    }
}

/**
 * Allocate an entry on disk. Caller must hold dp->lock.
 */
struct dirent *ealloc(struct dirent *dp, char *name, int attr)
{
    if (!(dp->attribute & ATTR_DIRECTORY)) {
        panic("ealloc not dir");
    }
    if (dp->valid != 1 || !(name = formatname(name))) {        // detect illegal character
        return NULL;
    }
    struct dirent *ep;
    uint off = 0;
    if ((ep = dirlookup(dp, name, &off)) != 0) {      // entry exists
        return ep;
    }
    ep = eget(dp, name);
    elock(ep);
    ep->attribute = attr;
    ep->file_size = 0;
    ep->first_clus = 0;
    ep->parent = edup(dp);
    ep->off = off;
    ep->clus_cnt = 0;
    ep->cur_clus = 0;
    ep->dirty = 0;
    strncpy(ep->filename, name, FAT32_MAX_FILENAME);
    ep->filename[FAT32_MAX_FILENAME] = '\0';
    if (attr == ATTR_DIRECTORY) {    // generate "." and ".." for ep
        ep->attribute |= ATTR_DIRECTORY;
        ep->cur_clus = ep->first_clus = alloc_clus(dp->dev);
        emake(ep, ep, 0);
        emake(ep, dp, 32);
    } else {
        ep->attribute |= ATTR_ARCHIVE;
    }
    emake(dp, ep, off);
    ep->valid = 1;
    eunlock(ep);
    return ep;
}

struct dirent *edup(struct dirent *entry)
{
    if (entry != 0) {
        acquire(&ecache.lock);
        entry->ref++;
        release(&ecache.lock);
    }
    return entry;
}

// Only update filesize and first cluster in this case.
// caller must hold entry->parent->lock
void eupdate(struct dirent *entry)
{
    if (!entry->dirty || entry->valid != 1) { return; }
    uint entcnt = 0;
    uint32 off = reloc_clus(entry->parent, entry->off, 0);
    rw_clus(entry->parent->cur_clus, 0, 0, (uint64) &entcnt, off, 1);
    entcnt &= ~LAST_LONG_ENTRY;
    off = reloc_clus(entry->parent, entry->off + (entcnt << 5), 0);
    union dentry de;
    rw_clus(entry->parent->cur_clus, 0, 0, (uint64)&de, off, sizeof(de));
    de.short_name.fst_clus_hi = (uint16)(entry->first_clus >> 16);
    de.short_name.fst_clus_lo = (uint16)(entry->first_clus & 0xffff);
    de.short_name.file_size = entry->file_size;
    rw_clus(entry->parent->cur_clus, 1, 0, (uint64)&de, off, sizeof(de));
    entry->dirty = 0;
}

// caller must hold entry->lock
// caller must hold entry->parent->lock
// remove the entry in its parent directory
void eremove(struct dirent *entry)
{
    if (entry->valid != 1) { return; }
    uint entcnt = 0;
    uint32 off = entry->off;
    uint32 off2 = reloc_clus(entry->parent, off, 0);
    rw_clus(entry->parent->cur_clus, 0, 0, (uint64) &entcnt, off2, 1);
    entcnt &= ~LAST_LONG_ENTRY;
    uint8 flag = EMPTY_ENTRY;
    for (int i = 0; i <= entcnt; i++) {
        rw_clus(entry->parent->cur_clus, 1, 0, (uint64) &flag, off2, 1);
        off += 32;
        off2 = reloc_clus(entry->parent, off, 0);
    }
    entry->valid = -1;
}

// truncate a file
// caller must hold entry->lock
void etrunc(struct dirent *entry)
{
    for (uint32 clus = entry->first_clus; clus >= 2 && clus < FAT32_EOC; ) {
        uint32 next = read_fat(clus);
        free_clus(clus);
        clus = next;
    }
    entry->file_size = 0;
    entry->first_clus = 0;
    entry->dirty = 1;
}

void elock(struct dirent *entry)
{
    if (entry == 0 || entry->ref < 1)
        panic("elock");
    acquiresleep(&entry->lock);
}

void eunlock(struct dirent *entry)
{
    if (entry == 0 || !holdingsleep(&entry->lock) || entry->ref < 1)
        panic("eunlock");
    releasesleep(&entry->lock);
}

void eput(struct dirent *entry)
{
    acquire(&ecache.lock);
    if (entry != &root && entry->valid != 0 && entry->ref == 1) {
        // ref == 1 means no other process can have entry locked,
        // so this acquiresleep() won't block (or deadlock).
        acquiresleep(&entry->lock);
        entry->next->prev = entry->prev;
        entry->prev->next = entry->next;
        entry->next = root.next;
        entry->prev = &root;
        root.next->prev = entry;
        root.next = entry;
        release(&ecache.lock);
        if (entry->valid == -1) {       // this means some one has called eremove()
            etrunc(entry);
        } else {
            elock(entry->parent);
            eupdate(entry);
            eunlock(entry->parent);
        }
        releasesleep(&entry->lock);

        // Once entry->ref decreases down to 0, we can't guarantee the entry->parent field remains unchanged.
        // Because eget() may take the entry away and write it.
        struct dirent *eparent = entry->parent;
        acquire(&ecache.lock);
        entry->ref--;
        release(&ecache.lock);
        if (entry->ref == 0) {
            eput(eparent);
        }
        return;
    }
    entry->ref--;
    release(&ecache.lock);
}

void estat(struct dirent *de, struct stat *st)
{
    strncpy(st->name, de->filename, STAT_MAX_NAME);
    st->type = (de->attribute & ATTR_DIRECTORY) ? T_DIR : T_FILE;
    st->dev = de->dev;
    st->size = de->file_size;
}

/**
 * Read filename from directory entry.
 * @param   buffer      pointer to the array that stores the name
 * @param   raw_entry   pointer to the entry in a sector buffer
 * @param   islong      if non-zero, read as l-n-e, otherwise s-n-e.
 */
static void read_entry_name(char *buffer, union dentry *d)
{
    if (d->long_name.attr == ATTR_LONG_NAME) {                       // long entry branch
        unsigned short temp[NELEM(d->long_name.name1)];
        memmove(temp, d->long_name.name1, sizeof(temp));
        snstr(buffer, temp, NELEM(d->long_name.name1));
        buffer += NELEM(d->long_name.name1);
        snstr(buffer, d->long_name.name2, NELEM(d->long_name.name2));
        buffer += NELEM(d->long_name.name2);
        snstr(buffer, d->long_name.name3, NELEM(d->long_name.name3));
    } else {
        // assert: only "." and ".." will enter this branch
        memset(buffer, 0, CHAR_SHORT_NAME + 2); // plus '.' and '\0'
        int i;
        for (i = 0; d->short_name.name[i] != ' ' && i < 8; i++) {
            buffer[i] = d->short_name.name[i];
        }
        if (d->short_name.name[8] != ' ') {
            buffer[i++] = '.';
        }
        for (int j = 8; j < CHAR_SHORT_NAME; j++, i++) {
            if (d->short_name.name[j] == ' ') { break; }
            buffer[i] = d->short_name.name[j];
        }
    }
}

/**
 * Read entry_info from directory entry.
 * @param   entry       pointer to the structure that stores the entry info
 * @param   raw_entry   pointer to the entry in a sector buffer
 */
static void read_entry_info(struct dirent *entry, union dentry *d)
{
    entry->attribute = d->short_name.attr;
    entry->first_clus = ((uint32)d->short_name.fst_clus_hi << 16) | d->short_name.fst_clus_lo;
    entry->file_size = d->short_name.file_size;
    entry->cur_clus = entry->first_clus;
    entry->clus_cnt = 0;
}

/**
 * Read a directory from off, parse the next entry(ies) associated with one file, or find empty entry slots.
 * Caller must hold dp->lock.
 * @param   dp      the directory
 * @param   ep      the struct to be written with info
 * @param   off     offset off the directory
 * @param   count   to write the count of entries
 * @return  -1      meet the end of dir
 *          0       find empty slots
 *          1       find a file with all its entries
 */
int enext(struct dirent *dp, struct dirent *ep, uint off, int *count)
{
    if (!(dp->attribute & ATTR_DIRECTORY))
        panic("enext not dir");
    if (ep->valid)
        panic("enext ep valid");
    if (off % 32)
        panic("enext not align");
    if (dp->valid != 1) { return -1; }

    union dentry de;
    int cnt = 0;
    memset(ep->filename, 0, FAT32_MAX_FILENAME + 1);
    for (int off2; (off2 = reloc_clus(dp, off, 0)) != -1; off += 32) {
        if (rw_clus(dp->cur_clus, 0, 0, (uint64)&de, off2, 32) != 32 || de.long_name.order == END_OF_ENTRY) {
            return -1;
        }
        if (de.long_name.order == EMPTY_ENTRY) {
            cnt++;
            continue;
        } else if (cnt) {
            *count = cnt;
            return 0;
        }
        if (de.long_name.attr == ATTR_LONG_NAME) {
            int lcnt = de.long_name.order & ~LAST_LONG_ENTRY;
            if (de.long_name.order & LAST_LONG_ENTRY) {
                *count = lcnt + 1;                              // plus the s-n-e;
                count = 0;
            }
            read_entry_name(ep->filename + (lcnt - 1) * CHAR_LONG_NAME, &de);
        } else {
            if (count) {
                *count = 1;
                read_entry_name(ep->filename, &de);
            }
            read_entry_info(ep, &de);
            return 1;
        }
    }
    return -1;
}

/**
 * Seacher for the entry in a directory and return a structure. Besides, record the offset of
 * some continuous empty slots that can fit the length of filename.
 * Caller must hold entry->lock.
 * @param   dp          entry of a directory file
 * @param   filename    target filename
 * @param   poff        offset of proper empty entry slots from the beginning of the dir
 */
struct dirent *dirlookup(struct dirent *dp, char *filename, uint *poff)
{
    if (!(dp->attribute & ATTR_DIRECTORY))
        panic("dirlookup not DIR");
    if (strncmp(filename, ".", FAT32_MAX_FILENAME) == 0) {
        return edup(dp);
    } else if (strncmp(filename, "..", FAT32_MAX_FILENAME) == 0) {
        if (dp == &root) {
            return edup(&root);
        }
        return edup(dp->parent);
    }
    if (dp->valid != 1) {
        return NULL;
    }
    struct dirent *ep = eget(dp, filename);
    if (ep->valid == 1) { return ep; }                               // ecache hits

    int len = strlen(filename);
    int entcnt = (len + CHAR_LONG_NAME - 1) / CHAR_LONG_NAME + 1;   // count of l-n-entries, rounds up. plus s-n-e
    int count = 0;
    int type;
    uint off = 0;
    reloc_clus(dp, 0, 0);
    while ((type = enext(dp, ep, off, &count) != -1)) {
        if (type == 0) {
            if (poff && count >= entcnt) {
                *poff = off;
                poff = 0;
            }
        } else if (strncmp(filename, ep->filename, FAT32_MAX_FILENAME) == 0) {
            ep->parent = edup(dp);
            ep->off = off;
            ep->valid = 1;
            return ep;
        }
        off += count << 5;
    }
    if (poff) {
        *poff = off;
    }
    eput(ep);
    return NULL;
}

static char *skipelem(char *path, char *name)
{
    while (*path == '/') {
        path++;
    }
    if (*path == 0) { return NULL; }
    char *s = path;
    while (*path != '/' && *path != 0) {
        path++;
    }
    int len = path - s;
    if (len > FAT32_MAX_FILENAME) {
        len = FAT32_MAX_FILENAME;
    }
    name[len] = 0;
    memmove(name, s, len);
    while (*path == '/') {
        path++;
    }
    return path;
}

// FAT32 version of namex in xv6's original file system.
static struct dirent *lookup_path(char *path, int parent, char *name)
{
    struct dirent *entry, *next;
    if (*path == '/') {
        entry = edup(&root);
    } else if (*path != '\0') {
        entry = edup(myproc()->cwd);
    } else {
        return NULL;
    }
    while ((path = skipelem(path, name)) != 0) {
        elock(entry);
        if (!(entry->attribute & ATTR_DIRECTORY)) {
            eunlock(entry);
            eput(entry);
            return NULL;
        }
        if (parent && *path == '\0') {
            eunlock(entry);
            return entry;
        }
        if ((next = dirlookup(entry, name, 0)) == 0) {
            eunlock(entry);
            eput(entry);
            return NULL;
        }
        eunlock(entry);
        eput(entry);
        entry = next;
    }
    if (parent) {
        eput(entry);
        return NULL;
    }
    return entry;
}

struct dirent *ename(char *path)
{
    char name[FAT32_MAX_FILENAME + 1];
    return lookup_path(path, 0, name);
}

struct dirent *enameparent(char *path, char *name)
{
    return lookup_path(path, 1, name);
}
