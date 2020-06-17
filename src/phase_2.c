#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/phase_2.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
solve_phase_2 (Bomb* bomb)
{
  char* file;
  char* answer;

  INFO("Solving phase 2");
  /* Create safe file for brute-forcing */
  file = create_tmpfile(FILE_PHASE2);
  if (file == NULL
      || clean_copy(file, bomb, PHASE_2))
    ERROR("Error while creating patched file %s.", FILE_PHASE2);

  answer = malloc(ANSWER_MAX_LEN + 1);
  answer[ANSWER_MAX_LEN] = '\0';

  /* Generating possible answers here */
  char* trials[] = { "0 1 1 2 3 5", "0 1 3 6 10 15", "1 2 4 8 16 32"};
  for (int i = 0; i < 3; i++)
    {
      char* trial = trials[i];
      strncpy(answer, trial, ANSWER_MAX_LEN);

      if (try_answer(file, answer) == 0)
        {
          FOUND(answer);
          bomb->answer[PHASE_2] = answer;
        }
    }

  if (bomb->answer[PHASE_2] == NULL)
    ERROR("Can't find answer for phase %d.", PHASE_2);

#ifndef VERBOSE
  unlink(file);
#endif
  free(file);

  return 0;

  return bomb->size;
}
