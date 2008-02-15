// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma implementation "lazy_operand.h"
#endif

#include "A++.h"

#define PRINTOUT_TASK_LIST       FALSE

// ****************************************************************************
// ****************************************************************************
//               Operand_Storage class member functions
// ****************************************************************************
// ****************************************************************************

inline Operand_Storage::Operand_Storage ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of Operand_Storage constructor! \n");

  // if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
  //    {
  //      printf ("ERROR: This constructor cannot be used with defered evaluation! \n");
  //      APP_ABORT();
  //    }
#endif
   } 

#if !defined(PPP)
Operand_Storage::~Operand_Storage ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of Operand_Storage destructor! \n");
#endif
   } 

Operand_Storage::Operand_Storage ( const Operand_Storage & X )
   {
  // This is required for the GNU compiler but it is not used!
     printf ("ERROR: Inside of Operand_Storage copy constructor! \n");
     APP_ABORT();
   } 

void Operand_Storage::Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer )
   {
     X_Pointer = NULL; // Avoids compiler warning!
     printf ("ERROR: Base class function should not be called! \n");
     APP_ABORT();
   }

// This is now a virtual function!
// int Operand_Storage::Array_ID () const
//    {
//      printf ("ERROR: Array_ID should be a pure virtual functions so that this is not called! \n");
//      APP_ABORT();
//      return -1;
//    }

void Operand_Storage::Process_List_Of_Operands ()
   {
     if (APP_DEBUG > 3)
          printf ("Inside of Operand_Storage::Process_List_Of_Operands \n");

  // error checking!
     if (Tail_Of_List == NULL)
        { 
          printf ("Tail_Of_List == NULL \n");
          APP_ABORT();
        }

     for (Operand_Storage* p = Tail_Of_List; p != NULL; p = p->Next_Array_Defered_Operand)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Inside of LOOP! \n");
#endif
          p->Make_Local_Variable_Persistant_And_Update_Storage ( Tail_Of_List->Get_Original_Array_Pointer() );
        }
   }
#endif

// Now present the macros to provide defered evaluation support!







//#if !defined(GNU)
//inline 
//#endif
INLINE
Operand_Storage::Operand_Storage ( const doubleArray & X )
   {
  // Execution is from tail --> head!  We add new elements to the head!
  // This matches the chronological order of evaluation

  // This creates a list for each variable recording its use and 
  // provides a linked list that can be traversed (backward) to 
  // build a persistant array variable (for the first use recorded
  // in the list) and then change the pointers to point to that
  // persistant variable in the subsequent recorded uses.
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of Operand_Storage::Operand_Storage ( const doubleArray & X ) \n");
#endif

  // If we are not using defered evaluation then we don't have to keep track of all the 
  // linked list stuff.  This is the case in abstract_op.C where the abstract operators
  // can just creat execution objects and execute them rather than duplicating all
  // the code in each abstract operator.  This make the abstract operators simple enough
  // to inline in the operators defined in operator.C!

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // X.display("X in Operand_Storage::Operand_Storage!");

       // This is the simple way to test to see if this variable was built under defered evaluation!
       // And this checks to see if this is the first use of the variable!
       // if (X.Array_Storage == NULL && !X.Array_Descriptor.Is_A_View)

       // Views now have local scope and so they must be processed just like local variables!
          if (X.Array_Storage == NULL)
             {
            // This is how we restrict the linked lists of references to local
            // variables only. This makes it faster to do the defered evaluation.
               if ( (X.Is_Built_By_Defered_Evaluation()) &&
                    (!X.isTemporary()) )
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("X.Array_Storage == NULL and Built_By_Defered_Evaluation (fixing that up!) \n");
#endif
                    ((doubleArray &) X).Array_Storage = this;
                  }

               Tail_Of_List               = this;
               Next_Array_Defered_Operand = NULL;
               Head_Of_List               = this;
             } 
            else
             {  
#if COMPILE_DEBUG_STATEMENTS
            // error checking
               if (X.Is_Built_By_Defered_Evaluation() == FALSE)
                  {
                    printf ("ERROR: X.Array_Descriptor.Is_Bult_By_Defered_Evaluation == FALSE \n");
                    APP_ABORT();
                  }
#endif

  // We have to allow the processing of views since they are now have local scope!
#if 0
               if (X.Array_Descriptor.Is_A_View)
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("X is a view! \n");

                 // error checking
                 // if (X.Array_Storage != NULL)  // Then X is a view!
                 //    {
                 //      printf ("ERROR: X.Array_Storage != NULL \n");
                 //      APP_ABORT();
                 //    }
#endif

                 // error checking
                    if (X.Array_Descriptor.Original_Array->Array_Storage == NULL)
                       {
                      // There is no list to add this object to! So do not add it to any list!
                      // printf ("Sorry, not supported yet, the object of which this is a view of was not built under defered evaluation! \n");
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 3)
                              printf ("NOTE: X.Array_Descriptor.Original_Array->Array_Storage == NULL -- There is no list to add this object to! \n");
                      // APP_ABORT();
#endif
                       }
                      else
                       {
#if COMPILE_DEBUG_STATEMENTS
                      // error checking
                         if (X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List == NULL)
                            {
                              printf ("ERROR: X.Array_Descriptor.Original_Array->Array_Storage == NULL \n");
                              APP_ABORT();
                            }

                         if (APP_DEBUG > 3)
                              printf ("NOTE: (In Operand_Storage constructor) Setting up the NEXT LINK! ... \n");
#endif

                         X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List->Next_Array_Defered_Operand = this;
                         X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List = this;
                         Tail_Of_List               = NULL;
                         Next_Array_Defered_Operand = NULL;
                         Head_Of_List               = NULL; 
                       }
                  }
                 else
                  {
#endif
#if COMPILE_DEBUG_STATEMENTS
                    if (X.Array_Storage->Head_Of_List == NULL)
                       {
                         printf ("ERROR: X.Array_Storage->Head_Of_List == NULL \n");
                         APP_ABORT();
                       }

                    if (APP_DEBUG > 3)
                         printf ("NOTE: (In Operand_Storage constructor) Setting up the NEXT LINK! ... \n");
#endif

                    X.Array_Storage->Head_Of_List->Next_Array_Defered_Operand = this;
                    X.Array_Storage->Head_Of_List = this;
                    Tail_Of_List               = NULL;
                    Next_Array_Defered_Operand = NULL;
                    Head_Of_List               = NULL; 
               // }
             }
        }  // END if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
   } 
 
