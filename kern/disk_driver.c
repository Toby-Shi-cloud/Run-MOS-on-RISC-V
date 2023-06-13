#include <mmu.h>
#include <pmap.h>
#include <disk.h>
#include <printk.h>

// reference: https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/virtio_disk.c
// also http://docs.oasis-open.org/virtio/virtio/v1.0/cs04/virtio-v1.0-cs04.pdf (p23)

#define GPA(pa) ((uint32_t)(pa))

// DO NOT add KSEG1 when init, because vm haven't been init yet.
#define R(r) ((volatile u_int *)(DEV_DISK_ADDRESS + (r)))

static struct vring disk;

void virtio_disk_init(void) {
	u_int status = 0;

	// Step 0. Check the virtio disk.
	if (*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 || *R(VIRTIO_MMIO_VERSION) != 2) {
		panic("Virtio disk invalid");
	}
	if (*R(VIRTIO_MMIO_DEVICE_ID) != 2 || *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551) {
		panic("Virtio disk not found");
	}

	// Step 1. Reset the device.
	*R(VIRTIO_MMIO_STATUS) = status;

	// Step 2. Set the ACKNOWLEDGE status bit: the guest OS has notice the device.
	status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
	*R(VIRTIO_MMIO_STATUS) = status;

	// Step 3. Set the DRIVER status bit: the guest OS knows how to drive the device.
	status |= VIRTIO_CONFIG_S_DRIVER;
	*R(VIRTIO_MMIO_STATUS) = status;

	// Step 4. Read device feature bits, and write the subset of feature bits understood
	// by the OS and driver to the device. During this step the driver MAY read (but MUST
	// NOT write) the device-specific configuration fields to check that it can support
	// the device before accepting it.
	u_int features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
	printk("@features: %032b\n", features);
	// printk("%032b\n", features);
	// 0011 0000 0000 0000 0110 1110 0101 0100;
	// 2, 4, 6, 9, 10, 11, 13, 14, 28, 29
	features &= ~(1 << VIRTIO_BLK_F_RO);
	features &= ~(1 << VIRTIO_BLK_F_SCSI);
	features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
	features &= ~(1 << VIRTIO_BLK_F_MQ);
	features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
	features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
	features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
	*R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

	// Step 5. Set the FEATURES_OK status bit. The driver MUST NOT accept new feature
	// bits after this step.
	status |= VIRTIO_CONFIG_S_FEATURES_OK;
	*R(VIRTIO_MMIO_STATUS) = status;

	// Step 6. Re-read device status to ensure the FEATURES_OK bit is still set: otherwise,
	// the device does not support our subset of features and the device is unusable.
	status = *R(VIRTIO_MMIO_STATUS);
	if (!(status & VIRTIO_CONFIG_S_FEATURES_OK)) {
		panic("Virtio disk FEATURES_OK unset");
	}

	// Step 7. Perform device-specific setup, including discovery of virtqueues for the device,
	// optional per-bus setup, reading and possibly writing the device’s virtio configuration
	// space, and population of virtqueues.

	// initialize queue 0.
	*R(VIRTIO_MMIO_QUEUE_SEL) = 0;

	// ensure queue 0 is not in use.
	if(*R(VIRTIO_MMIO_QUEUE_READY)) {
		panic("Virtio disk should not be ready");
	}

	// check maximum queue size.
	u_int max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
	if(max == 0) {
		panic("Virtio disk does not have queue NO.0.");
	} else if(max < VIRTIO_QUEUE_NUM) {
		panic("Virtio disk queue too short");
	}

	// allocate and zero queue memory.
	disk.desc = alloc(VIRTIO_QUEUE_NUM * sizeof(struct vring_desc), VRING_DESC_ALIGN_SIZE, 1);
	disk.avail = alloc(sizeof(struct vring_avail), VRING_AVAIL_ALIGN_SIZE, 1);
	disk.used = alloc(sizeof(struct vring_used), VRING_USED_ALIGN_SIZE, 1);
	disk.outhdr = alloc(VIRTIO_QUEUE_NUM * sizeof(struct virtio_blk_outhdr),
			    sizeof(struct virtio_blk_outhdr), 1);
	disk.avail->flags = 1;

	// set queue size.
	*R(VIRTIO_MMIO_QUEUE_NUM) = VIRTIO_QUEUE_NUM;

	// write physical addresses.
	// in riscv-32 the high 32 bit should always be zero.
	// because `virtio_disk_init` is called before vm setup, so the addr is pa
	*R(VIRTIO_MMIO_QUEUE_DESC_LOW) = GPA(disk.desc);
	*R(VIRTIO_MMIO_QUEUE_DESC_HIGH) = 0;
	*R(VIRTIO_MMIO_QUEUE_AVAIL_LOW) = GPA(disk.avail);
	*R(VIRTIO_MMIO_QUEUE_AVAIL_HIGH) = 0;
	*R(VIRTIO_MMIO_QUEUE_USED_LOW) = GPA(disk.used);
	*R(VIRTIO_MMIO_QUEUE_USED_HIGH) = 0;

	// queue is ready.
	*R(VIRTIO_MMIO_QUEUE_READY) = 0x1;

	// all NUM descriptors start out unused.
	disk.desc_free_bitmap = (1llu << VIRTIO_QUEUE_NUM) - 1;

	// Step 8. Set the DRIVER_OK status bit. At this point the device is “live”.
	status |= VIRTIO_CONFIG_S_DRIVER_OK;
	*R(VIRTIO_MMIO_STATUS) = status;

	printk("disk_driver.c: virtio_disk_init success\n");

	// virtio_read_config();
}

