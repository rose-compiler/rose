/* Jochen's #include file bug
// An error because the expressions are not tested for if they should be unparsed.
// Curently only statements are tested for if they should be unparsed and since the
// statement passes the test all the expressions in the statement are unparsed.
// The the #include directive putout in the code generation and the combination of
// the two of these causes an error.
*/

int k = 0;

int foo()
   {
  // int k = 0;

  // Currentl problem is that the SgVariableRefExp is the location of the variable declaration 
  // instead of the variable reference.  We need to fix this first.

  // Need to handle cases where this would be a compiler generated value from constant folding.
  // The solution is to recognize that "k" is not in the current file and that initializer ends
  // in a different file, so the ";" must be output after the include directive!  But I can't
  // tell were the "," is so this is a fragile solution.  A more robust solution is to
  // eliminate the include directive from the generated code, but this would be a problem is 
  // it included additional statements.  So there is no easy solution here!
     int i 
  // Should this include directive be unparsed?  
#include "test2006_33.h"
             ;

#if 0
  // This will cause the intializer to have a start an end in the current source file!
  // Remove the "(" and ")" to trigger error to be caught (start and end position in different files).
     int j = (
#include "test2006_33.h"
             );
#endif

     return 0x1110;
   }
