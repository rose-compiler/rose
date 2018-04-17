
#ifndef _array_domain_h
#define _array_domain_h

/* include "int_array.h" */
   typedef struct Array_Descriptor_c array_descriptor;
   typedef struct Array_Domain_c array_domain;
   typedef struct intArray_c int_array;

/*
  This is a C style data structure to mimic the C++ class so that the C code can access the
  C++ class directly and as efficiently as possible.
 */

struct Array_Domain_c
   {
     int Size          [MAX_ARRAY_DIMENSION];  
     int Data_Base     [MAX_ARRAY_DIMENSION];  /* First element along each dimension */
  /* int Geometry_Base [MAX_ARRAY_DIMENSION];  */

     int Base          [MAX_ARRAY_DIMENSION]; /* base of view */  
     int Bound         [MAX_ARRAY_DIMENSION];  
     int Stride        [MAX_ARRAY_DIMENSION];  

     int Is_A_View;           
     int Is_A_Temporary;      
     int Is_Contiguous_Data;  
     int Is_A_Null_Array;     
     int Is_Built_By_Defered_Evaluation;  

  /* required for indirect addressing support */
     int_array* Index_Array[MAX_ARRAY_DIMENSION];  

     int Uses_Indirect_Addressing;  

     int Array_ID;  

  /* don't need for MDI but here for size consistemcy */

     array_domain *freepointer;
  /* int builtUsingExistingData; */

  /* descriptor fields added to support efficient caching! */

     int Descriptor_Dimension;      
     int Constant_Data_Base;    
     int Constant_Unit_Stride;  

     int builtUsingExistingData;

  /* array_domain* Next_Cache_Link; */

     int View_Offset;
     int Scalar_Offset[MAX_ARRAY_DIMENSION];
     int User_Base     [MAX_ARRAY_DIMENSION];  

     int typeCode;

     int IndexBase         [MAX_ARRAY_DIMENSION];
     int IndexStride       [MAX_ARRAY_DIMENSION];
     int* IndexDataPointer [MAX_ARRAY_DIMENSION];
     int ExpressionTemplateOffset;
     int referenceCount;
   };

struct Array_Descriptor_c
   {
     array_domain Array_Domain;

  /* Used only for new delete memory pools */
     array_descriptor* freepointer;
   
     int* Array_Data;
     int* Array_View_Pointer0;

#if MAX_ARRAY_DIMENSION>1
     int* Array_View_Pointer1;
#endif

#if MAX_ARRAY_DIMENSION>2
     int* Array_View_Pointer2;
#endif

#if MAX_ARRAY_DIMENSION>3
     int* Array_View_Pointer3;
#endif

#if MAX_ARRAY_DIMENSION>4
     int* Array_View_Pointer4;
#endif

#if MAX_ARRAY_DIMENSION>5
     int* Array_View_Pointer5;
#endif

#if MAX_ARRAY_DIMENSION>6
     int* Array_View_Pointer6;
#endif

#if MAX_ARRAY_DIMENSION>7
     int* Array_View_Pointer7;
#endif

     int* ExpressionTemplateDataPointer;
     int referenceCount;
   };

struct intArray_c
   {
  /* Note that the intArray_c defined in the int_array.h file is not used! */
  /* It seems we don't need this pointer once we have only a single virtual
     function in the BaseArray class -- previously it was required when we had
     two virtual base class member functions.  I think that since one of the
     two member functions was only defined for P++ that this pointer 
     (more likely a virtual function table pointer) only existing when
     a function was defined.  So I have redefined the function Dummy_Array_ID
     and now we have to have this pointer defined here (again).
   */
     int* BaseArray_Base_Class_Virtual_Function_Table_Pointer;

  /* Array_Descriptors are not data members of the array class */
     array_descriptor Array_Descriptor;

  /* This supports the defered evaluation (specifically it supports the means by
     which the lifetimes of variables are extended by the defered evaluation)! */

  /* this assumes an Operand_Storage pointer is the same size as int* */
     int* Array_Storage;
     int_array *freepointer;
     int referenceCount;
   };

#endif /* _array_domain_h */

