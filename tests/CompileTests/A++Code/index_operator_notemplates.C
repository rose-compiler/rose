










#include "A++.h"

#define INLINE_DESCRIPTOR_SUPPORT FALSE
#define FILE_LEVEL_DEBUG 4

// This turns on the calls to the bounds checking function
#define BOUNDS_ERROR_CHECKING TRUE

// It helps to set this to FALSE sometimes for debugging code
// this enambles the A++/P++ operations in the bounds checking function
#define TURN_ON_BOUNDS_CHECKING   TRUE

extern "C"
   {
/* machine.h is found in MDI/machine.h through a link in A++/inc lude and P++/inc lude */
#include "machine.h"
   }






/* Array_Descriptors_for_Index_Operators() */

doubleArray doubleArray::operator() () const
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf ("Inside of doubleArray::operator()! \n");
#endif

#if defined(PPP)
   // I'm not sure how this should be done at the moment so let's debug the other 
   // stuff first.  This would require message passing to redistribute the array as 
   // a 1D array and so this step would be complicated (and inefficient as well). 
   // More thought about this function is required before implementation for P++. 
   printf ("ERROR: doubleArray::operator()() not supported in P++ yet! \n");
   APP_ABORT();
   return *this;
#else

   // Increment the reference count for the array data!
   incrementRawDataReferenceCount();

   double* New_Data_Pointer = Array_Descriptor.Array_Data; 
// Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION>* Vectorized_Descriptor_Pointer = 
//    Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION>::Vectorizing_Descriptor (Array_Descriptor);
   Array_Domain_Type* Vectorized_Domain_Pointer = 
      Array_Domain_Type::Vectorizing_Domain (Array_Descriptor.Array_Domain);
     
   return doubleArray ( New_Data_Pointer , Vectorized_Domain_Pointer );
#endif
}

//---------------------------------------------------------------

// This function takes an array of pointers to Internal_Index objects
doubleArray doubleArray::operator() 
   (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
    Index_Pointer_List ) const
{
   int i = 0;
   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}

