/**
 *  @see /usr/include/linux/virtio_config.h
*/

#ifndef _DRIVERS_VIRTIO_CONFIG_H_
#define _DRIVERS_VIRTIO_CONFIG_H_

/* status bits */
#define VIRTIO_CONFIG_S_ACKNOWLEDGE         0b00000001
#define VIRTIO_CONFIG_S_DRIVER              0b00000010
#define VIRTIO_CONFIG_S_DRIVER_OK           0b00000100
#define VIRTIO_CONFIG_S_FEATURES_OK         0b00001000
#define VIRTIO_CONFIG_S_DEVICE_NEEDS_RESET  0b01000000
#define VIRTIO_CONFIG_S_FAILED              0b10000000

/* Feature bits */

/*
 * Virtio feature bits VIRTIO_TRANSPORT_F_START through
 * VIRTIO_TRANSPORT_F_END are reserved for the transport
 * being used (e.g. virtio_ring, virtio_pci etc.), the
 * rest are per-device feature bits.
 */
#define VIRTIO_TRANSPORT_F_START	28
#define VIRTIO_TRANSPORT_F_END		38

#ifndef VIRTIO_CONFIG_NO_LEGACY
/* Do we get callbacks when the ring is completely used, even if we've
 * suppressed them? */
#define VIRTIO_F_NOTIFY_ON_EMPTY	24

/* Can the device handle any descriptor layout? */
#define VIRTIO_F_ANY_LAYOUT		27
#endif /* VIRTIO_CONFIG_NO_LEGACY */

/* v1.0 compliant. */
#define VIRTIO_F_VERSION_1		32

/*
 * If clear - device has the IOMMU bypass quirk feature.
 * If set - use platform tools to detect the IOMMU.
 *
 * Note the reverse polarity (compared to most other features),
 * this is for compatibility with legacy systems.
 */
#define VIRTIO_F_IOMMU_PLATFORM		33

/* This feature indicates support for the packed virtqueue layout. */
#define VIRTIO_F_RING_PACKED		34

/*
 * This feature indicates that memory accesses by the driver and the
 * device are ordered in a way described by the platform.
 */
#define VIRTIO_F_ORDER_PLATFORM		36

/*
 * Does the device support Single Root I/O Virtualization?
 */
#define VIRTIO_F_SR_IOV			37

#endif /* _DRIVERS_VIRTIO_CONFIG_H_ */