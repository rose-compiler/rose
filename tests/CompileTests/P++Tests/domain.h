// Allow repeated includes of domain.h without error
#ifndef _APP_DOMAIN_H
#define _APP_DOMAIN_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif

















// This macro builds the mechanism by which we print internal warnings 
// about poor implementation details which are in the process of being fixed
// These are FALSE except during code development
#define PRINT_SOURCE_CODE_IMPLEMENTATION_WARNINGS FALSE

// pragma implementation "A++.h"
// pragma implementation "descriptor.h"

// Typedefs used to simplify (and make more clear) the dimension 
// independent function interface of the A++/P++ implementation.
typedef int* Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Index* Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Indirect_Addressing_Index* Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];

typedef int Integer_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];
typedef Internal_Index Index_Array_MAX_ARRAY_DIMENSION_Type [MAX_ARRAY_DIMENSION];

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
class MemoryManagerType
   {
     public:
          MemoryManagerType ();
         ~MemoryManagerType ();

     private:
          MemoryManagerType ( const MemoryManagerType & X );
          MemoryManagerType & operator= ( const MemoryManagerType & X );
   };
#endif

#if defined(PPP)
class Array_Conformability_Info_Type;
#endif

#if 0
extern int*    APP_Array_Reference_Count_Array;
extern int     APP_Array_Reference_Count_Array_Length;
extern bool    APP_smartReleaseOfInternalMemory;
extern int     APP_Max_Stack_Size;
extern int     APP_Stack_Depth;
extern int*    APP_Top_Of_Stack;
extern int*    APP_Bottom_Of_Stack;
#endif


#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
class AppString
   {
  // This class is for use internally within A++/P++!
  // This class was created to force a single level of automated
  // promotion of char* within the initialization of an array object
  // using a string.  It is not intended to be part of the user
  // interface for A++/P++.  It avoids the ambiguity within the C++
  // compiler when overloaded functions taking array objects or strings
  // are used together.  Basically it cleans up the interface and better
  // allows array constructors taking strings to co-exist with Overture.
  // function definitions are located in domain.C
     private:
          char* internalString;

     public:
         ~AppString ();
          AppString ();
          AppString ( const char* c );
          AppString ( const AppString & X );
          AppString & operator= ( const AppString & X );
          char* getInternalString() const;
          void setInternalString( const char* c );
   };
#endif

