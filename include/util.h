#pragma once

#include <elf.h>

extern char* error_message;

void print_banner(void);
int check_sanity_header (Elf64_Ehdr* elf_header);
