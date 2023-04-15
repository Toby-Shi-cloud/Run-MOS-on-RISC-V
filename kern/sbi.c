#include <drivers/sbi.h>
#include <printk.h>

/* Overview:
 * 	use ecall to access Machine Level
 */
static uint32_t sbi_call(uint32_t sbi_type, uint32_t arg0, uint32_t arg1, uint32_t arg2) {
	uint32_t ret_val;
	asm("mv a0, %0" : : "r"(arg0));
	asm("mv a1, %0" : : "r"(arg1));
	asm("mv a2, %0" : : "r"(arg2));
	asm("mv a7, %0" : : "r"(sbi_type));
	asm("ecall");
	asm("mv %0, a0" : "=r"(ret_val) :);
	return ret_val;
}

#define sbi_call_zero(sbi_type) sbi_call((sbi_type), 0, 0, 0)
#define sbi_call_one(sbi_type, arg) sbi_call((sbi_type), (arg), 0, 0)

/**
 * Programs the clock for next event after stime_value time. This function also
 * clears the pending timer interrupt bit.
 * 
 * If the supervisor wishes to clear the timer interrupt without scheduling the
 * next timer event, it can either request a timer interrupt infinitely far into
 * the future (i.e., (uint64_t)-1), or it can instead mask the timer interrupt
 * by clearing sie.STIE CSR bit.
 * 
 * This SBI call returns 0 upon success or an implementation specific negative error code.
 */
long sbi_set_timer(uint32_t stime_value) {
	panic("not yet implemented");
}

/**
 * Write data present in ch to debug console.
 * 
 * Unlike sbi_console_getchar(), this SBI call will block if there remain any pending
 * characters to be transmitted or if the receiving terminal is not yet ready to receive
 * the byte. However, if the console doesn’t exist at all, then the character is thrown away.
 * 
 * This SBI call returns 0 upon success or an implementation specific negative error code.
 */
long sbi_console_putchar(int ch) {
	return sbi_call_one(1, ch);
}

/**
 * Read a byte from debug console.
 * 
 * The SBI call returns the byte on success, or -1 for failure.
 */
long sbi_console_getchar(void) {
	return sbi_call_zero(2);
}

/**
 * Clears the pending IPIs if any. The IPI is cleared only in the hart for which this
 * SBI call is invoked. sbi_clear_ipi() is deprecated because S-mode code can clear
 * sip.SSIP CSR bit directly.
 * 
 * This SBI call returns 0 if no IPI had been pending, or an implementation specific
 * positive value if an IPI had been pending.
 */
long sbi_clear_ipi(void) {
	panic("not yet implemented");
}

/**
 * Send an inter-processor interrupt to all the harts defined in hart_mask.Interprocessor
 * interrupts manifest at the receiving harts as Supervisor Software Interrupts.
 * 
 * hart_mask is a virtual address that points to a bit-vector of harts. The bit vector is
 * represented as a sequence of unsigned longs whose length equals the number of harts in
 * the system divided by the number of bits in an unsigned long, rounded up to the next integer.
 * 
 * This SBI call returns 0 upon success or an implementation specific negative error code.
 */
long sbi_send_ipi(const unsigned long *hart_mask) {
	panic("not yet implemented");
}

/**
 * Instructs remote harts to execute FENCE.I instruction. The hart_mask is same as
 * described in sbi_send_ipi().
 * 
 * This SBI call returns 0 upon success or an implementation specific negative error code.
 */
long sbi_remote_fence_i(const unsigned long *hart_mask) {
	panic("not yet implemented");
}

/**
 * Instructs the remote harts to execute one or more SFENCE.VMA instructions, covering
 * the range of virtual addresses between start and size.
 * 
 * This SBI call returns 0 upon success or an implementation specific negative error code.
 */
long sbi_remote_sfence_vma(
	const unsigned long *hart_mask, unsigned long start, unsigned long size) {
	panic("not yet implemented");
}

/**
 * Instruct the remote harts to execute one or more SFENCE.VMA instructions, covering
 * the range of virtual addresses between start and size. This covers only the given ASID.
 * 
 * This SBI call returns 0 upon success or an implementation specific negative error code.
 */
long sbi_remote_sfence_vma_asid(
	const unsigned long *hart_mask, unsigned long start,
	unsigned long size, unsigned long asid) {
	panic("not yet implemented");
}

/**
 * Puts all the harts to shutdown state from supervisor point of view.
 * 
 * This SBI call doesn’t return irrespective whether it succeeds or fails.
 */
void sbi_shutdown(void) {
	sbi_call_zero(8);
}
