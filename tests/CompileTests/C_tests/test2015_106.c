#include <stdio.h>

#define DEMONSTRATE_ISSUE 1

void bug20150126(int type, const char *fmt, ... )
   {
     volatile char buf[512];
#if DEMONSTRATE_ISSUE
     volatile va_list args;
#else
     va_list args;
#endif
     __builtin_va_start(args,fmt);
     vsnprintf(buf,sizeof(buf),fmt,args);
     __builtin_va_end(args);
   }

int main() 
   {
     return 0;
   }
