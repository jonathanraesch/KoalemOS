CROSSDIR := /usr/local/x86_64-elf-toolchain
CC := $(CROSSDIR)/bin/x86_64-elf-gcc
AS := nasm
LD := $(CROSSDIR)/bin/x86_64-elf-ld

ENVSUBST := envsubst

KERNELASFLAGS := -w+all -f elf64
KERNELCFLAGS := -std=c11 -pedantic-errors -Wall -I $(INCLUDEDIR) -mcmodel=large -ffreestanding -Og -ggdb -nostdlib -mno-red-zone -masm=intel -fno-asynchronous-unwind-tables

KERNELBUILDDIR := $(BUILDDIR)/kernel
OUTCONFDIR := $(BUILDDIR)/config
ISODIR := $(BUILDDIR)/iso

LIBGCCDIR := $(dir $(shell $(CC) $(CFLAGS) -print-libgcc-file-name))

SRCTYPES := .s .c
KERNELSRCS := $(foreach srctype, $(SRCTYPES), $(shell find $(KERNELSRCDIR) -name *$(srctype)))
KERNELOBJS := $(KERNELSRCS:$(KERNELSRCDIR)/%=$(KERNELBUILDDIR)/%.o)
KERNELDEPS := $(KERNELOBJS:%.o=%.d)

KERNELELFFILE := $(KERNELBUILDDIR)/koalemos.elf
KERNELBINARY := $(KERNELBUILDDIR)/koalemos.bin
LDSCRIPT := $(OUTCONFDIR)/linker.ld

export KERNELBUILDDIR

-include $(KERNELDEPS)


$(KERNELBINARY): $(KERNELOBJS) $(LDSCRIPT)
	$(LD) -n -T $(LDSCRIPT) -L $(LIBGCCDIR) -lgcc -o $(KERNELELFFILE) $(KERNELOBJS)
	objcopy -O binary $(KERNELELFFILE) $@

$(OUTCONFDIR)/%: $(CONFDIR)/%
	mkdir -p $(OUTCONFDIR)
	$(ENVSUBST) < $^ > $@

$(KERNELBUILDDIR)/%.s.o: $(KERNELSRCDIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(KERNELASFLAGS) -MD $(@:%.o=%.d) -o $@ $<

$(KERNELBUILDDIR)/%.c.o: $(KERNELSRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(KERNELCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $<
