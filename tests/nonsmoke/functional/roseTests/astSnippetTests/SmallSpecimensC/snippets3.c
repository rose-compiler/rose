/* Snippets with #include directives */

#include <stdio.h>

int copy_me_too()
{
    fputs("stupid example", stderr);
    return 0;
}


void print(const char *v1)
{
    fputs(v1, stderr);
}
