
// Test for const cast (unparser appears to drop the cast)

class X
   {
     public:
          int x;
          int x_array[100];
          void foo() const;
   };

void X::foo() const
   {
  // Example of problem code
  // ((Array_Domain_Type*)(this))->IndexBase        [i] = Index_Array[i]->Array_Descriptor.Array_Domain.Base   [i];

      ((X*)(this))->x = 0;

      ((X*)(this))->x_array[0] = 0;
   }

