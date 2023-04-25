#ifndef _KCLOCK_H_
#define _KCLOCK_H_
// qemu clock frequency: 10MHz = 10000000Hz
// set the timer_irq frequency: 200Hz
#define TIMECLOCK 50000
#ifndef __ASSEMBLER__
void kclock_init(void);
#endif /* !__ASSEMBLER__ */
#endif
