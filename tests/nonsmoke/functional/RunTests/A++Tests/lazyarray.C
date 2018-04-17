// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma implementation "lazyarray.h"
#pragma implementation "lazy_expression.h"
#endif

#include "A++.h"
extern "C"
   {
#include "machine.h"
   }

// The alternative is to just execute the list of unseperated array expressions!
#define EXECUTE_MDI_FUNCTIONS TRUE

#if !defined(PPP)
int Global_Zero_Array [4] = {0,0,0,0};
#else
extern int Global_Zero_Array [4];
#endif

// ****************************************************************************
// ****************************************************************************
//               Operand_Storage class member functions
// ****************************************************************************
// ****************************************************************************






INLINE void Expression_Tree_Node_Type::Delete_If_Temporary_Or_View ( const doubleArray & X )
   {
  /* This function is similar to the global Delete_If_Temporary_Or_View function in
     abstarct_op.C but it only deletes the data since the doubleArray object and
     it's array descriptor can be reused (and should be) to allow multiple
     executions of the of the execution objects!
  */

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Expression_Tree_Node_Type::Delete_If_Temporary_Or_View! \n");
#endif

  /* Constants can never be a temporary, it is the law! */
  // if (X.isTemporary() || X.Array_Descriptor->Is_A_View)
     if (X.isTemporary())
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Inside of Delete_If_Temporary_Or_View (DELETING Array object)! \n");
#endif

       // delete & (doubleArray &) X;    // Notice that we cast away const here!
          ((doubleArray &) X).Delete_Array_Data ();
        }
   }

INLINE void Expression_Tree_Node_Type::Delete_If_Temporary_Or_View ( const floatArray & X )
   {
  /* This function is similar to the global Delete_If_Temporary_Or_View function in
     abstarct_op.C but it only deletes the data since the floatArray object and
     it's array descriptor can be reused (and should be) to allow multiple
     executions of the of the execution objects!
  */

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Expression_Tree_Node_Type::Delete_If_Temporary_Or_View! \n");
#endif

  /* Constants can never be a temporary, it is the law! */
  // if (X.isTemporary() || X.Array_Descriptor->Is_A_View)
     if (X.isTemporary())
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Inside of Delete_If_Temporary_Or_View (DELETING Array object)! \n");
#endif

       // delete & (floatArray &) X;    // Notice that we cast away const here!
          ((floatArray &) X).Delete_Array_Data ();
        }
   }

INLINE void Expression_Tree_Node_Type::Delete_If_Temporary_Or_View ( const intArray & X )
   {
  /* This function is similar to the global Delete_If_Temporary_Or_View function in
     abstarct_op.C but it only deletes the data since the intArray object and
     it's array descriptor can be reused (and should be) to allow multiple
     executions of the of the execution objects!
  */

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Expression_Tree_Node_Type::Delete_If_Temporary_Or_View! \n");
#endif

  /* Constants can never be a temporary, it is the law! */
  // if (X.isTemporary() || X.Array_Descriptor->Is_A_View)
     if (X.isTemporary())
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Inside of Delete_If_Temporary_Or_View (DELETING Array object)! \n");
#endif

       // delete & (intArray &) X;    // Notice that we cast away const here!
          ((intArray &) X).Delete_Array_Data ();
        }
   }


doubleArray_Function_Steal_Data::doubleArray_Function_Steal_Data ( int Input_Operation_Code , 
                                         const doubleArray & Result , const doubleArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Operand_Result(Result) , Operand_X(X)
   { 
     Execute_MDI_Function = TRUE;
   }

// This is a faster constructor for when we do not use defered evaluation!
doubleArray_Function_Steal_Data::doubleArray_Function_Steal_Data ( const doubleArray & Result , const doubleArray & X )
   : Operand_Result(&((doubleArray &)Result)) , Operand_X(&((doubleArray &)X))
   { 
     Execute_MDI_Function = TRUE;
   }

void doubleArray_Function_Steal_Data::Display_Expression ()
   {
     printf (" S Lhs %d = %d :",Operand_Result.Array_ID(),Operand_X.Array_ID());
   }

#if 1
int doubleArray_Function_Steal_Data::Get_Lhs_Array_ID ()
   {
  // return Operand_Result.Array_ID();
  // The following is not as safe but it is faster!
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_Steal_Data::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
  // Rhs_Array_ID_Array [1] = Operand_X.Array_ID();
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_Steal_Data::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_Steal_Data::Is_Rhs_A_Temporary \n");

  // return FALSE;
     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_Steal_Data::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_Steal_Data::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_Steal_Data::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_Steal_Data::Skip_MDI_Function ()
   {
     Execute_MDI_Function = FALSE;
   }

void doubleArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // This function is use to optimize out the use of temporaries from the List of Execution Objects (LEO)!
  // We have to input a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
#if defined(PPP)
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray != NULL) 
#else
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL) 
#endif
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n");
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary 
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((doubleArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
#if defined(PPP)
               Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray = ((doubleArray *) (X_Array))->Array_Descriptor.SerialArray;
#else
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
#endif
             }
        }
       else
        {
          printf ("ERROR: in doubleArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID does not exist in this expression! \n");
          APP_ABORT();
        }
   }

void doubleArray_Function_Steal_Data::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_Steal_Data::Execute \n");
#endif

     if (Execute_MDI_Function)
        {
#if COMPILE_DEBUG_STATEMENTS
       // error checking
          if ( !( Operand_Result.Original_Referenced_Array->Binary_Conformable (*Operand_X.Original_Referenced_Array) && 
                  Operand_X.Original_Referenced_Array->isTemporary() ) )
             {
               printf ("ERROR: in doubleArray_Function_0::Execute -- !( Binary_Conformable (Operand_X) && Operand_X.isTemporary() ) \n");
               APP_ABORT();
             }
#endif

#if defined(PPP)
          doubleSerialArray** Lhs_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray);
          doubleSerialArray** X_Data_Pointer   = &(Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray);
#else
          double*APP_RESTRICT_MACRO* Lhs_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
          double*APP_RESTRICT_MACRO* X_Data_Pointer   = &(Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data);
#endif

#if COMPILE_DEBUG_STATEMENTS
       // error checking
          if (*Lhs_Data_Pointer == NULL)
             {
               printf ("ERROR: in doubleArray_Function_Steal_Data::Execute *Lhs_Data_Pointer == NULL! \n");
               APP_ABORT();
             }
 
       // error checking
          if (*X_Data_Pointer == NULL)
             {
               printf ("ERROR: in doubleArray_Function_Steal_Data::Execute *X_Data_Pointer == NULL! \n");
               APP_ABORT();
             }
 
          if (APP_DEBUG > 3)
               printf ("After Setup pointers to pointers! \n");
#endif

       // Make space for the new data by deleting the old data!
          Operand_Result.Original_Referenced_Array->Delete_Array_Data ();
 
       // Steal the pointer (to the original array) from the Rhs's data
          *Lhs_Data_Pointer = *X_Data_Pointer;
 
       // We can set the correct one by using the fact that
       // X_Data_Pointer is a pointer to a pointer to the array data!
          *X_Data_Pointer = NULL;
 
       // The X will be deleted but until then we can't really call any
       // of it's member functions since there is no data associated with it.
          Delete_If_Temporary_Or_View ( *Operand_X.Original_Referenced_Array );
 
       // Now we have to delete the "this" object if it is a View! But we are the this object
       // so we have to be very carefull how this is done!

       // In the case of execution of the defered evaluation objects we cannot
       // delete this Lhs since we want to do multiple executions of the 
       // defered execution list!  We have used it's data and that is enough!
       // Delete_Lhs_If_Temporary_Or_View ( *Operand_Result.Original_Referenced_Array );
        }
   }

doubleArray_Function_0::doubleArray_Function_0 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_0 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_X(X)
   { }

doubleArray_Function_0::doubleArray_Function_0 ( MDI_double_Prototype_0 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((doubleArray &)Result)) , Operand_X(&((doubleArray &)X))
   { }

void doubleArray_Function_0::Display_Expression ()
   {
     printf ("U Lhs %d = %d :",Operand_Result.Array_ID(),Operand_X.Array_ID());
   }

#if 1
int doubleArray_Function_0::Get_Lhs_Array_ID ()
   {
  // return Operand_Result.Array_ID();
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_0::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
  // Rhs_Array_ID_Array [1] = Operand_X.Array_ID();
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_0::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_0::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_0::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_0::Get_Result_Array () 
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_0::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_0::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_0::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

     if ( Operand_Result.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
#if defined(PPP)
          if (Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray != NULL) 
#else
          if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL) 
#endif
             {
               printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary 
          Operand_Result.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((doubleArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!
               Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Result.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
#if defined(PPP)
               Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray = ((doubleArray *) (X_Array))->Array_Descriptor.SerialArray;
#else
               Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
#endif
             }
        }
       else
        {
          printf ("ERROR: X_Array_ID does not exist in this expression! \n");
          APP_ABORT();
        }
   }


void doubleArray_Function_0::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_0::Execute \n");
#endif

#if !defined(PPP)
     double*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     double*APP_RESTRICT_MACRO X_Data_Pointer       = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_0::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) X_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , X_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain),
                           (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain),
                           Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_X.Original_Referenced_Array );
#endif
   }

doubleArray_Function_1::doubleArray_Function_1 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_1 Input_Function_Pointer , 
                                         const doubleArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

doubleArray_Function_1::doubleArray_Function_1 ( MDI_double_Prototype_1 Input_Function_Pointer , 
                                         const doubleArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((doubleArray &)X))
   { }

void doubleArray_Function_1::Display_Expression ()
   {
     printf (" U %d :",Operand_X.Array_ID());
   }

#if 1
int doubleArray_Function_1::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_1::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_1::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_1::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_1::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_1::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_1::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_1::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_1::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((doubleArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: doubleArray_Function_1::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void doubleArray_Function_1::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_1::Execute \n");
#endif

#if !defined(PPP)
     double*APP_RESTRICT_MACRO X_Data_Pointer = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_1::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( X_Data_Pointer , Mask_Array_Data ,
                 (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                 Mask_Descriptor );
#endif
#endif
   }

doubleArray_Function_2::~doubleArray_Function_2 ()
   {
     printf ("Inside of doubleArray_Function_2 destructor! \n");
   }

doubleArray_Function_2::doubleArray_Function_2 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_2 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & Lhs , const doubleArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_2 constructor! \n");
#endif
   }

doubleArray_Function_2::doubleArray_Function_2 ( MDI_double_Prototype_2 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & Lhs , const doubleArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((doubleArray &)Result)) , Operand_Lhs(&((doubleArray &)Lhs)) , Operand_Rhs(&((doubleArray &)Rhs))
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_2 constructor! \n");
#endif
   }

