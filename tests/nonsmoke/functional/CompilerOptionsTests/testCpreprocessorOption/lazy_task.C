// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if !defined(COMPILE_APP)
#define COMPILE_PPP
#endif

#include "A++.h"

#define PRINTOUT_TASK_LIST FALSE

int Task_Type::Number_Of_Tasks = 0;

// ****************************************************************************
// ****************************************************************************
//                     Task_Type class member functions
// ****************************************************************************
// ****************************************************************************

Task_Type::~Task_Type ()
   {
     if (APP_DEBUG > 3)
          printf ("Task_Type destructor! \n");

  // Static variabe to count the total number of tasks!
     Number_Of_Tasks--;
// COMMENT BETWEEN FUNCTIONS A
// COMMENT BETWEEN FUNCTIONS B
// COMMENT BETWEEN FUNCTIONS C
// COMMENT BETWEEN FUNCTIONS D
   }

// COMMENT BETWEEN FUNCTIONS X

Task_Type::Task_Type ()
   {
     if (APP_DEBUG > 3)
          printf ("Task_Type constructor! \n");

  // Static variabe to count the total number of tasks!
     Number_Of_Tasks++;

     Number_Of_Statements   = 0;
     Number_Of_Operations   = 0;
     Head_Of_Statement_List = NULL;
   }

Task_Type::Task_Type ( Array_Statement_Type *Statement_List )
   {
     if (APP_DEBUG > 3)
          printf ("Inside of Task_Type::Task_Type! \n");

  // if (Head_Of_Statement_List != NULL)
  //    {
  //      printf ("ERROR: Task already setup! \n");
  //      APP_ABORT();
  //    }

     if (Statement_List == NULL)
        {
          printf ("ERROR: in Task_Type::Task_Type -- Statement_List == NULL! \n");
          APP_ABORT();
        }

  // Static variabe to count the total number of tasks!
     Number_Of_Tasks++;

     Head_Of_Statement_List = Statement_List;
     Number_Of_Statements   = 0;
     Number_Of_Operations   = 0;

  // Now count the number of operations in the task!
     for (Array_Statement_Type *s = Head_Of_Statement_List; s != NULL; s = s->Next_List_Element_In_Region)
        {
          Number_Of_Statements++;
       // Number_Of_Operations += Temp_Statement_Pointer->Number_Of_Operations;
          Expression_Tree_Node_Type* Terminate = (s->Prev_List_Element == NULL) ? NULL :
                                                  s->Prev_List_Element->Equals_Operation;
          for (Expression_Tree_Node_Type* p = s->Equals_Operation; p != Terminate; p=p->Prev_List_Element)
             {
               Number_Of_Operations++;
             }
        }
   }

void Task_Type::Execute ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of Task_Type::Execute! \n");

     if (Head_Of_Statement_List == NULL)
        {
          printf ("ERROR: in Task_Type::Execute() -- Task NOT setup! \n");
          APP_ABORT();
        }

  // Execute each statement!
     for (Array_Statement_Type *s = Head_Of_Statement_List; s != NULL; s = s->Next_List_Element_In_Region)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
             {
               printf ("Execute the following statement! \n");
               s->Display();
             }
#endif

          Expression_Tree_Node_Type* Right_End = (s->Prev_List_Element == NULL) ? NULL :
                                                  s->Prev_List_Element->Equals_Operation;
       // Find the end of the statement (so that execution occures in the original and correct order)!
          Expression_Tree_Node_Type* p = s->Equals_Operation;
          while (p->Prev_List_Element != Right_End)
             {
            // Move pointer to previous array expression (elements of the statement)!
               p=p->Prev_List_Element;
             }

       // error checking
          if (p == NULL)
             {
               printf ("ERROR: p == NULL in Task_Type::Execute()! \n");
               APP_ABORT();
             }

          Expression_Tree_Node_Type* Left_End = s->Equals_Operation->Next_List_Element;

       // Starting with the right most (end) array expression start execution until we get to the
       // Equals_Operation associated with the statement (s->Equals_Operation)
          for (Expression_Tree_Node_Type* t = p; t != Left_End; t=t->Next_List_Element)
             {
            // error checking
               if (t == NULL)
                  {
                    printf ("ERROR: t == NULL in Task_Type::Execute()! \n");
                    s->Display();
                    APP_ABORT();
                  }

            // Execute array expression!
               t->Execute();
             }
        }
   }

void Task_Type::Optimize_Assignment ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of Task_Type::Optimize_Assignments! \n");

     if (Head_Of_Statement_List == NULL)
        {
          printf ("ERROR: in Task_Type::Optimize_Assignments() -- Task NOT setup! \n");
          APP_ABORT();
        }

  // Execute each statement!
     for (Array_Statement_Type *s = Head_Of_Statement_List; s != NULL; s = s->Next_List_Element_In_Region)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
             {
               printf ("Execute the following statement! \n");
               s->Display();
             }
#endif

          s->Optimize_Assignment();
        }
   }

void Task_Type::Display ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of Task_Type::Display! \n");

     printf ("Task: Number_Of_Statements = %d \n",Number_Of_Statements);
     printf ("Task: Number_Of_Operations = %d \n",Number_Of_Operations);

     if (Head_Of_Statement_List == NULL)
        {
          printf ("ERROR: in Task_Type::Display() -- Task NOT setup! \n");
          APP_ABORT();
        }

#if PRINTOUT_TASK_LIST
     for (Array_Statement_Type *s = Head_Of_Statement_List; s != NULL; s = s->Next_List_Element_In_Region)
        {
          s->Display();
        }
#endif
   }


void Task_Type::Optimize_Binary_Operators_To_Aggregate_Operators ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of Task_Type::Optimize_Binary_Operators_To_Aggregate_Operators! \n");

     if (Head_Of_Statement_List == NULL)
        {
          printf ("ERROR: in Task_Type::Optimize_Binary_Operators_To_Aggregate_Operators() -- Task NOT setup! \n");
          APP_ABORT();
        }

  // Execute each statement!
     for (Array_Statement_Type *s = Head_Of_Statement_List; s != NULL; s = s->Next_List_Element_In_Region)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
             {
               printf ("Execute the following statement! \n");
               s->Display();
             }
#endif

#if COMPILE_AGGREGATE_OPERATOR_OPTIMIZATIONS
          s->Optimize_Binary_Operators_To_Aggregate_Operators();
#else
          printf ("A++ compiled with COMPILE_AGGREGATE_OPERATOR_OPTIMIZATIONS == FALSE \n");
#endif
        }
   }


