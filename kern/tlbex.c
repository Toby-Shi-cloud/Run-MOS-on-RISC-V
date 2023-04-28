#include <env.h>
#include <pmap.h>

/* Overview:
 *   When user tries to access to a page which is not map in pgdir, exception 12, 13, or 15
 *   will occur. Therefore, this function has duty to handle these exception.
 */
#if !defined(LAB) || LAB >= 3
void do_tlb_miss(struct Trapframe *tf) {
	u_int addr; // where the user tries to access.
	struct Page *p;
	asm("csrr %0, stval" : "=r"(addr));
	addr = ROUNDDOWN(addr, BY2PG); // align.
	if (addr < UTEMP) panic("address too low.");
	if (addr >= ULIM) panic("permission denied.");
	if (tf->scause == 15) { // Store/AMO page fault
		// alloc a page...
		//todo maybe it's unsafe.
		panic_on(page_alloc(&p));
		page_insert(cur_pgdir, curenv->env_asid, p, addr, PTE_R | PTE_W | PTE_U);
		return;
	}
	panic("bad addr");
}
#endif

#if !defined(LAB) || LAB >= 4
/* Overview:
 *   This is the TLB Mod exception handler in kernel.
 *   Our kernel allows user programs to handle TLB Mod exception in user mode, so we copy its
 *   context 'tf' into UXSTACK and modify the EPC to the registered user exception entry.
 *
 * Hints:
 *   'env_user_tlb_mod_entry' is the user space entry registered using
 *   'sys_set_user_tlb_mod_entry'.
 *
 *   The user entry should handle this TLB Mod exception and restore the context.
 */
void do_tlb_mod(struct Trapframe *tf) {
	struct Trapframe tmp_tf = *tf;

	if (tf->regs[29] < USTACKTOP || tf->regs[29] >= UXSTACKTOP) {
		tf->regs[29] = UXSTACKTOP;
	}
	tf->regs[29] -= sizeof(struct Trapframe);
	*(struct Trapframe *)tf->regs[29] = tmp_tf;

	if (curenv->env_user_tlb_mod_entry) {
		tf->regs[4] = tf->regs[29];
		tf->regs[29] -= sizeof(tf->regs[4]);
		// Hint: Set 'cp0_epc' in the context 'tf' to 'curenv->env_user_tlb_mod_entry'.
		/* Exercise 4.11: Your code here. */

	} else {
		panic("TLB Mod but no user handler registered");
	}
}
#endif
