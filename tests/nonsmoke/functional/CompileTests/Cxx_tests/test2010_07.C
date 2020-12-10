
// #include "sage3.h"
// #include "rose.h"
// #include "sage3basic.h"
// #include "processSupport.h" // This includes RoseAsserts.h

#include "assert.h"

#ifndef __APPLE__
#include "roseInternal.h"
#endif

// void __assert_fail(const char*, const char*, unsigned int, const char*); // Not defined on OS X
// extern "C" void __assert_fail();

// Note that the problem is that the use of ROSE_ASSERT() is in a function in a struct.
// Used elsewhere ROSE_ASSERT() is not a problem.  Also the issue is that
// CPP will translate this into:
// ((!"Inside of struct Y::foo1 (using assert)") ? static_cast<void> (0) : (__assert_fail ("!\"Inside of struct Y::foo1 (using assert)\"", "/home/dquinlan/ROSE/git-dq-main-rc/tests/nonsmoke/functional/CompileTests/Cxx_tests/test2010_07.C", 21, __PRETTY_FUNCTION__), static_cast<void> (0)));
// But EDG will substitute "__PRETTY_FUNCTION__" with "__assert_fail", or so it seems.

struct Y
   {
     void foo1()
        {
#ifndef __APPLE__
          ROSE_ASSERT (!"Inside of struct Y::foo1 (using ROSE_ASSERT)");
#endif
          assert (!"Inside of struct Y::foo1 (using assert)");
        }
   };

