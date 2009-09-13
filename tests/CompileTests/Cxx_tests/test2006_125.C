
#include<assert.h>

void foo()
   {
  // This is not a problem!
  // assert(!"Bad child in initialized name");
     if (false)
        {
          return;
        }
       else
          assert(false);

// Expands to be:
     (static_cast<void> ((!"Bad child in initialized name") ? 0 : (__assert_fail ("!\"Bad child in initialized name\"", "/home/dquinlan/ROSE/NEW_ROSE/developersScratchSpace/Dan/Cxx_tests/test2006_125.C", 17, __PRETTY_FUNCTION__), 0)));
   }

