CROSSDIR := /usr/local/x86_64-elf-toolchain
AS := nasm
AR := $(CROSSDIR)/bin/x86_64-elf-ar
CC := $(CROSSDIR)/bin/x86_64-elf-gcc
CXX := $(CROSSDIR)/bin/x86_64-elf-c++
LD := $(CROSSDIR)/bin/x86_64-elf-ld


ASFLAGS := -f elf64
CFLAGS := -ffreestanding -nostdlib -mno-red-zone -fno-asynchronous-unwind-tables

LIBGCCDIR := $(dir $(shell $(CC) $(CFLAGS) -print-libgcc-file-name))
LDFLAGS := -L $(LIBGCCDIR) -lgcc
