// Allow repeated includes of lazy_expression.h without error
#ifndef _APP_LAZY_EXPRESSION_H
#define _APP_LAZY_EXPRESSION_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif

// Give these file scope only!
const int MAX_NUMBER_OF_OPERANDS   = 2;
const int MAX_NUMBER_OF_RHS_ARRAYS = MAX_NUMBER_OF_OPERANDS + 2; // Add 1 for the Result and 1 for the Mask!

const int MAX_NUMBER_OF_OPERATIONS_IN_OPERATOR     = 1;
const int INITIAL_DEFINITION_NODE_STATEMENT_NUMBER = 0;

class Optimization_Manager; // Forward declaration

class Expression_Tree_Node_Type
   {
  // We have to support mixed operations so we need all three types that we support!
     friend class doubleArray;
     friend class floatArray;
     friend class intArray;
#ifdef COMPILE_SERIAL_APP
     friend class SerialArray_Statement_Type;
     friend class doubleSerialArray;
     friend class floatSerialArray;
     friend class intSerialArray;
#endif

     friend class Array_Statement_Type;
     friend class Task_Type;
     friend class Optimization_Manager;

     protected:
          int Degree_Of_Operation;   // Number of operands that the current
                                     // operator takes in its evaluation!
                                     // Always has value = 1 or 2, until we decide to support
                                     // more than just binary operations!

          int Operation_Code;
          Array_Statement_Type *Output_Dependence_Predesessor;
          Array_Statement_Type *Flow_Dependence_Predesessor [MAX_NUMBER_OF_RHS_ARRAYS];

     private:
          static Expression_Tree_Node_Type *Head_Of_List;
          static Expression_Tree_Node_Type *Tail_Of_List;
          Expression_Tree_Node_Type *Prev_List_Element;
          Expression_Tree_Node_Type *Next_List_Element;

       // This is an array which holds the Elementry operation codes.
       // More complicated codes are built by concatination of the elementry
       // operations.  For simple binary operations the length of the code is 1.
       // Thus initially we will only be using the 0th element (so this is a length 1 array)!
       // *****************************************************************

     public:
          static bool DEFER_EXPRESSION_EVALUATION;

       // Let's not for the implementation of these now!
       // It makes no sense to do so for an abstract base class (ABC)
       // void *operator new    (size_t) = 0;
       // void operator  delete (void*)  = 0;

     public:
          Expression_Tree_Node_Type ();
          Expression_Tree_Node_Type ( int Input_Operation_Code );

       // Note we use a virtual destructor!
#ifndef __ATT__
          virtual ~Expression_Tree_Node_Type ();
#else
          ~Expression_Tree_Node_Type ();
#endif

          virtual void Execute () = 0;
          static void Execute_List ();
          INLINE void Delete_If_Temporary_Or_View ( const doubleArray & X );
          INLINE void Delete_If_Temporary_Or_View ( const floatArray  & X );
          INLINE void Delete_If_Temporary_Or_View ( const intArray    & X );
#ifdef COMPILE_SERIAL_APP
          INLINE void Delete_If_Temporary_Or_View ( const doubleSerialArray & X );
          INLINE void Delete_If_Temporary_Or_View ( const floatSerialArray  & X );
          INLINE void Delete_If_Temporary_Or_View ( const intSerialArray    & X );
#endif
          virtual void display ();  // Print out details of expression tree!
          virtual void Display_Expression () = 0;  // Print out operand and operator codes!
          static void Defered_Evaluation ( On_Off_Type On_Off_Setting );
          virtual int Get_Lhs_Array_ID () = 0;
          virtual void Get_Rhs_Array_ID ( int *Rhs_Array_ID ) = 0;
          virtual bool Is_Rhs_A_Temporary () = 0;

          static void Catagorize_Operators ( int Operation_Code_1 , int Operation_Code_2 );
          static void Catagorize_Operators ( char* String_Var , int Operation_Code_2 );

          Expression_Tree_Node_Type ( const Expression_Tree_Node_Type & X );
          Expression_Tree_Node_Type & operator= ( const Expression_Tree_Node_Type & X );

          virtual int Get_Rhs_Temporary_Operand_ID() const = 0;
          virtual void *Get_Result_Array () = 0;
          virtual void Get_Rhs_Array ( void** Operand_List ) = 0;
          virtual void Replace_Temporary_Result_With_Lhs_Array ( void * , int X_Array_ID ) = 0;

          virtual void Skip_MDI_Function ();
          virtual Expression_Tree_Node_Type *Build_Aggregate_Operator ( int Number_Of_Operators, void* Function_Pointer, void* Result, void** Operand_List );
   };

#endif  /* !defined(_APP_LAZY_EXPRESSION_H) */