// This is a non-templated class so that we can easily return a pointer to it
// from an expression of an complex collection of types as simply as possible.
// Actually we need to template this class on the array dimension in the near future.
class SerialArray_Domain_Type
   {
  // **********************************************************************************************
  // We might only require one descriptor (instead of one for each different type of array object)!
  // This is because we no longer require the use of the Original_Array field in this class!
  // **********************************************************************************************

     friend class Where_Statement_Support;
  // friend class doubleSerialArray;
  // friend class floatSerialArray;
  // friend class intSerialArray;
#if defined(PPP)
     friend double sum ( const doubleSerialArray & X );
     friend float  sum ( const floatSerialArray  & X );
     friend int    sum ( const intSerialArray    & X );
#endif

  // friend doubleSerialArray sum ( const doubleSerialArray & X , int Axis );
  // friend floatSerialArray  sum ( const floatSerialArray  & X , int Axis );
  // friend intSerialArray    sum ( const intSerialArray    & X , int Axis );

     friend doubleSerialArray & transpose ( const doubleSerialArray & X );
     friend floatSerialArray  & transpose ( const floatSerialArray  & X );
     friend intSerialArray    & transpose ( const intSerialArray    & X );

  // Allow access to private variables Top_Of_Stack and Bottom_Of_Stack
     friend class MemoryManagerType;

#if defined(SERIAL_APP) && !defined(PPP)
  // error make doubleArray a friend
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;

  // friend class Partitioning_Type;

  // friend doubleArray sum ( const doubleArray & X , int Axis );
  // friend floatArray  sum ( const floatArray  & X , int Axis );
  // friend intArray    sum ( const intArray    & X , int Axis );
#endif

#if defined(APP) || defined(PPP)
  // error make doubleSerialArray a friend

  // Force the class to work through the interface
  // template<class T , int Template_Dimension> friend class Array_Descriptor_Type;
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

     friend class Internal_Partitioning_Type;
  // friend class Partitioning_Type;
#endif

#if defined(PPP)
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;
     friend class Array_Conformability_Info_Type;
#endif

     public:
     /* A lot of other things will be added here, but it should be a minimal set! */
        int Size          [MAX_ARRAY_DIMENSION];

     /* First element along each dimension (setBase modifies this value) (needed to 
        fix Karen Pao bug). The Data_Base is not meaningful to the user and represents
        a required vaule for the the internal representation of A++ objects so that
        they can have user defined bases in each dimension.
     */
        int Data_Base     [MAX_ARRAY_DIMENSION];

     /* Geometry must be preserved seperately since views have a base of ZERO (by default)
     // and so do not preserve the original base. The reason why a view does not
     // preserve the original base is that a view represents an A++ object and a
     // language element (a new one) thus it has the default base same as any new
     // A++ object.  This is a point of consistancy and it is not a trivial point.
     // Similarly assignment can not be allowed to alter the base on an object.
     // In addition to the language argument the use of the default base permits
     // consistancy with FORTRAN 90 and thus HPF.  Similarly it is consistant with
     // the implementation of pointers into arrays (which is why it is this way in FORTRAN
     // and FORTRAN 90).  The use of a seperate Geometry Base allows for the geometry 
     // to be preserved seperately.  The reason we want to preserve the geometry is that
     // it is information about an array object that the user specifies and which
     // the user can use within the application.  The importance ot this became clear
     // in the contrast we did of the FArrayBox that LLNL uses and the A++ objects.
     */
     // THIS IS NO LONGER USED!
     // int Geometry_Base [MAX_ARRAY_DIMENSION];

     /* This is the base of the view of the array! */
     // int Access_Base   [MAX_ARRAY_DIMENSION];  Access_Base is a better name than Base!
     // Note that the Base and Bound are set to ZERO if indirect addressing is 
     // used (in which case these are of no use!) See the User_Base for more info!
        int Base          [MAX_ARRAY_DIMENSION];
        int Bound         [MAX_ARRAY_DIMENSION];
        int Stride        [MAX_ARRAY_DIMENSION];

     // Don't make these bool because then we can't interface to C code (since bool is not a type in C).
        int Is_A_View;
        int Is_A_Temporary;
        int Is_Contiguous_Data;

     // Allows undefined array which is useful but required for P++ support!
     // Don't make these bool because then we can't interface to C code (since bool is not a type in C).
        int Is_A_Null_Array;

     // I'm not certain that we need this! 
     // Don't make these bool because then we can't interface to C code (since bool is not a type in C).
        int Is_Built_By_Defered_Evaluation;

     // intSerialArray indexing support for each array dimension (required for inirect addressing support)!
        intSerialArray *Index_Array[MAX_ARRAY_DIMENSION];
#if defined(PPP)
     // Used to store local parts of P++ intArrays for indirect addressing support
        intSerialArray *Local_Index_Array[MAX_ARRAY_DIMENSION];
#endif

     // If TRUE then array access requires use of indirect addressing!
     // We might want to require that scalar indexing support for Indirect Addressed Arrays
     // be done through the use of the operator[] sice otherwise a conditional test would be
     // required in the operator(int) family of operators!
     // Don't make these bool because then we can't interface to C code (since bool is not a type in C).
        int Uses_Indirect_Addressing;

     // (12/2/93) we will use this Array_ID for indexng into the Reference count array.
     // For more details see the note in the header file array.h (next to the reference variables).
     // (6/1/94?) A stack is used to avoid an overly long reference count array (lenght
     // equal to the largest Array_ID - since this could be in the millions).
     // This is used to support the dependence analysis for defered evaluation!
     // int Array_ID;
        int internalArrayID;

     // access function
        int Array_ID() const { return internalArrayID; }
        void setArray_ID(int newArrayID) { internalArrayID = newArrayID; }


     // Data required for the "new" and "delete" operators!
     // These are not used so much now that the array descriptors have array domains
     // as member data (instead of just pointers to the array descriptors).
     private:
        static SerialArray_Domain_Type *Current_Link;
        SerialArray_Domain_Type        *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

     public:
#if defined(PPP)
     // Pointer to a P++ partitioning object
     // Partitioning_Type *Partitioning_Object_Pointer;
        Internal_Partitioning_Type *Partitioning_Object_Pointer;

#if defined(USE_PADRE)
     // May have to have Array_Domain_Type constructors call a particular constructor within PADRE
     // Also note that the PADRE_Representation object need not be built explicitly. I.e. it
     // is not required that it correspond to an object in P++.  This does not effect the usefulness
     // of the PADRE design to P++ or any other library using PADRE (I think).
     // PADRE_Descriptor<BaseArray,Array_Domain_Type,Array_Domain_Type> parallelDescriptor;
     // PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> parallelDescriptor;
     // I'm trying to figure out the cause of some stack frame errors in the combination of P++/PADRE
        PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *parallelPADRE_DescriptorPointer;
     PADRE_Descriptor<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *getParallelPADRE_DescriptorPointer () const {
       return parallelPADRE_DescriptorPointer;
     }
     PADRE_Representation<BaseArray,Array_Domain_Type,SerialArray_Domain_Type> *getParallelPADRE_RepresentationPointer () const {
       if ( parallelPADRE_DescriptorPointer ) {
	 return parallelPADRE_DescriptorPointer->representation;
       }
       else return NULL;
     }

     // Use a second pointer to make this object the same size independent of using PADRE or not.
     // This is part of an investigation of a long standing stack frame problem with P++ when using
     // PADRE.  It seems to be impossible to get rid of but it does not happen in P++ only in the 
     // combination of P++ and PADRE (where it happens often, but not always).
     // This does infact fix the problem.  The point was that this object was being seen to be
     // two different sizes (one when compiled with COMPILE_PPP defined and the other when it was compiled
     // within the final application code (without COMPILE_PPP defined)).
     // At some point we should devise a better fix for this problem.
        int * DummyPointer;

#if 0
     /* These should be implemented in PADRE when PADRE uses Parti.
      */
        DECOMP *getBlockPartiArrayDecomposition() const
           { 
             DECOMP* ReturnPointer = NULL;
             if (parallelPADRE_DescriptorPointer != NULL)
                {
                  APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
                  APP_ASSERT (parallelPADRE_DescriptorPointer->representation != NULL);
                  //APP_ASSERT (parallelPADRE_DescriptorPointer->representation->
                  //            pPARTI_Representation != NULL);
                  if (parallelPADRE_DescriptorPointer->representation->
                              pPARTI_Representation != NULL)
                  {
                    APP_ASSERT 
		      (parallelPADRE_DescriptorPointer->representation->
                       pPARTI_Representation->BlockPartiArrayDecomposition != NULL);
                    ReturnPointer = 
		      parallelPADRE_DescriptorPointer->representation->
                      pPARTI_Representation->BlockPartiArrayDecomposition; 
                  }
		  else 
		    ReturnPointer = NULL; 

                }

             return ReturnPointer;
           }

        DARRAY *getBlockPartiArrayDomain() const
           { 
             DARRAY* ReturnPointer = NULL;
             if (parallelPADRE_DescriptorPointer != NULL)
                {
                  APP_ASSERT (parallelPADRE_DescriptorPointer != NULL);
                  APP_ASSERT (parallelPADRE_DescriptorPointer->representation != NULL);
                  //APP_ASSERT (parallelPADRE_DescriptorPointer->representation->
                  //            pPARTI_Representation != NULL);
                  if (parallelPADRE_DescriptorPointer->representation->
                      pPARTI_Representation != NULL)
                  {
                    APP_ASSERT 
		      (parallelPADRE_DescriptorPointer->representation->
                       pPARTI_Representation->BlockPartiArrayDescriptor != NULL);
                    ReturnPointer = 
		      parallelPADRE_DescriptorPointer->representation->
                      pPARTI_Representation->BlockPartiArrayDescriptor; 
                  }
		  else
                    ReturnPointer = NULL; 
                }

             return ReturnPointer;
           }
#endif
#else
     // Pointer to Block-Parti parallel decomposition (object)
        DECOMP *BlockPartiArrayDecomposition;
     // Pointer to Block-Parti parallel distributed array descriptor
        DARRAY *BlockPartiArrayDomain;

        DECOMP *getBlockPartiArrayDecomposition() const
           { 
             APP_ASSERT (BlockPartiArrayDecomposition != NULL);
             return BlockPartiArrayDecomposition; 
           }
        DARRAY *getBlockPartiArrayDomain() const
           { 
             APP_ASSERT (BlockPartiArrayDomain != NULL);
             return BlockPartiArrayDomain; 
           }
#endif

     // ghost cell info for partitioned arrays (I think these may overlap with a feature in PADRE)
        int InternalGhostCellWidth [MAX_ARRAY_DIMENSION];
        int ExternalGhostCellWidth [MAX_ARRAY_DIMENSION];

     // Specialized P++ support for interpretation of message passing
     // This might make more sense in a seperate structure that was started at the 
     // beginning of a statements evaluation and ended at the end of a statements
     // evaluation and was passed along through the binary operators in the processing.
     // (12/13/2000) Note: These are set to ZERO on processors where the SerialArray is a NullArray.

     /*
       BTNG made Left_Number_Of_Points and Right_Number_Of_Points
       private because there are alreeady access functions for them.
       This is meant to reduce the scope for which to look for bugs.
     */
        private: int Left_Number_Of_Points  [MAX_ARRAY_DIMENSION];
        private: int Right_Number_Of_Points [MAX_ARRAY_DIMENSION];

   public:

     // **********************************************************************************
     // Specialized P++ support for interpretation of message passing
     // It is simpler to contain this information in a separate structure (object)
     // and then pass this pointer along in the process of evaluation of an array statement
     // rather than copy the information back and forth.
        Array_Conformability_Info_Type *Array_Conformability_Info;
     // **********************************************************************************

     // Specialized P++ support for interpretation of message passing
        Internal_Index Global_Index     [MAX_ARRAY_DIMENSION];
        Internal_Index Local_Mask_Index [MAX_ARRAY_DIMENSION];
#endif

     // descriptor fields added to support efficient caching!
     /*
       Domain_Dimension is the dimension the user is using (<=MAX_ARRAY_DIMENSION).
       It is stored to avoid redundant computing.
     */
        int Domain_Dimension;
     // Don't make these bool because then we can't interface to C code (since bool is not a type in C).
        int Constant_Data_Base;
        int Constant_Unit_Stride;

     // This records that an array object was built using the adopt function
     // and thus the use of Array_IDs is more complex because the adopt function
     // is forced to use a new Array_ID value where as the reference function reuses
     // the Array_ID from what it is referencing. Which of these methods is used is important
     // because the redim (and destructors) must know whether it must return the Array_ID for
     // reuse.  If the Array_IDs are not used and returned properly then within time-dependent
     // codes the length of the reference count array will grow unbounded.
     // Don't make these bool because then we can't interface to C code (since bool is not a type in C).
        int builtUsingExistingData;

      // ... store the offset of the view from Array_Data so later a pointer
      //  can be set up to make scalar indexing more efficient ...
      int View_Offset;

      // ... store sum of offset due to nonzero Data_Base and View_Offset to
      //  make scalar indexing more efficient ...
      int Scalar_Offset[MAX_ARRAY_DIMENSION];

      // The User_Base is used to record the base of the array access for both 
      // normal Index and Range objects as well as for intArrray indexing.
      // for normal indexing it is the sum of the data_base and base elements
      // for indirect addressing it is the ???
      // ... store the base the user needs for scalar indexing. this should
      //  be stored near Data_Base but is put here for now so the MDI code
      //  doesn't break ...
      // For a strided view the User_Base is the base of the Range objects used to specify that view (for each axis).
      int User_Base     [MAX_ARRAY_DIMENSION];

   // This records the type of object using the domain.  This allows the internal diagnostics to 
   // track the use of array objects and catagorize the uses by the different types of objects that 
   // use the array domain object.  For example knowing if this domain is used for a double or float
   // allows us to compute the memory used (different for each element type).  This also avoids
   // maintaining separate list for each type (and permits the reuse of the already debugged reference
   // counting mechanism to maintain the list).
      int typeCode;

#if 0
   // This function outputs a string based upon the value of the typeCode
      char* getTypeString() const;
      int   getTypeSize  () const;
      void  setTypeCode ( int x ) { typeCode = x; }
#endif

     public:

     // The ROSE preprocessor will likely use some features of 
     // the expression template implementation in at least its initial implementation
// if defined(USE_EXPRESSION_TEMPLATES)
     // Functions specific to the expression template implementation
     // int conformsTo () const { return TRUE; };
     // int conformsTo ( const Array_Domain_Type & X ) const { return TRUE; };
        int conformsTo ( const Array_Domain_Type & X ) const;

     // These pointers are used for the support of indirect addressing within the
     // expression template implementation.  We require an intermediate form of
     // representation of indexing information for an array so that we can avoid
     // complex conditionals within the body of the innermost for loop.
     // to a certain extent these variables amount to workspace that the
     // expression template implementation can use.
        int IndexBase         [MAX_ARRAY_DIMENSION];
        int IndexStride       [MAX_ARRAY_DIMENSION];
        int* APP_RESTRICT_MACRO IndexDataPointer [MAX_ARRAY_DIMENSION];


     // Offset of Array_Data to the pointer used for expression templates
     // to make the subscript computation as simple as possible.
        int ExpressionTemplateOffset;

        static int  IndirectAddressingMap_Index;
        static int* IndirectAddressingMap_Data [APP_STATIC_NUMBER_OF_MAPS];
        static int  IndirectAddressingMap_Minimum_Base [APP_STATIC_NUMBER_OF_MAPS];
        static int  IndirectAddressingMap_Maximum_Bound [APP_STATIC_NUMBER_OF_MAPS];
        static int  IndirectAddressingMap_Length [APP_STATIC_NUMBER_OF_MAPS];

        int *getDefaultIndexMapData ( int Local_Min_Base, int Local_Max_Bound ) const;
        void setupIndirectAddressingSupport() const;

     // Indexing function. Used to find the ith item - taking account of strides.
     // This function could be made more efficient which would improve the efficency of the
     // indirect addressing overall.  The trick would be to remove the use of the base in the
     // subscript computation.  But since this is always a 1D computation it is not a big deal.
        int computeSubscript ( int axis, int i ) const APP_RESTRICT_MACRO
           { return IndexDataPointer[axis][IndexBase[axis] + (i * IndexStride[axis])]; }

     /* SUGGESTION:  Since the expression templates have different demands on
     //              the scalar indexing we should consider combining the 
     //              operations to simplify the operations below (i.e multiply out
     //              offset and seperate the variant parts dependent upon i,j,k, etc. and the
     //              invariant parts which could be stored seperately).
     */
        inline int offset(int i) const APP_RESTRICT_MACRO
           { return i * Stride[0]; }
        inline int offset0(int i) const APP_RESTRICT_MACRO
           { return Base[0] + i * Stride[0]; }
        inline int indirectOffset_1D ( int i ) const APP_RESTRICT_MACRO
           { return offset(computeSubscript(0,i)); }

#if (MAX_ARRAY_DIMENSION >= 2)
        inline int offset(int i, int j) const APP_RESTRICT_MACRO
           { return i * Stride[0] + j * Stride[1] * Size[0]; }
        inline int offset0(int i, int j) const APP_RESTRICT_MACRO
        // { return Base[0] + i * Stride[0] + (Base[1] + j * Stride[1]) * Size[0]; }
           { return i * Stride[0] + j * Stride[1] * Size[0] + (Base[0] + Base[1] * Size[0]); }
        inline int indirectOffset_2D ( int i ) const APP_RESTRICT_MACRO
           { return offset(computeSubscript(0,i), computeSubscript(1,i)); }
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        inline int offset(int i, int j, int k) const APP_RESTRICT_MACRO
           { return i * Stride[0] + j * Stride[1] * Size[0] + k * Stride[2] * Size[1]; }
        inline int offset0(int i, int j, int k) const APP_RESTRICT_MACRO
        // { return Base[0] + i * Stride[0] + (Base[1] + j * Stride[1]) * Size[0] + 
        //         (Base[2] + k * Stride[2]) * Size[1]; }
           { return i * Stride[0] + j * Stride[1] * Size[0] +
                    k * Stride[2] * Size[1] + (Base[0] + Base[1] * Size[0] + Base[2] * Size[1]); }
        inline int indirectOffset_3D ( int i ) const APP_RESTRICT_MACRO
           { return offset(computeSubscript(0,i), computeSubscript(1,i),
                           computeSubscript(2,i)); }
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        inline int offset(int i, int j, int k, int l) const APP_RESTRICT_MACRO
           { return i * Stride[0]           + j * Stride[1] * Size[0] + 
                    k * Stride[2] * Size[1] + l * Stride[3] * Size[2]; }
        inline int offset0(int i, int j, int k, int l) const APP_RESTRICT_MACRO
        // { return Base[0] + i * Stride[0] + (Base[1] + j * Stride[1]) * Size[0] + 
        //         (Base[2] + k * Stride[2]) * Size[1] + (Base[3] + l * Stride[3]) * Size[2]; }
           { return i * Stride[0] + j * Stride[1] * Size[0] + 
                    k * Stride[2] * Size[1] + l * Stride[3] * Size[2] + 
                    (Base[0] + Base[1] * Size[0] + Base[2] * Size[1] + Base[3] * Size[2]); }
        inline int indirectOffset_4D ( int i ) const APP_RESTRICT_MACRO
           { return offset(computeSubscript(0,i), computeSubscript(1,i),
                           computeSubscript(2,i), computeSubscript(3,i)); }
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        inline int offset(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
           { return offset(i,j,k,l) + m * Stride[4] * Size[3]; }
        inline int offset0(int i, int j, int k, int l, int m) const APP_RESTRICT_MACRO
        // { return offset0(i,j,k,l) + (Base[4] + m * Stride[4]) * Size[3]; }
           { return offset0(i,j,k,l) + m * Stride[4] * Size[3] + Base[4] * Size[3]; }
        inline int indirectOffset_5D ( int i ) const APP_RESTRICT_MACRO
           { return offset(computeSubscript(0,i), computeSubscript(1,i),
                           computeSubscript(2,i), computeSubscript(3,i),
                           computeSubscript(4,i)); }
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        inline int offset(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
           { return offset(i,j,k,l,m) + n * Stride[5] * Size[4]; }
        inline int offset0(int i, int j, int k, int l, int m, int n) const APP_RESTRICT_MACRO
        // { return offset0(i,j,k,l,m) + (Base[5] + n * Stride[5]) * Size[4]; }
           { return offset0(i,j,k,l,m) + n * Stride[5] * Size[4] + Base[5] * Size[4]; }
        inline int indirectOffset_6D ( int i ) const APP_RESTRICT_MACRO
           { return offset(computeSubscript(0,i), computeSubscript(1,i),
                           computeSubscript(2,i), computeSubscript(3,i),
                           computeSubscript(4,i), computeSubscript(5,i)); }
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        inline int offset(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
           { return offset(i,j,k,l,m,n) + o * Stride[6] * Size[5]; }
        inline int offset0(int i, int j, int k, int l, int m, int n, int o) const APP_RESTRICT_MACRO
        // { return offset0(i,j,k,l,m,n) + (Base[6] + o * Stride[6]) * Size[5]; }
           { return offset0(i,j,k,l,m,n) + o * Stride[6] * Size[5] + Base[6] * Size[5]; }
        inline int indirectOffset_7D ( int i ) const APP_RESTRICT_MACRO
           { return offset(computeSubscript(0,i), computeSubscript(1,i),
                           computeSubscript(2,i), computeSubscript(3,i),
                           computeSubscript(4,i), computeSubscript(5,i),
                           computeSubscript(6,i)); }
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        inline int offset(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
           { return offset(i,j,k,l,m,n,o) + p * Stride[7] * Size[6]; }
        inline int offset0(int i, int j, int k, int l, int m, int n, int o, int p) const APP_RESTRICT_MACRO
        // { return offset0(i,j,k,l,m,n,o) + (Base[7] + p * Stride[7]) * Size[6]; }
           { return offset0(i,j,k,l,m,n,o) + p * Stride[7] * Size[6] + Base[7] * Size[6]; }
        inline int indirectOffset_8D ( int i ) const APP_RESTRICT_MACRO
           { return offset(computeSubscript(0,i), computeSubscript(1,i),
                           computeSubscript(2,i), computeSubscript(3,i),
                           computeSubscript(4,i), computeSubscript(5,i),
                           computeSubscript(6,i), computeSubscript(7,i));
           }
#endif

// end of defined(USE_EXPRESSION_TEMPLATES)

     // *************************************************
     // Reference counting data and functions
     private:
          int referenceCount;

     public:
       // We are forced to make much more public that I would like becase the
       // Sun C++ compiler can't represent templated classes as friends within
       // not templated classes.
#if 1
       // Use of reference count access is made available for debugging
          int getReferenceCount() const 
             { 
               APP_ASSERT (referenceCount >= -2);
               return referenceCount; 
             }
#endif
       // Support functions for reference counting (inlined for performance)
          inline void decrementReferenceCount() const { ((SerialArray_Domain_Type*) this)->referenceCount--; }
          inline void incrementReferenceCount() const { ((SerialArray_Domain_Type*) this)->referenceCount++; }
       // inline void referenceCountedDelete () { delete this; }

          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return APP_REFERENCE_COUNT_BASE;
             }

          inline void resetReferenceCount()
             {
           // reset the Reference Count in the array object!
              referenceCount = getReferenceCountBase();
             }

     // *************************************************

     // abort function for A++
     // These aredefined in A++_headers.h and should be part of a more common support.h header file
     // friend void APP_ABORT();
     // friend void APP_Assertion_Support( char* Source_File_With_Error, unsigned Line_Number_In_File );

#if !defined(PPP)
     // Set mechanism for auto deletion of internal memory after last
     // array is deleted. (it is OK to make these bool because they are static)
        static bool smartReleaseOfInternalMemory;
	static bool exitFromGlobalMemoryRelease;
	static bool PppIsAvailableToCallGlobalMemoryRelease;
	static bool letPppCallGlobalMemoryRelease;
        static bool globalMemoryReleaseAlreadyCalled;
#endif

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
     // Stack management of array ids is handled only for A++ arrays and P++ serial
     // arrays. P++ parallel array objects use the serial array stack management 
     // mechanism so that we maintain only a single stack management mechanism for
     // array ids.

     // Data required for the "Push" and "Pop" stack operators!
     private:
        static int Max_Stack_Size;
        static int Stack_Depth;
        static int *Top_Of_Stack;
        static int *Bottom_Of_Stack;

     public:
     //! Access functions for when P++ Test_Consitancy function needs to do error checking on the stack
        static int* getTopOfArrayIDStack()   { return Top_Of_Stack; }
        static int  getDepthOfArrayIDStack() { return Stack_Depth; }

     //! Setup the diagnostic information (previously implemented in the Pop_Array_ID() function)
        void setupDiagnostics();
#endif

        void initializeReferenceCount();

     public:
#if defined(APP_DIAGNOSTICS)
      // This object is an optional mechanism that permits the recording of localized
      // diagnostics on a per object basis (the default is to record only on a global basis).
	 Diagnostic_Manager diagnosticManager;
#endif

#if !defined(PPP)
     // Let the reference counts of the A++ arrays be carried with the A++ array 
     // (i.e. SerialArray) objects (i.e. use referenceCount of the doubleSerialArray).
     // This simplifies the implementation of the view and reference functions
     // it is important because views are an important part of A++.  The support
     // of views of views (recursively, etc.) would be more complex that the existing
     // implementation and would be slower since overhead would be required to
     // find the array data (since it must be handed to the MDI functions) would
     // be expensive (though that is a minor point, mostly it is complex).
     // This new implementation was started on (12/2/93).

     // With the class templated we have to have a unique instance of these variables
        static int Array_Reference_Count_Array_Length;
        static int* Array_Reference_Count_Array;

        inline static int getRawDataReferenceCountBase()
           {
          // We want a simple access function for the base value of the reference
          // count.  This value implies that there is a single reference.
             return APP_REFERENCE_COUNT_BASE;
           }

        inline void resetRawDataReferenceCount() const
           {
          // When we steal the data and reuse it from a temporary we have to delete it
          // which decrements the reference count to -1 (typically) and then we copy the
          // data pointers.  But we have to reset the reference cout to ZERO as well.
          // This function allows that operation to be insulated from the details of how the
          // reference count is implemented.  It is used in the A++ operator= ( arrayobject )
             Array_Reference_Count_Array [Array_ID()] = getRawDataReferenceCountBase();
           }

        inline int getRawDataReferenceCount() const
           {
          // Within A++ we can't store the reference count with the array data so we have to
          // provide a seperate array of reference counts indexed by the Array_IDs.
             APP_ASSERT( Array_Reference_Count_Array_Length > Array_ID() );
             return Array_Reference_Count_Array [Array_ID()];
           }

        inline void incrementRawDataReferenceCount() const
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of SerialArray_Domain_Type::incrementRawDataReferenceCount() rc = %d + 1 \n",
                       Array_Reference_Count_Array [Array_ID()]);
#endif
             APP_ASSERT (getRawDataReferenceCount() >= 0);
             Array_Reference_Count_Array [Array_ID()]++;
           }

        inline void decrementRawDataReferenceCount() const
           {
#if COMPILE_DEBUG_STATEMENTS
             if (APP_DEBUG > 0)
                  printf ("Inside of SerialArray_Domain_Type::decrementRawDataReferenceCount() rc = %d - 1 \n",
                       Array_Reference_Count_Array [Array_ID()]);
#endif
             APP_ASSERT (getRawDataReferenceCount() >= 0);
             Array_Reference_Count_Array [Array_ID()]--;
           }
#endif

     // Bug fix (5/3/94) fix for time dependent problems that can generate millions of 
     // array operations and in so doing make Array_Reference_Count_Array TOO long!
     // This allows reuse of Array_IDs and so avoids 
     // the unbounded growth of the Array_Reference_Count_Array
     // static Array_ID_Stack Stack_Of_Array_IDs;

#if defined(APP) || ( defined(SERIAL_APP) && !defined(PPP) )
     // This is a mechanism for detection of the last array being deleted so that
     // if specialized debugging is turned on -- A++ internalmemory can be deleted.
        static int getNumberOfArraysInUse();
#endif
        friend void GlobalMemoryRelease();

     // Error checking functions
        void Test_Distribution_Consistency ( const char *Label ) const;
        void Test_Preliminary_Consistency  ( const char *Label ) const;
        void Test_Consistency              ( const char *Label ) const;

#if !defined(PPP)
     // Error checking support
        static int checkForArrayIDOnStack ( int x );

     // static int queryNextArrayID ();
        static void Push_Array_ID ( int x );

     // DQ (4/18/2001) made Pop_Array_ID static since we need to 
     //                call it from P++ array domain member functions
        static int Pop_Array_ID ();
     // int Pop_Array_ID ();
#endif

#if !defined(PPP)
        static void Allocate_Larger_Reference_Count_Array ( int New_Size );
#endif
        static void New_Function_Loop ();

        void Initialize_View_Of_Target ( int Axis , int x , 
                                         const SerialArray_Domain_Type & Target_Domain );
        void Initialize_View_Of_Target ( int Axis , const Internal_Index & X , 
                                         const SerialArray_Domain_Type & Target_Domain );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , int x , 
                                         const SerialArray_Domain_Type & Target_Domain );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , const Internal_Index & X , 
                                         const SerialArray_Domain_Type & Target_Domain );
        void Initialize_Indirect_Addressing_View_Of_Target ( int Axis , const intSerialArray & X , 
                                         const SerialArray_Domain_Type & Target_Domain );
