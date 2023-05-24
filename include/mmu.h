#ifndef _MMU_H_
#define _MMU_H_

#include <error.h>

#define PMEMSIZE (64 * 1024 * 1024) // 64MB default memsize by qemu
#define BY2PG 4096		// bytes to a page
#define PDMAP (4 * 1024 * 1024) // bytes mapped by a page directory entry
#define PGSHIFT 12
#define PDSHIFT 22 // log2(PDMAP)
#define PDX(va) ((((u_long)(va)) >> 22) & 0x03FF)
#define PTX(va) ((((u_long)(va)) >> 12) & 0x03FF)
#define PTE_ADDR(pte) (((u_long)(pte) & ~0x3FF) << 2) // porting note: page entry in riscv is different.
#define ADDR_PTE(addr) (PPN(addr) << 10)

// Page number field of an address
#define PPN(va) (((u_long)(va)) >> 12)
#define VPN(va) (((u_long)(va)) >> 12)

#define SV32MODE 0x80000000U

/* Page Table/Directory Entry flags */

// Valid bit. If 0 any address matching this entry will cause a tlb miss exception (TLBL/TLBS).
#define PTE_V 0x001

// Read permission bit. If try to load a page without read bit set, a load page-fault exception will occur.
#define PTE_R 0x002

// Write permission bit. If try to store a page without write bit set, a store page-fault exception will occur.
// Note: any page with write bit set should have read bit set.
#define PTE_W 0x004

// Execute permission bit. If try to fetch instruction on a page without execute bit set,
// a fetch page-fault exception will occur.
#define PTE_X 0x008

// User mode permission bit. User mode can only access a page with u-mode bit set.
// If sstatus.SUM is set, s-mode can access a page with and without u-mode bit set.
// Otherwise, s-mode can only access a page without u-mode bit set.
// Usually sstatus.SUM is NOT set, thus causing exception if s-mode software try to access a u-mode page.
#define PTE_U 0x010

// Global bit. When the G bit in a TLB entry is set, that TLB entry will match solely on the VPN
// field, regardless of whether the TLB entry’s ASID field matches the value in EntryHi.
#define PTE_G 0x020

// Access bit. To mark whether this page has been accessed since the last time A-bit cleared.
#define PTE_A 0x040

// Dirty bit. To mark whether this page has been changed since the last time A-bit cleared.
#define PTE_D 0x080

// Copy On Write. Reserved for software, used by fork.
#define PTE_COW 0x100

// Shared memmory. Reserved for software, used by fork.
#define PTE_LIBRARY 0x200

// Memory segments (32-bit kernel mode addresses)
#define KUSEG 0x00000000U
#define KSEG0 0x80000000U
#define KSEG1 0xA0000000U
#define KSEG2 0xC0000000U

/*
 * Part 2.  Our conventions.
 */

/*
 o     4G ----------->  +----------------------------+------------0x100000000
 o                      |       ...                  |  kseg2
 o      KSEG2    -----> +----------------------------+------------0xc000 0000
 o                      |          Devices           |  kseg1
 o      KSEG1    -----> +----------------------------+------------0xa000 0000
 o                      |      Invalid Memory        |   /|\
 o                      +----------------------------+----|-------Physical Memory Max
 o                      |       ...                  |  kseg0
 o      KSTACKTOP-----> +----------------------------+----|-------0x8060 0000-------end
 o                      |       Kernel Stack         |    | KSTKSIZE            /|\
 o                      +----------------------------+----|------                |
 o                      |       Kernel Text          |    |                    PDMAP
 o      KERNBASE -----> +----------------------------+----|-------0x8020 0000    |
 o                      |         Open SBI           |   \|/                    \|/
 o      ULIM     -----> +----------------------------+------------0x8000 0000-------
 o                      |         User VPT           |     PDMAP                /|\
 o      UVPT     -----> +----------------------------+------------0x7fc0 0000    |
 o                      |           pages            |     PDMAP                 |
 o      UPAGES   -----> +----------------------------+------------0x7f80 0000    |
 o                      |           envs             |     PDMAP                 |
 o  UTOP,UENVS   -----> +----------------------------+------------0x7f40 0000    |
 o  UXSTACKTOP -/       |     user exception stack   |     BY2PG                 |
 o                      +----------------------------+------------0x7f3f f000    |
 o                      |                            |     BY2PG                 |
 o      USTACKTOP ----> +----------------------------+------------0x7f3f e000    |
 o                      |     normal user stack      |     BY2PG                 |
 o                      +----------------------------+------------0x7f3f d000    |
 a                      |                            |                           |
 a                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                           |
 a                      .                            .                           |
 a                      .                            .                         kuseg
 a                      .                            .                           |
 a                      |~~~~~~~~~~~~~~~~~~~~~~~~~~~~|                           |
 a                      |                            |                           |
 o       UTEXT   -----> +----------------------------+------------0x0040 0000    |
 o                      |      reserved for COW      |     BY2PG                 |
 o       UCOW    -----> +----------------------------+------------0x003f f000    |
 o                      |   reversed for temporary   |     BY2PG                \|/
 o       UTEMP   -----> +----------------------------+------------0x003f e000-------
 o                      |       invalid memory       |
 a     0 ------------>  +----------------------------+ -----------------------------
 o
*/

#define KERNBASE 0x80200000
#define OPENSBISIZE 0x200000

#define KSTACKTOP 0x80600000
#define ULIM 0x80000000

#define UVPT (ULIM - PDMAP)
#define UPAGES (UVPT - PDMAP)
#define UENVS (UPAGES - PDMAP)

#define UTOP UENVS
#define UXSTACKTOP UTOP

#define USTACKTOP (UTOP - 2 * BY2PG)
#define UTEXT PDMAP
#define UCOW (UTEXT - BY2PG)
#define UTEMP (UCOW - BY2PG)

#ifndef __ASSEMBLER__

/*
 * Part 3.  Our helper functions.
 */
#include <string.h>
#include <types.h>

extern u_long npage;

typedef u_long Pde;
typedef u_long Pte;

// Porting note: in riscv, kva == pa (equivariant mapping)

// translates from kernel virtual address to physical address
#define PADDR(kva)                                                                                 \
	({                                                                                         \
		u_long a = (u_long)(kva);                                                          \
		if (a < ULIM)                                                                      \
			panic("PADDR called with invalid kva %08lx", a);                           \
		(a);                                                                               \
	})

// translates from physical address to kernel virtual address
#define KADDR(pa)                                                                                  \
	({                                                                                         \
		u_long ppn = PPN(pa) - PPN(KERNBASE);                                              \
		if (ppn >= npage) {                                                                \
			panic("KADDR called with invalid pa %08lx", (u_long)pa);                   \
		}                                                                                  \
		(pa);                                                                              \
	})

#define assert(x)                                                                                  \
	do {                                                                                       \
		if (!(x)) {                                                                        \
			panic("assertion failed: %s", #x);                                         \
		}                                                                                  \
	} while (0)

//todo: what's this?
#define TRUP(_p)                                                                                   \
	({                                                                                         \
		typeof((_p)) __m_p = (_p);                                                         \
		(u_int) __m_p > ULIM ? (typeof(_p))ULIM : __m_p;                                   \
	})

#endif //!__ASSEMBLER__
#endif // !_MMU_H_
