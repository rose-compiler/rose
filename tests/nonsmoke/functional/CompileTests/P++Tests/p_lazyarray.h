// Allow repeated includes of lazyarray.h without error
#ifndef _PPP_LAZYARRAY_H
#define _PPP_LAZYARRAY_H

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
class doubleArray_Function_Steal_Data : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!

     // Storage for function operands!
     // Note that we cannot use a reference here since then the object 
     // referenced will be deleted (destructor called by the compiler)!
        const doubleArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_Steal_Data *Current_Link;
        doubleArray_Function_Steal_Data *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

        bool Execute_MDI_Function;

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_Steal_Data ( int Input_Operation_Code , 
                                      const doubleArray & Result , const doubleArray & X );
        doubleArray_Function_Steal_Data ( const doubleArray & Result , const doubleArray & X );
        void Execute ();
     // void Display_Statement_List ();  // Print out operand and operator codes!
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );

        void Skip_MDI_Function();
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_0 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_0 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_0 *Current_Link;
        doubleArray_Function_0 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_0 ( int Input_Operation_Code , 
                             MDI_double_Prototype_0 Input_Function_Pointer , 
                             const doubleArray & Result , const doubleArray & X );
        doubleArray_Function_0 ( MDI_double_Prototype_0 Input_Function_Pointer , 
                             const doubleArray & Result , const doubleArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_1 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_1 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_1 *Current_Link;
        doubleArray_Function_1 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_1 ( int Input_Operation_Code , 
                             MDI_double_Prototype_1 Input_Function_Pointer , 
                             const doubleArray & X );
        doubleArray_Function_1 ( MDI_double_Prototype_1 Input_Function_Pointer , 
                             const doubleArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_2 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_2 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage Operand_Lhs;
        const doubleArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_2 *Current_Link;
        doubleArray_Function_2 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
       ~doubleArray_Function_2 ();
        doubleArray_Function_2 ( int Input_Operation_Code , 
                             MDI_double_Prototype_2 Input_Function_Pointer , 
                             const doubleArray & Result , const doubleArray & Lhs , const doubleArray & Rhs );
        doubleArray_Function_2 ( MDI_double_Prototype_2 Input_Function_Pointer , 
                             const doubleArray & Result , const doubleArray & Lhs , const doubleArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_3 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_3 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_3 *Current_Link;
        doubleArray_Function_3 *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

        bool Execute_MDI_Function;

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_3 ( int Input_Operation_Code , 
                             MDI_double_Prototype_3 Input_Function_Pointer , 
                             const doubleArray & Result , const doubleArray & Rhs );
        doubleArray_Function_3 ( MDI_double_Prototype_3 Input_Function_Pointer , 
                             const doubleArray & Result , const doubleArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );

        void Skip_MDI_Function();
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_4 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_4 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage Operand_Lhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_4 *Current_Link;
        doubleArray_Function_4 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_4 ( int Input_Operation_Code , 
                             MDI_double_Prototype_4 Input_Function_Pointer , 
                             const doubleArray & Result , const doubleArray & Lhs , double Scalar);
        doubleArray_Function_4 ( MDI_double_Prototype_4 Input_Function_Pointer , 
                             const doubleArray & Result , const doubleArray & Lhs , double Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_5 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_5 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_Lhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_5 *Current_Link;
        doubleArray_Function_5 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_5 ( int Input_Operation_Code , 
                             MDI_double_Prototype_5 Input_Function_Pointer , 
                             const doubleArray & Lhs , double Scalar);
        doubleArray_Function_5 ( MDI_double_Prototype_5 Input_Function_Pointer , 
                             const doubleArray & Lhs , double Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_6 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_6 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        const doubleArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_6 *Current_Link;
        doubleArray_Function_6 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_6 ( int Input_Operation_Code , 
                             MDI_double_Prototype_6 Input_Function_Pointer , 
                             const doubleArray & Result , const intArray & Lhs , const doubleArray & Rhs );
        doubleArray_Function_6 ( MDI_double_Prototype_6 Input_Function_Pointer , 
                             const doubleArray & Result , const intArray & Lhs , const doubleArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_7 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_7 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_7 *Current_Link;
        doubleArray_Function_7 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_7 ( int Input_Operation_Code , 
                             MDI_double_Prototype_7 Input_Function_Pointer , 
                             const doubleArray & Result , const intArray & Lhs , double Scalar);
        doubleArray_Function_7 ( MDI_double_Prototype_7 Input_Function_Pointer , 
                             const doubleArray & Result , const intArray & Lhs , double Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#ifndef INTARRAY
class doubleArray_Function_8 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_8 Function_Pointer;

     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_8 *Current_Link;
        doubleArray_Function_8 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_8 ( int Input_Operation_Code , 
                             MDI_double_Prototype_8 Input_Function_Pointer , 
                             const doubleArray & Operand_Result , const doubleArray & Lhs , int Scalar);
        doubleArray_Function_8 ( MDI_double_Prototype_8 Input_Function_Pointer , 
                             const doubleArray & Operand_Result , const doubleArray & Lhs , int Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

class doubleArray_Function_9 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_9 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage  Operand_Lhs;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_9 *Current_Link;
        doubleArray_Function_9 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_9 ( int Input_Operation_Code , 
                             MDI_double_Prototype_9 Input_Function_Pointer , 
                             const intArray & Result , const doubleArray & Lhs );
        doubleArray_Function_9 ( MDI_double_Prototype_9 Input_Function_Pointer , 
                             const intArray & Result , const doubleArray & Lhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#if 0
class doubleArray_Function_10 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_10 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_10 *Current_Link;
        doubleArray_Function_10 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_10 ( int Input_Operation_Code , 
                              MDI_double_Prototype_10 Input_Function_Pointer );
        doubleArray_Function_10 ( MDI_double_Prototype_10 Input_Function_Pointer );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

class doubleArray_Function_11 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_11 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage Operand_Lhs;
        const doubleArray_Operand_Storage Operand_Rhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_11 *Current_Link;
        doubleArray_Function_11 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_11 ( int Input_Operation_Code , 
                              MDI_double_Prototype_11 Input_Function_Pointer , 
                              const intArray & Result , const doubleArray & Lhs , const doubleArray & Rhs );
        doubleArray_Function_11 ( MDI_double_Prototype_11 Input_Function_Pointer , 
                              const intArray & Result , const doubleArray & Lhs , const doubleArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class doubleArray_Function_12 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_12 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const doubleArray_Operand_Storage Operand_Lhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_12 *Current_Link;
        doubleArray_Function_12 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_12 ( int Input_Operation_Code , 
                              MDI_double_Prototype_12 Input_Function_Pointer , 
                              const intArray & Result , const doubleArray & Lhs , double Scalar);
        doubleArray_Function_12 ( MDI_double_Prototype_12 Input_Function_Pointer , 
                              const intArray & Result , const doubleArray & Lhs , double Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#if 0
class doubleArray_Function_13 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_13 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_13 *Current_Link;
        doubleArray_Function_13 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_13 ( int Input_Operation_Code , 
                              MDI_double_Prototype_13 Input_Function_Pointer );
        doubleArray_Function_13 ( MDI_double_Prototype_13 Input_Function_Pointer );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

#if 1
class doubleArray_Function_14 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_14 Function_Pointer;
 
     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_14 *Current_Link;
        doubleArray_Function_14 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_14 ( int Input_Operation_Code ,
                              MDI_double_Prototype_14 Input_Function_Pointer ,
                              const doubleArray & X );
        doubleArray_Function_14 ( MDI_double_Prototype_14 Input_Function_Pointer ,
                              const doubleArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

// Used for the intArray::indexMap member function!
class doubleArray_Function_15 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_15 Function_Pointer;
 
     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_15 *Current_Link;
        doubleArray_Function_15 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        doubleArray_Function_15 ( int Input_Operation_Code ,
                              MDI_double_Prototype_15 Input_Function_Pointer ,
                              const doubleArray & X );
        doubleArray_Function_15 ( MDI_double_Prototype_15 Input_Function_Pointer ,
                              const doubleArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
 
#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
class doubleArray_Function_16 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_16 Function_Pointer;
        const char *Label;
 
     // Storage for function operands!
        const doubleArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static doubleArray_Function_16 *Current_Link;
        doubleArray_Function_16 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);
 
     public:
        doubleArray_Function_16 ( int Input_Operation_Code ,
                              MDI_double_Prototype_16 Input_Function_Pointer ,
                              const doubleArray & X , const char *Label );
        doubleArray_Function_16 ( MDI_double_Prototype_16 Input_Function_Pointer ,
                              const doubleArray & X , const char *Label );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();
 
        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif


// This uses an abstract base class!
class floatArray_Function_Steal_Data : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!

     // Storage for function operands!
     // Note that we cannot use a reference here since then the object 
     // referenced will be deleted (destructor called by the compiler)!
        const floatArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_Steal_Data *Current_Link;
        floatArray_Function_Steal_Data *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

        bool Execute_MDI_Function;

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_Steal_Data ( int Input_Operation_Code , 
                                      const floatArray & Result , const floatArray & X );
        floatArray_Function_Steal_Data ( const floatArray & Result , const floatArray & X );
        void Execute ();
     // void Display_Statement_List ();  // Print out operand and operator codes!
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );

        void Skip_MDI_Function();
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_0 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_0 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_0 *Current_Link;
        floatArray_Function_0 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_0 ( int Input_Operation_Code , 
                             MDI_float_Prototype_0 Input_Function_Pointer , 
                             const floatArray & Result , const floatArray & X );
        floatArray_Function_0 ( MDI_float_Prototype_0 Input_Function_Pointer , 
                             const floatArray & Result , const floatArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_1 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_1 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_1 *Current_Link;
        floatArray_Function_1 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_1 ( int Input_Operation_Code , 
                             MDI_float_Prototype_1 Input_Function_Pointer , 
                             const floatArray & X );
        floatArray_Function_1 ( MDI_float_Prototype_1 Input_Function_Pointer , 
                             const floatArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_2 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_2 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage Operand_Lhs;
        const floatArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_2 *Current_Link;
        floatArray_Function_2 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
       ~floatArray_Function_2 ();
        floatArray_Function_2 ( int Input_Operation_Code , 
                             MDI_float_Prototype_2 Input_Function_Pointer , 
                             const floatArray & Result , const floatArray & Lhs , const floatArray & Rhs );
        floatArray_Function_2 ( MDI_float_Prototype_2 Input_Function_Pointer , 
                             const floatArray & Result , const floatArray & Lhs , const floatArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_3 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_3 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_3 *Current_Link;
        floatArray_Function_3 *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

        bool Execute_MDI_Function;

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_3 ( int Input_Operation_Code , 
                             MDI_float_Prototype_3 Input_Function_Pointer , 
                             const floatArray & Result , const floatArray & Rhs );
        floatArray_Function_3 ( MDI_float_Prototype_3 Input_Function_Pointer , 
                             const floatArray & Result , const floatArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );

        void Skip_MDI_Function();
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_4 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_4 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage Operand_Lhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_4 *Current_Link;
        floatArray_Function_4 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_4 ( int Input_Operation_Code , 
                             MDI_float_Prototype_4 Input_Function_Pointer , 
                             const floatArray & Result , const floatArray & Lhs , float Scalar);
        floatArray_Function_4 ( MDI_float_Prototype_4 Input_Function_Pointer , 
                             const floatArray & Result , const floatArray & Lhs , float Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_5 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_5 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_Lhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_5 *Current_Link;
        floatArray_Function_5 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_5 ( int Input_Operation_Code , 
                             MDI_float_Prototype_5 Input_Function_Pointer , 
                             const floatArray & Lhs , float Scalar);
        floatArray_Function_5 ( MDI_float_Prototype_5 Input_Function_Pointer , 
                             const floatArray & Lhs , float Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_6 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_6 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        const floatArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_6 *Current_Link;
        floatArray_Function_6 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_6 ( int Input_Operation_Code , 
                             MDI_float_Prototype_6 Input_Function_Pointer , 
                             const floatArray & Result , const intArray & Lhs , const floatArray & Rhs );
        floatArray_Function_6 ( MDI_float_Prototype_6 Input_Function_Pointer , 
                             const floatArray & Result , const intArray & Lhs , const floatArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_7 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_7 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_7 *Current_Link;
        floatArray_Function_7 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_7 ( int Input_Operation_Code , 
                             MDI_float_Prototype_7 Input_Function_Pointer , 
                             const floatArray & Result , const intArray & Lhs , float Scalar);
        floatArray_Function_7 ( MDI_float_Prototype_7 Input_Function_Pointer , 
                             const floatArray & Result , const intArray & Lhs , float Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#ifndef INTARRAY
class floatArray_Function_8 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_8 Function_Pointer;

     // Storage for function operands!
        const floatArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_8 *Current_Link;
        floatArray_Function_8 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_8 ( int Input_Operation_Code , 
                             MDI_float_Prototype_8 Input_Function_Pointer , 
                             const floatArray & Operand_Result , const floatArray & Lhs , int Scalar);
        floatArray_Function_8 ( MDI_float_Prototype_8 Input_Function_Pointer , 
                             const floatArray & Operand_Result , const floatArray & Lhs , int Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

class floatArray_Function_9 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_9 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage  Operand_Lhs;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_9 *Current_Link;
        floatArray_Function_9 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_9 ( int Input_Operation_Code , 
                             MDI_float_Prototype_9 Input_Function_Pointer , 
                             const intArray & Result , const floatArray & Lhs );
        floatArray_Function_9 ( MDI_float_Prototype_9 Input_Function_Pointer , 
                             const intArray & Result , const floatArray & Lhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#if 0
class floatArray_Function_10 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_10 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_10 *Current_Link;
        floatArray_Function_10 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_10 ( int Input_Operation_Code , 
                              MDI_float_Prototype_10 Input_Function_Pointer );
        floatArray_Function_10 ( MDI_float_Prototype_10 Input_Function_Pointer );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

class floatArray_Function_11 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_11 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage Operand_Lhs;
        const floatArray_Operand_Storage Operand_Rhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_11 *Current_Link;
        floatArray_Function_11 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_11 ( int Input_Operation_Code , 
                              MDI_float_Prototype_11 Input_Function_Pointer , 
                              const intArray & Result , const floatArray & Lhs , const floatArray & Rhs );
        floatArray_Function_11 ( MDI_float_Prototype_11 Input_Function_Pointer , 
                              const intArray & Result , const floatArray & Lhs , const floatArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class floatArray_Function_12 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_12 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const floatArray_Operand_Storage Operand_Lhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_12 *Current_Link;
        floatArray_Function_12 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_12 ( int Input_Operation_Code , 
                              MDI_float_Prototype_12 Input_Function_Pointer , 
                              const intArray & Result , const floatArray & Lhs , float Scalar);
        floatArray_Function_12 ( MDI_float_Prototype_12 Input_Function_Pointer , 
                              const intArray & Result , const floatArray & Lhs , float Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#if 0
class floatArray_Function_13 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_13 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static floatArray_Function_13 *Current_Link;
        floatArray_Function_13 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_13 ( int Input_Operation_Code , 
                              MDI_float_Prototype_13 Input_Function_Pointer );
        floatArray_Function_13 ( MDI_float_Prototype_13 Input_Function_Pointer );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

#if 1
class floatArray_Function_14 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_14 Function_Pointer;
 
     // Storage for function operands!
        const floatArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static floatArray_Function_14 *Current_Link;
        floatArray_Function_14 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_14 ( int Input_Operation_Code ,
                              MDI_float_Prototype_14 Input_Function_Pointer ,
                              const floatArray & X );
        floatArray_Function_14 ( MDI_float_Prototype_14 Input_Function_Pointer ,
                              const floatArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

// Used for the intArray::indexMap member function!
class floatArray_Function_15 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_15 Function_Pointer;
 
     // Storage for function operands!
        const floatArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static floatArray_Function_15 *Current_Link;
        floatArray_Function_15 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        floatArray_Function_15 ( int Input_Operation_Code ,
                              MDI_float_Prototype_15 Input_Function_Pointer ,
                              const floatArray & X );
        floatArray_Function_15 ( MDI_float_Prototype_15 Input_Function_Pointer ,
                              const floatArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
 
#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
class floatArray_Function_16 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_16 Function_Pointer;
        const char *Label;
 
     // Storage for function operands!
        const floatArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static floatArray_Function_16 *Current_Link;
        floatArray_Function_16 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);
 
     public:
        floatArray_Function_16 ( int Input_Operation_Code ,
                              MDI_float_Prototype_16 Input_Function_Pointer ,
                              const floatArray & X , const char *Label );
        floatArray_Function_16 ( MDI_float_Prototype_16 Input_Function_Pointer ,
                              const floatArray & X , const char *Label );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();
 
        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif



#define INTARRAY
// This uses an abstract base class!
class intArray_Function_Steal_Data : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!

     // Storage for function operands!
     // Note that we cannot use a reference here since then the object 
     // referenced will be deleted (destructor called by the compiler)!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_Steal_Data *Current_Link;
        intArray_Function_Steal_Data *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

        bool Execute_MDI_Function;

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_Steal_Data ( int Input_Operation_Code , 
                                      const intArray & Result , const intArray & X );
        intArray_Function_Steal_Data ( const intArray & Result , const intArray & X );
        void Execute ();
     // void Display_Statement_List ();  // Print out operand and operator codes!
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );

        void Skip_MDI_Function();
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_0 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_0 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_0 *Current_Link;
        intArray_Function_0 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_0 ( int Input_Operation_Code , 
                             MDI_int_Prototype_0 Input_Function_Pointer , 
                             const intArray & Result , const intArray & X );
        intArray_Function_0 ( MDI_int_Prototype_0 Input_Function_Pointer , 
                             const intArray & Result , const intArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_1 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_1 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_1 *Current_Link;
        intArray_Function_1 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_1 ( int Input_Operation_Code , 
                             MDI_int_Prototype_1 Input_Function_Pointer , 
                             const intArray & X );
        intArray_Function_1 ( MDI_int_Prototype_1 Input_Function_Pointer , 
                             const intArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_2 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_2 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        const intArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_2 *Current_Link;
        intArray_Function_2 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
       ~intArray_Function_2 ();
        intArray_Function_2 ( int Input_Operation_Code , 
                             MDI_int_Prototype_2 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs , const intArray & Rhs );
        intArray_Function_2 ( MDI_int_Prototype_2 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs , const intArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_3 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_3 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_3 *Current_Link;
        intArray_Function_3 *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

        bool Execute_MDI_Function;

     public:
#ifdef INLINE_NEW_OPERATORS
        inline void *operator new (size_t Size);
#else
        void *operator new (size_t Size);
#endif
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_3 ( int Input_Operation_Code , 
                             MDI_int_Prototype_3 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Rhs );
        intArray_Function_3 ( MDI_int_Prototype_3 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );

        void Skip_MDI_Function();
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_4 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_4 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_4 *Current_Link;
        intArray_Function_4 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_4 ( int Input_Operation_Code , 
                             MDI_int_Prototype_4 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs , int Scalar);
        intArray_Function_4 ( MDI_int_Prototype_4 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs , int Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_5 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_5 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_5 *Current_Link;
        intArray_Function_5 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_5 ( int Input_Operation_Code , 
                             MDI_int_Prototype_5 Input_Function_Pointer , 
                             const intArray & Lhs , int Scalar);
        intArray_Function_5 ( MDI_int_Prototype_5 Input_Function_Pointer , 
                             const intArray & Lhs , int Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_6 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_6 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        const intArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_6 *Current_Link;
        intArray_Function_6 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_6 ( int Input_Operation_Code , 
                             MDI_int_Prototype_6 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs , const intArray & Rhs );
        intArray_Function_6 ( MDI_int_Prototype_6 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs , const intArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_7 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_7 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_7 *Current_Link;
        intArray_Function_7 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_7 ( int Input_Operation_Code , 
                             MDI_int_Prototype_7 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs , int Scalar);
        intArray_Function_7 ( MDI_int_Prototype_7 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs , int Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#ifndef INTARRAY
class intArray_Function_8 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_8 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_8 *Current_Link;
        intArray_Function_8 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_8 ( int Input_Operation_Code , 
                             MDI_int_Prototype_8 Input_Function_Pointer , 
                             const intArray & Operand_Result , const intArray & Lhs , int Scalar);
        intArray_Function_8 ( MDI_int_Prototype_8 Input_Function_Pointer , 
                             const intArray & Operand_Result , const intArray & Lhs , int Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

class intArray_Function_9 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_9 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage  Operand_Lhs;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_9 *Current_Link;
        intArray_Function_9 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_9 ( int Input_Operation_Code , 
                             MDI_int_Prototype_9 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs );
        intArray_Function_9 ( MDI_int_Prototype_9 Input_Function_Pointer , 
                             const intArray & Result , const intArray & Lhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#if 0
class intArray_Function_10 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_10 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static intArray_Function_10 *Current_Link;
        intArray_Function_10 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_10 ( int Input_Operation_Code , 
                              MDI_int_Prototype_10 Input_Function_Pointer );
        intArray_Function_10 ( MDI_int_Prototype_10 Input_Function_Pointer );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

class intArray_Function_11 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_11 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        const intArray_Operand_Storage Operand_Rhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_11 *Current_Link;
        intArray_Function_11 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_11 ( int Input_Operation_Code , 
                              MDI_int_Prototype_11 Input_Function_Pointer , 
                              const intArray & Result , const intArray & Lhs , const intArray & Rhs );
        intArray_Function_11 ( MDI_int_Prototype_11 Input_Function_Pointer , 
                              const intArray & Result , const intArray & Lhs , const intArray & Rhs );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

class intArray_Function_12 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_12 Function_Pointer;

     // Storage for function operands!
        const intArray_Operand_Storage Operand_Result;
        const intArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intArray_Function_12 *Current_Link;
        intArray_Function_12 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_12 ( int Input_Operation_Code , 
                              MDI_int_Prototype_12 Input_Function_Pointer , 
                              const intArray & Result , const intArray & Lhs , int Scalar);
        intArray_Function_12 ( MDI_int_Prototype_12 Input_Function_Pointer , 
                              const intArray & Result , const intArray & Lhs , int Scalar);
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };

#if 0
class intArray_Function_13 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_13 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static intArray_Function_13 *Current_Link;
        intArray_Function_13 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_13 ( int Input_Operation_Code , 
                              MDI_int_Prototype_13 Input_Function_Pointer );
        intArray_Function_13 ( MDI_int_Prototype_13 Input_Function_Pointer );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

#if 1
class intArray_Function_14 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_14 Function_Pointer;
 
     // Storage for function operands!
        const intArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static intArray_Function_14 *Current_Link;
        intArray_Function_14 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_14 ( int Input_Operation_Code ,
                              MDI_int_Prototype_14 Input_Function_Pointer ,
                              const intArray & X );
        intArray_Function_14 ( MDI_int_Prototype_14 Input_Function_Pointer ,
                              const intArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif

// Used for the intArray::indexMap member function!
class intArray_Function_15 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_15 Function_Pointer;
 
     // Storage for function operands!
        const intArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static intArray_Function_15 *Current_Link;
        intArray_Function_15 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);

     public:
        intArray_Function_15 ( int Input_Operation_Code ,
                              MDI_int_Prototype_15 Input_Function_Pointer ,
                              const intArray & X );
        intArray_Function_15 ( MDI_int_Prototype_15 Input_Function_Pointer ,
                              const intArray & X );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();

        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
 
#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
class intArray_Function_16 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_16 Function_Pointer;
        const char *Label;
 
     // Storage for function operands!
        const intArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static intArray_Function_16 *Current_Link;
        intArray_Function_16 *freepointer;
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
        void operator  delete (void* Pointer, size_t Size);
 
     public:
        intArray_Function_16 ( int Input_Operation_Code ,
                              MDI_int_Prototype_16 Input_Function_Pointer ,
                              const intArray & X , const char *Label );
        intArray_Function_16 ( MDI_int_Prototype_16 Input_Function_Pointer ,
                              const intArray & X , const char *Label );
        void Execute ();
        void Display_Expression ();  // Print out operand and operator codes!
        int Get_Lhs_Array_ID ();
        void Get_Rhs_Array_ID ( int *Rhs_Array_ID );
        bool Is_Rhs_A_Temporary ();
 
        int Get_Rhs_Temporary_Operand_ID() const;
        void *Get_Result_Array ();
        void Get_Rhs_Array ( void** Operand_List );
        void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID );
        Expression_Tree_Node_Type *Build_Aggregate_Operator ( 
             int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List );
   };
#endif


#undef INTARRAY

#endif  /* !defined(_PPP_LAZYARRAY_H) */