#if defined(PPP)
        void Compute_Local_Index_Arrays ( const SerialArray_Domain_Type & Original_Domain );
        void Initialize_Parallel_Parts_Of_Domain ( const SerialArray_Domain_Type & X );

     // static int Get_Processor_Position ( int Lhs_Left_Number_Of_Points  , int Rhs_Left_Number_Of_Points  ,
     //                                     int Lhs_Right_Number_Of_Points , int Rhs_Right_Number_Of_Points );
        int Get_Processor_Position ( int Axis ) const;

        static int Overlap_Update_Case_Left_Processor   ( 
                  Array_Conformability_Info_Type  & Array_Set,
                  const Array_Domain_Type       & Lhs_Parallel_Domain,
                  const SerialArray_Domain_Type & Lhs_Serial_Domain,
                  const Array_Domain_Type       & Rhs_Parallel_Domain,
                  const SerialArray_Domain_Type & Rhs_Serial_Domain ,
                  Internal_Index *Lhs_Partition_Index,
                  Internal_Index *Rhs_Partition_Index,
                  int Axis );
        static int Overlap_Update_Case_Middle_Processor ( 
                  Array_Conformability_Info_Type  & Array_Set,
                  const Array_Domain_Type       & Lhs_Parallel_Domain,
                  const SerialArray_Domain_Type & Lhs_Serial_Domain,
                  const Array_Domain_Type       & Rhs_Parallel_Domain,
                  const SerialArray_Domain_Type & Rhs_Serial_Domain ,
                  Internal_Index *Lhs_Partition_Index,
                  Internal_Index *Rhs_Partition_Index,
                  int Axis );
        static int Overlap_Update_Case_Right_Processor  ( 
                  Array_Conformability_Info_Type  & Array_Set,
                  const Array_Domain_Type       & Lhs_Parallel_Domain,
                  const SerialArray_Domain_Type & Lhs_Serial_Domain,
                  const Array_Domain_Type       & Rhs_Parallel_Domain,
                  const SerialArray_Domain_Type & Rhs_Serial_Domain ,
                  Internal_Index *Lhs_Partition_Index,
                  Internal_Index *Rhs_Partition_Index,
                  int Axis);

        static int Check_Ghost_Case_Left_Processor  ( 
                  Array_Conformability_Info_Type  & Array_Set,
                  const Array_Domain_Type       & Parallel_Domain,
                  const SerialArray_Domain_Type & Serial_Domain,
                  Internal_Index *Partition_Index,
                  int Axis );
        static int Check_Ghost_Case_Middle_Processor  ( 
                  Array_Conformability_Info_Type  & Array_Set,
                  const Array_Domain_Type       & Parallel_Domain,
                  const SerialArray_Domain_Type & Serial_Domain,
                  Internal_Index *Partition_Index,
                  int Axis );
        static int Check_Ghost_Case_Right_Processor  ( 
                  Array_Conformability_Info_Type  & Array_Set,
                  const Array_Domain_Type       & Parallel_Domain,
                  const SerialArray_Domain_Type & Serial_Domain,
                  Internal_Index *Partition_Index,
                  int Axis );

        static int Check_Global_Values ( 
                  Array_Conformability_Info_Type  & Array_Set,
                  const Array_Domain_Type       & Lhs_Parallel_Domain,
                  const Array_Domain_Type       & Rhs_Parallel_Domain,
                  int Axis);

     // These functions will be removed eventually and there equivalent
     // functionality will be put into the above function
     // ... NOTE: this is an added function that actually modifies
     //  Rhs_Partition_Index. (Array_Set probably isn't needed here.) ...
        static void Overlap_Update_Fix_Rhs( 
                  Array_Conformability_Info_Type  & Array_Set,
                  const Array_Domain_Type       & Lhs_Parallel_Domain,
                  const SerialArray_Domain_Type & Lhs_Serial_Domain,
                  const Array_Domain_Type       & Rhs_Parallel_Domain,
                  const SerialArray_Domain_Type & Rhs_Serial_Domain ,
                  Internal_Index *Lhs_Partition_Index,
                  Internal_Index *Rhs_Partition_Index,
                  int Axis );
      // ... (10/11/96,kdb) new function to make a compatible 
      //  Array_Conformability_Info when Lhs isn't a temporary
      //  so usual returned value is incorrect ...
        static void Fix_Array_Conformability( 
                  Array_Conformability_Info_Type    & Array_Set,
                  const Array_Domain_Type       & Parallel_Domain,
                  const SerialArray_Domain_Type & Serial_Domain,
		  bool                             reverse_offset);
