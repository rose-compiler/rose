// Allow repeated includes of lazyarray.h without error
#ifndef _APP_LAZYARRAY_H
#define _APP_LAZYARRAY_H

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
class doubleSerialArray_Function_Steal_Data : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!

     // Storage for function operands!
     // Note that we cannot use a reference here since then the object 
     // referenced will be deleted (destructor called by the compiler)!
        const doubleSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_Steal_Data *Current_Link;
        doubleSerialArray_Function_Steal_Data *freepointer;
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
        doubleSerialArray_Function_Steal_Data ( int Input_Operation_Code , 
                                      const doubleSerialArray & Result , const doubleSerialArray & X );
        doubleSerialArray_Function_Steal_Data ( const doubleSerialArray & Result , const doubleSerialArray & X );
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

class doubleSerialArray_Function_0 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_0 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_0 *Current_Link;
        doubleSerialArray_Function_0 *freepointer;
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
        doubleSerialArray_Function_0 ( int Input_Operation_Code , 
                             MDI_double_Prototype_0 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const doubleSerialArray & X );
        doubleSerialArray_Function_0 ( MDI_double_Prototype_0 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const doubleSerialArray & X );
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

class doubleSerialArray_Function_1 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_1 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_1 *Current_Link;
        doubleSerialArray_Function_1 *freepointer;
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
        doubleSerialArray_Function_1 ( int Input_Operation_Code , 
                             MDI_double_Prototype_1 Input_Function_Pointer , 
                             const doubleSerialArray & X );
        doubleSerialArray_Function_1 ( MDI_double_Prototype_1 Input_Function_Pointer , 
                             const doubleSerialArray & X );
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

