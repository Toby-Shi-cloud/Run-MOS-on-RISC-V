/**
 * @see /usr/include/linux/virtio_ring.h
*/

#ifndef _DRIVERS_VIRTIO_RING_H_
#define _DRIVERS_VIRTIO_RING_H_

/* This marks a buffer as continuing via the next field. */
#define VRING_DESC_F_NEXT	1
/* This marks a buffer as write-only (otherwise read-only). */
#define VRING_DESC_F_WRITE	2
/* This means the buffer contains a list of buffer descriptors. */
#define VRING_DESC_F_INDIRECT	4

/*
 * Mark a descriptor as available or used in packed ring.
 * Notice: they are defined as shifts instead of shifted values.
 */
#define VRING_PACKED_DESC_F_AVAIL	7
#define VRING_PACKED_DESC_F_USED	15

/* The Host uses this in used->flags to advise the Guest: don't kick me when
 * you add a buffer.  It's unreliable, so it's simply an optimization.  Guest
 * will still kick if it's out of buffers. */
#define VRING_USED_F_NO_NOTIFY	1
/* The Guest uses this in avail->flags to advise the Host: don't interrupt me
 * when you consume a buffer.  It's unreliable, so it's simply an
 * optimization.  */
#define VRING_AVAIL_F_NO_INTERRUPT	1

/* Enable events in packed ring. */
#define VRING_PACKED_EVENT_FLAG_ENABLE	0x0
/* Disable events in packed ring. */
#define VRING_PACKED_EVENT_FLAG_DISABLE	0x1
/*
 * Enable events for a specific descriptor in packed ring.
 * (as specified by Descriptor Ring Change Event Offset/Wrap Counter).
 * Only valid if VIRTIO_RING_F_EVENT_IDX has been negotiated.
 */
#define VRING_PACKED_EVENT_FLAG_DESC	0x2

/*
 * Wrap counter bit shift in event suppression structure
 * of packed ring.
 */
#define VRING_PACKED_EVENT_F_WRAP_CTR	15

/* We support indirect buffer descriptors */
#define VIRTIO_RING_F_INDIRECT_DESC	28

/* The Guest publishes the used index for which it expects an interrupt
 * at the end of the avail ring. Host should ignore the avail->flags field. */
/* The Host publishes the avail index for which it expects a kick
 * at the end of the used ring. Guest should ignore the used->flags field. */
#define VIRTIO_RING_F_EVENT_IDX		29

// This many virtio descriptors.
// Must be a power of two.
#define VIRTIO_QUEUE_NUM 16

/* Virtio ring descriptors: 16 bytes.  These can chain together via "next". */
struct vring_desc {
	/* Address (guest-physical). */
	u_long addr;
	/* Length. */
	uint32_t len;
	/* The flags as indicated above. */
	uint16_t flags;
	/* We chain unused descriptors via this, too */
	uint16_t next;
        /* No used (for align reason). */
	u_char _pad[4];
};

struct vring_avail {
	uint16_t flags;
	uint16_t idx;
	uint16_t ring[VIRTIO_QUEUE_NUM];
};

/* u32 is used here for ids for padding reasons. */
struct vring_used_elem {
	/* Index of start of used descriptor chain. */
	uint32_t id;
	/* Total length of the descriptor chain which was used (written to) */
	uint32_t len;
};

struct vring_used {
	uint16_t flags;
	uint16_t idx;
	struct vring_used_elem ring[VIRTIO_QUEUE_NUM];
};

struct vring {
	/* A set (not a ring) of DMA descriptors, with which the driver 
	 * tells the device where to read and write individual disk
	 * operations. There are NUM descriptors. Most commands consist
	 * of a "chain" (a linked list) of a couple of these descriptors.
	 */
	struct vring_desc *desc;

	/* A ring in which the driver writes descriptor numbers that the
	 * driver would like the device to process. It only includes the
	 * head descriptor of each chain. The ring has NUM elements.
	 */
	struct vring_avail *avail;

	/* A ring in which the device writes descriptor numbers that the
	 * device has finished processing (just the head of each chain).
	 * There are NUM used ring entries.
	 */
	struct vring_used *used;

	/* A bitmap to indicate if a specific DMA descriptor is free.
	 * (desc[i]) is free iff (desc_free_bitmap & (1 << i)).
	 */
	uint16_t desc_free_bitmap;
};

/* Alignment requirements for vring elements.
 * When using pre-virtio 1.0 layout, these fall out naturally.
 */
#define VRING_AVAIL_ALIGN_SIZE 2
#define VRING_USED_ALIGN_SIZE 4
#define VRING_DESC_ALIGN_SIZE 16

_Static_assert(sizeof(struct vring_desc) == 16);
_Static_assert(sizeof(struct vring_avail) == 4 + 2 * VIRTIO_QUEUE_NUM);
_Static_assert(sizeof(struct vring_used_elem) == 8);
_Static_assert(sizeof(struct vring_used) == 4 + 8 * VIRTIO_QUEUE_NUM);

/* The standard layout for the ring is a continuous chunk of memory which looks
 * like this.  We assume num is a power of 2.
 *
 * struct vring
 * {
 *	// The actual descriptors (16 bytes each)
 *	struct vring_desc desc[num];
 *
 *	// A ring of available descriptor heads with free-running index.
 *	__virtio16 avail_flags;
 *	__virtio16 avail_idx;
 *	__virtio16 available[num];
 *	__virtio16 used_event_idx;
 *
 *	// Padding to the next align boundary.
 *	char pad[];
 *
 *	// A ring of used descriptor heads with free-running index.
 *	__virtio16 used_flags;
 *	__virtio16 used_idx;
 *	struct vring_used_elem used[num];
 *	__virtio16 avail_event_idx;
 * };
 */

#endif /* _DRIVERS_VIRTIO_RING_H_ */