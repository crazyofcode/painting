#include <types.h>
#include <memlayout.h>
#include <virt.h>
#include <macro.h>
#include <stdio.h>

void virtio_disk_init(void) {
  
	// 检查设备的魔术值、版本、设备ID和厂商ID，确保找到了virtio磁盘设备。如果条件不满足，会触发panic
	if (*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 || *R(VIRTIO_MMIO_VERSION) != 1 ||
	    *R(VIRTIO_MMIO_DEVICE_ID) != 2 || *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
		panic("could not find virtio disk");
	}
  log("virtio_disk_init finish\n");

}
