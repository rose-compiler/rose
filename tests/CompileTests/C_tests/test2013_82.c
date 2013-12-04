/* This case demonstrates a set of tokens that are
// between two statements and thus could be associated
// with the AST IR node statement(s) that are between the 
// same statements.  This needs to be computed in the
// evaluation of the Synthesized Attribute.
// The feature supporting this case is complicated
// by the source position information in EDG being
// unavailable (for statements constrcuted from CPP macros).
 */

#define TOKY_3(x,y,z) x y z

void foobar()
   {
     int a;
     TOKY_3(int,b,;)
     int c;
   }