#endif

     // void Fixup_Array_Domain_For_Copy_Constructor ( const SerialArray_Domain_Type & Target_Domain );

     public:
     // Used to free memory-in-use internally in SerialArray_Domain_Type objects! 
     // Eliminates memory-in-use errors from Purify
        static void freeMemoryInUse();

#if defined(PPP)
     // We would like to have this function available in P++ and Serial array objects
     // but we use only the SerialArray_Domain_Type::Array_Reference_Count_Array_Length
     // so this function mearly accesses the SerialArray static member function
        static int getMaxNumberOfArrays() { return SerialArray_Domain_Type::getMaxNumberOfArrays(); };
#else
     // Access function to Array_Reference_Count_Array_Length which stores the
     // number of elements in the Array_Reference_Count_Array which is
     // the max number of arrays that exist at any one point in time within
     // a given application.
        static int getMaxNumberOfArrays() { return Array_Reference_Count_Array_Length; };
#endif

#ifdef INLINE_FUNCTIONS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        static void operator delete (void* Pointer, size_t Size);

     // ********************************************************************************
     // ********************************************************************************
     // *******************  SerialArray_Domain_Type DESTRUCTORS  ********************
     // ********************************************************************************
     // ********************************************************************************

       ~SerialArray_Domain_Type();

     // ********************************************************************************
     // ********************************************************************************
     // *************************  CONSTRUCTOR INITIALIZATION  *************************
     // ********************************************************************************
     // ********************************************************************************

        void Initialize_Non_Indexed_Dimension ( int x , const SerialArray_Domain_Type & X );
        void Initialize_Indirect_Addressing_View_Of_Target_Non_Indexed_Dimension ( int x , const SerialArray_Domain_Type & X );

        void Preinitialize_Domain ( const SerialArray_Domain_Type & X );

     // Is this function used anywhere?
     // 1. This function calls NO other Initialize_Domain functions
        void Initialize_Domain ( int Number_Of_Valid_Dimensions ,
                                 const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // 2. This function CALLs function Initialize_Domain #1
        void Initialize_Domain ( int Number_Of_Valid_Dimensions ,
                                 const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

     // 3. This function CALLs function Initialize_Domain #1
        void Initialize_Domain 
	   ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

     // 4. This function CALLs function Initialize_Domain #?
     // this function represents support for indexing operations
        void Initialize_Domain
           ( const SerialArray_Domain_Type & X,
             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type
	     Internal_Index_List );

     // 5. This function CALLs function Initialize_Domain #?
     // this function represents support for indirect addressing indexing operations
        void Initialize_Domain
           ( const SerialArray_Domain_Type & X,
             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type
	     Indirect_Index_List );

#if defined(APP) || defined(PPP)
     // 6. This function CALLs function Initialize_Domain #1
        void Initialize_Domain 
	   ( int Number_Of_Valid_Dimensions,
	     const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List,
             const Internal_Partitioning_Type & Internal_Partition );

     // 7. This function is not implemented (P++ currently lacks support to handle an arbitrary existing distribution)
        void Initialize_Domain ( ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE );

     // 8. This function CALLs function Initialize_Domain #6
        void Initialize_Domain
	   ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
             const Internal_Partitioning_Type & Internal_Partition );

     // 9. This function CALLs function Initialize_Domain #1
        void Initialize_Domain
	   ( int Number_Of_Valid_Dimensions,
	     const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ,
             const Internal_Partitioning_Type & Internal_Partition );
#endif

     // Support specific to indirect addressing
        void Initialize_IndirectAddressing_Domain_Using_IndirectAddressing
                  ( const SerialArray_Domain_Type & X,
                    const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

        void Initialize_NonindirectAddressing_Domain_Using_IndirectAddressing
                  ( const SerialArray_Domain_Type & X,
                    const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );

#if defined(PPP)
     // Support for P++ (case where serialArray is a NullArray (view or non-view))
        void resetDomainForNullArray();
#endif

     // Support for reshape function
        void reshape
	   ( int Number_Of_Valid_Dimensions, 
	     const int* View_Sizes, const int* View_Bases );

     // Support function for computing the valid number of dimensions from the Index list
        static int computeDimension ( const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type internalIndexList );
        static int computeDimension ( const Integer_Array_MAX_ARRAY_DIMENSION_Type integerList );

     // ********************************************************************************
     // ********************************************************************************
     // *******************  SerialArray_Domain_Type CONSTRUCTORS  *******************
     // ********************************************************************************
     // ********************************************************************************

     // ********************************************************************************
     // ************************** A++ Constructors ************************************
     // ********************************************************************************
        SerialArray_Domain_Type();
        SerialArray_Domain_Type( int Number_Of_Valid_Dimensions , 
                             const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size );
        SerialArray_Domain_Type( int Number_Of_Valid_Dimensions ,
                             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List );

        SerialArray_Domain_Type    ( ARGUMENT_LIST_MACRO_INTEGER );

  // We need these because the array object constructors take a different
  // number of parameteres to represent the size in each axis and we
  // have to call constructors for the member objects within the array objects
  // within the preinitialization lists for the array objects.  So we need 
  // constructors for the Array_Descriptor_Type and Array_Domain_Type objects
  // that have a similar interface to the constructors in the array class interface.
  // This simplifies the preinitialization lists.
        SerialArray_Domain_Type ( const Internal_Index & I );
#if (MAX_ARRAY_DIMENSION >= 2)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Index & M );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Index & M, const Internal_Index & N );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Index & M, const Internal_Index & N,
                              const Internal_Index & O );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Index & M, const Internal_Index & N,
                              const Internal_Index & O, const Internal_Index & P );
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif


        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
                              bool AvoidBuildingIndirectAddressingView = FALSE, 
                              int Type_Of_Copy = DEEPCOPY );
