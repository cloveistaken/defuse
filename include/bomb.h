#pragma once

typedef struct
{
  int a;
} Bomb;

int
parse_bomb (char* addr, Bomb* bomb);
