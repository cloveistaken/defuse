#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/bomb.h"
#include "../include/util.h"

int
parse_bomb (char* addr, Bomb* bomb)
{
  bomb->a = 0;

  Elf64_Ehdr hdr;
  Elf64_Shdr shdr_shstrtab;
  char* table_sh_name;

  memcpy(&hdr, addr, sizeof(hdr));

  if (hdr.e_ident[EI_MAG0] != ELFMAG0
      || hdr.e_ident[EI_MAG1] != ELFMAG1
      || hdr.e_ident[EI_MAG2] != ELFMAG2
      || hdr.e_ident[EI_MAG3] != ELFMAG3)
    ERROR("File does not appear to be in ELF format.");

  if (hdr.e_ident[EI_CLASS] != ELFCLASS64
      || hdr.e_machine != EM_X86_64)
    ERROR("Only 64-bit ELF (x86_64) is supported.");

  if (hdr.e_shstrndx == SHN_UNDEF)
    ERROR(".shstrtab not found. File appears to be stripped.");

  shdr_shstrtab = *(Elf64_Shdr*) (addr + (hdr.e_shoff + hdr.e_shstrndx * hdr.e_shentsize));
  table_sh_name = addr + shdr_shstrtab.sh_offset;

#ifdef VERBOSE
  DEBUG("Found .shstrtab at offset 0x%lx", shdr_shstrtab.sh_offset);
#endif

  for (int i = 0; i < hdr.e_shnum; i++)
    {
      Elf64_Shdr shdr;

      shdr = *(Elf64_Shdr*) (addr + (hdr.e_shoff + i * hdr.e_shentsize));

      if (shdr.sh_type == SHT_SYMTAB)
        printf("Symbol table at section #%d - %s\n", i, table_sh_name + shdr.sh_name);
    }

  return 0;
}
