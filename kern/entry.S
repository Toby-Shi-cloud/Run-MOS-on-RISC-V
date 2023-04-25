#include <asm/asm.h>
#include <stackframe.h>

.section .text.entry
EXPORT(exception_entry)
	SAVE_ALL
	csrr	t0, scause
	li	t1, 0x80000000
	and	t2, t1, t0
	bnez	t2, interrupt
exception:
	la	t2, exception_handlers
	slli	t1, t0, 2
	add	t1, t1, t2
	lw	t1, 0(t1)
	jr	t1
interrupt:
	li	t1, 0x80000005 // timer
	beq	t0, t1, handle_timer
	j	handle_reserved 
