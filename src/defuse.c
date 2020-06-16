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
#include "../include/phase_2.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
main (int argc, char* argv[])
{
  char* addr;
  int fd;
  struct stat sb;
  Bomb bomb;

  print_banner();
  if (argc != 2)
    {
      printf("Usage: %s <bomb>\n", argv[0]);
      return 1;
    }
  
  fd = open(argv[1], O_RDONLY);
  if (fd == -1)
    FATAL("Error opening file \"%s\"", argv[1]);

  if (fstat(fd, &sb) == -1)
    FATAL("Error reading stat of \"%s\"", argv[1]);

#ifdef VERBOSE
  DEBUG("File size: %ld bytes", sb.st_size);
#endif

  addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED)
    FATAL("Error loading \"%s\" into memory", argv[1]);

  bomb.original = addr;
  bomb.size = sb.st_size;

  if (parse_bomb(addr, &bomb) == -1)
    FATAL("Error parsing \"%s\". The binary might be incorrect or corrupted.", argv[1]);

  if (bootstrap(&bomb) == -1)
    FATAL("Error while doing bootstrap.");

  munmap(addr, sb.st_size);
  close(fd);
  return 0;
}
