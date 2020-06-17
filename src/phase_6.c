#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/phase_6.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
solve_phase_6 (Bomb* bomb)
{
  char* file;
  char* answer;

  INFO("Solving phase 6");
  /* Create safe file for brute-forcing */
  file = create_tmpfile(FILE_PHASE6);
  if (file == NULL
      || clean_copy(file, bomb, PHASE_6))
    ERROR("Error while creating patched file %s.", FILE_PHASE6);

  answer = malloc(ANSWER_MAX_LEN + 1);
  answer[ANSWER_MAX_LEN] = '\0';

  /* Generating possible answers here */
  int i1, i2, i3, i4, i5, i6;
  for (i1 = 1; i1 <= 6; i1++)
    for (i2 = 1; i2 <= 6; i2++)
      {
        if (i2 == i1)
          continue;

        for (i3 = 1; i3 <= 6; i3++)
          {
            if (i3 == i1 || i3 == i2)
              continue;

            for (i4 = 1; i4 <= 6; i4++)
              {
                if (i4 == i1 || i4 == i2 || i4 == i3)
                  continue;

                for (i5 = 1; i5 <= 6; i5++)
                  {
                    if (i5 == i1 || i5 == i2 || i5 == i3 || i5 == i4)
                      continue;

                    for (i6 = 1; i6 <= 6; i6++)
                      {
                        if (i6  == i1 || i6 == i2 || i6 == i3 || i6 == i4 || i6 == i5)
                          continue;

                        snprintf(answer, ANSWER_MAX_LEN + 1, "%d %d %d %d %d %d",
                                 i1, i2, i3, i4, i5, i6);

                        if (try_answer(file, answer) == 0)
                          {
                            FOUND(answer);
                            bomb->answer[PHASE_6] = answer;
                            goto done_phase_6;
                          }
                      }
                  }
              }
          }
      }

done_phase_6:
  if (bomb->answer[PHASE_6] == NULL)
    ERROR("Can't find answer for phase %d.", PHASE_6);

#ifndef VERBOSE
  unlink(file);
#endif
  free(file);

  return 0;
}
