#include "A++.h"
extern "C"
   {
/* machine.h is found in MDI/machine.h through a link in A++/inc lude and P++/inc lude */
#include "machine.h"
   }

// The alternative is to just execute the list of unseperated array expressions!
#define EXECUTE_MDI_FUNCTIONS TRUE

// ****************************************************************************
// ****************************************************************************
//               $1$2Array_Aggregate_Operator class member functions
// ****************************************************************************
// ****************************************************************************





doubleArray_Aggregate_Operator::~doubleArray_Aggregate_Operator ()
   {
     printf ("Inside of doubleArray_Aggregate_Operator destructor! \n");
   }

doubleArray_Aggregate_Operator::doubleArray_Aggregate_Operator ()
  // Bugfix (5/12/95) reported by Ted Stern (required for new SGI C++ compiler)
     : Operand_Result()
   {
     printf ("Inside of doubleArray_Aggregate_Operator default constructor! \n");
   }

doubleArray_Aggregate_Operator::doubleArray_Aggregate_Operator ( 
               int Number_Of_Operators ,
               int Input_Operation_Code , 
               MDI_double_Prototype_Aggregate Input_Function_Pointer , 
               doubleArray* Result ,
               doubleArray** Input_Operand_List )
   : Expression_Tree_Node_Type() ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) ,
     Operand_List(Input_Operand_List)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Aggregate_Operator constructor! \n");
#endif

  // We want to assign this here and avoid calling the Expression_Tree_Node_Type
  // constructor taking the op code as input since this constructor would build
  // the execution object into the list of execution objects.  We use this object 
  // to replace an existing chain of execution objects and so we will 
  // do the list surgery explicitly.
     Operation_Code = Input_Operation_Code;
     Degree_Of_Operation = Number_Of_Operators;
   }

doubleArray_Aggregate_Operator::doubleArray_Aggregate_Operator ( 
               int Number_Of_Operators ,
               MDI_double_Prototype_Aggregate Input_Function_Pointer , 
               doubleArray* Result ,
               doubleArray** Input_Operand_List )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) ,
     Operand_List(Input_Operand_List)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Aggregate_Operator constructor! \n");
#endif

     Degree_Of_Operation = Number_Of_Operators;
   }

int doubleArray_Aggregate_Operator::Get_Lhs_Array_ID ()
   {
     printf ("ERROR: Inside of doubleArray_Aggregate_Operator::Get_Lhs_Array_ID -- Exiting ... \n");
     APP_ABORT();
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Aggregate_Operator::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     printf ("ERROR: Inside of doubleArray_Aggregate_Operator::Get_Rhs_Array_ID -- Exiting ... \n");
     APP_ABORT();
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = -1;
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }

void doubleArray_Aggregate_Operator::Display_Expression ()
   {
     printf (" Agg (%d) ",Operand_Result.Array_ID() );

     printf ("ERROR: Inside of doubleArray_Aggregate_Operator::Display_Expression -- BOUND NOT COMPUTED PROPERLY! \n");
     APP_ABORT();
     for (int i=0; i < 1; i++)
        {
          printf ("%d ",Operand_List[i]->Array_ID() );
        }
     printf (": \n");
   }

bool doubleArray_Aggregate_Operator::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Aggregate_Operator::Is_Rhs_A_Temporary \n");

     printf ("ERROR: Inside of doubleArray_Aggregate_Operator::Is_Rhs_A_Temporary -- Exiting ... \n");
     APP_ABORT();
  // return Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ||
  //        Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary;
     return FALSE;
   }

int doubleArray_Aggregate_Operator::Get_Rhs_Temporary_Operand_ID () const
   {
     printf ("ERROR: Inside of doubleArray_Aggregate_Operator::Get_Rhs_Temporary_Operand_ID -- Exiting ... \n");
     APP_ABORT();
  // if ( ! (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ||
  //         Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary) )
  //    {
  //      printf ("ERROR: Both Operant_Lhs and Operand_Rhs are not temporaries! \n");
  //      APP_ABORT();
  //    }

  // return  ( Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ) ?
  //         Operand_Lhs.Original_Referenced_Array->Array_ID() :
  //         Operand_Rhs.Original_Referenced_Array->Array_ID();

     return -1;
   }

void * doubleArray_Aggregate_Operator::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Aggregate_Operator::Get_Rhs_Array ( void** Operand_List )
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: void doubleArray_Aggregate_Operator::Get_Rhs_Array should not be called! Operand_List = %p \n",Operand_List);
     APP_ABORT();
   }

void doubleArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: Inside of doubleArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array -- Exiting ... \n");
     APP_ABORT();
   }

void doubleArray_Aggregate_Operator::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Aggregate_Operator::Execute \n");
#endif    
 
