#include "../include/bomb.h"

int
parse_bomb (char* addr, Bomb* bomb)
{
  bomb->a = *((int *) addr);

  return -1;
}
