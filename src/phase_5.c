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

  size_t ptr;
  size_t begin = bomb->function[PHASE_5].paddr;
  size_t size = bomb->function[PHASE_5].size;

  /* Case 2: string(6) -> int array[64] */
  if (bomb->object[ARRAY].size == 64)
    {
      char rev[17];
      for (int i = 0; i < 64; i += 4)
        {
          int value = *(int*) (bomb->original + bomb->object[ARRAY].paddr + i);
          if (i)
            rev[value] = (int) 'a' + i / 4 - 1;
          else
            rev[value] = 'p';
#ifdef VERBOSE
          DEBUG("   i = %d (= '%c') => value = %d", i, rev[value], value);
#endif
        }

      for (ptr = begin; ptr < begin + size; ptr++)
        {
          if (*(unsigned char*) (bomb->original + ptr) == 0x83)
            /* cmp r/m16 imm8 or cmp r/m32, imm8 */
            {
              int sum = *(unsigned char*) (bomb->original + ptr + 2);
              if (sum < 6)
                continue;

              int av = sum / 6;
              snprintf(answer, ANSWER_MAX_LEN + 1, "%c%c%c%c%c%c",
                       rev[av], rev[av], rev[av], rev[av], rev[av], rev[sum - 5*av]);

              if (try_answer(file, answer) == 0)
                {
                  FOUND(answer);
                  bomb->answer[PHASE_5] = answer;
                  goto done_phase_5;
                }
            }
        }
    }


  /* Case 3: string(6) -> char array[16] */
  if (bomb->object[ARRAY].size == 16)
    {
      char rev[26];
      for (int i = 0; i < 16; i++)
        {
          char c = *(bomb->original + bomb->object[ARRAY].paddr + i);
          if (i)
            rev[c - 'a'] = (int) 'a' + i - 1;
          else
            rev[c - 'a'] = 'p';
#ifdef VERBOSE
          DEBUG("   i = %d (= '%c') => c = %c", i, rev[c - 'a'], c);
#endif
        }

      for (ptr = begin; ptr < begin + size; ptr++)
        {
          if (*(unsigned char*) (bomb->original + ptr) == 0xbe)
            /* mov esi - Similar to phase 1*/
            {
              ptr = *(unsigned int*) (bomb->original + ptr + 1) - bomb->section[RODATA].offset;
              char t[6];
              memcpy(t, bomb->original + ptr, 6);

              snprintf(answer, ANSWER_MAX_LEN + 1, "%c%c%c%c%c%c",
                       rev[t[0] - 'a'], rev[t[1] - 'a'], rev[t[2] - 'a'],
                       rev[t[3] - 'a'], rev[t[4] - 'a'], rev[t[5] - 'a']);

              if (try_answer(file, answer) == 0)
                {
                  FOUND(answer);
                  bomb->answer[PHASE_5] = answer;
                  goto done_phase_5;
                }
            }
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
