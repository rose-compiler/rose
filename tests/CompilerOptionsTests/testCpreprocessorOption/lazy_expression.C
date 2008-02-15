// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if !defined(COMPILE_APP)
#define COMPILE_PPP
#endif
// define COMPILE_SERIAL_APP

#include "A++.h"

#define PRINTOUT_OPERATOR_PAIRS  FALSE

bool Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION     = FALSE;
Expression_Tree_Node_Type *Expression_Tree_Node_Type::Head_Of_List = NULL;
Expression_Tree_Node_Type *Expression_Tree_Node_Type::Tail_Of_List = NULL;

// ****************************************************************************
// ****************************************************************************
//             Expression_Tree_Node_Type class member functions
// ****************************************************************************
// ****************************************************************************

Expression_Tree_Node_Type::~Expression_Tree_Node_Type ()
   {
     if (APP_DEBUG > 3)
          printf ("Expression_Tree_Node_Type destructor! \n");
   }

Expression_Tree_Node_Type::Expression_Tree_Node_Type ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Calling (default) Expression_Tree_Node_Type (abstract base class)! \n");

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          printf ("ERROR: This constructor cannot be used with defered evaluation! \n");
          APP_ABORT();
        }
#endif

     Degree_Of_Operation             = 0;
     Operation_Code                  = -1;
     Output_Dependence_Predesessor   = NULL;
     Flow_Dependence_Predesessor [0] = NULL;
     Flow_Dependence_Predesessor [1] = NULL;
     Flow_Dependence_Predesessor [2] = NULL;
     Flow_Dependence_Predesessor [3] = NULL;

  // printf ("ERROR: This constructor does not take the Operator code! \n");
  // APP_ABORT();
   }

Expression_Tree_Node_Type::Expression_Tree_Node_Type ( const Expression_Tree_Node_Type & X )
   {
  // This is required for linking using the GNU compiler  - but it is not used!
  // Avoid Compiler Warning by using X
     printf ("Calling copy constructor for Expression_Tree_Node_Type (abstract base class)! X.Degree_Of_Operation = %d \n",X.Degree_Of_Operation);
     APP_ABORT();
   }

Expression_Tree_Node_Type::Expression_Tree_Node_Type ( int Input_Operation_Code )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Calling Expression_Tree_Node_Type (abstract base class)! \n");
#endif

  // if we use the execution objects with defered evaluation turned off then
  // we can skip the setup of the linked lists which would otherwise be
  // required!  This makes the use of the execution object in by the
  // abstract operators in abstract_op.C more efficient!

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Degree_Of_Operation = 0;
          Operation_Code      = Input_Operation_Code;

          Output_Dependence_Predesessor = NULL;
          Flow_Dependence_Predesessor [0] = NULL;
          Flow_Dependence_Predesessor [1] = NULL;
          Flow_Dependence_Predesessor [2] = NULL;
          Flow_Dependence_Predesessor [3] = NULL;

       // if (MAX_NUMBER_OF_RHS_ARRAYS != 4)
       //    {
       //      printf ("ERROR: Loops unrolled dependent on MAX_NUMBER_OF_RHS_ARRAYS == 4! \n");
       //      APP_ABORT();
       //    }

       // This would execute the list in the FIFO order (Head --> Tail)
          if (Head_Of_List == NULL)
             {
               Head_Of_List = this;
             }
            else
             {
               if (Tail_Of_List != NULL)
                    Tail_Of_List->Next_List_Element = this;
                 else
                  {
                    printf ("ERROR: (Tail_Of_List == NULL) \n");
                    APP_ABORT();
                  }
             }
          Prev_List_Element = Tail_Of_List;
          Tail_Of_List      = this;
          Next_List_Element = NULL;

       // Now build the statement linked list!
       // if ( (Operation_Code == doubleArray::Assignment) ||
       //      (Operation_Code == doubleArray::replace_Function) )

       // There are many ways to end a statement operator= is just one way.
       // The op codes of operations that can end a statement are given
       // values greater than or equal to that of doubleArray::Assignment.
          if (Operation_Code >= doubleArray::Assignment)
             {
            // This builds a linked list so we don't worry about the return from "new"
            // This generates a warning using the GNU g++ compiler, but it is not a problem!
            // printf ("Operation_Code = %d >= doubleArray::Assignment -- build Array_Statement_Type! \n",Operation_Code);
               new Array_Statement_Type ( this );
             }
        }

  // if (APP_DEBUG > 3)
  //      printf ("Leaving Expression_Tree_Node_Type::Expression_Tree_Node_Type (int) \n");
   }

