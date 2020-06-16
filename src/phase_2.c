#include <stdio.h>

#include "../include/phase_2.h"
#include "../include/bomb.h"
#include "../include/util.h"

int
bootstrap_phase_2 (Bomb* bomb)
{
  char* tmpfile;

  tmpfile = create_tmpfile(FILE_PHASE_2);
  if (tmpfile == NULL)
    return -1;

  if (clean_copy(tmpfile, bomb, PHASE_2) == -1)
    return -1;

  return 0;
}

void
solve_phase_2 (void)
{
}
