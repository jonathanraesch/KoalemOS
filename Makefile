SHELL := /bin/sh

CROSSDIR := /usr/local/x86_64-elf-toolchain
CC := $(CROSSDIR)/bin/x86_64-elf-gcc
AS := nasm
LD := $(CROSSDIR)/bin/x86_64-elf-ld
LIBC := $(CROSSDIR)/lib/gcc/x86_64-elf/10.2.0

ENVSUBST := envsubst
OBJCOPY := objcopy

ASFLAGS := -w+all -f elf64
CFLAGS := -Wall -ffreestanding -O0 -nostdlib -mno-red-zone -masm=intel -fno-asynchronous-unwind-tables
BOOTASFLAGS := $(ASFLAGS)
BOOTCFLAGS := $(CFLAGS) -m32
KERNELASFLAGS := $(ASFLAGS)
KERNELCFLAGS := $(CFLAGS)

srcdir := .
BUILDDIR := build
BINDIR := $(BUILDDIR)/bin
BOOTSRCDIR := $(srcdir)/boot
BOOTBINDIR := $(BINDIR)/boot
UEFISRCDIR := $(srcdir)/boot-uefi
KERNELSRCDIR := $(srcdir)/kernel
KERNELBINDIR := $(BINDIR)/kernel
INCONFDIR := $(srcdir)/config
OUTCONFDIR := $(BUILDDIR)/config
ISODIR := $(BUILDDIR)/iso

SRCTYPES := .s .S .c .i .ii .cc .cxx .cpp .C
BOOTSRCS := $(foreach srctype, $(SRCTYPES), $(shell find $(BOOTSRCDIR) -name *$(srctype)))
KERNELSRCS := $(foreach srctype, $(SRCTYPES), $(shell find $(KERNELSRCDIR) -name *$(srctype)))
SRCS := $(BOOTSRCS) $(KERNELSRCS)
OBJS := $(SRCS:$(srcdir)/%=$(BINDIR)/%.o)
DEPS := $(OBJS:%.o=%.d)
UEFIMAKEFILE := $(UEFISRCDIR)/Makefile

MBHEADEROBJ := $(BOOTBINDIR)/multiboot_header.c.o
BINARY := $(BINDIR)/koalemos.elf
OSIMAGE := $(BUILDDIR)/koalemos.iso
UEFIIMAGE := $(BUILDDIR)/koalemos-uefi.img
LDSCRIPT := $(OUTCONFDIR)/linker.ld
GRUBCONFIG := $(OUTCONFDIR)/grub.cfg

DEFAULT_EMULATION = run-qemu
BOCHSCONFIG := $(OUTCONFDIR)/bochsrc
BOCHSDEBUG := $(OUTCONFDIR)/bochsdebug.rc

UEFIIMGSIZE := 2880

export BOOTBINDIR
export KERNELBINDIR
export MBHEADEROBJ
export GRUBBINPATH := /boot/$(notdir $(BINARY))
export BOCHSOUT := $(BUILDDIR)/bochsout.txt
export OSIMAGE


-include $(DEPS)
-include $(UEFIMAKEFILE)


.PHONY: all clean run run-qemu run-bochs run-uefi

.DEFAULT_GOAL := all


all: $(OSIMAGE) $(UEFIIMAGE)

run: $(DEFAULT_EMULATION)

run-qemu: $(OSIMAGE)
	qemu-system-x86_64 $(OSIMAGE)

run-bochs: $(OSIMAGE) $(BOCHSCONFIG) $(BOCHSDEBUG)
	bochs -qf $(BOCHSCONFIG) -rc $(BOCHSDEBUG)

run-uefi: $(UEFIIMAGE)
	qemu-system-x86_64 -bios OVMF.fd -net none -hdb $<

$(UEFIIMAGE): $(UEFIBINARY) $(BINARY)
	dd if=/dev/zero of=$@ bs=1k count=$(UEFIIMGSIZE)
	mformat -i $@ -f $(UEFIIMGSIZE) ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $(UEFIBINARY) ::/EFI/BOOT
	mcopy -i $@ $(BINARY) ::/EFI/BOOT

$(OSIMAGE): $(BINARY) $(GRUBCONFIG)
	mkdir -p $(ISODIR)/boot/grub
	cp $(GRUBCONFIG) $(ISODIR)/boot/grub
	cp $(BINARY) $(ISODIR)/boot/
	grub-mkrescue -o $@ $(ISODIR)

$(BINARY): $(OBJS) $(LDSCRIPT)
	$(LD) -n -L $(LIBC) -lgcc -T $(LDSCRIPT) -o $@ $(OBJS)
	grub-file --is-x86-multiboot2 $(BINARY)

$(OUTCONFDIR)/%: $(INCONFDIR)/%
	mkdir -p $(OUTCONFDIR)
	export MBHEADEROBJ=$(MBHEADEROBJ)
	$(ENVSUBST) < $^ > $@

$(BOOTBINDIR)/%.s.o: $(BOOTSRCDIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(BOOTASFLAGS) -MD $(@:%.o=%.d) -o $@ $< 

$(BOOTBINDIR)/%.S.o: $(BOOTSRCDIR)/%.S
	mkdir -p $(dir $@)
	$(AS) $(BOOTASFLAGS) -MD $(@:%.o=%.d) -o $@ $< 

$(BOOTBINDIR)/%.o: $(BOOTSRCDIR)/%
	mkdir -p $(dir $@)
	$(CC) $(BOOTCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $< 
	$(OBJCOPY) -O elf64-x86-64 $@

$(KERNELBINDIR)/%.s.o: $(KERNELSRCDIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(KERNELASFLAGS) -MD $(@:%.o=%.d) -o $@ $< 

$(KERNELBINDIR)/%.S.o: $(KERNELSRCDIR)/%.S
	mkdir -p $(dir $@)
	$(AS) $(KERNELASFLAGS) -MD $(@:%.o=%.d) -o $@ $< 

$(KERNELBINDIR)/%.o: $(KERNELSRCDIR)/%
	mkdir -p $(dir $@)
	$(CC) $(KERNELCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $< 

clean:
	-rm -rf $(BUILDDIR)