//---------------------------------------------------------------
doubleArray doubleArray::operator() ( const Internal_Index & I ) const
{
// For the expression template version we want to avoid building a
// descriptor object and then passing it to an array object constructor
// and recopied within that constructor.  So the operator() should be 
// returning a constructor that internally builds the array's descriptor
// directly (thus avoiding the copying of the descriptor).  This will
// be done after we get the basic expression template implementation done.

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
   {
      printf ("Inside of doubleArray::operator(Internal_Index)! --");
      printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
               Array_ID(),getRawDataReferenceCount() );

   }
   Test_Consistency ("In doubleArray::operator() (Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   int i = 0;
   for (i=1; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

// display("Inside of operator()(const Internal_Index & I) const");
   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=2)
doubleArray doubleArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J ) const
{

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of doubleArray::operator(Internal_Index,Internal_Index)! -- Array_ID = %d getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In doubleArray::operator() (Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   int i = 0;
   for (i=2; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=3)

doubleArray doubleArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K ) const
{

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of doubleArray::operator(Internal_Index,Internal_Index,Internal_Index)! -- Array_ID = %d getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In doubleArray::operator() (Internal_Index,Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   int i = 0;
   for (i=3; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif


}
#endif
//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=4)
doubleArray doubleArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L ) const
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of doubleArray::operator(Internal_Index,Internal_Index,Internal_Index,Internal_Index)! -- Array_ID = %d  getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In doubleArray::operator() (Internal_Index,Internal_Index,Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   int i = 0;
   for (i=4; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif


}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=5)
doubleArray doubleArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   int i = 0;
   for (i=5; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=6)
doubleArray doubleArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   int i = 0;
   for (i=6; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=7)
doubleArray doubleArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N , 
     const Internal_Index & O ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   Index_Pointer_List[6] = &((Internal_Index &) O);
   int i = 0;
   for (i=7; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------


#if (MAX_ARRAY_DIMENSION >=8)
doubleArray doubleArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N , 
     const Internal_Index & O , const Internal_Index & P ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   Index_Pointer_List[6] = &((Internal_Index &) O);
   Index_Pointer_List[7] = &((Internal_Index &) P);
   int i = 0;
   for (i=8; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of doubleArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In doubleArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     doubleSerialArray *View = new doubleSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return doubleArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return doubleArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------
// *************************************************************************************
// *************************************************************************************
//                          INTARRAY INDEXING OPERATORS
// *************************************************************************************
// *************************************************************************************

// ****************************************************************
// Index operators for intArray objects (this provides indirect addressing and will
// be supported in the parallel P++ using work from Joel Saltz).  Maybe or maybe not!
// ****************************************************************

//---------------------------------------------------------------
doubleArray doubleArray::operator() ( const Internal_Indirect_Addressing_Index & I ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);

   int num_dims = 1;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     doubleArray_Descriptor_Type *Parallel_Descriptor = 
          new doubleArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* doubleSerialArray *View = new doubleSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(double,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     doubleSerialArray *View = new doubleSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return doubleArray ( View , Parallel_Descriptor );
     return doubleArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return doubleArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=2
doubleArray doubleArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);

   int num_dims = 2;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     doubleArray_Descriptor_Type *Parallel_Descriptor = 
          new doubleArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* doubleSerialArray *View = new doubleSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(double,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     doubleSerialArray *View = new doubleSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return doubleArray ( View , Parallel_Descriptor );
     return doubleArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return doubleArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=3
doubleArray doubleArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);

   int num_dims = 3;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     doubleArray_Descriptor_Type *Parallel_Descriptor = 
          new doubleArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* doubleSerialArray *View = new doubleSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(double,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     doubleSerialArray *View = new doubleSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return doubleArray ( View , Parallel_Descriptor );
     return doubleArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return doubleArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=4
doubleArray doubleArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);

   int num_dims = 4;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     doubleArray_Descriptor_Type *Parallel_Descriptor = 
          new doubleArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* doubleSerialArray *View = new doubleSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(double,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     doubleSerialArray *View = new doubleSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return doubleArray ( View , Parallel_Descriptor );
     return doubleArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return doubleArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=5
doubleArray doubleArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);

   int num_dims = 5;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     doubleArray_Descriptor_Type *Parallel_Descriptor = 
          new doubleArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* doubleSerialArray *View = new doubleSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(double,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     doubleSerialArray *View = new doubleSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return doubleArray ( View , Parallel_Descriptor );
     return doubleArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return doubleArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=6
doubleArray doubleArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);

   int num_dims = 6;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     doubleArray_Descriptor_Type *Parallel_Descriptor = 
          new doubleArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* doubleSerialArray *View = new doubleSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(double,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     doubleSerialArray *View = new doubleSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return doubleArray ( View , Parallel_Descriptor );
     return doubleArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return doubleArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=7
doubleArray doubleArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N , 
                                  const Internal_Indirect_Addressing_Index & O ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) O);


   int num_dims = 7;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     doubleArray_Descriptor_Type *Parallel_Descriptor = 
          new doubleArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* doubleSerialArray *View = new doubleSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(double,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     doubleSerialArray *View = new doubleSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return doubleArray ( View , Parallel_Descriptor );
     return doubleArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return doubleArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=8
doubleArray doubleArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N , 
                                  const Internal_Indirect_Addressing_Index & O , 
                                  const Internal_Indirect_Addressing_Index & P ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) O);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) P);

   int num_dims = 8;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     doubleArray_Descriptor_Type *Parallel_Descriptor = 
          new doubleArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* doubleSerialArray *View = new doubleSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(double,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     doubleSerialArray *View = new doubleSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return doubleArray ( View , Parallel_Descriptor );
     return doubleArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<double,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return doubleArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return doubleArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif
//---------------------------------------------------------------


floatArray floatArray::operator() () const
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf ("Inside of floatArray::operator()! \n");
#endif

#if defined(PPP)
   // I'm not sure how this should be done at the moment so let's debug the other 
   // stuff first.  This would require message passing to redistribute the array as 
   // a 1D array and so this step would be complicated (and inefficient as well). 
   // More thought about this function is required before implementation for P++. 
   printf ("ERROR: floatArray::operator()() not supported in P++ yet! \n");
   APP_ABORT();
   return *this;
#else

   // Increment the reference count for the array data!
   incrementRawDataReferenceCount();

   float* New_Data_Pointer = Array_Descriptor.Array_Data; 
// Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION>* Vectorized_Descriptor_Pointer = 
//    Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION>::Vectorizing_Descriptor (Array_Descriptor);
   Array_Domain_Type* Vectorized_Domain_Pointer = 
      Array_Domain_Type::Vectorizing_Domain (Array_Descriptor.Array_Domain);
     
   return floatArray ( New_Data_Pointer , Vectorized_Domain_Pointer );
#endif
}

