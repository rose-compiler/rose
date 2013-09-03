// #include <stdio.h>                      
// #include <stdlib.h>
#include <stdarg.h>

void func(int num, ...) {
    va_list arguments;
int i;
va_start(arguments, num);
for (i = 0; i < num; i++)
   0; // printf("Received %d\n", va_arg(arguments, int));
va_end(arguments);
}

