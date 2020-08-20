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
* set up kernel stack
* set up IDT
* make graphical output capabilities available to kernel
* create memory manager(s)
* expand paging support
  * add memory typing using PAT/MTRRs
  * add code to edit page structures
* check header file dependency management in Makefile
