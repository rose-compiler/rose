// Allow repeated includes of inline_func.h without error
#ifndef _APP_INLINE_FUNC_H
#define _APP_INLINE_FUNC_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif








#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *SerialArray_Descriptor_Type::operator new ( size_t Size )
#endif


#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_Steal_Data::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_0::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_1::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_2::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_3::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_4::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_5::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_6::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_7::operator new ( size_t Size )
#endif


#ifndef INTARRAY
#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_8::operator new ( size_t Size )
#endif

#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_9::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_11::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *doubleSerialArray_Function_12::operator new ( size_t Size )
#endif


#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *SerialArray_Descriptor_Type::operator new ( size_t Size )
#endif


#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_Steal_Data::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_0::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_1::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_2::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_3::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_4::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_5::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_6::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_7::operator new ( size_t Size )
#endif


#ifndef INTARRAY
#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_8::operator new ( size_t Size )
#endif

#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_9::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_11::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *floatSerialArray_Function_12::operator new ( size_t Size )
#endif



#define INTARRAY
#ifdef INLINE_FUNCTIONS

inline void *intSerialArray::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *SerialArray_Descriptor_Type::operator new ( size_t Size )
#endif


#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_Steal_Data::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_0::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_1::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_2::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_3::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_4::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_5::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_6::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_7::operator new ( size_t Size )
#endif


#ifndef INTARRAY
#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_8::operator new ( size_t Size )
#endif

#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_9::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_11::operator new ( size_t Size )
#endif

#ifdef INLINE_FUNCTIONS

inline void *intSerialArray_Function_12::operator new ( size_t Size )
#endif


#undef INTARRAY

/* Macro Expansion below */
#ifdef INLINE_FUNCTIONS

inline void *SerialArray_Statement_Type::operator new ( size_t Size )
#endif



/* MACRO EXPANSION BEGINS HERE */

/* We have to put the INTARRAY expansion first since these
   have inlined descriptor functions that are used later in the 
   DOUBLEARRAY and FLOATARRAY and some compilers (SGI) will
   complain if the descriptor member function is used before 
   being declared as inline as any good C++ compiler should.
 */

#define INTARRAY
// **********************************************************************
//  Inlined scalar i n d e x operators (this is the only way to make such 
//  things fast).  Though there use is officially discouraged since the  
//  performance is poor!
// **********************************************************************

/* inlining the i n d e x operators */

#if 1
#if !defined(USE_EXPRESSION_TEMPLATES)
// *************************************************************************
// *************************************************************************
//                      INLINED SCALAR OPERATOR()'S
// *************************************************************************
// *************************************************************************

#if HPF_INDEXING
#error Scalar Indexing operators written for non HPF_INDEXING
#endif

inline int & intSerialArray::operator() ( int i ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class. The prefered way of indexing array objects is to use the Index class!

//==============================================================================
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
        printf ("Inside of intSerialArray::operator() ( int i=%d ) \n",i);
   Test_Consistency();
#endif
//==============================================================================

#if defined(BOUNDS_CHECK)
   // This could be simplified if we just assumed that the first
   // number of entries were valid up to the problem dimension.
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   for (int temp_index=1; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 1);
   int Address_Subscript = 0;

#if 0
#if defined(PPP)
   const int* Base      = getSerialDomain().Base;
   const int* Data_Base = getSerialDomain().Data_Base;
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Base      = Array_Descriptor.Array_Domain.Base;
   const int* Data_Base = Array_Descriptor.Array_Domain.Data_Base;
   const int* Size      = Array_Descriptor.Array_Domain.Size;
   const int* Stride    = Array_Descriptor.Array_Domain.Stride;
#endif
#endif

   int temp;
   if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing)
   {
      // This handles the case of scalar indexing applied to a array object
      // previously indexed using indirect addressing (a view).
      // Since all such views are defined to be 1 dimensional arrays
      // we need only worry about this detail in the scalar opeerator(int) 
      // (taking a single scalar parameter).

           
#if defined(PPP)
      Address_Subscript = 
         ( (getSerialDomain().Index_Array [0] != NULL) ? 
         ((*(getSerialDomain().Index_Array[0]))(i) - 
         getSerialDomain().Data_Base[0]) : 
         getSerialDomain().Base[0] ) * 
         getSerialDomain().Stride [0];
      for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
         Address_Subscript += 
            ( (getSerialDomain().Index_Array [temp] != NULL) ? 
            ((*(getSerialDomain().Index_Array[temp]))(i) - 
            getSerialDomain().Data_Base[temp]) : 
            getSerialDomain().Base[temp] ) * 
            getSerialDomain().Stride [temp] * 
            getSerialDomain().Size[temp-1];
#else
      Address_Subscript = 
         ( (Array_Descriptor.Array_Domain.Index_Array [0] != NULL) ? 
         ((*(Array_Descriptor.Array_Domain.Index_Array[0]))(i) - 
         Array_Descriptor.Array_Domain.Data_Base[0]) : 
         Array_Descriptor.Array_Domain.Base[0] ) * 
         Array_Descriptor.Array_Domain.Stride [0];
      for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
         Address_Subscript += 
            ( (Array_Descriptor.Array_Domain.Index_Array [temp] != NULL) ? 
            ((*(Array_Descriptor.Array_Domain.Index_Array[temp]))(i) - 
            Array_Descriptor.Array_Domain.Data_Base[temp]) : 
            Array_Descriptor.Array_Domain.Base[temp] ) * 
            Array_Descriptor.Array_Domain.Stride [temp] * 
            Array_Descriptor.Array_Domain.Size[temp-1];
#endif
   }
   else
   {
      // This is the more common case (normal indexing using a scalar)
      // We have to use all the dimensions becase we might be accessing
      // a lower dimensional view of a higher dimensional array
#if defined(PPP)
      Address_Subscript = i * getSerialDomain().Stride[0];
#else
      Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0];
#endif
   }

#if defined(PPP)
   APP_ASSERT (usesIndirectAddressing() == FALSE);

   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
      Off_Processor_Including_Ghost_Boundaries =
         ((ii < Local_Data_Base[0])|| 
            (ii > Local_Data_Base[0]+Local_Size[0]-1));

      Off_Processor_Excluding_Ghost_Boundaries =
         ((ii < Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
            (ii > Local_Data_Base[0]+Local_Size[0]-1-
              InternalGhostCellWidth[0]));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
       {
         Scalar_Indexing_For_intArray_With_Message_Passing 
              (Address_Subscript+Local_Scalar_Offset[0], 
              Off_Processor_Including_Ghost_Boundaries,
              Off_Processor_Excluding_Ghost_Boundaries, 
              Array_Index_For_int_Variable);
       }
      else
       {
      // Bugfix (1/2/97) We have to reset the Array_Index_For_int_Variable to zero because it
      // in incremented in the code below and otherwize only reset in the function
      // Scalar_Indexing_For_intArray_With_Message_Passing.
         APP_ASSERT (Array_Index_For_int_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_int_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_int_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_int_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer0 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[0]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_int_Variable[Array_Index_For_int_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer0 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else

   APP_ASSERT (Array_Descriptor.Array_View_Pointer0 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[0]);
   return Array_Descriptor.Array_View_Pointer0 [Address_Subscript];
#endif
}

#if MAX_ARRAY_DIMENSION >= 2
inline int & intSerialArray::operator() ( int i , int j ) const
{
   // Note that the use of a scalar  i n d e x i n g  is not efficient in an array 
   // class the perfered way to  i n d e x  array objects is to use the Index class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Inside of intSerialArray::operator() ( int i=%d , int j=%d ) \n",i,j);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   for (int temp_index=2; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
      Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 2);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];

   /*
   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells, Local_Bound is only related to view so size must be
   //  used instead ...
   */

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {

      Off_Processor_Including_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]) ||
           (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
          ((jj<Local_Data_Base[1]) ||
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)));

      Off_Processor_Excluding_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1
              -InternalGhostCellWidth[0])) ||
          ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
              -InternalGhostCellWidth[1])));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_intArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[1], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries,
          Array_Index_For_int_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_int_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_int_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_int_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_int_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer1 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[1]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_int_Variable[Array_Index_For_int_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer1 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else

   APP_ASSERT (Array_Descriptor.Array_View_Pointer1 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[1]);
   return Array_Descriptor.Array_View_Pointer1 [Address_Subscript];
#endif
}
#endif 

