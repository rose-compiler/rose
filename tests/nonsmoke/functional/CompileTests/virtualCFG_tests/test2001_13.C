#include "A++.h"

int
main ()
   {
  // Test variable declarations of A++ objects
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List_1;
     intArray Mask ( Internal_Index_List_1 );

#if 1
     intArray X(10);
     Index I = Index( *(Internal_Index_List_1[0]) );

  // Use the variable to make sure we have the name unparsed correctly
  // Mask(I) = 0;
     Mask(I) = X(I);
#else
  // Use the variable to make sure we have the name unparsed correctly
     Mask = 0;
#endif

     return 0;
   }


