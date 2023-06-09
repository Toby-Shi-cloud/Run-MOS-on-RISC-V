#include <asm/asm.h>

// int spawnl(char *prog, char *args, ...)
EXPORT(spawnl)
	// Assuming all the arguments are char *
	addi    sp, sp, -32
	sw      ra, 0(sp)
	sw      a7, 28(sp)
	sw      a6, 24(sp)
	sw      a5, 20(sp)
	sw      a4, 16(sp)
	sw      a3, 12(sp)
	sw      a2, 8(sp)
	sw      a1, 4(sp)
	addi    a1, sp, 4
	call    spawn
	lw      ra, 0(sp)
	addi    sp, sp, 32
	ret     // directly ret spawn(...)