#if MAX_ARRAY_DIMENSION >= 3
inline int & intSerialArray::operator() ( int i , int j , int k ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
     printf ("Inside of intSerialArray::operator() ( int i=%d , int j=%d , int k=%d ) \n",
             i,j,k);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   for (int temp_index=3; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
      Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 3);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
     Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...


   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
      Off_Processor_Including_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
          ((jj<Local_Data_Base[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
          ((kk<Local_Data_Base[2]) || 
           (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)));

      Off_Processor_Excluding_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1
              -InternalGhostCellWidth[0]))||
          ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
              -InternalGhostCellWidth[1]))||
          ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
           (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
              -InternalGhostCellWidth[2])));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
       Scalar_Indexing_For_intArray_With_Message_Passing 
          (Address_Subscript+Local_Scalar_Offset[2], Off_Processor_Including_Ghost_Boundaries,
           Off_Processor_Excluding_Ghost_Boundaries, 
           Array_Index_For_int_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_int_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_int_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_int_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_int_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer2 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[2]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_int_Variable[Array_Index_For_int_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer2 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer2 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[2]);
   return Array_Descriptor.Array_View_Pointer2 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 4
inline int & intSerialArray::operator() ( int i , int j , int k , int l ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
     printf("Inside of intSerialArray::operator()(int i=%d,int j=%d,int k=%d,int l=%d)\n",
            i,j,k,l);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   for (int temp_index=4; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 4);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_intArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[3], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_int_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_int_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_int_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_int_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_int_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer3 == 
      getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[3]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_int_Variable[Array_Index_For_int_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer3 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer3 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[3]);
   return Array_Descriptor.Array_View_Pointer3 [Address_Subscript];
   //return Array_Data [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 5
inline int & intSerialArray::operator() ( int i , int j , int k , int l, int m ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   for (int temp_index=5; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 5);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_intArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[4], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_int_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_int_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_int_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_int_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_int_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer4 == 
           getSerialArrayDescriptor().Array_Data + 
           getSerialDomain().Scalar_Offset[4]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_int_Variable[Array_Index_For_int_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer4 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer4 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[4]);
   return Array_Descriptor.Array_View_Pointer4 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 6
inline int & intSerialArray::operator() (int i, int j, int k, int l, int m, int n) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   for (int temp_index=6; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 6);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_intArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[5], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_int_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_int_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_int_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_int_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_int_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer5 == 
           getSerialArrayDescriptor().Array_Data + 
           getSerialDomain().Scalar_Offset[5]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_int_Variable[Array_Index_For_int_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer5 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer5 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[5]);
   return Array_Descriptor.Array_View_Pointer5 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 7
inline int & intSerialArray::operator() (int i, int j, int k, int l, int m, 
                                   int n, int o) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   Scalar_Index_List [6] = &o;
   for (int temp_index=7; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

 //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 7);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4] +
                       o * getSerialDomain().Stride[6] * getSerialDomain().Size[5];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4] +
                       o * Array_Descriptor.Array_Domain.Stride[6] * 
                       Array_Descriptor.Array_Domain.Size[5];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];
   int oo = (o-Local_User_Base[6]) * Local_Stride[6] + Local_Data_Base[6]+
      Local_Base[6];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)) ||
       ((oo<Local_Data_Base[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5]))||
       ((oo<Local_Data_Base[6]+InternalGhostCellWidth[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1
           -InternalGhostCellWidth[6])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_intArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[6], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_int_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_int_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_int_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_int_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_int_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer6 == 
            getSerialArrayDescriptor().Array_Data + 
            getSerialDomain().Scalar_Offset[6]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_int_Variable[Array_Index_For_int_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer6 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer6 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[6]);
   return Array_Descriptor.Array_View_Pointer6 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 8
inline int & intSerialArray::operator() (int i, int j, int k, int l, int m, 
                                   int n, int o, int p) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   Scalar_Index_List [6] = &o;
   Scalar_Index_List [7] = &p;
   for (int temp_index=8; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 8);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4] +
                       o * getSerialDomain().Stride[6] * getSerialDomain().Size[5] +
                       p * getSerialDomain().Stride[7] * getSerialDomain().Size[6];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4] +
                       o * Array_Descriptor.Array_Domain.Stride[6] * 
                       Array_Descriptor.Array_Domain.Size[5] +
                       p * Array_Descriptor.Array_Domain.Stride[7] * 
                       Array_Descriptor.Array_Domain.Size[6];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];
   int oo = (o-Local_User_Base[6]) * Local_Stride[6] + Local_Data_Base[6]+
      Local_Base[6];
   int pp = (p-Local_User_Base[7]) * Local_Stride[7] + Local_Data_Base[7]+
      Local_Base[7];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)) ||
       ((oo<Local_Data_Base[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1)) ||
       ((pp<Local_Data_Base[7]) || 
        (pp>Local_Data_Base[7]+(Local_Size[7]/Local_Size[6])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5]))||
       ((oo<Local_Data_Base[6]+InternalGhostCellWidth[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1
           -InternalGhostCellWidth[6]))||
       ((pp<Local_Data_Base[7]+InternalGhostCellWidth[7]) || 
        (pp>Local_Data_Base[7]+(Local_Size[7]/Local_Size[6])-1
           -InternalGhostCellWidth[7])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_intArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[7], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_int_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_int_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_int_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_int_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_int_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer7 == 
            getSerialArrayDescriptor().Array_Data + 
            getSerialDomain().Scalar_Offset[7]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_int_Variable[Array_Index_For_int_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer7 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer7 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[7]);
   return Array_Descriptor.Array_View_Pointer7 [Address_Subscript];
#endif
}
#endif

// End of if !defined(USE_EXPRESSION_TEMPLATES)
#endif

// Endif for inlining of  i n d e x  operators! 

// ********************************************************************************
// ********************************************************************************
//        INLINING FUNCTIONS SPECIFIC TO A++/P++ (Not user application code)
// ********************************************************************************
// ******************************************************************************** 

#if defined(INTARRAY)
// Use if d e f to force only ONE instance of the dimension function!
// Dimension function is used so much that it should be inlined!

inline int
SerialArray_Domain_Type::getBase( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
  // return Data_Base[Axis] + Base[Axis];
     return User_Base[Axis];
   }

inline int
SerialArray_Domain_Type::getRawBase( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
     return Data_Base[Axis] + Base[Axis];
   }

inline int
SerialArray_Domain_Type::getBound( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
  // return Data_Base[Axis] + Bound[Axis];

     return User_Base[Axis] + (Bound[Axis]-Base[Axis]) / Stride[Axis];
   }

inline int
SerialArray_Domain_Type::getRawBound( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Data_Base[Axis] + Bound[Axis];
   }

inline int
SerialArray_Domain_Type::getStride( int Axis ) const
   {
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
  // return Stride[Axis];
     return 1;
   }

inline int
SerialArray_Domain_Type::getRawStride( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Stride[Axis];
   }

inline Range
SerialArray_Domain_Type::dimension( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Range(getBase(Axis),getBound(Axis),getStride(Axis));
   }

inline Range
SerialArray_Domain_Type::getFullRange( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     int length = Size[Axis];
     if (Axis>0) length /= Size[Axis-1];
  // return Range(Data_Base[Axis],length+Data_Base[Axis]-1,Stride[Axis]);
  // return Array_Domain.getFullRange(Axis);

#if 0
#if defined(PPP)
  // This fails on P++/TESTS/test2000_02.C (as I think it should)
     APP_ASSERT (Size[Axis]      == Local_Mask_Index[Axis].getLength());
     APP_ASSERT (Data_Base[Axis] == Local_Mask_Index[Axis].getBase());
     APP_ASSERT (Data_Base[Axis]+Size[Axis] == Local_Mask_Index[Axis].getBound());
     APP_ASSERT (1 == Local_Mask_Index[Axis].getStride());
#endif
#endif

  // (4/29/2000) Fixed bug in stride (which should always be 1 for the full range)
  // return Range(Data_Base[Axis],length+Data_Base[Axis]-1,Stride[Axis]);
     return Range(Data_Base[Axis],length+Data_Base[Axis]-1,1);
   }

inline int
SerialArray_Domain_Type::getLength( int Axis ) const
   {
  // Computes the length of any axis of the array object using this descriptor!
  // This function should be inlined for better efficency!
  // This could be made a little more efficient by avioding the division by the
  // stride in the most common case where the unit stride.
  // Also in the commmon case where Is_A_View == FALSE we could just return the Size[Axis].
 
  // int Temp_Size = (Bound [Axis] - Base [Axis]) + 1;
  // return ( Temp_Size % Stride [Axis] == 0 ) ? Temp_Size / Stride [Axis] : Temp_Size / Stride [Axis] + 1;

     int Temp_Size = 0;
  // Bugfix (11/1/95) Previously did not handle case of Uses_Indirect_Addressing == TRUE
     if (Uses_Indirect_Addressing)
        {
       // Bugfix (11/2/95) Only return Array_Size for case of querey along Axis == 0
          Temp_Size = (Axis == 0) ? Array_Size() : 1;
        }
       else
        {
          Temp_Size = (Bound [Axis] - Base [Axis]) + 1;
          if ( Temp_Size % Stride [Axis] == 0 ) 
               Temp_Size /= Stride [Axis];
            else
               Temp_Size = Temp_Size / Stride [Axis] + 1;
        }

     return Temp_Size;
   }

inline int
SerialArray_Domain_Type::maxNumberOfDimensions()
   {
  // Get static upper bound on number of dimensions of the array objects
     return MAX_ARRAY_DIMENSION;
   }

inline int
SerialArray_Domain_Type::numberOfDimensions() const
   {
  // Get number of dimensions of the array objects (note that a 1x2 array is 2 dimensional)
     return Domain_Dimension;
   }

inline int
SerialArray_Domain_Type::internalNumberOfDimensions() const
   {
  // For array objects not using indirect addressing we use the 
  // current value of Descriptor_Dimension.  But for array objects
  // using indirect addressing we return the number of intArrays in
  // use to discribe the indirect addressing internally.

     int Return_Value = -1;
     if (usesIndirectAddressing() == TRUE)
        {
       // for (int i= MAX_ARRAY_DIMENSION-1; i >= 0; i--)
          int i = MAX_ARRAY_DIMENSION-1;
          while (Return_Value == -1)
             {
               APP_ASSERT (i >= 0);
            // if (Index_Array[i] != NULL)
            // We want the larges value of the axis that is used accounting for the
            // way that Internal_Index objects and intArray objects can be mixed in the indexing.
            // if ( (Index_Array[i] != NULL) || (Base[i] != Bound[i]) )
            // Checking for the base not equal to zero allows us to 
            // detect scalar indexing in the higher dimensions!
               if ( (Index_Array[i] != NULL) || (Base[i] != 0) )
                  Return_Value = i+1;
               i--;
             }
          APP_ASSERT (Return_Value > 0);
        }
       else
        {
          Return_Value = Domain_Dimension;
        }
     return Return_Value;
   }

// End of if defined(INTARRAY)
#endif

inline int* APP_RESTRICT_MACRO
intSerialArray::getDataPointer () const
   {
     return Array_Descriptor.getDataPointer();
   }

#if defined(PPP)
inline intSerialArray*
intSerialArray::getSerialArrayPointer () const
   {
  // return pointer to A++ array
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     return Array_Descriptor.SerialArray;
   }
inline intSerialArray**
intSerialArray::getSerialArrayPointerLoc () const
   {
  // return pointer to A++ array address
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     return (intSerialArray**)&Array_Descriptor.SerialArray;
   }
#endif

// Bugfix (1/13/97) moved this function from the collection
// of local access functions below.  The problem was that it 
// was not being found at link time.
inline Range
intSerialArray::getFullRange (int Axis) const
   {
      return Array_Descriptor.getFullRange(Axis);
   }

// This is defined for APP SERIAL_APP and PPP
inline Range
intSerialArray::getLocalFullRange (int Axis) const
   {
#if defined(PPP)
      return getSerialArrayDescriptor().getFullRange(Axis);
#else
      return getFullRange(Axis);
#endif
   }

#if defined(PPP) || defined(APP)
// These are acces functions to local parts of the P++ array.  They must exist in A++
// as well - though they return the global data if used in A++ since that is what local means
// in a serial environment.

#if defined(PPP) 
inline intSerialArray intSerialArray::getLocalArray () const
#else
inline intArray intSerialArray::getLocalArray () const
#endif
   {
  // return reference to A++ array

#if defined(PPP)
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);
     return intSerialArray(*Array_Descriptor.SerialArray,SHALLOWCOPY);
  // printf("getLocalArray doesn't work in parallel right now\n");
#else
     return intArray(*this,SHALLOWCOPY);
#endif
   }

// Must be made available in PPP - SERIAL_APP - APP
// inline Range intSerialArray::getFullRange (int Axis) const
//    {
//       return Array_Descriptor.getFullRange(Axis);
//    }

#if 0
// This function was placed into array.C file (11/29/2000)
#if defined(PPP)
inline intSerialArray
intSerialArray::getLocalArrayWithGhostBoundaries () const
#else
inline intArray
intSerialArray::getLocalArrayWithGhostBoundaries () const
#endif
   {
  // return A++ array that includes ghost cells if P++ or a shallow copy 
  // of itself if A++

  // APP_ASSERT(!Array_Descriptor.Is_A_View);

#if defined(PPP) 
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     if ((!Array_Descriptor.Array_Domain.Is_A_View) && 
         (!Array_Descriptor.Array_Domain.Is_A_Null_Array) &&
         (!getSerialDomain().Is_A_Null_Array))
     {
        Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
        int i;
        for (i=0;i<MAX_ARRAY_DIMENSION;i++)
           {
             Internal_Index_List[i] = new Range(getLocalFullRange(i));
             APP_ASSERT (Internal_Index_List[i] != NULL);
           }
        
        intSerialArray ReturnArray;
        ReturnArray.adopt(getDataPointer(),Internal_Index_List);
        for (i=0;i<MAX_ARRAY_DIMENSION;i++) 
           {
          // Range objects don't have referenceCounting! (should they?)
             delete Internal_Index_List[i];
           }
        return intSerialArray(ReturnArray,SHALLOWCOPY);
     }
     else
     {
        return intSerialArray(*Array_Descriptor.SerialArray,SHALLOWCOPY);
     }
     //printf("getLocalArrayWithGhostBoundaries doesn't work in parallel right now\n");
#else
     return intArray(*this,SHALLOWCOPY);
#endif
   }
#endif

// end of defined(PPP) || defined(APP)
#endif

// Endif for function inlining
#endif


// We needed to place this below the Allocate_Array_Data function because
// The expression template constructor needed to see it at compile time.
// if defined(INLINE_APP1_FUNCTIONS)

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
// forward declaration
// typedef struct Array_Domain_c     array_domain;
// typedef struct Array_Descriptor_c array_descriptor;

#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" int* MDI_int_Allocate ( array_domain* Descriptor );
#endif

#if defined(INTARRAY)
// error "Inside of inline_func.h"
#endif

//inline void intSerialArray::Allocate_Array_Data ( bool Force_Memory_Allocation ) const
inline
void
intSerialArray::Allocate_Array_Data ( bool Force_Memory_Allocation ) 
   {
  // This function allocates the internal data for the intSerialArray object.  In A++
  // this allocates the the raw int data array.  In P++ it allocates the internal
  // A++ array for the current processor (using the sizes defined in the DARRAY
  // parallel descriptor).

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("*** Allocating the Array_Data (or getting it from the hash table)! \n");
#endif  

#if defined(PPP)
  // The allocation of data in the parallel environment is especially complicated
  // so this is broken out as a separate function and is implemented only for P++.
     Array_Descriptor.Allocate_Parallel_Array (Force_Memory_Allocation);
#else
  // To improve the implementation we have moved this functionality into the
  // Array_Descriptor_Type object because that is where the pointers to the
  // Array_Data (or Serial_Array_Data_Pointer in the case of P++) now live.
     APP_ASSERT (getDataPointer() == NULL);
     Array_Descriptor.Allocate_Array_Data(Force_Memory_Allocation);
#endif  // if !defined(PPP)

#if !defined(PPP)
  // Since this is a new array object is should have an initialize reference count on its
  // raw data.  This is required here because the reference counting mechanism reused the
  // value of zero for one existing reference and no references (this will be fixed soon).
     resetRawDataReferenceCount();
#endif
   }          

// We needed to place this below the Allocate_Array_Data function because
// The expression template constructor needed to see it at compile time.
#if defined(INLINE_APP1_FUNCTIONS)

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" void MDI_int_Deallocate ( int* Data_Pointer , array_domain* Descriptor );
#endif
inline
void
intSerialArray::Delete_Array_Data ()
   {
  // Main body of code moved to Array_Descriptor_Type
  // To improve the implementation we have moved this functionality into the
  // Array_Descriptor_Type object because that is where the pointers to the
  // Array_Data (or Serial_Array_Data_Pointer in the case of P++) now live.
     Array_Descriptor.Delete_Array_Data();
   }

