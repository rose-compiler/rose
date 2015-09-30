// Similar to test2014_143.C (this code fails when
// the contant folding is applied to the array type size).

// const int const_size = 4;

template< class T >
class Y
   {
  // T* array[X::const_size];
   };

class Z {};

namespace X
   {
     const int const_size = 5;

#if 1
  // Type syntax not yet supported for variabel declarations.
     Y<Z[const_size]> a;
#endif
   }

#if 0
  // Type syntax not yet supported for variabel declarations.
int array[X::const_size];

// This should unparse as: "Y<Z[X::const_size]> a;"
Y<Z[X::const_size]> b;

Z c[X::const_size];
#endif

void foobar (int array[X::const_size]);