class doubleSerialArray_Function_2 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_2 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage Operand_Lhs;
        const doubleSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_2 *Current_Link;
        doubleSerialArray_Function_2 *freepointer;
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
       ~doubleSerialArray_Function_2 ();
        doubleSerialArray_Function_2 ( int Input_Operation_Code , 
                             MDI_double_Prototype_2 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        doubleSerialArray_Function_2 ( MDI_double_Prototype_2 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
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

class doubleSerialArray_Function_3 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_3 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_3 *Current_Link;
        doubleSerialArray_Function_3 *freepointer;
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
        doubleSerialArray_Function_3 ( int Input_Operation_Code , 
                             MDI_double_Prototype_3 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const doubleSerialArray & Rhs );
        doubleSerialArray_Function_3 ( MDI_double_Prototype_3 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const doubleSerialArray & Rhs );
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

class doubleSerialArray_Function_4 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_4 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage Operand_Lhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_4 *Current_Link;
        doubleSerialArray_Function_4 *freepointer;
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
        doubleSerialArray_Function_4 ( int Input_Operation_Code , 
                             MDI_double_Prototype_4 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const doubleSerialArray & Lhs , double Scalar);
        doubleSerialArray_Function_4 ( MDI_double_Prototype_4 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const doubleSerialArray & Lhs , double Scalar);
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

class doubleSerialArray_Function_5 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_5 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Lhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_5 *Current_Link;
        doubleSerialArray_Function_5 *freepointer;
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
        doubleSerialArray_Function_5 ( int Input_Operation_Code , 
                             MDI_double_Prototype_5 Input_Function_Pointer , 
                             const doubleSerialArray & Lhs , double Scalar);
        doubleSerialArray_Function_5 ( MDI_double_Prototype_5 Input_Function_Pointer , 
                             const doubleSerialArray & Lhs , double Scalar);
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

class doubleSerialArray_Function_6 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_6 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        const doubleSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_6 *Current_Link;
        doubleSerialArray_Function_6 *freepointer;
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
        doubleSerialArray_Function_6 ( int Input_Operation_Code , 
                             MDI_double_Prototype_6 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const intSerialArray & Lhs , const doubleSerialArray & Rhs );
        doubleSerialArray_Function_6 ( MDI_double_Prototype_6 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const intSerialArray & Lhs , const doubleSerialArray & Rhs );
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

class doubleSerialArray_Function_7 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_7 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_7 *Current_Link;
        doubleSerialArray_Function_7 *freepointer;
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
        doubleSerialArray_Function_7 ( int Input_Operation_Code , 
                             MDI_double_Prototype_7 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const intSerialArray & Lhs , double Scalar);
        doubleSerialArray_Function_7 ( MDI_double_Prototype_7 Input_Function_Pointer , 
                             const doubleSerialArray & Result , const intSerialArray & Lhs , double Scalar);
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
class doubleSerialArray_Function_8 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_8 Function_Pointer;

     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_8 *Current_Link;
        doubleSerialArray_Function_8 *freepointer;
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
        doubleSerialArray_Function_8 ( int Input_Operation_Code , 
                             MDI_double_Prototype_8 Input_Function_Pointer , 
                             const doubleSerialArray & Operand_Result , const doubleSerialArray & Lhs , int Scalar);
        doubleSerialArray_Function_8 ( MDI_double_Prototype_8 Input_Function_Pointer , 
                             const doubleSerialArray & Operand_Result , const doubleSerialArray & Lhs , int Scalar);
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

class doubleSerialArray_Function_9 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_9 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage  Operand_Lhs;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_9 *Current_Link;
        doubleSerialArray_Function_9 *freepointer;
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
        doubleSerialArray_Function_9 ( int Input_Operation_Code , 
                             MDI_double_Prototype_9 Input_Function_Pointer , 
                             const intSerialArray & Result , const doubleSerialArray & Lhs );
        doubleSerialArray_Function_9 ( MDI_double_Prototype_9 Input_Function_Pointer , 
                             const intSerialArray & Result , const doubleSerialArray & Lhs );
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
class doubleSerialArray_Function_10 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_10 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_10 *Current_Link;
        doubleSerialArray_Function_10 *freepointer;
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
        doubleSerialArray_Function_10 ( int Input_Operation_Code , 
                              MDI_double_Prototype_10 Input_Function_Pointer );
        doubleSerialArray_Function_10 ( MDI_double_Prototype_10 Input_Function_Pointer );
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

class doubleSerialArray_Function_11 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_11 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage Operand_Lhs;
        const doubleSerialArray_Operand_Storage Operand_Rhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_11 *Current_Link;
        doubleSerialArray_Function_11 *freepointer;
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
        doubleSerialArray_Function_11 ( int Input_Operation_Code , 
                              MDI_double_Prototype_11 Input_Function_Pointer , 
                              const intSerialArray & Result , const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
        doubleSerialArray_Function_11 ( MDI_double_Prototype_11 Input_Function_Pointer , 
                              const intSerialArray & Result , const doubleSerialArray & Lhs , const doubleSerialArray & Rhs );
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

class doubleSerialArray_Function_12 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_12 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const doubleSerialArray_Operand_Storage Operand_Lhs;
        double Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_12 *Current_Link;
        doubleSerialArray_Function_12 *freepointer;
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
        doubleSerialArray_Function_12 ( int Input_Operation_Code , 
                              MDI_double_Prototype_12 Input_Function_Pointer , 
                              const intSerialArray & Result , const doubleSerialArray & Lhs , double Scalar);
        doubleSerialArray_Function_12 ( MDI_double_Prototype_12 Input_Function_Pointer , 
                              const intSerialArray & Result , const doubleSerialArray & Lhs , double Scalar);
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
class doubleSerialArray_Function_13 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_13 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_13 *Current_Link;
        doubleSerialArray_Function_13 *freepointer;
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
        doubleSerialArray_Function_13 ( int Input_Operation_Code , 
                              MDI_double_Prototype_13 Input_Function_Pointer );
        doubleSerialArray_Function_13 ( MDI_double_Prototype_13 Input_Function_Pointer );
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
class doubleSerialArray_Function_14 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_14 Function_Pointer;
 
     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_14 *Current_Link;
        doubleSerialArray_Function_14 *freepointer;
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
        doubleSerialArray_Function_14 ( int Input_Operation_Code ,
                              MDI_double_Prototype_14 Input_Function_Pointer ,
                              const doubleSerialArray & X );
        doubleSerialArray_Function_14 ( MDI_double_Prototype_14 Input_Function_Pointer ,
                              const doubleSerialArray & X );
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

// Used for the intSerialArray::indexMap member function!
class doubleSerialArray_Function_15 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_15 Function_Pointer;
 
     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_15 *Current_Link;
        doubleSerialArray_Function_15 *freepointer;
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
        doubleSerialArray_Function_15 ( int Input_Operation_Code ,
                              MDI_double_Prototype_15 Input_Function_Pointer ,
                              const doubleSerialArray & X );
        doubleSerialArray_Function_15 ( MDI_double_Prototype_15 Input_Function_Pointer ,
                              const doubleSerialArray & X );
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
class doubleSerialArray_Function_16 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_double_Prototype_16 Function_Pointer;
        const char *Label;
 
     // Storage for function operands!
        const doubleSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static doubleSerialArray_Function_16 *Current_Link;
        doubleSerialArray_Function_16 *freepointer;
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
        doubleSerialArray_Function_16 ( int Input_Operation_Code ,
                              MDI_double_Prototype_16 Input_Function_Pointer ,
                              const doubleSerialArray & X , const char *Label );
        doubleSerialArray_Function_16 ( MDI_double_Prototype_16 Input_Function_Pointer ,
                              const doubleSerialArray & X , const char *Label );
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
class floatSerialArray_Function_Steal_Data : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!

     // Storage for function operands!
     // Note that we cannot use a reference here since then the object 
     // referenced will be deleted (destructor called by the compiler)!
        const floatSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_Steal_Data *Current_Link;
        floatSerialArray_Function_Steal_Data *freepointer;
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
        floatSerialArray_Function_Steal_Data ( int Input_Operation_Code , 
                                      const floatSerialArray & Result , const floatSerialArray & X );
        floatSerialArray_Function_Steal_Data ( const floatSerialArray & Result , const floatSerialArray & X );
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

class floatSerialArray_Function_0 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_0 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_0 *Current_Link;
        floatSerialArray_Function_0 *freepointer;
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
        floatSerialArray_Function_0 ( int Input_Operation_Code , 
                             MDI_float_Prototype_0 Input_Function_Pointer , 
                             const floatSerialArray & Result , const floatSerialArray & X );
        floatSerialArray_Function_0 ( MDI_float_Prototype_0 Input_Function_Pointer , 
                             const floatSerialArray & Result , const floatSerialArray & X );
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

class floatSerialArray_Function_1 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_1 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_1 *Current_Link;
        floatSerialArray_Function_1 *freepointer;
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
        floatSerialArray_Function_1 ( int Input_Operation_Code , 
                             MDI_float_Prototype_1 Input_Function_Pointer , 
                             const floatSerialArray & X );
        floatSerialArray_Function_1 ( MDI_float_Prototype_1 Input_Function_Pointer , 
                             const floatSerialArray & X );
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

class floatSerialArray_Function_2 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_2 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage Operand_Lhs;
        const floatSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_2 *Current_Link;
        floatSerialArray_Function_2 *freepointer;
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
       ~floatSerialArray_Function_2 ();
        floatSerialArray_Function_2 ( int Input_Operation_Code , 
                             MDI_float_Prototype_2 Input_Function_Pointer , 
                             const floatSerialArray & Result , const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        floatSerialArray_Function_2 ( MDI_float_Prototype_2 Input_Function_Pointer , 
                             const floatSerialArray & Result , const floatSerialArray & Lhs , const floatSerialArray & Rhs );
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

class floatSerialArray_Function_3 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_3 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_3 *Current_Link;
        floatSerialArray_Function_3 *freepointer;
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
        floatSerialArray_Function_3 ( int Input_Operation_Code , 
                             MDI_float_Prototype_3 Input_Function_Pointer , 
                             const floatSerialArray & Result , const floatSerialArray & Rhs );
        floatSerialArray_Function_3 ( MDI_float_Prototype_3 Input_Function_Pointer , 
                             const floatSerialArray & Result , const floatSerialArray & Rhs );
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

class floatSerialArray_Function_4 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_4 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage Operand_Lhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_4 *Current_Link;
        floatSerialArray_Function_4 *freepointer;
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
        floatSerialArray_Function_4 ( int Input_Operation_Code , 
                             MDI_float_Prototype_4 Input_Function_Pointer , 
                             const floatSerialArray & Result , const floatSerialArray & Lhs , float Scalar);
        floatSerialArray_Function_4 ( MDI_float_Prototype_4 Input_Function_Pointer , 
                             const floatSerialArray & Result , const floatSerialArray & Lhs , float Scalar);
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

class floatSerialArray_Function_5 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_5 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Lhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_5 *Current_Link;
        floatSerialArray_Function_5 *freepointer;
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
        floatSerialArray_Function_5 ( int Input_Operation_Code , 
                             MDI_float_Prototype_5 Input_Function_Pointer , 
                             const floatSerialArray & Lhs , float Scalar);
        floatSerialArray_Function_5 ( MDI_float_Prototype_5 Input_Function_Pointer , 
                             const floatSerialArray & Lhs , float Scalar);
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

class floatSerialArray_Function_6 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_6 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        const floatSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_6 *Current_Link;
        floatSerialArray_Function_6 *freepointer;
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
        floatSerialArray_Function_6 ( int Input_Operation_Code , 
                             MDI_float_Prototype_6 Input_Function_Pointer , 
                             const floatSerialArray & Result , const intSerialArray & Lhs , const floatSerialArray & Rhs );
        floatSerialArray_Function_6 ( MDI_float_Prototype_6 Input_Function_Pointer , 
                             const floatSerialArray & Result , const intSerialArray & Lhs , const floatSerialArray & Rhs );
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

class floatSerialArray_Function_7 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_7 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_7 *Current_Link;
        floatSerialArray_Function_7 *freepointer;
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
        floatSerialArray_Function_7 ( int Input_Operation_Code , 
                             MDI_float_Prototype_7 Input_Function_Pointer , 
                             const floatSerialArray & Result , const intSerialArray & Lhs , float Scalar);
        floatSerialArray_Function_7 ( MDI_float_Prototype_7 Input_Function_Pointer , 
                             const floatSerialArray & Result , const intSerialArray & Lhs , float Scalar);
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
class floatSerialArray_Function_8 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_8 Function_Pointer;

     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_8 *Current_Link;
        floatSerialArray_Function_8 *freepointer;
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
        floatSerialArray_Function_8 ( int Input_Operation_Code , 
                             MDI_float_Prototype_8 Input_Function_Pointer , 
                             const floatSerialArray & Operand_Result , const floatSerialArray & Lhs , int Scalar);
        floatSerialArray_Function_8 ( MDI_float_Prototype_8 Input_Function_Pointer , 
                             const floatSerialArray & Operand_Result , const floatSerialArray & Lhs , int Scalar);
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

class floatSerialArray_Function_9 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_9 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage  Operand_Lhs;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_9 *Current_Link;
        floatSerialArray_Function_9 *freepointer;
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
        floatSerialArray_Function_9 ( int Input_Operation_Code , 
                             MDI_float_Prototype_9 Input_Function_Pointer , 
                             const intSerialArray & Result , const floatSerialArray & Lhs );
        floatSerialArray_Function_9 ( MDI_float_Prototype_9 Input_Function_Pointer , 
                             const intSerialArray & Result , const floatSerialArray & Lhs );
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
class floatSerialArray_Function_10 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_10 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_10 *Current_Link;
        floatSerialArray_Function_10 *freepointer;
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
        floatSerialArray_Function_10 ( int Input_Operation_Code , 
                              MDI_float_Prototype_10 Input_Function_Pointer );
        floatSerialArray_Function_10 ( MDI_float_Prototype_10 Input_Function_Pointer );
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

class floatSerialArray_Function_11 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_11 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage Operand_Lhs;
        const floatSerialArray_Operand_Storage Operand_Rhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_11 *Current_Link;
        floatSerialArray_Function_11 *freepointer;
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
        floatSerialArray_Function_11 ( int Input_Operation_Code , 
                              MDI_float_Prototype_11 Input_Function_Pointer , 
                              const intSerialArray & Result , const floatSerialArray & Lhs , const floatSerialArray & Rhs );
        floatSerialArray_Function_11 ( MDI_float_Prototype_11 Input_Function_Pointer , 
                              const intSerialArray & Result , const floatSerialArray & Lhs , const floatSerialArray & Rhs );
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

class floatSerialArray_Function_12 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_12 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const floatSerialArray_Operand_Storage Operand_Lhs;
        float Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_12 *Current_Link;
        floatSerialArray_Function_12 *freepointer;
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
        floatSerialArray_Function_12 ( int Input_Operation_Code , 
                              MDI_float_Prototype_12 Input_Function_Pointer , 
                              const intSerialArray & Result , const floatSerialArray & Lhs , float Scalar);
        floatSerialArray_Function_12 ( MDI_float_Prototype_12 Input_Function_Pointer , 
                              const intSerialArray & Result , const floatSerialArray & Lhs , float Scalar);
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
class floatSerialArray_Function_13 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_13 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_13 *Current_Link;
        floatSerialArray_Function_13 *freepointer;
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
        floatSerialArray_Function_13 ( int Input_Operation_Code , 
                              MDI_float_Prototype_13 Input_Function_Pointer );
        floatSerialArray_Function_13 ( MDI_float_Prototype_13 Input_Function_Pointer );
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
class floatSerialArray_Function_14 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_14 Function_Pointer;
 
     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_14 *Current_Link;
        floatSerialArray_Function_14 *freepointer;
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
        floatSerialArray_Function_14 ( int Input_Operation_Code ,
                              MDI_float_Prototype_14 Input_Function_Pointer ,
                              const floatSerialArray & X );
        floatSerialArray_Function_14 ( MDI_float_Prototype_14 Input_Function_Pointer ,
                              const floatSerialArray & X );
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

// Used for the intSerialArray::indexMap member function!
class floatSerialArray_Function_15 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_15 Function_Pointer;
 
     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_15 *Current_Link;
        floatSerialArray_Function_15 *freepointer;
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
        floatSerialArray_Function_15 ( int Input_Operation_Code ,
                              MDI_float_Prototype_15 Input_Function_Pointer ,
                              const floatSerialArray & X );
        floatSerialArray_Function_15 ( MDI_float_Prototype_15 Input_Function_Pointer ,
                              const floatSerialArray & X );
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
class floatSerialArray_Function_16 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_float_Prototype_16 Function_Pointer;
        const char *Label;
 
     // Storage for function operands!
        const floatSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static floatSerialArray_Function_16 *Current_Link;
        floatSerialArray_Function_16 *freepointer;
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
        floatSerialArray_Function_16 ( int Input_Operation_Code ,
                              MDI_float_Prototype_16 Input_Function_Pointer ,
                              const floatSerialArray & X , const char *Label );
        floatSerialArray_Function_16 ( MDI_float_Prototype_16 Input_Function_Pointer ,
                              const floatSerialArray & X , const char *Label );
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
class intSerialArray_Function_Steal_Data : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!

     // Storage for function operands!
     // Note that we cannot use a reference here since then the object 
     // referenced will be deleted (destructor called by the compiler)!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_Steal_Data *Current_Link;
        intSerialArray_Function_Steal_Data *freepointer;
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
        intSerialArray_Function_Steal_Data ( int Input_Operation_Code , 
                                      const intSerialArray & Result , const intSerialArray & X );
        intSerialArray_Function_Steal_Data ( const intSerialArray & Result , const intSerialArray & X );
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

class intSerialArray_Function_0 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_0 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_0 *Current_Link;
        intSerialArray_Function_0 *freepointer;
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
        intSerialArray_Function_0 ( int Input_Operation_Code , 
                             MDI_int_Prototype_0 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & X );
        intSerialArray_Function_0 ( MDI_int_Prototype_0 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & X );
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

class intSerialArray_Function_1 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_1 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_X;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_1 *Current_Link;
        intSerialArray_Function_1 *freepointer;
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
        intSerialArray_Function_1 ( int Input_Operation_Code , 
                             MDI_int_Prototype_1 Input_Function_Pointer , 
                             const intSerialArray & X );
        intSerialArray_Function_1 ( MDI_int_Prototype_1 Input_Function_Pointer , 
                             const intSerialArray & X );
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

class intSerialArray_Function_2 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_2 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        const intSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_2 *Current_Link;
        intSerialArray_Function_2 *freepointer;
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
       ~intSerialArray_Function_2 ();
        intSerialArray_Function_2 ( int Input_Operation_Code , 
                             MDI_int_Prototype_2 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs , const intSerialArray & Rhs );
        intSerialArray_Function_2 ( MDI_int_Prototype_2 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs , const intSerialArray & Rhs );
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

class intSerialArray_Function_3 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_3 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_3 *Current_Link;
        intSerialArray_Function_3 *freepointer;
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
        intSerialArray_Function_3 ( int Input_Operation_Code , 
                             MDI_int_Prototype_3 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Rhs );
        intSerialArray_Function_3 ( MDI_int_Prototype_3 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Rhs );
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

class intSerialArray_Function_4 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_4 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_4 *Current_Link;
        intSerialArray_Function_4 *freepointer;
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
        intSerialArray_Function_4 ( int Input_Operation_Code , 
                             MDI_int_Prototype_4 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs , int Scalar);
        intSerialArray_Function_4 ( MDI_int_Prototype_4 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs , int Scalar);
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

class intSerialArray_Function_5 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_5 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_5 *Current_Link;
        intSerialArray_Function_5 *freepointer;
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
        intSerialArray_Function_5 ( int Input_Operation_Code , 
                             MDI_int_Prototype_5 Input_Function_Pointer , 
                             const intSerialArray & Lhs , int Scalar);
        intSerialArray_Function_5 ( MDI_int_Prototype_5 Input_Function_Pointer , 
                             const intSerialArray & Lhs , int Scalar);
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

class intSerialArray_Function_6 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_6 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        const intSerialArray_Operand_Storage Operand_Rhs;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_6 *Current_Link;
        intSerialArray_Function_6 *freepointer;
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
        intSerialArray_Function_6 ( int Input_Operation_Code , 
                             MDI_int_Prototype_6 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs , const intSerialArray & Rhs );
        intSerialArray_Function_6 ( MDI_int_Prototype_6 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs , const intSerialArray & Rhs );
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

class intSerialArray_Function_7 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_7 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_7 *Current_Link;
        intSerialArray_Function_7 *freepointer;
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
        intSerialArray_Function_7 ( int Input_Operation_Code , 
                             MDI_int_Prototype_7 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs , int Scalar);
        intSerialArray_Function_7 ( MDI_int_Prototype_7 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs , int Scalar);
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
class intSerialArray_Function_8 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_8 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_8 *Current_Link;
        intSerialArray_Function_8 *freepointer;
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
        intSerialArray_Function_8 ( int Input_Operation_Code , 
                             MDI_int_Prototype_8 Input_Function_Pointer , 
                             const intSerialArray & Operand_Result , const intSerialArray & Lhs , int Scalar);
        intSerialArray_Function_8 ( MDI_int_Prototype_8 Input_Function_Pointer , 
                             const intSerialArray & Operand_Result , const intSerialArray & Lhs , int Scalar);
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

class intSerialArray_Function_9 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_9 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage  Operand_Lhs;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_9 *Current_Link;
        intSerialArray_Function_9 *freepointer;
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
        intSerialArray_Function_9 ( int Input_Operation_Code , 
                             MDI_int_Prototype_9 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs );
        intSerialArray_Function_9 ( MDI_int_Prototype_9 Input_Function_Pointer , 
                             const intSerialArray & Result , const intSerialArray & Lhs );
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
class intSerialArray_Function_10 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_10 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_10 *Current_Link;
        intSerialArray_Function_10 *freepointer;
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
        intSerialArray_Function_10 ( int Input_Operation_Code , 
                              MDI_int_Prototype_10 Input_Function_Pointer );
        intSerialArray_Function_10 ( MDI_int_Prototype_10 Input_Function_Pointer );
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

class intSerialArray_Function_11 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_11 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        const intSerialArray_Operand_Storage Operand_Rhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_11 *Current_Link;
        intSerialArray_Function_11 *freepointer;
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
        intSerialArray_Function_11 ( int Input_Operation_Code , 
                              MDI_int_Prototype_11 Input_Function_Pointer , 
                              const intSerialArray & Result , const intSerialArray & Lhs , const intSerialArray & Rhs );
        intSerialArray_Function_11 ( MDI_int_Prototype_11 Input_Function_Pointer , 
                              const intSerialArray & Result , const intSerialArray & Lhs , const intSerialArray & Rhs );
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

class intSerialArray_Function_12 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_12 Function_Pointer;

     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_Result;
        const intSerialArray_Operand_Storage Operand_Lhs;
        int Operand_Scalar;

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_12 *Current_Link;
        intSerialArray_Function_12 *freepointer;
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
        intSerialArray_Function_12 ( int Input_Operation_Code , 
                              MDI_int_Prototype_12 Input_Function_Pointer , 
                              const intSerialArray & Result , const intSerialArray & Lhs , int Scalar);
        intSerialArray_Function_12 ( MDI_int_Prototype_12 Input_Function_Pointer , 
                              const intSerialArray & Result , const intSerialArray & Lhs , int Scalar);
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
class intSerialArray_Function_13 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_13 Function_Pointer;

     // Storage for function operands!

     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_13 *Current_Link;
        intSerialArray_Function_13 *freepointer;
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
        intSerialArray_Function_13 ( int Input_Operation_Code , 
                              MDI_int_Prototype_13 Input_Function_Pointer );
        intSerialArray_Function_13 ( MDI_int_Prototype_13 Input_Function_Pointer );
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
class intSerialArray_Function_14 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_14 Function_Pointer;
 
     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_14 *Current_Link;
        intSerialArray_Function_14 *freepointer;
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
        intSerialArray_Function_14 ( int Input_Operation_Code ,
                              MDI_int_Prototype_14 Input_Function_Pointer ,
                              const intSerialArray & X );
        intSerialArray_Function_14 ( MDI_int_Prototype_14 Input_Function_Pointer ,
                              const intSerialArray & X );
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

// Used for the intSerialArray::indexMap member function!
class intSerialArray_Function_15 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_15 Function_Pointer;
 
     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_15 *Current_Link;
        intSerialArray_Function_15 *freepointer;
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
        intSerialArray_Function_15 ( int Input_Operation_Code ,
                              MDI_int_Prototype_15 Input_Function_Pointer ,
                              const intSerialArray & X );
        intSerialArray_Function_15 ( MDI_int_Prototype_15 Input_Function_Pointer ,
                              const intSerialArray & X );
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
class intSerialArray_Function_16 : public Expression_Tree_Node_Type
   {
     private:
     // Storage for function pointer!
        MDI_int_Prototype_16 Function_Pointer;
        const char *Label;
 
     // Storage for function operands!
        const intSerialArray_Operand_Storage Operand_X;
 
     // Data required for the "new" and "delete" operators!
        static intSerialArray_Function_16 *Current_Link;
        intSerialArray_Function_16 *freepointer;
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
        intSerialArray_Function_16 ( int Input_Operation_Code ,
                              MDI_int_Prototype_16 Input_Function_Pointer ,
                              const intSerialArray & X , const char *Label );
        intSerialArray_Function_16 ( MDI_int_Prototype_16 Input_Function_Pointer ,
                              const intSerialArray & X , const char *Label );
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

#endif  /* !defined(_APP_LAZYARRAY_H) */





