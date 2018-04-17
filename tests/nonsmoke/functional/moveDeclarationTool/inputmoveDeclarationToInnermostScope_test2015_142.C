
#include "inputmoveDeclarationToInnermostScope_test2015_142.h"

#if 1

int foobar()
{
   double abc;

   do {

      global_abc = abc;

   } while (true);

   return 42;
}

#else

int foobar() {
  return 0;
}

#endif
