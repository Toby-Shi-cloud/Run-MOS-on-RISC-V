#include <mmu.h>
#include <pmap.h>
#include <disk.h>
#include <printk.h>

// reference: https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/virtio_disk.c
// also http://docs.oasis-open.org/virtio/virtio/v1.0/cs04/virtio-v1.0-cs04.pdf (p23)

#define R(r) ((volatile u_int *)(DEV_DISK_ADDRESS + (r)))

static struct vring disk;


void virtio_disk_init() {
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

	// set queue size.
	*R(VIRTIO_MMIO_QUEUE_NUM) = VIRTIO_QUEUE_NUM;

	// write physical addresses.
	// in riscv-32 the high 32 bit should always be zero.
	// because `virtio_disk_init` is called before vm setup, so the addr is pa
	*R(VIRTIO_MMIO_QUEUE_DESC_LOW) = (u_long)disk.desc;
	*R(VIRTIO_MMIO_QUEUE_DESC_HIGH) = 0;
	*R(VIRTIO_MMIO_QUEUE_AVAIL_LOW) = (u_long)disk.avail;
	*R(VIRTIO_MMIO_QUEUE_AVAIL_HIGH) = 0;
	*R(VIRTIO_MMIO_QUEUE_USED_LOW) = (u_long)disk.used;
	*R(VIRTIO_MMIO_QUEUE_USED_HIGH) = 0;

	// queue is ready.
	*R(VIRTIO_MMIO_QUEUE_READY) = 0x1;

	// all NUM descriptors start out unused.
	disk.desc_free_bitmap = 0xFFFFU;

	// Step 8. Set the DRIVER_OK status bit. At this point the device is “live”.
	status |= VIRTIO_CONFIG_S_DRIVER_OK;
	*R(VIRTIO_MMIO_STATUS) = status;

	printk("disk_driver.c: virtio_disk_init success\n");
}
