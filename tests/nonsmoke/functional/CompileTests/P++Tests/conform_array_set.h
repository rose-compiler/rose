// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif

#if !defined(PPP)
#error (conform_enforce.C) This is only code for P++
#endif

// OLD NAME -- class Conformable_Array_Set_Type
class Array_Conformability_Info_Type
   {
  // This class represents that data from the LHS and RHS of every
  // binary operation.  It is initialized with the original LHS
  // (the partition for the current processor) and the RHS that is
  // obtained though message passing (if VSG update is used) or
  // from the ghost boundaries if the references are sufficiently aligned.

     public:
#if 0
       // Used to describe the conformable part of a partition 
       // (only makes sense relative to another $1Array_Conformability_Info object)
       // Index Lhs_Lvalue_Assignment_Index [MAX_ARRAY_DIMENSION];

       // P++ message passing info required for propogation through expressions.
       // Do we need two sets of these variable for the left and right operands of each 
       // binary operator?
       // bool Left_Boundary_Accessed_Flag                  [MAX_ARRAY_DIMENSION];
       // bool Right_Boundary_Accessed_Flag                 [MAX_ARRAY_DIMENSION];

       // We will worry about this later
       // bool Left_Overlap_Accessed_Flag                   [MAX_ARRAY_DIMENSION];
       // bool Right_Overlap_Accessed_Flag                  [MAX_ARRAY_DIMENSION];

       // I forget how these interact with the Lhs_Lvalue_Assignment_Index which is an Index object
       // int Lhs_Aggregate_Start_Truncation                   [MAX_ARRAY_DIMENSION];
       // int Lhs_Aggregate_End_Truncation                     [MAX_ARRAY_DIMENSION];

       // I don't know if these are really required!
       // int Aggregate_Lhs_Truncate_Start_In_Middle_Processor [MAX_ARRAY_DIMENSION];
       // int Aggregate_Lhs_Truncate_End_In_Middle_Processor   [MAX_ARRAY_DIMENSION];

       // bool Last_Update_Was_Overlap_Update;
#endif
          bool Full_VSG_Update_Required;

       // Amount of ghost boundaries which should be avoided in subsequent operations
          int Truncate_Left_Ghost_Boundary_Width  [MAX_ARRAY_DIMENSION];
          int Truncate_Right_Ghost_Boundary_Width [MAX_ARRAY_DIMENSION];

       // This records if an operation's indexing of the lhs and Rhs
       // should later force an update of the left of right ghost boundary
       // in the operator= (or other statement terminating array operation (like replace)).
          bool Update_Left_Ghost_Boundary_Width [MAX_ARRAY_DIMENSION];
          bool Update_Right_Ghost_Boundary_Width [MAX_ARRAY_DIMENSION];

       // This is a positive or negative (or zero) value which records 
       // how the data is stored. For a nontemporary this value should be ZERO
       // but for a temporary this value is used to allow the partitioning to
       // of the temporary to be identical to that of the Lhs of each binary operation.
       // even if this results in an offset which is inconsistant with the placement
       // of the data where we would want to center it.
          int Offset_To_Centered_Data_Storage [MAX_ARRAY_DIMENSION];

          int referenceCount;

      // Offset two the Left and Right Number_Of_Points in the Array_Descriptor
         int Left_Number_Of_Points_Truncated  [MAX_ARRAY_DIMENSION];
         int Right_Number_Of_Points_Truncated [MAX_ARRAY_DIMENSION];

         int Data_Storage_Offset_In_Temporary [MAX_ARRAY_DIMENSION];

      // Data required for the "new" and "delete" operators!
         static Array_Conformability_Info_Type *Current_Link;
         Array_Conformability_Info_Type *freepointer;
         static int CLASS_ALLOCATION_POOL_SIZE;
         static const int Max_Number_Of_Memory_Blocks;
         static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
         static int Memory_Block_Index;

       // Function interface
         ~Array_Conformability_Info_Type ();
          Array_Conformability_Info_Type ();
          //Array_Conformability_Info_Type ( const Array_Descriptor_Type & X );
          Array_Conformability_Info_Type ( const Array_Domain_Type & X );

          void Initialize ();
          void display          ( const char* Label = "") const;
          void Test_Consistency ( const char *Label = "") const;
 
#if defined(INLINE_FUNCTIONS)
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator  delete (void* Pointer, size_t Size);

        static void freeMemoryInUse();

       // Support functions for reference counting (inlined for performance)
          inline void decrementReferenceCount() const { ((Array_Conformability_Info_Type*) this)->referenceCount--; }
          inline void incrementReferenceCount() const { ((Array_Conformability_Info_Type*) this)->referenceCount++; }

          inline static int getReferenceCountBase()
             {
            // This is the base value for the reference counts
            // This value is used to record a single reference to the object
            // Currently this value is zero but we want to change it
            // to be 1 and we want to have it be specified in a single place
            // within the code so we use this access function.
               return APP_REFERENCE_COUNT_BASE;
             }

       // Use of reference count access is made available for debugging
          int getReferenceCount() const
             {
            // if (referenceCount < getReferenceCountBase()+1)
            //      printf ("referenceCount < getReferenceCountBase()-1: referenceCount = %d getReferenceCountBase() = %d \n",
            //           referenceCount,getReferenceCountBase());
               APP_ASSERT (referenceCount >= getReferenceCountBase()-1);
               return referenceCount;
             }

          inline void resetReferenceCount()
             {
           // reset the Reference Count in the array object!
              referenceCount = getReferenceCountBase();
             }

     private:
          Array_Conformability_Info_Type & operator= ( const Array_Conformability_Info_Type & X );
          Array_Conformability_Info_Type ( const Array_Conformability_Info_Type & X );
   };

