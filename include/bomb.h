#pragma once

#define NUM_PHASE         6

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
#define NUM_FUNCTION      9
#define MAIN              0
#define PHASE_1           1
#define PHASE_2           2
#define PHASE_3           3
#define PHASE_4           4
#define PHASE_5           5
#define PHASE_6           6
#define PHASE_DEFUSED     7
#define EXPLODE_BOMB      8

#define NUM_OBJECT        2
#define INPUT_STRINGS     0
#define ARRAY             1

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
  char*   answer[NUM_PHASE + 1];  /* List of answers (answer[0] is always NULL */
} Bomb;

#define NOP                 '\x90'

#define SHELLCODE_MAIN      "\x53" \
                            "\x31\xc0" \
                            "\xb0\x03" \
                            "\x31\xdb" \
                            "\xb9\xff\xff\xff\xff" \
                            "\x31\xd2" \
                            "\xb2\x64" \
                            "\xcd\x80" \
                            "\x48\x31\xff" \
                            "\xbf\xff\xff\xff\xff" \
                            "\x48\x89\xfe" \
                            "\xe8\xff\xff\xff\xff" \
                            "\x31\xc0" \
                            "\xff\xc0" \
                            "\x31\xdb" \
                            "\xcd\x80"

#define SHELLCODE_EXPLODE   "\x31\xc0" \
                            "\xff\xc0" \
                            "\x31\xdb" \
                            "\xb3\x2a" \
                            "\xcd\x80"

int
clean_copy (char* filename, Bomb* bomb, int phase);

int
parse_bomb (char* addr, Bomb* bomb);