//---------------------------------------------------------------

// This function takes an array of pointers to Internal_Index objects
floatArray floatArray::operator() 
   (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
    Index_Pointer_List ) const
{
   int i = 0;
   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}

//---------------------------------------------------------------
floatArray floatArray::operator() ( const Internal_Index & I ) const
{
// For the expression template version we want to avoid building a
// descriptor object and then passing it to an array object constructor
// and recopied within that constructor.  So the operator() should be 
// returning a constructor that internally builds the array's descriptor
// directly (thus avoiding the copying of the descriptor).  This will
// be done after we get the basic expression template implementation done.

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
   {
      printf ("Inside of floatArray::operator(Internal_Index)! --");
      printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
               Array_ID(),getRawDataReferenceCount() );

   }
   Test_Consistency ("In floatArray::operator() (Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   int i = 0;
   for (i=1; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

// display("Inside of operator()(const Internal_Index & I) const");
   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=2)
floatArray floatArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J ) const
{

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of floatArray::operator(Internal_Index,Internal_Index)! -- Array_ID = %d getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In floatArray::operator() (Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   int i = 0;
   for (i=2; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=3)

floatArray floatArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K ) const
{

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of floatArray::operator(Internal_Index,Internal_Index,Internal_Index)! -- Array_ID = %d getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In floatArray::operator() (Internal_Index,Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   int i = 0;
   for (i=3; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif


}
#endif
//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=4)
floatArray floatArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L ) const
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of floatArray::operator(Internal_Index,Internal_Index,Internal_Index,Internal_Index)! -- Array_ID = %d  getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In floatArray::operator() (Internal_Index,Internal_Index,Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   int i = 0;
   for (i=4; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif


}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=5)
floatArray floatArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   int i = 0;
   for (i=5; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=6)
floatArray floatArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   int i = 0;
   for (i=6; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=7)
floatArray floatArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N , 
     const Internal_Index & O ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   Index_Pointer_List[6] = &((Internal_Index &) O);
   int i = 0;
   for (i=7; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------


#if (MAX_ARRAY_DIMENSION >=8)
floatArray floatArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N , 
     const Internal_Index & O , const Internal_Index & P ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   Index_Pointer_List[6] = &((Internal_Index &) O);
   Index_Pointer_List[7] = &((Internal_Index &) P);
   int i = 0;
   for (i=8; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of floatArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In floatArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     floatSerialArray *View = new floatSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return floatArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return floatArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------
// *************************************************************************************
// *************************************************************************************
//                          INTARRAY INDEXING OPERATORS
// *************************************************************************************
// *************************************************************************************

// ****************************************************************
// Index operators for intArray objects (this provides indirect addressing and will
// be supported in the parallel P++ using work from Joel Saltz).  Maybe or maybe not!
// ****************************************************************

//---------------------------------------------------------------
floatArray floatArray::operator() ( const Internal_Indirect_Addressing_Index & I ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);

   int num_dims = 1;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     floatArray_Descriptor_Type *Parallel_Descriptor = 
          new floatArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* floatSerialArray *View = new floatSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(float,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     floatSerialArray *View = new floatSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return floatArray ( View , Parallel_Descriptor );
     return floatArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return floatArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=2
floatArray floatArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);

   int num_dims = 2;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     floatArray_Descriptor_Type *Parallel_Descriptor = 
          new floatArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* floatSerialArray *View = new floatSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(float,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     floatSerialArray *View = new floatSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return floatArray ( View , Parallel_Descriptor );
     return floatArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return floatArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=3
floatArray floatArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);

   int num_dims = 3;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     floatArray_Descriptor_Type *Parallel_Descriptor = 
          new floatArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* floatSerialArray *View = new floatSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(float,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     floatSerialArray *View = new floatSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return floatArray ( View , Parallel_Descriptor );
     return floatArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return floatArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=4
floatArray floatArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);

   int num_dims = 4;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     floatArray_Descriptor_Type *Parallel_Descriptor = 
          new floatArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* floatSerialArray *View = new floatSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(float,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     floatSerialArray *View = new floatSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return floatArray ( View , Parallel_Descriptor );
     return floatArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return floatArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=5
floatArray floatArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);

   int num_dims = 5;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     floatArray_Descriptor_Type *Parallel_Descriptor = 
          new floatArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* floatSerialArray *View = new floatSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(float,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     floatSerialArray *View = new floatSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return floatArray ( View , Parallel_Descriptor );
     return floatArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return floatArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=6
floatArray floatArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);

   int num_dims = 6;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     floatArray_Descriptor_Type *Parallel_Descriptor = 
          new floatArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* floatSerialArray *View = new floatSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(float,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     floatSerialArray *View = new floatSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return floatArray ( View , Parallel_Descriptor );
     return floatArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return floatArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=7
floatArray floatArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N , 
                                  const Internal_Indirect_Addressing_Index & O ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) O);


   int num_dims = 7;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     floatArray_Descriptor_Type *Parallel_Descriptor = 
          new floatArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* floatSerialArray *View = new floatSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(float,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     floatSerialArray *View = new floatSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return floatArray ( View , Parallel_Descriptor );
     return floatArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return floatArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=8
floatArray floatArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N , 
                                  const Internal_Indirect_Addressing_Index & O , 
                                  const Internal_Indirect_Addressing_Index & P ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) O);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) P);

   int num_dims = 8;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     floatArray_Descriptor_Type *Parallel_Descriptor = 
          new floatArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* floatSerialArray *View = new floatSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(float,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     floatSerialArray *View = new floatSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return floatArray ( View , Parallel_Descriptor );
     return floatArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<float,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return floatArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return floatArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif
//---------------------------------------------------------------


intArray intArray::operator() () const
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf ("Inside of intArray::operator()! \n");
#endif

#if defined(PPP)
   // I'm not sure how this should be done at the moment so let's debug the other 
   // stuff first.  This would require message passing to redistribute the array as 
   // a 1D array and so this step would be complicated (and inefficient as well). 
   // More thought about this function is required before implementation for P++. 
   printf ("ERROR: intArray::operator()() not supported in P++ yet! \n");
   APP_ABORT();
   return *this;
#else

   // Increment the reference count for the array data!
   incrementRawDataReferenceCount();

   int* New_Data_Pointer = Array_Descriptor.Array_Data; 
// Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION>* Vectorized_Descriptor_Pointer = 
//    Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION>::Vectorizing_Descriptor (Array_Descriptor);
   Array_Domain_Type* Vectorized_Domain_Pointer = 
      Array_Domain_Type::Vectorizing_Domain (Array_Descriptor.Array_Domain);
     
   return intArray ( New_Data_Pointer , Vectorized_Domain_Pointer );
#endif
}

