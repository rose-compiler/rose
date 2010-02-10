
// #include "sage3.h"
// #include "rose.h"
// #include "sage3basic.h"
// #include "processSupport.h"

#include "assert.h"
#include "roseInternal.h"

// void __assert_fail(const char*, const char*, unsigned int, const char*); // Not defined on OS X
// extern "C" void __assert_fail();


#if 0
void foo1()
   {
     ROSE_ASSERT (!"Inside of foo1");
   }

void foo2( int x )
   {
     switch (x)
        {
          case 1: ROSE_ASSERT (!"fail in case 1");
          default: ROSE_ASSERT (!"fail in default case");
        }

     ROSE_ASSERT (!"fail at end of foo2");
   }

namespace X
   {
     void foo1()
        {
          ROSE_ASSERT (!"Inside of X::foo1");
        }


     void foo2( int x )
        {
          switch (x)
             {
               case 1: ROSE_ASSERT (!"fail in case 1");
               default: ROSE_ASSERT (!"fail in default case");
             }

          ROSE_ASSERT (!"fail at end of X::foo2");
        }
   }
#endif

struct Y
   {
     void foo1()
        {
          ROSE_ASSERT (!"Inside of struct Y::foo1 (using ROSE_ASSERT)");
          assert (!"Inside of struct Y::foo1 (using assert)");
        }

#if 0
     void foo2( int x )
        {
          switch (x)
             {
               case 1: ROSE_ASSERT (!"fail in case 1");
               default: ROSE_ASSERT (!"fail in default case");
             }

          ROSE_ASSERT (!"fail at end of struct Y::foo2");
        }
#endif
   };

