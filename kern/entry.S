#include <asm/asm.h>

/* Exception Code for Supervisor (SXLEN == 32):
 o ------------+----------------+------------------
 o   Interrupt | Exception Code | Description
 o ------------+----------------+------------------
 o       1     |        1       | Supervisor software interrupt
 o       1     |        5       | Supervisor timer interrupt
 o       1     |        9       | Supervisor external interrupt
 o       1     |      ≥16       | Designated for platform use
 o ------------+----------------+------------------
 o       0     |        0       | Instruction address misaligned
 o       0     |        1       | Instruction access fault
 o       0     |        2       | Illegal instruction
 o       0     |        3       | Breakpoint
 o       0     |        4       | Load address misaligned
 o       0     |        5       | Load access fault
 o       0     |        6       | Store/AMO address misaligned
 o       0     |        7       | Store/AMO access fault
 o       0     |        8       | Environment call from U-mode
 o       0     |        9       | Environment call from S-mode
 o       0     |       12       | Instruction page fault
 o       0     |       13       | Load page fault
 o       0     |       15       | Store/AMO page fault
 o       0     |      ≥24       | Designated for custom use
 o ------------+----------------+------------------
 */

.section .text.entry
.align 4
EXPORT(exception_handlers)
	j	handle_reserved	// 0
	j	handle_reserved	// 1
	j	handle_reserved	// 2
	j	handle_reserved	// 3
	j	handle_reserved	// 4
	j	handle_timer	// 5
	j	handle_reserved	// 6
	j	handle_reserved	// 7
	j	handle_reserved	// 8
	j	handle_reserved	// 9
	j	handle_reserved	// 10
	j	handle_reserved	// 11
	j	handle_reserved	// 12
	j	handle_reserved	// 13
	j	handle_reserved	// 14
	j	handle_reserved	// 15
	j	handle_reserved	// 16
	j	handle_reserved	// 17
	j	handle_reserved	// 18
	j	handle_reserved	// 19
	j	handle_reserved	// 20
	j	handle_reserved	// 21
	j	handle_reserved	// 22
	j	handle_reserved	// 23
	j	handle_reserved	// 24
	j	handle_reserved	// 25
	j	handle_reserved	// 26
	j	handle_reserved	// 27
	j	handle_reserved	// 28
	j	handle_reserved	// 29
	j	handle_reserved	// 30
	j	handle_reserved	// 31