//---------------------------------------------------------------

// This function takes an array of pointers to Internal_Index objects
intArray intArray::operator() 
   (const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type 
    Index_Pointer_List ) const
{
   int i = 0;
   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}

//---------------------------------------------------------------
intArray intArray::operator() ( const Internal_Index & I ) const
{
// For the expression template version we want to avoid building a
// descriptor object and then passing it to an array object constructor
// and recopied within that constructor.  So the operator() should be 
// returning a constructor that internally builds the array's descriptor
// directly (thus avoiding the copying of the descriptor).  This will
// be done after we get the basic expression template implementation done.

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
   {
      printf ("Inside of intArray::operator(Internal_Index)! --");
      printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
               Array_ID(),getRawDataReferenceCount() );

   }
   Test_Consistency ("In intArray::operator() (Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   int i = 0;
   for (i=1; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

// display("Inside of operator()(const Internal_Index & I) const");
   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=2)
intArray intArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J ) const
{

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of intArray::operator(Internal_Index,Internal_Index)! -- Array_ID = %d getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In intArray::operator() (Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   int i = 0;
   for (i=2; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=3)

intArray intArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K ) const
{

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of intArray::operator(Internal_Index,Internal_Index,Internal_Index)! -- Array_ID = %d getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In intArray::operator() (Internal_Index,Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   int i = 0;
   for (i=3; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif


}
#endif
//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=4)
intArray intArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L ) const
{
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > FILE_LEVEL_DEBUG)
      printf 
	 ("Inside of intArray::operator(Internal_Index,Internal_Index,Internal_Index,Internal_Index)! -- Array_ID = %d  getRawDataReferenceCount() = %d \n",
          Array_ID(),getRawDataReferenceCount() );

   Test_Consistency ("In intArray::operator() (Internal_Index,Internal_Index,Internal_Index,Internal_Index)");
#endif

   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   int i = 0;
   for (i=4; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif


}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=5)
intArray intArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   int i = 0;
   for (i=5; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=6)
intArray intArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   int i = 0;
   for (i=6; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------

#if (MAX_ARRAY_DIMENSION >=7)
intArray intArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N , 
     const Internal_Index & O ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   Index_Pointer_List[6] = &((Internal_Index &) O);
   int i = 0;
   for (i=7; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------


#if (MAX_ARRAY_DIMENSION >=8)
intArray intArray::operator() 
   ( const Internal_Index & I , const Internal_Index & J , 
     const Internal_Index & K , const Internal_Index & L , 
     const Internal_Index & M , const Internal_Index & N , 
     const Internal_Index & O , const Internal_Index & P ) const
{
   Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
   Index_Pointer_List[0] = &((Internal_Index &) I);
   Index_Pointer_List[1] = &((Internal_Index &) J);
   Index_Pointer_List[2] = &((Internal_Index &) K);
   Index_Pointer_List[3] = &((Internal_Index &) L);
   Index_Pointer_List[4] = &((Internal_Index &) M);
   Index_Pointer_List[5] = &((Internal_Index &) N);
   Index_Pointer_List[6] = &((Internal_Index &) O);
   Index_Pointer_List[7] = &((Internal_Index &) P);
   int i = 0;
   for (i=8; i < MAX_ARRAY_DIMENSION; i++)
      Index_Pointer_List[i] = NULL;

   // Start of macro defining common part of all operator()

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > FILE_LEVEL_DEBUG)
        {
          printf ("Inside of intArray::operator(Internal_Index*)! -- ");
          printf ("Array_ID = %d getRawDataReferenceCount() = %d \n",
                   Array_ID(),getRawDataReferenceCount() );
        }

     Test_Consistency ("In intArray::operator() (Internal_Index*)");
#endif

#if BOUNDS_ERROR_CHECKING
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Index_Pointer_List );
        }
#endif

#if defined(PPP)
  // Bug fix (11/7/94) we have to have the SerialArray be a view in the 
  // case where the view is a restriction on the local partition (i.e. 
  // smaller number of rows or columns that what the partition 
  // describes).
  // In the parallel environment we have to build the local Index 
  // particular to the this partition (i.e. the intersection of the 
  // partition domain and the global Index object).

     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Index_Pointer_List;
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Index_Pointer_List[i] != NULL)
             {
            // Cray T3D compiler will not allow us to take the address of 
            // the result of getLocalPart so this fix allows for a more 
            // clean implementation (though we have to free up the memory 
            // afterward).
               Local_Index_Pointer_List[i] = 
                    Index_Pointer_List[i]->getPointerToLocalPart(Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
             }
            else
             {
               Local_Index_Pointer_List[i] = NULL;
             }
        }

