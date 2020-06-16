#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/phase_1.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
solve_phase_1 (Bomb* bomb)
{
  /* Create safe file for brute-forcing */
  char* file;

  file = create_tmpfile(FILE_PHASE1);
  if (file == NULL)
    ERROR("Can't create %s.", FILE_PHASE1);

  return bomb->size;
}

/*
  //char* file = "tmp/phase_1";
  char* args[] = { file, NULL };
  char* answer = "Wow! Brazil is big.";

  int fd[2];
  pipe(fd);

  pid_t pid = fork();

  if (pid == 0)
    {
      // Child
      close(fd[1]);
      dup2(fd[0], 0);
      execve(file, args, NULL);
    }
  else
    {
      // Parent
      close(fd[0]);
      write(fd[1], answer, strlen(answer) + 1);

      int status;
      wait(&status); 
      printf("%d\n", WEXITSTATUS(status));
    }
  return 0;
  */
