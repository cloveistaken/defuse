#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/phase_5.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
solve_phase_5 (Bomb* bomb)
{
  char* file;
  char* answer;

  INFO("Solving phase 5");
  /* Create safe file for brute-forcing */
  file = create_tmpfile(FILE_PHASE5);
  if (file == NULL
      || clean_copy(file, bomb, PHASE_5))
    ERROR("Error while creating patched file %s.", FILE_PHASE5);

  answer = malloc(ANSWER_MAX_LEN + 1);
  answer[ANSWER_MAX_LEN] = '\0';

  /* Generating possible answers here */
  
  /* Case 1: %d %d (often 5 115) */
  for (int i = 0; i <= 15; i++)
    {
      snprintf(answer, ANSWER_MAX_LEN + 1, "%d %d", i, 120 - i);

      if (try_answer(file, answer) == 0)
        {
          FOUND(answer);
          bomb->answer[PHASE_5] = answer;
          goto done_phase_5;
        }
    }

  /* Case 2: string(6)
     Quite heavy to brute force - Worst case ~ 16^6 (or 26^6)

     Better way but novelty:
     Modify strings_not_equal (static / dynamic with LD_PRELOAD constructor)
     to emit the arguments. Then input the whole alphabet to get decryption table.
     */

  for (char c1 = 'a'; c1 <= 'q'; c1++)
    for (char c2 = 'a'; c2 <= 'q'; c2++)
      for (char c3 = 'a'; c3 <= 'q'; c3++)
        for (char c4 = 'a'; c4 <= 'q'; c4++)
          for (char c5 = 'a'; c5 <= 'q'; c5++)
            for (char c6 = 'a'; c6 <= 'q'; c6++)
              {
                snprintf(answer, ANSWER_MAX_LEN + 1, "%c%c%c%c%c%c",
                         c1, c2, c3, c4, c5, c6);

                if (try_answer(file, answer) == 0)
                  {
                    FOUND(answer);
                    bomb->answer[PHASE_5] = answer;
                    goto done_phase_5;
                  }
              }
  
done_phase_5:
  if (bomb->answer[PHASE_5] == NULL)
    ERROR("Can't find answer for phase %d.", PHASE_5);

#ifndef VERBOSE
  unlink(file);
#endif
  free(file);

  return 0;
}
