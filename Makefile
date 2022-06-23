SHELL := /bin/sh

SRCDIR := src
INCLUDEDIR := include
LIBCINCLUDEDIR := $(INCLUDEDIR)/libc
CONFDIR := config
BUILDDIR := build
LIBDIR := $(BUILDDIR)/lib
FONTDIR := fonts
KERNELLIBDIR := $(LIBDIR)/kernel
USERLIBDIR := $(LIBDIR)/user
UEFISRCDIR := $(SRCDIR)/boot
KERNELSRCDIR := $(SRCDIR)/kernel
APBOOTSRCDIR := $(SRCDIR)/ap_boot
LIBCSRCDIR := $(SRCDIR)/libc

CROSSMAKEFILE := cross.mk
FREETYPEMAKEFILE := freetype.mk
KERNELMAKEFILE := $(KERNELSRCDIR)/Makefile
APBOOTMAKEFILE := $(APBOOTSRCDIR)/Makefile
UEFIMAKEFILE := $(UEFISRCDIR)/Makefile
LIBCMAKEFILE := $(LIBCSRCDIR)/Makefile

UEFIIMAGE := $(BUILDDIR)/koalemos-uefi.img

UEFIIMGSIZE := 2880

SRCTYPES := .s .c

QEMUFLAGS := -bios OVMF.fd -smp 4 -net none -M q35 -drive file=$(UEFIIMAGE),format=raw

-include $(CROSSMAKEFILE)
-include $(LIBCMAKEFILE)
-include $(FREETYPEMAKEFILE)
-include $(APBOOTMAKEFILE)
-include $(KERNELMAKEFILE)
-include $(UEFIMAKEFILE)


.PHONY: all clean run run-debug

.DEFAULT_GOAL := all


all: $(UEFIIMAGE) $(LIBC) $(KLIBC)

run: $(UEFIIMAGE)
	qemu-system-x86_64 $(QEMUFLAGS)

run-debug: $(UEFIIMAGE)
	qemu-system-x86_64 -s $(QEMUFLAGS)

run-wait-debug: $(UEFIIMAGE)
	qemu-system-x86_64 -s -S $(QEMUFLAGS)

$(UEFIIMAGE): $(UEFIBINARY) $(KERNELBINARY)
	dd if=/dev/zero of=$@ bs=1k count=$(UEFIIMGSIZE)
	mformat -i $@ -f $(UEFIIMGSIZE) ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $(UEFIBINARY) ::/EFI/BOOT
	mcopy -i $@ $(KERNELBINARY) ::/EFI/BOOT

clean:
	-rm -rf $(BUILDDIR)
