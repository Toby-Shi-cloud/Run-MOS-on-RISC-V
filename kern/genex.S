#include <asm/asm.h>
#include <kclock.h>
#include <stackframe.h>

.macro BUILD_HANDLER exception handler
FEXPORT(handle_\exception)
	mv	a0, sp
	jal	\handler
	j	ret_from_exception
.endm

.text

FEXPORT(ret_from_exception)
	//todo do sth latter

FEXPORT(handle_timer)
	mv	a0, sp
	csrr	t0, scause
	li	t2, 0x80000000
	and	t1, t0, t2
	bne	t1, zero, timer_irq // interrupt
	// Load access fault
	jal	do_reserved
	j	ret_from_exception
timer_irq:
	// git current time
	rdtime	a0
	li	t1, TIMECLOCK
	add	a0, a0, t1
	call	clock_set_next_event
	li	a0, 0
	j	schedule

#if !defined(LAB) || LAB >= 4
BUILD_HANDLER mod do_tlb_mod
BUILD_HANDLER sys do_syscall
#endif

BUILD_HANDLER reserved do_reserved