#if (defined(SERIAL_APP) || defined(USE_PADRE)) && !defined(PPP)
     // This is here for use with PADRE
        SerialArray_Domain_Type ( const Array_Domain_Type & X,
                                  bool AvoidBuildingIndirectAddressingView = FALSE, 
                                  int Type_Of_Copy = DEEPCOPY );
        void normalize();
#endif

     // SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
     //                           Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type* Index_List );
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List );

        SerialArray_Domain_Type ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );

#if 0
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Internal_Index & I );
#if (MAX_ARRAY_DIMENSION >= 2)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Internal_Index & I, const Internal_Index & J );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Internal_Index & I, const Internal_Index & J, 
                                  const Internal_Index & K );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Internal_Index & I, const Internal_Index & J, 
                                  const Internal_Index & K, const Internal_Index & L );

#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Internal_Index & I, const Internal_Index & J, 
                                  const Internal_Index & K, const Internal_Index & L,
                                  const Internal_Index & M );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Internal_Index & I, const Internal_Index & J, 
                                  const Internal_Index & K, const Internal_Index & L,
                                  const Internal_Index & M, const Internal_Index & N );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Internal_Index & I, const Internal_Index & J, 
                                  const Internal_Index & K, const Internal_Index & L,
                                  const Internal_Index & M, const Internal_Index & N,
                                  const Internal_Index & O );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X, 
                                  const Internal_Index & I, const Internal_Index & J, 
                                  const Internal_Index & K, const Internal_Index & L,
                                  const Internal_Index & M, const Internal_Index & N,
                                  const Internal_Index & O, const Internal_Index & P );
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif
#endif


        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Indirect_Index_List );
