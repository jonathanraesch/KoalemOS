CROSSDIR := /usr/local/x86_64-elf-toolchain
CC := $(CROSSDIR)/bin/x86_64-elf-gcc
AS := nasm
LD := $(CROSSDIR)/bin/x86_64-elf-ld

ENVSUBST := envsubst

KERNELASFLAGS := -w+all -f elf64
KERNELCFLAGS := -std=c11 -pedantic-errors -Wall -mcmodel=large -ffreestanding -O0 -nostdlib -mno-red-zone -masm=intel -fno-asynchronous-unwind-tables

BINDIR := $(BUILDDIR)/bin
KERNELBINDIR := $(BINDIR)/kernel
INCONFDIR := $(srcdir)/config
OUTCONFDIR := $(BUILDDIR)/config
ISODIR := $(BUILDDIR)/iso

SRCTYPES := .s .S .c .i .ii .cc .cxx .cpp .C
SRCS := $(foreach srctype, $(SRCTYPES), $(shell find $(KERNELSRCDIR) -name *$(srctype)))
OBJS := $(SRCS:$(srcdir)/%=$(BINDIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

BINARY := $(BINDIR)/koalemos.bin
LDSCRIPT := $(OUTCONFDIR)/linker.ld

export KERNELBINDIR

-include $(DEPS)

.PHONY: all
.DEFAULT_GOAL := all


all: $(BINARY)

$(BINARY): $(OBJS) $(LDSCRIPT)
	$(LD) -n -T $(LDSCRIPT) -o $@ $(OBJS)

$(OUTCONFDIR)/%: $(INCONFDIR)/% 
	mkdir -p $(OUTCONFDIR)
	$(ENVSUBST) < $^ > $@

$(KERNELBINDIR)/%.s.o: $(KERNELSRCDIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(KERNELASFLAGS) -MD $(@:%.o=%.d) -o $@ $< 

$(KERNELBINDIR)/%.S.o: $(KERNELSRCDIR)/%.S
	mkdir -p $(dir $@)
	$(AS) $(KERNELASFLAGS) -MD $(@:%.o=%.d) -o $@ $< 

$(KERNELBINDIR)/%.o: $(KERNELSRCDIR)/%
	mkdir -p $(dir $@)
	$(CC) $(KERNELCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $< 
