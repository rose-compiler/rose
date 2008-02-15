// Allow repeated includes of lazy_task.h without error
#ifndef _APP_LAZY_TASK_H
#define _APP_LAZY_TASK_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif

class Array_Statement_Type;

class Task_Type
   {
  // Stores info about the task (list of array expressions)!
     public:
          int Number_Of_Statements;
          int Number_Of_Operations;
          Array_Statement_Type *Head_Of_Statement_List;
          Task_Type *Next_Task;

          static int Number_Of_Tasks;

         ~Task_Type();
          Task_Type();
          Task_Type ( Array_Statement_Type *Head_Of_Statement_List );

          void Execute();
          void Display();

       // Optimization member functions
          void Optimize_Assignment();
          void Optimize_Binary_Operators_To_Aggregate_Operators();

          Task_Type ( const Task_Type & X );
          Task_Type & operator= ( const Task_Type & X );
   };

#endif  /* !defined(_APP_LAZY_TASK_H) */

