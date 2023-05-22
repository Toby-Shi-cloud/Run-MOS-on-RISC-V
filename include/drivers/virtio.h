#ifndef _DRIVERS_VIRTIO_H_
#define _DRIVERS_VIRTIO_H_

#include <drivers/virtio_blk.h>
#include <drivers/virtio_config.h>
#include <drivers/virtio_mmio.h>
#include <drivers/virtio_ring.h>
#define VIRTIO(_i) 0x1000##_i##000      // address of virtio device _i (1, ..., 8)
#define VIRTIOTOP  VIRTIO(9)            // the top of virtio mmio
#define VIRTIOBASE VIRTIO(1)            // the bottom of virtio mmio
#define VIRTIOSIZE (VIRTIOTOP - VIRTIOBASE)

#endif /* _DRIVERS_VIRTIO_H_ */