void doubleArray_Function_2::Display_Expression ()
   {
     printf (" B (%d) %d %d  :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int doubleArray_Function_2::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_2::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_2::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_2::Is_Rhs_A_Temporary \n");

     return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
            Operand_Rhs.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_2::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( ! (Operand_Lhs.Original_Referenced_Array->isTemporary() ||
             Operand_Rhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Both Operant_Lhs and Operand_Rhs are not temporaries! \n");
          APP_ABORT();
        }

     return  ( Operand_Lhs.Original_Referenced_Array->isTemporary() ) ?
             Operand_Lhs.Original_Referenced_Array->Array_ID() :
             Operand_Rhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_2::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_2::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void doubleArray_Function_2::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_2::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Result to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Result is a view then setting this to a view is redundent!
          if ( !((doubleArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          if ( Operand_Rhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
             {
               if (Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
                  {
                    printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n");
                    APP_ABORT();
                  }
     
            // Now assign array descriptor from Result to the Temporary
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;
     
            // Technically all we want is a view -- if the Result is a view then setting this to a view is redundent!
               if ( !((doubleArray *) (X_Array))->isView() )
                  {
                 // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!
                    Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
                 // Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
                    Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
                  }
             }
            else
             {
               printf ("ERROR: doubleArray_Function_2::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
               APP_ABORT();
             }
        }
#endif
   }

void doubleArray_Function_2::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_2::Execute \n");
#endif

#if !defined(PPP)
     double*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     double*APP_RESTRICT_MACRO  Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     double*APP_RESTRICT_MACRO  Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_2::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_2::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          if (APP_DEBUG > 0)
               printf ("Allocating memory for array data! \n");
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }
       else
        {
          if (APP_DEBUG > 0)
               printf ("Memory already allocated! \n");
        }

     if (APP_DEBUG > 0)
          Operand_Result.Original_Referenced_Array->display ("Operand_Result.Original_Referenced_Array");

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , Lhs_Data_Pointer , Rhs_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif

     if (APP_DEBUG > 0)
          Operand_Result.Original_Referenced_Array->display ("Operand_Result.Original_Referenced_Array");

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );

  // printf ("NOTE: Possible small and infrequent memory leak in doubleArray_Function_2::Execute () \n");
  /* This should not be done in the case where the LHS was reused! 
  // When the this function has been called because a temporary was formed then we 
  // don't want to just delete the RHS? But if the Rhs was not used and it is
  // a temporary then it should be deleted?  Look at the use in abstract_op.C.m4!
  // if this has to be deleted then check for pointer equality between Result and Rhs
  // to test for reuse of the Rhs.  It might actually be fine to just uncomment the
  // following delete since it might have not been a problem as I first thought
  // when I was debugging this section!
  */
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data != Rhs_Data_Pointer)
     if (*Result_Data_Pointer != Rhs_Data_Pointer)
          Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }

doubleArray_Function_3::doubleArray_Function_3 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_3 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Rhs(Rhs)
   { 
     Execute_MDI_Function = TRUE;
   }

doubleArray_Function_3::doubleArray_Function_3 ( MDI_double_Prototype_3 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((doubleArray &)Result)) , Operand_Rhs(&((doubleArray &)Rhs))
   { 
     Execute_MDI_Function = TRUE;
   }

void doubleArray_Function_3::Display_Expression ()
   {
     printf (" B|E %d  %d :",Operand_Result.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int doubleArray_Function_3::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_3::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_3::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_3::Is_Rhs_A_Temporary \n");

     return Operand_Rhs.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_3::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Rhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Rhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Rhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_3::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_3::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void doubleArray_Function_3::Skip_MDI_Function ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_3::Skip_MDI_Function! \n");

     Execute_MDI_Function = FALSE;
   }

void doubleArray_Function_3::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_3::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Rhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Rhs.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((doubleArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Setup the view of the Lhs since Lhs was not a view! \n");
#endif
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
            else
             {
            // printf ("The Lhs was a view so the array descriptor need not be modified! \n");
             }
        }
       else
        {
          printf ("ERROR: doubleArray_Function_3::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void doubleArray_Function_3::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_3::Execute \n");
#endif

#if !defined(PPP)
     double*APP_RESTRICT_MACRO Result_Data_Pointer = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
     double*APP_RESTRICT_MACRO Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Result_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_3::Execute Result_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_3::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Rhs_Data_Pointer;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     if (Execute_MDI_Function)
        {
          (*Function_Pointer) ( Result_Data_Pointer , Rhs_Data_Pointer , Mask_Array_Data ,
                                (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                                (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                                Mask_Descriptor );
        }
#endif

     Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }


doubleArray_Function_4::doubleArray_Function_4 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_4 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & Lhs , double Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

doubleArray_Function_4::doubleArray_Function_4 ( MDI_double_Prototype_4 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & Lhs , double Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((doubleArray &)Result)) , Operand_Lhs(&((doubleArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void doubleArray_Function_4::Display_Expression ()
   {
     printf (" B (%d)  %d  Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int doubleArray_Function_4::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_4::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_4::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_4::Is_Rhs_A_Temporary \n");

     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_4::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_4::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_4::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void doubleArray_Function_4::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_4::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((doubleArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: doubleArray_Function_4::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void doubleArray_Function_4::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_4::Execute \n");
#endif

#if !defined(PPP)
     double*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     double*APP_RESTRICT_MACRO  Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_4::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
                           Operand_Scalar , 
#else
                           &Operand_Scalar , 
#endif
                           Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }


doubleArray_Function_5::doubleArray_Function_5 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_5 Input_Function_Pointer , 
                                         const doubleArray & Lhs , double Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

doubleArray_Function_5::doubleArray_Function_5 ( MDI_double_Prototype_5 Input_Function_Pointer , 
                                         const doubleArray & Lhs , double Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Lhs(&((doubleArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void doubleArray_Function_5::Display_Expression ()
   {
     printf (" B|E %d  Scalar :",Operand_Lhs.Array_ID());
   }

#if 1
int doubleArray_Function_5::Get_Lhs_Array_ID ()
   {
     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_5::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_5::Is_Rhs_A_Temporary ()
   {
  // Since the Result and Lhs have the same Array_Id there is an anti-dependence here
  // so we could return false!

     printf ("Inside of  doubleArray_Function_5::Is_Rhs_A_Temporary -- This function should not be called! \n");
     APP_ABORT();

     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_5::Get_Rhs_Temporary_Operand_ID () const
   {
     printf ("Inside of  doubleArray_Function_5::Get_Rhs_Temporary_Operand_ID -- This function should not be called! \n");
     APP_ABORT();

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_5::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Lhs.Original_Referenced_Array;
   }

void doubleArray_Function_5::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void doubleArray_Function_5::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_5::Replace_Temporary_Result_With_Lhs_Array X_Array = %p X_Array_ID = %d \n",X_Array,X_Array_ID);
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: doubleArray_Function_5::Replace_Temporary_Result_With_Lhs_Array -- Should not be called! \n");
     APP_ABORT();
   }


void doubleArray_Function_5::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_5::Execute \n");
#endif

#if !defined(PPP)
     double*APP_RESTRICT_MACRO This_Array_Data = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (This_Array_Data == NULL)
        {
          printf ("ERROR: in doubleArray_Function_5::Execute This_Array_Data == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) This_Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( This_Array_Data , 
#ifndef FORTRAN_MDI
                           Operand_Scalar , 
#else
                           &Operand_Scalar , 
#endif
                           Mask_Array_Data ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif
#endif
   }

doubleArray_Function_6::doubleArray_Function_6 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_6 Input_Function_Pointer , 
                                         const doubleArray & Result , const intArray & Lhs , const doubleArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { }

doubleArray_Function_6::doubleArray_Function_6 ( MDI_double_Prototype_6 Input_Function_Pointer , 
                                         const doubleArray & Result , const intArray & Lhs , const doubleArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((doubleArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Rhs(&((doubleArray &)Rhs))
   { }

void doubleArray_Function_6::Display_Expression ()
   {
  // printf ("Inside of doubleArray_Function_6::Display_Expression \n");
  // printf ("Calling the array ID now! \n");
     printf (" M (%d) %d %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int doubleArray_Function_6::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_6::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_6::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a doubleArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
  //        Operand_Rhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int doubleArray_Function_6::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_6::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in doubleArray_Function_6::Get_Result_Array -- Can't use doubleArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_6::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void doubleArray_Function_6::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_6::Replace_Temporary_Result_With_Lhs_Array X_Array = %p X_Array_ID = %d \n",X_Array,X_Array_ID);
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of doubleArray_Function_6::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void doubleArray_Function_6::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_6::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     double*APP_RESTRICT_MACRO  Rhs_Data_Pointer = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     double*APP_RESTRICT_MACRO  Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_6::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_6::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_6::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( Data_Pointer , Lhs_Data_Pointer , 
                           Rhs_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor);
#endif
#endif
   }

doubleArray_Function_7::doubleArray_Function_7 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_7 Input_Function_Pointer , 
                                         const doubleArray & Result , const intArray & Lhs , double Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

doubleArray_Function_7::doubleArray_Function_7 ( MDI_double_Prototype_7 Input_Function_Pointer , 
                                         const doubleArray & Result , const intArray & Lhs , double Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((doubleArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void doubleArray_Function_7::Display_Expression ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_7::Display_Expression \n");

     printf (" M (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int doubleArray_Function_7::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_7::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_7::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a doubleArray!
     return FALSE;
   }

int doubleArray_Function_7::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_7::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in doubleArray_Function_7::Get_Result_Array -- Can't use doubleArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_7::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void doubleArray_Function_7::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_7::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of doubleArray_Function_7::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void doubleArray_Function_7::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_7::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     double*APP_RESTRICT_MACRO  Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_7::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_7::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
                    Operand_Scalar , 
#else
                    &Operand_Scalar , 
#endif
                    Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif
#endif
   }

#ifndef INTARRAY
doubleArray_Function_8::doubleArray_Function_8 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_8 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & Lhs , int Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

doubleArray_Function_8::doubleArray_Function_8 ( MDI_double_Prototype_8 Input_Function_Pointer , 
                                         const doubleArray & Result , const doubleArray & Lhs , int Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((doubleArray &)Result)) , Operand_Lhs(&((doubleArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void doubleArray_Function_8::Display_Expression ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_8::Display_Expression \n");

     printf (" M (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int doubleArray_Function_8::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_8::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_8::Is_Rhs_A_Temporary ()
   {
     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_8::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_8::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Lhs.Original_Referenced_Array;
   }

void doubleArray_Function_8::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void doubleArray_Function_8::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_8::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((doubleArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: doubleArray_Function_8::Get_Result_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void doubleArray_Function_8::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_8::Execute \n");
#endif

#if !defined(PPP)
     double*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     double*APP_RESTRICT_MACRO Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_8::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_8::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
               Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
               Operand_Scalar , 
#else
               &Operand_Scalar , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }
#endif

doubleArray_Function_9::doubleArray_Function_9 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_9 Input_Function_Pointer , 
                                         const intArray & Result , const doubleArray & Lhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs)
   { }

doubleArray_Function_9::doubleArray_Function_9 ( MDI_double_Prototype_9 Input_Function_Pointer , 
                                         const intArray & Result , const doubleArray & Lhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((doubleArray &)Lhs))
   { }

void doubleArray_Function_9::Display_Expression ()
   {
     printf (" R %d  %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int doubleArray_Function_9::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_9::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_9::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a doubleArray!
     return FALSE;
   }

int doubleArray_Function_9::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_9::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in doubleArray_Function_9::Get_Result_Array -- Can't use doubleArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_9::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void doubleArray_Function_9::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_9::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of doubleArray_Function_9::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void doubleArray_Function_9::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_9::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     double*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_9::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */ 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    *Result_Data_Pointer , Lhs_Data_Pointer , Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }

#if 0
doubleArray_Function_10::doubleArray_Function_10 ( int Input_Operation_Code , 
                                           MDI_double_Prototype_10 Input_Function_Pointer )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer)
   { }

void doubleArray_Function_10::Display_Expression ()
   {
     printf ("ERROR: Inside of doubleArray_Function_10::Display_Expression \n");
     APP_ABORT();
   }

void doubleArray_Function_10::Execute ()
   {
     printf ("ERROR: This object is not used! \n");
     APP_ABORT();
   }
#endif

doubleArray_Function_11::doubleArray_Function_11 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_11 Input_Function_Pointer , 
                                         const intArray & Result , const doubleArray & Lhs , const doubleArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { }

doubleArray_Function_11::doubleArray_Function_11 ( MDI_double_Prototype_11 Input_Function_Pointer , 
                                           const intArray & Result , const doubleArray & Lhs , const doubleArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((doubleArray &)Lhs)) , Operand_Rhs(&((doubleArray &)Rhs))
   { }

void doubleArray_Function_11::Display_Expression ()
   {
     printf (" R (%d) %d %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int doubleArray_Function_11::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_11::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_11::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a doubleArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
  //        Operand_Rhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int doubleArray_Function_11::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_11::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in doubleArray_Function_11::Get_Result_Array -- Can't use doubleArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_11::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void doubleArray_Function_11::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_11::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of doubleArray_Function_11::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void doubleArray_Function_11::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_11::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     double*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     double*APP_RESTRICT_MACRO   Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_11::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_11::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */ 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    *Result_Data_Pointer , Lhs_Data_Pointer ,
                    Rhs_Data_Pointer , Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
     Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }

doubleArray_Function_12::doubleArray_Function_12 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_12 Input_Function_Pointer , 
                                         const intArray & Result , const doubleArray & Lhs , double Scalar )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

doubleArray_Function_12::doubleArray_Function_12 ( MDI_double_Prototype_12 Input_Function_Pointer , 
                                           const intArray & Result , const doubleArray & Lhs , double Scalar )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((doubleArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void doubleArray_Function_12::Display_Expression ()
   {
     printf (" R (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int doubleArray_Function_12::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_12::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_12::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a doubleArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int doubleArray_Function_12::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_12::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in doubleArray_Function_12::Get_Result_Array -- Can't use doubleArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void doubleArray_Function_12::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void doubleArray_Function_12::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_12::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of doubleArray_Function_12::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void doubleArray_Function_12::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_12::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     double*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_12::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
               *Result_Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
               Operand_Scalar , 
#else
               &Operand_Scalar , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }

#if 0
doubleArray_Function_13::doubleArray_Function_13 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_13 Input_Function_Pointer ) 
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer)
   { }

void doubleArray_Function_13::Display_Expression ()
   {
     printf ("Inside of doubleArray_Function_13::Display_Expression \n");
     APP_ABORT();
   }

void doubleArray_Function_13::Execute ()
   {
     printf ("ERROR: This object is not used! \n");
     APP_ABORT();
   }
#endif

#if 1
// This type is not presently used (intArray::indexMap)
doubleArray_Function_14::doubleArray_Function_14 ( int Input_Operation_Code , 
                                         MDI_double_Prototype_14 Input_Function_Pointer , 
                                         const doubleArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

doubleArray_Function_14::doubleArray_Function_14 ( MDI_double_Prototype_14 Input_Function_Pointer , 
                                         const doubleArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((doubleArray &)X))
   { }

void doubleArray_Function_14::Display_Expression ()
   {
     printf (" U %d :",Operand_X.Array_ID());
   }

#if 1
int doubleArray_Function_14::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_14::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_14::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_14::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_14::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_14::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_14::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_14::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_1::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((doubleArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((doubleArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this doubleArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) (X_Array);
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((doubleArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: doubleArray_Function_1::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void doubleArray_Function_14::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_14::Execute \n");
#endif

#if !defined(PPP)
     double*APP_RESTRICT_MACRO X_Data_Pointer = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in doubleArray_Function_14::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( X_Data_Pointer , Mask_Array_Data ,
                 (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                 Mask_Descriptor );
#endif
#endif
   }
#endif

doubleArray_Function_15::doubleArray_Function_15 ( int Input_Operation_Code , 
                                           MDI_double_Prototype_15 Input_Function_Pointer , 
                                           const doubleArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

doubleArray_Function_15::doubleArray_Function_15 ( MDI_double_Prototype_15 Input_Function_Pointer , 
                                         const doubleArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((doubleArray &)X))
   { }

void doubleArray_Function_15::Display_Expression ()
   {
     printf (" indexMap %d :",Operand_X.Array_ID());
   }

#if 1
int doubleArray_Function_15::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_15::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_15::Is_Rhs_A_Temporary ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_15::Is_Rhs_A_Temporary \n");
#endif

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_15::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_15::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_15::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_15::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_15::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // This makes no sense in the case of display and view member functions! (I think)

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: doubleArray_Function_15::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not make sense in this expression! \n",X_Array_ID);
     APP_ABORT();
   }


void doubleArray_Function_15::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_15::Execute \n");
#endif

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          printf ("Note: Where_Statement_Mask ignored for display and view functions! \n");
        }

#if !defined(PPP)
#if EXECUTE_MDI_FUNCTIONS
     printf ("Commented out execution of defered operator 15 \n");
     APP_ABORT();
  // (Operand_X.Original_Referenced_Array->*Function_Pointer) ();
#endif
#endif
   }

#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
doubleArray_Function_16::doubleArray_Function_16 ( int Input_Operation_Code , 
                                           MDI_double_Prototype_16 Input_Function_Pointer , 
                                           const doubleArray & X , const char *Input_Label )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X) ,
     Label (Input_Label)
   { 
  // It might be that this should be a deep copy instead of a shallow copy! (Later)
  // Label = Input_Label;
   }

doubleArray_Function_16::doubleArray_Function_16 ( MDI_double_Prototype_16 Input_Function_Pointer , 
                                         const doubleArray & X , const char *Input_Label )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((doubleArray &)X)) ,
     Label (Input_Label)
   {
  // It might be that this should be a deep copy instead of a shallow copy! (Later)
  // Label = Input_Label;
   }

void doubleArray_Function_16::Display_Expression ()
   {
     printf (" D/V %d :",Operand_X.Array_ID());
   }

#if 1
int doubleArray_Function_16::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void doubleArray_Function_16::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool doubleArray_Function_16::Is_Rhs_A_Temporary ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Function_16::Is_Rhs_A_Temporary \n");
#endif

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int doubleArray_Function_16::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * doubleArray_Function_16::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_16::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void doubleArray_Function_16::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_16::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // This makes no sense in the case of display and view member functions! (I think)

     printf ("ERROR: doubleArray_Function_16::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not make sense in this expression! \n",X_Array_ID);
     APP_ABORT();
   }


void doubleArray_Function_16::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Function_16::Execute \n");
#endif

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          printf ("Note: Where_Statement_Mask ignored for display and view functions! \n");
        }

#if EXECUTE_MDI_FUNCTIONS
     printf ("Commented out execution of defered operator 16 \n");
     APP_ABORT();
  // (Operand_X.Original_Referenced_Array->*Function_Pointer) (Label);
#endif
   }
#endif // for COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS

void doubleArray_Function_Steal_Data::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_Steal_Data::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_0::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_0::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_1::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_1::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_2::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_2::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_3::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_3::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_4::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_4::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_5::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_5::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_6::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_6::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_7::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_7::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }


#ifndef INTARRAY
void doubleArray_Function_8::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_8::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

#endif

void doubleArray_Function_9::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_9::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_11::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_11::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_12::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_12::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_14::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_14::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

void doubleArray_Function_15::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_15::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }


#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
void doubleArray_Function_16::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* doubleArray_Function_16::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new doubleArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_double_Prototype_Aggregate) Function_Pointer, 
                     (doubleArray*) Result, 
                     (doubleArray**) Operand_List );
   }

#endif

floatArray_Function_Steal_Data::floatArray_Function_Steal_Data ( int Input_Operation_Code , 
                                         const floatArray & Result , const floatArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Operand_Result(Result) , Operand_X(X)
   { 
     Execute_MDI_Function = TRUE;
   }

// This is a faster constructor for when we do not use defered evaluation!
floatArray_Function_Steal_Data::floatArray_Function_Steal_Data ( const floatArray & Result , const floatArray & X )
   : Operand_Result(&((floatArray &)Result)) , Operand_X(&((floatArray &)X))
   { 
     Execute_MDI_Function = TRUE;
   }

void floatArray_Function_Steal_Data::Display_Expression ()
   {
     printf (" S Lhs %d = %d :",Operand_Result.Array_ID(),Operand_X.Array_ID());
   }

#if 1
int floatArray_Function_Steal_Data::Get_Lhs_Array_ID ()
   {
  // return Operand_Result.Array_ID();
  // The following is not as safe but it is faster!
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_Steal_Data::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
  // Rhs_Array_ID_Array [1] = Operand_X.Array_ID();
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_Steal_Data::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_Steal_Data::Is_Rhs_A_Temporary \n");

  // return FALSE;
     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_Steal_Data::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_Steal_Data::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_Steal_Data::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_Steal_Data::Skip_MDI_Function ()
   {
     Execute_MDI_Function = FALSE;
   }

void floatArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // This function is use to optimize out the use of temporaries from the List of Execution Objects (LEO)!
  // We have to input a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
#if defined(PPP)
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray != NULL) 
#else
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL) 
#endif
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n");
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary 
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((floatArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this floatArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
#if defined(PPP)
               Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray = ((floatArray *) (X_Array))->Array_Descriptor.SerialArray;
#else
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
#endif
             }
        }
       else
        {
          printf ("ERROR: in floatArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID does not exist in this expression! \n");
          APP_ABORT();
        }
   }

void floatArray_Function_Steal_Data::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_Steal_Data::Execute \n");
#endif

     if (Execute_MDI_Function)
        {
#if COMPILE_DEBUG_STATEMENTS
       // error checking
          if ( !( Operand_Result.Original_Referenced_Array->Binary_Conformable (*Operand_X.Original_Referenced_Array) && 
                  Operand_X.Original_Referenced_Array->isTemporary() ) )
             {
               printf ("ERROR: in floatArray_Function_0::Execute -- !( Binary_Conformable (Operand_X) && Operand_X.isTemporary() ) \n");
               APP_ABORT();
             }
#endif

#if defined(PPP)
          floatSerialArray** Lhs_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray);
          floatSerialArray** X_Data_Pointer   = &(Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray);
#else
          float*APP_RESTRICT_MACRO* Lhs_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
          float*APP_RESTRICT_MACRO* X_Data_Pointer   = &(Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data);
#endif

#if COMPILE_DEBUG_STATEMENTS
       // error checking
          if (*Lhs_Data_Pointer == NULL)
             {
               printf ("ERROR: in floatArray_Function_Steal_Data::Execute *Lhs_Data_Pointer == NULL! \n");
               APP_ABORT();
             }
 
       // error checking
          if (*X_Data_Pointer == NULL)
             {
               printf ("ERROR: in floatArray_Function_Steal_Data::Execute *X_Data_Pointer == NULL! \n");
               APP_ABORT();
             }
 
          if (APP_DEBUG > 3)
               printf ("After Setup pointers to pointers! \n");
#endif

       // Make space for the new data by deleting the old data!
          Operand_Result.Original_Referenced_Array->Delete_Array_Data ();
 
       // Steal the pointer (to the original array) from the Rhs's data
          *Lhs_Data_Pointer = *X_Data_Pointer;
 
       // We can set the correct one by using the fact that
       // X_Data_Pointer is a pointer to a pointer to the array data!
          *X_Data_Pointer = NULL;
 
       // The X will be deleted but until then we can't really call any
       // of it's member functions since there is no data associated with it.
          Delete_If_Temporary_Or_View ( *Operand_X.Original_Referenced_Array );
 
       // Now we have to delete the "this" object if it is a View! But we are the this object
       // so we have to be very carefull how this is done!

       // In the case of execution of the defered evaluation objects we cannot
       // delete this Lhs since we want to do multiple executions of the 
       // defered execution list!  We have used it's data and that is enough!
       // Delete_Lhs_If_Temporary_Or_View ( *Operand_Result.Original_Referenced_Array );
        }
   }

floatArray_Function_0::floatArray_Function_0 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_0 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_X(X)
   { }

floatArray_Function_0::floatArray_Function_0 ( MDI_float_Prototype_0 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((floatArray &)Result)) , Operand_X(&((floatArray &)X))
   { }

void floatArray_Function_0::Display_Expression ()
   {
     printf ("U Lhs %d = %d :",Operand_Result.Array_ID(),Operand_X.Array_ID());
   }

#if 1
int floatArray_Function_0::Get_Lhs_Array_ID ()
   {
  // return Operand_Result.Array_ID();
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_0::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
  // Rhs_Array_ID_Array [1] = Operand_X.Array_ID();
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_0::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_0::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_0::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_0::Get_Result_Array () 
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_0::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_0::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_0::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

     if ( Operand_Result.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
#if defined(PPP)
          if (Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray != NULL) 
#else
          if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL) 
#endif
             {
               printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary 
          Operand_Result.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((floatArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this floatArray will be deleted and the array data preserved!
               Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Result.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
#if defined(PPP)
               Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray = ((floatArray *) (X_Array))->Array_Descriptor.SerialArray;
#else
               Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
#endif
             }
        }
       else
        {
          printf ("ERROR: X_Array_ID does not exist in this expression! \n");
          APP_ABORT();
        }
   }


void floatArray_Function_0::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_0::Execute \n");
#endif

#if !defined(PPP)
     float*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     float*APP_RESTRICT_MACRO X_Data_Pointer       = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_0::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) X_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , X_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain),
                           (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain),
                           Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_X.Original_Referenced_Array );
#endif
   }

floatArray_Function_1::floatArray_Function_1 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_1 Input_Function_Pointer , 
                                         const floatArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

floatArray_Function_1::floatArray_Function_1 ( MDI_float_Prototype_1 Input_Function_Pointer , 
                                         const floatArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((floatArray &)X))
   { }

void floatArray_Function_1::Display_Expression ()
   {
     printf (" U %d :",Operand_X.Array_ID());
   }

#if 1
int floatArray_Function_1::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_1::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_1::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_1::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_1::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_1::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_1::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_1::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_1::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((floatArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this floatArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: floatArray_Function_1::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void floatArray_Function_1::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_1::Execute \n");
#endif

#if !defined(PPP)
     float*APP_RESTRICT_MACRO X_Data_Pointer = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_1::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( X_Data_Pointer , Mask_Array_Data ,
                 (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                 Mask_Descriptor );
#endif
#endif
   }

floatArray_Function_2::~floatArray_Function_2 ()
   {
     printf ("Inside of floatArray_Function_2 destructor! \n");
   }

floatArray_Function_2::floatArray_Function_2 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_2 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & Lhs , const floatArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_2 constructor! \n");
#endif
   }

floatArray_Function_2::floatArray_Function_2 ( MDI_float_Prototype_2 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & Lhs , const floatArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((floatArray &)Result)) , Operand_Lhs(&((floatArray &)Lhs)) , Operand_Rhs(&((floatArray &)Rhs))
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_2 constructor! \n");
#endif
   }

void floatArray_Function_2::Display_Expression ()
   {
     printf (" B (%d) %d %d  :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int floatArray_Function_2::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_2::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_2::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_2::Is_Rhs_A_Temporary \n");

     return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
            Operand_Rhs.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_2::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( ! (Operand_Lhs.Original_Referenced_Array->isTemporary() ||
             Operand_Rhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Both Operant_Lhs and Operand_Rhs are not temporaries! \n");
          APP_ABORT();
        }

     return  ( Operand_Lhs.Original_Referenced_Array->isTemporary() ) ?
             Operand_Lhs.Original_Referenced_Array->Array_ID() :
             Operand_Rhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_2::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_2::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void floatArray_Function_2::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_2::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Result to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Result is a view then setting this to a view is redundent!
          if ( !((floatArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this floatArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          if ( Operand_Rhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
             {
               if (Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
                  {
                    printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n");
                    APP_ABORT();
                  }
     
            // Now assign array descriptor from Result to the Temporary
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;
     
            // Technically all we want is a view -- if the Result is a view then setting this to a view is redundent!
               if ( !((floatArray *) (X_Array))->isView() )
                  {
                 // Now mark as a view so that the this floatArray will be deleted and the array data preserved!
                    Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
                 // Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
                    Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
                  }
             }
            else
             {
               printf ("ERROR: floatArray_Function_2::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
               APP_ABORT();
             }
        }
#endif
   }

void floatArray_Function_2::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_2::Execute \n");
#endif

#if !defined(PPP)
     float*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     float*APP_RESTRICT_MACRO  Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     float*APP_RESTRICT_MACRO  Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_2::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_2::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          if (APP_DEBUG > 0)
               printf ("Allocating memory for array data! \n");
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }
       else
        {
          if (APP_DEBUG > 0)
               printf ("Memory already allocated! \n");
        }

     if (APP_DEBUG > 0)
          Operand_Result.Original_Referenced_Array->display ("Operand_Result.Original_Referenced_Array");

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , Lhs_Data_Pointer , Rhs_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif

     if (APP_DEBUG > 0)
          Operand_Result.Original_Referenced_Array->display ("Operand_Result.Original_Referenced_Array");

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );

  // printf ("NOTE: Possible small and infrequent memory leak in floatArray_Function_2::Execute () \n");
  /* This should not be done in the case where the LHS was reused! 
  // When the this function has been called because a temporary was formed then we 
  // don't want to just delete the RHS? But if the Rhs was not used and it is
  // a temporary then it should be deleted?  Look at the use in abstract_op.C.m4!
  // if this has to be deleted then check for pointer equality between Result and Rhs
  // to test for reuse of the Rhs.  It might actually be fine to just uncomment the
  // following delete since it might have not been a problem as I first thought
  // when I was debugging this section!
  */
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data != Rhs_Data_Pointer)
     if (*Result_Data_Pointer != Rhs_Data_Pointer)
          Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }

floatArray_Function_3::floatArray_Function_3 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_3 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Rhs(Rhs)
   { 
     Execute_MDI_Function = TRUE;
   }

floatArray_Function_3::floatArray_Function_3 ( MDI_float_Prototype_3 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((floatArray &)Result)) , Operand_Rhs(&((floatArray &)Rhs))
   { 
     Execute_MDI_Function = TRUE;
   }

void floatArray_Function_3::Display_Expression ()
   {
     printf (" B|E %d  %d :",Operand_Result.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int floatArray_Function_3::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_3::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_3::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_3::Is_Rhs_A_Temporary \n");

     return Operand_Rhs.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_3::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Rhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Rhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Rhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_3::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_3::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void floatArray_Function_3::Skip_MDI_Function ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_3::Skip_MDI_Function! \n");

     Execute_MDI_Function = FALSE;
   }

void floatArray_Function_3::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_3::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Rhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Rhs.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((floatArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this floatArray will be deleted and the array data preserved!

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Setup the view of the Lhs since Lhs was not a view! \n");
#endif
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
            else
             {
            // printf ("The Lhs was a view so the array descriptor need not be modified! \n");
             }
        }
       else
        {
          printf ("ERROR: floatArray_Function_3::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void floatArray_Function_3::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_3::Execute \n");
#endif

#if !defined(PPP)
     float*APP_RESTRICT_MACRO Result_Data_Pointer = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
     float*APP_RESTRICT_MACRO Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Result_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_3::Execute Result_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_3::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Rhs_Data_Pointer;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     if (Execute_MDI_Function)
        {
          (*Function_Pointer) ( Result_Data_Pointer , Rhs_Data_Pointer , Mask_Array_Data ,
                                (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                                (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                                Mask_Descriptor );
        }
#endif

     Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }


floatArray_Function_4::floatArray_Function_4 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_4 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & Lhs , float Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

floatArray_Function_4::floatArray_Function_4 ( MDI_float_Prototype_4 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & Lhs , float Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((floatArray &)Result)) , Operand_Lhs(&((floatArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void floatArray_Function_4::Display_Expression ()
   {
     printf (" B (%d)  %d  Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int floatArray_Function_4::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_4::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_4::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_4::Is_Rhs_A_Temporary \n");

     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_4::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_4::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_4::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void floatArray_Function_4::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_4::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((floatArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this floatArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: floatArray_Function_4::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void floatArray_Function_4::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_4::Execute \n");
#endif

#if !defined(PPP)
     float*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     float*APP_RESTRICT_MACRO  Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_4::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
                           Operand_Scalar , 
#else
                           &Operand_Scalar , 
#endif
                           Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }


floatArray_Function_5::floatArray_Function_5 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_5 Input_Function_Pointer , 
                                         const floatArray & Lhs , float Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

floatArray_Function_5::floatArray_Function_5 ( MDI_float_Prototype_5 Input_Function_Pointer , 
                                         const floatArray & Lhs , float Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Lhs(&((floatArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void floatArray_Function_5::Display_Expression ()
   {
     printf (" B|E %d  Scalar :",Operand_Lhs.Array_ID());
   }

#if 1
int floatArray_Function_5::Get_Lhs_Array_ID ()
   {
     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_5::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_5::Is_Rhs_A_Temporary ()
   {
  // Since the Result and Lhs have the same Array_Id there is an anti-dependence here
  // so we could return false!

     printf ("Inside of  floatArray_Function_5::Is_Rhs_A_Temporary -- This function should not be called! \n");
     APP_ABORT();

     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_5::Get_Rhs_Temporary_Operand_ID () const
   {
     printf ("Inside of  floatArray_Function_5::Get_Rhs_Temporary_Operand_ID -- This function should not be called! \n");
     APP_ABORT();

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_5::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Lhs.Original_Referenced_Array;
   }

void floatArray_Function_5::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void floatArray_Function_5::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_5::Replace_Temporary_Result_With_Lhs_Array X_Array = %p X_Array_ID = %d \n",X_Array,X_Array_ID);
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: floatArray_Function_5::Replace_Temporary_Result_With_Lhs_Array -- Should not be called! \n");
     APP_ABORT();
   }


void floatArray_Function_5::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_5::Execute \n");
#endif

#if !defined(PPP)
     float*APP_RESTRICT_MACRO This_Array_Data = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (This_Array_Data == NULL)
        {
          printf ("ERROR: in floatArray_Function_5::Execute This_Array_Data == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) This_Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( This_Array_Data , 
#ifndef FORTRAN_MDI
                           Operand_Scalar , 
#else
                           &Operand_Scalar , 
#endif
                           Mask_Array_Data ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif
#endif
   }

floatArray_Function_6::floatArray_Function_6 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_6 Input_Function_Pointer , 
                                         const floatArray & Result , const intArray & Lhs , const floatArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { }

floatArray_Function_6::floatArray_Function_6 ( MDI_float_Prototype_6 Input_Function_Pointer , 
                                         const floatArray & Result , const intArray & Lhs , const floatArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((floatArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Rhs(&((floatArray &)Rhs))
   { }

void floatArray_Function_6::Display_Expression ()
   {
  // printf ("Inside of floatArray_Function_6::Display_Expression \n");
  // printf ("Calling the array ID now! \n");
     printf (" M (%d) %d %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int floatArray_Function_6::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_6::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_6::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a floatArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
  //        Operand_Rhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int floatArray_Function_6::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_6::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in floatArray_Function_6::Get_Result_Array -- Can't use floatArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_6::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void floatArray_Function_6::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_6::Replace_Temporary_Result_With_Lhs_Array X_Array = %p X_Array_ID = %d \n",X_Array,X_Array_ID);
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of floatArray_Function_6::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void floatArray_Function_6::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_6::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     float*APP_RESTRICT_MACRO  Rhs_Data_Pointer = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     float*APP_RESTRICT_MACRO  Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_6::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_6::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_6::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( Data_Pointer , Lhs_Data_Pointer , 
                           Rhs_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor);
#endif
#endif
   }

floatArray_Function_7::floatArray_Function_7 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_7 Input_Function_Pointer , 
                                         const floatArray & Result , const intArray & Lhs , float Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

floatArray_Function_7::floatArray_Function_7 ( MDI_float_Prototype_7 Input_Function_Pointer , 
                                         const floatArray & Result , const intArray & Lhs , float Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((floatArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void floatArray_Function_7::Display_Expression ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_7::Display_Expression \n");

     printf (" M (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int floatArray_Function_7::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_7::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_7::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a floatArray!
     return FALSE;
   }

int floatArray_Function_7::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_7::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in floatArray_Function_7::Get_Result_Array -- Can't use floatArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_7::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void floatArray_Function_7::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_7::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of floatArray_Function_7::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void floatArray_Function_7::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_7::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     float*APP_RESTRICT_MACRO  Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_7::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_7::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
                    Operand_Scalar , 
#else
                    &Operand_Scalar , 
#endif
                    Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif
#endif
   }

#ifndef INTARRAY
floatArray_Function_8::floatArray_Function_8 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_8 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & Lhs , int Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

floatArray_Function_8::floatArray_Function_8 ( MDI_float_Prototype_8 Input_Function_Pointer , 
                                         const floatArray & Result , const floatArray & Lhs , int Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((floatArray &)Result)) , Operand_Lhs(&((floatArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void floatArray_Function_8::Display_Expression ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_8::Display_Expression \n");

     printf (" M (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int floatArray_Function_8::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_8::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_8::Is_Rhs_A_Temporary ()
   {
     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_8::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_8::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Lhs.Original_Referenced_Array;
   }

void floatArray_Function_8::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void floatArray_Function_8::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_8::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((floatArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this floatArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: floatArray_Function_8::Get_Result_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void floatArray_Function_8::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_8::Execute \n");
#endif

#if !defined(PPP)
     float*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     float*APP_RESTRICT_MACRO Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_8::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_8::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
               Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
               Operand_Scalar , 
#else
               &Operand_Scalar , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }
#endif

floatArray_Function_9::floatArray_Function_9 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_9 Input_Function_Pointer , 
                                         const intArray & Result , const floatArray & Lhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs)
   { }

floatArray_Function_9::floatArray_Function_9 ( MDI_float_Prototype_9 Input_Function_Pointer , 
                                         const intArray & Result , const floatArray & Lhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((floatArray &)Lhs))
   { }

void floatArray_Function_9::Display_Expression ()
   {
     printf (" R %d  %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int floatArray_Function_9::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_9::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_9::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a floatArray!
     return FALSE;
   }

int floatArray_Function_9::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_9::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in floatArray_Function_9::Get_Result_Array -- Can't use floatArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_9::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void floatArray_Function_9::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_9::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of floatArray_Function_9::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void floatArray_Function_9::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_9::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     float*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_9::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */ 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    *Result_Data_Pointer , Lhs_Data_Pointer , Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }

#if 0
floatArray_Function_10::floatArray_Function_10 ( int Input_Operation_Code , 
                                           MDI_float_Prototype_10 Input_Function_Pointer )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer)
   { }

void floatArray_Function_10::Display_Expression ()
   {
     printf ("ERROR: Inside of floatArray_Function_10::Display_Expression \n");
     APP_ABORT();
   }

void floatArray_Function_10::Execute ()
   {
     printf ("ERROR: This object is not used! \n");
     APP_ABORT();
   }
#endif

floatArray_Function_11::floatArray_Function_11 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_11 Input_Function_Pointer , 
                                         const intArray & Result , const floatArray & Lhs , const floatArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { }

floatArray_Function_11::floatArray_Function_11 ( MDI_float_Prototype_11 Input_Function_Pointer , 
                                           const intArray & Result , const floatArray & Lhs , const floatArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((floatArray &)Lhs)) , Operand_Rhs(&((floatArray &)Rhs))
   { }

void floatArray_Function_11::Display_Expression ()
   {
     printf (" R (%d) %d %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int floatArray_Function_11::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_11::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_11::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a floatArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
  //        Operand_Rhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int floatArray_Function_11::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_11::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in floatArray_Function_11::Get_Result_Array -- Can't use floatArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_11::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void floatArray_Function_11::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_11::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of floatArray_Function_11::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void floatArray_Function_11::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_11::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     float*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     float*APP_RESTRICT_MACRO   Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_11::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_11::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */ 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    *Result_Data_Pointer , Lhs_Data_Pointer ,
                    Rhs_Data_Pointer , Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
     Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }

floatArray_Function_12::floatArray_Function_12 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_12 Input_Function_Pointer , 
                                         const intArray & Result , const floatArray & Lhs , float Scalar )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

floatArray_Function_12::floatArray_Function_12 ( MDI_float_Prototype_12 Input_Function_Pointer , 
                                           const intArray & Result , const floatArray & Lhs , float Scalar )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((floatArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void floatArray_Function_12::Display_Expression ()
   {
     printf (" R (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int floatArray_Function_12::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_12::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_12::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a floatArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int floatArray_Function_12::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_12::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in floatArray_Function_12::Get_Result_Array -- Can't use floatArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void floatArray_Function_12::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void floatArray_Function_12::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_12::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of floatArray_Function_12::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void floatArray_Function_12::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_12::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     float*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_12::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
               *Result_Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
               Operand_Scalar , 
#else
               &Operand_Scalar , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }

#if 0
floatArray_Function_13::floatArray_Function_13 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_13 Input_Function_Pointer ) 
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer)
   { }

void floatArray_Function_13::Display_Expression ()
   {
     printf ("Inside of floatArray_Function_13::Display_Expression \n");
     APP_ABORT();
   }

void floatArray_Function_13::Execute ()
   {
     printf ("ERROR: This object is not used! \n");
     APP_ABORT();
   }
#endif

#if 1
// This type is not presently used (intArray::indexMap)
floatArray_Function_14::floatArray_Function_14 ( int Input_Operation_Code , 
                                         MDI_float_Prototype_14 Input_Function_Pointer , 
                                         const floatArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

floatArray_Function_14::floatArray_Function_14 ( MDI_float_Prototype_14 Input_Function_Pointer , 
                                         const floatArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((floatArray &)X))
   { }

void floatArray_Function_14::Display_Expression ()
   {
     printf (" U %d :",Operand_X.Array_ID());
   }

#if 1
int floatArray_Function_14::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_14::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_14::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_14::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_14::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_14::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_14::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_14::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_1::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((floatArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((floatArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this floatArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) (X_Array);
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((floatArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: floatArray_Function_1::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void floatArray_Function_14::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_14::Execute \n");
#endif

#if !defined(PPP)
     float*APP_RESTRICT_MACRO X_Data_Pointer = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in floatArray_Function_14::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( X_Data_Pointer , Mask_Array_Data ,
                 (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                 Mask_Descriptor );
#endif
#endif
   }
#endif

floatArray_Function_15::floatArray_Function_15 ( int Input_Operation_Code , 
                                           MDI_float_Prototype_15 Input_Function_Pointer , 
                                           const floatArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

floatArray_Function_15::floatArray_Function_15 ( MDI_float_Prototype_15 Input_Function_Pointer , 
                                         const floatArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((floatArray &)X))
   { }

void floatArray_Function_15::Display_Expression ()
   {
     printf (" indexMap %d :",Operand_X.Array_ID());
   }

#if 1
int floatArray_Function_15::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_15::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_15::Is_Rhs_A_Temporary ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_15::Is_Rhs_A_Temporary \n");
#endif

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_15::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_15::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_15::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_15::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_15::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // This makes no sense in the case of display and view member functions! (I think)

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: floatArray_Function_15::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not make sense in this expression! \n",X_Array_ID);
     APP_ABORT();
   }


void floatArray_Function_15::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_15::Execute \n");
#endif

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          printf ("Note: Where_Statement_Mask ignored for display and view functions! \n");
        }

#if !defined(PPP)
#if EXECUTE_MDI_FUNCTIONS
     printf ("Commented out execution of defered operator 15 \n");
     APP_ABORT();
  // (Operand_X.Original_Referenced_Array->*Function_Pointer) ();
#endif
#endif
   }

#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
floatArray_Function_16::floatArray_Function_16 ( int Input_Operation_Code , 
                                           MDI_float_Prototype_16 Input_Function_Pointer , 
                                           const floatArray & X , const char *Input_Label )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X) ,
     Label (Input_Label)
   { 
  // It might be that this should be a deep copy instead of a shallow copy! (Later)
  // Label = Input_Label;
   }

floatArray_Function_16::floatArray_Function_16 ( MDI_float_Prototype_16 Input_Function_Pointer , 
                                         const floatArray & X , const char *Input_Label )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((floatArray &)X)) ,
     Label (Input_Label)
   {
  // It might be that this should be a deep copy instead of a shallow copy! (Later)
  // Label = Input_Label;
   }

void floatArray_Function_16::Display_Expression ()
   {
     printf (" D/V %d :",Operand_X.Array_ID());
   }

#if 1
int floatArray_Function_16::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void floatArray_Function_16::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool floatArray_Function_16::Is_Rhs_A_Temporary ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Function_16::Is_Rhs_A_Temporary \n");
#endif

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int floatArray_Function_16::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * floatArray_Function_16::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_16::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void floatArray_Function_16::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_16::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // This makes no sense in the case of display and view member functions! (I think)

     printf ("ERROR: floatArray_Function_16::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not make sense in this expression! \n",X_Array_ID);
     APP_ABORT();
   }


void floatArray_Function_16::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Function_16::Execute \n");
#endif

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          printf ("Note: Where_Statement_Mask ignored for display and view functions! \n");
        }

#if EXECUTE_MDI_FUNCTIONS
     printf ("Commented out execution of defered operator 16 \n");
     APP_ABORT();
  // (Operand_X.Original_Referenced_Array->*Function_Pointer) (Label);
#endif
   }
#endif // for COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS

void floatArray_Function_Steal_Data::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_Steal_Data::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_0::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_0::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_1::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_1::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_2::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_2::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_3::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_3::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_4::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_4::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_5::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_5::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_6::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_6::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_7::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_7::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }


#ifndef INTARRAY
void floatArray_Function_8::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_8::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

#endif

void floatArray_Function_9::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_9::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_11::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_11::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_12::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_12::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_14::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_14::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

void floatArray_Function_15::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_15::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }


#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
void floatArray_Function_16::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* floatArray_Function_16::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new floatArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_float_Prototype_Aggregate) Function_Pointer, 
                     (floatArray*) Result, 
                     (floatArray**) Operand_List );
   }

#endif


#define INTARRAY 
intArray_Function_Steal_Data::intArray_Function_Steal_Data ( int Input_Operation_Code , 
                                         const intArray & Result , const intArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Operand_Result(Result) , Operand_X(X)
   { 
     Execute_MDI_Function = TRUE;
   }

// This is a faster constructor for when we do not use defered evaluation!
intArray_Function_Steal_Data::intArray_Function_Steal_Data ( const intArray & Result , const intArray & X )
   : Operand_Result(&((intArray &)Result)) , Operand_X(&((intArray &)X))
   { 
     Execute_MDI_Function = TRUE;
   }

void intArray_Function_Steal_Data::Display_Expression ()
   {
     printf (" S Lhs %d = %d :",Operand_Result.Array_ID(),Operand_X.Array_ID());
   }

#if 1
int intArray_Function_Steal_Data::Get_Lhs_Array_ID ()
   {
  // return Operand_Result.Array_ID();
  // The following is not as safe but it is faster!
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_Steal_Data::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
  // Rhs_Array_ID_Array [1] = Operand_X.Array_ID();
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_Steal_Data::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_Steal_Data::Is_Rhs_A_Temporary \n");

  // return FALSE;
     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_Steal_Data::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_Steal_Data::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_Steal_Data::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_Steal_Data::Skip_MDI_Function ()
   {
     Execute_MDI_Function = FALSE;
   }

void intArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // This function is use to optimize out the use of temporaries from the List of Execution Objects (LEO)!
  // We have to input a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
#if defined(PPP)
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray != NULL) 
#else
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL) 
#endif
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n");
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary 
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((intArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this intArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
#if defined(PPP)
               Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray = ((intArray *) (X_Array))->Array_Descriptor.SerialArray;
#else
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
#endif
             }
        }
       else
        {
          printf ("ERROR: in intArray_Function_Steal_Data::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID does not exist in this expression! \n");
          APP_ABORT();
        }
   }

void intArray_Function_Steal_Data::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_Steal_Data::Execute \n");
#endif

     if (Execute_MDI_Function)
        {
#if COMPILE_DEBUG_STATEMENTS
       // error checking
          if ( !( Operand_Result.Original_Referenced_Array->Binary_Conformable (*Operand_X.Original_Referenced_Array) && 
                  Operand_X.Original_Referenced_Array->isTemporary() ) )
             {
               printf ("ERROR: in intArray_Function_0::Execute -- !( Binary_Conformable (Operand_X) && Operand_X.isTemporary() ) \n");
               APP_ABORT();
             }
#endif

#if defined(PPP)
          intSerialArray** Lhs_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray);
          intSerialArray** X_Data_Pointer   = &(Operand_X.Original_Referenced_Array->Array_Descriptor.SerialArray);
#else
          int*APP_RESTRICT_MACRO* Lhs_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
          int*APP_RESTRICT_MACRO* X_Data_Pointer   = &(Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data);
#endif

#if COMPILE_DEBUG_STATEMENTS
       // error checking
          if (*Lhs_Data_Pointer == NULL)
             {
               printf ("ERROR: in intArray_Function_Steal_Data::Execute *Lhs_Data_Pointer == NULL! \n");
               APP_ABORT();
             }
 
       // error checking
          if (*X_Data_Pointer == NULL)
             {
               printf ("ERROR: in intArray_Function_Steal_Data::Execute *X_Data_Pointer == NULL! \n");
               APP_ABORT();
             }
 
          if (APP_DEBUG > 3)
               printf ("After Setup pointers to pointers! \n");
#endif

       // Make space for the new data by deleting the old data!
          Operand_Result.Original_Referenced_Array->Delete_Array_Data ();
 
       // Steal the pointer (to the original array) from the Rhs's data
          *Lhs_Data_Pointer = *X_Data_Pointer;
 
       // We can set the correct one by using the fact that
       // X_Data_Pointer is a pointer to a pointer to the array data!
          *X_Data_Pointer = NULL;
 
       // The X will be deleted but until then we can't really call any
       // of it's member functions since there is no data associated with it.
          Delete_If_Temporary_Or_View ( *Operand_X.Original_Referenced_Array );
 
       // Now we have to delete the "this" object if it is a View! But we are the this object
       // so we have to be very carefull how this is done!

       // In the case of execution of the defered evaluation objects we cannot
       // delete this Lhs since we want to do multiple executions of the 
       // defered execution list!  We have used it's data and that is enough!
       // Delete_Lhs_If_Temporary_Or_View ( *Operand_Result.Original_Referenced_Array );
        }
   }

intArray_Function_0::intArray_Function_0 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_0 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_X(X)
   { }

intArray_Function_0::intArray_Function_0 ( MDI_int_Prototype_0 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_X(&((intArray &)X))
   { }

void intArray_Function_0::Display_Expression ()
   {
     printf ("U Lhs %d = %d :",Operand_Result.Array_ID(),Operand_X.Array_ID());
   }

#if 1
int intArray_Function_0::Get_Lhs_Array_ID ()
   {
  // return Operand_Result.Array_ID();
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_0::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
  // Rhs_Array_ID_Array [1] = Operand_X.Array_ID();
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_0::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_0::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_0::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_0::Get_Result_Array () 
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_0::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_0::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the 
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_0::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

     if ( Operand_Result.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
#if defined(PPP)
          if (Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray != NULL) 
#else
          if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL) 
#endif
             {
               printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary 
          Operand_Result.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((intArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this intArray will be deleted and the array data preserved!
               Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Result.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
#if defined(PPP)
               Operand_Result.Original_Referenced_Array->Array_Descriptor.SerialArray = ((intArray *) (X_Array))->Array_Descriptor.SerialArray;
#else
               Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
#endif
             }
        }
       else
        {
          printf ("ERROR: X_Array_ID does not exist in this expression! \n");
          APP_ABORT();
        }
   }


void intArray_Function_0::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_0::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     int*APP_RESTRICT_MACRO X_Data_Pointer       = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_0::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) X_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , X_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain),
                           (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain),
                           Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_X.Original_Referenced_Array );
#endif
   }

intArray_Function_1::intArray_Function_1 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_1 Input_Function_Pointer , 
                                         const intArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

intArray_Function_1::intArray_Function_1 ( MDI_int_Prototype_1 Input_Function_Pointer , 
                                         const intArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((intArray &)X))
   { }

void intArray_Function_1::Display_Expression ()
   {
     printf (" U %d :",Operand_X.Array_ID());
   }

#if 1
int intArray_Function_1::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_1::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_1::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_1::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_1::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_1::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_1::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_1::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_1::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((intArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this intArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: intArray_Function_1::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void intArray_Function_1::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_1::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO X_Data_Pointer = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_1::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( X_Data_Pointer , Mask_Array_Data ,
                 (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                 Mask_Descriptor );
#endif
#endif
   }

intArray_Function_2::~intArray_Function_2 ()
   {
     printf ("Inside of intArray_Function_2 destructor! \n");
   }

intArray_Function_2::intArray_Function_2 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_2 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , const intArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_2 constructor! \n");
#endif
   }

intArray_Function_2::intArray_Function_2 ( MDI_int_Prototype_2 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , const intArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Rhs(&((intArray &)Rhs))
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_2 constructor! \n");
#endif
   }

void intArray_Function_2::Display_Expression ()
   {
     printf (" B (%d) %d %d  :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int intArray_Function_2::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_2::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_2::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_2::Is_Rhs_A_Temporary \n");

     return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
            Operand_Rhs.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_2::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( ! (Operand_Lhs.Original_Referenced_Array->isTemporary() ||
             Operand_Rhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Both Operant_Lhs and Operand_Rhs are not temporaries! \n");
          APP_ABORT();
        }

     return  ( Operand_Lhs.Original_Referenced_Array->isTemporary() ) ?
             Operand_Lhs.Original_Referenced_Array->Array_ID() :
             Operand_Rhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_2::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_2::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void intArray_Function_2::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_2::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Result to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Result is a view then setting this to a view is redundent!
          if ( !((intArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this intArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          if ( Operand_Rhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
             {
               if (Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
                  {
                    printf ("This temporary should have had a NULL pointer for its data since it was build by defered evaluation! \n");
                    APP_ABORT();
                  }
     
            // Now assign array descriptor from Result to the Temporary
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;
     
            // Technically all we want is a view -- if the Result is a view then setting this to a view is redundent!
               if ( !((intArray *) (X_Array))->isView() )
                  {
                 // Now mark as a view so that the this intArray will be deleted and the array data preserved!
                    Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
                 // Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
                    Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
                  }
             }
            else
             {
               printf ("ERROR: intArray_Function_2::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
               APP_ABORT();
             }
        }
#endif
   }

void intArray_Function_2::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_2::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     int*APP_RESTRICT_MACRO  Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int*APP_RESTRICT_MACRO  Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_2::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_2::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          if (APP_DEBUG > 0)
               printf ("Allocating memory for array data! \n");
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }
       else
        {
          if (APP_DEBUG > 0)
               printf ("Memory already allocated! \n");
        }

     if (APP_DEBUG > 0)
          Operand_Result.Original_Referenced_Array->display ("Operand_Result.Original_Referenced_Array");

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , Lhs_Data_Pointer , Rhs_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif

     if (APP_DEBUG > 0)
          Operand_Result.Original_Referenced_Array->display ("Operand_Result.Original_Referenced_Array");

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );

  // printf ("NOTE: Possible small and infrequent memory leak in intArray_Function_2::Execute () \n");
  /* This should not be done in the case where the LHS was reused! 
  // When the this function has been called because a temporary was formed then we 
  // don't want to just delete the RHS? But if the Rhs was not used and it is
  // a temporary then it should be deleted?  Look at the use in abstract_op.C.m4!
  // if this has to be deleted then check for pointer equality between Result and Rhs
  // to test for reuse of the Rhs.  It might actually be fine to just uncomment the
  // following delete since it might have not been a problem as I first thought
  // when I was debugging this section!
  */
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data != Rhs_Data_Pointer)
     if (*Result_Data_Pointer != Rhs_Data_Pointer)
          Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }

intArray_Function_3::intArray_Function_3 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_3 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Rhs(Rhs)
   { 
     Execute_MDI_Function = TRUE;
   }

intArray_Function_3::intArray_Function_3 ( MDI_int_Prototype_3 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Rhs(&((intArray &)Rhs))
   { 
     Execute_MDI_Function = TRUE;
   }

void intArray_Function_3::Display_Expression ()
   {
     printf (" B|E %d  %d :",Operand_Result.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int intArray_Function_3::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_3::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_3::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_3::Is_Rhs_A_Temporary \n");

     return Operand_Rhs.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_3::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Rhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Rhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Rhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_3::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_3::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void intArray_Function_3::Skip_MDI_Function ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_3::Skip_MDI_Function! \n");

     Execute_MDI_Function = FALSE;
   }

void intArray_Function_3::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_3::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Rhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Rhs.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((intArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this intArray will be deleted and the array data preserved!

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Setup the view of the Lhs since Lhs was not a view! \n");
#endif
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
               Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
            else
             {
            // printf ("The Lhs was a view so the array descriptor need not be modified! \n");
             }
        }
       else
        {
          printf ("ERROR: intArray_Function_3::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void intArray_Function_3::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_3::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO Result_Data_Pointer = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int*APP_RESTRICT_MACRO Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Result_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_3::Execute Result_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_3::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
  // int* Mask_Stride     = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Rhs_Data_Pointer;
     int* Mask_Stride     = Global_Zero_Array;
#endif
  // int* Mask_Base       = NULL;
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     if (Execute_MDI_Function)
        {
          (*Function_Pointer) ( Result_Data_Pointer , Rhs_Data_Pointer , Mask_Array_Data ,
                                (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                                (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                                Mask_Descriptor );
        }
#endif

     Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }


intArray_Function_4::intArray_Function_4 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_4 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , int Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

intArray_Function_4::intArray_Function_4 ( MDI_int_Prototype_4 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , int Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void intArray_Function_4::Display_Expression ()
   {
     printf (" B (%d)  %d  Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int intArray_Function_4::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_4::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_4::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_4::Is_Rhs_A_Temporary \n");

     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_4::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_4::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_4::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void intArray_Function_4::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_4::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((intArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this intArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: intArray_Function_4::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void intArray_Function_4::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_4::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     int*APP_RESTRICT_MACRO  Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_4::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( *Result_Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
                           Operand_Scalar , 
#else
                           &Operand_Scalar , 
#endif
                           Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }


intArray_Function_5::intArray_Function_5 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_5 Input_Function_Pointer , 
                                         const intArray & Lhs , int Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

intArray_Function_5::intArray_Function_5 ( MDI_int_Prototype_5 Input_Function_Pointer , 
                                         const intArray & Lhs , int Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Lhs(&((intArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void intArray_Function_5::Display_Expression ()
   {
     printf (" B|E %d  Scalar :",Operand_Lhs.Array_ID());
   }

#if 1
int intArray_Function_5::Get_Lhs_Array_ID ()
   {
     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_5::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_5::Is_Rhs_A_Temporary ()
   {
  // Since the Result and Lhs have the same Array_Id there is an anti-dependence here
  // so we could return false!

     printf ("Inside of  intArray_Function_5::Is_Rhs_A_Temporary -- This function should not be called! \n");
     APP_ABORT();

     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_5::Get_Rhs_Temporary_Operand_ID () const
   {
     printf ("Inside of  intArray_Function_5::Get_Rhs_Temporary_Operand_ID -- This function should not be called! \n");
     APP_ABORT();

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_5::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Lhs.Original_Referenced_Array;
   }

void intArray_Function_5::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void intArray_Function_5::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_5::Replace_Temporary_Result_With_Lhs_Array X_Array = %p X_Array_ID = %d \n",X_Array,X_Array_ID);
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: intArray_Function_5::Replace_Temporary_Result_With_Lhs_Array -- Should not be called! \n");
     APP_ABORT();
   }


void intArray_Function_5::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_5::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO This_Array_Data = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (This_Array_Data == NULL)
        {
          printf ("ERROR: in intArray_Function_5::Execute This_Array_Data == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) This_Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( This_Array_Data , 
#ifndef FORTRAN_MDI
                           Operand_Scalar , 
#else
                           &Operand_Scalar , 
#endif
                           Mask_Array_Data ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor );
#endif
#endif
   }

intArray_Function_6::intArray_Function_6 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_6 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , const intArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { }

intArray_Function_6::intArray_Function_6 ( MDI_int_Prototype_6 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , const intArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Rhs(&((intArray &)Rhs))
   { }

void intArray_Function_6::Display_Expression ()
   {
  // printf ("Inside of intArray_Function_6::Display_Expression \n");
  // printf ("Calling the array ID now! \n");
     printf (" M (%d) %d %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int intArray_Function_6::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_6::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_6::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a intArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
  //        Operand_Rhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int intArray_Function_6::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_6::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in intArray_Function_6::Get_Result_Array -- Can't use intArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_6::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void intArray_Function_6::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_6::Replace_Temporary_Result_With_Lhs_Array X_Array = %p X_Array_ID = %d \n",X_Array,X_Array_ID);
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of intArray_Function_6::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void intArray_Function_6::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_6::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int*APP_RESTRICT_MACRO  Rhs_Data_Pointer = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int*APP_RESTRICT_MACRO  Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_6::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_6::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_6::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( Data_Pointer , Lhs_Data_Pointer , 
                           Rhs_Data_Pointer , Mask_Array_Data ,
                           (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                           Mask_Descriptor);
#endif
#endif
   }

intArray_Function_7::intArray_Function_7 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_7 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , int Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

intArray_Function_7::intArray_Function_7 ( MDI_int_Prototype_7 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , int Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void intArray_Function_7::Display_Expression ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_7::Display_Expression \n");

     printf (" M (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int intArray_Function_7::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_7::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_7::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a intArray!
     return FALSE;
   }

int intArray_Function_7::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_7::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in intArray_Function_7::Get_Result_Array -- Can't use intArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_7::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void intArray_Function_7::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_7::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of intArray_Function_7::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void intArray_Function_7::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_7::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int*APP_RESTRICT_MACRO  Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_7::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_7::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
                    Operand_Scalar , 
#else
                    &Operand_Scalar , 
#endif
                    Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif
#endif
   }

#ifndef INTARRAY
intArray_Function_8::intArray_Function_8 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_8 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , int Scalar)
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

intArray_Function_8::intArray_Function_8 ( MDI_int_Prototype_8 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , int Scalar)
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void intArray_Function_8::Display_Expression ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_8::Display_Expression \n");

     printf (" M (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int intArray_Function_8::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_8::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_8::Is_Rhs_A_Temporary ()
   {
     return Operand_Lhs.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_8::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_8::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_Lhs.Original_Referenced_Array;
   }

void intArray_Function_8::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void intArray_Function_8::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_8::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_Lhs.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_Lhs.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((intArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this intArray will be deleted and the array data preserved!
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
               Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: intArray_Function_8::Get_Result_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void intArray_Function_8::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_8::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO Lhs_Data_Pointer = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int*APP_RESTRICT_MACRO Data_Pointer     = Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_8::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_8::Execute Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
               Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
               Operand_Scalar , 
#else
               &Operand_Scalar , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }
#endif

intArray_Function_9::intArray_Function_9 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_9 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs)
   { }

intArray_Function_9::intArray_Function_9 ( MDI_int_Prototype_9 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((intArray &)Lhs))
   { }

void intArray_Function_9::Display_Expression ()
   {
     printf (" R %d  %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int intArray_Function_9::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_9::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_9::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a intArray!
     return FALSE;
   }

int intArray_Function_9::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_9::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in intArray_Function_9::Get_Result_Array -- Can't use intArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_9::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void intArray_Function_9::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_9::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of intArray_Function_9::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void intArray_Function_9::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_9::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     int*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_9::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */ 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    *Result_Data_Pointer , Lhs_Data_Pointer , Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }

#if 0
intArray_Function_10::intArray_Function_10 ( int Input_Operation_Code , 
                                           MDI_int_Prototype_10 Input_Function_Pointer )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer)
   { }

void intArray_Function_10::Display_Expression ()
   {
     printf ("ERROR: Inside of intArray_Function_10::Display_Expression \n");
     APP_ABORT();
   }

void intArray_Function_10::Execute ()
   {
     printf ("ERROR: This object is not used! \n");
     APP_ABORT();
   }
#endif

intArray_Function_11::intArray_Function_11 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_11 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , const intArray & Rhs )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Rhs(Rhs)
   { }

intArray_Function_11::intArray_Function_11 ( MDI_int_Prototype_11 Input_Function_Pointer , 
                                           const intArray & Result , const intArray & Lhs , const intArray & Rhs )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Rhs(&((intArray &)Rhs))
   { }

void intArray_Function_11::Display_Expression ()
   {
     printf (" R (%d) %d %d :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID(),Operand_Rhs.Array_ID());
   }

#if 1
int intArray_Function_11::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_11::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = Operand_Rhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_11::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a intArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary() ||
  //        Operand_Rhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int intArray_Function_11::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_11::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in intArray_Function_11::Get_Result_Array -- Can't use intArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_11::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
     Operand_List[1] = (void *) Operand_Rhs.Original_Referenced_Array;
   }

void intArray_Function_11::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_11::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of intArray_Function_11::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void intArray_Function_11::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_11::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     int*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
     int*APP_RESTRICT_MACRO   Rhs_Data_Pointer    = Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Data;
 
#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_11::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
 
  // error checking
     if (Rhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_11::Execute Rhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */ 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
                    *Result_Data_Pointer , Lhs_Data_Pointer ,
                    Rhs_Data_Pointer , Mask_Array_Data ,
                    (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    (array_domain*) (&Operand_Rhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                    Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
     Delete_If_Temporary_Or_View ( *Operand_Rhs.Original_Referenced_Array );
#endif
   }

intArray_Function_12::intArray_Function_12 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_12 Input_Function_Pointer , 
                                         const intArray & Result , const intArray & Lhs , int Scalar )
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(Result) , Operand_Lhs(Lhs) , Operand_Scalar(Scalar)
   { }

intArray_Function_12::intArray_Function_12 ( MDI_int_Prototype_12 Input_Function_Pointer , 
                                           const intArray & Result , const intArray & Lhs , int Scalar )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_Result(&((intArray &)Result)) , Operand_Lhs(&((intArray &)Lhs)) , Operand_Scalar(Scalar)
   { }

void intArray_Function_12::Display_Expression ()
   {
     printf (" R (%d) %d Scalar :",Operand_Result.Array_ID(),Operand_Lhs.Array_ID());
   }

#if 1
int intArray_Function_12::Get_Lhs_Array_ID ()
   {
     return Operand_Result.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_12::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_Lhs.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_12::Is_Rhs_A_Temporary ()
   {
  // This returns FALSE because the Lhs is an intArray and can't be replaced by a intArray!
  // return Operand_Lhs.Original_Referenced_Array->isTemporary();
     return FALSE;
   }

int intArray_Function_12::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_Lhs.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_Lhs is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_Lhs.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_12::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     printf ("ERROR: This function should not be called! \n");
     printf ("ERROR: in intArray_Function_12::Get_Result_Array -- Can't use intArray Result for intArray Temporary! \n");
     APP_ABORT();

     return (void *) Operand_Result.Original_Referenced_Array;
   }

void intArray_Function_12::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_Lhs.Original_Referenced_Array;
   }

void intArray_Function_12::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_12::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: (Function can't be used for this type) -- Inside of intArray_Function_12::Replace_Temporary_Result_With_Lhs_Array \n");
     APP_ABORT();
   }

void intArray_Function_12::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_12::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO* Result_Data_Pointer = &(Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data);
     int*APP_RESTRICT_MACRO   Lhs_Data_Pointer    = Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (Lhs_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_12::Execute Lhs_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
  // if (Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Data == NULL)
     if (*Result_Data_Pointer == NULL)
        {
          Operand_Result.Original_Referenced_Array->Allocate_Array_Data ();
        }

  /* Notice that we assume that the mask might be a view!
  // For Machine dependent function implementations in FORTRAN,
  // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
  */
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Lhs_Data_Pointer;
#endif
     array_domain* Mask_Descriptor = NULL;
 
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }
 
#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) (
               *Result_Data_Pointer , Lhs_Data_Pointer , 
#ifndef FORTRAN_MDI
               Operand_Scalar , 
#else
               &Operand_Scalar , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Operand_Result.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Operand_Lhs.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif

     Delete_If_Temporary_Or_View ( *Operand_Lhs.Original_Referenced_Array );
#endif
   }

#if 0
intArray_Function_13::intArray_Function_13 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_13 Input_Function_Pointer ) 
   : Expression_Tree_Node_Type(Input_Operation_Code) , 
     Function_Pointer(Input_Function_Pointer)
   { }

void intArray_Function_13::Display_Expression ()
   {
     printf ("Inside of intArray_Function_13::Display_Expression \n");
     APP_ABORT();
   }

void intArray_Function_13::Execute ()
   {
     printf ("ERROR: This object is not used! \n");
     APP_ABORT();
   }
#endif

#if 1
// This type is not presently used (intArray::indexMap)
intArray_Function_14::intArray_Function_14 ( int Input_Operation_Code , 
                                         MDI_int_Prototype_14 Input_Function_Pointer , 
                                         const intArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

intArray_Function_14::intArray_Function_14 ( MDI_int_Prototype_14 Input_Function_Pointer , 
                                         const intArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((intArray &)X))
   { }

void intArray_Function_14::Display_Expression ()
   {
     printf (" U %d :",Operand_X.Array_ID());
   }

#if 1
int intArray_Function_14::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_14::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_14::Is_Rhs_A_Temporary ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_14::Is_Rhs_A_Temporary \n");

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_14::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_14::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_14::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_14::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_1::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

#if !defined(PPP)
     if ( Operand_X.Original_Referenced_Array->Array_ID() == X_Array_ID )
        {
          if (Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data != NULL)
             {
               printf ("This temporary should have had a NULL pointer for ist data since it was build by defered evaluation! \n"
);
               APP_ABORT();
             }

       // Now assign array descriptor from Lhs to the Temporary
          Operand_X.Original_Referenced_Array->Array_Descriptor = ((intArray *) (X_Array))->Array_Descriptor;

       // Technically all we want is a view -- if the Lhs is a view then setting this to a view is redundent!
          if ( !((intArray *) (X_Array))->isView() )
             {
            // Now mark as a view so that the this intArray will be deleted and the array data preserved!
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain.Is_A_View = TRUE;
            // Operand_X.Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) (X_Array);
               Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data = ((intArray *) (X_Array))->Array_Descriptor.Array_Data;
             }
        }
       else
        {
          printf ("ERROR: intArray_Function_1::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not exist in this expression! \n",X_Array_ID);
          APP_ABORT();
        }
#endif
   }


void intArray_Function_14::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_14::Execute \n");
#endif

#if !defined(PPP)
     int*APP_RESTRICT_MACRO X_Data_Pointer = Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;

#if COMPILE_DEBUG_STATEMENTS
  // error checking
     if (X_Data_Pointer == NULL)
        {
          printf ("ERROR: in intArray_Function_14::Execute X_Data_Pointer == NULL! \n");
          APP_ABORT();
        }
#endif
 
#ifndef FORTRAN_MDI
     int* Mask_Array_Data = NULL;
#else
  // We can't pass a NULL pointer to the CM-5 FORTRAN!
     int* Mask_Array_Data = (int*) Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Data;
#endif
     array_domain* Mask_Descriptor = NULL;

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
#if defined(APP)
          Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(SERIAL_APP)
          Mask_Array_Data = Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
          Mask_Descriptor = (array_domain*) (&Where_Statement_Support::Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
#elif defined(PPP)
          Mask_Array_Data = NULL;
          Mask_Descriptor = NULL;
#else
#error Niether PPP || SERIAL_APP || or APP defined
#endif
        }

#if EXECUTE_MDI_FUNCTIONS
     (*Function_Pointer) ( X_Data_Pointer , Mask_Array_Data ,
                 (array_domain*) (&Operand_X.Original_Referenced_Array->Array_Descriptor.Array_Domain) ,
                 Mask_Descriptor );
#endif
#endif
   }
#endif

intArray_Function_15::intArray_Function_15 ( int Input_Operation_Code , 
                                           MDI_int_Prototype_15 Input_Function_Pointer , 
                                           const intArray & X )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X)
   { }

intArray_Function_15::intArray_Function_15 ( MDI_int_Prototype_15 Input_Function_Pointer , 
                                         const intArray & X )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((intArray &)X))
   { }

void intArray_Function_15::Display_Expression ()
   {
     printf (" indexMap %d :",Operand_X.Array_ID());
   }

#if 1
int intArray_Function_15::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_15::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_15::Is_Rhs_A_Temporary ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_15::Is_Rhs_A_Temporary \n");
#endif

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_15::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_15::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_15::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_15::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_15::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // This makes no sense in the case of display and view member functions! (I think)

  // Use these to avoid compiler warning
     X_Array = NULL;
     X_Array_ID = 0;
     printf ("ERROR: intArray_Function_15::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not make sense in this expression! \n",X_Array_ID);
     APP_ABORT();
   }


void intArray_Function_15::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_15::Execute \n");
#endif

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          printf ("Note: Where_Statement_Mask ignored for display and view functions! \n");
        }

#if !defined(PPP)
#if EXECUTE_MDI_FUNCTIONS
     printf ("Commented out execution of defered operator 15 \n");
     APP_ABORT();
  // (Operand_X.Original_Referenced_Array->*Function_Pointer) ();
#endif
#endif
   }

#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
intArray_Function_16::intArray_Function_16 ( int Input_Operation_Code , 
                                           MDI_int_Prototype_16 Input_Function_Pointer , 
                                           const intArray & X , const char *Input_Label )
   : Expression_Tree_Node_Type(Input_Operation_Code) ,
     Function_Pointer(Input_Function_Pointer) ,
     Operand_X(X) ,
     Label (Input_Label)
   { 
  // It might be that this should be a deep copy instead of a shallow copy! (Later)
  // Label = Input_Label;
   }

intArray_Function_16::intArray_Function_16 ( MDI_int_Prototype_16 Input_Function_Pointer , 
                                         const intArray & X , const char *Input_Label )
   : Function_Pointer(Input_Function_Pointer) ,
     Operand_X(&((intArray &)X)) ,
     Label (Input_Label)
   {
  // It might be that this should be a deep copy instead of a shallow copy! (Later)
  // Label = Input_Label;
   }

void intArray_Function_16::Display_Expression ()
   {
     printf (" D/V %d :",Operand_X.Array_ID());
   }

#if 1
int intArray_Function_16::Get_Lhs_Array_ID ()
   {
     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void intArray_Function_16::Get_Rhs_Array_ID ( int *Rhs_Array_ID_Array )
   {
     Rhs_Array_ID_Array [0] = -1;
     Rhs_Array_ID_Array [1] = Operand_X.Original_Referenced_Array->Array_ID();
     Rhs_Array_ID_Array [2] = -1;
     Rhs_Array_ID_Array [3] = -1;
   }
#endif

bool intArray_Function_16::Is_Rhs_A_Temporary ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Function_16::Is_Rhs_A_Temporary \n");
#endif

     return Operand_X.Original_Referenced_Array->isTemporary();
   }

int intArray_Function_16::Get_Rhs_Temporary_Operand_ID () const
   {
     if ( !(Operand_X.Original_Referenced_Array->isTemporary()) )
        {
          printf ("ERROR: Operant_X is not a temporary! \n");
          APP_ABORT();
        }

     return Operand_X.Original_Referenced_Array->Array_ID();
   }

void * intArray_Function_16::Get_Result_Array ()
   {
  // We have to return a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

     return (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_16::Get_Rhs_Array ( void** Operand_List )
   {
     Operand_List[0] = (void *) Operand_X.Original_Referenced_Array;
   }

void intArray_Function_16::Replace_Temporary_Result_With_Lhs_Array ( void * X_Array , int X_Array_ID )
   {
  // We have to input a pointer since this maps to a virtual function and the
  // type is different depending on the derived class that is used!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_16::Replace_Temporary_Result_With_Lhs_Array \n");
#endif

  // This makes no sense in the case of display and view member functions! (I think)

     printf ("ERROR: intArray_Function_16::Replace_Temporary_Result_With_Lhs_Array -- X_Array_ID  = %d does not make sense in this expression! \n",X_Array_ID);
     APP_ABORT();
   }


void intArray_Function_16::Execute ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Function_16::Execute \n");
#endif

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          printf ("Note: Where_Statement_Mask ignored for display and view functions! \n");
        }

#if EXECUTE_MDI_FUNCTIONS
     printf ("Commented out execution of defered operator 16 \n");
     APP_ABORT();
  // (Operand_X.Original_Referenced_Array->*Function_Pointer) (Label);
#endif
   }
#endif // for COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS

void intArray_Function_Steal_Data::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_Steal_Data::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_0::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_0::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_1::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_1::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_2::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_2::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_3::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_3::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_4::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_4::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_5::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_5::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_6::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_6::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_7::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_7::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }


#ifndef INTARRAY
void intArray_Function_8::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_8::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

#endif

void intArray_Function_9::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_9::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_11::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_11::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_12::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_12::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_14::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_14::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

void intArray_Function_15::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_15::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }


#if COMPILE_DEFERRED_DISPLAY_AND_VIEW_FUNCTIONS
void intArray_Function_16::New_Function_Loop ()
   {
  // Initialize the free list of pointers!
     for (int i=0; i < CLASS_ALLOCATION_POOL_SIZE-1; i++)
        {
          Current_Link [i].freepointer = &(Current_Link[i+1]);
        }
   }

Expression_Tree_Node_Type* intArray_Function_16::Build_Aggregate_Operator (
             int Number_Of_Operators , void* Function_Pointer , void* Result , void** Operand_List )
   {
     return new intArray_Aggregate_Operator ( 
                     Number_Of_Operators,
                     (MDI_int_Prototype_Aggregate) Function_Pointer, 
                     (intArray*) Result, 
                     (intArray**) Operand_List );
   }

#endif

#undef INTARRAY 
 








