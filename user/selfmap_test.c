#include <lib.h>

Pde *pgdir = (Pde *)(UVPT | UVPT >> 10);

typedef struct _buffer
{
	u_int vaddr;
	u_int paddr;
	u_int size;
	u_int attr;
} buffer;

static void putchar(int ch)
{
	static char buf[64] = {};
	static u_int pos = 0;
	buf[pos++] = ch;
	buf[pos] = 0;
	if (ch == '\n') debugf(buf), pos = 0;
}

static void print(buffer buf)
{
	if (!(buf.attr & PTE_V)) return;
	debugf("%08x %08x %08x ", buf.vaddr, buf.paddr, buf.size);
	if (buf.attr & PTE_R) putchar('r');
	else putchar('-');
	if (buf.attr & PTE_W) putchar('w');
	else putchar('-');
	if (buf.attr & PTE_X) putchar('x');
	else putchar('-');
	if (buf.attr & PTE_U) putchar('u');
	else putchar('-');
	if (buf.attr & PTE_G) putchar('g');
	else putchar('-');
	if (buf.attr & PTE_A) putchar('a');
	else putchar('-');
	if (buf.attr & PTE_D) putchar('d');
	else putchar('-');
	if (buf.attr & PTE_COW) putchar('c');
	else putchar('-');
	if (buf.attr & PTE_LIBRARY) putchar('l');
	else putchar('-');
	putchar('\n');
}

static buffer buf = {};
static void pte_search(Pte *pte) {
	u_int va_pte = ((u_int) pte - UVPT) << 10;
	u_int va, pa, attr;
	if ((u_int) pte == (u_int) pgdir) return;
	for (int i = 0; i < 1024; i++) {
		if (pte[i] & PTE_V) {
			va = va_pte | i << 12;
			pa = PTE_ADDR(pte[i]);
			attr = pte[i] & 0x3ff;
			if (attr == buf.attr &&
			    va == buf.vaddr + buf.size &&
			    pa == buf.paddr + buf.size) {
				buf.size += BY2PG;
			} else {
				print(buf);
				buf.vaddr = va;
				buf.paddr = pa;
				buf.size = BY2PG;
				buf.attr = attr;
			}
		}
	}
}

static void map_search(Pde *pde) {
	for (int i = 0; i < 1024; i++) {
		if (pde[i] & PTE_V) {
			user_assert((pde[i] & 0x3ff) == PTE_V);
			pte_search((Pte *)(UVPT | i << 12));
		}
	}
}

int main() {
	debugf("%-8s %-8s %-8s %-9s\n", "vaddr", "paddr", "size", "attr");
	debugf("-------- -------- -------- ---------\n");
	map_search(pgdir);
	print(buf);
	return 0;
}