#include <env.h>
#include <pmap.h>

static void passive_alloc(u_int va, Pde *pgdir, u_int asid) {
	struct Page *p = NULL;

	if (va < UTEMP) {
		panic("address too low");
	}

	if (va >= USTACKTOP && va < USTACKTOP + BY2PG) {
		panic("invalid memory");
	}

	if (va >= UENVS && va < UPAGES) {
		panic("envs zone");
	}

	if (va >= UPAGES && va < UVPT) {
		panic("pages zone");
	}

	if (va >= ULIM) {
		panic("kernel address");
	}

	panic_on(page_alloc(&p));
	panic_on(page_insert(pgdir, asid, p, va, PTE_R | PTE_W | PTE_U));
}

/* Overview:
 *   When user tries to access to a page which is not map in pgdir, exception 12, 13, or 15
 *   will occur. Therefore, this function has duty to handle these exception.
 */
void do_tlb_miss(struct Trapframe *tf) {
	u_int addr; // where the user tries to access.
	struct Page *p;
	addr = tf->stval;
	addr = ROUNDDOWN(addr, BY2PG); // align.
	if (tf->scause == 15) { // Store/AMO page fault
		// alloc a page...
		//todo maybe it's unsafe.
		passive_alloc(addr, cur_pgdir, curenv->env_asid);
		return;
	} else if (tf->scause == 13) { // Load page fault
		if (addr == USTACKTOP - BY2PG) {
			// init stack
			passive_alloc(addr, cur_pgdir, curenv->env_asid);
			return;
		}
		if (addr < UVPT || addr >= ULIM) {
			panic("bad addr");
		}
		if (curenv->env_pgdir_copy_pa == 0) {
			panic_on(page_alloc(&p));
			p->pp_ref++;
			curenv->env_pgdir_copy_pa = page2pa(p);
		} else {
			p = pa2page(curenv->env_pgdir_copy_pa);
		}
		u_int kva = page2kva(p);
		Pte * pte = (Pte *)kva;
		memcpy((void *)kva, (void *)cur_pgdir, BY2PG);
		for (int i = 0; i < 1024; i++) {
			if (pte[i] & PTE_V) {
				pte[i] |= PTE_R | PTE_U;
			}
		}
		pte[PDX(UVPT)] = ADDR_PTE(cur_pgdir) | PTE_V | PTE_R | PTE_U;
		cur_pgdir[PDX(UVPT)] = ADDR_PTE(kva) | PTE_V;
		return;
	}
	panic("bad addr");
}

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
