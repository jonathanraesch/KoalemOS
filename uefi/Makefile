EFIINCLUDE := /usr/include/efi
EFILIBDIR := /usr/lib

CC := gcc
LD := ld
OBJCOPY := objcopy

CFLAGS := -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -I /usr/include/efi -I /usr/include/efi/x86_64 -DEFI_FUNCTION_WRAPPER
LDFLAGS := -nostdlib -znocombreloc -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -l:libgnuefi.a -l:libefi.a
OBJCOPYFLAGS := -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64

IMGSIZE = 1440

.PHONY = run clean

run: build/main.img
	qemu-system-x86_64 -L /usr/share_ovmf -bios OVMF.fd -net none -hdb $<

build/main.img: build/BOOTX64.efi
	dd if=/dev/zero of=$@ bs=1k count=$(IMGSIZE)
	mformat -i $@ -f $(IMGSIZE) ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ build/BOOTX64.efi ::/EFI/BOOT
	touch build/kernel.elf
	mcopy -i $@ build/kernel.elf ::/EFI/BOOT

build/BOOTX64.efi: build/main.so
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

build/main.so: build/main.o
	$(LD) $< $(EFILIBDIR)/crt0-efi-x86_64.o $(LDFLAGS) -o $@

build/main.o: main.c
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -rf build/*