#if 0
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Internal_Indirect_Addressing_Index & I );
#if (MAX_ARRAY_DIMENSION >= 2)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
             const Internal_Indirect_Addressing_Index & K );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
             const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
             const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
             const Internal_Indirect_Addressing_Index & M );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
             const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
             const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
             const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
             const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N,
             const Internal_Indirect_Addressing_Index & O );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        SerialArray_Domain_Type ( const SerialArray_Domain_Type & X,
             const Internal_Indirect_Addressing_Index & I, const Internal_Indirect_Addressing_Index & J,
             const Internal_Indirect_Addressing_Index & K, const Internal_Indirect_Addressing_Index & L,
             const Internal_Indirect_Addressing_Index & M, const Internal_Indirect_Addressing_Index & N,
             const Internal_Indirect_Addressing_Index & O, const Internal_Indirect_Addressing_Index & P );
#endif

#if (MAX_ARRAY_DIMENSION > 8)
#error MAX_ARRAY_DIMENSION exceeded!
#endif
#endif

     // ********************************************************************************
     // ************************** P++ Constructors ************************************
     // ********************************************************************************
#if defined(PPP)
        SerialArray_Domain_Type    ( ARGUMENT_LIST_MACRO_INTEGER_AND_CONST_REF_RANGE );
#endif

     // ********************************************************************************
     // ************************ A++ and P++ Constructors ******************************
     // ********************************************************************************
#if defined(APP) || defined(PPP)
     // SerialArray_Domain_Type( int Array_Size_I , int Array_Size_J , 
     //                          int Array_Size_K , int Array_Size_L , 
     //                          const Partitioning_Type & Partition );
        SerialArray_Domain_Type
	   ( int Number_Of_Valid_Dimensions , 
             const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
             const Internal_Partitioning_Type & Internal_Partition );
        SerialArray_Domain_Type
	   ( const Integer_Array_MAX_ARRAY_DIMENSION_Type Array_Size ,
             const Internal_Partitioning_Type & Internal_Partition );

        SerialArray_Domain_Type
           ( int Number_Of_Valid_Dimensions ,
             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Internal_Index_List ,
             const Internal_Partitioning_Type & Internal_Partition );

        SerialArray_Domain_Type ( const Internal_Index & I,
                              const Internal_Partitioning_Type & Internal_Partition );
#if (MAX_ARRAY_DIMENSION >= 2)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 3)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K,
                              const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 4)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 5)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Index & M,
                              const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 6)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Index & M, const Internal_Index & N,
                              const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 7)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Index & M, const Internal_Index & N,
                              const Internal_Index & O,
                              const Internal_Partitioning_Type & Internal_Partition );
#endif
#if (MAX_ARRAY_DIMENSION >= 8)
        SerialArray_Domain_Type ( const Internal_Index & I, const Internal_Index & J,
                              const Internal_Index & K, const Internal_Index & L,
                              const Internal_Index & M, const Internal_Index & N,
                              const Internal_Index & O, const Internal_Index & P,
                              const Internal_Partitioning_Type & Internal_Partition );
#endif
#endif

#if defined(USE_STRING_SPECIFIC_CODE)
     // Until we have verified portability of the string functions we require we make
     // the complilation of this code conditional!
     // void parseString ( const AppString & dataString, int & pos , int dimentionArray[] );
        void parseString ( const char* dataString, int & pos , int dimentionArray[] );
