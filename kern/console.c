#include <console.h>
#include <printk.h>
#include <mmu.h>

// include sbi_ecall_interface to use the macro needed.
#include <sbi/sbi_ecall_interface.h>

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
		"mv a0, %[a0]\n"
		"mv a1, %[a1]\n"
		"mv a2, %[a2]\n"
		"mv a3, %[a3]\n"
		"mv a4, %[a4]\n"
		"mv a5, %[a5]\n"
		"mv a6, %[fid]\n"
		"mv a7, %[eid]\n"
		"ecall\n"
		"mv %[error], a0\n"
		"mv %[value], a1\n"
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
		: "a0", "a1", "a2", "a3",
		"a4", "a5", "a6", "a7", "memory"
	);
	return res;
}

// Do **NOT** use LegacyEcall anymore

void printcharc(char ch) {
	struct sbiret ret = sbi_ecall(
		SBI_EXT_DBCN, SBI_EXT_DBCN_CONSOLE_WRITE_BYTE,
		ch, 0, 0, 0, 0, 0);
	panic_on(ret.error); // should be no error
	assert(ret.value == 0); // expect return 0
}

int scancharc(void) {
	char ch;
	struct sbiret ret = sbi_ecall(
		SBI_EXT_DBCN, SBI_EXT_DBCN_CONSOLE_READ,
		1, (u_int)&ch, 0, 0, 0, 0);
	panic_on(ret.error); // should be no error
	if (ret.value == 0) {
		return 0; // read nothing
	} else if (ret.value == 1) {
		return ch; // read a byte
	} else {
		panic("bad ret.value: %d", ret.value);
	}
	return -1;
}

void nputs(const char *str, u_long length) {
	struct sbiret ret = sbi_ecall(
		SBI_EXT_DBCN, SBI_EXT_DBCN_CONSOLE_WRITE,
		length, (u_int)str, 0, 0, 0, 0);
	panic_on(ret.error); // should be no error
	assert(ret.value == length); // expect return length
}

u_long ngets(char *str, u_long length) {
	struct sbiret ret = sbi_ecall(
		SBI_EXT_DBCN, SBI_EXT_DBCN_CONSOLE_READ,
		length, (u_int)str, 0, 0, 0, 0);
	panic_on(ret.error); // should be no error
	return ret.value;
}

void __attribute__((noreturn)) halt(void) {
	// use assembly directly...
	// if this call sbi_ecall, compiler may optimize out...
	asm (
		"li a0, 0\n"
		"li a1, 0\n"
		"li a2, 0\n"
		"li a3, 0\n"
		"li a4, 0\n"
		"li a5, 0\n"
		"li a6, 0\n"
		// SBI_EXT_SRST = 0x53525354
		"li a7, 0x53525354\n"
		"ecall\n"
	);
	__builtin_unreachable(); // this ecall should not return
	panic("unreachable code");
}

// timer

void clock_set_next_event(u_int next_time) {
	struct sbiret ret = sbi_ecall(
		SBI_EXT_TIME, SBI_EXT_TIME_SET_TIMER,
		next_time, 0, 0, 0, 0, 0);
	panic_on(ret.error); // should be no error
	assert(ret.value == 0); // expect return 0
}