// ***********************************************************************
// Part of error checking is to verify binary conformability (i.e. the
// array operations between arrays of the same size IN EACH DIMENSION).
// ***********************************************************************
inline bool
intSerialArray::Binary_Conformable ( const intSerialArray & X ) const
   {
  // We must defin e binary conformable to exclude the use of the where statement
     return ( ( (Array_Descriptor.Array_Domain.Is_Contiguous_Data == TRUE)     &&
                (X.Array_Descriptor.Array_Domain.Is_Contiguous_Data == TRUE) ) &&
              (Where_Statement_Support::Where_Statement_Mask == NULL) ) ||
            Array_Descriptor.Array_Domain.Is_A_Null_Array;
   }

// *****************************************************************
// *****************************************************************
//            inline support functions for the operators
// *****************************************************************
// *****************************************************************

// ***********************************************************************************
// This function handles the case of deleting a temporary that appears on
// the Lhs (which is rare and for the most part does not happen in sane user
// code).  Example: (A+B) = 1;
// But the case is handled anyway.  It used to be a that views are managed similar
// to temporaries - but this was an error (view have to have the scope of the
// scope that builds them).  This is easy to demonstrate in the case where a
// view is passed into a function (the function can't delete the view).
// ***** Come to think of it: this function may be not be useful!  *****
// ***** WRONG! *****
// Even taking the view of a temporary requires the deletion of the temporary -
// so for example: A = (B+C)(I,J) requires that we delete the temporary (B+C)
// this is done within the operator() for each of the different versions of operator().
// It is an obscure point - but it happens in the testcode.C and forced a failure on the SGI
// ***********************************************************************************
extern intSerialArray *Last_Lhs_intSerialArray_Operand;

#if 0
inline void
Delete_Lhs_If_Temporary ( const intSerialArray & Lhs )
   {
  // The Solaris C++ compiler v3.0 can't inline a function with a static variable we get a
  // "sorry not implemented: cannot expand inline function with static Last_Lhs_intSerialArray_Operand" message.
  // So it is a variable with file scope (later we should make it a static data member).

#if !defined(SERIAL_APP)
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
       // printf ("Inside of Delete_Lhs_If_Temporary! \n");
          printf ("################################################################### \n");
          printf ("########### Inside of intSerialArray::Delete_Lhs_If_Temporary! ######### \n");
          printf ("################################################################### \n");
        }
#endif

  // Last_Lhs_intSerialArray_Operand is given file scope! We could make it a static member of the class?
     if (Last_Lhs_intSerialArray_Operand != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Inside of Delete_Lhs_If_Temporary (DELETING THE LAST LHS)! \n");
#endif
       // I don't think this function is ever called called so as to delete anything!
       // printf ("Exiting from Delete_Lhs_If_Temporary ( const intSerialArray & Lhs ) ... \n");
       // exit(1);

       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Last_Lhs_intSerialArray_Operand->decrementReferenceCount();
          if (Last_Lhs_intSerialArray_Operand->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Last_Lhs_intSerialArray_Operand;
       // Last_Lhs_intSerialArray_Operand = NULL;
        }

  // Bug fix (11/30/93) Views have to have local scope else they are
  // deleted too soon when passed by reference to an expression.
  // Since they have no data associated with them they are not
  // a problem with their longer lifetime! (only a few words are
  // required to hold a view where as a temporary has array data
  // associated with it).

  // Last_Lhs_intSerialArray_Operand = (Lhs.Array_Descriptor.Array_Domain.Is_A_Temporary) ? &((intSerialArray &) Lhs) : NULL;

  // This might be a memory leak for P++ since views can be temporaries and must be deleted (sometimes)
     Last_Lhs_intSerialArray_Operand = (Lhs.Array_Descriptor.Array_Domain.Is_A_Temporary && !Lhs.Array_Descriptor.Array_Domain.Is_A_View) ? &((intSerialArray &) Lhs) : NULL;

#else
  // printf ("DELETE LHS IF TEMPORARY turned off! \n");
  // APP_ASSERT(Lhs.Array_Descriptor != NULL);
#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency ("Called from Delete_Lhs_If_Temporary(intSerialArray)");
#endif
#endif
   }
#endif
 
#if 0
inline void
Delete_If_Temporary ( const intSerialArray & X )
   {
  // Constants can never be a temporary - it is the law! 
  // I admit that it seems questionable to delete an object
  // that is passed in by a const ref but mutable types will 
  // make this a more acceptable thing to do when they 
  // appear in C++ (next version -- I think)

#if !defined(SERIAL_APP)
  // These behavior was reverified on (2/26/96) while fixing memory leaks in P++.
  // We only activate the temporary management for the A++ and P++ objects
  // not for the Serial_A++ objects.  This is to aviod the redundent
  // management of P++ arrays and Serial_A++ arrays that are used internally in
  // P++.  The way we avoid this temporary management iof the Serial_A++ objects is
  // to eliminate the body of this function when it is used to generate the
  // Serial_A++ member function

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("################################################################### \n");
          printf ("############# Inside of intSerialArray::Delete_If_Temporary! ########### \n");
          printf ("################################################################### \n");
        }
#endif

  // Bug fix (11/30/93) Views have to have local scope else they are
  // deleted too soon when passed by reference to an expression.
  // Since they have no data associated with them they are not
  // a problem with their longer lifetime! (only a few words are
  // required to hold a view where as a temporary has array data
  // associated with it).

     if (X.isTemporary())
        {
#if COMPILE_DEBUG_STATEMENTS
          if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Inside of Delete_If_Temporary (DELETING intSerialArray object (id=%d)) X.getReferenceCount() = %d \n",
                    X.Array_ID(),X.getReferenceCount());
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          X.decrementReferenceCount();
          if (X.getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete & (intSerialArray &) X;    // Notice that we cast away const here!
        }
#if COMPILE_DEBUG_STATEMENTS
       else
        {
          if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Inside of Delete_If_Temporary (NOT A TEMPORARY intSerialArray object (id=%d)) X.getReferenceCount() = %d \n",
                    X.Array_ID(),X.getReferenceCount());
        }
#endif
#else
  // printf ("DELETE IF TEMPORARY turned off! \n");
  // APP_ASSERT(X.Array_Descriptor != NULL);
#if COMPILE_DEBUG_STATEMENTS
     X.Test_Consistency ("Called from Delete_Lhs_If_Temporary(intSerialArray)");
#endif
#endif
   }
#endif

// End of inlineable functions!
#endif


#undef INTARRAY

#define DOUBLEARRAY
// **********************************************************************
//  Inlined scalar i n d e x operators (this is the only way to make such 
//  things fast).  Though there use is officially discouraged since the  
//  performance is poor!
// **********************************************************************

/* inlining the i n d e x operators */

#if 1
#if !defined(USE_EXPRESSION_TEMPLATES)
// *************************************************************************
// *************************************************************************
//                      INLINED SCALAR OPERATOR()'S
// *************************************************************************
// *************************************************************************

#if HPF_INDEXING
#error Scalar Indexing operators written for non HPF_INDEXING
#endif

inline double & doubleSerialArray::operator() ( int i ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class. The prefered way of indexing array objects is to use the Index class!

//==============================================================================
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
        printf ("Inside of doubleSerialArray::operator() ( int i=%d ) \n",i);
   Test_Consistency();
#endif
//==============================================================================

#if defined(BOUNDS_CHECK)
   // This could be simplified if we just assumed that the first
   // number of entries were valid up to the problem dimension.
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   for (int temp_index=1; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 1);
   int Address_Subscript = 0;

#if 0
#if defined(PPP)
   const int* Base      = getSerialDomain().Base;
   const int* Data_Base = getSerialDomain().Data_Base;
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Base      = Array_Descriptor.Array_Domain.Base;
   const int* Data_Base = Array_Descriptor.Array_Domain.Data_Base;
   const int* Size      = Array_Descriptor.Array_Domain.Size;
   const int* Stride    = Array_Descriptor.Array_Domain.Stride;
#endif
#endif

   int temp;
   if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing)
   {
      // This handles the case of scalar indexing applied to a array object
      // previously indexed using indirect addressing (a view).
      // Since all such views are defined to be 1 dimensional arrays
      // we need only worry about this detail in the scalar opeerator(int) 
      // (taking a single scalar parameter).

           
#if defined(PPP)
      Address_Subscript = 
         ( (getSerialDomain().Index_Array [0] != NULL) ? 
         ((*(getSerialDomain().Index_Array[0]))(i) - 
         getSerialDomain().Data_Base[0]) : 
         getSerialDomain().Base[0] ) * 
         getSerialDomain().Stride [0];
      for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
         Address_Subscript += 
            ( (getSerialDomain().Index_Array [temp] != NULL) ? 
            ((*(getSerialDomain().Index_Array[temp]))(i) - 
            getSerialDomain().Data_Base[temp]) : 
            getSerialDomain().Base[temp] ) * 
            getSerialDomain().Stride [temp] * 
            getSerialDomain().Size[temp-1];
#else
      Address_Subscript = 
         ( (Array_Descriptor.Array_Domain.Index_Array [0] != NULL) ? 
         ((*(Array_Descriptor.Array_Domain.Index_Array[0]))(i) - 
         Array_Descriptor.Array_Domain.Data_Base[0]) : 
         Array_Descriptor.Array_Domain.Base[0] ) * 
         Array_Descriptor.Array_Domain.Stride [0];
      for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
         Address_Subscript += 
            ( (Array_Descriptor.Array_Domain.Index_Array [temp] != NULL) ? 
            ((*(Array_Descriptor.Array_Domain.Index_Array[temp]))(i) - 
            Array_Descriptor.Array_Domain.Data_Base[temp]) : 
            Array_Descriptor.Array_Domain.Base[temp] ) * 
            Array_Descriptor.Array_Domain.Stride [temp] * 
            Array_Descriptor.Array_Domain.Size[temp-1];
#endif
   }
   else
   {
      // This is the more common case (normal indexing using a scalar)
      // We have to use all the dimensions becase we might be accessing
      // a lower dimensional view of a higher dimensional array
#if defined(PPP)
      Address_Subscript = i * getSerialDomain().Stride[0];
#else
      Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0];
#endif
   }

#if defined(PPP)
   APP_ASSERT (usesIndirectAddressing() == FALSE);

   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
      Off_Processor_Including_Ghost_Boundaries =
         ((ii < Local_Data_Base[0])|| 
            (ii > Local_Data_Base[0]+Local_Size[0]-1));

      Off_Processor_Excluding_Ghost_Boundaries =
         ((ii < Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
            (ii > Local_Data_Base[0]+Local_Size[0]-1-
              InternalGhostCellWidth[0]));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
       {
         Scalar_Indexing_For_doubleArray_With_Message_Passing 
              (Address_Subscript+Local_Scalar_Offset[0], 
              Off_Processor_Including_Ghost_Boundaries,
              Off_Processor_Excluding_Ghost_Boundaries, 
              Array_Index_For_double_Variable);
       }
      else
       {
      // Bugfix (1/2/97) We have to reset the Array_Index_For_double_Variable to zero because it
      // in incremented in the code below and otherwize only reset in the function
      // Scalar_Indexing_For_doubleArray_With_Message_Passing.
         APP_ASSERT (Array_Index_For_double_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_double_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_double_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_double_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer0 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[0]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_double_Variable[Array_Index_For_double_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer0 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else

   APP_ASSERT (Array_Descriptor.Array_View_Pointer0 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[0]);
   return Array_Descriptor.Array_View_Pointer0 [Address_Subscript];
#endif
}

#if MAX_ARRAY_DIMENSION >= 2
inline double & doubleSerialArray::operator() ( int i , int j ) const
{
   // Note that the use of a scalar  i n d e x i n g  is not efficient in an array 
   // class the perfered way to  i n d e x  array objects is to use the Index class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Inside of doubleSerialArray::operator() ( int i=%d , int j=%d ) \n",i,j);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   for (int temp_index=2; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
      Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 2);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];

   /*
   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells, Local_Bound is only related to view so size must be
   //  used instead ...
   */

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {

      Off_Processor_Including_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]) ||
           (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
          ((jj<Local_Data_Base[1]) ||
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)));

      Off_Processor_Excluding_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1
              -InternalGhostCellWidth[0])) ||
          ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
              -InternalGhostCellWidth[1])));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_doubleArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[1], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries,
          Array_Index_For_double_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_double_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_double_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_double_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_double_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer1 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[1]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_double_Variable[Array_Index_For_double_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer1 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else

   APP_ASSERT (Array_Descriptor.Array_View_Pointer1 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[1]);
   return Array_Descriptor.Array_View_Pointer1 [Address_Subscript];
#endif
}
#endif 

