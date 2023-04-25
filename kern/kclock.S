#include <asm/asm.h>
#include <kclock.h>

FEXPORT(kclock_init)
	rdtime	t0
	li	t1, TIMECLOCK
	add	a0, t0, t1
	mv	a1, zero
	mv	a2, zero
	mv	a3, zero
	mv	a4, zero
	mv	a5, zero
	mv	a6, zero
	mv	a7, zero
	ecall
	jr	ra
