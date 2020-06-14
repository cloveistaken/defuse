#include <elf.h>
#include <stdio.h>

#include "../include/util.h"

char* error_message = "";

void print_banner(void)
{
  printf("Placeholder for a banner\n");
}

int check_sanity_header (Elf64_Ehdr* elf_header)
{
  Elf64_Ehdr header;
  
  header = *elf_header;

  if (header.e_ident[EI_MAG0] != ELFMAG0)
    {
      error_message = "Not an ELF binary";
      return -1;
    }

  if (header.e_ident[EI_MAG1] != ELFMAG1)
    {
      error_message = "Not an ELF binary";
      return -1;
    }

  if (header.e_ident[EI_MAG2] != ELFMAG2)
    {
      error_message = "Not an ELF binary";
      return -1;
    }

  if (header.e_ident[EI_MAG3] != ELFMAG3)
    {
      error_message = "Not an ELF binary";
      return -1;
    }

  if (header.e_ident[EI_CLASS] != ELFCLASS64)
    {
      error_message = "Only 64-bit ELF is supported";
      return -1;
    }

  if (header.e_machine != EM_X86_64)
    {
      error_message = "Required x86_64 architecture";
      return -1;
    }

  return 0;
}

