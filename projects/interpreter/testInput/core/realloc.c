#include <stdlib.h>

int test(int x)
   {
     long *foo = malloc(4 * sizeof(long));
     foo[3] = 11;
     foo = realloc(foo, 8 * sizeof(long));
     foo[7] = 22;
     char *bar = realloc(NULL, sizeof(char));
     bar[0] = 33;
     return foo[3] + foo[7] + bar[0];
   }
