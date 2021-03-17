# KoalemOS #

KoalemOS is a hobby project to explore OS development.

## License

This software is copyright © 2020 Jonathan Räsch and is released under the GNU GPL v3 license (`LICENSE`), unless otherwise specified.

Portions of this software are copyright © 2020 The FreeType Project ([www.freetype.org](https://www.freetype.org/)).  All rights reserved.
Those portions can be found in `include/freetype` and are released under the FreeType License (`FTL.TXT`).

The Roboto Mono font, designed by Christian Robertson, is released under the Apache License, Version 2 (`APACHE-LICENSE`).
It can be found in the `fonts` directory. Other ttf files and more information can be found at [fonts.google.com/specimen/Roboto+Mono](https://fonts.google.com/specimen/Roboto+Mono).

## Build Requirements

* qemu-system-x86_64
* ovmf
* x86_64-cross-toolchain (see below)
* mtools
* FreeType sources (set `$LIBFTDIR` appropriately)


## Cross Compiler
To install the required gcc cross compiler run the `install_cross_compiler.sh` script.
The configurable options are set by environment variables:
* `$BUILD_PATH` is the (temporary) build directory (default: `build-cross`)
* `$PREFIX` is the base directory where gcc and binutils are install (default: `/usr/local/x86_64-elf-toolchain`)
* `$GCC_VER` is the version of gcc to use (default: `10.2.0`)
* `$BINUTILS_VER` is the version of binutils to use (default: `2.35`)


## ToDo

### build system
* verify build requirements
* check header file dependency management in Makefile
* make separate debug build

### error checking/prevention
* add tests
* clean up/codify interface between bootloader and kernel
* specify dependencies between parts of kernel (e.g. init_acpi needing to be called before init_pci)
  * could likely be solved by implementing kernel modules
* check ACPI table checksums
* check kernel heap limit in kmalloc/krealloc
* possibly add static bounds checking for stacks

### memory
* fix memory leak from paging structures (if feasible, also from physical memory map)
* expand paging support
  * add memory typing using PAT/MTRRs
* improve memory manager
* create proper memory map, including:
  * system structures (MMIO, ACPI tables, ...)
  * userspace memory
  * kernel memory
* revisit ACPI table paging
  * map dynamically using alloc_virt_pages
  * is mapping the first page of ACPI tables is always sufficient to read header information?
  * possibly unmap tables, when not needed
* maybe allocate phys_mmap dynamically
* add proper handling/removal of page sizes other than 4K
* free AP bootstrap memory after use

### PCI
* add support for PCI 3.0 compatible PCIe configuration mechanism
* possibly add support for PCI-CardBus bridges
* possibly add support for deprecated <1MB PCI BAR encoding (bits[2:1]=1)

### libc
* check for optimizations of str- and mem- functions in string.h
* improve qsort performance (change pivot to median of three, use introsort)

### misc
* extend graphical capabilities
  * add support for other GOP pixel formats
  * add support for displaying bitmaps
  * add drivers for other graphics output interfaces
* improve performance of printing characters to screen (maybe improve cache / use FreeType cache)
* implement methods of timing CPU clocks using CPUID and/or MSRs (see Intel SDM Vol3 Ch10.5.4, Ch18.7.3)
  * these methods should work, but are currently not testable
  * may resolve timing issues when testing with KVM
