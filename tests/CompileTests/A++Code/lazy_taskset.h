// Allow repeated includes of lazy_taskset.h without error
#ifndef _APP_LAZY_TASKSET_H
#define _APP_LAZY_TASKSET_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will 
// build the library (factor of 5-10). 
#ifdef GNU
#pragma interface
#endif

class Task_Type;

class Set_Of_Tasks
   {
  // This class stores tasks as a linked list!
     public:
         Task_Type *Task_List_Head_Pointer;
         Task_Type *Task_List_Tail_Pointer;

        ~Set_Of_Tasks ();
         Set_Of_Tasks ();
         Set_Of_Tasks ( Task_Type *Head_Of_Statement_List );

         void Add_Task ( Task_Type *Head_Of_Statement_List );

         void Execute() const;
         void Display() const;
         void Display_Dependence_Info() const;
         bool Setup() const;
         void Clear();

         Set_Of_Tasks ( const Set_Of_Tasks & X );
         Set_Of_Tasks & operator= ( const Set_Of_Tasks & X );
   };

#endif  /* !defined(_APP_LAZY_TASKSET_H) */

