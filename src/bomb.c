#include <elf.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/bomb.h"
#include "../include/util.h"

int
bootstrap (void)
{
  char* tmpfile_test;

  tmpfile_test = "dQw4w9WgXcQ";
  tmpfile_test = create_tmpfile(tmpfile_test);
  if (tmpfile_test == NULL)
    ERROR("Can't create temporary file.");
  unlink(tmpfile_test);
  free(tmpfile_test);

  return 0;
}

int
parse_bomb (char* addr, Bomb* bomb)
{
  bomb->a = 0;

  Elf64_Ehdr hdr;
  Elf64_Shdr shdr_shstrtab, shdr_symtab;
  int loaded_diff;
  char* table_sh_name;
  char* table_st_name;

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
    ERROR(".shstrtab not found. File might to be stripped.");

  shdr_shstrtab = *(Elf64_Shdr*) (addr + (hdr.e_shoff + hdr.e_shstrndx * hdr.e_shentsize));
  table_sh_name = addr + shdr_shstrtab.sh_offset;

#ifdef VERBOSE
  DEBUG("Found .shstrtab: Section %d - Offset 0x%lx", hdr.e_shstrndx, shdr_shstrtab.sh_offset);
#endif

  shdr_symtab.sh_type = SHT_NULL;
  table_st_name = NULL;
  loaded_diff = -1;
  for (int i = 0; i < hdr.e_shnum; i++)
    {
      Elf64_Shdr shdr;

      shdr = *(Elf64_Shdr*) (addr + (hdr.e_shoff + i * hdr.e_shentsize));

      if (shdr.sh_type == SHT_SYMTAB
          && strcmp(table_sh_name + shdr.sh_name, ".symtab") == 0)
        {
          memcpy(&shdr_symtab, &shdr, sizeof(shdr));
#ifdef VERBOSE
          DEBUG("Found .symtab: Section %d - Offset 0x%lx", i, shdr.sh_offset);
#endif
        }

      if (shdr.sh_type == SHT_PROGBITS
          && strcmp(table_sh_name + shdr.sh_name, ".rodata") == 0)
        {
          loaded_diff = shdr.sh_addr - shdr.sh_offset;
#ifdef VERBOSE
          DEBUG("Found .text: Section %d - Offset 0x%lx - Loaded address 0x%lx", i, shdr.sh_offset, shdr.sh_addr);
#endif
        }

      if (shdr.sh_type == SHT_STRTAB
          && strcmp(table_sh_name + shdr.sh_name, ".strtab") == 0)
        {
          table_st_name = addr + shdr.sh_offset;
#ifdef VERBOSE
          DEBUG("Found .strtab: Section %d - Offset 0x%lx", i, shdr.sh_offset);
#endif
        }
    }

  if (shdr_symtab.sh_type == SHT_NULL)
    ERROR(".symtab not found. File might be stripped.");

  if (table_st_name == NULL)
    ERROR(".strtab not found. File might be stripped.");

  if (loaded_diff == -1)
    ERROR(".text not found. File might be stripped.");

  for (unsigned int i = 0; i < shdr_symtab.sh_size / shdr_symtab.sh_entsize; i++)
    {
      Elf64_Sym sym;

      sym = *(Elf64_Sym*) (addr + (shdr_symtab.sh_offset + i * shdr_symtab.sh_entsize));

      if (sym.st_name == STN_UNDEF)
        continue;

      if (1
          && strcmp(table_st_name + sym.st_name, "explode_bomb") == 0)
        {
          printf("     %s\n", table_st_name + sym.st_name);
          printf("st_info = %d\n", sym.st_info);
          printf("st_shndx = %d\n", sym.st_shndx);
          printf("st_value = %p\n", (void *)sym.st_value);
          printf("st_size = %zd\n", sym.st_size);
        }
    }

  return 0;
}
