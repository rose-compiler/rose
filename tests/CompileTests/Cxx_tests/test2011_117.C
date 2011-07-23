// const int const_size = 4;

template< class T >
class Y
   {
//   T* array[X::const_size];
   };
   
class Z {};

namespace X
   {
     const int const_size = 5;

     Y<Z[const_size]> a;
   }

// This should unparse as: "Y<Z[X::const_size]> a;"
Y<Z[X::const_size]> b;

// Z c[X::const_size];