#if BOUNDS_ERROR_CHECKING
  // Since we build a view of the serial array using the Index object 
  // -- don't forget the error checking on the SerialArray (since we 
  // don't call the operator() directly).
     if (Internal_Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Index_Pointer_List );
        }
#endif

  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if BOUNDS_ERROR_CHECKING
#if 0
  // (7/27/2000) added assert
     if (Array_Descriptor.SerialArray->isNullArray() == TRUE)
        {
          if (Array_Descriptor.SerialArray->isView() == TRUE)
               APP_ASSERT (getDataPointer() != NULL);
        }
  // APP_ASSERT (Array_Descriptor.SerialArray->isNullArray() == FALSE);
     APP_ASSERT (getDataPointer() != NULL);
     APP_ASSERT (Array_Descriptor.SerialArray->Array_Descriptor.Array_Data != NULL);
#endif
#endif

  // building the serial array object!
     intSerialArray *View = new intSerialArray ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
                                               Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
                                               Local_Index_Pointer_List );
     APP_ASSERT (View != NULL);

  // ... Fixup User_Base for the SerialArray so that same cell in 
  // two partitions (a ghost cell) will be scalar indexed by the same
  // subscript(might want to make this a SerialArray function) ...
     View->Fixup_User_Base (Index_Pointer_List,Local_Index_Pointer_List);

  // Free up the data allocated via the heap! This is part of the fix 
  // for the P++ code so that it can work properly with the Cray T3D 
  // compiler. We could make this more efficient if we avoided allocation
  // and deallocation from the heap.
     for (i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
          if (Local_Index_Pointer_List[i] != NULL)
             {
               delete Local_Index_Pointer_List [i];
               Local_Index_Pointer_List[i] = NULL;
             }
        }

     Delete_Lhs_If_Temporary (*this);

     return intArray ( View, Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of P++ case
#else
  // A++ or SerialArray case:

  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should 
  // only be executed in A++ and the Serial_A++ classes.  It was 
  // previously executed in P++ as well.
  // Increment the reference count for the array data!
     incrementRawDataReferenceCount();

     Delete_Lhs_If_Temporary (*this);
     return intArray ( Array_Descriptor.Array_Data , Array_Descriptor.Array_Domain, Index_Pointer_List );

  // End of A++ or SerialArray case
#endif

}
#endif

