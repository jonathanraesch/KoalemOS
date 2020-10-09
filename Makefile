SHELL := /bin/sh

SRCDIR := src
INCLUDEDIR := include
LIBCINCLUDEDIR := $(INCLUDEDIR)/libc
CONFDIR := config
BUILDDIR := build
LIBDIR := $(BUILDDIR)/lib
KERNELLIBDIR := $(LIBDIR)/kernel
USERLIBDIR := $(LIBDIR)/user
UEFISRCDIR := $(SRCDIR)/boot
KERNELSRCDIR := $(SRCDIR)/kernel
LIBCSRCDIR := $(SRCDIR)/libc

CROSSMAKEFILE := cross.mk
FREETYPEMAKEFILE := freetype.mk
KERNELMAKEFILE := $(KERNELSRCDIR)/Makefile
UEFIMAKEFILE := $(UEFISRCDIR)/Makefile
LIBCMAKEFILE := $(LIBCSRCDIR)/Makefile

UEFIIMAGE := $(BUILDDIR)/koalemos-uefi.img

UEFIIMGSIZE := 2880

SRCTYPES := .s .c

-include $(CROSSMAKEFILE)
-include $(LIBCMAKEFILE)
-include $(FREETYPEMAKEFILE)
-include $(KERNELMAKEFILE)
-include $(UEFIMAKEFILE)


.PHONY: all clean run run-debug

.DEFAULT_GOAL := all


all: $(UEFIIMAGE) $(LIBC) $(KLIBC)

run: $(UEFIIMAGE)
	qemu-system-x86_64 -bios OVMF.fd -net none -drive file=$<,format=raw -M q35

run-debug: $(UEFIIMAGE)
	qemu-system-x86_64 -s -bios OVMF.fd -net none -drive file=$<,format=raw -M q35

$(UEFIIMAGE): $(UEFIBINARY) $(KERNELBINARY)
	dd if=/dev/zero of=$@ bs=1k count=$(UEFIIMGSIZE)
	mformat -i $@ -f $(UEFIIMGSIZE) ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $(UEFIBINARY) ::/EFI/BOOT
	mcopy -i $@ $(KERNELBINARY) ::/EFI/BOOT

clean:
	-rm -rf $(BUILDDIR)
