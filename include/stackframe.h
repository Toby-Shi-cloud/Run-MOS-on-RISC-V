#include <asm/asm.h>
#include <mmu.h>
#include <trap.h>

// clang-format off
.macro SAVE_ALL
	csrw	sscratch, sp
	bltz	sp, $1f	// sp < 0 if and only if sp is in kernel address
	li	sp, KSTACKTOP
$1f:	addi	sp, sp, -TRAPFRAME_SIZE
	sw	x0, 0(sp)
	sw	x1, 4(sp)
	sw	x2, 8(sp)
	sw	x3, 12(sp)
	sw	x4, 16(sp)
	sw	x5, 20(sp)
	sw	x6, 24(sp)
	sw	x7, 28(sp)
	sw	x8, 32(sp)
	sw	x9, 36(sp)
	sw	x10, 40(sp)
	sw	x11, 44(sp)
	sw	x12, 48(sp)
	sw	x13, 52(sp)
	sw	x14, 56(sp)
	sw	x15, 60(sp)
	sw	x16, 64(sp)
	sw	x17, 68(sp)
	sw	x18, 72(sp)
	sw	x19, 76(sp)
	sw	x20, 80(sp)
	sw	x21, 84(sp)
	sw	x22, 88(sp)
	sw	x23, 92(sp)
	sw	x24, 96(sp)
	sw	x25, 100(sp)
	sw	x26, 104(sp)
	sw	x27, 108(sp)
	sw	x28, 112(sp)
	sw	x29, 116(sp)
	sw	x30, 120(sp)
	sw	x31, 124(sp)
	csrr	t0, sstatus
	sw	t0, 128(sp)
	csrr	t0, sscratch
	sw	t0, 132(sp)
	csrr	t0, scause
	sw	t0, 136(sp)
	csrr	t0, sepc
	sw	t0, 140(sp)
	csrr	t0, stval
	sw	t0, 144(sp)
	csrr	t0, sip
	sw	t0, 148(sp)
.endm
/*
 * Note that we restore the IE flags from stack. This means
 * that a modified IE mask will be nullified.
 */
.macro RESTORE_ALL
	lw	t0, 128(sp)
	csrw	sstatus, t0
	lw	t0, 132(sp)
	csrw	sscratch, t0
	lw	t0, 136(sp)
	csrw	scause, t0
	lw	t0, 140(sp)
	csrw	sepc, t0
	lw	t0, 144(sp)
	csrw	stval, t0
	// do not write sip back
	lw	x1, 4(sp)
	lw	x3, 12(sp)
	lw	x4, 16(sp)
	lw	x5, 20(sp)
	lw	x6, 24(sp)
	lw	x7, 28(sp)
	lw	x8, 32(sp)
	lw	x9, 36(sp)
	lw	x10, 40(sp)
	lw	x11, 44(sp)
	lw	x12, 48(sp)
	lw	x13, 52(sp)
	lw	x14, 56(sp)
	lw	x15, 60(sp)
	lw	x16, 64(sp)
	lw	x17, 68(sp)
	lw	x18, 72(sp)
	lw	x19, 76(sp)
	lw	x20, 80(sp)
	lw	x21, 84(sp)
	lw	x22, 88(sp)
	lw	x23, 92(sp)
	lw	x24, 96(sp)
	lw	x25, 100(sp)
	lw	x26, 104(sp)
	lw	x27, 108(sp)
	lw	x28, 112(sp)
	lw	x29, 116(sp)
	lw	x30, 120(sp)
	lw	x31, 124(sp)
	lw	x2, 8(sp)
	addi	sp, sp, TRAPFRAME_SIZE
	csrrw	sp, sscratch, sp
.endm
