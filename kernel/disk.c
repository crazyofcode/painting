/*
 * 磁盘读写
*/

#include "include/types.h"
#include "include/param.h"
#include "include/memlayout.h"  /* changed */
#include "include/riscv.h"

#include "include/buf.h"
#include "include/virtio.h"

void disk_init(void)
{

    virtio_disk_init();

}

void disk_read(struct buf *b)
{
    
	virtio_disk_rw(b, 0);

}

void disk_write(struct buf *b)
{
  
	virtio_disk_rw(b, 1);

}
void disk_intr(void)
{
   
    virtio_disk_intr();

}