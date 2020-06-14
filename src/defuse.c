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

  Elf64_Ehdr elf_header;
  memcpy(&elf_header, addr, sizeof(elf_header));

  if (check_sanity_header(&elf_header) == -1)
    {
      printf("%s\n", error_message);
      return 1;
    }



  return 0;
}