doubleArray_Operand_Storage::~doubleArray_Operand_Storage ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray_Operand_Storage::~doubleArray_Operand_Storage \n");
   }
 
doubleArray_Operand_Storage::doubleArray_Operand_Storage ()
   {
     printf ("ERROR: This constructor should not be called (inside of doubleArray_Operand_Storage)! \n");
     APP_ABORT();
   }
 
doubleArray_Operand_Storage::doubleArray_Operand_Storage ( const doubleArray & X )
   : // Original_Referenced_Array(&((doubleArray &) X)) , 
     Operand_Storage(X)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Operand_Storage::doubleArray_Operand_Storage constructor(X)! \n");
#endif

     Original_Referenced_Array = &((doubleArray &) X);
   } 
 
doubleArray_Operand_Storage::doubleArray_Operand_Storage ( const doubleArray_Operand_Storage & X )
   : Operand_Storage(X)
   {
     printf ("ERROR: Inside of copy constructor doubleArray_Operand_Storage::doubleArray_Operand_Storage constructor(const & X)! \n");
     APP_ABORT();
   } 
 
doubleArray_Operand_Storage::doubleArray_Operand_Storage ( doubleArray * X_Pointer )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Operand_Storage::doubleArray_Operand_Storage constructor(X_Pointer)! \n");
#endif

     Original_Referenced_Array = X_Pointer;
   } 
 
// This is a virtual function used to access the type specific doubleArray reference!
void * doubleArray_Operand_Storage::Get_Original_Array_Pointer()
   {
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in doubleArray_Operand_Storage::Get_Original_Array_Pointer()! \n");
          APP_ABORT();
        }

     return (void*) Original_Referenced_Array;
   }

#if 0
int doubleArray_Operand_Storage::Is_A_Temporary () const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in Is_A_Temporary() \n");
          APP_ABORT();
        }
  // if (Original_Referenced_Array->Array_Descriptor == NULL)
  //    {
  //      printf ("ERROR: Original_Referenced_Array == NULL in Is_A_Temporary() \n");
  //      APP_ABORT();
  //    }
#endif
     return Original_Referenced_Array->isTemporary();
   }
#endif

int doubleArray_Operand_Storage::Array_ID () const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in Array_ID() \n");
          APP_ABORT();
        }
  // if (Original_Referenced_Array->Array_Descriptor == NULL)
  //    {
  //      printf ("ERROR: Original_Referenced_Array == NULL in Array_ID() \n");
  //      APP_ABORT();
  //    }
