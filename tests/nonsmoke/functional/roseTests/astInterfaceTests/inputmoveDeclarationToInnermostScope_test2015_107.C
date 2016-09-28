// This is an example of a macro usage that does not work (because the expanded macro is transformed).
// The problem is that there is not token sequence for the partially unparsed for loop).
// So perhaps it should be unparsed from the AST directly.
#define COPYNODAL(A,B) \
  for (int local_i=0; local_i<=42; local_i++) { \
    B = A ; \
  }

void foobar()
   {
     int i;
     int j;

     COPYNODAL(i,j);
   }
