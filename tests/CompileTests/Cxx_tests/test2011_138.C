const int const_size_A = 7;
const int const_size_B = 42;

int array_A[const_size_A+1][const_size_B+1];

#if 0
namespace X
   {
     const int const_size_X = 5;
   }

namespace Y
   {
     const int const_size = 6;
   }

int array_A1[X::const_size_X+Y::const_size][Y::const_size+1];
int array_B[X::const_size_X][Y::const_size];
int array_C[Y::const_size][X::const_size_X][X::const_size_X];
// int array_D[Y::const_size+X::const_size_X][X::const_size_X+X::const_size_X][X::const_size_X+Y::const_size];
#endif
