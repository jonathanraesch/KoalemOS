# KoalemOS #

KoalemOS is a hobby project to explore OS development.

## Build Requirements

* qemu-system-x86_64
* ovmf
* x86_64-cross-toolchain
* mtools

## ToDo

* verify build requirements
* set up dynamic kernel stack
* do bounds checking on initial static kernel stack
* extend graphical capabilities
  * add support for other GOP pixel formats
  * add support for displaying text/bitmaps
  * add drivers for other graphics output interfaces
* fix memory leak from paging structures (if feasible, also from physical memory map)
* expand paging support
  * add memory typing using PAT/MTRRs
* check header file dependency management in Makefile
* clean up/codify interface between bootloader and kernel
* load kernel as ELF file
  * fix phys_mmap space restrictions
* add tests
* extend debugging support
* improve memory manager
* make separate debug build
* add debugging for assembly code
* check ACPI table checksums
* revisit ACPI table paging
  * is mapping the first page of ACPI tables is always sufficient to read header information?
  * possibly unmap tables, when not needed
* create proper memory map, including:
  * system structures (MMIO, ACPI tables, ...)
  * userspace memory
  * kernel memory
* specify dependencies between parts of kernel (e.g. init_acpi needing to be called before init_pci)
  * could likely be solved by implementing kernel modules
* implement PCI 3.0 compatible PCIe configuration mechanism
