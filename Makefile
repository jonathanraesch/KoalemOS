SHELL := /bin/sh

srcdir := .
BUILDDIR := build
UEFISRCDIR := $(srcdir)/boot
KERNELSRCDIR := $(srcdir)/kernel

KERNELMAKEFILE := $(KERNELSRCDIR)/Makefile
UEFIMAKEFILE := $(UEFISRCDIR)/Makefile

UEFIIMAGE := $(BUILDDIR)/koalemos-uefi.img

UEFIIMGSIZE := 2880

-include $(KERNELMAKEFILE)
-include $(UEFIMAKEFILE)


.PHONY: all clean run

.DEFAULT_GOAL := all


all: $(UEFIIMAGE)

run: $(UEFIIMAGE)
	qemu-system-x86_64 -bios OVMF.fd -net none -hdb $<

$(UEFIIMAGE): $(UEFIBINARY) $(BINARY)
	dd if=/dev/zero of=$@ bs=1k count=$(UEFIIMGSIZE)
	mformat -i $@ -f $(UEFIIMGSIZE) ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $(UEFIBINARY) ::/EFI/BOOT
	mcopy -i $@ $(BINARY) ::/EFI/BOOT

clean:
	-rm -rf $(BUILDDIR)
