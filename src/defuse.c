#include <elf.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/bomb.h"
#include "../include/util.h"

int
main (int argc, char* argv[])
{
  char* addr;
  int fd;
  struct stat sb;
  size_t length;

  print_banner();
  if (argc != 2)
    {
      printf("Usage: %s <bomb>\n", argv[0]);
      return 1;
    }
  
  fd = open(argv[1], O_RDONLY);
  if (fd == -1)
    {
      perror("open");
      return 1;
    }

  if (fstat(fd, &sb) == -1)
    {
      perror("fstat");
      return 1;
    }

  length = sb.st_size;
  addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED)
    {
      perror("mmap");
      return 1;
    }

  ///////

  Elf64_Ehdr elf_header;

  memcpy(&elf_header, addr, sizeof(elf_header));
  if (check_sanity_header(&elf_header) == -1)
    {
      printf("%s\n", error_message);
      return 1;
    }

  // shstrtab for names of sections
  Elf64_Shdr shstrtab;
  if (elf_header.e_shstrndx == SHN_UNDEF) // Stripped shstrtab
    return 1;

  size_t off = elf_header.e_shoff + elf_header.e_shstrndx * elf_header.e_shentsize;
  memcpy(&shstrtab, addr + off, sizeof(shstrtab));

  char* table = addr + shstrtab.sh_offset;

  for (int i = 0; i < elf_header.e_shnum; i++)
    {
      Elf64_Shdr section_header;
      size_t offset = elf_header.e_shoff + i * elf_header.e_shentsize;
      memcpy(&section_header, addr + offset, sizeof(section_header));
    
      if (section_header.sh_type == SHT_SYMTAB)
        {
          printf("Symbol table at section #%d - sh_name = %d\n", i, section_header.sh_name);
          printf("Name;: %s\n", table + section_header.sh_name);
        }

      if (section_header.sh_type == SHT_STRTAB)
        {
          printf("String table at section #%d - sh_name = %d\n", i, section_header.sh_name);
          printf("Name;: %s\n", table + section_header.sh_name);
        }

    }

  return 0;
}