#endif
  // return Original_Referenced_Array->Array_ID();
     return Original_Referenced_Array->Array_ID();
   }

void doubleArray_Operand_Storage::Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of doubleArray_Operand_Storage::Make_Local_Variable_Persistant_And_Update_Storage - X_Pointer = %p \n",X_Pointer);

  /* This function takes the doubleArray pointers and places them into a doubleArray 
  // variable with non-local persistence.  Thus we extend the lifetime of
  // the local variable to suport the defered evaluation which will take 
  // place at some later time!

  // (Since defered evaluation is turned on and off in the same scope (a rule of use),
  // local variables that are built by defered evaluation are deleted while
  // defered evaluation is still in effect!  All local variables built during
  // defered evaluation are marked as such by a variable in the Array_Descriptor!).
  */
  // Make sure that we are only doing this for local variables!
     if (!Original_Referenced_Array->Is_Built_By_Defered_Evaluation())
        {
          printf ("ERROR: doubleArray_Operand_Storage not built during Deffered evaluation! \n");
          APP_ABORT();
        }
#endif

  /* The first iteration of the loop to extend the lifetime of the local
  // variable has to build a new variable on the heap. Since the input is the
  // pointer to the Tail_Of_List's doubleArray in storage we check to see if they
  // are the same and if so it means we are at the first iteration.
  */
     if (Original_Referenced_Array == X_Pointer)
        {
#if COMPILE_DEBUG_STATEMENTS
       // Views have to have local scope so we have to extend there lifetime under deferred evaluation!
       // if (Original_Referenced_Array->Array_Descriptor.Is_A_View)
       //    {
       //      printf ("ERROR in Make_Local_Variable_Persistant_And_Update_Storage -- Initial reference is a view! \n");
       //      APP_ABORT();
       //    }

       // So we create a doubleArray on the heap using the data in the Original_Referenced_Array
          if (APP_DEBUG > 3)
               printf ("Create a new variable on the heap! \n");
#endif

       // Call the version of this constructor that will not build another Storage Object!
       // The other version of the constructor must build a new storage opbject since
       // it is cvalled by the indexing operators and this is how they (and some other
       // functions like the reference function) obtain their support for deferred evaluation!
#if 0
#if defined(PPP)
       // Comment out this constructor because we don't want to support it and in general P++
       // is not well tested with the deffered evaluation mechanism anyway!
          Original_Referenced_Array = new doubleArray ( Original_Referenced_Array->Array_Descriptor.SerialArray ,
                                                      &(Original_Referenced_Array->Array_Descriptor) , NULL );
#else
          Original_Referenced_Array = new doubleArray ( Original_Referenced_Array->Array_Descriptor.Array_Data ,
                                                      &(Original_Referenced_Array->Array_Descriptor) , NULL );
#endif
#else
          printf ("Not implemented! Support for some defered evaluation in removed! \n");
	  APP_ABORT();

          Original_Referenced_Array = NULL;
#endif

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
              printf ("Original_Referenced_Array = %p \n",Original_Referenced_Array);
#endif
        }
       else
        {
       // Otherwise we reset the Original_Referenced_Array to the new heap doubleArray!
       // We handle the case of where the referenced array of the view was a local variable!
#if 0
       OLD CODE to handle view seperately!
          if (Original_Referenced_Array->Array_Descriptor.Is_A_View)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                   printf ("Reset Original_Referenced_Array->Array_Descriptor.Original_Array pointers! \n");
#endif
            // Original_Referenced_Array->Array_Descriptor.Original_Array = (doubleArray *) X_Pointer;
            // Original_Referenced_Array->Array_Data = ((doubleArray *) X_Pointer)->Array_Data;

            // Views are local in scope!  (Bug fix: 3/2/94)
               Original_Referenced_Array = (doubleArray *) X_Pointer;
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Reset the Original_Referenced_Array to the new heap doubleArray! \n");
#endif
               Original_Referenced_Array = (doubleArray *) X_Pointer;
             }
#else
       // NEW CODE (3/2/94) treats views same as local variables!
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Reset the Original_Referenced_Array to the new heap doubleArray! \n");
#endif
          Original_Referenced_Array = (doubleArray *) X_Pointer;
#endif
        }
   }

