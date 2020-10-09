
LIBFTDIR ?= /opt/freetype-2.10.2
LIBFTSRCDIR := $(LIBFTDIR)/src
LIBFTINCDIR := $(LIBFTDIR)/include
FTBUILDDIR := $(BUILDDIR)/freetype

FTCFLAGS := $(CFLAGS) -DFT2_BUILD_LIBRARY -mcmodel=large -I $(INCLUDEDIR) -I $(LIBFTINCDIR) -I $(LIBCINCLUDEDIR)

FTRAWSRCS := $(shell cat ftsrcs)
FTSRCS := $(addprefix $(LIBFTSRCDIR), $(FTRAWSRCS))
FTOBJS := $(addsuffix .o, $(addprefix $(FTBUILDDIR), $(FTRAWSRCS)))

LIBFREETYPE := $(KERNELLIBDIR)/libfreetype.a


$(LIBFREETYPE): $(FTOBJS)
	mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(FTBUILDDIR)/%.c.o: $(LIBFTSRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(FTCFLAGS) -c -o $@ $<
