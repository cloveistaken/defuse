#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/phase_1.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
solve_phase_1 (Bomb* bomb)
{
  char* file;
  char* answer;

  INFO("Solving phase 1");
  /* Create safe file for brute-forcing */
  file = create_tmpfile(FILE_PHASE1);
  if (file == NULL
      || clean_copy(file, bomb, PHASE_1))
    ERROR("Error while creating patched file %s.", FILE_PHASE1);

  answer = malloc(ANSWER_MAX_LEN + 1);
  answer[ANSWER_MAX_LEN] = '\0';

  /* Generating possible answers here */
  char* trial = "Wow! Brazil is big.!";
  strncpy(answer, trial, ANSWER_MAX_LEN);
  
  if (try_answer(file, answer) == 0)
    {
      INFO(FORMAT_ANSWER_FOUND, PHASE_1, answer);
      bomb->answer[PHASE_1] = answer;
    }

  if (bomb->answer[PHASE_1] == NULL)
    ERROR("Can't find answer for phase %d", PHASE_1);

#ifndef VERBOSE
  unlink(file);
#endif
  free(file);

  return 0;
}