//#if !defined(GNU)
//inline 
//#endif
INLINE
Operand_Storage::Operand_Storage ( const floatArray & X )
   {
  // Execution is from tail --> head!  We add new elements to the head!
  // This matches the chronological order of evaluation

  // This creates a list for each variable recording its use and 
  // provides a linked list that can be traversed (backward) to 
  // build a persistant array variable (for the first use recorded
  // in the list) and then change the pointers to point to that
  // persistant variable in the subsequent recorded uses.
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of Operand_Storage::Operand_Storage ( const floatArray & X ) \n");
#endif

  // If we are not using defered evaluation then we don't have to keep track of all the 
  // linked list stuff.  This is the case in abstract_op.C where the abstract operators
  // can just creat execution objects and execute them rather than duplicating all
  // the code in each abstract operator.  This make the abstract operators simple enough
  // to inline in the operators defined in operator.C!

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // X.display("X in Operand_Storage::Operand_Storage!");

       // This is the simple way to test to see if this variable was built under defered evaluation!
       // And this checks to see if this is the first use of the variable!
       // if (X.Array_Storage == NULL && !X.Array_Descriptor.Is_A_View)

       // Views now have local scope and so they must be processed just like local variables!
          if (X.Array_Storage == NULL)
             {
            // This is how we restrict the linked lists of references to local
            // variables only. This makes it faster to do the defered evaluation.
               if ( (X.Is_Built_By_Defered_Evaluation()) &&
                    (!X.isTemporary()) )
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("X.Array_Storage == NULL and Built_By_Defered_Evaluation (fixing that up!) \n");
#endif
                    ((floatArray &) X).Array_Storage = this;
                  }

               Tail_Of_List               = this;
               Next_Array_Defered_Operand = NULL;
               Head_Of_List               = this;
             } 
            else
             {  
#if COMPILE_DEBUG_STATEMENTS
            // error checking
               if (X.Is_Built_By_Defered_Evaluation() == FALSE)
                  {
                    printf ("ERROR: X.Array_Descriptor.Is_Bult_By_Defered_Evaluation == FALSE \n");
                    APP_ABORT();
                  }
#endif

  // We have to allow the processing of views since they are now have local scope!
#if 0
               if (X.Array_Descriptor.Is_A_View)
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("X is a view! \n");

                 // error checking
                 // if (X.Array_Storage != NULL)  // Then X is a view!
                 //    {
                 //      printf ("ERROR: X.Array_Storage != NULL \n");
                 //      APP_ABORT();
                 //    }
#endif

                 // error checking
                    if (X.Array_Descriptor.Original_Array->Array_Storage == NULL)
                       {
                      // There is no list to add this object to! So do not add it to any list!
                      // printf ("Sorry, not supported yet, the object of which this is a view of was not built under defered evaluation! \n");
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 3)
                              printf ("NOTE: X.Array_Descriptor.Original_Array->Array_Storage == NULL -- There is no list to add this object to! \n");
                      // APP_ABORT();
#endif
                       }
                      else
                       {
#if COMPILE_DEBUG_STATEMENTS
                      // error checking
                         if (X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List == NULL)
                            {
                              printf ("ERROR: X.Array_Descriptor.Original_Array->Array_Storage == NULL \n");
                              APP_ABORT();
                            }

                         if (APP_DEBUG > 3)
                              printf ("NOTE: (In Operand_Storage constructor) Setting up the NEXT LINK! ... \n");
#endif

                         X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List->Next_Array_Defered_Operand = this;
                         X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List = this;
                         Tail_Of_List               = NULL;
                         Next_Array_Defered_Operand = NULL;
                         Head_Of_List               = NULL; 
                       }
                  }
                 else
                  {
#endif
#if COMPILE_DEBUG_STATEMENTS
                    if (X.Array_Storage->Head_Of_List == NULL)
                       {
                         printf ("ERROR: X.Array_Storage->Head_Of_List == NULL \n");
                         APP_ABORT();
                       }

                    if (APP_DEBUG > 3)
                         printf ("NOTE: (In Operand_Storage constructor) Setting up the NEXT LINK! ... \n");
#endif

                    X.Array_Storage->Head_Of_List->Next_Array_Defered_Operand = this;
                    X.Array_Storage->Head_Of_List = this;
                    Tail_Of_List               = NULL;
                    Next_Array_Defered_Operand = NULL;
                    Head_Of_List               = NULL; 
               // }
             }
        }  // END if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
   } 
 
floatArray_Operand_Storage::~floatArray_Operand_Storage ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray_Operand_Storage::~floatArray_Operand_Storage \n");
   }
 
