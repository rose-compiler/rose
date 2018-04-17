#include "A++.h"

doubleArray doubleArray::operator() () const
   {
     double* New_Data_Pointer = NULL;
     Array_Domain_Type* Vectorized_Domain_Pointer = NULL;

  // Test 5
     return doubleArray ( New_Data_Pointer , Vectorized_Domain_Pointer );
   }

int
main ()
   {
  // Test 1
     doubleArray* arrayPtr1 = new doubleArray(42);

  // Test 2
     doubleArray* arrayPtr2 = new doubleArray();

  // Test 3
     char* copyString = new char[1];

  // Test variable declarations of A++ objects
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List_1;
     intArray Mask ( Internal_Index_List_1 );

  // Use the variable to make sure we have the name unparsed correctly
     Mask = 0;

     return 0;
   }

