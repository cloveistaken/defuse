#pragma once

typedef struct
{
  char* original;
  size_t size;
} Bomb;

int
bootstrap (Bomb* bomb);

int
clean_copy (char* filename, Bomb* bomb);

int
parse_bomb (char* addr, Bomb* bomb);
