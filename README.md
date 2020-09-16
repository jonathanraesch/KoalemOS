# KoalemOS #

KoalemOS is a hobby project to explore OS development.

## Build Requirements

* bochs
* bochs-sdl
* qemu-system-x86_64
* ovmf
* x86_64-cross-toolchain
* mtools
* grub-pc-bin
* xorriso

## ToDo

* verify build requirements
* set up dynamic kernel stack
* do bounds checking on initial static kernel stack
* set up IDT
* make graphical output capabilities available to kernel
* fix memory leak from paging structures (if feasible, also from physical memory map)
* expand paging support
  * add memory typing using PAT/MTRRs
* check header file dependency management in Makefile
* clean up/codify interface between bootloader and kernel
* add tests
* extend debugging support
* improve memory manager
