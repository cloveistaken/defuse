#pragma once

/* Section */
#define NUM_SECTION       4
#define TEXT              0
#define RODATA            1
#define DATA              2
#define BSS               3

typedef struct
{
  int     ndx;            /* Absolute section number */
  size_t  laddr;          /* Loaded address */
  size_t  paddr;          /* Physical address */
  size_t  offset;         /* Offset between loaded and physical addresses */
} Section;

/* Symbol */
#define NUM_FUNCTION      10
#define MAIN              0
#define PHASE_1           1
#define PHASE_2           2
#define PHASE_3           3
#define PHASE_4           4
#define PHASE_5           5
#define PHASE_6           6
#define PHASE_DEFUSED     7
#define INITIALIZE_BOMB   8
#define EXPLODE_BOMB      9

#define NUM_OBJECT        2
#define BOMB_ID           0
#define INPUT_STRINGS     1

typedef struct
{
  size_t  laddr;          /* Loaded address */
  size_t  paddr;          /* Physical address */
  size_t  size;           /* Size of the symbol */
} Symbol;

/* Bomb */
typedef struct
{
  char*   original;               /* Pointer to mmap() original file */
  size_t  size;                   /* Size of the binary */
  Section section[NUM_SECTION];   /* List of important sections */
  Symbol  function[NUM_FUNCTION]; /* List of important functions */
  Symbol  object[NUM_OBJECT];     /* List of important objects */
} Bomb;

int
bootstrap (Bomb* bomb);

int
clean_copy (char* filename, Bomb* bomb);

int
parse_bomb (char* addr, Bomb* bomb);