#if MAX_ARRAY_DIMENSION >= 3
inline double & doubleSerialArray::operator() ( int i , int j , int k ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
     printf ("Inside of doubleSerialArray::operator() ( int i=%d , int j=%d , int k=%d ) \n",
             i,j,k);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   for (int temp_index=3; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
      Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 3);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
     Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...


   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
      Off_Processor_Including_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
          ((jj<Local_Data_Base[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
          ((kk<Local_Data_Base[2]) || 
           (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)));

      Off_Processor_Excluding_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1
              -InternalGhostCellWidth[0]))||
          ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
              -InternalGhostCellWidth[1]))||
          ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
           (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
              -InternalGhostCellWidth[2])));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
       Scalar_Indexing_For_doubleArray_With_Message_Passing 
          (Address_Subscript+Local_Scalar_Offset[2], Off_Processor_Including_Ghost_Boundaries,
           Off_Processor_Excluding_Ghost_Boundaries, 
           Array_Index_For_double_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_double_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_double_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_double_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_double_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer2 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[2]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_double_Variable[Array_Index_For_double_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer2 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer2 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[2]);
   return Array_Descriptor.Array_View_Pointer2 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 4
inline double & doubleSerialArray::operator() ( int i , int j , int k , int l ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
     printf("Inside of doubleSerialArray::operator()(int i=%d,int j=%d,int k=%d,int l=%d)\n",
            i,j,k,l);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   for (int temp_index=4; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 4);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_doubleArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[3], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_double_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_double_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_double_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_double_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_double_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer3 == 
      getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[3]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_double_Variable[Array_Index_For_double_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer3 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer3 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[3]);
   return Array_Descriptor.Array_View_Pointer3 [Address_Subscript];
   //return Array_Data [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 5
inline double & doubleSerialArray::operator() ( int i , int j , int k , int l, int m ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   for (int temp_index=5; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 5);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_doubleArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[4], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_double_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_double_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_double_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_double_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_double_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer4 == 
           getSerialArrayDescriptor().Array_Data + 
           getSerialDomain().Scalar_Offset[4]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_double_Variable[Array_Index_For_double_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer4 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer4 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[4]);
   return Array_Descriptor.Array_View_Pointer4 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 6
inline double & doubleSerialArray::operator() (int i, int j, int k, int l, int m, int n) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   for (int temp_index=6; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 6);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_doubleArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[5], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_double_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_double_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_double_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_double_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_double_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer5 == 
           getSerialArrayDescriptor().Array_Data + 
           getSerialDomain().Scalar_Offset[5]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_double_Variable[Array_Index_For_double_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer5 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer5 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[5]);
   return Array_Descriptor.Array_View_Pointer5 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 7
inline double & doubleSerialArray::operator() (int i, int j, int k, int l, int m, 
                                   int n, int o) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   Scalar_Index_List [6] = &o;
   for (int temp_index=7; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

 //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 7);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4] +
                       o * getSerialDomain().Stride[6] * getSerialDomain().Size[5];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4] +
                       o * Array_Descriptor.Array_Domain.Stride[6] * 
                       Array_Descriptor.Array_Domain.Size[5];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];
   int oo = (o-Local_User_Base[6]) * Local_Stride[6] + Local_Data_Base[6]+
      Local_Base[6];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)) ||
       ((oo<Local_Data_Base[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5]))||
       ((oo<Local_Data_Base[6]+InternalGhostCellWidth[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1
           -InternalGhostCellWidth[6])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_doubleArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[6], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_double_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_double_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_double_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_double_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_double_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer6 == 
            getSerialArrayDescriptor().Array_Data + 
            getSerialDomain().Scalar_Offset[6]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_double_Variable[Array_Index_For_double_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer6 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer6 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[6]);
   return Array_Descriptor.Array_View_Pointer6 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 8
inline double & doubleSerialArray::operator() (int i, int j, int k, int l, int m, 
                                   int n, int o, int p) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   Scalar_Index_List [6] = &o;
   Scalar_Index_List [7] = &p;
   for (int temp_index=8; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 8);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4] +
                       o * getSerialDomain().Stride[6] * getSerialDomain().Size[5] +
                       p * getSerialDomain().Stride[7] * getSerialDomain().Size[6];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4] +
                       o * Array_Descriptor.Array_Domain.Stride[6] * 
                       Array_Descriptor.Array_Domain.Size[5] +
                       p * Array_Descriptor.Array_Domain.Stride[7] * 
                       Array_Descriptor.Array_Domain.Size[6];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];
   int oo = (o-Local_User_Base[6]) * Local_Stride[6] + Local_Data_Base[6]+
      Local_Base[6];
   int pp = (p-Local_User_Base[7]) * Local_Stride[7] + Local_Data_Base[7]+
      Local_Base[7];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)) ||
       ((oo<Local_Data_Base[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1)) ||
       ((pp<Local_Data_Base[7]) || 
        (pp>Local_Data_Base[7]+(Local_Size[7]/Local_Size[6])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5]))||
       ((oo<Local_Data_Base[6]+InternalGhostCellWidth[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1
           -InternalGhostCellWidth[6]))||
       ((pp<Local_Data_Base[7]+InternalGhostCellWidth[7]) || 
        (pp>Local_Data_Base[7]+(Local_Size[7]/Local_Size[6])-1
           -InternalGhostCellWidth[7])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_doubleArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[7], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_double_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_double_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_double_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_double_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_double_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer7 == 
            getSerialArrayDescriptor().Array_Data + 
            getSerialDomain().Scalar_Offset[7]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_double_Variable[Array_Index_For_double_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer7 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer7 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[7]);
   return Array_Descriptor.Array_View_Pointer7 [Address_Subscript];
#endif
}
#endif

// End of if !defined(USE_EXPRESSION_TEMPLATES)
#endif

// Endif for inlining of  i n d e x  operators! 

// ********************************************************************************
// ********************************************************************************
//        INLINING FUNCTIONS SPECIFIC TO A++/P++ (Not user application code)
// ********************************************************************************
// ******************************************************************************** 

#if defined(INTARRAY)
// Use if d e f to force only ONE instance of the dimension function!
// Dimension function is used so much that it should be inlined!

inline int
SerialArray_Domain_Type::getBase( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
  // return Data_Base[Axis] + Base[Axis];
     return User_Base[Axis];
   }

inline int
SerialArray_Domain_Type::getRawBase( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
     return Data_Base[Axis] + Base[Axis];
   }

inline int
SerialArray_Domain_Type::getBound( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
  // return Data_Base[Axis] + Bound[Axis];

     return User_Base[Axis] + (Bound[Axis]-Base[Axis]) / Stride[Axis];
   }

inline int
SerialArray_Domain_Type::getRawBound( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Data_Base[Axis] + Bound[Axis];
   }

inline int
SerialArray_Domain_Type::getStride( int Axis ) const
   {
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
  // return Stride[Axis];
     return 1;
   }

inline int
SerialArray_Domain_Type::getRawStride( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Stride[Axis];
   }

inline Range
SerialArray_Domain_Type::dimension( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Range(getBase(Axis),getBound(Axis),getStride(Axis));
   }

inline Range
SerialArray_Domain_Type::getFullRange( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     int length = Size[Axis];
     if (Axis>0) length /= Size[Axis-1];
  // return Range(Data_Base[Axis],length+Data_Base[Axis]-1,Stride[Axis]);
  // return Array_Domain.getFullRange(Axis);

#if 0
#if defined(PPP)
  // This fails on P++/TESTS/test2000_02.C (as I think it should)
     APP_ASSERT (Size[Axis]      == Local_Mask_Index[Axis].getLength());
     APP_ASSERT (Data_Base[Axis] == Local_Mask_Index[Axis].getBase());
     APP_ASSERT (Data_Base[Axis]+Size[Axis] == Local_Mask_Index[Axis].getBound());
     APP_ASSERT (1 == Local_Mask_Index[Axis].getStride());
#endif
#endif

  // (4/29/2000) Fixed bug in stride (which should always be 1 for the full range)
  // return Range(Data_Base[Axis],length+Data_Base[Axis]-1,Stride[Axis]);
     return Range(Data_Base[Axis],length+Data_Base[Axis]-1,1);
   }

inline int
SerialArray_Domain_Type::getLength( int Axis ) const
   {
  // Computes the length of any axis of the array object using this descriptor!
  // This function should be inlined for better efficency!
  // This could be made a little more efficient by avioding the division by the
  // stride in the most common case where the unit stride.
  // Also in the commmon case where Is_A_View == FALSE we could just return the Size[Axis].
 
  // int Temp_Size = (Bound [Axis] - Base [Axis]) + 1;
  // return ( Temp_Size % Stride [Axis] == 0 ) ? Temp_Size / Stride [Axis] : Temp_Size / Stride [Axis] + 1;

     int Temp_Size = 0;
  // Bugfix (11/1/95) Previously did not handle case of Uses_Indirect_Addressing == TRUE
     if (Uses_Indirect_Addressing)
        {
       // Bugfix (11/2/95) Only return Array_Size for case of querey along Axis == 0
          Temp_Size = (Axis == 0) ? Array_Size() : 1;
        }
       else
        {
          Temp_Size = (Bound [Axis] - Base [Axis]) + 1;
          if ( Temp_Size % Stride [Axis] == 0 ) 
               Temp_Size /= Stride [Axis];
            else
               Temp_Size = Temp_Size / Stride [Axis] + 1;
        }

     return Temp_Size;
   }

inline int
SerialArray_Domain_Type::maxNumberOfDimensions()
   {
  // Get static upper bound on number of dimensions of the array objects
     return MAX_ARRAY_DIMENSION;
   }

inline int
SerialArray_Domain_Type::numberOfDimensions() const
   {
  // Get number of dimensions of the array objects (note that a 1x2 array is 2 dimensional)
     return Domain_Dimension;
   }

inline int
SerialArray_Domain_Type::internalNumberOfDimensions() const
   {
  // For array objects not using indirect addressing we use the 
  // current value of Descriptor_Dimension.  But for array objects
  // using indirect addressing we return the number of intArrays in
  // use to discribe the indirect addressing internally.

     int Return_Value = -1;
     if (usesIndirectAddressing() == TRUE)
        {
       // for (int i= MAX_ARRAY_DIMENSION-1; i >= 0; i--)
          int i = MAX_ARRAY_DIMENSION-1;
          while (Return_Value == -1)
             {
               APP_ASSERT (i >= 0);
            // if (Index_Array[i] != NULL)
            // We want the larges value of the axis that is used accounting for the
            // way that Internal_Index objects and intArray objects can be mixed in the indexing.
            // if ( (Index_Array[i] != NULL) || (Base[i] != Bound[i]) )
            // Checking for the base not equal to zero allows us to 
            // detect scalar indexing in the higher dimensions!
               if ( (Index_Array[i] != NULL) || (Base[i] != 0) )
                  Return_Value = i+1;
               i--;
             }
          APP_ASSERT (Return_Value > 0);
        }
       else
        {
          Return_Value = Domain_Dimension;
        }
     return Return_Value;
   }

// End of if defined(INTARRAY)
#endif

inline double* APP_RESTRICT_MACRO
doubleSerialArray::getDataPointer () const
   {
     return Array_Descriptor.getDataPointer();
   }

#if defined(PPP)
inline doubleSerialArray*
doubleSerialArray::getSerialArrayPointer () const
   {
  // return pointer to A++ array
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     return Array_Descriptor.SerialArray;
   }
inline doubleSerialArray**
doubleSerialArray::getSerialArrayPointerLoc () const
   {
  // return pointer to A++ array address
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     return (doubleSerialArray**)&Array_Descriptor.SerialArray;
   }
#endif

// Bugfix (1/13/97) moved this function from the collection
// of local access functions below.  The problem was that it 
// was not being found at link time.
inline Range
doubleSerialArray::getFullRange (int Axis) const
   {
      return Array_Descriptor.getFullRange(Axis);
   }

// This is defined for APP SERIAL_APP and PPP
inline Range
doubleSerialArray::getLocalFullRange (int Axis) const
   {
#if defined(PPP)
      return getSerialArrayDescriptor().getFullRange(Axis);
#else
      return getFullRange(Axis);
#endif
   }

#if defined(PPP) || defined(APP)
// These are acces functions to local parts of the P++ array.  They must exist in A++
// as well - though they return the global data if used in A++ since that is what local means
// in a serial environment.

#if defined(PPP) 
inline doubleSerialArray doubleSerialArray::getLocalArray () const
#else
inline doubleArray doubleSerialArray::getLocalArray () const
#endif
   {
  // return reference to A++ array

#if defined(PPP)
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);
     return doubleSerialArray(*Array_Descriptor.SerialArray,SHALLOWCOPY);
  // printf("getLocalArray doesn't work in parallel right now\n");
#else
     return doubleArray(*this,SHALLOWCOPY);
#endif
   }

