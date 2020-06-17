#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/phase_3.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
solve_phase_3 (Bomb* bomb)
{
  char* file;
  char* answer;

  INFO("Solving phase 3");
  /* Create safe file for brute-forcing */
  file = create_tmpfile(FILE_PHASE3);
  if (file == NULL
      || clean_copy(file, bomb, PHASE_3))
    ERROR("Error while creating patched file %s.", FILE_PHASE3);

  answer = malloc(ANSWER_MAX_LEN + 1);
  answer[ANSWER_MAX_LEN] = '\0';

  /* Generating possible answers here */

  /* Case 1: %d %d */
  for (int i = 0; i <= 7; i++)
    for (int j = 0; j <= 200; j++) /* 200 is not a good bound, at least 1000 is better */
      {
        snprintf(answer, ANSWER_MAX_LEN + 1, "%d %d", i, j);

        if (try_answer(file, answer) == 0)
          {
            FOUND(answer);
            bomb->answer[PHASE_3] = answer;
            goto done_phase_3;
          }
      }

  /* Case 2: %d %c %d */
  for (int i = 0; i <= 7; i++)
    for (int j = 0; j <= 200; j++) /* 200 is not a good bound, at least 1000 is better */
      for (char c = 'a'; c <= 'z'; c++)
        {
          snprintf(answer, ANSWER_MAX_LEN + 1, "%d %c %d", i, c, j);

          if (try_answer(file, answer) == 0)
            {
              FOUND(answer);
              bomb->answer[PHASE_3] = answer;
              goto done_phase_3;
            }
        }

done_phase_3:
  if (bomb->answer[PHASE_3] == NULL)
    ERROR("Can't find answer for phase %d.", PHASE_3);

#ifndef VERBOSE
  unlink(file);
#endif
  free(file);

  return 0;
}
