#ifndef _TRAP_H_
#define _TRAP_H_

#ifndef __ASSEMBLER__ /* !__ASSEMBLER__ */

#include <types.h>

struct Trapframe {
	/* Saved main processor registers. */
	unsigned long regs[32];

	/* Saved special registers. */
	unsigned long sstatus;
	unsigned long scause;
	unsigned long sscratch;
	unsigned long sepc;
};

void print_tf(struct Trapframe *tf);

#else /* __ASSEMBLER__ */

.macro push reg
	addi	sp, sp, -8
	sw	\reg, 0(sp)
.endm

.macro pop reg
        lw	\reg, 0(sp)
        addi	sp, sp, 8
.endm

#endif /* __ASSEMBLER__ */

#define TRAPFRAME_SIZE 144

#endif /* _TRAP_H_ */
