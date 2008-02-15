// int x;

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

// int y;

void foobar()
   {
     foo(3.14);

  // Now force instatiation of a template which ROSE will build as a 
  // specialization and which will be defined twice (once in each file).
  // foo(1);
   }
