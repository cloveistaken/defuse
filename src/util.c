#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  int fd, length;

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

  length = strlen(tmpdir) + strlen(filename) + 2;
  tmpfile = malloc(length);
  snprintf(tmpfile, length, "%s/%s", tmpdir, filename);

  /* No need to check exit status */
  unlink(tmpfile);

  fd = open(tmpfile, O_CREAT | O_WRONLY, 0700);
  if (fd == -1)
    return NULL;
  close(fd);

  return tmpfile;
}

int
try_answer (char* filename, char* answer)
{
  int fd[2];
  pid_t pid;

  /* Create an anonymous pipe:
     Parent -> fd[1] -> (fd[0] == STDIN) -> child */
  if (pipe(fd) == -1)
    ERROR("Problem while creating anonymous pipe.");

  pid = fork();   
  if (pid == -1)
    ERROR("Problem while forking.");

  if (pid == 0)
    {
      /* Child */
      char* argv[] = { filename, NULL };

      close(fd[1]); 
      dup2(fd[0], 0);
      execve(filename, argv, NULL);
    }

  if (pid > 0)
    {
      /* Parent */
      int status;
      close(fd[0]);

      /* Need to write the last '\0' byte */
      write(fd[1], answer, strlen(answer) + 1);

      /* Not sure if this would hang or not ? */
      waitpid(pid, &status, 0);

      close(fd[1]);
      return WEXITSTATUS(status);
    }

  return 0;
}
