#ifndef _DISK_H_
#define _DISK_H_

#include <types.h>
#include <drivers/dev_disk.h>

#define DISK_STATUS_NO_RESP 0xff
#define DISK_BUFFUER_LEN 512

struct virtio_blk_req {
        u_int sector; // the sector of the disk
        u_char write; // 1 if write, 0 if read
        volatile u_char status; // the status of this req
        char buf[DISK_BUFFUER_LEN]; // 512 bytes
};

void virtio_disk_init(void);
void virtio_read_config(void);
int virtio_disk_req(struct virtio_blk_req *req);
void virtio_disk_intr(void);

#endif /* _DISK_H_ */