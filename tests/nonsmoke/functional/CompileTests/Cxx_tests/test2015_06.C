
#include <stdio.h>

void bug20150126(int type,const char *fmt,... )
   {
     volatile char buf[512];
     volatile va_list args;
     __builtin_va_start(args,fmt);
     vsnprintf(buf,sizeof(buf),fmt,args);
     __builtin_va_end(args);
   }

int main() 
   {
     return 0;
   }

