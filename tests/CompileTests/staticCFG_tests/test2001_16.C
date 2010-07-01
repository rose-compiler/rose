// This test code deomstrates a bug in the unparsing of the function call using a function pointer

/*
Original code:
     (*Operation_Array_Data) (
          Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
          (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
          (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
          Mask_Descriptor );

Unparsed code:
     *Operation_Array_Data ( (Temporary_Array.Array_Descriptor).Array_Data,
                             (X.Array_Descriptor).Array_Data,
                             Mask_Array_Data,
                             ((array_domain * )&(Temporary_Array.Array_Descriptor).Array_Domain),
                             ((array_domain * )&(X.Array_Descriptor).Array_Domain),
                             Mask_Descriptor); 
 */

#include <A++.h>

void
foo ( MDI_double_Prototype_0 Operation_Array_Data )
   {
     double* Temp_Array_Data = NULL;
     double* X_Array_Data    = NULL;
     int* Mask_Array_Data = NULL;

     array_domain* Temp_Descriptor = NULL;
     array_domain* X_Descriptor    = NULL;
     array_domain* Mask_Descriptor = NULL;

  // Either form of these function call now work
     (*Operation_Array_Data) ( Temp_Array_Data,
                               X_Array_Data,
                               Mask_Array_Data,
                               Temp_Descriptor,
                               X_Descriptor,
                               Mask_Descriptor);

  // This causes an error in the EDG/Sage III translation
     Operation_Array_Data ( Temp_Array_Data,
                               X_Array_Data,
                               Mask_Array_Data,
                               Temp_Descriptor,
                               X_Descriptor,
                               Mask_Descriptor);
   }












