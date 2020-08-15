EFIINCLUDE := /usr/include/efi
EFILIBDIR := /usr/lib

CC := gcc
LD := ld
OBJCOPY := objcopy

CFLAGS := -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -I /usr/include/efi -I /usr/include/efi/x86_64 -DEFI_FUNCTION_WRAPPER
LDFLAGS := -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -l:libgnuefi.a -l:libefi.a
OBJCOPYFLAGS := -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64

.PHONY = all clean

all: build/BOOTX64.efi

build/BOOTX64.efi: build/boot.so
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

build/boot.so: build/boot.o
	$(LD) $< $(EFILIBDIR)/crt0-efi-x86_64.o $(LDFLAGS) -o $@

build/boot.o: boot.c
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -rf build/*