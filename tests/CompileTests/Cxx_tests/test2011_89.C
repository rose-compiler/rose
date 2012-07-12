namespace X
   {
  // int a;
     int namespace_function( int x );
   }

// Declaration of pointers to namespace specific functions is not allowed!
// typedef int (X::*PNF)(int);
typedef int (*PNF)(int);

void foobar()
   {
  // DQ (6/20/2011): I think that we are only missing the unparsing of the qualified name in the initializer...
     PNF namespace_function_pointer = X::namespace_function;

  // int b = X::a;
   }
