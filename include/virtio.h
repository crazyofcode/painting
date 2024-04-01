/*
* changed by lm. 3.31
*/

#ifndef __VIRTIO_H__
#define __VIRTIO_H__

/*add lm*/
#include "types.h"
#include "buf.h"

//
// virtio device definitions.
// for both the mmio interface, and virtio descriptors.
// only tested with qemu.
//
// the virtio spec:
// https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.pdf
//

// virtio mmio control registers, mapped starting at 0x10001000.
// from qemu virtio_mmio.h
#define VIRTIO_MMIO_MAGIC_VALUE		0x000 // 0x74726976
#define VIRTIO_MMIO_VERSION		0x004 // version; should be 2
#define VIRTIO_MMIO_DEVICE_ID		0x008 // device type; 1 is net, 2 is disk
#define VIRTIO_MMIO_VENDOR_ID		0x00c // 0x554d4551
#define VIRTIO_MMIO_DEVICE_FEATURES	0x010
#define VIRTIO_MMIO_DRIVER_FEATURES	0x020

#define VIRTIO_MMIO_GUEST_PAGE_SIZE	0x028 // page size for PFN, write-only .....

#define VIRTIO_MMIO_QUEUE_SEL		0x030 // select queue, write-only
#define VIRTIO_MMIO_QUEUE_NUM_MAX	0x034 // max size of current queue, read-only
#define VIRTIO_MMIO_QUEUE_NUM		0x038 // size of current queue, write-only

#define VIRTIO_MMIO_QUEUE_ALIGN		0x03c // used ring alignment, write-only ......
#define VIRTIO_MMIO_QUEUE_PFN		0x040 // physical page number for queue, read/write ......

#define VIRTIO_MMIO_QUEUE_READY		0x044 // ready bit
#define VIRTIO_MMIO_QUEUE_NOTIFY	0x050 // write-only
#define VIRTIO_MMIO_INTERRUPT_STATUS	0x060 // read-only
#define VIRTIO_MMIO_INTERRUPT_ACK	0x064 // write-only
#define VIRTIO_MMIO_STATUS		0x070 // read/write
#define VIRTIO_MMIO_QUEUE_DESC_LOW	0x080 // physical address for descriptor table, write-only
#define VIRTIO_MMIO_QUEUE_DESC_HIGH	0x084
#define VIRTIO_MMIO_DRIVER_DESC_LOW	0x090 // physical address for available ring, write-only
#define VIRTIO_MMIO_DRIVER_DESC_HIGH	0x094
#define VIRTIO_MMIO_DEVICE_DESC_LOW	0x0a0 // physical address for used ring, write-only
#define VIRTIO_MMIO_DEVICE_DESC_HIGH	0x0a4

// status register bits, from qemu virtio_config.h
#define VIRTIO_CONFIG_S_ACKNOWLEDGE	1
#define VIRTIO_CONFIG_S_DRIVER		2
#define VIRTIO_CONFIG_S_DRIVER_OK	4
#define VIRTIO_CONFIG_S_FEATURES_OK	8

// device feature bits
#define VIRTIO_BLK_F_RO              5	/* Disk is read-only */
#define VIRTIO_BLK_F_SCSI            7	/* Supports scsi command passthru */
#define VIRTIO_BLK_F_CONFIG_WCE     11	/* Writeback mode available in config */
#define VIRTIO_BLK_F_MQ             12	/* support more than one vq */
#define VIRTIO_F_ANY_LAYOUT         27
#define VIRTIO_RING_F_INDIRECT_DESC 28
#define VIRTIO_RING_F_EVENT_IDX     29

// this many virtio descriptors.
// must be a power of two.
#define NUM 8

// // a single descriptor, from the spec.
// struct virtq_desc {
//   uint64 addr;
//   uint32 len;
//   uint16 flags;
//   uint16 next;
// };
// #define VRING_DESC_F_NEXT  1 // chained with another descriptor
// #define VRING_DESC_F_WRITE 2 // device writes (vs read)

// // the (entire) avail ring, from the spec.
// struct virtq_avail {
//   uint16 flags; // always zero
//   uint16 idx;   // driver will write ring[idx] next
//   uint16 ring[NUM]; // descriptor numbers of chain heads
//   uint16 unused;
// };

// // one entry in the "used" ring, with which the
// // device tells the driver about completed requests.
// struct virtq_used_elem {
//   uint32 id;   // index of start of completed descriptor chain
//   uint32 len;
// };

// struct virtq_used {
//   uint16 flags; // always zero
//   uint16 idx;   // device increments when it adds a ring[] entry
//   struct virtq_used_elem ring[NUM];
// };

// // these are specific to virtio block devices, e.g. disks,
// // described in Section 5.2 of the spec.

// #define VIRTIO_BLK_T_IN  0 // read the disk
// #define VIRTIO_BLK_T_OUT 1 // write the disk

// // the format of the first descriptor in a disk request.
// // to be followed by two more descriptors containing
// // the block, and a one-byte status.
// struct virtio_blk_req {
//   uint32 type; // VIRTIO_BLK_T_IN or ..._OUT
//   uint32 reserved;
//   uint64 sector;
// };

// #define R(r) ((volatile uint32 *)(VIRTIO0 + (r)))

// static struct disk {
//   char pages[2 * PGSIZE];
  
//   // a set (not a ring) of DMA descriptors, with which the
//   // driver tells the device where to read and write individual
//   // disk operations. there are NUM descriptors.
//   // most commands consist of a "chain" (a linked list) of a couple of
//   // these descriptors.
//   struct virtq_desc *desc;

//   // a ring in which the driver writes descriptor numbers
//   // that the driver would like the device to process.  it only
//   // includes the head descriptor of each chain. the ring has
//   // NUM elements.
//   struct virtq_avail *avail;

//   // a ring in which the device writes descriptor numbers that
//   // the device has finished processing (just the head of each chain).
//   // there are NUM used ring entries.
//   struct virtq_used *used;

//   // our own book-keeping.
//   char free[NUM];  // is a descriptor free?
//   uint16 used_idx; // we've looked this far in used[2..NUM].

//   // track info about in-flight operations,
//   // for use when completion interrupt arrives.
//   // indexed by first descriptor index of chain.
//   struct {
//     struct buf *b;
//     char status;
//   } info[NUM];

//   // disk command headers.
//   // one-for-one with descriptors, for convenience.
//   struct virtio_blk_req ops[NUM];
  
//   struct spinlock vdisk_lock;
  
// } disk;

struct VRingDesc {
  uint64 addr;
  uint32 len;
  uint16 flags;
  uint16 next;
};
#define VRING_DESC_F_NEXT  1 // chained with another descriptor
#define VRING_DESC_F_WRITE 2 // device writes (vs read)

struct VRingUsedElem {
  uint32 id;   // index of start of completed descriptor chain
  uint32 len;
};

// for disk ops
#define VIRTIO_BLK_T_IN  0 // read the disk
#define VIRTIO_BLK_T_OUT 1 // write the disk

struct UsedArea {
  uint16 flags;
  uint16 id;
  struct VRingUsedElem elems[NUM];
};

void            virtio_disk_init(void);
void            virtio_disk_rw(struct buf *b, int write);
void            virtio_disk_intr(void);

#endif // !__VIRTIO_H__