floatArray_Operand_Storage::floatArray_Operand_Storage ()
   {
     printf ("ERROR: This constructor should not be called (inside of floatArray_Operand_Storage)! \n");
     APP_ABORT();
   }
 
floatArray_Operand_Storage::floatArray_Operand_Storage ( const floatArray & X )
   : // Original_Referenced_Array(&((floatArray &) X)) , 
     Operand_Storage(X)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Operand_Storage::floatArray_Operand_Storage constructor(X)! \n");
#endif

     Original_Referenced_Array = &((floatArray &) X);
   } 
 
floatArray_Operand_Storage::floatArray_Operand_Storage ( const floatArray_Operand_Storage & X )
   : Operand_Storage(X)
   {
     printf ("ERROR: Inside of copy constructor floatArray_Operand_Storage::floatArray_Operand_Storage constructor(const & X)! \n");
     APP_ABORT();
   } 
 
floatArray_Operand_Storage::floatArray_Operand_Storage ( floatArray * X_Pointer )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Operand_Storage::floatArray_Operand_Storage constructor(X_Pointer)! \n");
#endif

     Original_Referenced_Array = X_Pointer;
   } 
 
// This is a virtual function used to access the type specific floatArray reference!
void * floatArray_Operand_Storage::Get_Original_Array_Pointer()
   {
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in floatArray_Operand_Storage::Get_Original_Array_Pointer()! \n");
          APP_ABORT();
        }

     return (void*) Original_Referenced_Array;
   }

#if 0
int floatArray_Operand_Storage::Is_A_Temporary () const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in Is_A_Temporary() \n");
          APP_ABORT();
        }
  // if (Original_Referenced_Array->Array_Descriptor == NULL)
  //    {
  //      printf ("ERROR: Original_Referenced_Array == NULL in Is_A_Temporary() \n");
  //      APP_ABORT();
  //    }
#endif
     return Original_Referenced_Array->isTemporary();
   }
#endif

int floatArray_Operand_Storage::Array_ID () const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in Array_ID() \n");
          APP_ABORT();
        }
  // if (Original_Referenced_Array->Array_Descriptor == NULL)
  //    {
  //      printf ("ERROR: Original_Referenced_Array == NULL in Array_ID() \n");
  //      APP_ABORT();
  //    }
#endif
  // return Original_Referenced_Array->Array_ID();
     return Original_Referenced_Array->Array_ID();
   }

void floatArray_Operand_Storage::Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of floatArray_Operand_Storage::Make_Local_Variable_Persistant_And_Update_Storage - X_Pointer = %p \n",X_Pointer);

  /* This function takes the floatArray pointers and places them into a floatArray 
  // variable with non-local persistence.  Thus we extend the lifetime of
  // the local variable to suport the defered evaluation which will take 
  // place at some later time!

  // (Since defered evaluation is turned on and off in the same scope (a rule of use),
  // local variables that are built by defered evaluation are deleted while
  // defered evaluation is still in effect!  All local variables built during
  // defered evaluation are marked as such by a variable in the Array_Descriptor!).
  */
  // Make sure that we are only doing this for local variables!
     if (!Original_Referenced_Array->Is_Built_By_Defered_Evaluation())
        {
          printf ("ERROR: floatArray_Operand_Storage not built during Deffered evaluation! \n");
          APP_ABORT();
        }
