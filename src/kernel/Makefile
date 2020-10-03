ENVSUBST := envsubst

KERNELASFLAGS := $(ASFLAGS) -w+all
KERNELCFLAGS := $(CFLAGS) -std=c11 -pedantic-errors -Wall -I $(INCLUDEDIR) -mcmodel=large -Og -ggdb
KERNELLDFLAGS := $(LDFLAGS)

KERNELBUILDDIR := $(BUILDDIR)/kernel
OUTCONFDIR := $(BUILDDIR)/config
ISODIR := $(BUILDDIR)/iso

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
	$(LD) -n -T $(LDSCRIPT) $(KERNELLDFLAGS) -o $(KERNELELFFILE) $(KERNELOBJS)
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
