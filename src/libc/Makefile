
LIBCASFLAGS := $(ASFLAGS) -w+all
LIBCCFLAGS := $(CFLAGS) -I $(INCLUDEDIR) -std=c11 -pedantic-errors -Wall -O3

KLIBCASFLAGS := $(ASFLAGS) -w+all -D __klibc__
KLIBCCFLAGS := $(CFLAGS) -I $(INCLUDEDIR) -std=c11 -pedantic-errors -Wall -O3 -D__klibc__


LIBCBUILDDIR := $(BUILDDIR)/libc
KLIBCBUILDDIR := $(BUILDDIR)/klibc

LIBCSRCS := $(foreach srctype, $(SRCTYPES), $(shell find $(LIBCSRCDIR) -name *$(srctype)))
LIBCOBJS := $(LIBCSRCS:$(LIBCSRCDIR)/%=$(LIBCBUILDDIR)/%.o)
KLIBCOBJS := $(LIBCSRCS:$(LIBCSRCDIR)/%=$(KLIBCBUILDDIR)/%.o)
LIBCDEPS := $(LIBCOBJS:%.o=%.d)
KLIBCDEPS := $(KLIBCOBJS:%.o=%.d)

LIBC := $(LIBCBUILDDIR)/libc.a
KLIBC := $(KLIBCBUILDDIR)/klibc.a


-include $(LIBCDEPS)
-include $(KLIBCDEPS)


$(LIBC): $(LIBCOBJS)
	$(RM) -f $@
	$(AR) -crs $@ $^

$(LIBCBUILDDIR)/%.s.o: $(LIBCSRCDIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(LIBCASFLAGS) -MD $(@:%.o=%.d) -o $@ $<

$(LIBCBUILDDIR)/%.c.o: $(LIBCSRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(LIBCCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $<


$(KLIBC): $(KLIBCOBJS)
	$(RM) -f $@
	$(AR) -crs $@ $^

$(KLIBCBUILDDIR)/%.s.o: $(LIBCSRCDIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(KLIBCASFLAGS) -MD $(@:%.o=%.d) -o $@ $<

$(KLIBCBUILDDIR)/%.c.o: $(LIBCSRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(KLIBCCFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ -c $<
