#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/phase_4.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
solve_phase_4 (Bomb* bomb)
{
  char* file;
  char* answer;

  INFO("Solving phase 4");
  /* Create safe file for brute-forcing */
  file = create_tmpfile(FILE_PHASE4);
  if (file == NULL
      || clean_copy(file, bomb, PHASE_4))
    ERROR("Error while creating patched file %s.", FILE_PHASE4);

  answer = malloc(ANSWER_MAX_LEN + 4);
  answer[ANSWER_MAX_LEN] = '\0';

  /* Generating possible answers here */
  char* trial = "Wow! Brazil is big.";
  strncpy(answer, trial, ANSWER_MAX_LEN);
  
  if (try_answer(file, answer) == 0)
    {
      FOUND(answer);
      bomb->answer[PHASE_4] = answer;
    }

  if (bomb->answer[PHASE_4] == NULL)
    ERROR("Can't find answer for phase %d.", PHASE_4);

#ifndef VERBOSE
  unlink(file);
#endif
  free(file);

  return 0;
}