// Must be made available in PPP - SERIAL_APP - APP
// inline Range doubleSerialArray::getFullRange (int Axis) const
//    {
//       return Array_Descriptor.getFullRange(Axis);
//    }

#if 0
// This function was placed into array.C file (11/29/2000)
#if defined(PPP)
inline doubleSerialArray
doubleSerialArray::getLocalArrayWithGhostBoundaries () const
#else
inline doubleArray
doubleSerialArray::getLocalArrayWithGhostBoundaries () const
#endif
   {
  // return A++ array that includes ghost cells if P++ or a shallow copy 
  // of itself if A++

  // APP_ASSERT(!Array_Descriptor.Is_A_View);

#if defined(PPP) 
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     if ((!Array_Descriptor.Array_Domain.Is_A_View) && 
         (!Array_Descriptor.Array_Domain.Is_A_Null_Array) &&
         (!getSerialDomain().Is_A_Null_Array))
     {
        Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
        int i;
        for (i=0;i<MAX_ARRAY_DIMENSION;i++)
           {
             Internal_Index_List[i] = new Range(getLocalFullRange(i));
             APP_ASSERT (Internal_Index_List[i] != NULL);
           }
        
        doubleSerialArray ReturnArray;
        ReturnArray.adopt(getDataPointer(),Internal_Index_List);
        for (i=0;i<MAX_ARRAY_DIMENSION;i++) 
           {
          // Range objects don't have referenceCounting! (should they?)
             delete Internal_Index_List[i];
           }
        return doubleSerialArray(ReturnArray,SHALLOWCOPY);
     }
     else
     {
        return doubleSerialArray(*Array_Descriptor.SerialArray,SHALLOWCOPY);
     }
     //printf("getLocalArrayWithGhostBoundaries doesn't work in parallel right now\n");
#else
     return doubleArray(*this,SHALLOWCOPY);
#endif
   }
#endif

// end of defined(PPP) || defined(APP)
#endif

// Endif for function inlining
#endif


// We needed to place this below the Allocate_Array_Data function because
// The expression template constructor needed to see it at compile time.
// if defined(INLINE_APP1_FUNCTIONS)

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
// forward declaration
// typedef struct Array_Domain_c     array_domain;
// typedef struct Array_Descriptor_c array_descriptor;

#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" double* MDI_double_Allocate ( array_domain* Descriptor );
#endif

#if defined(INTARRAY)
// error "Inside of inline_func.h"
#endif

//inline void doubleSerialArray::Allocate_Array_Data ( bool Force_Memory_Allocation ) const
inline
void
doubleSerialArray::Allocate_Array_Data ( bool Force_Memory_Allocation ) 
   {
  // This function allocates the internal data for the doubleSerialArray object.  In A++
  // this allocates the the raw double data array.  In P++ it allocates the internal
  // A++ array for the current processor (using the sizes defined in the DARRAY
  // parallel descriptor).

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("*** Allocating the Array_Data (or getting it from the hash table)! \n");
#endif  

#if defined(PPP)
  // The allocation of data in the parallel environment is especially complicated
  // so this is broken out as a separate function and is implemented only for P++.
     Array_Descriptor.Allocate_Parallel_Array (Force_Memory_Allocation);
#else
  // To improve the implementation we have moved this functionality into the
  // Array_Descriptor_Type object because that is where the pointers to the
  // Array_Data (or Serial_Array_Data_Pointer in the case of P++) now live.
     APP_ASSERT (getDataPointer() == NULL);
     Array_Descriptor.Allocate_Array_Data(Force_Memory_Allocation);
#endif  // if !defined(PPP)

#if !defined(PPP)
  // Since this is a new array object is should have an initialize reference count on its
  // raw data.  This is required here because the reference counting mechanism reused the
  // value of zero for one existing reference and no references (this will be fixed soon).
     resetRawDataReferenceCount();
#endif
   }          

// We needed to place this below the Allocate_Array_Data function because
// The expression template constructor needed to see it at compile time.
#if defined(INLINE_APP1_FUNCTIONS)

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" void MDI_double_Deallocate ( double* Data_Pointer , array_domain* Descriptor );
#endif
inline
void
doubleSerialArray::Delete_Array_Data ()
   {
  // Main body of code moved to Array_Descriptor_Type
  // To improve the implementation we have moved this functionality into the
  // Array_Descriptor_Type object because that is where the pointers to the
  // Array_Data (or Serial_Array_Data_Pointer in the case of P++) now live.
     Array_Descriptor.Delete_Array_Data();
   }

// ***********************************************************************
// Part of error checking is to verify binary conformability (i.e. the
// array operations between arrays of the same size IN EACH DIMENSION).
// ***********************************************************************
inline bool
doubleSerialArray::Binary_Conformable ( const doubleSerialArray & X ) const
   {
  // We must defin e binary conformable to exclude the use of the where statement
     return ( ( (Array_Descriptor.Array_Domain.Is_Contiguous_Data == TRUE)     &&
                (X.Array_Descriptor.Array_Domain.Is_Contiguous_Data == TRUE) ) &&
              (Where_Statement_Support::Where_Statement_Mask == NULL) ) ||
            Array_Descriptor.Array_Domain.Is_A_Null_Array;
   }

// *****************************************************************
// *****************************************************************
//            inline support functions for the operators
// *****************************************************************
// *****************************************************************

// ***********************************************************************************
// This function handles the case of deleting a temporary that appears on
// the Lhs (which is rare and for the most part does not happen in sane user
// code).  Example: (A+B) = 1;
// But the case is handled anyway.  It used to be a that views are managed similar
// to temporaries - but this was an error (view have to have the scope of the
// scope that builds them).  This is easy to demonstrate in the case where a
// view is passed into a function (the function can't delete the view).
// ***** Come to think of it: this function may be not be useful!  *****
// ***** WRONG! *****
// Even taking the view of a temporary requires the deletion of the temporary -
// so for example: A = (B+C)(I,J) requires that we delete the temporary (B+C)
// this is done within the operator() for each of the different versions of operator().
// It is an obscure point - but it happens in the testcode.C and forced a failure on the SGI
// ***********************************************************************************
extern doubleSerialArray *Last_Lhs_doubleSerialArray_Operand;

#if 0
inline void
Delete_Lhs_If_Temporary ( const doubleSerialArray & Lhs )
   {
  // The Solaris C++ compiler v3.0 can't inline a function with a static variable we get a
  // "sorry not implemented: cannot expand inline function with static Last_Lhs_doubleSerialArray_Operand" message.
  // So it is a variable with file scope (later we should make it a static data member).

#if !defined(SERIAL_APP)
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
       // printf ("Inside of Delete_Lhs_If_Temporary! \n");
          printf ("################################################################### \n");
          printf ("########### Inside of doubleSerialArray::Delete_Lhs_If_Temporary! ######### \n");
          printf ("################################################################### \n");
        }
#endif

  // Last_Lhs_doubleSerialArray_Operand is given file scope! We could make it a static member of the class?
     if (Last_Lhs_doubleSerialArray_Operand != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Inside of Delete_Lhs_If_Temporary (DELETING THE LAST LHS)! \n");
#endif
       // I don't think this function is ever called called so as to delete anything!
       // printf ("Exiting from Delete_Lhs_If_Temporary ( const doubleSerialArray & Lhs ) ... \n");
       // exit(1);

       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Last_Lhs_doubleSerialArray_Operand->decrementReferenceCount();
          if (Last_Lhs_doubleSerialArray_Operand->getReferenceCount() < doubleSerialArray::getReferenceCountBase())
               delete Last_Lhs_doubleSerialArray_Operand;
       // Last_Lhs_doubleSerialArray_Operand = NULL;
        }

  // Bug fix (11/30/93) Views have to have local scope else they are
  // deleted too soon when passed by reference to an expression.
  // Since they have no data associated with them they are not
  // a problem with their longer lifetime! (only a few words are
  // required to hold a view where as a temporary has array data
  // associated with it).

  // Last_Lhs_doubleSerialArray_Operand = (Lhs.Array_Descriptor.Array_Domain.Is_A_Temporary) ? &((doubleSerialArray &) Lhs) : NULL;

  // This might be a memory leak for P++ since views can be temporaries and must be deleted (sometimes)
     Last_Lhs_doubleSerialArray_Operand = (Lhs.Array_Descriptor.Array_Domain.Is_A_Temporary && !Lhs.Array_Descriptor.Array_Domain.Is_A_View) ? &((doubleSerialArray &) Lhs) : NULL;

#else
  // printf ("DELETE LHS IF TEMPORARY turned off! \n");
  // APP_ASSERT(Lhs.Array_Descriptor != NULL);
#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency ("Called from Delete_Lhs_If_Temporary(doubleSerialArray)");
#endif
#endif
   }
#endif
 
#if 0
inline void
Delete_If_Temporary ( const doubleSerialArray & X )
   {
  // Constants can never be a temporary - it is the law! 
  // I admit that it seems questionable to delete an object
  // that is passed in by a const ref but mutable types will 
  // make this a more acceptable thing to do when they 
  // appear in C++ (next version -- I think)

#if !defined(SERIAL_APP)
  // These behavior was reverified on (2/26/96) while fixing memory leaks in P++.
  // We only activate the temporary management for the A++ and P++ objects
  // not for the Serial_A++ objects.  This is to aviod the redundent
  // management of P++ arrays and Serial_A++ arrays that are used internally in
  // P++.  The way we avoid this temporary management iof the Serial_A++ objects is
  // to eliminate the body of this function when it is used to generate the
  // Serial_A++ member function

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("################################################################### \n");
          printf ("############# Inside of doubleSerialArray::Delete_If_Temporary! ########### \n");
          printf ("################################################################### \n");
        }
#endif

  // Bug fix (11/30/93) Views have to have local scope else they are
  // deleted too soon when passed by reference to an expression.
  // Since they have no data associated with them they are not
  // a problem with their longer lifetime! (only a few words are
  // required to hold a view where as a temporary has array data
  // associated with it).

     if (X.isTemporary())
        {
#if COMPILE_DEBUG_STATEMENTS
          if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Inside of Delete_If_Temporary (DELETING doubleSerialArray object (id=%d)) X.getReferenceCount() = %d \n",
                    X.Array_ID(),X.getReferenceCount());
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          X.decrementReferenceCount();
          if (X.getReferenceCount() < doubleSerialArray::getReferenceCountBase())
               delete & (doubleSerialArray &) X;    // Notice that we cast away const here!
        }
#if COMPILE_DEBUG_STATEMENTS
       else
        {
          if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Inside of Delete_If_Temporary (NOT A TEMPORARY doubleSerialArray object (id=%d)) X.getReferenceCount() = %d \n",
                    X.Array_ID(),X.getReferenceCount());
        }
#endif
#else
  // printf ("DELETE IF TEMPORARY turned off! \n");
  // APP_ASSERT(X.Array_Descriptor != NULL);
#if COMPILE_DEBUG_STATEMENTS
     X.Test_Consistency ("Called from Delete_Lhs_If_Temporary(doubleSerialArray)");
#endif
#endif
   }
#endif

// End of inlineable functions!
#endif


#undef DOUBLEARRAY

#define FLOATARRAY
// **********************************************************************
//  Inlined scalar i n d e x operators (this is the only way to make such 
//  things fast).  Though there use is officially discouraged since the  
//  performance is poor!
// **********************************************************************

/* inlining the i n d e x operators */

#if 1
#if !defined(USE_EXPRESSION_TEMPLATES)
// *************************************************************************
// *************************************************************************
//                      INLINED SCALAR OPERATOR()'S
// *************************************************************************
// *************************************************************************

#if HPF_INDEXING
#error Scalar Indexing operators written for non HPF_INDEXING
#endif

