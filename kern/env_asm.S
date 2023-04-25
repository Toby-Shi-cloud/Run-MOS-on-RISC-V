#include <asm/asm.h>
#include <mmu.h>
#include <trap.h>

.text
FEXPORT(env_pop_tf) // void env_pop_tf(struct Trapframe *tf, u_int satp)
	csrw	satp, a1
	sfence.vma zero, zero
	fence.i
	mv      sp, a0
	j       ret_from_exception

FEXPORT(enable_irq)
	// interrupt setup
	la	t0, exception_entry
	csrw	stvec, t0
	li	t0, 32
	csrs	sie, t0
	csrsi	sstatus, 2
	jr	ra
