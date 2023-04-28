#include <console.h>
#include <printk.h>
#include <types.h>

struct sbiret {
	u_int error;
	u_int value;
};

/* Overview:
 * 	use ecall to access Machine Level
 */
static inline struct sbiret
sbi_ecall(u_int ext, u_int fid, u_int arg0, u_int arg1,
	  u_int arg2, u_int arg3, u_int arg4, u_int arg5) {
	struct sbiret res;
	asm (
		"addi sp, sp, -8\n"
		"mv t0, %[a0]\n"
		"mv t1, %[a1]\n"
		"mv t2, %[a2]\n"
		"mv t3, %[a3]\n"
		"mv t4, %[a4]\n"
		"mv t5, %[a5]\n"
		"sw %[fid], 0(sp)\n"
		"sw %[eid], 4(sp)\n"
		"mv a0, t0\n"
		"mv a1, t1\n"
		"mv a2, t2\n"
		"mv a3, t3\n"
		"mv a4, t4\n"
		"mv a5, t5\n"
		"lw a6, 0(sp)\n"
		"lw a7, 4(sp)\n"
		"ecall\n"
		"mv %[error], a0\n"
		"mv %[value], a1\n"
		"addi sp, sp, 8\n"
		: [error] "=r" (res.error),
		[value] "=r" (res.value)
		: [a0] "r" (arg0),
		[a1] "r" (arg1),
		[a2] "r" (arg2),
		[a3] "r" (arg3),
		[a4] "r" (arg4),
		[a5] "r" (arg5),
		[fid] "r" (fid),
		[eid] "r" (ext)
		: "memory"
	);
	return res;
}

#define LegacyEcall(ext, a0, a1, a2) sbi_ecall((ext), 0, (a0), (a1), (a2), 0, 0, 0)

void printcharc(char ch) {
	panic_on(LegacyEcall(1, ch, 0, 0).error);
}

int scancharc(void) {
	return LegacyEcall(2, 0, 0, 0).value;
}

void halt(void) {
	panic_on(LegacyEcall(8, 0, 0, 0).error);
}

void clock_set_next_event(u_int next_time) {
	panic_on(LegacyEcall(0, next_time, 0, 0).error);
}
