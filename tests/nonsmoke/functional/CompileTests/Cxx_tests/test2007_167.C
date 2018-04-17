
// Example code from regex.h

// This works when the struct is unparsed, since it is unparsed with a unique (generated) name.
// But it fails when the struct is defined in a header file since then the unique name is used,
// but the code for the struct using the unique name was never generated so we get an error
// (undefined variable).

// We can simulate this with a #line directive (actually we can't, I wonder why not! Likely because we would need the absolute path.
// #line 1 "rose_test2007_167.C"
// #line 1 "test2007_167.h"

#if 0
typedef struct
   {
     int rm_so;
     int rm_eo;
   } regmatch_t;
#else
// Test with this in a header file
#include "test2007_167.h"
#endif

// Also test with this in the current file
typedef struct
   {
     int rm_so;
     int rm_eo;
   } regmatch_t_alt;

// #line 17 "rose_test2007_167.C"
void foo()
   {
     regmatch_t var;
     var.rm_so = 0;

  // Example from sla++.C
     var.rm_so = var.rm_eo = -1;
   }


void foo_alt()
   {
     regmatch_t_alt var;
     var.rm_so = 0;

  // Example from sla++.C
     var.rm_so = var.rm_eo = -1;
   }
