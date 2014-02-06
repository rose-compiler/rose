#define xstrtou(rest) xstrtoull##rest

#include "test2014_06.h"

void foo()
   {
     xstrtou_range_sfx(42);
   }
