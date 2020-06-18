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
  size_t ptr;
  size_t begin = bomb->function[PHASE_1].paddr;
  size_t size = bomb->function[PHASE_1].size;

  for (ptr = begin; ptr < begin + size; ptr++)
    {
      if (*(unsigned char*) (bomb->original + ptr) == 0xbe)
        /* mov esi <- ... - Should work on 64-bits */
        {
          ptr += 1;
          /* ptr now should point to the string
             that is passed to strings_not_equal */
          break;
        }
        
    }

  /* I might want to add a check (ptr == begin + size)
     However now it's not needed,
     which means no edge cases
  */

  ptr = *(unsigned int*) (bomb->original + ptr) - bomb->section[RODATA].offset;
  strncpy(answer, bomb->original + ptr, ANSWER_MAX_LEN);
  
  if (try_answer(file, answer) == 0)
    {
      FOUND(answer);
      bomb->answer[PHASE_1] = answer;
      goto done_phase_1;
    }

done_phase_1:
  if (bomb->answer[PHASE_1] == NULL)
    ERROR("Can't find answer for phase %d.", PHASE_1);

#ifndef VERBOSE
  unlink(file);
#endif
  free(file);

  return 0;
}
