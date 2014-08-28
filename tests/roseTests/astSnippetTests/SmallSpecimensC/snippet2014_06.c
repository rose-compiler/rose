/* Snippets with #include directives */

// #include <stdio.h>                                      // fputs
// This function declaration must be present in order to insert a function call that would reference this function. 
// void fputs(v1, stderr);
typedef int FILE;
extern int fputs(const char *s, FILE *stream);
extern int *stderr;

void print(const char *v1)
{
    fputs(v1, stderr);
}