//---------------------------------------------------------------
// *************************************************************************************
// *************************************************************************************
//                          INTARRAY INDEXING OPERATORS
// *************************************************************************************
// *************************************************************************************

// ****************************************************************
// Index operators for intArray objects (this provides indirect addressing and will
// be supported in the parallel P++ using work from Joel Saltz).  Maybe or maybe not!
// ****************************************************************

//---------------------------------------------------------------
intArray intArray::operator() ( const Internal_Indirect_Addressing_Index & I ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);

   int num_dims = 1;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     intArray_Descriptor_Type *Parallel_Descriptor = 
          new intArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* intSerialArray *View = new intSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(int,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     intSerialArray *View = new intSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return intArray ( View , Parallel_Descriptor );
     return intArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return intArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=2
intArray intArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);

   int num_dims = 2;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     intArray_Descriptor_Type *Parallel_Descriptor = 
          new intArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* intSerialArray *View = new intSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(int,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     intSerialArray *View = new intSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return intArray ( View , Parallel_Descriptor );
     return intArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return intArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=3
intArray intArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);

   int num_dims = 3;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     intArray_Descriptor_Type *Parallel_Descriptor = 
          new intArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* intSerialArray *View = new intSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(int,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     intSerialArray *View = new intSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return intArray ( View , Parallel_Descriptor );
     return intArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return intArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=4
intArray intArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);

   int num_dims = 4;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     intArray_Descriptor_Type *Parallel_Descriptor = 
          new intArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* intSerialArray *View = new intSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(int,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     intSerialArray *View = new intSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return intArray ( View , Parallel_Descriptor );
     return intArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return intArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=5
intArray intArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);

   int num_dims = 5;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     intArray_Descriptor_Type *Parallel_Descriptor = 
          new intArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* intSerialArray *View = new intSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(int,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     intSerialArray *View = new intSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return intArray ( View , Parallel_Descriptor );
     return intArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return intArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=6
intArray intArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);

   int num_dims = 6;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     intArray_Descriptor_Type *Parallel_Descriptor = 
          new intArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* intSerialArray *View = new intSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(int,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     intSerialArray *View = new intSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return intArray ( View , Parallel_Descriptor );
     return intArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return intArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------
#if MAX_ARRAY_DIMENSION >=7
intArray intArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N , 
                                  const Internal_Indirect_Addressing_Index & O ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) O);


   int num_dims = 7;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     intArray_Descriptor_Type *Parallel_Descriptor = 
          new intArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* intSerialArray *View = new intSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(int,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     intSerialArray *View = new intSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return intArray ( View , Parallel_Descriptor );
     return intArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return intArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif

//---------------------------------------------------------------

#if MAX_ARRAY_DIMENSION >=8
intArray intArray::operator() ( const Internal_Indirect_Addressing_Index & I , 
                                  const Internal_Indirect_Addressing_Index & J , 
                                  const Internal_Indirect_Addressing_Index & K , 
                                  const Internal_Indirect_Addressing_Index & L , 
                                  const Internal_Indirect_Addressing_Index & M , 
                                  const Internal_Indirect_Addressing_Index & N , 
                                  const Internal_Indirect_Addressing_Index & O , 
                                  const Internal_Indirect_Addressing_Index & P ) const
{

   Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_Pointer_List;
   Indirect_Index_Pointer_List[0] = &((Internal_Indirect_Addressing_Index &) I);
   Indirect_Index_Pointer_List[1] = &((Internal_Indirect_Addressing_Index &) J);
   Indirect_Index_Pointer_List[2] = &((Internal_Indirect_Addressing_Index &) K);
   Indirect_Index_Pointer_List[3] = &((Internal_Indirect_Addressing_Index &) L);
   Indirect_Index_Pointer_List[4] = &((Internal_Indirect_Addressing_Index &) M);
   Indirect_Index_Pointer_List[5] = &((Internal_Indirect_Addressing_Index &) N);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) O);
   Indirect_Index_Pointer_List[6] = &((Internal_Indirect_Addressing_Index &) P);

   int num_dims = 8;
   // ... (7/18/96, kdb) This same code was repeated reguardless of the number of 
   //  arguments so it is pulled out into a macro.  This makes it easier to
   //  make changes and to increase the number of dimensions.  It has been
   //  changed slightly so that some lists have been turned into loops.
   //  This may be less inefficient on the cray and so we may have to modify this
   //  later. ...

     int i = 0;
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Indirect_Index_Pointer_List[i] = NULL;

  // printf ("Inside of Macro \n");
