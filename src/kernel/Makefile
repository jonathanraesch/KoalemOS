CROSSDIR := /usr/local/x86_64-elf-toolchain
CC := $(CROSSDIR)/bin/x86_64-elf-gcc
AS := nasm
LD := $(CROSSDIR)/bin/x86_64-elf-ld

ENVSUBST := envsubst

KERNELASFLAGS := -w+all -f elf64
KERNELCFLAGS := -std=c11 -pedantic-errors -Wall -I $(INCLUDEDIR) -mcmodel=large -ffreestanding -Og -ggdb -nostdlib -mno-red-zone -masm=intel -fno-asynchronous-unwind-tables

BINDIR := $(BUILDDIR)/bin
KERNELBINDIR := $(BINDIR)/kernel
OUTCONFDIR := $(BUILDDIR)/config
ISODIR := $(BUILDDIR)/iso

LIBGCCDIR := $(dir $(shell $(CC) $(CFLAGS) -print-libgcc-file-name))

SRCTYPES := .s .c
SRCS := $(foreach srctype, $(SRCTYPES), $(shell find $(KERNELSRCDIR) -name *$(srctype)))
OBJS := $(SRCS:$(KERNELSRCDIR)/%=$(KERNELBINDIR)/%.o)
DEPS := $(OBJS:%.o=%.d)

ELFFILE := $(BINDIR)/koalemos.elf
BINARY := $(BINDIR)/koalemos.bin
LDSCRIPT := $(OUTCONFDIR)/linker.ld

export KERNELBINDIR

-include $(DEPS)


$(BINARY): $(OBJS) $(LDSCRIPT)
	$(LD) -n -T $(LDSCRIPT) -L $(LIBGCCDIR) -lgcc -o $(ELFFILE) $(OBJS)
	objcopy -O binary $(ELFFILE) $@

$(OUTCONFDIR)/%: $(CONFDIR)/%
	mkdir -p $(OUTCONFDIR)
	$(ENVSUBST) < $^ > $@

$(KERNELBINDIR)/%.s.o: $(KERNELSRCDIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(KERNELASFLAGS) -MD $(@:%.o=%.d) -o $@ $<

$(KERNELBINDIR)/%.c.o: $(KERNELSRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(KERNELCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $<
