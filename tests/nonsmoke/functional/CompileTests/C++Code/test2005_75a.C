#if 1
#include "test2005_75.h"
#else
template <class T>
void foo ( T u )
   {
     u++;
   }

void foobar();
#endif

int main()
   {
     foo(42);

     foobar();
     
     return 0;
   }

/* END OF FILE COMMENT */
