CROSSDIR := /usr/local/x86_64-elf-toolchain
AS := $(CROSSDIR)/bin/x86_64-elf-as
AR := $(CROSSDIR)/bin/x86_64-elf-ar
CC := $(CROSSDIR)/bin/x86_64-elf-gcc
CXX := $(CROSSDIR)/bin/x86_64-elf-c++
LD := $(CROSSDIR)/bin/x86_64-elf-ld


ASFLAGS := -msyntax=intel -mnaked-reg
CFLAGS := -ffreestanding -nostdlib -mno-red-zone -fno-asynchronous-unwind-tables

LIBGCCDIR := $(dir $(shell $(CC) $(CFLAGS) -print-libgcc-file-name))
LDFLAGS := -L $(LIBGCCDIR) -lgcc
