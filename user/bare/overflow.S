# This user-space program makes MIPS overflow exception.
#
# To test this manually:
# - Add the following one line in init/init.c:
#	ENV_CREATE(user_bare_overflow);
# - Then build the kernel with `make`

.text
.globl _start
_start:
	li	t0, 0x80000000
	li	t1, 0xffff
	add	t2, t0, t1  # overflow