void virtio_read_config(void) {
	struct virtio_blk_config config;
	config = *(volatile struct virtio_blk_config *)R(VIRTIO_MMIO_CONFIG_REGION);
	printk("@capacity: %ld\n", (long)config.capacity);
	printk("@seg_max: %d\n", (int)config.seg_max);
	printk("@cylinders: %d\n", (int)config.geometry.cylinders);
	printk("@heads: %d\n", (int)config.geometry.heads);
	printk("@sectors: %d\n", (int)config.geometry.sectors);
	printk("@blk_size: %d\n", (int)config.blk_size);
	printk("@min_io_size: %d\n", (int)config.min_io_size);
}

#undef R
#define R(r) ((volatile u_int *)(KSEG1 + DEV_DISK_ADDRESS + (r)))

static int alloc_desc() {
	if (disk.desc_free_bitmap == 0) {
		return -E_NO_RING_DESC;
	}
	for (int i = 0; i < VIRTIO_QUEUE_NUM; i++) {
		if (disk.desc_free_bitmap & (1 << i)) {
			disk.desc_free_bitmap &= ~(1 << i);
			return i;
		}
	}
	panic("unreachable code");
	__builtin_unreachable();
}

static void free_desc(int idx) {
	assert(idx >= 0 && idx < VIRTIO_QUEUE_NUM);
	assert(!(disk.desc_free_bitmap & (1 << idx)));
	disk.desc_free_bitmap |= 1 << idx;
}

// alloc 3 ring descriptors at once
static int alloc_3_desc(int idx[]) {
	for (int i = 0; i < 3; i++) {
		if ((idx[i] = alloc_desc()) < 0) {
			for (int j = 0; j < i; j++) {
				free_desc(idx[j]);
			}
			return idx[i];
		}
	}
	return 0;
}

static void free_chain(int idx) {
	uint16_t last_flag, last_next;
	do {
		last_flag = disk.desc[idx].flags;
		last_next = disk.desc[idx].next;
		free_desc(idx);
		idx = last_next;
	} while (last_flag & VRING_DESC_F_NEXT);
}

// Should not do busy waiting or yield in kernel.
// Because when all runnable env are blocked in kernel, the interrupt
// will never happened, making the kernel a dead loop.
// Therefore, virtio_disk_req will return immediately no matter whether
// the req is sent. And the user should always check req->status before
// assuming the req is finished.
// Note: the req should be in a physical page and pass by pa.
int virtio_disk_req(struct virtio_blk_req *req) {
	if ((u_long)req < KSTACKTOP) {
		return -E_INVAL;
	}

	int idx[3];
	try(alloc_3_desc(idx));
	// printk("disk.desc_free_bitmap: %08x\n", disk.desc_free_bitmap);

	struct virtio_blk_outhdr *outhdr = &disk.outhdr[idx[0]];
	if (req->write) {
		outhdr->type = VIRTIO_BLK_T_OUT;
	} else {
		outhdr->type = VIRTIO_BLK_T_IN;
	}
	outhdr->ioprio = 0; // not used yet.
	outhdr->sector = req->sector;

	// The first descriptor is used to store a virtio_blk_outhdr.
	disk.desc[idx[0]].addr = GPA(outhdr);
	disk.desc[idx[0]].len = sizeof(struct virtio_blk_outhdr);
	disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
	disk.desc[idx[0]].next = idx[1];

	// The second descriptor is used to store the read/write buffer.
	disk.desc[idx[1]].addr = GPA(req->buf);
	disk.desc[idx[1]].len = DISK_BUFFUER_LEN;
	if (req->write) {
		disk.desc[idx[1]].flags = 0;
	} else {
		// if req->read, the device would write.
		disk.desc[idx[1]].flags = VRING_DESC_F_WRITE;
	}
	disk.desc[idx[1]].flags |= VRING_DESC_F_NEXT;
	disk.desc[idx[1]].next = idx[2];

	// set the status to indicate no response yet
	req->status = DISK_STATUS_NO_RESP;
	// The third descriptor is used to store the status.
	disk.desc[idx[2]].addr = GPA(&req->status);
	disk.desc[idx[2]].len = 1;
	disk.desc[idx[2]].flags = VRING_DESC_F_WRITE;
	disk.desc[idx[2]].next = 0;

	// set this ring to avail ring to tell the device
	disk.avail->ring[disk.avail->idx % VIRTIO_QUEUE_NUM] = idx[0];
	__sync_synchronize();
	disk.avail->idx++;
	__sync_synchronize();

	// notify the device
	if (disk.used->flags == 0) {
		*R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0;
	} else if (disk.used->flags != 1) {
		panic("unexpected value of flags: %d", disk.used->flags);
	}
	return 0;
}

void virtio_disk_intr(void) {
	if (*R(VIRTIO_MMIO_INTERRUPT_STATUS)) {
		printk("interrupt status: %d\n", *R(VIRTIO_MMIO_INTERRUPT_STATUS));
		*R(VIRTIO_MMIO_INTERRUPT_ACK) = *R(VIRTIO_MMIO_INTERRUPT_STATUS) & 0x3;
		__sync_synchronize();
	}
	while (disk.used_idx != disk.used->idx) {
		__sync_synchronize();
		int id = disk.used->ring[disk.used_idx % VIRTIO_QUEUE_NUM].id;
		free_chain(id);
		disk.used_idx++;
		// printk("disk.used_idx = %d\n", (int)disk.used_idx);
	}
}
