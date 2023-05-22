/**
 * @see http://docs.oasis-open.org/virtio/virtio/v1.0/cs04/virtio-v1.0-cs04.pdf (p39; table 4.1)
*/
#ifndef _DRIVERS_VIRTIO_MMIO_H_
#define _DRIVERS_VIRTIO_MMIO_H_

/* mmio registers */
#define VIRTIO_MMIO_MAGIC_VALUE         0x000 // (R) "virt"; 0x74726976
#define VIRTIO_MMIO_VERSION             0x004 // (R) Version; should be 2
#define VIRTIO_MMIO_DEVICE_ID           0x008 // (R) Device type; 1 is net, 2 is disk; should be 2
#define VIRTIO_MMIO_VENDOR_ID           0x00c // (R) 0x554d4551
#define VIRTIO_MMIO_DEVICE_FEATURES     0x010 // (R) Flags representing features the device supports.
#define VIRTIO_MMIO_DEVICE_FEATURES_SEL 0x014 // (W) Device (Host) features word selection.
#define VIRTIO_MMIO_DRIVER_FEATURES     0x020 // (W) Flags representing device features understood and activated by the driver.
#define VIRTIO_MMIO_DRIVER_FEATURES_SEL 0x024 // (W) Activated (Guest) features word selection.
#define VIRTIO_MMIO_QUEUE_SEL           0x030 // (W) Virtual queue index (first queue is 0).
#define VIRTIO_MMIO_QUEUE_NUM_MAX       0x034 // (R) Maximum virtual queue size.
#define VIRTIO_MMIO_QUEUE_NUM           0x038 // (W) Virtual queue size.
#define VIRTIO_MMIO_QUEUE_READY         0x044 // (RW) Virtual queue ready bit.
#define VIRTIO_MMIO_QUEUE_NOTIFY        0x050 // (W) Queue notifier.
#define VIRTIO_MMIO_INTERRUPT_STATUS    0x060 // (R) Interrupt status.
#define VIRTIO_MMIO_INTERRUPT_ACK       0x064 // (W) Interrupt acknowledge.
#define VIRTIO_MMIO_STATUS              0x070 // (RW) Device status.
#define VIRTIO_MMIO_QUEUE_DESC_LOW      0x080 // (W) Virtual queue’s Descriptor Table physical address (lower 32 bit).
#define VIRTIO_MMIO_QUEUE_DESC_HIGH     0x084 // (W) Virtual queue’s Descriptor Table physical address (higher 32 bit).
#define VIRTIO_MMIO_QUEUE_AVAIL_LOW     0x090 // (W) Virtual queue’s Available Ring physical address (lower 32 bit).
#define VIRTIO_MMIO_QUEUE_AVAIL_HIGH    0x094 // (W) Virtual queue’s Available Ring physical address (higher 32 bit).
#define VIRTIO_MMIO_QUEUE_USED_LOW      0x0a0 // (W) Virtual queue’s Used Ring physical address (lower 32 bit).
#define VIRTIO_MMIO_QUEUE_USED_HIGH     0x0a4 // (W) Virtual queue’s Used Ring physical address (higher 32 bit).
#define VIRTIO_MMIO_CONFIG_GENERERATION 0x0fc // (R) Configuration atomicity value.

#endif /* _DRIVERS_VIRTIO_MMIO_H_ */