inline float & floatSerialArray::operator() ( int i ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class. The prefered way of indexing array objects is to use the Index class!

//==============================================================================
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
        printf ("Inside of floatSerialArray::operator() ( int i=%d ) \n",i);
   Test_Consistency();
#endif
//==============================================================================

#if defined(BOUNDS_CHECK)
   // This could be simplified if we just assumed that the first
   // number of entries were valid up to the problem dimension.
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   for (int temp_index=1; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 1);
   int Address_Subscript = 0;

#if 0
#if defined(PPP)
   const int* Base      = getSerialDomain().Base;
   const int* Data_Base = getSerialDomain().Data_Base;
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Base      = Array_Descriptor.Array_Domain.Base;
   const int* Data_Base = Array_Descriptor.Array_Domain.Data_Base;
   const int* Size      = Array_Descriptor.Array_Domain.Size;
   const int* Stride    = Array_Descriptor.Array_Domain.Stride;
#endif
#endif

   int temp;
   if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing)
   {
      // This handles the case of scalar indexing applied to a array object
      // previously indexed using indirect addressing (a view).
      // Since all such views are defined to be 1 dimensional arrays
      // we need only worry about this detail in the scalar opeerator(int) 
      // (taking a single scalar parameter).

           
#if defined(PPP)
      Address_Subscript = 
         ( (getSerialDomain().Index_Array [0] != NULL) ? 
         ((*(getSerialDomain().Index_Array[0]))(i) - 
         getSerialDomain().Data_Base[0]) : 
         getSerialDomain().Base[0] ) * 
         getSerialDomain().Stride [0];
      for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
         Address_Subscript += 
            ( (getSerialDomain().Index_Array [temp] != NULL) ? 
            ((*(getSerialDomain().Index_Array[temp]))(i) - 
            getSerialDomain().Data_Base[temp]) : 
            getSerialDomain().Base[temp] ) * 
            getSerialDomain().Stride [temp] * 
            getSerialDomain().Size[temp-1];
#else
      Address_Subscript = 
         ( (Array_Descriptor.Array_Domain.Index_Array [0] != NULL) ? 
         ((*(Array_Descriptor.Array_Domain.Index_Array[0]))(i) - 
         Array_Descriptor.Array_Domain.Data_Base[0]) : 
         Array_Descriptor.Array_Domain.Base[0] ) * 
         Array_Descriptor.Array_Domain.Stride [0];
      for (temp=1; temp < MAX_ARRAY_DIMENSION; temp++)
         Address_Subscript += 
            ( (Array_Descriptor.Array_Domain.Index_Array [temp] != NULL) ? 
            ((*(Array_Descriptor.Array_Domain.Index_Array[temp]))(i) - 
            Array_Descriptor.Array_Domain.Data_Base[temp]) : 
            Array_Descriptor.Array_Domain.Base[temp] ) * 
            Array_Descriptor.Array_Domain.Stride [temp] * 
            Array_Descriptor.Array_Domain.Size[temp-1];
#endif
   }
   else
   {
      // This is the more common case (normal indexing using a scalar)
      // We have to use all the dimensions becase we might be accessing
      // a lower dimensional view of a higher dimensional array
#if defined(PPP)
      Address_Subscript = i * getSerialDomain().Stride[0];
#else
      Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0];
#endif
   }

#if defined(PPP)
   APP_ASSERT (usesIndirectAddressing() == FALSE);

   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
      Off_Processor_Including_Ghost_Boundaries =
         ((ii < Local_Data_Base[0])|| 
            (ii > Local_Data_Base[0]+Local_Size[0]-1));

      Off_Processor_Excluding_Ghost_Boundaries =
         ((ii < Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
            (ii > Local_Data_Base[0]+Local_Size[0]-1-
              InternalGhostCellWidth[0]));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
       {
         Scalar_Indexing_For_floatArray_With_Message_Passing 
              (Address_Subscript+Local_Scalar_Offset[0], 
              Off_Processor_Including_Ghost_Boundaries,
              Off_Processor_Excluding_Ghost_Boundaries, 
              Array_Index_For_float_Variable);
       }
      else
       {
      // Bugfix (1/2/97) We have to reset the Array_Index_For_float_Variable to zero because it
      // in incremented in the code below and otherwize only reset in the function
      // Scalar_Indexing_For_floatArray_With_Message_Passing.
         APP_ASSERT (Array_Index_For_float_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_float_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_float_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_float_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer0 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[0]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_float_Variable[Array_Index_For_float_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer0 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else

   APP_ASSERT (Array_Descriptor.Array_View_Pointer0 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[0]);
   return Array_Descriptor.Array_View_Pointer0 [Address_Subscript];
#endif
}

#if MAX_ARRAY_DIMENSION >= 2
inline float & floatSerialArray::operator() ( int i , int j ) const
{
   // Note that the use of a scalar  i n d e x i n g  is not efficient in an array 
   // class the perfered way to  i n d e x  array objects is to use the Index class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Inside of floatSerialArray::operator() ( int i=%d , int j=%d ) \n",i,j);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   for (int temp_index=2; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
      Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 2);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];

   /*
   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells, Local_Bound is only related to view so size must be
   //  used instead ...
   */

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {

      Off_Processor_Including_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]) ||
           (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
          ((jj<Local_Data_Base[1]) ||
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)));

      Off_Processor_Excluding_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1
              -InternalGhostCellWidth[0])) ||
          ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
              -InternalGhostCellWidth[1])));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_floatArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[1], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries,
          Array_Index_For_float_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_float_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_float_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_float_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_float_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer1 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[1]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_float_Variable[Array_Index_For_float_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer1 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else

   APP_ASSERT (Array_Descriptor.Array_View_Pointer1 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[1]);
   return Array_Descriptor.Array_View_Pointer1 [Address_Subscript];
#endif
}
#endif 

