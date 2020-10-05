#pragma once
#include <stdint.h>


#define EI_NIDENT 16

typedef struct {
	uint8_t		ident[EI_NIDENT];
	uint16_t	type;
	uint16_t	machine;
	uint32_t	version;
	uint64_t	entry;
	uint64_t	phoff;
	uint64_t	shoff;
	uint32_t	flags;
	uint16_t	ehsize;
	uint16_t	phentsize;
	uint16_t	phnum;
	uint16_t	shentsize;
	uint16_t	shnum;
	uint16_t	shstrndx;
} elf64_elf_header;


typedef struct {
	uint32_t	type;
	uint32_t	flags;
	uint64_t	offset;
	uint64_t	vaddr;
	uint64_t	paddr;
	uint64_t	filesz;
	uint64_t	memsz;
	uint64_t	align;
} elf64_program_header;

// elf64_program_header.type values
#define PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB	5
#define PT_PHDR		6
#define PT_TLS		7
#define PT_LOOS		0x60000000
#define PT_HIOS		0x6fffffff
#define PT_LOPROC	0x70000000
#define PT_HIPROC	0x7fffffff
