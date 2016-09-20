// Jochen's #include file bug
// An error because the expressions are not tested for if they should be unparsed.
// Curently only statements are tested for if they should be unparsed and since the
// statement passes the test all the expressions in the statement are unparsed.
// The the #include directive putout in the code generation and the combination of
// the two of these causes an error.

void foo()
   {
     int k = 0;
     int i = (
#include "test2006_33.h"
             );
   }