#endif

  /* The first iteration of the loop to extend the lifetime of the local
  // variable has to build a new variable on the heap. Since the input is the
  // pointer to the Tail_Of_List's floatArray in storage we check to see if they
  // are the same and if so it means we are at the first iteration.
  */
     if (Original_Referenced_Array == X_Pointer)
        {
#if COMPILE_DEBUG_STATEMENTS
       // Views have to have local scope so we have to extend there lifetime under deferred evaluation!
       // if (Original_Referenced_Array->Array_Descriptor.Is_A_View)
       //    {
       //      printf ("ERROR in Make_Local_Variable_Persistant_And_Update_Storage -- Initial reference is a view! \n");
       //      APP_ABORT();
       //    }

       // So we create a floatArray on the heap using the data in the Original_Referenced_Array
          if (APP_DEBUG > 3)
               printf ("Create a new variable on the heap! \n");
#endif

       // Call the version of this constructor that will not build another Storage Object!
       // The other version of the constructor must build a new storage opbject since
       // it is cvalled by the indexing operators and this is how they (and some other
       // functions like the reference function) obtain their support for deferred evaluation!
#if 0
#if defined(PPP)
       // Comment out this constructor because we don't want to support it and in general P++
       // is not well tested with the deffered evaluation mechanism anyway!
          Original_Referenced_Array = new floatArray ( Original_Referenced_Array->Array_Descriptor.SerialArray ,
                                                      &(Original_Referenced_Array->Array_Descriptor) , NULL );
#else
          Original_Referenced_Array = new floatArray ( Original_Referenced_Array->Array_Descriptor.Array_Data ,
                                                      &(Original_Referenced_Array->Array_Descriptor) , NULL );
#endif
#else
          printf ("Not implemented! Support for some defered evaluation in removed! \n");
	  APP_ABORT();

          Original_Referenced_Array = NULL;
#endif

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
              printf ("Original_Referenced_Array = %p \n",Original_Referenced_Array);
#endif
        }
       else
        {
       // Otherwise we reset the Original_Referenced_Array to the new heap floatArray!
       // We handle the case of where the referenced array of the view was a local variable!
#if 0
       OLD CODE to handle view seperately!
          if (Original_Referenced_Array->Array_Descriptor.Is_A_View)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                   printf ("Reset Original_Referenced_Array->Array_Descriptor.Original_Array pointers! \n");
#endif
            // Original_Referenced_Array->Array_Descriptor.Original_Array = (floatArray *) X_Pointer;
            // Original_Referenced_Array->Array_Data = ((floatArray *) X_Pointer)->Array_Data;

            // Views are local in scope!  (Bug fix: 3/2/94)
               Original_Referenced_Array = (floatArray *) X_Pointer;
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Reset the Original_Referenced_Array to the new heap floatArray! \n");
#endif
               Original_Referenced_Array = (floatArray *) X_Pointer;
             }
#else
       // NEW CODE (3/2/94) treats views same as local variables!
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Reset the Original_Referenced_Array to the new heap floatArray! \n");
#endif
          Original_Referenced_Array = (floatArray *) X_Pointer;
#endif
        }
   }


#define INTARRAY 
//#if !defined(GNU)
//inline 
//#endif
INLINE
Operand_Storage::Operand_Storage ( const intArray & X )
   {
  // Execution is from tail --> head!  We add new elements to the head!
  // This matches the chronological order of evaluation

  // This creates a list for each variable recording its use and 
  // provides a linked list that can be traversed (backward) to 
  // build a persistant array variable (for the first use recorded
  // in the list) and then change the pointers to point to that
  // persistant variable in the subsequent recorded uses.
 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of Operand_Storage::Operand_Storage ( const intArray & X ) \n");
#endif

  // If we are not using defered evaluation then we don't have to keep track of all the 
  // linked list stuff.  This is the case in abstract_op.C where the abstract operators
  // can just creat execution objects and execute them rather than duplicating all
  // the code in each abstract operator.  This make the abstract operators simple enough
  // to inline in the operators defined in operator.C!

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // X.display("X in Operand_Storage::Operand_Storage!");

       // This is the simple way to test to see if this variable was built under defered evaluation!
       // And this checks to see if this is the first use of the variable!
       // if (X.Array_Storage == NULL && !X.Array_Descriptor.Is_A_View)

       // Views now have local scope and so they must be processed just like local variables!
          if (X.Array_Storage == NULL)
             {
            // This is how we restrict the linked lists of references to local
            // variables only. This makes it faster to do the defered evaluation.
               if ( (X.Is_Built_By_Defered_Evaluation()) &&
                    (!X.isTemporary()) )
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("X.Array_Storage == NULL and Built_By_Defered_Evaluation (fixing that up!) \n");
#endif
                    ((intArray &) X).Array_Storage = this;
                  }

               Tail_Of_List               = this;
               Next_Array_Defered_Operand = NULL;
               Head_Of_List               = this;
             } 
            else
             {  
#if COMPILE_DEBUG_STATEMENTS
            // error checking
               if (X.Is_Built_By_Defered_Evaluation() == FALSE)
                  {
                    printf ("ERROR: X.Array_Descriptor.Is_Bult_By_Defered_Evaluation == FALSE \n");
                    APP_ABORT();
                  }
#endif

  // We have to allow the processing of views since they are now have local scope!
#if 0
               if (X.Array_Descriptor.Is_A_View)
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 3)
                         printf ("X is a view! \n");

                 // error checking
                 // if (X.Array_Storage != NULL)  // Then X is a view!
                 //    {
                 //      printf ("ERROR: X.Array_Storage != NULL \n");
                 //      APP_ABORT();
                 //    }