#if MAX_ARRAY_DIMENSION >= 3
inline float & floatSerialArray::operator() ( int i , int j , int k ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
     printf ("Inside of floatSerialArray::operator() ( int i=%d , int j=%d , int k=%d ) \n",
             i,j,k);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   for (int temp_index=3; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
      Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 3);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
     Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...


   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
      Off_Processor_Including_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
          ((jj<Local_Data_Base[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
          ((kk<Local_Data_Base[2]) || 
           (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)));

      Off_Processor_Excluding_Ghost_Boundaries =
         (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
           (ii>Local_Data_Base[0]+Local_Size[0]-1
              -InternalGhostCellWidth[0]))||
          ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
           (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
              -InternalGhostCellWidth[1]))||
          ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
           (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
              -InternalGhostCellWidth[2])));
   }

   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
       Scalar_Indexing_For_floatArray_With_Message_Passing 
          (Address_Subscript+Local_Scalar_Offset[2], Off_Processor_Including_Ghost_Boundaries,
           Off_Processor_Excluding_Ghost_Boundaries, 
           Array_Index_For_float_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_float_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_float_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_float_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_float_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer2 == 
       getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[2]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_float_Variable[Array_Index_For_float_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer2 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer2 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[2]);
   return Array_Descriptor.Array_View_Pointer2 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 4
inline float & floatSerialArray::operator() ( int i , int j , int k , int l ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
     printf("Inside of floatSerialArray::operator()(int i=%d,int j=%d,int k=%d,int l=%d)\n",
            i,j,k,l);
   Test_Consistency();
#endif

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   for (int temp_index=4; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 4);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_floatArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[3], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_float_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_float_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_float_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_float_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_float_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer3 == 
      getSerialArrayDescriptor().Array_Data + getSerialDomain().Scalar_Offset[3]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_float_Variable[Array_Index_For_float_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer3 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer3 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[3]);
   return Array_Descriptor.Array_View_Pointer3 [Address_Subscript];
   //return Array_Data [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 5
inline float & floatSerialArray::operator() ( int i , int j , int k , int l, int m ) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   for (int temp_index=5; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 5);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];


   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_floatArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[4], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_float_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_float_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_float_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_float_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_float_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer4 == 
           getSerialArrayDescriptor().Array_Data + 
           getSerialDomain().Scalar_Offset[4]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_float_Variable[Array_Index_For_float_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer4 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer4 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[4]);
   return Array_Descriptor.Array_View_Pointer4 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 6
inline float & floatSerialArray::operator() (int i, int j, int k, int l, int m, int n) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   for (int temp_index=6; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 6);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_floatArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[5], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_float_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_float_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_float_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_float_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_float_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer5 == 
           getSerialArrayDescriptor().Array_Data + 
           getSerialDomain().Scalar_Offset[5]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_float_Variable[Array_Index_For_float_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer5 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer5 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[5]);
   return Array_Descriptor.Array_View_Pointer5 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 7
inline float & floatSerialArray::operator() (int i, int j, int k, int l, int m, 
                                   int n, int o) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   Scalar_Index_List [6] = &o;
   for (int temp_index=7; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

 //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 7);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4] +
                       o * getSerialDomain().Stride[6] * getSerialDomain().Size[5];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4] +
                       o * Array_Descriptor.Array_Domain.Stride[6] * 
                       Array_Descriptor.Array_Domain.Size[5];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];
   int oo = (o-Local_User_Base[6]) * Local_Stride[6] + Local_Data_Base[6]+
      Local_Base[6];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)) ||
       ((oo<Local_Data_Base[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5]))||
       ((oo<Local_Data_Base[6]+InternalGhostCellWidth[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1
           -InternalGhostCellWidth[6])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_floatArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[6], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_float_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_float_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_float_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_float_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_float_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer6 == 
            getSerialArrayDescriptor().Array_Data + 
            getSerialDomain().Scalar_Offset[6]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_float_Variable[Array_Index_For_float_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer6 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer6 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[6]);
   return Array_Descriptor.Array_View_Pointer6 [Address_Subscript];
#endif
}
#endif

#if MAX_ARRAY_DIMENSION >= 8
inline float & floatSerialArray::operator() (int i, int j, int k, int l, int m, 
                                   int n, int o, int p) const
{
   // Note that the use of an scalar  i n d e x i n g  is not efficient in an array 
   // class.  The perfered way to  i n d e x  array objects is to use the Index 
   // class!

   APP_ASSERT (usesIndirectAddressing() == FALSE);

#if defined(BOUNDS_CHECK)
   Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Scalar_Index_List;
   Scalar_Index_List [0] = &i;
   Scalar_Index_List [1] = &j;
   Scalar_Index_List [2] = &k;
   Scalar_Index_List [3] = &l;
   Scalar_Index_List [4] = &m;
   Scalar_Index_List [5] = &n;
   Scalar_Index_List [6] = &o;
   Scalar_Index_List [7] = &p;
   for (int temp_index=8; temp_index < MAX_ARRAY_DIMENSION; temp_index++)
        Scalar_Index_List[temp_index] = NULL;
   Array_Descriptor.Error_Checking_For_Scalar_Index_Operators ( Scalar_Index_List );
#endif

#if 0
#if defined(PPP)
   const int* Size      = getSerialDomain().Size;
   const int* Stride    = getSerialDomain().Stride;
#else
   const int* Size      = Array_Descriptor.Size;
   const int* Stride    = Array_Descriptor.Stride;
#endif
#endif

   //APP_ASSERT(Array_Descriptor.Descriptor_Dimension == 8);
   int Address_Subscript = 0;
#if defined(PPP)
   Address_Subscript = i * getSerialDomain().Stride[0] + 
                       j * getSerialDomain().Stride[1] * getSerialDomain().Size[0] +
                       k * getSerialDomain().Stride[2] * getSerialDomain().Size[1] +
                       l * getSerialDomain().Stride[3] * getSerialDomain().Size[2] +
                       m * getSerialDomain().Stride[4] * getSerialDomain().Size[3] +
                       n * getSerialDomain().Stride[5] * getSerialDomain().Size[4] +
                       o * getSerialDomain().Stride[6] * getSerialDomain().Size[5] +
                       p * getSerialDomain().Stride[7] * getSerialDomain().Size[6];
#else
   Address_Subscript = i * Array_Descriptor.Array_Domain.Stride[0] + 
                       j * Array_Descriptor.Array_Domain.Stride[1] * 
                       Array_Descriptor.Array_Domain.Size[0] +
                       k * Array_Descriptor.Array_Domain.Stride[2] * 
                       Array_Descriptor.Array_Domain.Size[1] +
                       l * Array_Descriptor.Array_Domain.Stride[3] * 
                       Array_Descriptor.Array_Domain.Size[2] +
                       m * Array_Descriptor.Array_Domain.Stride[4] * 
                       Array_Descriptor.Array_Domain.Size[3] +
                       n * Array_Descriptor.Array_Domain.Stride[5] * 
                       Array_Descriptor.Array_Domain.Size[4] +
                       o * Array_Descriptor.Array_Domain.Stride[6] * 
                       Array_Descriptor.Array_Domain.Size[5] +
                       p * Array_Descriptor.Array_Domain.Stride[7] * 
                       Array_Descriptor.Array_Domain.Size[6];
#endif

#if defined(PPP)
   // The determination of a value being off processor or not should INCLUDE ghost 
   // boundaries so that assignment to a value accessed via scalar indexing is 
   // updated on it's ghost boundaries too.  But then only the one processor should 
   // do the broadcast.
   // Bugfix (10/18/95) The bound of the SerialArray already includes the 
   // InternalGhostCellWidth for that axis!
   const int *InternalGhostCellWidth = 
      Array_Descriptor.Array_Domain.InternalGhostCellWidth;
   int *Local_Data_Base = getSerialDomain().Data_Base;
   int *Local_Base      = getSerialDomain().Base;
   //int *Local_Bound     = getSerialDomain().Bound;
   // ... need this too ...
   int *Local_Size          = getSerialDomain().Size;
   int *Local_Scalar_Offset = getSerialDomain().Scalar_Offset;
   int *Local_User_Base = getSerialDomain().User_Base;
   int *Local_Stride = getSerialDomain().Stride;

   // ... subscripts don't give real location since the stride for user is
   //  1 but we need the real location here ...
   int ii = (i-Local_User_Base[0]) * Local_Stride[0] + Local_Data_Base[0]+
      Local_Base[0];
   int jj = (j-Local_User_Base[1]) * Local_Stride[1] + Local_Data_Base[1]+
      Local_Base[1];
   int kk = (k-Local_User_Base[2]) * Local_Stride[2] + Local_Data_Base[2]+
      Local_Base[2];
   int ll = (l-Local_User_Base[3]) * Local_Stride[3] + Local_Data_Base[3]+
      Local_Base[3];
   int mm = (m-Local_User_Base[4]) * Local_Stride[4] + Local_Data_Base[4]+
      Local_Base[4];
   int nn = (n-Local_User_Base[5]) * Local_Stride[5] + Local_Data_Base[5]+
      Local_Base[5];
   int oo = (o-Local_User_Base[6]) * Local_Stride[6] + Local_Data_Base[6]+
      Local_Base[6];
   int pp = (p-Local_User_Base[7]) * Local_Stride[7] + Local_Data_Base[7]+
      Local_Base[7];

   // ... (bug fix, kdb, 6/4/96) Local_Data_Base already includes
   //  ghost cells Local_Bound is only related to view so size must be
   //  used instead ...

   bool Off_Processor_Including_Ghost_Boundaries = TRUE;
   bool Off_Processor_Excluding_Ghost_Boundaries = TRUE;

   if (!getSerialDomain().Is_A_Null_Array)
   {
     Off_Processor_Including_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]) ||
        (ii>Local_Data_Base[0]+Local_Size[0]-1)) ||
       ((jj<Local_Data_Base[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1)) ||
       ((kk<Local_Data_Base[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1)) ||
       ((ll<Local_Data_Base[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1)) ||
       ((mm<Local_Data_Base[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1)) ||
       ((nn<Local_Data_Base[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1)) ||
       ((oo<Local_Data_Base[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1)) ||
       ((pp<Local_Data_Base[7]) || 
        (pp>Local_Data_Base[7]+(Local_Size[7]/Local_Size[6])-1)));

     Off_Processor_Excluding_Ghost_Boundaries =
      (((ii<Local_Data_Base[0]+InternalGhostCellWidth[0]) || 
        (ii>Local_Data_Base[0]+Local_Size[0]-1
           -InternalGhostCellWidth[0]))||
       ((jj<Local_Data_Base[1]+InternalGhostCellWidth[1]) || 
        (jj>Local_Data_Base[1]+(Local_Size[1]/Local_Size[0])-1
           -InternalGhostCellWidth[1]))||
       ((kk<Local_Data_Base[2]+InternalGhostCellWidth[2]) || 
        (kk>Local_Data_Base[2]+(Local_Size[2]/Local_Size[1])-1
           -InternalGhostCellWidth[2]))||
       ((ll<Local_Data_Base[3]+InternalGhostCellWidth[3]) || 
        (ll>Local_Data_Base[3]+(Local_Size[3]/Local_Size[2])-1
           -InternalGhostCellWidth[3]))||
       ((mm<Local_Data_Base[4]+InternalGhostCellWidth[4]) || 
        (mm>Local_Data_Base[4]+(Local_Size[4]/Local_Size[3])-1
           -InternalGhostCellWidth[4]))||
       ((nn<Local_Data_Base[5]+InternalGhostCellWidth[5]) || 
        (nn>Local_Data_Base[5]+(Local_Size[5]/Local_Size[4])-1
           -InternalGhostCellWidth[5]))||
       ((oo<Local_Data_Base[6]+InternalGhostCellWidth[6]) || 
        (oo>Local_Data_Base[6]+(Local_Size[6]/Local_Size[5])-1
           -InternalGhostCellWidth[6]))||
       ((pp<Local_Data_Base[7]+InternalGhostCellWidth[7]) || 
        (pp>Local_Data_Base[7]+(Local_Size[7]/Local_Size[6])-1
           -InternalGhostCellWidth[7])));
   }


   if (Optimization_Manager::Optimize_Scalar_Indexing == FALSE)
      Scalar_Indexing_For_floatArray_With_Message_Passing 
         (Address_Subscript+Local_Scalar_Offset[7], 
          Off_Processor_Including_Ghost_Boundaries,
          Off_Processor_Excluding_Ghost_Boundaries, 
          Array_Index_For_float_Variable);
      else
       {
         APP_ASSERT (Array_Index_For_float_Variable <= STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);
         if (Array_Index_For_float_Variable == STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES)
              Array_Index_For_float_Variable = 0;
       }

   APP_ASSERT (Array_Index_For_float_Variable < STATIC_LIMIT_FOR_PARALLEL_SCALAR_INDEXING_REFERENCES);

   APP_ASSERT (getSerialArrayDescriptor().Array_View_Pointer7 == 
            getSerialArrayDescriptor().Array_Data + 
            getSerialDomain().Scalar_Offset[7]);
   return (Off_Processor_Including_Ghost_Boundaries) ? 
           Static_float_Variable[Array_Index_For_float_Variable++] : 
           getSerialArrayDescriptor().Array_View_Pointer7 [Address_Subscript];
           //getSerialArrayDescriptor().Array_Data [Address_Subscript];
#else
   APP_ASSERT (Array_Descriptor.Array_View_Pointer7 == 
               Array_Descriptor.Array_Data + Array_Descriptor.Array_Domain.Scalar_Offset[7]);
   return Array_Descriptor.Array_View_Pointer7 [Address_Subscript];
#endif
}
#endif

// End of if !defined(USE_EXPRESSION_TEMPLATES)
#endif

// Endif for inlining of  i n d e x  operators! 

// ********************************************************************************
// ********************************************************************************
//        INLINING FUNCTIONS SPECIFIC TO A++/P++ (Not user application code)
// ********************************************************************************
// ******************************************************************************** 

#if defined(INTARRAY)
// Use if d e f to force only ONE instance of the dimension function!
// Dimension function is used so much that it should be inlined!

inline int
SerialArray_Domain_Type::getBase( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
  // return Data_Base[Axis] + Base[Axis];
     return User_Base[Axis];
   }

inline int
SerialArray_Domain_Type::getRawBase( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
     return Data_Base[Axis] + Base[Axis];
   }

inline int
SerialArray_Domain_Type::getBound( int Axis ) const
   {
  // APP_ASSERT (usesIndirectAddressing() == FALSE);
  // return Data_Base[Axis] + Bound[Axis];

     return User_Base[Axis] + (Bound[Axis]-Base[Axis]) / Stride[Axis];
   }

inline int
SerialArray_Domain_Type::getRawBound( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Data_Base[Axis] + Bound[Axis];
   }

inline int
SerialArray_Domain_Type::getStride( int Axis ) const
   {
     APP_ASSERT((Axis >= 0) && (Axis < MAX_ARRAY_DIMENSION));
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
  // return Stride[Axis];
     return 1;
   }

inline int
SerialArray_Domain_Type::getRawStride( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Stride[Axis];
   }

inline Range
SerialArray_Domain_Type::dimension( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     return Range(getBase(Axis),getBound(Axis),getStride(Axis));
   }

inline Range
SerialArray_Domain_Type::getFullRange( int Axis ) const
   {
  // APP_ASSERT (Uses_Indirect_Addressing == FALSE);
     int length = Size[Axis];
     if (Axis>0) length /= Size[Axis-1];
  // return Range(Data_Base[Axis],length+Data_Base[Axis]-1,Stride[Axis]);
  // return Array_Domain.getFullRange(Axis);

#if 0
#if defined(PPP)
  // This fails on P++/TESTS/test2000_02.C (as I think it should)
     APP_ASSERT (Size[Axis]      == Local_Mask_Index[Axis].getLength());
     APP_ASSERT (Data_Base[Axis] == Local_Mask_Index[Axis].getBase());
     APP_ASSERT (Data_Base[Axis]+Size[Axis] == Local_Mask_Index[Axis].getBound());
     APP_ASSERT (1 == Local_Mask_Index[Axis].getStride());
#endif
#endif

  // (4/29/2000) Fixed bug in stride (which should always be 1 for the full range)
  // return Range(Data_Base[Axis],length+Data_Base[Axis]-1,Stride[Axis]);
     return Range(Data_Base[Axis],length+Data_Base[Axis]-1,1);
   }

inline int
SerialArray_Domain_Type::getLength( int Axis ) const
   {
  // Computes the length of any axis of the array object using this descriptor!
  // This function should be inlined for better efficency!
  // This could be made a little more efficient by avioding the division by the
  // stride in the most common case where the unit stride.
  // Also in the commmon case where Is_A_View == FALSE we could just return the Size[Axis].
 
  // int Temp_Size = (Bound [Axis] - Base [Axis]) + 1;
  // return ( Temp_Size % Stride [Axis] == 0 ) ? Temp_Size / Stride [Axis] : Temp_Size / Stride [Axis] + 1;

     int Temp_Size = 0;
  // Bugfix (11/1/95) Previously did not handle case of Uses_Indirect_Addressing == TRUE
     if (Uses_Indirect_Addressing)
        {
       // Bugfix (11/2/95) Only return Array_Size for case of querey along Axis == 0
          Temp_Size = (Axis == 0) ? Array_Size() : 1;
        }
       else
        {
          Temp_Size = (Bound [Axis] - Base [Axis]) + 1;
          if ( Temp_Size % Stride [Axis] == 0 ) 
               Temp_Size /= Stride [Axis];
            else
               Temp_Size = Temp_Size / Stride [Axis] + 1;
        }

     return Temp_Size;
   }

inline int
SerialArray_Domain_Type::maxNumberOfDimensions()
   {
  // Get static upper bound on number of dimensions of the array objects
     return MAX_ARRAY_DIMENSION;
   }

inline int
SerialArray_Domain_Type::numberOfDimensions() const
   {
  // Get number of dimensions of the array objects (note that a 1x2 array is 2 dimensional)
     return Domain_Dimension;
   }

inline int
SerialArray_Domain_Type::internalNumberOfDimensions() const
   {
  // For array objects not using indirect addressing we use the 
  // current value of Descriptor_Dimension.  But for array objects
  // using indirect addressing we return the number of intArrays in
  // use to discribe the indirect addressing internally.

     int Return_Value = -1;
     if (usesIndirectAddressing() == TRUE)
        {
       // for (int i= MAX_ARRAY_DIMENSION-1; i >= 0; i--)
          int i = MAX_ARRAY_DIMENSION-1;
          while (Return_Value == -1)
             {
               APP_ASSERT (i >= 0);
            // if (Index_Array[i] != NULL)
            // We want the larges value of the axis that is used accounting for the
            // way that Internal_Index objects and intArray objects can be mixed in the indexing.
            // if ( (Index_Array[i] != NULL) || (Base[i] != Bound[i]) )
            // Checking for the base not equal to zero allows us to 
            // detect scalar indexing in the higher dimensions!
               if ( (Index_Array[i] != NULL) || (Base[i] != 0) )
                  Return_Value = i+1;
               i--;
             }
          APP_ASSERT (Return_Value > 0);
        }
       else
        {
          Return_Value = Domain_Dimension;
        }
     return Return_Value;
   }

// End of if defined(INTARRAY)
#endif

inline float* APP_RESTRICT_MACRO
floatSerialArray::getDataPointer () const
   {
     return Array_Descriptor.getDataPointer();
   }

#if defined(PPP)
inline floatSerialArray*
floatSerialArray::getSerialArrayPointer () const
   {
  // return pointer to A++ array
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     return Array_Descriptor.SerialArray;
   }
inline floatSerialArray**
floatSerialArray::getSerialArrayPointerLoc () const
   {
  // return pointer to A++ array address
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     return (floatSerialArray**)&Array_Descriptor.SerialArray;
   }
#endif

// Bugfix (1/13/97) moved this function from the collection
// of local access functions below.  The problem was that it 
// was not being found at link time.
inline Range
floatSerialArray::getFullRange (int Axis) const
   {
      return Array_Descriptor.getFullRange(Axis);
   }

// This is defined for APP SERIAL_APP and PPP
inline Range
floatSerialArray::getLocalFullRange (int Axis) const
   {
#if defined(PPP)
      return getSerialArrayDescriptor().getFullRange(Axis);
#else
      return getFullRange(Axis);
#endif
   }

#if defined(PPP) || defined(APP)
// These are acces functions to local parts of the P++ array.  They must exist in A++
// as well - though they return the global data if used in A++ since that is what local means
// in a serial environment.

#if defined(PPP) 
inline floatSerialArray floatSerialArray::getLocalArray () const
#else
inline floatArray floatSerialArray::getLocalArray () const
#endif
   {
  // return reference to A++ array

#if defined(PPP)
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);
     return floatSerialArray(*Array_Descriptor.SerialArray,SHALLOWCOPY);
  // printf("getLocalArray doesn't work in parallel right now\n");
#else
     return floatArray(*this,SHALLOWCOPY);
#endif
   }

// Must be made available in PPP - SERIAL_APP - APP
// inline Range floatSerialArray::getFullRange (int Axis) const
//    {
//       return Array_Descriptor.getFullRange(Axis);
//    }

#if 0
// This function was placed into array.C file (11/29/2000)
#if defined(PPP)
inline floatSerialArray
floatSerialArray::getLocalArrayWithGhostBoundaries () const
#else
inline floatArray
floatSerialArray::getLocalArrayWithGhostBoundaries () const
#endif
   {
  // return A++ array that includes ghost cells if P++ or a shallow copy 
  // of itself if A++

  // APP_ASSERT(!Array_Descriptor.Is_A_View);

#if defined(PPP) 
     APP_ASSERT(Array_Descriptor.SerialArray != NULL);

     if ((!Array_Descriptor.Array_Domain.Is_A_View) && 
         (!Array_Descriptor.Array_Domain.Is_A_Null_Array) &&
         (!getSerialDomain().Is_A_Null_Array))
     {
        Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List;
        int i;
        for (i=0;i<MAX_ARRAY_DIMENSION;i++)
           {
             Internal_Index_List[i] = new Range(getLocalFullRange(i));
             APP_ASSERT (Internal_Index_List[i] != NULL);
           }
        
        floatSerialArray ReturnArray;
        ReturnArray.adopt(getDataPointer(),Internal_Index_List);
        for (i=0;i<MAX_ARRAY_DIMENSION;i++) 
           {
          // Range objects don't have referenceCounting! (should they?)
             delete Internal_Index_List[i];
           }
        return floatSerialArray(ReturnArray,SHALLOWCOPY);
     }
     else
     {
        return floatSerialArray(*Array_Descriptor.SerialArray,SHALLOWCOPY);
     }
     //printf("getLocalArrayWithGhostBoundaries doesn't work in parallel right now\n");
#else
     return floatArray(*this,SHALLOWCOPY);
#endif
   }
#endif

// end of defined(PPP) || defined(APP)
#endif

// Endif for function inlining
#endif


// We needed to place this below the Allocate_Array_Data function because
// The expression template constructor needed to see it at compile time.
// if defined(INLINE_APP1_FUNCTIONS)

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
// forward declaration
// typedef struct Array_Domain_c     array_domain;
// typedef struct Array_Descriptor_c array_descriptor;

#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" float* MDI_float_Allocate ( array_domain* Descriptor );
#endif

#if defined(INTARRAY)
// error "Inside of inline_func.h"
#endif

//inline void floatSerialArray::Allocate_Array_Data ( bool Force_Memory_Allocation ) const
inline
void
floatSerialArray::Allocate_Array_Data ( bool Force_Memory_Allocation ) 
   {
  // This function allocates the internal data for the floatSerialArray object.  In A++
  // this allocates the the raw float data array.  In P++ it allocates the internal
  // A++ array for the current processor (using the sizes defined in the DARRAY
  // parallel descriptor).

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
          printf ("*** Allocating the Array_Data (or getting it from the hash table)! \n");
#endif  

#if defined(PPP)
  // The allocation of data in the parallel environment is especially complicated
  // so this is broken out as a separate function and is implemented only for P++.
     Array_Descriptor.Allocate_Parallel_Array (Force_Memory_Allocation);
#else
  // To improve the implementation we have moved this functionality into the
  // Array_Descriptor_Type object because that is where the pointers to the
  // Array_Data (or Serial_Array_Data_Pointer in the case of P++) now live.
     APP_ASSERT (getDataPointer() == NULL);
     Array_Descriptor.Allocate_Array_Data(Force_Memory_Allocation);
#endif  // if !defined(PPP)

#if !defined(PPP)
  // Since this is a new array object is should have an initialize reference count on its
  // raw data.  This is required here because the reference counting mechanism reused the
  // value of zero for one existing reference and no references (this will be fixed soon).
     resetRawDataReferenceCount();
#endif
   }          

// We needed to place this below the Allocate_Array_Data function because
// The expression template constructor needed to see it at compile time.
#if defined(INLINE_APP1_FUNCTIONS)

// ******************************************************************************
// This function allocates the array data - the raw memory for A++ - and the
// Serial_A++ array for P++.
// ******************************************************************************
#if !defined(USE_EXPRESSION_TEMPLATES)
extern "C" void MDI_float_Deallocate ( float* Data_Pointer , array_domain* Descriptor );
#endif
inline
void
floatSerialArray::Delete_Array_Data ()
   {
  // Main body of code moved to Array_Descriptor_Type
  // To improve the implementation we have moved this functionality into the
  // Array_Descriptor_Type object because that is where the pointers to the
  // Array_Data (or Serial_Array_Data_Pointer in the case of P++) now live.
     Array_Descriptor.Delete_Array_Data();
   }

// ***********************************************************************
// Part of error checking is to verify binary conformability (i.e. the
// array operations between arrays of the same size IN EACH DIMENSION).
// ***********************************************************************
inline bool
floatSerialArray::Binary_Conformable ( const floatSerialArray & X ) const
   {
  // We must defin e binary conformable to exclude the use of the where statement
     return ( ( (Array_Descriptor.Array_Domain.Is_Contiguous_Data == TRUE)     &&
                (X.Array_Descriptor.Array_Domain.Is_Contiguous_Data == TRUE) ) &&
              (Where_Statement_Support::Where_Statement_Mask == NULL) ) ||
            Array_Descriptor.Array_Domain.Is_A_Null_Array;
   }

// *****************************************************************
// *****************************************************************
//            inline support functions for the operators
// *****************************************************************
// *****************************************************************

// ***********************************************************************************
// This function handles the case of deleting a temporary that appears on
// the Lhs (which is rare and for the most part does not happen in sane user
// code).  Example: (A+B) = 1;
// But the case is handled anyway.  It used to be a that views are managed similar
// to temporaries - but this was an error (view have to have the scope of the
// scope that builds them).  This is easy to demonstrate in the case where a
// view is passed into a function (the function can't delete the view).
// ***** Come to think of it: this function may be not be useful!  *****
// ***** WRONG! *****
// Even taking the view of a temporary requires the deletion of the temporary -
// so for example: A = (B+C)(I,J) requires that we delete the temporary (B+C)
// this is done within the operator() for each of the different versions of operator().
// It is an obscure point - but it happens in the testcode.C and forced a failure on the SGI
// ***********************************************************************************
extern floatSerialArray *Last_Lhs_floatSerialArray_Operand;

#if 0
inline void
Delete_Lhs_If_Temporary ( const floatSerialArray & Lhs )
   {
  // The Solaris C++ compiler v3.0 can't inline a function with a static variable we get a
  // "sorry not implemented: cannot expand inline function with static Last_Lhs_floatSerialArray_Operand" message.
  // So it is a variable with file scope (later we should make it a static data member).

#if !defined(SERIAL_APP)
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
       // printf ("Inside of Delete_Lhs_If_Temporary! \n");
          printf ("################################################################### \n");
          printf ("########### Inside of floatSerialArray::Delete_Lhs_If_Temporary! ######### \n");
          printf ("################################################################### \n");
        }
#endif

  // Last_Lhs_floatSerialArray_Operand is given file scope! We could make it a static member of the class?
     if (Last_Lhs_floatSerialArray_Operand != NULL)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Inside of Delete_Lhs_If_Temporary (DELETING THE LAST LHS)! \n");
#endif
       // I don't think this function is ever called called so as to delete anything!
       // printf ("Exiting from Delete_Lhs_If_Temporary ( const floatSerialArray & Lhs ) ... \n");
       // exit(1);

       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Last_Lhs_floatSerialArray_Operand->decrementReferenceCount();
          if (Last_Lhs_floatSerialArray_Operand->getReferenceCount() < floatSerialArray::getReferenceCountBase())
               delete Last_Lhs_floatSerialArray_Operand;
       // Last_Lhs_floatSerialArray_Operand = NULL;
        }

  // Bug fix (11/30/93) Views have to have local scope else they are
  // deleted too soon when passed by reference to an expression.
  // Since they have no data associated with them they are not
  // a problem with their longer lifetime! (only a few words are
  // required to hold a view where as a temporary has array data
  // associated with it).

  // Last_Lhs_floatSerialArray_Operand = (Lhs.Array_Descriptor.Array_Domain.Is_A_Temporary) ? &((floatSerialArray &) Lhs) : NULL;

  // This might be a memory leak for P++ since views can be temporaries and must be deleted (sometimes)
     Last_Lhs_floatSerialArray_Operand = (Lhs.Array_Descriptor.Array_Domain.Is_A_Temporary && !Lhs.Array_Descriptor.Array_Domain.Is_A_View) ? &((floatSerialArray &) Lhs) : NULL;

#else
  // printf ("DELETE LHS IF TEMPORARY turned off! \n");
  // APP_ASSERT(Lhs.Array_Descriptor != NULL);
#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency ("Called from Delete_Lhs_If_Temporary(floatSerialArray)");
#endif
#endif
   }
