include include.mk

lab                     ?= $(shell cat .mos-this-lab 2>/dev/null || echo 6)

target_dir              := target
mos_elf                 := $(target_dir)/mos
user_disk               := $(target_dir)/fs.img
link_script             := kernel.lds

modules                 := init lib kern
targets                 := $(mos_elf)
syms_file               := $(target_dir)/prog.syms

lab-ge = $(shell [ "$$(echo $(lab)_ | cut -f1 -d_)" -ge $(1) ] && echo true)

ifeq ($(call lab-ge,3),true)
	user_modules    += user/bare
endif

ifeq ($(call lab-ge,4),true)
	user_modules    += user
endif

ifeq ($(call lab-ge,5),true)
	user_modules    += fs
	targets         += fs-image
endif

qemu_bin                := qemu-system-riscv32
opensbi                 := sbi_bin/fw_dynamic.bin
qemu_flags              += -machine virt -m 64M -nographic -bios $(opensbi)
CFLAGS                  += -DLAB=$(shell echo $(lab) | cut -f1 -d_)

objects                 := $(addsuffix /*.o, $(modules)) $(addsuffix /*.x, $(user_modules))
modules                 += $(user_modules)

.PHONY: all test tools $(modules) clean run dbg objdump fs-image clean-and-all

.ONESHELL:
clean-and-all: clean
	$(MAKE) all

test: export test_dir = tests/lab$(lab)
test: clean-and-all

include mk/tests.mk mk/profiles.mk
export CC CFLAGS LD LDFLAGS lab

all: $(targets)

$(target_dir):
	mkdir -p $@

tools:
	CC="$(HOST_CC)" CFLAGS="$(HOST_CFLAGS)" $(MAKE) --directory=$@

$(modules): tools
	$(MAKE) --directory=$@

$(mos_elf): $(modules) $(target_dir)
	$(LD) $(LDFLAGS) -o $(mos_elf) -T $(link_script) $(objects) $(LDLIBS)

fs-image: $(target_dir) user
	$(MAKE) --directory=fs image fs-files="$(addprefix ../, $(fs-files))"

fs: user
user: lib

clean:
	for d in $(modules) tools/readelf user/* tests/*; do
		if [ -f $$d/Makefile ]; then
			$(MAKE) --directory=$$d clean
		fi
	done
	rm -rf *.o *~ $(target_dir) include/generated
	find . -name '*.objdump' -exec rm {} ';'

dbg: qemu_flags += -no-reboot -s -S
dbg: objdump run

gdb:
	$(CROSS_COMPILE)gdb --eval-command 'target remote :1234' $(mos_elf)

run:
	$(qemu_bin) $(qemu_flags) -kernel $(mos_elf)

objdump:
	@find * \( -name '*.b' -o -path $(mos_elf) \) -exec sh -c \
	'$(CROSS_COMPILE)objdump {} -aldS > {}.objdump && echo {}.objdump' ';'
