namespace X
   {
     int namespace_function( int x );
   }

// Declaration of pointers to namespace specific functions is not allowed!
// typedef int (X::*PNF)(int);
typedef int (*PNF)(int);

void foobar()
   {
     PNF namespace_function_pointer = X::namespace_function;
   }
