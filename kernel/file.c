#include "types.h"
#include "param.h"
#include "riscv.h"
#include "file.h"
#include "defs.h"

struct devsw devsw[NDEV];

int
read(uint64 addr, int offset, int n)
{
    if(n % 512 != 0 || offset % 512 != 0)
        return -1;

    char *buf = (char *)addr;
    int sector = offset / 512;
    int end = (offset + n) / 512;

    for(; sector < end; sector++, buf += 512)
        Virtioread(buf, sector);

    return n;
}

int
write(uint64 addr, int offset, int n)
{
    if(n % 512 != 0 || offset % 512 != 0)
        return -1;

    char *buf = (char *)addr;
    int sector = offset / 512;
    int end = (offset + n) / 512;

    for(; sector < end; sector++, buf += 512)
        Virtiowrite(buf, sector);

    return n;
}

void
devswinit()
{
    int i;
    for(i = 0; i < NDEV; i++) {
        if(i != 1) {
            devsw[i].read = read;
            devsw[i].write = write;
        }
    }

    // printf("devsw init finished\n");
}

int
RwDevRead(int dev, char *buf, int offset, int n)
{
    return devsw[dev].read((uint64)buf, offset, n);
}

int
RwDevWrite(int dev, char *buf, int offset, int n)
{
    return devsw[dev].write((uint64)buf, offset, n);
}