#include <cstdarg>

void foobar ( std::va_list args)
   {
     int count = 0;
     va_list args1;
     // va_start(args1, count);
     // va_start(args1, count);
     va_list args2;
     va_copy(args2, args1);

     // va_end(args1);
     // va_end(args2);
   }
