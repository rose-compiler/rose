#include <malloc.h>
#include <string.h>

int test(int x)
   {
     void *foo = malloc(sizeof(int) * 4);
     memset(foo, 0, sizeof(int) * 4);
     int *fooChr = foo;
     fooChr[1] = 1;
     fooChr[2] = 2;
     int result = fooChr[0]+fooChr[1]+fooChr[2]+fooChr[3];
     free(foo);
     return result;
   }
