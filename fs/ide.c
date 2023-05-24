/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <lib.h>
#include <mmu.h>
#include <disk.h>

// Virtual address at which to store disk request to the driver.
#define REQVA 0x0eeff000

// There is no need to call mem_alloc before writing.
static struct virtio_blk_req *req = (void *)REQVA;

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		req->sector = secno + off / BY2SECT;
		req->write = 0;
		panic_on(syscall_dev_req(diskno, req));
		while (req->status == 0xff) {
			syscall_yield();
		}
		debugf("ide_read: req->status: %d\n", req->status);
		memcpy(dst + off, req->buf, BY2SECT);
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		req->sector = secno + off / BY2SECT;
		req->write = 1;
		memcpy(req->buf, src + off, BY2SECT);
		panic_on(syscall_dev_req(diskno, req));
		while (req->status == 0xff) {
			syscall_yield();
		}
		debugf("ide_write: req->status: %d\n", req->status);
	}
}
