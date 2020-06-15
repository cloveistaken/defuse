#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/util.h"

void
print_banner (void)
{
  char* banner =
"                                              \n"
"                                              \n"
"         /\\         | |                      \n"
"        /  \\   ___ _| |_  _   _  ____ ___    \n"
"       / /\\ \\ / __)     \\| | | |/  ._) __) \n"
"      / /__\\ \\> _| (| |) ) |_| ( () )> _)   \n"
"     /________\\___)_   _/ \\___/ \\__/ \\___)\n"
"                    | |                       \n"
"                    |_|                       \n"
"                                              \n"
"                                              \n";

  printf("%s\n", banner);
}

char*
create_tmpfile (char* filename)
{
  char* tmpdir;
  char* tmpfile;
  int fd;

  tmpdir = getenv("TMPDIR");
  if (tmpdir == NULL)
#ifdef VERBOSE
    tmpdir = "tmp";
#else
    tmpdir = "/tmp";
#endif

  fd = open(tmpdir, O_DIRECTORY);
  if (fd == -1)
    return NULL;
  close(fd);

  tmpfile = malloc(100);
  snprintf(tmpfile, 100, "%s/%s", tmpdir, filename);

  /* No need to check exit status */
  unlink(tmpfile);

  fd = open(tmpfile, O_CREAT | O_WRONLY, 0700);
  if (fd == -1)
    return NULL;
  close(fd);

  return tmpfile;
}