#endif
 
#if 0
inline void
Delete_If_Temporary ( const floatSerialArray & X )
   {
  // Constants can never be a temporary - it is the law! 
  // I admit that it seems questionable to delete an object
  // that is passed in by a const ref but mutable types will 
  // make this a more acceptable thing to do when they 
  // appear in C++ (next version -- I think)

#if !defined(SERIAL_APP)
  // These behavior was reverified on (2/26/96) while fixing memory leaks in P++.
  // We only activate the temporary management for the A++ and P++ objects
  // not for the Serial_A++ objects.  This is to aviod the redundent
  // management of P++ arrays and Serial_A++ arrays that are used internally in
  // P++.  The way we avoid this temporary management iof the Serial_A++ objects is
  // to eliminate the body of this function when it is used to generate the
  // Serial_A++ member function

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("################################################################### \n");
          printf ("############# Inside of floatSerialArray::Delete_If_Temporary! ########### \n");
          printf ("################################################################### \n");
        }
#endif

  // Bug fix (11/30/93) Views have to have local scope else they are
  // deleted too soon when passed by reference to an expression.
  // Since they have no data associated with them they are not
  // a problem with their longer lifetime! (only a few words are
  // required to hold a view where as a temporary has array data
  // associated with it).

     if (X.isTemporary())
        {
#if COMPILE_DEBUG_STATEMENTS
          if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Inside of Delete_If_Temporary (DELETING floatSerialArray object (id=%d)) X.getReferenceCount() = %d \n",
                    X.Array_ID(),X.getReferenceCount());
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          X.decrementReferenceCount();
          if (X.getReferenceCount() < floatSerialArray::getReferenceCountBase())
               delete & (floatSerialArray &) X;    // Notice that we cast away const here!
        }
#if COMPILE_DEBUG_STATEMENTS
       else
        {
          if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
               printf ("Inside of Delete_If_Temporary (NOT A TEMPORARY floatSerialArray object (id=%d)) X.getReferenceCount() = %d \n",
                    X.Array_ID(),X.getReferenceCount());
        }
#endif
#else
  // printf ("DELETE IF TEMPORARY turned off! \n");
  // APP_ASSERT(X.Array_Descriptor != NULL);
#if COMPILE_DEBUG_STATEMENTS
     X.Test_Consistency ("Called from Delete_Lhs_If_Temporary(floatSerialArray)");
#endif
#endif
   }
#endif

// End of inlineable functions!
#endif


#undef FLOATARRAY

#endif  /* !defined(_APP_INLINE_FUNC_H) */












