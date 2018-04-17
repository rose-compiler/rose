// Allow repeated includes of lazy_statement.h without error
#ifndef _APP_LAZY_STATEMENT_H
#define _APP_LAZY_STATEMENT_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif

const int MAX_HASH_TABLE_SIZE = 100000;

class Optimization_Manager;  // Forward declaration

class Array_Statement_Type
   {
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;

#ifdef PPP
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;
#endif

     friend class Task_Type;
     friend class Optimization_Manager;  

  // struct aggtruct agg_struct
  // struct aggtruct
     struct agg_struct
        {
          int agg_code;
          int start_code;
          int end_code;
          void *agg_function;
        };
     // } agg_struct;

     struct op_struct 
        {
          int op_code;
          int result_id;
          int op1_id;
          int op2_id;
        };

     class Set_Union_Type
        {
           private:
                int* Count;
                int* Father;
                int* Name;
                int* Root;
                int* Temp;

           public:
               ~Set_Union_Type ();
                Set_Union_Type ();
                Set_Union_Type ( int Max_Number_Of_Statements );

                int Fast_Find  ( int Statement_ID_Of_Dependent_Statement );
                void Fast_Union ( int Statement_ID_Of_Current_Statement , int Statement_ID_Of_Dependent_Statement );

           private:
                Set_Union_Type ( const Set_Union_Type & X );
                Set_Union_Type & operator= ( const Set_Union_Type & X );
        };

     private:
       // Required for Statement reference in the dependence analysis
          int Statement_ID;

       // Used for seperation of array statement sequences into independent lists!
       // More specifically this is done through set union (so the Region_ID lables a set)!
       // int Region_ID;

          Expression_Tree_Node_Type *Equals_Operation;

       /* Required for initialization of the statement ID's */
          static int Statement_ID_Counter;

          static Array_Statement_Type *Head_Of_List;
          static Array_Statement_Type *Tail_Of_List;
          Array_Statement_Type *Prev_List_Element;
          Array_Statement_Type *Next_List_Element;
          Array_Statement_Type *Next_List_Element_In_Region;

       // Hash table for dependence info!
       // static bool Hash_Table_Setup;
          static Array_Statement_Type *Hash_Table_Of_Statement_Pointers [MAX_HASH_TABLE_SIZE];

     // Data required for the "new" and "delete" operators!
        static Array_Statement_Type *Current_Link;
        Array_Statement_Type *freepointer;
        static int CLASS_ALLOCATION_POOL_SIZE;
        static const int Max_Number_Of_Memory_Blocks;
        static unsigned char *Memory_Block_List [MAX_NUMBER_OF_MEMORY_BLOCKS];
        static int Memory_Block_Index;

        static void New_Function_Loop ();

     public:
#ifdef INLINE_FUNCTIONS
        inline void *operator new (size_t Size);
#else
        void *operator new    (size_t Size);
#endif
        void operator  delete (void* Pointer, size_t Size);

     public:
          Array_Statement_Type ();
          Array_Statement_Type ( int iDef_Statement_Number );
          Array_Statement_Type ( Expression_Tree_Node_Type* X );
         ~Array_Statement_Type ();

          inline Array_Statement_Type *Test_Array_ID_For_Collision_In_Dependence_Hash_Table 
               ( int Rhs_Operand_ID , int Statement_ID );
          inline Array_Statement_Type *Put_Array_ID_Into_Dependence_Hash_Table_And_Test_For_Collision 
               ( int Lhs_Operand_ID , int Statement_ID );
          void Display();

       // Optimization member functions!
          void Optimize_Assignment();
          static void Optimize_Assignment_In_Statement_List();

       // Optimize to form aggragate operators!
          void Optimize_Binary_Operators_To_Aggregate_Operators();
          static void Optimize_Binary_Operators_To_Aggregate_Operators_In_Statement_List ();

          static void Execute_Statement_List ();
          static void Display_Statement_List ();
          static void Build_Dependence_Info ();
          static void Display_Dependence_Info ();
          static Set_Of_Tasks Find_Independent_Sets_Of_Statements ();

          Array_Statement_Type ( const Array_Statement_Type & X );
          Array_Statement_Type & operator= ( const Array_Statement_Type & X );

     private:
          void Find_Aggregate_Operators ( struct op_struct  *op_info  , int num_codes, 
                                          struct agg_struct *agg_codes, int & agg_len );          
          void Test_Case();
   };

#endif  /* !defined(_APP_LAZY_STATEMENT_H) */

