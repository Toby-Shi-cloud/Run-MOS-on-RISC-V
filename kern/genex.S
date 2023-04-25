#include <asm/asm.h>
#include <kclock.h>
#include <stackframe.h>

.macro BUILD_HANDLER exception handler
FEXPORT(handle_\exception)
	mv	a0, sp
	call	\handler
	j	ret_from_exception
.endm

.section .text
FEXPORT(ret_from_exception)
	RESTORE_ALL
	sret
	nop

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
BUILD_HANDLER instr_miss do_tlb_miss
BUILD_HANDLER load_miss do_tlb_miss
BUILD_HANDLER store_miss do_tlb_miss
