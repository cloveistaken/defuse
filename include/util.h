#pragma once

#include <elf.h>

#define RED(str) "\033[0;31m" str "\033[0m" " "
#define GREEN(str) "\033[0;32m" str "\033[0m" " "
#define YELLOW(str) "\033[0;33m" str "\033[0m" " "
#define BLUE(str) "\033[0;34m" str "\033[0m" " "

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

#define VERBOSE
