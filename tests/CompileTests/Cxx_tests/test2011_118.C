#if 0
const unsigned long foo()
   {
     return 7;
   }

// Fortunately a const expression can't include a function call.
// function call is not allowed in a constant expression
// int array_A[foo()]; // Error

void foobar()
   {
  // Fortunately a const expression can't include a function call.
  // function call is not allowed in a constant expression
  // int array_A[foo()]; // Error
   }
#endif


namespace X
   {
     const int const_size = 5;

     int non_const_size = const_size;
   }

int non_const_size = X::const_size;
