// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if !defined(COMPILE_APP)
#define COMPILE_PPP
#endif

#include "A++.h"

#define EXECUTE_TASKS_SEPERATELY TRUE

// ****************************************************************************
// ****************************************************************************
//                     Set_Of_Tasks class member functions
// ****************************************************************************
// ****************************************************************************

Set_Of_Tasks::~Set_Of_Tasks ()
   {
     if (APP_DEBUG > 0)
          printf ("Destructor called for Set_Of_Tasks! \n");

     Task_List_Head_Pointer = NULL;
     Task_List_Tail_Pointer = NULL;
   }

Set_Of_Tasks::Set_Of_Tasks ()
   {
     if (APP_DEBUG > 0)
          printf ("Constructor called for Set_Of_Tasks! \n");

  // printf ("Error: default constructor called for Set_Of_Tasks! \n");
  // APP_ABORT();

     Task_List_Head_Pointer = NULL;
     Task_List_Tail_Pointer = NULL;

  // *this = Array_Statement_Type::Find_Independent_Sets_Of_Statements ()
   }

void Set_Of_Tasks::Add_Task ( Task_Type *Head_Of_Task_Statement_List )
   {
     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::Add_Task()! \n");

     if (Head_Of_Task_Statement_List == NULL)
        {
          printf ("ERROR: Head_Of_Task_Statement_List == NULL in Set_Of_Tasks::Add_Task! \n");
          APP_ABORT();
        }

     if (Task_List_Head_Pointer == NULL)
        {
          Task_List_Head_Pointer = Head_Of_Task_Statement_List;
          Task_List_Tail_Pointer = Task_List_Head_Pointer;
        }
       else
        {
          Task_List_Tail_Pointer->Next_Task = Head_Of_Task_Statement_List;
          Task_List_Tail_Pointer = Task_List_Tail_Pointer->Next_Task;
        }
   }


void Set_Of_Tasks::Execute () const
   {
     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::Execute ()! \n");

#if EXECUTE_TASKS_SEPERATELY
  // This executes the tasks one at a time, but with the correct task support
  // they could be executed in parallel (but we do that later)!
     for (Task_Type *t = Task_List_Head_Pointer; t != NULL; t=t->Next_Task)
        {
          if (APP_DEBUG > 0)
               printf ("Call t->Execute()! \n");

          t->Execute();
        }
#else
     if (APP_DEBUG > 0)
          printf ("Just execute the BIG list of operations -- Call Array_Statement_Type::Execute_Statement_List() \n");

     Array_Statement_Type::Execute_Statement_List ();
#endif
   }

void Set_Of_Tasks::Display() const
   {
     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::Display ()! \n");

     printf ("\n");
     printf ("Display the Task Set! \n");
     printf ("\n");

     int Task_Index = 0;

     for (Task_Type *t = Task_List_Head_Pointer; t != NULL; t=t->Next_Task)
        {
          printf ("\n");
          printf ("*********************************************************** \n");
          printf ("Task_Index = %d \n",Task_Index);
          t->Display();
          printf ("*********************************************************** \n");
          Task_Index++;
          printf ("\n");
        }
   }

void Set_Of_Tasks::Display_Dependence_Info() const
   {
  // This provides a better interface for output of task info!

     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::Display_Dependence_Info ()! \n");

     Array_Statement_Type::Display_Dependence_Info();
   }

bool Set_Of_Tasks::Setup () const
   {
  // Check to see if Set_Of_Tasks has been previously setup.
     bool Return_Value = TRUE;

     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::Setup () \n");

     if ( (Task_List_Head_Pointer == NULL) && (Task_List_Tail_Pointer == NULL) )
        {
          Return_Value = FALSE;
        }

  // printf ("Task_List_Head_Pointer = %p \n",Task_List_Head_Pointer);
  // printf ("Task_List_Tail_Pointer = %p \n",Task_List_Tail_Pointer);

     return Return_Value;
   }

void Set_Of_Tasks::Clear ()
   {
  // Void this set of tasks.

     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::Clear () \n");

     printf ("Warning: possible memory leakage in Set_Of_Tasks::Clear! \n");

     Task_List_Head_Pointer = NULL;
     Task_List_Tail_Pointer = NULL;
   }

Set_Of_Tasks::Set_Of_Tasks ( const Set_Of_Tasks & X )
   {
  // Copy constructor!

     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::Set_Of_Tasks ( const Set_Of_Tasks & X )! \n");

     Task_List_Head_Pointer = X.Task_List_Head_Pointer;
     Task_List_Tail_Pointer = X.Task_List_Tail_Pointer;
   }


Set_Of_Tasks & Set_Of_Tasks::operator= ( const Set_Of_Tasks & X )
   {
     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::operator= ( const Set_Of_Tasks & X )! \n");

     if (Task_List_Head_Pointer != NULL)
        {
          if (APP_DEBUG > 0)
               printf ("NOTE: in Set_Of_Tasks::operator= current valid task will be overwritten (and perhaps lost)! \n");
       // APP_ABORT();
        }

     if (!X.Setup())
        {
          printf ("ERROR: in Set_Of_Tasks::operator= Lhs is not setup! \n");
          APP_ABORT();
        }

     Task_List_Head_Pointer = X.Task_List_Head_Pointer;
     Task_List_Tail_Pointer = X.Task_List_Tail_Pointer;

     return *this;
   }

#if 0
Set_Of_Tasks & Set_Of_Tasks::operator= ( const void * X )
   {
     if (APP_DEBUG > 0)
          printf ("Inside of Set_Of_Tasks::operator= ( const void * X )! \n");

     if (X != NULL)
        {
          printf ("ERROR: in Set_Of_Tasks::operator= NULL assigned a non NULL pointer! \n");
          APP_ABORT();
        }

     Task_List_Head_Pointer = NULL;
     Task_List_Tail_Pointer = NULL;

     return *this;
   }
#endif





