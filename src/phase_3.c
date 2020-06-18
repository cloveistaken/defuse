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

  size_t ptr;
  size_t begin = bomb->function[PHASE_3].paddr;
  size_t size = bomb->function[PHASE_3].size;

  /* Case 1: %d %d - Find mov eax instruction */
  for (ptr = begin; ptr < begin + size; ptr++)
    {
      if (*(unsigned char*) (bomb->original + ptr) == 0xb8)
        /* mov eax <- ... => Very compiler-dependent */
        {
          unsigned int guess = *(unsigned int*) (bomb->original + ptr + 1);
          for (int i = 0; i <= 7; i++)
            {
              snprintf(answer, ANSWER_MAX_LEN + 1, "%u %u", i, guess);

              if (try_answer(file, answer) == 0)
                {
                  FOUND(answer);
                  bomb->answer[PHASE_3] = answer;
                  goto done_phase_3;
                }
            }
        }
    }
  
  /* Case 2: %d %c %d - Find cmp ..., ... instruction */
  for (ptr = begin; ptr < begin + size; ptr++)
    {
      if (*(unsigned char*) (bomb->original + ptr) == 0x81)
        /* cmp r/m16 imm16 or cmp r/m32, imm32 */
        {
          int guess = *(short*) (bomb->original + ptr + 4); // Maybe I should consider +1 as well
          for (int i = 0; i <= 7; i++)
            for (char c = 'a'; c <= 'z'; c++)
              {
                snprintf(answer, ANSWER_MAX_LEN + 1, "%d %c %d", i, c, guess);

                if (try_answer(file, answer) == 0)
                  {
                    FOUND(answer);
                    bomb->answer[PHASE_3] = answer;
                    goto done_phase_3;
                  }
              }
        }
    }

  exit(10);

  /* Case 1: %d %d - Raw brute-force (Slow) */
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

  /* Case 2: %d %c %d - Raw brute-force (Slow) */
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
