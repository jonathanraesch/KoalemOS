SHELL := /bin/sh

SRCDIR := src
INCLUDEDIR := include
CONFDIR := config
BUILDDIR := build
UEFISRCDIR := $(SRCDIR)/boot
KERNELSRCDIR := $(SRCDIR)/kernel

KERNELMAKEFILE := $(KERNELSRCDIR)/Makefile
UEFIMAKEFILE := $(UEFISRCDIR)/Makefile

UEFIIMAGE := $(BUILDDIR)/koalemos-uefi.img

UEFIIMGSIZE := 2880

-include $(KERNELMAKEFILE)
-include $(UEFIMAKEFILE)


.PHONY: all clean run run-debug

.DEFAULT_GOAL := all


all: $(UEFIIMAGE)

run: $(UEFIIMAGE)
	qemu-system-x86_64 -bios OVMF.fd -net none -drive file=$<,format=raw -M q35

run-debug: $(UEFIIMAGE)
	qemu-system-x86_64 -s -bios OVMF.fd -net none -drive file=$<,format=raw -M q35

$(UEFIIMAGE): $(UEFIBINARY) $(BINARY)
	dd if=/dev/zero of=$@ bs=1k count=$(UEFIIMGSIZE)
	mformat -i $@ -f $(UEFIIMGSIZE) ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $(UEFIBINARY) ::/EFI/BOOT
	mcopy -i $@ $(BINARY) ::/EFI/BOOT

clean:
	-rm -rf $(BUILDDIR)
