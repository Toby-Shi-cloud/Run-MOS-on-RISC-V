#include <asm/asm.h>
#include <kclock.h>

FEXPORT(kclock_init)
	rdtime	t0
	li	t1, TIMECLOCK
	add	a0, t0, t1
        mv	a6, zero
        mv	a7, zero
        ecall
	jr      ra