#if BOUNDS_ERROR_CHECKING
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.Error_Checking_For_Index_Operators ( Indirect_Index_Pointer_List ); 
        }
#endif
  // printf ("DONE error checking: Inside of Macro \n");

#if defined(PPP)
  // Increment the reference count for the SerialArray's data!
     Array_Descriptor.SerialArray->incrementRawDataReferenceCount();

#if PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS
     printf ("WARNING: Redundent construction of parallel descriptor in operator() \n");
#endif

  // Build the parallel P++ descriptor with the Local_Index_Array(s)
  // This is a rather expensive operation (I'm not sure why we do this here!)
     intArray_Descriptor_Type *Parallel_Descriptor = 
          new intArray_Descriptor_Type (Array_Descriptor.Array_Domain,Indirect_Index_Pointer_List);
     APP_ASSERT(Parallel_Descriptor != NULL);

     Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Local_Indirect_Index_Pointer_List;
     for (i=0; i<num_dims; i++)
        {
          if (Parallel_Descriptor->Array_Domain.Local_Index_Array[i] != NULL)
             {
               Local_Indirect_Index_Pointer_List[i] = 
                    new Internal_Indirect_Addressing_Index (*(Parallel_Descriptor->Array_Domain.Local_Index_Array[i])); 
               APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
             }
            else
             {
	       if ( Indirect_Index_Pointer_List[i]->IndexInStorage )
                  {
                 // I think we need reference counting here!
	            Local_Indirect_Index_Pointer_List[i] = Indirect_Index_Pointer_List[i]->getPointerToLocalPart
                                                              (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,i);
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
	         else
                  {
	            Local_Indirect_Index_Pointer_List[i] = new Internal_Indirect_Addressing_Index ( Indirect_Index_Pointer_List[i]->Scalar );
                    APP_ASSERT(Local_Indirect_Index_Pointer_List[i] != NULL);
                  }
             }
        }

  // Make the rest of the pointers NULL for the remaining dimensions
     for (i=num_dims; i < MAX_ARRAY_DIMENSION; i++)
          Local_Indirect_Index_Pointer_List[i] = NULL;

#if BOUNDS_ERROR_CHECKING
  // We must to the error checking on the local serial processor level too!
     if (Index::Index_Bounds_Checking)
        {
          Array_Descriptor.SerialArray->Array_Descriptor.Error_Checking_For_Index_Operators ( Local_Indirect_Index_Pointer_List );
        }
#endif

  /* intSerialArray *View = new intSerialArray (
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          ARRAY_DESCRIPTOR_TYPE_SPECIFIC(int,Serial,MAX_ARRAY_DIMENSION) 
          (Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List ) );
   */
     intSerialArray *View = new intSerialArray 
        ( Array_Descriptor.SerialArray->Array_Descriptor.Array_Data ,
          Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,
          Local_Indirect_Index_Pointer_List );
     APP_ASSERT(View != NULL);

     for (i=0; i<num_dims; i++)
          delete Local_Indirect_Index_Pointer_List[i];

     APP_ASSERT(View != NULL);
     Delete_Lhs_If_Temporary (*this);
  // WARNING: Parallel_Descriptor constructed twice
  // return intArray ( View , Parallel_Descriptor );
     return intArray ( View , Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List);
#else
  // Bugfix (8/31/95) this fixes a memory leak in P++ this line should only
  // be executed in A++ and the Serial_A++ classes.  It was previously executed in 
  // P++ as well.  
  // Increment the reference count for the array data!
  // printf ("BEFORE incrementRawDataReferenceCount in operator()(intArray) \n");
     incrementRawDataReferenceCount();
  // printf ("in operator() and after incrementRawDataReferenceCount: Reference count = %d \n",ia.getReferenceCount());
  // printf ("Delete_Lhs_If_Temporary in operator()(intArray) \n");
     Delete_Lhs_If_Temporary (*this);
  // printf ("DONE: Delete_Lhs_If_Temporary in operator()(intArray) \n");
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Descriptor_Type<int,MAX_ARRAY_DIMENSION> (Array_Descriptor, Indirect_Index_Pointer_List) );
  // return intArray ( Array_Descriptor.Array_Data ,  
  //      new Array_Domain_Type (Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List) );
   return intArray ( Array_Descriptor.Array_Data, Array_Descriptor.Array_Domain, Indirect_Index_Pointer_List );
#endif
;
}
#endif
//---------------------------------------------------------------







/*include(../src/descriptor_macro.m4)*/

// ***************************************************************************************
//     Array_Descriptor constructors put here to allow inlining in Indexing operators!
// ***************************************************************************************

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

/* These index operators have been inlined in inline_func.h -- NOT! */