void Expression_Tree_Node_Type::Execute_List ()
   {
  // This would execute the list in the FIFO order (Head --> Tail)
     for (Expression_Tree_Node_Type* p = Head_Of_List; p != NULL; p=p->Next_List_Element)
        {
       // if (APP_DEBUG > 3)
       //      printf ("Going to execute p Operation_Code = %d! \n",p->Operation_Code);

#if PRINTOUT_OPERATOR_PAIRS
          if ( p->Next_List_Element != NULL )
               Catagorize_Operators ( p->Operation_Code , p->Next_List_Element->Operation_Code );
#endif

          p->Execute();

       // if (APP_DEBUG > 3)
       //      printf ("DONE: execute p! \n");
        }
   }

// This is now a pure virtual function!
// void Expression_Tree_Node_Type::Display_Expression ()
//    {
//      printf ("Base class virtual function called!");
//    }

void Expression_Tree_Node_Type::display ()
   {
     printf ("Expression_Tree_Node::display() \n");
     printf ("Degree_Of_Operation        = %d \n",Degree_Of_Operation);
  // printf ("Operation_Complexity       = %d \n",Operation_Complexity);
  // printf ("Number_Of_Operations       = %d \n",Number_Of_Operations);
     printf ("Operation_Code             = %d \n",Operation_Code);
   }

int Expression_Tree_Node_Type::Get_Lhs_Array_ID ()
   {
     printf ("ERROR: Base Class function Expression_Tree_Node_Type::Get_Lhs_Array_ID should not be called! \n");
     APP_ABORT();

     return 0;
   }

void Expression_Tree_Node_Type::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
  // Avoid Compiler Warning
     Rhs_Array_ID_Array = NULL;
     printf ("ERROR: Base Class function Expression_Tree_Node_Type::Get_Rhs_Array_ID should not be called! \n");
     APP_ABORT();
   }

bool Expression_Tree_Node_Type::Is_Rhs_A_Temporary ()
   {
     printf ("ERROR: Base Class function Expression_Tree_Node_Type::Is_Rhs_A_Temporary should not be called! \n");
     APP_ABORT();

     return 0;
   }

void Expression_Tree_Node_Type::Skip_MDI_Function ()
   {
  // Virtual function is defined in direved classes if it is required.
  // Otherwise this instance is called and there is nothing to do!
   }

Expression_Tree_Node_Type* Expression_Tree_Node_Type::Build_Aggregate_Operator ( int Number_Of_Operators , void* Function_Pointer , void* Result, void** Operand_List )
   {
  // Virtual function is defined in direved classes if it is required.
  // Otherwise this instance is called and there is nothing to do!

  // Avoid Compiler Warning
     Number_Of_Operators = 0;
     Function_Pointer    = NULL;
     Result              = NULL;
     Operand_List        = NULL;

     printf ("ERROR: inside Expression_Tree_Node_Type::Build_Aggregate_Operator -- virtual function of base class! \n");
     APP_ABORT();

     return NULL;
   }

