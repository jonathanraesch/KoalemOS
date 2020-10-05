# KoalemOS #

KoalemOS is a hobby project to explore OS development.


## Build Requirements

* qemu-system-x86_64
* ovmf
* x86_64-cross-toolchain (see below)
* nasm
* mtools


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
* add debugging for assembly code
* make separate debug build

### error checking/prevention
* add tests
* do bounds checking on initial static kernel stack
* clean up/codify interface between bootloader and kernel
* specify dependencies between parts of kernel (e.g. init_acpi needing to be called before init_pci)
  * could likely be solved by implementing kernel modules
* check ACPI table checksums

### memory
* set up dynamic kernel stack
* fix memory leak from paging structures (if feasible, also from physical memory map)
* expand paging support
  * add memory typing using PAT/MTRRs
* improve memory manager
* create proper memory map, including:
  * system structures (MMIO, ACPI tables, ...)
  * userspace memory
  * kernel memory
* revisit ACPI table paging
  * is mapping the first page of ACPI tables is always sufficient to read header information?
  * possibly unmap tables, when not needed
* fix phys_mmap space restrictions

### misc
* extend graphical capabilities
  * add support for other GOP pixel formats
  * add support for displaying text/bitmaps
  * add drivers for other graphics output interfaces
* implement PCI 3.0 compatible PCIe configuration mechanism
