#ifndef _SBI_H_
#define _SBI_H_
#include <types.h>

#define SBI_SUCCESS 0
#define SBI_ERR_FAILED -1
#define SBI_ERR_NOT_SUPPORTED -2
#define SBI_ERR_INVALID_PARAM -3
#define SBI_ERR_DENIED -4
#define SBI_ERR_INVALID_ADDRESS -5
#define SBI_ERR_ALREADY_AVAILABLE -6
#define SBI_ERR_ALREADY_STARTED -7
#define SBI_ERR_ALREADY_STOPPED -8

long sbi_set_timer(uint32_t stime_value);
long sbi_console_putchar(int ch);
long sbi_console_getchar(void);
long sbi_clear_ipi(void);
long sbi_send_ipi(const unsigned long *hart_mask);
long sbi_remote_fence_i(const unsigned long *hart_mask);
long sbi_remote_sfence_vma(const unsigned long *hart_mask,
                           unsigned long start,
                           unsigned long size);
long sbi_remote_sfence_vma_asid(const unsigned long *hart_mask,
                                unsigned long start,
                                unsigned long size,
                                unsigned long asid);
void sbi_shutdown(void);

#endif