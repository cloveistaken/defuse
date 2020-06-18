#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/bomb.h"
#include "../include/util.h"

int
clean_copy (char* filename, Bomb* bomb, int phase)
{
  char* dest;
  int fd;
  char* patched_main;
  size_t size_main;
  int offset_to_phase;
  char* patched_explode;
  size_t size_explode;

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

  /* Patching main */
  size_main = bomb->function[MAIN].size;
  if (size_main < strlen(SHELLCODE_MAIN))
    ERROR("\"main\" is too small for the shellcode (size = %ld).", size_main);
  
  patched_main = malloc(size_main);
  if (patched_main == NULL)
    return -1;

  memset(patched_main, NOP, size_main);
  memcpy(patched_main, SHELLCODE_MAIN, strlen(SHELLCODE_MAIN));

    /* Maybe there is a better way than hardcoded index */
  memcpy(patched_main + 8, &(bomb->object[INPUT_STRINGS].laddr), 4);
  memcpy(patched_main + 22, &(bomb->object[INPUT_STRINGS].laddr), 4);

    /* phase should be PHASE_X (from 1 to 6) */
  offset_to_phase = bomb->function[phase].laddr - (bomb->function[MAIN].laddr + 34);
  memcpy(patched_main + 30, &offset_to_phase, 4);

  memcpy(dest + bomb->function[MAIN].paddr, patched_main, size_main);
  free(patched_main);
  /* Finish */


  /* Patching explode_bomb */
  size_explode = bomb->function[EXPLODE_BOMB].size;
  if (size_explode < strlen(SHELLCODE_EXPLODE))
    ERROR("\"explode_bomb\" is too small for the shellcode (size = %ld).", size_explode);

  patched_explode = malloc(size_explode);
  if (patched_explode == NULL)
    return -1;

  memset(patched_explode, NOP, size_explode);
  memcpy(patched_explode, SHELLCODE_EXPLODE, strlen(SHELLCODE_EXPLODE));

  memcpy(dest + bomb->function[EXPLODE_BOMB].paddr, patched_explode, size_explode);
  free(patched_explode);
  /* Finish */

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
      ".bss",
  };

  char* function_name[] = {
      "main",
      "phase_1",
      "phase_2",
      "phase_3",
      "phase_4",
      "phase_5",
      "phase_6",
      "phase_defused",
      "explode_bomb",
  };

  char* object_name[] = {
      "input_strings",
      "array",
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

  INFO("Parsing ELF sections"); 

  if (hdr.e_shstrndx == SHN_UNDEF)
    ERROR(".shstrtab not found. File might to be stripped.");

  shdr_shstrtab = *(Elf64_Shdr*) (addr + (hdr.e_shoff + hdr.e_shstrndx * hdr.e_shentsize));
  table_sh_name = addr + shdr_shstrtab.sh_offset;

  shdr_symtab.sh_type = SHT_NULL;
  table_st_name = NULL;
  for (int i = 0; i < hdr.e_shnum; i++)
    {
      Elf64_Shdr shdr;

      shdr = *(Elf64_Shdr*) (addr + (hdr.e_shoff + i * hdr.e_shentsize));

      if (shdr.sh_type == SHT_SYMTAB
          && strcmp(table_sh_name + shdr.sh_name, ".symtab") == 0)
        memcpy(&shdr_symtab, &shdr, sizeof(shdr));

      if (shdr.sh_type == SHT_STRTAB
          && strcmp(table_sh_name + shdr.sh_name, ".strtab") == 0)
        table_st_name = addr + shdr.sh_offset;

      for (int j = 0; j < NUM_SECTION; j++)
        {
          if (strcmp(table_sh_name + shdr.sh_name, section_name[j]) == 0)
            {
              bomb->section[j].ndx = i;
              bomb->section[j].laddr = shdr.sh_addr;
              bomb->section[j].paddr = shdr.sh_offset;
              bomb->section[j].offset = shdr.sh_addr - shdr.sh_offset;
            }
        }
    }

  if (shdr_symtab.sh_type == SHT_NULL)
    ERROR(".symtab not found. File might be stripped.");

  if (table_st_name == NULL)
    ERROR(".strtab not found. File might be stripped.");

#ifdef VERBOSE
  DEBUG("Sections:");
  for (int i = 0; i < NUM_SECTION; i++)
    DEBUG(FORMAT_DEBUG_SECTION, section_name[i],
          bomb->section[i].laddr, bomb->section[i].paddr, bomb->section[i].ndx);
#endif
  
  // TODO: Add a check here

  INFO("Parsing ELF symbols");

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
                  bomb->function[j].laddr = sym.st_value;
                  bomb->function[j].paddr = sym.st_value - bomb->section[TEXT].offset;
                  bomb->function[j].size = sym.st_size;
                }
            }
        }

      if ((sym.st_info & 0x0F) == STT_OBJECT)
        {
          for (int j = 0; j < NUM_OBJECT; j++)
            {
              /* This is for phase 5 */
              if (strncmp(table_st_name + sym.st_name, object_name[j], strlen(object_name[j])) == 0)
                {
                  bomb->object[j].laddr = sym.st_value;

                  /* Every object should fall into .rodata, .data or .bss
                   * (Not true in general, but here it is)
                   */
                  for (int k = 0; k < NUM_SECTION; k++)
                    if (sym.st_shndx == bomb->section[k].ndx)
                      bomb->object[j].paddr = sym.st_value - bomb->section[k].offset;
                  
                  bomb->object[j].size = sym.st_size;
                }
            }
        }
    }

#ifdef VERBOSE
  DEBUG("Functions:");
  for (int i = 0; i < NUM_FUNCTION; i++)
    DEBUG(FORMAT_DEBUG_SYMBOL, function_name[i],
          bomb->function[i].laddr, bomb->function[i].paddr, bomb->function[i].size);

  DEBUG("Objects:");
  for (int i = 0; i < NUM_OBJECT; i++)
    DEBUG(FORMAT_DEBUG_SYMBOL, object_name[i],
          bomb->object[i].laddr, bomb->object[i].paddr, bomb->object[i].size);
#endif

  return 0;
}
