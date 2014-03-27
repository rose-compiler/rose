// Test code demonstrating bug in A++Code/abstract_op.C file.

class doubleArray
   {
     public:
          double* Array_Data;
   };

void foo ( const doubleArray & Rhs )
   {
  // double** Rhs_Data_Pointer = &(((doubleArray &)Rhs).Array_Descriptor.Array_Data);
     double** Rhs_Data_Pointer = &(((doubleArray &)Rhs).Array_Data);
   }

