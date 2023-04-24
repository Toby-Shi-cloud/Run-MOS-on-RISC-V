#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

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

extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
#if !defined(LAB) || LAB >= 4
    [7] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %08x", tf->scause);
}