#if 1
void Expression_Tree_Node_Type::Catagorize_Operators ( int Operation_Code_1 , int Operation_Code_2 )
   {
     switch (Operation_Code_1)
        {
       // Note that there are a few operators that are not recoreded here (like acos, cosh, and acosh)!

          case 1000 : Catagorize_Operators ( "+" , Operation_Code_2 );
                      break;
          case 1001 : Catagorize_Operators ( "-" , Operation_Code_2 );
                      break;
          case 1002 : Catagorize_Operators ( "*" , Operation_Code_2 );
                      break;
          case 1003 : Catagorize_Operators ( "/" , Operation_Code_2 );
                      break;
          case 1004 : Catagorize_Operators ( "%" , Operation_Code_2 );
                      break;
          case 1005 : Catagorize_Operators ( "cos" , Operation_Code_2 );
                      break;
          case 1006 : Catagorize_Operators ( "sin" , Operation_Code_2 );
                      break;
          case 1007 : Catagorize_Operators ( "tan" , Operation_Code_2 );
                      break;
          case 1017 : Catagorize_Operators ( "log" , Operation_Code_2 );
                      break;
          case 1019 : Catagorize_Operators ( "exp" , Operation_Code_2 );
                      break;
          case 1020 : Catagorize_Operators ( "sqrt" , Operation_Code_2 );
                      break;
          case 1021 : Catagorize_Operators ( "fabs" , Operation_Code_2 );
                      break;
          case 1022 : Catagorize_Operators ( "abs" , Operation_Code_2 );
                      break;
          case 1023 : Catagorize_Operators ( "ceil" , Operation_Code_2 );
                      break;
          case 1024 : Catagorize_Operators ( "floor" , Operation_Code_2 );
                      break;
          case 1025 : Catagorize_Operators ( "replace" , Operation_Code_2 );
                      break;
          case 1026 : Catagorize_Operators ( "unary-" , Operation_Code_2 );
                      break;
          case 1027 : Catagorize_Operators ( "fmod" , Operation_Code_2 );
                      break;
          case 1028 : Catagorize_Operators ( "mod" , Operation_Code_2 );
                      break;
          case 1029 : Catagorize_Operators ( "pow" , Operation_Code_2 );
                      break;
          case 1030 : Catagorize_Operators ( "sign" , Operation_Code_2 );
                      break;
          case 1031 : Catagorize_Operators ( "min" , Operation_Code_2 );
                      break;
          case 1032 : Catagorize_Operators ( "max" , Operation_Code_2 );
                      break;
          case 1033 : Catagorize_Operators ( "!" , Operation_Code_2 );
                      break;
          case 1034 : Catagorize_Operators ( "<" , Operation_Code_2 );
                      break;
          case 1035 : Catagorize_Operators ( ">" , Operation_Code_2 );
                      break;
          case 1036 : Catagorize_Operators ( "<=" , Operation_Code_2 );
                      break;
          case 1037 : Catagorize_Operators ( ">=" , Operation_Code_2 );
                      break;
          case 1038 : Catagorize_Operators ( "eqeq" , Operation_Code_2 );
                      break;
          case 1039 : Catagorize_Operators ( "!=" , Operation_Code_2 );
                      break;
          case 1040 : Catagorize_Operators ( "&&" , Operation_Code_2 );
                      break;
          case 1041 : Catagorize_Operators ( "oror" , Operation_Code_2 );
                      break;
          case 1042 : Catagorize_Operators ( "=" , Operation_Code_2 );
                      break;
          default: Catagorize_Operators ( "?" , Operation_Code_2 );
        }
   }

void Expression_Tree_Node_Type::Catagorize_Operators ( char* String_Var , int Operation_Code_2 )
   {
     switch (Operation_Code_2)
        {
          case 1000 : printf ("%s + \n",String_Var);
                      break;
          case 1001 : printf ("%s - \n",String_Var);
                      break;
          case 1002 : printf ("%s * \n",String_Var);
                      break;
          case 1003 : printf ("%s / \n",String_Var);
                      break;
          case 1004 : printf ("%s mod \n",String_Var);  // cannot use % character in string since it is special
                      break;
          case 1005 : printf ("%s cos \n",String_Var);
                      break;
          case 1006 : printf ("%s sin \n",String_Var);
                      break;
          case 1007 : printf ("%s tan \n",String_Var);
                      break;
          case 1017 : printf ("%s log \n",String_Var);
                      break;
          case 1019 : printf ("%s exp \n",String_Var);
                      break;
          case 1020 : printf ("%s sqrt \n",String_Var);
                      break;
          case 1021 : printf ("%s fabs \n",String_Var);
                      break;
          case 1022 : printf ("%s abs \n",String_Var);
                      break;
          case 1023 : printf ("%s ceil \n",String_Var);
                      break;
          case 1024 : printf ("%s floor \n",String_Var);
                      break;
          case 1025 : printf ("%s replace \n",String_Var);
                      break;
          case 1026 : printf ("%s unary- \n",String_Var);
                      break;
          case 1027 : printf ("%s fmod \n",String_Var);
                      break;
          case 1028 : printf ("%s mod \n",String_Var);
                      break;
          case 1029 : printf ("%s pow \n",String_Var);
                      break;
          case 1030 : printf ("%s sign \n",String_Var);
                      break;
          case 1031 : printf ("%s min \n",String_Var);
                      break;
          case 1032 : printf ("%s max \n",String_Var);
                      break;
          case 1033 : printf ("%s ! \n",String_Var);
                      break;
          case 1034 : printf ("%s < \n",String_Var);
                      break;
          case 1035 : printf ("%s > \n",String_Var);
                      break;
          case 1036 : printf ("%s <= \n",String_Var);
                      break;
          case 1037 : printf ("%s >= \n",String_Var);
                      break;
          case 1038 : printf ("%s eqeq \n",String_Var);
                      break;
          case 1039 : printf ("%s != \n",String_Var);
                      break;
          case 1040 : printf ("%s && \n",String_Var);
                      break;
          case 1041 : printf ("%s oror \n",String_Var);
                      break;
          case 1042 : printf ("%s = \n",String_Var);
                      break;
          default: printf ("%s ? \n",String_Var);
        }
   }
#endif


