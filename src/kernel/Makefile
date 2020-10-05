ENVSUBST := envsubst

KERNELASFLAGS := $(ASFLAGS) -w+all
KERNELCFLAGS := $(CFLAGS) -std=c11 -pedantic-errors -Wall -I $(INCLUDEDIR) -I $(LIBCINCLUDEDIR) -mcmodel=large -Og -ggdb
KERNELLDFLAGS := $(LDFLAGS)

KERNELBUILDDIR := $(BUILDDIR)/kernel
OUTCONFDIR := $(BUILDDIR)/config
ISODIR := $(BUILDDIR)/iso

KERNELSRCS := $(foreach srctype, $(SRCTYPES), $(shell find $(KERNELSRCDIR) -name *$(srctype)))
KERNELOBJS := $(KERNELSRCS:$(KERNELSRCDIR)/%=$(KERNELBUILDDIR)/%.o)
KERNELDEPS := $(KERNELOBJS:%.o=%.d)

KERNELBINARY := $(KERNELBUILDDIR)/koalemos.elf
LDSCRIPT := $(OUTCONFDIR)/linker.ld

export KERNELBUILDDIR

-include $(KERNELDEPS)


$(KERNELBINARY): $(KERNELOBJS) $(LDSCRIPT) $(KLIBC)
	$(LD) -n -T $(LDSCRIPT) $(KERNELLDFLAGS) -o $@ $(KERNELOBJS) $(KLIBC)

$(OUTCONFDIR)/%: $(CONFDIR)/%
	mkdir -p $(OUTCONFDIR)
	$(ENVSUBST) < $^ > $@

$(KERNELBUILDDIR)/%.s.o: $(KERNELSRCDIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(KERNELASFLAGS) -MD $(@:%.o=%.d) -o $@ $<

$(KERNELBUILDDIR)/%.c.o: $(KERNELSRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(KERNELCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $<
