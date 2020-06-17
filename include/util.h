#pragma once

#define RED(str)        "\033[0;31m" str "\033[0m" " "
#define GREEN(str)      "\033[0;32m" str "\033[0m" " "
#define YELLOW(str)     "\033[0;33m" str "\033[0m" " "
#define BLUE(str)       "\033[0;34m" str "\033[0m" " "

#define FILE_TEST       "dQw4w9WgXcQ"
#define ANSWER_MAX_LEN  79

#define FORMAT_DEBUG_SECTION    "   %-15s  ->  0x%-6lx / 0x%-4lx (Section %d)"
#define FORMAT_DEBUG_SYMBOL     "   %-15s  ->  0x%-6lx / 0x%-4lx / %ld"
#define FORMAT_ANSWER_FOUND     "   Phase %d: \"%s\""

#define FOUND(answer) do { \
    printf("        => %s\n", answer); \
} while (0)

#define INFO(...) do { \
    printf(GREEN("[INFO] ")); \
    printf(__VA_ARGS__); \
    for (int l = 0; l < 3; l++) \
      { printf("."); fflush(stdout); usleep(100000); } \
    printf("\n"); \
} while (0)

#define DEBUG(...) do { \
    printf(BLUE("[DEBUG]")); \
    printf(__VA_ARGS__); \
    printf("\n"); \
} while (0)

#define ERROR(...) do { \
    fprintf(stderr, YELLOW("[ERROR]")); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    return (-1); \
} while (0)

#define FATAL(...) do { \
    fprintf(stderr, RED("[FATAL]")); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
    exit (1); \
} while (0)

void
print_banner (void);

char*
create_tmpfile (char* filename);

int
try_answer (char* filename, char* answer);
