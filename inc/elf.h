#ifndef __ELF_H
#define __ELF_H

#include <type.h>

// ?
#define ELF_MAGIC   0x464c457fu
/* elf 32 header */
struct elf32hdr {
  uint32_t magic;   // equal ELF_MAGIC
  char elf[12];
  uint16_t type;
  uint16_t machine;
  uint32_t version;
  uint32_t entry;   // program entry
  uint32_t phoff;   // offset of program header
  uint32_t shoff;
  uint32_t flags;
  uint16_t ehsize;
  uint16_t phentsize;
  uint16_t phnum;   // number of program header
  uint16_t shentsize;
  uint16_t shnum;
  uint16_t shstrndx;
};

/* proghdr type */
#define ELF_PROG_LOAD   1

/* proghdr flag*/
#define ELF_PROG_EXEC   0x1
#define ELF_PROG_WRITE  0x2
#define ELF_PROG_READ   0x4

struct proghdr{
    uint32_t type;
    uint32_t off;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flag;
    uint32_t align;
}

#endif
