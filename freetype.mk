
LIBFTDIR ?= /opt/freetype-2.10.2
LIBFTSRCDIR := $(LIBFTDIR)/src
LIBFTINCDIR := $(LIBFTDIR)/include
FTBUILDDIR := $(BUILDDIR)/freetype

FTCFLAGS := $(CFLAGS) -DFT2_BUILD_LIBRARY -mcmodel=large -I $(INCLUDEDIR) -I $(LIBFTINCDIR) -I $(LIBCINCLUDEDIR)

FTSRCSFILE := $(CONFDIR)/ftsrcs
FTRAWSRCS := $(shell cat $(FTSRCSFILE))
FTSRCS := $(addprefix $(LIBFTSRCDIR), $(FTRAWSRCS))
FTOBJS := $(addsuffix .o, $(addprefix $(FTBUILDDIR), $(FTRAWSRCS)))
FTCONFHEADERS := $(INCLUDEDIR)/freetype/config/ftmodule.h $(INCLUDEDIR)/freetype/config/ftoption.h

LIBFREETYPE := $(KERNELLIBDIR)/libfreetype.a


$(LIBFREETYPE): $(FTOBJS) $(FTSRCSFILE)
	mkdir -p $(dir $@)
	$(AR) rcs $@ $(FTOBJS)

$(FTBUILDDIR)/%.c.o: $(LIBFTSRCDIR)/%.c $(FTCONFHEADERS)
	mkdir -p $(dir $@)
	$(CC) $(FTCFLAGS) -c -o $@ $<
