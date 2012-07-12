namespace X
   {
     const int const_size = 5;
#if 0
  // int array_A[const_size];
     struct Y 
        {
       // Here the bit with specifier does not require name qualification.
          int fieldVar_Y:const_size;
        };
#endif
   }

// int array_C[X::const_size];

struct Z
   {
  // Here the bit with specifier requires name qualification.
     int fieldVar_Z:(X::const_size+X::const_size*2);
   };

