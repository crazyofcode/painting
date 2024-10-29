#ifndef VIRTIO_QUEUE_H__
#define VIRTIO_QUEUE_H__

/*this marks a buffer as continuing via the next filed*/
#define VIRTQ_DESC_F_NEXT 1  // chained with another descriptor
/*this marks a buffer as write-only (otherwise read_only)*/
#define VIRTQ_DESC_F_WRITE 2 // device writes (vs read)
/*this means the buffer contains a list of buffer descriptors*/
#define VIRTQ_DESC_F_INDIRECT 4

// a single descriptor, from the spec.
struct virtq_desc {
	uint64_t addr;
	uint32_t len;
	uint16_t flags;
	uint16_t next;
};

// this many virtio descriptors.
// must be a power of two.
#define NUM   8

// the (entire) avail ring, from the spec.
struct virtq_avail {
	uint16_t flags;	  // always zero
	uint16_t idx;	  // driver will write ring[idx] next
	uint16_t ring[NUM]; // descriptor numbers of chain heads
	uint16_t unused;
};

// one entry in the "used" ring, with which the
// device tells the driver about completed requests.
struct virtq_used_elem {
	uint32_t id; // index of start of completed descriptor chain
	uint32_t len;
};

struct virtq_used {
	uint16_t flags; // always zero
	uint16_t idx;   // device increments when it adds a ring[] entry
	struct virtq_used_elem ring[NUM];
	uint16_t unused;
};
#endif //!VIRTIO_QUEUE_H__
