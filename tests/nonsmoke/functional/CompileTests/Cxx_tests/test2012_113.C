// Include portable var arg mechanism
#include <stdio.h>                      

#include <stdlib.h>
// #include <stdarg.h>
// #include <varargs.h>

#if 0
void foo(char *fmt, ...)
   {
     va_list args;
     va_start(args,fmt);
     va_arg(args,int);
     va_end(args);
   }
#endif
