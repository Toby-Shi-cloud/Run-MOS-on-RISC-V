#include <console.h>
#include <printk.h>
#include <types.h>

/* Overview:
 * 	use ecall to access Machine Level
 */
static inline uint32_t sbi_call(uint32_t sbi_type, uint32_t arg0, uint32_t arg1, uint32_t arg2) {
	uint32_t ret_val;
	asm("mv a0, %0" : : "r"(arg0));
	asm("mv a1, %0" : : "r"(arg1));
	asm("mv a2, %0" : : "r"(arg2));
	asm("mv a7, %0" : : "r"(sbi_type));
	asm("ecall");
	asm("mv %0, a0" : "=r"(ret_val) :);
	return ret_val;
}

void printcharc(char ch) {
	panic_on(sbi_call(1, ch, 0, 0));
}

int scancharc(void) {
	return sbi_call(2, 0, 0, 0);
}

void halt(void) {
	sbi_call(8, 0, 0, 0);
}