#endif

                 // error checking
                    if (X.Array_Descriptor.Original_Array->Array_Storage == NULL)
                       {
                      // There is no list to add this object to! So do not add it to any list!
                      // printf ("Sorry, not supported yet, the object of which this is a view of was not built under defered evaluation! \n");
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 3)
                              printf ("NOTE: X.Array_Descriptor.Original_Array->Array_Storage == NULL -- There is no list to add this object to! \n");
                      // APP_ABORT();
#endif
                       }
                      else
                       {
#if COMPILE_DEBUG_STATEMENTS
                      // error checking
                         if (X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List == NULL)
                            {
                              printf ("ERROR: X.Array_Descriptor.Original_Array->Array_Storage == NULL \n");
                              APP_ABORT();
                            }

                         if (APP_DEBUG > 3)
                              printf ("NOTE: (In Operand_Storage constructor) Setting up the NEXT LINK! ... \n");
#endif

                         X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List->Next_Array_Defered_Operand = this;
                         X.Array_Descriptor.Original_Array->Array_Storage->Head_Of_List = this;
                         Tail_Of_List               = NULL;
                         Next_Array_Defered_Operand = NULL;
                         Head_Of_List               = NULL; 
                       }
                  }
                 else
                  {
#endif
#if COMPILE_DEBUG_STATEMENTS
                    if (X.Array_Storage->Head_Of_List == NULL)
                       {
                         printf ("ERROR: X.Array_Storage->Head_Of_List == NULL \n");
                         APP_ABORT();
                       }

                    if (APP_DEBUG > 3)
                         printf ("NOTE: (In Operand_Storage constructor) Setting up the NEXT LINK! ... \n");
#endif

                    X.Array_Storage->Head_Of_List->Next_Array_Defered_Operand = this;
                    X.Array_Storage->Head_Of_List = this;
                    Tail_Of_List               = NULL;
                    Next_Array_Defered_Operand = NULL;
                    Head_Of_List               = NULL; 
               // }
             }
        }  // END if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
   } 
 
intArray_Operand_Storage::~intArray_Operand_Storage ()
   {
     if (APP_DEBUG > 0)
          printf ("Inside of intArray_Operand_Storage::~intArray_Operand_Storage \n");
   }
 
intArray_Operand_Storage::intArray_Operand_Storage ()
   {
     printf ("ERROR: This constructor should not be called (inside of intArray_Operand_Storage)! \n");
     APP_ABORT();
   }
 
intArray_Operand_Storage::intArray_Operand_Storage ( const intArray & X )
   : // Original_Referenced_Array(&((intArray &) X)) , 
     Operand_Storage(X)
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Operand_Storage::intArray_Operand_Storage constructor(X)! \n");
#endif

     Original_Referenced_Array = &((intArray &) X);
   } 
 
intArray_Operand_Storage::intArray_Operand_Storage ( const intArray_Operand_Storage & X )
   : Operand_Storage(X)
   {
     printf ("ERROR: Inside of copy constructor intArray_Operand_Storage::intArray_Operand_Storage constructor(const & X)! \n");
     APP_ABORT();
   } 
 
intArray_Operand_Storage::intArray_Operand_Storage ( intArray * X_Pointer )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Operand_Storage::intArray_Operand_Storage constructor(X_Pointer)! \n");
#endif

     Original_Referenced_Array = X_Pointer;
   } 
 
// This is a virtual function used to access the type specific intArray reference!
void * intArray_Operand_Storage::Get_Original_Array_Pointer()
   {
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in intArray_Operand_Storage::Get_Original_Array_Pointer()! \n");
          APP_ABORT();
        }

     return (void*) Original_Referenced_Array;
   }

#if 0
int intArray_Operand_Storage::Is_A_Temporary () const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in Is_A_Temporary() \n");
          APP_ABORT();
        }
  // if (Original_Referenced_Array->Array_Descriptor == NULL)
  //    {
  //      printf ("ERROR: Original_Referenced_Array == NULL in Is_A_Temporary() \n");
  //      APP_ABORT();
  //    }
#endif
     return Original_Referenced_Array->isTemporary();
   }
#endif

int intArray_Operand_Storage::Array_ID () const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (Original_Referenced_Array == NULL)
        {
          printf ("ERROR: Original_Referenced_Array == NULL in Array_ID() \n");
          APP_ABORT();
        }
  // if (Original_Referenced_Array->Array_Descriptor == NULL)
  //    {
  //      printf ("ERROR: Original_Referenced_Array == NULL in Array_ID() \n");
  //      APP_ABORT();
  //    }
