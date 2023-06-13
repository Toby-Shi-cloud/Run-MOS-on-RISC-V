# ENDIAN is either EL (little endian) or EB (big endian)
ENDIAN         := EL

CROSS_COMPILE  := riscv64-unknown-elf-
CC             := $(CROSS_COMPILE)gcc
CFLAGS         := --std=gnu99 -march=rv32gc -mabi=ilp32 -Wall -fno-pic -ffreestanding -fno-stack-protector -fno-builtin -Werror
LD             := $(CROSS_COMPILE)ld
LDFLAGS        := -static -nostdlib --fatal-warnings -melf32lriscv -Lsbi_bin
LDLIBS         := -lsbi -lplatsbi

HOST_CC        := cc
HOST_CFLAGS    += --std=gnu99 -O2 -Wall
HOST_ENDIAN    := $(shell lscpu | grep -iq 'little endian' && echo EL || echo EB)

ifneq ($(HOST_ENDIAN), $(ENDIAN))
# CONFIG_REVERSE_ENDIAN is checked in tools/fsformat.c (lab5)
HOST_CFLAGS    += -DCONFIG_REVERSE_ENDIAN
endif