#endif

     // Constructor for initialization from a string
     // SerialArray_Domain_Type ( const char* );
        SerialArray_Domain_Type ( const AppString & X );

     // ********************************************************************************
     // ********************************************************************************
     // ******************** ERROR CHECKING MEMBER FUNCTIONS  **************************
     // ********************************************************************************
     // ********************************************************************************
     // function interfaces must be dimension independent to permit arbitrary dimension A++/P++ array objects
     // void Error_Checking_For_Index_Operators (
     //      const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndexParameterList, 
     //      const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndirectIndexParameterList );
        void Error_Checking_For_Index_Operators (
             const Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndexParameterList ) const;
        void Error_Checking_For_Index_Operators (
             const Indirect_Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type IndirectIndexParameterList ) const;

        void Error_Checking_For_Scalar_Index_Operators ( 
             const Integer_Pointer_Array_MAX_ARRAY_DIMENSION_Type Integer_List ) const;

     // These are used as part of error checking support (for test codes)
        void equalityTest ( bool & returnValue, bool test, const char* description ) const;
        bool operator== ( const SerialArray_Domain_Type & X ) const;

     // ********************************************************************************
     // ********************************************************************************
     // ***************************  OPERATOR EQUALS  **********************************
     // ********************************************************************************
     // ********************************************************************************
        SerialArray_Domain_Type & operator= ( const SerialArray_Domain_Type & X );

     // This builds a descriptor to represent a 1D (linear) view any data described by the input descriptor.
        static SerialArray_Domain_Type *Vectorizing_Domain ( const SerialArray_Domain_Type & X );

#if defined(PPP)
        void partition ( const Internal_Partitioning_Type & Internal_Partition );
#if !defined(USE_PADRE)
        DARRAY* Build_BlockPartiArrayDomain ();
#endif
        static void fixupLocalBase ( 
                              Array_Domain_Type & New_Parallel_Domain,
                        SerialArray_Domain_Type & New_Serial_Domain,
                  const       Array_Domain_Type & Old_Parallel_Domain,
                  const SerialArray_Domain_Type & Old_Serial_Domain );
        void Update_Parallel_Information_Using_Old_Domain ( const SerialArray_Domain_Type & Old_Array_Domain_Type );
#endif

        static SerialArray_Domain_Type* Build_Temporary_By_Example ( const SerialArray_Domain_Type & X );
     // void Convert_View_To_Nonview ();

        void setBase ( int New_Base_For_All_Axes );
        void setBase ( int New_Base , int Axis );

        int Array_Size () const;
        int size() const { return Array_Size(); };
        void getRawDataSize ( int* Sizes ) const;
        inline Range dimension ( int Axis ) const;
        inline Range getFullRange ( int Axis ) const;
        inline int getBase     ( int Axis ) const;
        inline int getBound    ( int Axis ) const;
        inline int getStride   ( int Axis ) const;
        inline int getLength   ( int Axis ) const;
        inline int getDataBase ( int Axis ) const { return Data_Base[Axis]; }

     // Low level function to access ghost boundary width
        int getGhostBoundaryWidth ( int Axis ) const;

     // These are the base bound stride and size data which are independent
     // of the View mechanism which hides the details of base bound stride
     // access to the actual data.  for example a stride of a view using the
     // getStride function is always 1 but the getRawStride is the actual
     // stride of the raw data.  However the getRawBase is the access into
     // the raw data offset by the Data_Base and so is a different sort 
     // of representation of the base of the data.
        int getRawDataSize ( int Axis ) const;
        inline int getRawBase   ( int Axis ) const;
        inline int getRawBound  ( int Axis ) const;
        inline int getRawStride ( int Axis ) const;

     // These three functions are all different
        inline static int maxNumberOfDimensions ();     // returns MAX_ARRAY_DIMENSION
        inline int numberOfDimensions () const;         // returns Domain_Dimension member variable
        inline int internalNumberOfDimensions () const; // returns a computed value --- number of 
                                                        // non-unit dimensions (see code)

        inline bool usesIndirectAddressing () const { return Uses_Indirect_Addressing; }
        inline void setTemporary( bool x ) const { ((SerialArray_Domain_Type*)(this))->Is_A_Temporary = x; }
        //inline void setTemporary( bool x ) const { ((Array_Domain_Type*)(this))->Is_A_Temporary = x; }
        inline bool isTemporary() const { return Is_A_Temporary; }
        inline bool isNullArray() const { return Is_A_Null_Array; }
        inline bool isView() const { return Is_A_View; }
        inline bool isContiguousData() const { return Is_Contiguous_Data; }

#if defined(PPP)
     // These are only defined in the case of using PPP (so they are 
     // for the parallel domain object not the serial domain object)
     // PADRE required functions
        inline void setLeftNumberOfPoints (int value, int i) {
	  Left_Number_Of_Points [i] = value;
	}
        inline void setRightNumberOfPoints(int value, int i) {
	  Right_Number_Of_Points[i] = value;
	}
        inline void setGlobalIndex(int base, int bound, int stride, int i) 
             { Global_Index[i] = Range(base,bound,stride); }
        inline void setLocalMaskIndex(int base, int bound, int stride, int i) 
             { Local_Mask_Index[i] = Range(base,bound,stride); }

     // We return by value here to prevent the corruption of data
        inline Internal_Index getLocalMaskIndex  (int i) const { return Local_Mask_Index[i]; }
        inline Internal_Index getGlobalMaskIndex (int i) const { return Global_Index[i]; }

     // PADRE required functions (required in parallel Array_Domain_Type)
        inline int getLocalMaskBase    (int i) const { return Local_Mask_Index[i].getBase();   }
        inline int getLocalMaskBound   (int i) const { return Local_Mask_Index[i].getBound();  }
        inline int getLocalMaskStride  (int i) const { return Local_Mask_Index[i].getStride(); }
        inline int getLocalMaskLength  (int i) const { return Local_Mask_Index[i].getLength(); }

        inline int getGlobalMaskBase   (int i) const { return Global_Index[i].getBase();   }
        inline int getGlobalMaskBound  (int i) const { return Global_Index[i].getBound();  }
        inline int getGlobalMaskStride (int i) const { return Global_Index[i].getStride(); }
        inline int getGlobalMaskLength (int i) const { return Global_Index[i].getLength(); }

        inline int getLeftNumberOfPoints  ( int Axis ) const { return Left_Number_Of_Points[Axis];  }
        inline int getRightNumberOfPoints ( int Axis ) const { return Right_Number_Of_Points[Axis]; }
#endif

     // ****************************************************************************************
     // ******************************   PADRE SPECIFIC CODE   *********************************
     // ****************************************************************************************
     // PADRE required functions (required in both SerialArray_Domain_Type and Array_Domain_Type)
     // These are the values of the field variables within the object.
        inline int getBaseVariable     ( int Axis ) const { return Base[Axis];  }
        inline int getBoundVariable    ( int Axis ) const { return Bound[Axis]; }
        inline int getStrideVariable   ( int Axis ) const { return Stride[Axis]; }
        inline int getDataBaseVariable ( int Axis ) const { return Data_Base[Axis]; }
        inline int getViewOffsetVariable ()         const { return View_Offset; }

        inline void setSizeVariable           (int value, int i)      { Size[i] = value; }
        inline void setContiguousDataVariable (bool value)         { Is_Contiguous_Data = value; }
        inline void setViewVariable                (bool value)    { Is_A_View = value; }
        inline void setViewOffsetVariable          (int value)        { View_Offset = value; }
        inline void setScalarOffsetVariable        (int value, int i) { Scalar_Offset[i] = value; }
        inline void setBaseVariable                (int value, int i) { Base[i] = value; }
        inline void setBoundVariable               (int value, int i) { Bound[i] = value; }
        inline void setDataBaseVariable            (int value, int i) { Data_Base[i] = value; }
        inline void setUserBaseVariable            (int value, int i) { User_Base[i] = value; }
        inline void setNull                        ()                 { Is_A_Null_Array = TRUE; }
     // ****************************************************************************************

     // PADRE required functions (required in both SerialArray_Domain_Type and Array_Domain_Type)
        inline bool isNull                   ()      const { return isNullArray(); }
        inline int getNumberOfDimensions        ()      const { return numberOfDimensions(); }
        inline int getUserBaseVariable          (int i) const { return User_Base[i]; }
        inline bool isAView                  ()      const { return isView(); }
        inline bool IsContiguousDataVariable ()      const { return Is_Contiguous_Data; }
     // inline int getViewOffsetVariable        ()      const { return View_Offset; }
        inline int getSizeVariable              (int i) const { return Size[i]; }
        inline int getScalarOffsetVariable      (int i) const { return Scalar_Offset[i]; }

     // Dimension independent query functions
        bool isSameBase   ( const SerialArray_Domain_Type & X ) const;
        bool isSameBound  ( const SerialArray_Domain_Type & X ) const;
        bool isSameStride ( const SerialArray_Domain_Type & X ) const;
        bool isSameLength ( const SerialArray_Domain_Type & X ) const;
