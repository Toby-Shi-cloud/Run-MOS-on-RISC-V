#include <asm/asm.h>
#include <mmu.h>

.section .text
EXPORT(_start)
	/* disable interrupts */
	csrwi	sstatus, 0

	/* hint: you can reference the memory layout in include/mmu.h */
	/* set up the kernel stack */
	/* Exercise 1.3: Your code here. (1/2) */
	li	sp, 0x803ffffc

	/* jump to mips_init */
	/* Exercise 1.3: Your code here. (2/2) */
	j	mips_init
