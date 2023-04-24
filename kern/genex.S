#include <asm/asm.h>
#include <kclock.h>
#include <stackframe.h>

.macro BUILD_HANDLER exception handler
FEXPORT(handle_\exception)
	mv	a0, sp
	jal	\handler
	j	ret_from_exception
.endm

.section .text
FEXPORT(ret_from_exception)
	RESTORE_ALL
	sret

FEXPORT(handle_timer)
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
