EFIINCLUDE := /usr/include/efi
EFILIBDIR := /usr/lib

UEFICC := gcc
UEFILD := ld
OBJCOPY := objcopy

UEFICFLAGS := -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -I /usr/include/efi -I /usr/include/efi/x86_64 -DEFI_FUNCTION_WRAPPER
UEFILDFLAGS := -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -l:libgnuefi.a -l:libefi.a
UEFIOBJCOPYFLAGS := -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64

UEFIBUILDDIR := $(BUILDDIR)/uefi
UEFIBINARY := $(UEFIBUILDDIR)/BOOTX64.efi

$(UEFIBINARY): $(UEFIBUILDDIR)/boot.so
	mkdir -p $(dir $@)
	$(OBJCOPY) $(UEFIOBJCOPYFLAGS) $< $@

$(UEFIBUILDDIR)/boot.so: $(UEFIBUILDDIR)/boot.o
	mkdir -p $(dir $@)
	$(UEFILD) $< $(EFILIBDIR)/crt0-efi-x86_64.o $(UEFILDFLAGS) -o $@

$(UEFIBUILDDIR)/boot.o: $(UEFISRCDIR)/boot.c
	mkdir -p $(dir $@)
	$(UEFICC) $< $(UEFICFLAGS) -o $@