#if !defined(PPP)
     if (Function_Pointer == NULL)
        { 
          printf ("ERROR: Inside of doubleArray_Aggregate_Operator::Execute -- Function_Pointer == NULL \n");
          APP_ABORT();
        } 
 
     if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
        { 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Allocating memory for array data (result array)! \n");
#endif    
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        } 
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( Degree_Of_Operation , *(Operand_Result.Original_Referenced_Array) , Operand_List );
#endif

  // Might have to delete temporaries!
#endif
   }

void doubleArray_Aggregate_Operator::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }


floatArray_Aggregate_Operator::~floatArray_Aggregate_Operator ()
   {
     printf ("Inside of floatArray_Aggregate_Operator destructor! \n");
   }

floatArray_Aggregate_Operator::floatArray_Aggregate_Operator ()
  // Bugfix (5/12/95) reported by Ted Stern (required for new SGI C++ compiler)
     : Operand_Result()
   {
     printf ("Inside of floatArray_Aggregate_Operator default constructor! \n");
   }

floatArray_Aggregate_Operator::floatArray_Aggregate_Operator ( 
               int Number_Of_Operators ,
               int Input_Operation_Code , 
               MDI_float_Prototype_Aggregate Input_Function_Pointer , 
               floatArray* Result ,
               floatArray** Input_Operand_List )
   : Expression_Tree_Node_Type() ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) ,
     Operand_List(Input_Operand_List)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Aggregate_Operator constructor! \n");
#endif

  // We want to assign this here and avoid calling the Expression_Tree_Node_Type
  // constructor taking the op code as input since this constructor would build
  // the execution object into the list of execution objects.  We use this object 
  // to replace an existing chain of execution objects and so we will 
  // do the list surgery explicitly.
     Operation_Code = Input_Operation_Code;
     Degree_Of_Operation = Number_Of_Operators;
   }

floatArray_Aggregate_Operator::floatArray_Aggregate_Operator ( 
               int Number_Of_Operators ,
               MDI_float_Prototype_Aggregate Input_Function_Pointer , 
               floatArray* Result ,
               floatArray** Input_Operand_List )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) ,
     Operand_List(Input_Operand_List)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Aggregate_Operator constructor! \n");
#endif

     Degree_Of_Operation = Number_Of_Operators;
   }

int floatArray_Aggregate_Operator::Get_Lhs_Array_ID ()
   {
     printf ("ERROR: Inside of floatArray_Aggregate_Operator::Get_Lhs_Array_ID -- Exiting ... \n");
     APP_ABORT();
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Aggregate_Operator::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     printf ("ERROR: Inside of floatArray_Aggregate_Operator::Get_Rhs_Array_ID -- Exiting ... \n");
     APP_ABORT();
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = -1;
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }

void floatArray_Aggregate_Operator::Display_Expression ()
   {
     printf (" Agg (%d) ",Operand_Result.Array_ID() );

     printf ("ERROR: Inside of floatArray_Aggregate_Operator::Display_Expression -- BOUND NOT COMPUTED PROPERLY! \n");
     APP_ABORT();
     for (int i=0; i < 1; i++)
        {
          printf ("%d ",Operand_List[i]->Array_ID() );
        }
     printf (": \n");
   }

bool floatArray_Aggregate_Operator::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Aggregate_Operator::Is_Rhs_A_Temporary \n");

     printf ("ERROR: Inside of floatArray_Aggregate_Operator::Is_Rhs_A_Temporary -- Exiting ... \n");
     APP_ABORT();
  // return Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ||
  //        Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary;
     return FALSE;
   }

int floatArray_Aggregate_Operator::Get_Rhs_Temporary_Operand_ID () const
   {
     printf ("ERROR: Inside of floatArray_Aggregate_Operator::Get_Rhs_Temporary_Operand_ID -- Exiting ... \n");
     APP_ABORT();
  // if ( ! (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ||
  //         Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary) )
  //    {
  //      printf ("ERROR: Both Operant_Lhs and Operand_Rhs are not temporaries! \n");
  //      APP_ABORT();
  //    }

  // return  ( Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ) ?
  //         Operand_Lhs.Original_Referenced_Array->Array_ID() :
  //         Operand_Rhs.Original_Referenced_Array->Array_ID();

     return -1;
   }

void * floatArray_Aggregate_Operator::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Aggregate_Operator::Get_Rhs_Array ( void** Operand_List )
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: void floatArray_Aggregate_Operator::Get_Rhs_Array should not be called! Operand_List = %p \n",Operand_List);
     APP_ABORT();
   }

void floatArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: Inside of floatArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array -- Exiting ... \n");
     APP_ABORT();
   }

void floatArray_Aggregate_Operator::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Aggregate_Operator::Execute \n");
#endif    
 
#if !defined(PPP)
     if (Function_Pointer == NULL)
        { 
          printf ("ERROR: Inside of floatArray_Aggregate_Operator::Execute -- Function_Pointer == NULL \n");
          APP_ABORT();
        } 
 
     if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
        { 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Allocating memory for array data (result array)! \n");
#endif    
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        } 
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( Degree_Of_Operation , *(Operand_Result.Original_Referenced_Array) , Operand_List );
#endif

  // Might have to delete temporaries!
