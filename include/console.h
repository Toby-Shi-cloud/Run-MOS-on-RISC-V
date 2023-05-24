#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include <types.h>

void printcharc(char ch);
int scancharc(void);
void nputs(const char *str, u_long length);
u_long ngets(char *str, u_long length);
void __attribute__((noreturn)) halt(void);

#endif