#if defined(APP) || defined(PPP)
        bool isSameGhostBoundaryWidth (const SerialArray_Domain_Type & X ) const;
        bool isSameDistribution ( const SerialArray_Domain_Type & X ) const;
#endif

     // We have a definition of similar which means that the 
     // bases bounds and strides are the same.
        bool isSimilar ( const SerialArray_Domain_Type & X ) const;

     // void expandGhostBoundariesOfRawData ( SerialArray_Domain_Type & X, 
     //                                       ARGUMENT_LIST_MACRO_INTEGER_WITH_PREINITIALIZATION_ZERO );
     // void expandGhostBoundariesOfRawData ( SerialArray_Domain_Type & X, 
     //                                       Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        void setInternalGhostCellWidth ( Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List );
        void resize ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE );
        void reshape ( ARGUMENT_LIST_MACRO_CONST_REF_RANGE );

#if defined(PPP)
     // Is a P++ array partitioned along an axis
        static bool isPartitioned ( const Array_Domain_Type & Parallel_Domain, 
                                       const SerialArray_Domain_Type & Serial_Domain, int Axis );

     // WARNING: MAKE PUBLIC SO DESCRIPTOR CAN USE FOR NOW
     //private:
     public:
        bool isLeftPartition  ( int Axis ) const;
        bool isMiddlePartition( int Axis ) const;
        bool isRightPartition ( int Axis ) const;

     // Case of array object not present on local processor 
     // Since not all processors have to have a partition of every array object
        bool isNonPartition   ( int Axis ) const;

     // Used in allocate.C to determin position of null array objects in distributions
        bool isLeftNullArray ( SerialArray_Domain_Type & serialArrayDomain , int Axis ) const;
        bool isRightNullArray( SerialArray_Domain_Type & serialArrayDomain , int Axis ) const;

     public:
	int findProcNum (int*) const;
	/*
	int* setupProcessorList 
	   (const intSerialArray& I, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (const intSerialArray& I, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorList 
	   (int* I_A,  int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (int* I_A, int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	*/
	int* setupProcessorList 
	   (intSerialArray** Index_Arrays, int* global_I_base,
	    int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (intSerialArray** Index_Arrays, int* global_I_base,
	    int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorList 
	   (int** I_A,  int list_size, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
	int* setupProcessorListOnPosition 
	   (int* I_A, int list_size, int I_A_base, int& numberOfSupplements,
	    int** supplementLocationsPtr, int** supplementDataPtr,
	    int& numberOfLocal, int** localLocationsPtr, int** localDataPtr) const;
#endif

     // These are intended for use in the debugging of A++/P++ arrays
     // static int computeArrayDimension ( int Array_Size_I, int Array_Size_J, int Array_Size_K, int Array_Size_L );
        static int computeArrayDimension ( int* Array_Sizes );
        static int computeArrayDimension ( const SerialArray_Domain_Type & X );

        void display ( const char *Label = "" ) const;
        SerialArray_Domain_Type & displayReferenceCounts (const char* label) const;

#if defined(PPP)
     // These functions are used to get or make the Array_Conformability_Info_Type objects
     // passed between subexpressions in the evaluation of expressions of parallel array objects
        static Array_Conformability_Info_Type* getArray_Conformability_Info ();
        static Array_Conformability_Info_Type* getArray_Conformability_Info (
                  const Array_Domain_Type & X_Domain );
        static Array_Conformability_Info_Type* getArray_Conformability_Info (
                  const Array_Domain_Type & Lhs_Domain ,
                  const Array_Domain_Type & Rhs_Domain );
        static Array_Conformability_Info_Type* getArray_Conformability_Info (
                  const Array_Domain_Type & This_Domain ,
                  const Array_Domain_Type & Lhs_Domain ,
                  const Array_Domain_Type & Rhs_Domain );
        static void Test_For_Sufficient_Ghost_Boundary_Width (
                  const Array_Domain_Type & Lhs_Parallel_Domain , 
                  const SerialArray_Domain_Type & Lhs_Serial_Domain , 
                  const Array_Domain_Type & Rhs_Parallel_Domain ,
                  const SerialArray_Domain_Type & Rhs_Serial_Domain , int Axis );
        static int Compute_Overlap_Update (
                  int Processor_Position,
                  Array_Conformability_Info_Type & Return_Array_Set,
                  const Array_Domain_Type & Lhs_Domain,
                  const SerialArray_Domain_Type & Lhs_Serial_Domain,
                  const Array_Domain_Type & Rhs_Domain,
                  const SerialArray_Domain_Type & Rhs_Serial_Domain,
                  Internal_Index* Lhs_Partition_Index, 
		  Internal_Index* Rhs_Partition_Index, int Axis); 

        static int Check_For_Ghost_Region_Only (
                  int Processor_Position,
                  Array_Conformability_Info_Type & Return_Array_Set,
                  const Array_Domain_Type & Domain,
                  const SerialArray_Domain_Type & Serial_Domain,
                  Internal_Index* Partition_Index, int Axis );

     // static int Interpret_Message_Passing ( Array_Conformability_Info_Type & Array_Set,
     //                                        const Array_Domain_Type & Lhs_Array_Domain_Type,
     //                                        int* VSG_Model );
        static int Interpret_Message_Passing ( Array_Conformability_Info_Type & Array_Set,
                                               const Array_Domain_Type & Lhs_Parallel_Domain,
                                               const SerialArray_Domain_Type & Lhs_Serial_Domain,
                                               const Array_Domain_Type & Rhs_Parallel_Domain,
                                               const SerialArray_Domain_Type & Rhs_Serial_Domain,
                                               Internal_Index* Lhs_Partition_Index, Internal_Index* Rhs_Partition_Index);
#endif

#if !defined(PPP)
        static void Put_Domain_Into_Cache ( SerialArray_Domain_Type* & X );

        static SerialArray_Domain_Type *Get_Domain_From_Cache ( 
             int Array_Size_I , int Array_Size_J , int Array_Size_K , int Array_Size_L );
        static SerialArray_Domain_Type *Get_Domain_From_Cache ( const SerialArray_Domain_Type & X );
        static SerialArray_Domain_Type *Get_Domain_From_Cache ();
        SerialArray_Domain_Type *Get_Domain_From_Cache ( const Internal_Index & I );
        SerialArray_Domain_Type *Get_Domain_From_Cache ( const Internal_Index & I , const Internal_Index & J );
        SerialArray_Domain_Type *Get_Domain_From_Cache ( const Internal_Index & I , const Internal_Index & J , 
                                                             const Internal_Index & K );
        SerialArray_Domain_Type *Get_Domain_From_Cache ( const Internal_Index & I , const Internal_Index & J , 
                                                             const Internal_Index & K , const Internal_Index & L );
#endif

      // functions specific to expression templates
      bool continuousData() const { return Is_Contiguous_Data; };

#if defined(SERIAL_APP)
        void adjustSerialDomainForStridedAllocation ( int axis,  const Array_Domain_Type & globalDomain );
#endif

#if defined(PPP)
     // Common support for PADRE and non-PADRE versions of P++ initialization of the descriptor and domain as part of allocation
        void postAllocationSupport ( SerialArray_Domain_Type & serialArrayDomain );
#endif

   };


#endif  /* !defined(_APP_DOMAIN_H) */