#endif
  // return Original_Referenced_Array->Array_ID();
     return Original_Referenced_Array->Array_ID();
   }

void intArray_Operand_Storage::Make_Local_Variable_Persistant_And_Update_Storage ( void *X_Pointer )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 3)
          printf ("Inside of intArray_Operand_Storage::Make_Local_Variable_Persistant_And_Update_Storage - X_Pointer = %p \n",X_Pointer);

  /* This function takes the intArray pointers and places them into a intArray 
  // variable with non-local persistence.  Thus we extend the lifetime of
  // the local variable to suport the defered evaluation which will take 
  // place at some later time!

  // (Since defered evaluation is turned on and off in the same scope (a rule of use),
  // local variables that are built by defered evaluation are deleted while
  // defered evaluation is still in effect!  All local variables built during
  // defered evaluation are marked as such by a variable in the Array_Descriptor!).
  */
  // Make sure that we are only doing this for local variables!
     if (!Original_Referenced_Array->Is_Built_By_Defered_Evaluation())
        {
          printf ("ERROR: intArray_Operand_Storage not built during Deffered evaluation! \n");
          APP_ABORT();
        }
#endif

  /* The first iteration of the loop to extend the lifetime of the local
  // variable has to build a new variable on the heap. Since the input is the
  // pointer to the Tail_Of_List's intArray in storage we check to see if they
  // are the same and if so it means we are at the first iteration.
  */
     if (Original_Referenced_Array == X_Pointer)
        {
#if COMPILE_DEBUG_STATEMENTS
       // Views have to have local scope so we have to extend there lifetime under deferred evaluation!
       // if (Original_Referenced_Array->Array_Descriptor.Is_A_View)
       //    {
       //      printf ("ERROR in Make_Local_Variable_Persistant_And_Update_Storage -- Initial reference is a view! \n");
       //      APP_ABORT();
       //    }

       // So we create a intArray on the heap using the data in the Original_Referenced_Array
          if (APP_DEBUG > 3)
               printf ("Create a new variable on the heap! \n");
#endif

       // Call the version of this constructor that will not build another Storage Object!
       // The other version of the constructor must build a new storage opbject since
       // it is cvalled by the indexing operators and this is how they (and some other
       // functions like the reference function) obtain their support for deferred evaluation!
#if 0
#if defined(PPP)
       // Comment out this constructor because we don't want to support it and in general P++
       // is not well tested with the deffered evaluation mechanism anyway!
          Original_Referenced_Array = new intArray ( Original_Referenced_Array->Array_Descriptor.SerialArray ,
                                                      &(Original_Referenced_Array->Array_Descriptor) , NULL );
#else
          Original_Referenced_Array = new intArray ( Original_Referenced_Array->Array_Descriptor.Array_Data ,
                                                      &(Original_Referenced_Array->Array_Descriptor) , NULL );
#endif
#else
          printf ("Not implemented! Support for some defered evaluation in removed! \n");
	  APP_ABORT();

          Original_Referenced_Array = NULL;
#endif

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
              printf ("Original_Referenced_Array = %p \n",Original_Referenced_Array);
#endif
        }
       else
        {
       // Otherwise we reset the Original_Referenced_Array to the new heap intArray!
       // We handle the case of where the referenced array of the view was a local variable!
#if 0
       OLD CODE to handle view seperately!
          if (Original_Referenced_Array->Array_Descriptor.Is_A_View)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                   printf ("Reset Original_Referenced_Array->Array_Descriptor.Original_Array pointers! \n");
#endif
            // Original_Referenced_Array->Array_Descriptor.Original_Array = (intArray *) X_Pointer;
            // Original_Referenced_Array->Array_Data = ((intArray *) X_Pointer)->Array_Data;

            // Views are local in scope!  (Bug fix: 3/2/94)
               Original_Referenced_Array = (intArray *) X_Pointer;
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 3)
                    printf ("Reset the Original_Referenced_Array to the new heap intArray! \n");
#endif
               Original_Referenced_Array = (intArray *) X_Pointer;
             }
#else
       // NEW CODE (3/2/94) treats views same as local variables!
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 3)
               printf ("Reset the Original_Referenced_Array to the new heap intArray! \n");
#endif
          Original_Referenced_Array = (intArray *) X_Pointer;
#endif
        }
   }

#undef INTARRAY 
 





