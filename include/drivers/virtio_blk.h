/**
 * @see /usr/include/linux/virtio_blk.h
*/

#ifndef _DRIVERS_VIRTIO_BLK_H_
#define _DRIVERS_VIRTIO_BLK_H_

/* Feature bits */
#define VIRTIO_BLK_F_SIZE_MAX		1	/* Indicates maximum segment size */
#define VIRTIO_BLK_F_SEG_MAX		2	/* Indicates maximum # of segments */
#define VIRTIO_BLK_F_GEOMETRY		4	/* Legacy geometry available  */
#define VIRTIO_BLK_F_RO			5	/* Disk is read-only */
#define VIRTIO_BLK_F_BLK_SIZE		6	/* Block size of disk is available*/
#define VIRTIO_BLK_F_TOPOLOGY		10	/* Topology information is available */
#define VIRTIO_BLK_F_MQ			12	/* support more than one vq */
#define VIRTIO_BLK_F_DISCARD		13	/* DISCARD is supported */
#define VIRTIO_BLK_F_WRITE_ZEROES	14	/* WRITE ZEROES is supported */

/* Legacy feature bits */
#ifndef VIRTIO_BLK_NO_LEGACY
#define VIRTIO_BLK_F_BARRIER	0	/* Does host support barriers? */
#define VIRTIO_BLK_F_SCSI	7	/* Supports scsi command passthru */
#define VIRTIO_BLK_F_FLUSH	9	/* Flush command supported */
#define VIRTIO_BLK_F_CONFIG_WCE	11	/* Writeback mode available in config */
/* Old (deprecated) name for VIRTIO_BLK_F_FLUSH. */
#define VIRTIO_BLK_F_WCE VIRTIO_BLK_F_FLUSH
#endif /* !VIRTIO_BLK_NO_LEGACY */

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
#define VIRTIO_BLK_T_GET_ID	8

/* Discard command */
#define VIRTIO_BLK_T_DISCARD	11

/* Write zeroes command */
#define VIRTIO_BLK_T_WRITE_ZEROES	13

#ifndef VIRTIO_BLK_NO_LEGACY
/* Barrier before this op. */
#define VIRTIO_BLK_T_BARRIER	0x80000000
#endif /* !VIRTIO_BLK_NO_LEGACY */

/*
 * This comes first in the read scatter-gather list.
 * For legacy virtio, if VIRTIO_F_ANY_LAYOUT is not negotiated,
 * this is the first element of the read scatter-gather list.
 */
struct virtio_blk_outhdr {
	/* VIRTIO_BLK_T* */
	uint32_t type;
	/* io priority. */
	uint32_t ioprio;
	/* Sector (ie. 512 byte offset) */
	uint64_t sector;
};

#endif /* _DRIVERS_VIRTIO_BLK_H_ */