#endif
   }

void floatArray_Aggregate_Operator::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }



#define INTARRAY 
intArray_Aggregate_Operator::~intArray_Aggregate_Operator ()
   {
     printf ("Inside of intArray_Aggregate_Operator destructor! \n");
   }

intArray_Aggregate_Operator::intArray_Aggregate_Operator ()
  // Bugfix (5/12/95) reported by Ted Stern (required for new SGI C++ compiler)
     : Operand_Result()
   {
     printf ("Inside of intArray_Aggregate_Operator default constructor! \n");
   }

intArray_Aggregate_Operator::intArray_Aggregate_Operator ( 
               int Number_Of_Operators ,
               int Input_Operation_Code , 
               MDI_int_Prototype_Aggregate Input_Function_Pointer , 
               intArray* Result ,
               intArray** Input_Operand_List )
   : Expression_Tree_Node_Type() ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) ,
     Operand_List(Input_Operand_List)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Aggregate_Operator constructor! \n");
#endif

  // We want to assign this here and avoid calling the Expression_Tree_Node_Type
  // constructor taking the op code as input since this constructor would build
  // the execution object into the list of execution objects.  We use this object 
  // to replace an existing chain of execution objects and so we will 
  // do the list surgery explicitly.
     Operation_Code = Input_Operation_Code;
     Degree_Of_Operation = Number_Of_Operators;
   }

intArray_Aggregate_Operator::intArray_Aggregate_Operator ( 
               int Number_Of_Operators ,
               MDI_int_Prototype_Aggregate Input_Function_Pointer , 
               intArray* Result ,
               intArray** Input_Operand_List )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) ,
     Operand_List(Input_Operand_List)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Aggregate_Operator constructor! \n");
#endif

     Degree_Of_Operation = Number_Of_Operators;
   }

int intArray_Aggregate_Operator::Get_Lhs_Array_ID ()
   {
     printf ("ERROR: Inside of intArray_Aggregate_Operator::Get_Lhs_Array_ID -- Exiting ... \n");
     APP_ABORT();
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Aggregate_Operator::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     printf ("ERROR: Inside of intArray_Aggregate_Operator::Get_Rhs_Array_ID -- Exiting ... \n");
     APP_ABORT();
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = -1;
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }

void intArray_Aggregate_Operator::Display_Expression ()
   {
     printf (" Agg (%d) ",Operand_Result.Array_ID() );

     printf ("ERROR: Inside of intArray_Aggregate_Operator::Display_Expression -- BOUND NOT COMPUTED PROPERLY! \n");
     APP_ABORT();
     for (int i=0; i < 1; i++)
        {
          printf ("%d ",Operand_List[i]->Array_ID() );
        }
     printf (": \n");
   }

bool intArray_Aggregate_Operator::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Aggregate_Operator::Is_Rhs_A_Temporary \n");

     printf ("ERROR: Inside of intArray_Aggregate_Operator::Is_Rhs_A_Temporary -- Exiting ... \n");
     APP_ABORT();
  // return Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ||
  //        Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary;
     return FALSE;
   }

int intArray_Aggregate_Operator::Get_Rhs_Temporary_Operand_ID () const
   {
     printf ("ERROR: Inside of intArray_Aggregate_Operator::Get_Rhs_Temporary_Operand_ID -- Exiting ... \n");
     APP_ABORT();
  // if ( ! (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ||
  //         Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary) )
  //    {
  //      printf ("ERROR: Both Operant_Lhs and Operand_Rhs are not temporaries! \n");
  //      APP_ABORT();
  //    }

  // return  ( Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Is_A_Temporary ) ?
  //         Operand_Lhs.Original_Referenced_Array->Array_ID() :
  //         Operand_Rhs.Original_Referenced_Array->Array_ID();

     return -1;
   }

void * intArray_Aggregate_Operator::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Aggregate_Operator::Get_Rhs_Array ( void** Operand_List )
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: void intArray_Aggregate_Operator::Get_Rhs_Array should not be called! Operand_List = %p \n",Operand_List);
     APP_ABORT();
   }

void intArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: Inside of intArray_Aggregate_Operator::Replace_Temporary_Result_With_Lhs_Array -- Exiting ... \n");
     APP_ABORT();
   }

void intArray_Aggregate_Operator::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Aggregate_Operator::Execute \n");
#endif    
 
#if !defined(PPP)
     if (Function_Pointer == NULL)
        { 
          printf ("ERROR: Inside of intArray_Aggregate_Operator::Execute -- Function_Pointer == NULL \n");
          APP_ABORT();
        } 
 
     if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
        { 
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Allocating memory for array data (result array)! \n");
#endif    
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        } 
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( Degree_Of_Operation , *(Operand_Result.Original_Referenced_Array) , Operand_List );
#endif

  // Might have to delete temporaries!
#endif
   }

void intArray_Aggregate_Operator::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }


#undef INTARRAY 
 











