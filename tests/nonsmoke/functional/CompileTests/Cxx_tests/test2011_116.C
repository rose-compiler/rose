// const int const_size = 4;

namespace X
   {
     const int const_size_X = 5;

     int array_X_A[const_size_X];
   }

namespace Y
   {
     const int const_size = 6;

     int array_Y_A[X::const_size_X][const_size];
     int array_Y_B[const_size][X::const_size_X];
     int array_Y_C[X::const_size_X][X::const_size_X];
     int array_Y_D[const_size][const_size];
   }

int array_B[X::const_size_X][Y::const_size];
int array_C[Y::const_size][X::const_size_X][X::const_size_X];
int array_D[Y::const_size+X::const_size_X][X::const_size_X+X::const_size_X][X::const_size_X+Y::const_size];
