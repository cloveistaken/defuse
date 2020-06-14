#include <stdio.h>

#include "../include/util.h"

void
print_banner (void)
{
  char* banner =
"                                              \n"
"         /\\         | |                      \n"
"        /  \\   ___ _| |_  _   _  ____ ___    \n"
"       / /\\ \\ / __)     \\| | | |/  ._) __) \n"
"      / /__\\ \\> _| (| |) ) |_| ( () )> _)   \n"
"     /________\\___)_   _/ \\___/ \\__/ \\___)\n"
"                    | |                       \n"
"                    |_|                       \n"
"                                              \n";

  printf("%s\n", banner);
}
