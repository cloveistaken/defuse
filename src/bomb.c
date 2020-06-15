#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/phase_1.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
bootstrap (Bomb* bomb)
{
  char* tmpfile_test;

  tmpfile_test = create_tmpfile(FILE_TEST);
  if (tmpfile_test == NULL)
    ERROR("Can't create temporary file.");

  unlink(tmpfile_test);
  free(tmpfile_test);

  if (bootstrap_phase_1(bomb) == -1)
    ERROR("Fail to bootstrap phase 1");

  return 0;
}

int
clean_copy (char* filename, Bomb* bomb)
{
  char* dest;
  int fd;

  fd = open(filename, O_RDWR);
  if (fd == -1)
    return -1;

  /* Need to resize because file size != mmap size, otherwise "bus error" */
  if (ftruncate(fd, bomb->size) == -1)
    return -1;

  dest = mmap(NULL, bomb->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (dest == MAP_FAILED)
    return -1;

  memcpy(dest, bomb->original, bomb->size);

  munmap(dest, bomb->size);
  close(fd);
  return 0;
}

int
parse_bomb (char* addr, Bomb* bomb)
{
  Elf64_Ehdr hdr;
  Elf64_Shdr shdr_shstrtab, shdr_symtab;
  char* table_sh_name;
  char* table_st_name;

  char* section_name[] = {
      ".text",
      ".rodata",
      ".data",
  };

  char* function_name[] = {
      "main",
      "phase_1",
      "phase_2",
      "phase_3",
      "phase_4",
      "phase_5",
      "phase_6",
      "sig_handler",
      "explode_bomb",
  };

  char* object_name[] = {
      "bomb_id",
  };

  memcpy(&hdr, addr, sizeof(hdr));

  if (hdr.e_ident[EI_MAG0] != ELFMAG0
      || hdr.e_ident[EI_MAG1] != ELFMAG1
      || hdr.e_ident[EI_MAG2] != ELFMAG2
      || hdr.e_ident[EI_MAG3] != ELFMAG3)
    ERROR("File does not appear to be in ELF format.");

  if (hdr.e_ident[EI_CLASS] != ELFCLASS64
      || hdr.e_machine != EM_X86_64)
    ERROR("Only 64-bit ELF (x86_64) is supported.");

#ifdef VERBOSE
  DEBUG("Parsing ELF sections:"); 
#endif

  if (hdr.e_shstrndx == SHN_UNDEF)
    ERROR(".shstrtab not found. File might to be stripped.");

  shdr_shstrtab = *(Elf64_Shdr*) (addr + (hdr.e_shoff + hdr.e_shstrndx * hdr.e_shentsize));
  table_sh_name = addr + shdr_shstrtab.sh_offset;

#ifdef VERBOSE
  DEBUG("  %s  ->  0x%lx (Section %d)", ".shstrtab", shdr_shstrtab.sh_offset, hdr.e_shstrndx);
#endif

  shdr_symtab.sh_type = SHT_NULL;
  table_st_name = NULL;
  for (int i = 0; i < hdr.e_shnum; i++)
    {
      Elf64_Shdr shdr;

      shdr = *(Elf64_Shdr*) (addr + (hdr.e_shoff + i * hdr.e_shentsize));

      if (shdr.sh_type == SHT_SYMTAB
          && strcmp(table_sh_name + shdr.sh_name, ".symtab") == 0)
        {
          memcpy(&shdr_symtab, &shdr, sizeof(shdr));
#ifdef VERBOSE
          DEBUG("  %s  ->  0x%lx (Section %d)", ".symtab", shdr.sh_offset, i);
#endif
        }

      if (shdr.sh_type == SHT_STRTAB
          && strcmp(table_sh_name + shdr.sh_name, ".strtab") == 0)
        {
          table_st_name = addr + shdr.sh_offset;
#ifdef VERBOSE
          DEBUG("  %s  ->  0x%lx (Section %d)", ".strtab", shdr.sh_offset, i);
#endif
        }

      if (shdr.sh_type == SHT_PROGBITS)
        {
          for (int j = 0; j < NUM_SECTION; j++)
            {
              if (strcmp(table_sh_name + shdr.sh_name, section_name[j]) == 0)
                {
                  bomb->section[j].laddr = shdr.sh_addr;
                  bomb->section[j].paddr = shdr.sh_offset;
                  bomb->section[j].offset = shdr.sh_addr - shdr.sh_offset;
#ifdef VERBOSE
                  DEBUG("  %s  ->  0x%lx (Section %d)", section_name[j], shdr.sh_offset, i);
#endif
                }
            }
        }
    }

  if (shdr_symtab.sh_type == SHT_NULL)
    ERROR(".symtab not found. File might be stripped.");

  if (table_st_name == NULL)
    ERROR(".strtab not found. File might be stripped.");

#ifdef VERBOSE
  DEBUG("Parsing ELF symbols:");
#endif

  for (unsigned int i = 0; i < shdr_symtab.sh_size / shdr_symtab.sh_entsize; i++)
    {
      Elf64_Sym sym;

      sym = *(Elf64_Sym*) (addr + (shdr_symtab.sh_offset + i * shdr_symtab.sh_entsize));

      if (sym.st_name == STN_UNDEF)
        continue;

      if ((sym.st_info & 0x0F) == STT_FUNC)
        {
          for (int j = 0; j < NUM_FUNCTION; j++)
            {
              if (strcmp(table_st_name + sym.st_name, function_name[j]) == 0)
                {
                  bomb->function[j].laddr = 0;
                  bomb->function[j].paddr = 0;
                  bomb->function[j].size = sym.st_size;
#ifdef VERBOSE
                  DEBUG("  %s  ->  0x%lx", function_name[j], sym.st_value);
#endif
                }
            }
        }

      if ((sym.st_info & 0x0F) == STT_OBJECT)
        {
          for(int j = 0; j < NUM_OBJECT; j++)
            {
              if (strcmp(table_st_name + sym.st_name, object_name[j]) == 0)
                {
                  bomb->object[j].laddr = 0;
                  bomb->object[j].paddr = 0;
                  bomb->object[j].size = sym.st_size;
                }
            }
        }

      /*
         printf("     %s\n", table_st_name + sym.st_name);
         printf("st_info = %d\n", sym.st_info);
         printf("st_shndx = %d\n", sym.st_shndx);
         printf("st_value = %p\n", (void *)sym.st_value);
         printf("st_size = %zd\n", sym.st_size);
         */
    }

  return 0;
}
