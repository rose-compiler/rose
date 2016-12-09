// Allow repeated includes of lazyarray.h without error
#ifndef _APP_LAZY_AGGREGATE_H
#define _APP_LAZY_AGGREGATE_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif


// we need the hash defines because the cases in a switch 
// statement can't be constants!
#define DOUBLEARRAY_TYPE_CODE 1001
#define FLOATARRAY_TYPE_CODE  1002
#define INTARRAY_TYPE_CODE    1003

class doubleArray;  // Forward declaration!
class floatArray;   // Forward declaration!
class intArray;     // Forward declaration!

#ifdef PPP
class doubleSerialArray;  // Forward declaration!
class floatSerialArray;   // Forward declaration!
class intSerialArray;     // Forward declaration!
#endif

class Task_Type;    // Forward declaration!
class Set_Of_Tasks; // Forward declaration!


#ifdef INLINE_FUNCTIONS
#define INLINE_NEW_OPERATORS
#endif





// This uses an abstract base class!
class doubleSerialArray_Aggregate_Operator : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_Aggregate Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Result;
        doubleSerialArray** Operand_List;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Aggregate_Operator *Current_Link;
        doubleSerialArray_Aggregate_Operator *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     public:
       ~doubleSerialArray_Aggregate_Operator ();
        doubleSerialArray_Aggregate_Operator ();
        doubleSerialArray_Aggregate_Operator ( 
                             int Number_Of_Operators ,
                             int Input_Operation_Code , 
                             MDI_double_Prototype_Aggregate Input_Function_Pointer , 
                             doubleSerialArray* Result,
                             doubleSerialArray** Input_Operand_List );
        doubleSerialArray_Aggregate_Operator ( 
                             int Number_Of_Operators ,
                             MDI_double_Prototype_Aggregate Input_Function_Pointer , 
                             doubleSerialArray* Result,
                             doubleSerialArray** Input_Operand_List );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
   };

// This uses an abstract base class!
class floatSerialArray_Aggregate_Operator : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_Aggregate Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Result;
        floatSerialArray** Operand_List;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Aggregate_Operator *Current_Link;
        floatSerialArray_Aggregate_Operator *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     public:
       ~floatSerialArray_Aggregate_Operator ();
        floatSerialArray_Aggregate_Operator ();
        floatSerialArray_Aggregate_Operator ( 
                             int Number_Of_Operators ,
                             int Input_Operation_Code , 
                             MDI_float_Prototype_Aggregate Input_Function_Pointer , 
                             floatSerialArray* Result,
                             floatSerialArray** Input_Operand_List );
        floatSerialArray_Aggregate_Operator ( 
                             int Number_Of_Operators ,
                             MDI_float_Prototype_Aggregate Input_Function_Pointer , 
                             floatSerialArray* Result,
                             floatSerialArray** Input_Operand_List );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
   };


#define INTARRAY
// This uses an abstract base class!
class intSerialArray_Aggregate_Operator : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_Aggregate Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        intSerialArray** Operand_List;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Aggregate_Operator *Current_Link;
        intSerialArray_Aggregate_Operator *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator delete (void* Pointer, size_t Size);

     public:
       ~intSerialArray_Aggregate_Operator ();
        intSerialArray_Aggregate_Operator ();
        intSerialArray_Aggregate_Operator ( 
                             int Number_Of_Operators ,
                             int Input_Operation_Code , 
                             MDI_int_Prototype_Aggregate Input_Function_Pointer , 
                             intSerialArray* Result,
                             intSerialArray** Input_Operand_List );
        intSerialArray_Aggregate_Operator ( 
                             int Number_Of_Operators ,
                             MDI_int_Prototype_Aggregate Input_Function_Pointer , 
                             intSerialArray* Result,
                             intSerialArray** Input_Operand_List );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
   };

#undef INTARRAY

#endif  /* !defined(_APP_LAZY_AGGREGATE_H) */






