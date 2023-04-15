#include <drivers/console.h>
#include <drivers/sbi.h>
#include <printk.h>

void printcharc(char ch) {
	panic_on(sbi_console_putchar(ch));
}

int scancharc(void) {
	return sbi_console_getchar();
}

void halt(void) {
	sbi_shutdown();
}
