EFIINCLUDE := /usr/include/efi
EFILIBDIR := /usr/lib

UEFICC := gcc
UEFILD := ld
OBJCOPY := objcopy

UEFICFLAGS := -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -I $(INCLUDEDIR) -I /usr/include/efi -I /usr/include/efi/x86_64 -DEFI_FUNCTION_WRAPPER
UEFICFLAGS += $(if $(MAX_HRES),-DMAX_VRES=$(MAX_HRES),)
UEFICFLAGS += $(if $(MAX_VRES),-DMAX_VRES=$(MAX_VRES),)
UEFILDFLAGS := -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -l:libgnuefi.a -l:libefi.a
UEFIOBJCOPYFLAGS := -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64

UEFIBUILDDIR := $(BUILDDIR)/boot
UEFIBINARY := $(UEFIBUILDDIR)/BOOTX64.efi

UEFISRCS := $(foreach srctype, $(SRCTYPES), $(shell find $(UEFISRCDIR) -name *$(srctype)))
UEFIOBJS := $(UEFISRCS:$(UEFISRCDIR)/%=$(UEFIBUILDDIR)/%.o)
UEFIDEPS := $(UEFIOBJS:%.o=%.d)


$(UEFIBINARY): $(UEFIBUILDDIR)/boot.so
	mkdir -p $(dir $@)
	$(OBJCOPY) $(UEFIOBJCOPYFLAGS) $< $@

$(UEFIBUILDDIR)/boot.so: $(UEFIOBJS)
	mkdir -p $(dir $@)
	$(UEFILD) $^ $(EFILIBDIR)/crt0-efi-x86_64.o $(UEFILDFLAGS) -o $@

$(UEFIBUILDDIR)/%.o: $(UEFISRCDIR)/%
	mkdir -p $(dir $@)
	$(UEFICC) $< $(UEFICFLAGS) -MMD -MF $(@:%.o=%.d) -o $@
