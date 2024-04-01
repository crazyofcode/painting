/*
 * created by lm.  3.31
 * 暂不支持 sdcard
*/

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"

#include "buf.h"

#ifndef QEMU
//  #include "sdcard.h"  //包含了与 SD 卡通信相关的定义和函数，用于在真实硬件上访问 SD 卡。
//  #include "dmac.h"    //包含了直接内存访问控制器（DMA Controller）相关的定义和函数，用于高效地进行数据传输。
#else
#include "virtio.h"  //virtio.h 文件可能包含了针对 QEMU 虚拟设备的定义和函数，用于在 QEMU 中模拟虚拟设备与系统进行通信。
#endif 

void disk_init(void)
{
    #ifdef QEMU
    virtio_disk_init();
	#else 
	// sdcard_init();
    #endif
}

void disk_read(struct buf *b)
{
    #ifdef QEMU
	virtio_disk_rw(b, 0);
    #else 
	// sdcard_read_sector(b->data, b->sectorno);
	#endif
}

void disk_write(struct buf *b)
{
    #ifdef QEMU
	virtio_disk_rw(b, 1);
    #else 
	// sdcard_write_sector(b->data, b->sectorno);
	#endif
}

void disk_intr(void)
{
    #ifdef QEMU
    virtio_disk_intr();
    #else 
    // dmac_intr(DMAC_CHANNEL0);
    #endif
}
