#ifndef VIRTIO_BLK_H__
#define VIRTIO_BLK_H__

/* Feature bits */
#define VIRTIO_BLK_F_SIZE_MAX	1	/* Indicates maximum segment size */
#define VIRTIO_BLK_F_SEG_MAX	2	/* Indicates maximum # of segments */
#define VIRTIO_BLK_F_GEOMETRY	4	/* Legacy geometry available  */
#define VIRTIO_BLK_F_RO		5	/* Disk is read-only */
#define VIRTIO_BLK_F_BLK_SIZE	6	/* Block size of disk is available*/
#define VIRTIO_BLK_F_TOPOLOGY	10	/* Topology information is available */
#define VIRTIO_BLK_F_MQ		12	/* support more than one vq */
#define VIRTIO_BLK_F_DISCARD	13	/* DISCARD is supported */
#define VIRTIO_BLK_F_WRITE_ZEROES	14	/* WRITE ZEROES is supported */
#define VIRTIO_BLK_F_SECURE_ERASE	16 /* Secure Erase is supported */
#define VIRTIO_BLK_F_ZONED		17	/* Zoned block device */

/* Legacy feature bits */
#ifndef VIRTIO_BLK_NO_LEGACY
#define VIRTIO_BLK_F_BARRIER	0	/* Does host support barriers? */
#define VIRTIO_BLK_F_SCSI	7	/* Supports scsi command passthru */
#define VIRTIO_BLK_F_FLUSH	9	/* Flush command supported */
#define VIRTIO_BLK_F_CONFIG_WCE	11	/* Writeback mode available in config */
/* Old (deprecated) name for VIRTIO_BLK_F_FLUSH. */
#define VIRTIO_BLK_F_WCE VIRTIO_BLK_F_FLUSH
#endif /* !VIRTIO_BLK_NO_LEGACY */

#define VIRTIO_BLK_ID_BYTES	20	/* ID string length */
#define VIRTIO_F_INDIRECT_DESC  28
#define VIRTIO_F_EVENT_IDX  29

/*
 * Command types
 *
 * Usage is a bit tricky as some bits are used as flags and some are not.
 *
 * Rules:
 *   VIRTIO_BLK_T_OUT may be combined with VIRTIO_BLK_T_SCSI_CMD or
 *   VIRTIO_BLK_T_BARRIER.  VIRTIO_BLK_T_FLUSH is a command of its own
 *   and may not be combined with any of the other flags.
 */

/* These two define direction. */
#define VIRTIO_BLK_T_IN		0
#define VIRTIO_BLK_T_OUT	1

#ifndef VIRTIO_BLK_NO_LEGACY
/* This bit says it's a scsi command, not an actual read or write. */
#define VIRTIO_BLK_T_SCSI_CMD	2
#endif /* VIRTIO_BLK_NO_LEGACY */

/* Cache flush command */
#define VIRTIO_BLK_T_FLUSH	4

/* Get device ID command */
#define VIRTIO_BLK_T_GET_ID    8

/* Discard command */
#define VIRTIO_BLK_T_DISCARD	11

/* Write zeroes command */
#define VIRTIO_BLK_T_WRITE_ZEROES	13

/* Secure erase command */
#define VIRTIO_BLK_T_SECURE_ERASE	14

/* Zone append command */
#define VIRTIO_BLK_T_ZONE_APPEND    15

/* Report zones command */
#define VIRTIO_BLK_T_ZONE_REPORT    16

/* Open zone command */
#define VIRTIO_BLK_T_ZONE_OPEN      18

/* Close zone command */
#define VIRTIO_BLK_T_ZONE_CLOSE     20

/* Finish zone command */
#define VIRTIO_BLK_T_ZONE_FINISH    22

/* Reset zone command */
#define VIRTIO_BLK_T_ZONE_RESET     24

/* Reset All zones command */
#define VIRTIO_BLK_T_ZONE_RESET_ALL 26

#ifndef VIRTIO_BLK_NO_LEGACY
/* Barrier before this op. */
#define VIRTIO_BLK_T_BARRIER	0x80000000
#endif /* !VIRTIO_BLK_NO_LEGACY */

/* Conventional zone */
#define VIRTIO_BLK_ZT_CONV         1
/* Sequential Write Required zone */
#define VIRTIO_BLK_ZT_SWR          2
/* Sequential Write Preferred zone */
#define VIRTIO_BLK_ZT_SWP          3

/*
 * Zone states that are available for zones of all types.
 */

/* Not a write pointer (conventional zones only) */
#define VIRTIO_BLK_ZS_NOT_WP       0
/* Empty */
#define VIRTIO_BLK_ZS_EMPTY        1
/* Implicitly Open */
#define VIRTIO_BLK_ZS_IOPEN        2
/* Explicitly Open */
#define VIRTIO_BLK_ZS_EOPEN        3
/* Closed */
#define VIRTIO_BLK_ZS_CLOSED       4
/* Read-Only */
#define VIRTIO_BLK_ZS_RDONLY       13
/* Full */
#define VIRTIO_BLK_ZS_FULL         14
/* Offline */
#define VIRTIO_BLK_ZS_OFFLINE      15

/* Unmap this range (only valid for write zeroes command) */
#define VIRTIO_BLK_WRITE_ZEROES_FLAG_UNMAP	0x00000001

/* Discard/write zeroes range for each request. */
struct virtio_blk_discard_write_zeroes {
	/* discard/write zeroes start sector */
	uint64_t sector;
	/* number of discard/write zeroes sectors */
	uint32_t num_sectors;
	/* flags for this range */
	uint32_t flags;
};

/* And this is the final byte of the write scatter-gather list. */
#define VIRTIO_BLK_S_OK		0
#define VIRTIO_BLK_S_IOERR	1
#define VIRTIO_BLK_S_UNSUPP	2

/* Error codes that are specific to zoned block devices */
#define VIRTIO_BLK_S_ZONE_INVALID_CMD     3
#define VIRTIO_BLK_S_ZONE_UNALIGNED_WP    4
#define VIRTIO_BLK_S_ZONE_OPEN_RESOURCE   5
#define VIRTIO_BLK_S_ZONE_ACTIVE_RESOURCE 6

struct virtio_blk_req {
  uint32_t type;
  uint32_t reserved;
  uint64_t sector;
} ;
#endif /* _LINUX_VIRTIO_BLK_H */
