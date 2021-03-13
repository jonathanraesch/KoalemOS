ENVSUBST := envsubst

KERNELASFLAGS := $(ASFLAGS) -I $(FONTDIR) -I $(APBOOTBUILDDIR)
KERNELCFLAGS := $(CFLAGS) -std=c11 -pedantic-errors -Wall -I $(INCLUDEDIR) -I $(LIBCINCLUDEDIR) -I $(LIBFTINCDIR) -mcmodel=large -Og -ggdb
KERNELLDFLAGS := $(LDFLAGS) -L $(KERNELLIBDIR)

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


$(KERNELBINARY): $(KERNELOBJS) $(LDSCRIPT) $(KLIBC) $(LIBFREETYPE)
	$(LD) -n -T $(LDSCRIPT) $(KERNELLDFLAGS) -o $@ $(KERNELOBJS) -lfreetype -lc

$(OUTCONFDIR)/%: $(CONFDIR)/%
	mkdir -p $(OUTCONFDIR)
	$(ENVSUBST) < $^ > $@

$(KERNELBUILDDIR)/%.s.o: $(KERNELSRCDIR)/%.s $(APBOOTBINARY)
	mkdir -p $(dir $@)
	$(AS) $(KERNELASFLAGS) -o $@ -c $<

$(KERNELBUILDDIR)/%.c.o: $(KERNELSRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(KERNELCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $<
