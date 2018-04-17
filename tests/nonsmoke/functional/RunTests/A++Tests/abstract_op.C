
#include "A++.h"
extern "C"
   {
/* machine.h is found in MDI/machine.h through a link in A++/inc lude and P++/inc lude */
#include "machine.h"
   }

#include "inline_support.h"

#if defined(PPP)
#error Common_Code/abstract_op.C not used for P++ (P++ uses its own version)
#endif

// extern int Global_Zero_Array [4];

#define EXECUTE_MDI_FUNCTIONS             TRUE
#define TEST_EXECUTION_OBJECT_ABSTRACTION FALSE
#define INITIALIZE_TEMPORARY_MEMORY       FALSE
#define USE_TEMPORARY_DELETE_FUNCTIONS    TRUE






// We give this variable file scope since inlining is not implemented in the C++ compliler
// in the case of a function containing a static variable! See function 
// Delete_Lhs_If_Temporary locaated in inline_func.h.
// static doubleArray *Last_Lhs_doubleArray_Operand = NULL;
doubleArray *Last_Lhs_doubleArray_Operand = NULL;

// We give this variable file scope since inlining is not implemented in the C++ compliler
// in the case of a function containing a static variable! See function 
// Delete_Lhs_If_Temporary locaated in inline_func.h.
// static floatArray *Last_Lhs_floatArray_Operand = NULL;
floatArray *Last_Lhs_floatArray_Operand = NULL;

// We give this variable file scope since inlining is not implemented in the C++ compliler
// in the case of a function containing a static variable! See function 
// Delete_Lhs_If_Temporary locaated in inline_func.h.
// static intArray *Last_Lhs_intArray_Operand = NULL;
intArray *Last_Lhs_intArray_Operand = NULL;




#if !defined(USE_EXPRESSION_TEMPLATES)

// *****************************************************************
// *****************************************************************
//                    operator= functions
// *****************************************************************
// *****************************************************************

// **********************************************************
// The equals operator taking a scalar
// **********************************************************
doubleArray & 
doubleArray::operator= ( double x )
   {
  // This prevents the annoying "feature" of M++ where a scalar is assigned to an array and the 
  // array object is set to be a 1x1x1x1 array.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of doubleArray::operator=! (double) (scalar value = %f)\n",double(x));
#endif
 
     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          doubleArray_Function_5 *Execution_Object = new doubleArray_Function_5 
	     ( Assignment , MDI_d_Assign_Array_Equals_Scalar_Accumulate_To_Operand ,
	       *this , x );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif

 
       /* The function name that we call here is slightly effected by the fact that we use
       // a marco processor to generate the machine dependent functions a better name would have
       // been "Assign_Array_Data_Equals_Scalar" but this would have not made for a good
       // name (after macro expansion) for the other operators (like +,-,*, and /).
       */
#if EXECUTE_MDI_FUNCTIONS
          MDI_d_Assign_Array_Equals_Scalar_Accumulate_To_Operand ( 
                         Array_Descriptor.Array_Data , 
#ifndef FORTRAN_MDI
                         x , 
#else
                         &x , 
#endif
                      Mask_Array_Data ,
                      (array_domain*) (&Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
 
       // Now we have to delete the "this" object if it is a View! But we are the this object
       // so we have to be very carefull how this is done!
          Delete_Lhs_If_Temporary ( *this );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in doubleArray::operator=");
#endif

     return *this;
   }

// **********************************************************
// The other equals operator taking an array object
// **********************************************************
doubleArray & 
doubleArray::operator= ( const doubleArray & Rhs )
   {
  // This function is only called once for each Array statment!

#if COMPILE_DEBUG_STATEMENTS
     if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
        {
          printf ("@@@@@ Inside of doubleArray::operator=(const doubleArray & Rhs) (id=%d) = (id=%d) \n",Array_ID(),Rhs.Array_ID());
          this->displayReferenceCounts("Lhs in doubleArray & operator=(doubleArray,doubleArray)");
          Rhs.displayReferenceCounts("Rhs in doubleArray & operator=(doubleArray,doubleArray)");
       }

     Test_Consistency("Test Lhs in doubleArray::operator=");
     Rhs.Test_Consistency("Test Rhs in doubleArray::operator=");
#endif

     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (isTemporary() == TRUE)
        {
          printf ("ERROR: Assignment to a temporary makes little sence so it has been ");
          printf ("disallowed! \n");
          printf ("       If you really feel you need this sort of functionality then ");
          printf ("let me know ... Dan Quinlan \n");
          APP_ABORT();
       }
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Check to see if the operator()() is used otherwise we do the assignment!
       // The function Binary_Conformable returns false if the "where" mask is present!
          if ( Binary_Conformable (Rhs) && Rhs.isTemporary() )
             {
               Add_Defered_Expression ( new doubleArray_Function_Steal_Data ( Assignment , *this , Rhs ) );
             }
            else
             {
               Add_Defered_Expression ( new doubleArray_Function_3 ( Assignment , MDI_d_Assign_Array_Equals_Array_Accumulate_To_Operand , *this , Rhs ) );
             }
        }
       else
        {
       // BUG FIX (July 29 1993; dquinlan)!
       // These can't be reference variables since they are then not initialized 
       // properly so they are implemented as pointers to pointers to the array data!  

          double** Lhs_Data_Pointer = &(Array_Descriptor.Array_Data);
          double** Rhs_Data_Pointer = &(((doubleArray &)Rhs).Array_Descriptor.Array_Data);

          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


       // Check to see if one of the operator()() is used otherwise we do the 
       // assignment!  The function Binary_Conformable returns false if the "where" 
       // mask is present! This works independent of the reference counting (I think)!

          if ( (getRawDataReferenceCount() == getRawDataReferenceCountBase()) &&
                Binary_Conformable (Rhs) && 
                Rhs.isTemporary() &&
               !Rhs.isView() )
             {
            // Give back the original Array_Data memory (but only if it exists)!
            // But it should always exist so we don't really have to check unless it is 
	    // a Null array!

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Skip the assignment by stealing the data! \n");
#endif

            /*
            // ... bug fix (8/26/96, kdb) this was previously called later
            // after deleteing the Array_Descriptor but because the data
            // wasn't deleted, the reference count in
            // Array_Reference_Count_Array[Array_Id] was too high and so
            // the Array_ID wasn't put back onto the list ...
            */
               Delete_Array_Data ();

               if (isNullArray())
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("Lhs is a Null Array so copy descriptor before assignment! \n");
#endif

                 // Bug Fix (8/17/94) Even a Null array must have a properly computed array descriptor
                    APP_ASSERT(isTemporary() == FALSE);

                 // We know it is a temporary because we only try to steal temporaries
                 // so check to temporaries that are a view -- but we don't allow views
                 // so it CAN'T be a view.
                    APP_ASSERT(Rhs.isView() == FALSE);

                 // We have to reset the reference count since the operator= used in the Build_Temporary_By_Example function
                 // will get trapped in the test_Consistancy test if we don't.
                    resetRawDataReferenceCount();
                    Array_Descriptor.Build_Temporary_By_Example (Rhs.Array_Descriptor);

                 // This is redundent since the base of the temporary is already set to ZERO!
                 // setBase(0);
                    APP_ASSERT (getBase() == 0);

                 // This is no longer going to be a temporary so mark it as a non temporary
                 // Array_Descriptor.Array_Domain.Is_A_Temporary = FALSE;
                    setTemporary(FALSE);

                 // Double check to make sure it is not a view
                    APP_ASSERT( isView() == FALSE );
                  }

            // ... bug fix (8/26/96,kdb) see comment above ...
            // Delete_Array_Data ();
               *Lhs_Data_Pointer = NULL;

            // BUG FIX: (18/1/94) fixed reference counts.
            // Reset the reference count to zero as we steal the data because the 
            // new data should have a reference count of zero (not -1 as would be 
            // the case if it had been deleted)! setting it to ZERO means there are
            // NO (ZERO) external references to this data (only the one by this array 
            // object).

            // I don't want to provide a means to reset or assign specific reference 
            // counts (dangerous I think)

            // Steal the pointer (to the original array) from the Rhs's data
            // Note that these are REFERENCE variables!

               *Lhs_Data_Pointer = *Rhs_Data_Pointer;
            // ... change (8/21/96,kdb) Add eight view pointers ...
            // Array_View_Pointer = Array_Descriptor.Array_Data + Array_Descriptor->Scalar_Offset;

               POINTER_LIST_INITIALIZATION_MACRO;

            // Since the previous copy of the array data was deleted the reference count 
	    // of the Raw Data is not -1 (typically) so we have to reset it.  We could 
            // just increment it -- but error checking within the 
            // incrementRawDataReferenceCount would trap out the case where the
            // reference count of the raw data is less than ZERO.  So we have a special 
            // inline function to force the reset value to ZERO.

               resetRawDataReferenceCount();

            // We can set the correct one by using the fact that 
            // Rhs_Data_Pointer is a pointer to a pointer to the array data!

            // ... (bug fix, 6/27/96, kdb) don't set this pointer to null here
            // because otherwise the array won't be deleted.  Also increment
            // the reference count for Rhs so Array_Data won't be deleted ...
            // *Rhs_Data_Pointer = NULL;

            // printf ("In operator=: Calling Rhs.incrementRawDataReferenceCount() \n");
               Rhs.incrementRawDataReferenceCount();

            /*
            // ... the Array_Descriptor will be deleted but because the
	    // Array_Reference_Count_Array was just incremented the array ID
            // won't be put back in the list like it should be.  Before
            // deleting, force the array ID onto the stack.  The 
            // Array_Reference_Count_Array[Array_ID] will already be 0 after
            // the delete.  (This problem comes up because of the assumption
            // that the only time 2 arrays share Array_Data is when one is a
            // view of another and so has the same Array_ID) ...
            */

            // Bugfix (11/6/2000) Deleting the array should put it back onto 
            // the stack of array ID for future use (I think).
            // Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(Rhs.Array_ID());

            // get the array id of the Rhs before it is deleted
               int rhsArrayID = Rhs.Array_ID();

            // The Rhs will be deleted but until then we can't really call any
            // of it's member functions since there is no data associated with it.
            // printf ("In doubleArray::operator= calling Delete_If_Temporary ( Rhs ) \n");
               Delete_If_Temporary ( Rhs );

            // Bugfix (11/7/2000) 
            // Move to after the call to Delete_If_Temporary() since that function 
            // calls the Test_Consistency function which will report an error if the 
            // array ID is pushed onto the stack while it is a valid array ID still in use.
            // Bugfix (11/6/2000) Deleting the array should put it back onto 
            // the stack of array ID for future use (I think).
            // Bugfix (11/11/2000) accessing data after it is deleted
            // Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(Rhs.Array_ID());
               Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(rhsArrayID);

            // Bugfix (12/6/2000) Also reset the reference count associated with this id value!
               Array_Domain_Type::Array_Reference_Count_Array [rhsArrayID] = getRawDataReferenceCountBase() - 1;

            // Now we have to delete the "this" object if it is a View! But we are the 
            // this object so we have to be very carefull how this is done!
            // printf ("In doubleArray::operator= calling Delete_Lhs_If_Temporary ( *this ) \n");
               Delete_Lhs_If_Temporary ( *this );
             }
            else // Do the assignment the hard way (element by element)!
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Do the assignment by calling the MDI function! \n");
#endif
            /* Call Assignment function
            // The function name that we call here is slightly effected by the fact that 
	    // we use a marco processor to generate the machine dependent functions a 
            // better name would have been "Assign_Array_Data_Equals_Scalar" but this 
            // would have not made for a good name (after macro expansion) for the other 
            // operators (like +,-,*, and /).
            */

            /* The array class must check for the case of self assignment,
            // if A=A then the assignement is skiped (using the logic below).
            // But if a region of A is assigned to another region of A then
            // because the overlap might not be computed correctly (we cannot
            // specify the order of evaluation in the MDI functions).
            */
               if (Array_ID() == Rhs.Array_ID())
                  {
                 // Possible case of A(I+1) = A(I) which would not evaluate correctly 
                 // because of the order of evaluation used in the MDI functions!
     
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("CASE: Array_ID() == Rhs.Array_ID() \n");
	               }
#endif
                    if ( !(isContiguousData() && Rhs.isContiguousData()) )
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                            printf ("Make a copy \n");
#endif

                      // Rhs.displayReferenceCounts("In operator=(doubleArray) BEFORE COPY: Rhs");

                      // Make a copy
                         doubleArray Rhs_Copy = Rhs;

                      // Rhs.displayReferenceCounts("In operator=(doubleArray) AFTER COPY: Rhs");
                      // Rhs_Copy.displayReferenceCounts("In operator=(doubleArray) AFTER COPY: Rhs_Copy");

                      // Now just call the operator= again (since Lhs and Rhs have different 
	              // array ids)!
                         *this = Rhs_Copy;
                       }
                      else
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                              printf ("CASE: A(I) = A(I) so skip the assignment operation! \n");
#endif
                      // We can skip the assignment of A(I) = A(I) but we have to delete the 
	              // Rhs view!
                         Delete_If_Temporary ( Rhs );
                       }
                  }
                 else
                  {
                 // Bugfix (3/29/95) fixed null array operations between Lhs and Rhs Null 
	         // array objects.  This fix is required for P++ to work properly on 
	         // processors where neither the Lhs of Rhs have an representation on the 
                 // local processor.
                    if (isNullArray() && !isView())
                       {
                      // Bug fix (8/9/94) need to build new descriptor to the correct size 
	              // of the Rhs
                      // APP_ASSERT( Array_Descriptor != NULL );

                      // This is much simpler thant what we have below and it allows the 
	              // bases of the Rhs to be preserved in the Lhs (which was not 
	              // previously true).
                         redim (Rhs);
                      // ... change (8/21/96,kdb) add 8 view pointers ...
	                 POINTER_LIST_INITIALIZATION_MACRO;
                       }

                    APP_ASSERT(isTemporary() == FALSE);

#if EXECUTE_MDI_FUNCTIONS
                    MDI_d_Assign_Array_Equals_Array_Accumulate_To_Operand
                       ( *Lhs_Data_Pointer , *Rhs_Data_Pointer , Mask_Array_Data ,
                         (array_domain*) (&Array_Descriptor.Array_Domain) , 
                         (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("Calling Delete_If_Temporary(Rhs) \n");
	               }
#endif
                 // If A(I) = A(I-1) then the view Rhs was handled by the copy constructor!
                    Delete_If_Temporary ( Rhs );

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("Calling Delete_Lhs_If_Temporary(*this) \n");
	               }
#endif
                 // Now we have to delete the "this" object if it is a View! But we are 
                 // the this object so we have to be very carefull how this is done!
                 // This function will be removed soon to streamline the operator= 
                 // member function
                    APP_ASSERT(isTemporary() == FALSE);
                    Delete_Lhs_If_Temporary ( *this );
                  }
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in doubleArray::operator=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("Leaving operator= (doubleArray) \n");
        }
#endif
  // operator= does the assignment and then returns a doubleArray
     return *this;
   }

// ******************************************************************
// ******************************************************************
//  Abstract Operators (with masking support for "where" statements)
// ******************************************************************
// ******************************************************************

/* These operators support the efficient handling of the Temporaries and Views 
// which are a part of the Array class.  The details of the manipulation are
// isolated into a few abstract operators below which take function pointers
// to make calls specific to the requirements of operator+, operator-, operator*,
// operator/, and other related operators.  This greatly simplifies the 
// implementation of the actual operators for +,-,*, and /.  It also
// isolates the mantainence of the code!  The functions that are used
// as parameters define the machine dependent interface.
// Each input function is handed a pointer to the Mask used in the support of
// the "where" statement that is a part of this array class library.  It is 
// the job of the input function (the machine dependent function) to
// recognize if the Mask pointer is valid (in FORTRAN the Mask_Stride != 0 for a 
// valid mask) and then use the mask to correctly compute the specific fuction.
*/

// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
doubleArray &
doubleArray::Abstract_Unary_Operator (
   const doubleArray & X ,
   MDI_double_Prototype_0 Operation_Array_Data ,
   MDI_double_Prototype_1 Operation_Array_Data_Accumulate_To_Existing_Operand ,
   int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #1 doubleArray::Abstract_Unary_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif
 
     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     doubleArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand ( X , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY)
             {
               Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND);
               Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_1 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , X ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY)
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) (
                    Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
                    (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
		    (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND);
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                    X.Array_Descriptor.Array_Data , Mask_Array_Data , 
		    (array_domain*) (&X.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif
            // Bugfix (12/4/2000) delete the temprary to be uniformally consistant with rest of code
               Delete_If_Temporary ( X );
             }
        } 

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #1 doubleArray::Abstract_Unary_Operator");

     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Temporary_Array.displayReferenceCounts("Temporary_Array in #1 doubleArray::Abstract_Unary_Operator");
        }
#endif

     return Temporary_Array;
   }



// Support for Conversion Operators
#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
intArray &
doubleArray::Abstract_int_Conversion_Operator ( 
   const doubleArray & X ,
   MDI_double_Prototype_convertTo_intArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & doubleArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the doubleArray::Abstract_int_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 doubleArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif

#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
floatArray &
doubleArray::Abstract_float_Conversion_Operator ( 
   const doubleArray & X ,
   MDI_double_Prototype_convertTo_floatArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & doubleArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     floatArray & Temporary_Array = *(floatArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the doubleArray::Abstract_float_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 doubleArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif

#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
doubleArray &
doubleArray::Abstract_double_Conversion_Operator ( 
   const doubleArray & X ,
   MDI_double_Prototype_convertTo_doubleArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & doubleArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     doubleArray & Temporary_Array = *(doubleArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the doubleArray::Abstract_double_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 doubleArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif


// ******************************************************************
//                   ABSTRACT BINARY OPERATOR
// ******************************************************************
doubleArray &
doubleArray::Abstract_Binary_Operator ( const doubleArray & Lhs , const doubleArray & Rhs ,
   MDI_double_Prototype_2 Operation_Array_Data ,
   MDI_double_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
   int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #3 doubleArray::Abstract_Binary_Operator (doubleArray,doubleArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #3 doubleArray::Abstract_Binary_Operator");
     Rhs.Test_Consistency("Test of Rhs in #3 doubleArray::Abstract_Binary_Operator");

  // Lhs.view("Lhs in (AT START OF FUNCTION) doubleArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AT START OF FUNCTION) doubleArray::Abstract_Binary_Operator");

  // Lhs.displayReferenceCounts("Lhs at TOP of #3 doubleArray::Abstract_Binary_Operator()");
  // Rhs.displayReferenceCounts("Rhs at TOP of #3 doubleArray::Abstract_Binary_Operator()");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;

#if COMPILE_DEBUG_STATEMENTS
  // Lhs.view("Lhs in (BEFORE SETUP OF TEMPORARY ARRAY) doubleArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (BEFORE SETUP OF TEMPORARY ARRAY) doubleArray::Abstract_Binary_Operator");
#endif

     doubleArray & Temporary_Array = 
          Build_New_Array_Or_Reuse_Operand ( Lhs , Rhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

#if COMPILE_DEBUG_STATEMENTS
  // Lhs.view("Lhs in (AFTER SETUP OF TEMPORARY ARRAY) doubleArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AFTER SETUP OF TEMPORARY ARRAY) doubleArray::Abstract_Binary_Operator");
     Temporary_Array.Test_Consistency("Test of Temporary_Array in #3 doubleArray::Abstract_Binary_Operator");
  // Temporary_Array.view("Temporary_Array in doubleArray::Abstract_Binary_Operator");
  // Lhs.view("Lhs in (AFTER SETUP OF TEMPORARY ARRAY AND TEST_CONSISTENCY) doubleArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AFTER SETUP OF TEMPORARY ARRAY AND TEST_CONSISTENCY) doubleArray::Abstract_Binary_Operator");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
             }
            else
             {
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
                    Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
                    Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Rhs , Lhs ) );
                  }
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Call machine dependent function! \n");

       // Temp code not valid for general use!
       // APP_ASSERT(Temporary_Array.Array_Data != NULL);
       // APP_ASSERT(Lhs.Array_Data != NULL);
       // APP_ASSERT(Rhs.Array_Data != NULL);
#endif

          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) 
		  (Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , 
		   Mask_Array_Data , 
		   (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
		   (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
                   (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
            // If we had to build the temporary using NEWLY_ALLOCATED_MEMORY then neither of the
            // input arrays where a temporary (which could be reused) -- so we don't have to
            // delete if temporary since we know that neither could be.  So the following
            // function calls can be eliminated.

            // printf ("Now call delete if temporary for Lhs and Rhs! \n");

            // Lhs.displayReferenceCounts("Lhs case NEWLY_ALLOCATED_MEMORY in #3 doubleArray::Abstract_Binary_Operator()");
            // Rhs.displayReferenceCounts("Rhs case NEWLY_ALLOCATED_MEMORY in #3 doubleArray::Abstract_Binary_Operator()");

               Delete_If_Temporary ( Lhs );
               Delete_If_Temporary ( Rhs );
             }
            else
             {
            // We use the same functions below but change the order 
            // of the Lhs and Rhs in the parameter list (and the order of the Base,Bound,etc)!
            // BUG FIX: (The first subtle error was that the Base,Bound,etc order was not swaped)
            // However we will leave the interface to the Machine_Dependent_Interface functions
            // such that these related functions are implemented once since we want to
            // limit the number of machine dependent functions (this enhances the portability).
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                     Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                     (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		     (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , 
		     Mask_Descriptor );
#endif
                 // Rhs.displayReferenceCounts("Rhs case MEMORY_FROM_LHS in #3 doubleArray::Abstract_Binary_Operator()");

                    Delete_If_Temporary ( Rhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS);
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                      Rhs.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                 // Lhs.displayReferenceCounts("Lhs case MEMORY_FROM_RHS in #3 doubleArray::Abstract_Binary_Operator()");

                 // Since we got the temporary from the Rhs the rule is that we first looked
                 // at the Lhs (and took it if it was a temporary) -- so the Lhs is NOT a 
                 // temporary.  So we can eliminate the function call to delete the Lhs.
                    Delete_If_Temporary ( Lhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Rhs );
                  }
             }
        } 

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #3 doubleArray::Abstract_Binary_Operator");
#endif
 
     return Temporary_Array;
   }

// ******************************************************************
//           ABSTRACT BINARY OPERATOR (NON COMMUTATIVE CASE)
// ******************************************************************
doubleArray &
doubleArray::Abstract_Binary_Operator_Non_Commutative ( 
     const doubleArray & Lhs , const doubleArray & Rhs ,
     MDI_double_Prototype_2 Operation_Array_Data ,
     MDI_double_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #4 doubleArray::Abstract_Binary_Operator_Non_Commutative (doubleArray,doubleArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #4 doubleArray::Abstract_Binary_Operator_Non_Commutative");
     Rhs.Test_Consistency("Test of Rhs in #4 doubleArray::Abstract_Binary_Operator_Non_Commutative");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     doubleArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Rhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Completed call to Build_New_Array_Or_Reuse_Operand() \n");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
             }
            else
             {
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
                    Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
                    Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
                  }
             }
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Do Where_Statement processing \n");
#endif

          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Call machine dependent function! \n");
#endif

          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) (
                 Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data ,
                 Rhs.Array_Descriptor.Array_Data , Mask_Array_Data , 
		 (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                 (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		 (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
               Delete_If_Temporary ( Lhs );
               Delete_If_Temporary ( Rhs );
             }
            else
             {
            // We use the same functions below but change the order 
            // of the Lhs and Rhs in the parameter list (and the order of the Base,Bound,etc)!
            // BUG FIX: (The first subtle error was that the Base,Bound,etc order was not swaped)
            // However we will leave the interface to the Machine_Dependent_Interface functions
            // such that these related functions are implemented once since we want to
            // limit the number of machine dependent functions (this enhances the portability).
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                      Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Rhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data) (
                      Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
                      Rhs.Array_Descriptor.Array_Data , Mask_Array_Data , 
		      (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Lhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Rhs );
                  }
             }
        } 
 
#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #4 doubleArray::Abstract_Binary_Operator_Non_Commutative");
#endif

     return Temporary_Array;
   }

// ******************************************************************
//                   ABSTRACT BINARY OPERATOR 
// ******************************************************************
doubleArray &
doubleArray::Abstract_Binary_Operator (
     const doubleArray & Lhs , double x ,
     MDI_double_Prototype_4 Operation_Array_Data ,
     MDI_double_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #5 doubleArray::Abstract_Binary_Operator (doubleArray,double) \n");

     Lhs.Test_Consistency("Test of Lhs in doubleArray::Abstract_Binary_Operator (doubleArray,double)");
#endif
 
     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     doubleArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_4 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
               Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
                    x , 
#else
                    &x , 
#endif
                    Mask_Array_Data , 
		    (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
		    (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Lhs );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) ( Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
                    x , 
#else
                    &x , 
#endif
                    Mask_Array_Data , (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
             }
        } 
#if 0
#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #5 doubleArray::Abstract_Binary_Operator");

     Temporary_Array.displayReferenceCounts("Called inside of #5 doubleArray::Abstract_Binary_Operator");
#endif
#endif

     return Temporary_Array;
   }

#if 1
// ******************************************************************
//                   ABSTRACT BINARY OPERATOR 
// ******************************************************************
doubleArray &
doubleArray::Abstract_Binary_Operator_Non_Commutative (
     const doubleArray & Lhs , double x,
     MDI_double_Prototype_4 Operation_Array_Data ,
     MDI_double_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #6 doubleArray::Abstract_Binary_Operator_Non_Commutative (double,doubleArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #6 doubleArray::Abstract_Binary_Operator_Non_Commutative (double,doubleArray)");
#endif

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     doubleArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_4 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
               Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , 
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

       // Don't delete the Lhs if it's data will be reused
       // Delete_If_Temporary ( Lhs );

       // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #6 doubleArray::Abstract_Binary_Operator_Non_Commutative");
#endif
 
     return Temporary_Array;
   }
#endif

// ******************************************************************
//              ABSTRACT OPERATOR ( +=, -=, *=, /=, %= )
//           Note that operator= does not use this function
// ******************************************************************
void
doubleArray::Abstract_Operator_Operation_Equals (
     const doubleArray & Lhs , const doubleArray & Rhs ,
     MDI_double_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #7 doubleArray::Abstract_Operator_Equals(doubleArray,doubleArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #7 doubleArray::Abstract_Operator_Equals (doubleArray,doubleArray)");
     Rhs.Test_Consistency("Test of Rhs in #7 doubleArray::Abstract_Operator_Equals (doubleArray,doubleArray)");
#endif

     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);
 
     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Lhs.Add_Defered_Expression ( new doubleArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


       /* The array class much check for the case of self reference,
       // if A += A then the operation is not skipped as in assignement.
       // But if a region of A is assigned to another region of A then
       // because the overlap might not be computed correctly (we cannot
       // specify the order of evaluation in the MDI functions).
       */
          if (Lhs.Array_ID() == Rhs.Array_ID())
             {
            // Possible case of A(I+1) (+,-,*,/,%)= A(I) which would not evaluate correctly 
            // because of the order of evaluation used in the MDI functions!

            // Make a copy of the Rhs to avoid self-reference.
            // The copy constructor also takes care of deleting the Rhs if is was
            // a temporary or view (so it should not be referenced beyond this point).
               doubleArray Rhs_Copy = Rhs;

            // Now just call the abstract operation function again with the Rhs_Copy parameters!
               doubleArray::Abstract_Operator_Operation_Equals ( Lhs , Rhs_Copy ,
                    Operation_Array_Data_Accumulate_To_Existing_Operand , Operation_Type );
             }
            else
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		(array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

               Delete_If_Temporary ( Rhs );
               Delete_Lhs_If_Temporary ( Lhs );
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency("Test of Lhs (FINAL RESULT) in #7 doubleArray::Abstract_Operator_Equals");
#endif
   }

// ******************************************************************
//              ABSTRACT OPERATOR ( +=, -=, *=, /=, %= )
//           Note that operator= does not use this function
// ******************************************************************
void
doubleArray::Abstract_Operator_Operation_Equals (
     const doubleArray & Lhs , double x ,
     MDI_double_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {   
  // This function is only called once for each Array statment!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #8 doubleArray::Abstract_Operator_Equals(doubleArray,double) \n");

     Lhs.Test_Consistency("Test of Lhs in #8 doubleArray::Abstract_Operator_Equals (doubleArray,double)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Lhs.Add_Defered_Expression ( new doubleArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif

 
#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data_Accumulate_To_Existing_Operand) ( Lhs.Array_Descriptor.Array_Data , 
#ifndef FORTRAN_MDI
                 x ,
#else
                 &x ,
#endif
                 Mask_Array_Data , (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
		 Mask_Descriptor );
#endif
          Delete_Lhs_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency("Test of Lhs (FINAL RESULT) in #8 doubleArray::Abstract_Operator_Equals");
#endif
   }

// ***********************************************************************
// ***********************************************************************
//              ABSTRACT OPERATORS RETURNING intArray
// ***********************************************************************
// ***********************************************************************

// ******************************************************************
//           ABSTRACT UNARY OPERATOR (logical function NOT)
//       The following function is required in the "operator!"
// ******************************************************************
intArray &
doubleArray::Abstract_Unary_Operator_Returning_IntArray ( const doubleArray & X ,
     MDI_double_Prototype_9  Operation_Array_Data ,
     MDI_double_Prototype_10 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
  /* In this fucntion we want to reuse the temporary (if X is a temporary)
  // but only if it is the correct size (the size of Array<int>), so we check to 
  // see the size of the Array<TYPE> and either try to reuse the temporary
  // (same as in the other runctions returning Array<TYPE>) or build a 
  // new return type Array<int>!
  */

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #9 doubleArray::Abstract_Unary_Operator_Returning_IntArray! \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }

  // This is the only test we can do on the input!
     X.Test_Consistency("Test of X in #9 doubleArray::Abstract_Unary_Operator_Returning_IntArray");
#endif
 
  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
  // intArray & Temporary_Array.Build_Temporary_By_Example (X);
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( 
               new doubleArray_Function_9 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
               (array_domain*) (&X.Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif
          Delete_If_Temporary ( X );
        }
 
  // printf ("Leaving Abstract Unary operator returning IntArray! \n");

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #9 doubleArray::Abstract_Unary_Operator_Returning_IntArray");
#endif

     return Temporary_Array;
   }

// ******************************************************************
//                     ABSTRACT BINARY OPERATOR
// ******************************************************************
intArray &
doubleArray::Abstract_Binary_Operator_Returning_IntArray (
     const doubleArray & Lhs , const doubleArray & Rhs ,
     MDI_double_Prototype_11 Operation_Array_Data ,
     MDI_double_Prototype_9  Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #10 doubleArray::Abstract_Binary_Operator_Returning_IntArray (doubleArray,doubleArray) \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }

     Lhs.Test_Consistency("Test of Lhs in doubleArray::Abstract_Binary_Operator_Returning_IntArray");
     Rhs.Test_Consistency("Test of Rhs in doubleArray::Abstract_Binary_Operator_Returning_IntArray");
#endif

  // avoid compiler warning with this useless statement (should be optimized out by compiler)
     if (Operation_Array_Data_Accumulate_To_Existing_Operand);

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (Lhs));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_11 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
                         Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
                         Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                         (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                         (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
                         (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) ,
                         Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
          Delete_If_Temporary ( Rhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #10 doubleArray::Abstract_Binary_Operator_Returning_IntArray");
#endif
 
     return Temporary_Array;
   }

// ******************************************************************
//                     ABSTRACT BINARY OPERATOR
// ******************************************************************
intArray &
doubleArray::Abstract_Binary_Operator_Returning_IntArray (
     const doubleArray & Lhs , double x ,
     MDI_double_Prototype_12 Operation_Array_Data ,
     MDI_double_Prototype_13 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #11 doubleArray::Abstract_Binary_Operator_Returning_IntArray (doubleArray,double) \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }
 
     Lhs.Test_Consistency("Inside of doubleArray::Abstract_Binary_Operator_Returning_IntArray (doubleArray,double)");
#endif

  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (Lhs));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( new doubleArray_Function_12 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #11 doubleArray::Abstract_Binary_Operator_Returning_IntArray");
#endif

     return Temporary_Array;
   }

// ***********************************************************************
// ***********************************************************************
// Abstract operators required for support of doubleArray::replace!
// ***********************************************************************
// ***********************************************************************

// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
doubleArray & doubleArray::Abstract_Modification_Operator ( const intArray & Lhs , const doubleArray & Rhs ,
     MDI_double_Prototype_6 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #12 doubleArray::Abstract_Modification_Operator (doubleArray,doubleArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #12 intArray::Abstract_Modification_Operator");
     Rhs.Test_Consistency("Test of Rhs in #12 doubleArray::Abstract_Modification_Operator");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new doubleArray_Function_6 ( Operation_Type , Operation_Array_Data , *this , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) 
	     (Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
              (array_domain*) (&Array_Descriptor.Array_Domain) , 
	      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
	      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
          Delete_If_Temporary ( Rhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #12 doubleArray::Abstract_Modification_Operator");
#endif

     return *this;
   }

// ******************************************************************
//                        ABSTRACT OPERATOR
//               (used for replace member functions)
// ******************************************************************
doubleArray & doubleArray::Abstract_Modification_Operator ( const intArray & Lhs , double x ,
     MDI_double_Prototype_7 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #13 doubleArray::Abstract_Modification_Operator (intArray,double) \n");
 
     Lhs.Test_Consistency("Inside of #13 doubleArray::Abstract_Modification_Operator (intArray,double)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new doubleArray_Function_7 ( Operation_Type , Operation_Array_Data , *this , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , (array_domain*) (&Array_Descriptor.Array_Domain) , 
	       (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
       // printf ("WARNING: Delete_If_Temporary ( Lhs ) not called in doubleArray::Abstract_Modification_Operator \n");
       // P++ uses the referenceCounting in the A++ objects to allow this delete to occur
       // and decrement the referenceCount but not remove the object.
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #13 doubleArray::Abstract_Modification_Operator");
#endif

     return *this;
   }

#if !defined(INTARRAY)
// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
doubleArray & doubleArray::Abstract_Modification_Operator ( const doubleArray & Lhs , int x ,
     MDI_double_Prototype_8 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #14 doubleArray::Abstract_Modification_Operator (doubleArray,int) \n");
 
     Lhs.Test_Consistency("Inside of #14 doubleArray::Abstract_Modification_Operator (doubleArray,int)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new doubleArray_Function_8 ( Operation_Type , Operation_Array_Data , *this , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , (array_domain*) (&Array_Descriptor.Array_Domain) , 
	       (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
        }
 
#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #14 doubleArray::Abstract_Modification_Operator");
#endif

     return *this;
   }
#endif


// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
double doubleArray::Abstract_Unary_Operator ( const doubleArray & X ,
     MDI_double_Prototype_14 Operation_Array_Data , int Operation_Type )
   { 
     double Return_Value = 0;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #15 Abstract_Operator (doubleArray) returning double \n");
 
     X.Test_Consistency("Test of X in #15 doubleArray::Abstract_Operator (doubleArray)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          int Avoid_Compiler_Warning = Operation_Type;
          printf ("ERROR: Misuse of Defered Evaluation -- Invalid use in reduction operator! \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          Return_Value = (*Operation_Array_Data)
	     ( X.Array_Descriptor.Array_Data , Mask_Array_Data ,
	       (array_domain*) (&X.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( X );
        }
 
     return Return_Value;
   }

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif




#if !defined(USE_EXPRESSION_TEMPLATES)

// *****************************************************************
// *****************************************************************
//                    operator= functions
// *****************************************************************
// *****************************************************************

// **********************************************************
// The equals operator taking a scalar
// **********************************************************
floatArray & 
floatArray::operator= ( float x )
   {
  // This prevents the annoying "feature" of M++ where a scalar is assigned to an array and the 
  // array object is set to be a 1x1x1x1 array.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floatArray::operator=! (float) (scalar value = %f)\n",double(x));
#endif
 
     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          floatArray_Function_5 *Execution_Object = new floatArray_Function_5 
	     ( Assignment , MDI_f_Assign_Array_Equals_Scalar_Accumulate_To_Operand ,
	       *this , x );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif

 
       /* The function name that we call here is slightly effected by the fact that we use
       // a marco processor to generate the machine dependent functions a better name would have
       // been "Assign_Array_Data_Equals_Scalar" but this would have not made for a good
       // name (after macro expansion) for the other operators (like +,-,*, and /).
       */
#if EXECUTE_MDI_FUNCTIONS
          MDI_f_Assign_Array_Equals_Scalar_Accumulate_To_Operand ( 
                         Array_Descriptor.Array_Data , 
#ifndef FORTRAN_MDI
                         x , 
#else
                         &x , 
#endif
                      Mask_Array_Data ,
                      (array_domain*) (&Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
 
       // Now we have to delete the "this" object if it is a View! But we are the this object
       // so we have to be very carefull how this is done!
          Delete_Lhs_If_Temporary ( *this );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in floatArray::operator=");
#endif

     return *this;
   }

// **********************************************************
// The other equals operator taking an array object
// **********************************************************
floatArray & 
floatArray::operator= ( const floatArray & Rhs )
   {
  // This function is only called once for each Array statment!

#if COMPILE_DEBUG_STATEMENTS
     if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
        {
          printf ("@@@@@ Inside of floatArray::operator=(const floatArray & Rhs) (id=%d) = (id=%d) \n",Array_ID(),Rhs.Array_ID());
          this->displayReferenceCounts("Lhs in floatArray & operator=(floatArray,floatArray)");
          Rhs.displayReferenceCounts("Rhs in floatArray & operator=(floatArray,floatArray)");
       }

     Test_Consistency("Test Lhs in floatArray::operator=");
     Rhs.Test_Consistency("Test Rhs in floatArray::operator=");
#endif

     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (isTemporary() == TRUE)
        {
          printf ("ERROR: Assignment to a temporary makes little sence so it has been ");
          printf ("disallowed! \n");
          printf ("       If you really feel you need this sort of functionality then ");
          printf ("let me know ... Dan Quinlan \n");
          APP_ABORT();
       }
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Check to see if the operator()() is used otherwise we do the assignment!
       // The function Binary_Conformable returns false if the "where" mask is present!
          if ( Binary_Conformable (Rhs) && Rhs.isTemporary() )
             {
               Add_Defered_Expression ( new floatArray_Function_Steal_Data ( Assignment , *this , Rhs ) );
             }
            else
             {
               Add_Defered_Expression ( new floatArray_Function_3 ( Assignment , MDI_f_Assign_Array_Equals_Array_Accumulate_To_Operand , *this , Rhs ) );
             }
        }
       else
        {
       // BUG FIX (July 29 1993; dquinlan)!
       // These can't be reference variables since they are then not initialized 
       // properly so they are implemented as pointers to pointers to the array data!  

          float** Lhs_Data_Pointer = &(Array_Descriptor.Array_Data);
          float** Rhs_Data_Pointer = &(((floatArray &)Rhs).Array_Descriptor.Array_Data);

          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


       // Check to see if one of the operator()() is used otherwise we do the 
       // assignment!  The function Binary_Conformable returns false if the "where" 
       // mask is present! This works independent of the reference counting (I think)!

          if ( (getRawDataReferenceCount() == getRawDataReferenceCountBase()) &&
                Binary_Conformable (Rhs) && 
                Rhs.isTemporary() &&
               !Rhs.isView() )
             {
            // Give back the original Array_Data memory (but only if it exists)!
            // But it should always exist so we don't really have to check unless it is 
	    // a Null array!

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Skip the assignment by stealing the data! \n");
#endif

            /*
            // ... bug fix (8/26/96, kdb) this was previously called later
            // after deleteing the Array_Descriptor but because the data
            // wasn't deleted, the reference count in
            // Array_Reference_Count_Array[Array_Id] was too high and so
            // the Array_ID wasn't put back onto the list ...
            */
               Delete_Array_Data ();

               if (isNullArray())
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("Lhs is a Null Array so copy descriptor before assignment! \n");
#endif

                 // Bug Fix (8/17/94) Even a Null array must have a properly computed array descriptor
                    APP_ASSERT(isTemporary() == FALSE);

                 // We know it is a temporary because we only try to steal temporaries
                 // so check to temporaries that are a view -- but we don't allow views
                 // so it CAN'T be a view.
                    APP_ASSERT(Rhs.isView() == FALSE);

                 // We have to reset the reference count since the operator= used in the Build_Temporary_By_Example function
                 // will get trapped in the test_Consistancy test if we don't.
                    resetRawDataReferenceCount();
                    Array_Descriptor.Build_Temporary_By_Example (Rhs.Array_Descriptor);

                 // This is redundent since the base of the temporary is already set to ZERO!
                 // setBase(0);
                    APP_ASSERT (getBase() == 0);

                 // This is no longer going to be a temporary so mark it as a non temporary
                 // Array_Descriptor.Array_Domain.Is_A_Temporary = FALSE;
                    setTemporary(FALSE);

                 // Double check to make sure it is not a view
                    APP_ASSERT( isView() == FALSE );
                  }

            // ... bug fix (8/26/96,kdb) see comment above ...
            // Delete_Array_Data ();
               *Lhs_Data_Pointer = NULL;

            // BUG FIX: (18/1/94) fixed reference counts.
            // Reset the reference count to zero as we steal the data because the 
            // new data should have a reference count of zero (not -1 as would be 
            // the case if it had been deleted)! setting it to ZERO means there are
            // NO (ZERO) external references to this data (only the one by this array 
            // object).

            // I don't want to provide a means to reset or assign specific reference 
            // counts (dangerous I think)

            // Steal the pointer (to the original array) from the Rhs's data
            // Note that these are REFERENCE variables!

               *Lhs_Data_Pointer = *Rhs_Data_Pointer;
            // ... change (8/21/96,kdb) Add eight view pointers ...
            // Array_View_Pointer = Array_Descriptor.Array_Data + Array_Descriptor->Scalar_Offset;

               POINTER_LIST_INITIALIZATION_MACRO;

            // Since the previous copy of the array data was deleted the reference count 
	    // of the Raw Data is not -1 (typically) so we have to reset it.  We could 
            // just increment it -- but error checking within the 
            // incrementRawDataReferenceCount would trap out the case where the
            // reference count of the raw data is less than ZERO.  So we have a special 
            // inline function to force the reset value to ZERO.

               resetRawDataReferenceCount();

            // We can set the correct one by using the fact that 
            // Rhs_Data_Pointer is a pointer to a pointer to the array data!

            // ... (bug fix, 6/27/96, kdb) don't set this pointer to null here
            // because otherwise the array won't be deleted.  Also increment
            // the reference count for Rhs so Array_Data won't be deleted ...
            // *Rhs_Data_Pointer = NULL;

            // printf ("In operator=: Calling Rhs.incrementRawDataReferenceCount() \n");
               Rhs.incrementRawDataReferenceCount();

            /*
            // ... the Array_Descriptor will be deleted but because the
	    // Array_Reference_Count_Array was just incremented the array ID
            // won't be put back in the list like it should be.  Before
            // deleting, force the array ID onto the stack.  The 
            // Array_Reference_Count_Array[Array_ID] will already be 0 after
            // the delete.  (This problem comes up because of the assumption
            // that the only time 2 arrays share Array_Data is when one is a
            // view of another and so has the same Array_ID) ...
            */

            // Bugfix (11/6/2000) Deleting the array should put it back onto 
            // the stack of array ID for future use (I think).
            // Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(Rhs.Array_ID());

            // get the array id of the Rhs before it is deleted
               int rhsArrayID = Rhs.Array_ID();

            // The Rhs will be deleted but until then we can't really call any
            // of it's member functions since there is no data associated with it.
            // printf ("In floatArray::operator= calling Delete_If_Temporary ( Rhs ) \n");
               Delete_If_Temporary ( Rhs );

            // Bugfix (11/7/2000) 
            // Move to after the call to Delete_If_Temporary() since that function 
            // calls the Test_Consistency function which will report an error if the 
            // array ID is pushed onto the stack while it is a valid array ID still in use.
            // Bugfix (11/6/2000) Deleting the array should put it back onto 
            // the stack of array ID for future use (I think).
            // Bugfix (11/11/2000) accessing data after it is deleted
            // Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(Rhs.Array_ID());
               Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(rhsArrayID);

            // Bugfix (12/6/2000) Also reset the reference count associated with this id value!
               Array_Domain_Type::Array_Reference_Count_Array [rhsArrayID] = getRawDataReferenceCountBase() - 1;

            // Now we have to delete the "this" object if it is a View! But we are the 
            // this object so we have to be very carefull how this is done!
            // printf ("In floatArray::operator= calling Delete_Lhs_If_Temporary ( *this ) \n");
               Delete_Lhs_If_Temporary ( *this );
             }
            else // Do the assignment the hard way (element by element)!
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Do the assignment by calling the MDI function! \n");
#endif
            /* Call Assignment function
            // The function name that we call here is slightly effected by the fact that 
	    // we use a marco processor to generate the machine dependent functions a 
            // better name would have been "Assign_Array_Data_Equals_Scalar" but this 
            // would have not made for a good name (after macro expansion) for the other 
            // operators (like +,-,*, and /).
            */

            /* The array class must check for the case of self assignment,
            // if A=A then the assignement is skiped (using the logic below).
            // But if a region of A is assigned to another region of A then
            // because the overlap might not be computed correctly (we cannot
            // specify the order of evaluation in the MDI functions).
            */
               if (Array_ID() == Rhs.Array_ID())
                  {
                 // Possible case of A(I+1) = A(I) which would not evaluate correctly 
                 // because of the order of evaluation used in the MDI functions!
     
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("CASE: Array_ID() == Rhs.Array_ID() \n");
	               }
#endif
                    if ( !(isContiguousData() && Rhs.isContiguousData()) )
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                            printf ("Make a copy \n");
#endif

                      // Rhs.displayReferenceCounts("In operator=(floatArray) BEFORE COPY: Rhs");

                      // Make a copy
                         floatArray Rhs_Copy = Rhs;

                      // Rhs.displayReferenceCounts("In operator=(floatArray) AFTER COPY: Rhs");
                      // Rhs_Copy.displayReferenceCounts("In operator=(floatArray) AFTER COPY: Rhs_Copy");

                      // Now just call the operator= again (since Lhs and Rhs have different 
	              // array ids)!
                         *this = Rhs_Copy;
                       }
                      else
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                              printf ("CASE: A(I) = A(I) so skip the assignment operation! \n");
#endif
                      // We can skip the assignment of A(I) = A(I) but we have to delete the 
	              // Rhs view!
                         Delete_If_Temporary ( Rhs );
                       }
                  }
                 else
                  {
                 // Bugfix (3/29/95) fixed null array operations between Lhs and Rhs Null 
	         // array objects.  This fix is required for P++ to work properly on 
	         // processors where neither the Lhs of Rhs have an representation on the 
                 // local processor.
                    if (isNullArray() && !isView())
                       {
                      // Bug fix (8/9/94) need to build new descriptor to the correct size 
	              // of the Rhs
                      // APP_ASSERT( Array_Descriptor != NULL );

                      // This is much simpler thant what we have below and it allows the 
	              // bases of the Rhs to be preserved in the Lhs (which was not 
	              // previously true).
                         redim (Rhs);
                      // ... change (8/21/96,kdb) add 8 view pointers ...
	                 POINTER_LIST_INITIALIZATION_MACRO;
                       }

                    APP_ASSERT(isTemporary() == FALSE);

#if EXECUTE_MDI_FUNCTIONS
                    MDI_f_Assign_Array_Equals_Array_Accumulate_To_Operand
                       ( *Lhs_Data_Pointer , *Rhs_Data_Pointer , Mask_Array_Data ,
                         (array_domain*) (&Array_Descriptor.Array_Domain) , 
                         (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("Calling Delete_If_Temporary(Rhs) \n");
	               }
#endif
                 // If A(I) = A(I-1) then the view Rhs was handled by the copy constructor!
                    Delete_If_Temporary ( Rhs );

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("Calling Delete_Lhs_If_Temporary(*this) \n");
	               }
#endif
                 // Now we have to delete the "this" object if it is a View! But we are 
                 // the this object so we have to be very carefull how this is done!
                 // This function will be removed soon to streamline the operator= 
                 // member function
                    APP_ASSERT(isTemporary() == FALSE);
                    Delete_Lhs_If_Temporary ( *this );
                  }
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in floatArray::operator=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("Leaving operator= (floatArray) \n");
        }
#endif
  // operator= does the assignment and then returns a floatArray
     return *this;
   }

// ******************************************************************
// ******************************************************************
//  Abstract Operators (with masking support for "where" statements)
// ******************************************************************
// ******************************************************************

/* These operators support the efficient handling of the Temporaries and Views 
// which are a part of the Array class.  The details of the manipulation are
// isolated into a few abstract operators below which take function pointers
// to make calls specific to the requirements of operator+, operator-, operator*,
// operator/, and other related operators.  This greatly simplifies the 
// implementation of the actual operators for +,-,*, and /.  It also
// isolates the mantainence of the code!  The functions that are used
// as parameters define the machine dependent interface.
// Each input function is handed a pointer to the Mask used in the support of
// the "where" statement that is a part of this array class library.  It is 
// the job of the input function (the machine dependent function) to
// recognize if the Mask pointer is valid (in FORTRAN the Mask_Stride != 0 for a 
// valid mask) and then use the mask to correctly compute the specific fuction.
*/

// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
floatArray &
floatArray::Abstract_Unary_Operator (
   const floatArray & X ,
   MDI_float_Prototype_0 Operation_Array_Data ,
   MDI_float_Prototype_1 Operation_Array_Data_Accumulate_To_Existing_Operand ,
   int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #1 floatArray::Abstract_Unary_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif
 
     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     floatArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand ( X , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY)
             {
               Temporary_Array.Add_Defered_Expression ( new floatArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND);
               Temporary_Array.Add_Defered_Expression ( new floatArray_Function_1 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , X ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY)
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) (
                    Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
                    (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
		    (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND);
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                    X.Array_Descriptor.Array_Data , Mask_Array_Data , 
		    (array_domain*) (&X.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif
            // Bugfix (12/4/2000) delete the temprary to be uniformally consistant with rest of code
               Delete_If_Temporary ( X );
             }
        } 

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #1 floatArray::Abstract_Unary_Operator");

     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Temporary_Array.displayReferenceCounts("Temporary_Array in #1 floatArray::Abstract_Unary_Operator");
        }
#endif

     return Temporary_Array;
   }



// Support for Conversion Operators
#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
intArray &
floatArray::Abstract_int_Conversion_Operator ( 
   const floatArray & X ,
   MDI_float_Prototype_convertTo_intArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & floatArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new floatArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the floatArray::Abstract_int_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 floatArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif

#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
floatArray &
floatArray::Abstract_float_Conversion_Operator ( 
   const floatArray & X ,
   MDI_float_Prototype_convertTo_floatArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & floatArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     floatArray & Temporary_Array = *(floatArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new floatArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the floatArray::Abstract_float_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 floatArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif

#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
doubleArray &
floatArray::Abstract_double_Conversion_Operator ( 
   const floatArray & X ,
   MDI_float_Prototype_convertTo_doubleArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & floatArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     doubleArray & Temporary_Array = *(doubleArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new floatArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the floatArray::Abstract_double_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 floatArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif


// ******************************************************************
//                   ABSTRACT BINARY OPERATOR
// ******************************************************************
floatArray &
floatArray::Abstract_Binary_Operator ( const floatArray & Lhs , const floatArray & Rhs ,
   MDI_float_Prototype_2 Operation_Array_Data ,
   MDI_float_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
   int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #3 floatArray::Abstract_Binary_Operator (floatArray,floatArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #3 floatArray::Abstract_Binary_Operator");
     Rhs.Test_Consistency("Test of Rhs in #3 floatArray::Abstract_Binary_Operator");

  // Lhs.view("Lhs in (AT START OF FUNCTION) floatArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AT START OF FUNCTION) floatArray::Abstract_Binary_Operator");

  // Lhs.displayReferenceCounts("Lhs at TOP of #3 floatArray::Abstract_Binary_Operator()");
  // Rhs.displayReferenceCounts("Rhs at TOP of #3 floatArray::Abstract_Binary_Operator()");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;

#if COMPILE_DEBUG_STATEMENTS
  // Lhs.view("Lhs in (BEFORE SETUP OF TEMPORARY ARRAY) floatArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (BEFORE SETUP OF TEMPORARY ARRAY) floatArray::Abstract_Binary_Operator");
#endif

     floatArray & Temporary_Array = 
          Build_New_Array_Or_Reuse_Operand ( Lhs , Rhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

#if COMPILE_DEBUG_STATEMENTS
  // Lhs.view("Lhs in (AFTER SETUP OF TEMPORARY ARRAY) floatArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AFTER SETUP OF TEMPORARY ARRAY) floatArray::Abstract_Binary_Operator");
     Temporary_Array.Test_Consistency("Test of Temporary_Array in #3 floatArray::Abstract_Binary_Operator");
  // Temporary_Array.view("Temporary_Array in floatArray::Abstract_Binary_Operator");
  // Lhs.view("Lhs in (AFTER SETUP OF TEMPORARY ARRAY AND TEST_CONSISTENCY) floatArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AFTER SETUP OF TEMPORARY ARRAY AND TEST_CONSISTENCY) floatArray::Abstract_Binary_Operator");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new floatArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
             }
            else
             {
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
                    Temporary_Array.Add_Defered_Expression ( new floatArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
                    Temporary_Array.Add_Defered_Expression ( new floatArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Rhs , Lhs ) );
                  }
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Call machine dependent function! \n");

       // Temp code not valid for general use!
       // APP_ASSERT(Temporary_Array.Array_Data != NULL);
       // APP_ASSERT(Lhs.Array_Data != NULL);
       // APP_ASSERT(Rhs.Array_Data != NULL);
#endif

          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) 
		  (Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , 
		   Mask_Array_Data , 
		   (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
		   (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
                   (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
            // If we had to build the temporary using NEWLY_ALLOCATED_MEMORY then neither of the
            // input arrays where a temporary (which could be reused) -- so we don't have to
            // delete if temporary since we know that neither could be.  So the following
            // function calls can be eliminated.

            // printf ("Now call delete if temporary for Lhs and Rhs! \n");

            // Lhs.displayReferenceCounts("Lhs case NEWLY_ALLOCATED_MEMORY in #3 floatArray::Abstract_Binary_Operator()");
            // Rhs.displayReferenceCounts("Rhs case NEWLY_ALLOCATED_MEMORY in #3 floatArray::Abstract_Binary_Operator()");

               Delete_If_Temporary ( Lhs );
               Delete_If_Temporary ( Rhs );
             }
            else
             {
            // We use the same functions below but change the order 
            // of the Lhs and Rhs in the parameter list (and the order of the Base,Bound,etc)!
            // BUG FIX: (The first subtle error was that the Base,Bound,etc order was not swaped)
            // However we will leave the interface to the Machine_Dependent_Interface functions
            // such that these related functions are implemented once since we want to
            // limit the number of machine dependent functions (this enhances the portability).
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                     Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                     (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		     (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , 
		     Mask_Descriptor );
#endif
                 // Rhs.displayReferenceCounts("Rhs case MEMORY_FROM_LHS in #3 floatArray::Abstract_Binary_Operator()");

                    Delete_If_Temporary ( Rhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS);
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                      Rhs.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                 // Lhs.displayReferenceCounts("Lhs case MEMORY_FROM_RHS in #3 floatArray::Abstract_Binary_Operator()");

                 // Since we got the temporary from the Rhs the rule is that we first looked
                 // at the Lhs (and took it if it was a temporary) -- so the Lhs is NOT a 
                 // temporary.  So we can eliminate the function call to delete the Lhs.
                    Delete_If_Temporary ( Lhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Rhs );
                  }
             }
        } 

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #3 floatArray::Abstract_Binary_Operator");
#endif
 
     return Temporary_Array;
   }

// ******************************************************************
//           ABSTRACT BINARY OPERATOR (NON COMMUTATIVE CASE)
// ******************************************************************
floatArray &
floatArray::Abstract_Binary_Operator_Non_Commutative ( 
     const floatArray & Lhs , const floatArray & Rhs ,
     MDI_float_Prototype_2 Operation_Array_Data ,
     MDI_float_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #4 floatArray::Abstract_Binary_Operator_Non_Commutative (floatArray,floatArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #4 floatArray::Abstract_Binary_Operator_Non_Commutative");
     Rhs.Test_Consistency("Test of Rhs in #4 floatArray::Abstract_Binary_Operator_Non_Commutative");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     floatArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Rhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Completed call to Build_New_Array_Or_Reuse_Operand() \n");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new floatArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
             }
            else
             {
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
                    Temporary_Array.Add_Defered_Expression ( new floatArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
                    Temporary_Array.Add_Defered_Expression ( new floatArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
                  }
             }
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Do Where_Statement processing \n");
#endif

          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Call machine dependent function! \n");
#endif

          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) (
                 Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data ,
                 Rhs.Array_Descriptor.Array_Data , Mask_Array_Data , 
		 (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                 (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		 (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
               Delete_If_Temporary ( Lhs );
               Delete_If_Temporary ( Rhs );
             }
            else
             {
            // We use the same functions below but change the order 
            // of the Lhs and Rhs in the parameter list (and the order of the Base,Bound,etc)!
            // BUG FIX: (The first subtle error was that the Base,Bound,etc order was not swaped)
            // However we will leave the interface to the Machine_Dependent_Interface functions
            // such that these related functions are implemented once since we want to
            // limit the number of machine dependent functions (this enhances the portability).
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                      Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Rhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data) (
                      Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
                      Rhs.Array_Descriptor.Array_Data , Mask_Array_Data , 
		      (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Lhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Rhs );
                  }
             }
        } 
 
#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #4 floatArray::Abstract_Binary_Operator_Non_Commutative");
#endif

     return Temporary_Array;
   }

// ******************************************************************
//                   ABSTRACT BINARY OPERATOR 
// ******************************************************************
floatArray &
floatArray::Abstract_Binary_Operator (
     const floatArray & Lhs , float x ,
     MDI_float_Prototype_4 Operation_Array_Data ,
     MDI_float_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #5 floatArray::Abstract_Binary_Operator (floatArray,float) \n");

     Lhs.Test_Consistency("Test of Lhs in floatArray::Abstract_Binary_Operator (floatArray,float)");
#endif
 
     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     floatArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new floatArray_Function_4 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
               Temporary_Array.Add_Defered_Expression ( new floatArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
                    x , 
#else
                    &x , 
#endif
                    Mask_Array_Data , 
		    (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
		    (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Lhs );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) ( Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
                    x , 
#else
                    &x , 
#endif
                    Mask_Array_Data , (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
             }
        } 
#if 0
#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #5 floatArray::Abstract_Binary_Operator");

     Temporary_Array.displayReferenceCounts("Called inside of #5 floatArray::Abstract_Binary_Operator");
#endif
#endif

     return Temporary_Array;
   }

#if 1
// ******************************************************************
//                   ABSTRACT BINARY OPERATOR 
// ******************************************************************
floatArray &
floatArray::Abstract_Binary_Operator_Non_Commutative (
     const floatArray & Lhs , float x,
     MDI_float_Prototype_4 Operation_Array_Data ,
     MDI_float_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #6 floatArray::Abstract_Binary_Operator_Non_Commutative (float,floatArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #6 floatArray::Abstract_Binary_Operator_Non_Commutative (float,floatArray)");
#endif

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     floatArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new floatArray_Function_4 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
               Temporary_Array.Add_Defered_Expression ( new floatArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , 
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

       // Don't delete the Lhs if it's data will be reused
       // Delete_If_Temporary ( Lhs );

       // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #6 floatArray::Abstract_Binary_Operator_Non_Commutative");
#endif
 
     return Temporary_Array;
   }
#endif

// ******************************************************************
//              ABSTRACT OPERATOR ( +=, -=, *=, /=, %= )
//           Note that operator= does not use this function
// ******************************************************************
void
floatArray::Abstract_Operator_Operation_Equals (
     const floatArray & Lhs , const floatArray & Rhs ,
     MDI_float_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #7 floatArray::Abstract_Operator_Equals(floatArray,floatArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #7 floatArray::Abstract_Operator_Equals (floatArray,floatArray)");
     Rhs.Test_Consistency("Test of Rhs in #7 floatArray::Abstract_Operator_Equals (floatArray,floatArray)");
#endif

     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);
 
     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Lhs.Add_Defered_Expression ( new floatArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


       /* The array class much check for the case of self reference,
       // if A += A then the operation is not skipped as in assignement.
       // But if a region of A is assigned to another region of A then
       // because the overlap might not be computed correctly (we cannot
       // specify the order of evaluation in the MDI functions).
       */
          if (Lhs.Array_ID() == Rhs.Array_ID())
             {
            // Possible case of A(I+1) (+,-,*,/,%)= A(I) which would not evaluate correctly 
            // because of the order of evaluation used in the MDI functions!

            // Make a copy of the Rhs to avoid self-reference.
            // The copy constructor also takes care of deleting the Rhs if is was
            // a temporary or view (so it should not be referenced beyond this point).
               floatArray Rhs_Copy = Rhs;

            // Now just call the abstract operation function again with the Rhs_Copy parameters!
               floatArray::Abstract_Operator_Operation_Equals ( Lhs , Rhs_Copy ,
                    Operation_Array_Data_Accumulate_To_Existing_Operand , Operation_Type );
             }
            else
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		(array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

               Delete_If_Temporary ( Rhs );
               Delete_Lhs_If_Temporary ( Lhs );
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency("Test of Lhs (FINAL RESULT) in #7 floatArray::Abstract_Operator_Equals");
#endif
   }

// ******************************************************************
//              ABSTRACT OPERATOR ( +=, -=, *=, /=, %= )
//           Note that operator= does not use this function
// ******************************************************************
void
floatArray::Abstract_Operator_Operation_Equals (
     const floatArray & Lhs , float x ,
     MDI_float_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {   
  // This function is only called once for each Array statment!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #8 floatArray::Abstract_Operator_Equals(floatArray,float) \n");

     Lhs.Test_Consistency("Test of Lhs in #8 floatArray::Abstract_Operator_Equals (floatArray,float)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Lhs.Add_Defered_Expression ( new floatArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif

 
#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data_Accumulate_To_Existing_Operand) ( Lhs.Array_Descriptor.Array_Data , 
#ifndef FORTRAN_MDI
                 x ,
#else
                 &x ,
#endif
                 Mask_Array_Data , (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
		 Mask_Descriptor );
#endif
          Delete_Lhs_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency("Test of Lhs (FINAL RESULT) in #8 floatArray::Abstract_Operator_Equals");
#endif
   }

// ***********************************************************************
// ***********************************************************************
//              ABSTRACT OPERATORS RETURNING intArray
// ***********************************************************************
// ***********************************************************************

// ******************************************************************
//           ABSTRACT UNARY OPERATOR (logical function NOT)
//       The following function is required in the "operator!"
// ******************************************************************
intArray &
floatArray::Abstract_Unary_Operator_Returning_IntArray ( const floatArray & X ,
     MDI_float_Prototype_9  Operation_Array_Data ,
     MDI_float_Prototype_10 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
  /* In this fucntion we want to reuse the temporary (if X is a temporary)
  // but only if it is the correct size (the size of Array<int>), so we check to 
  // see the size of the Array<TYPE> and either try to reuse the temporary
  // (same as in the other runctions returning Array<TYPE>) or build a 
  // new return type Array<int>!
  */

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #9 floatArray::Abstract_Unary_Operator_Returning_IntArray! \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }

  // This is the only test we can do on the input!
     X.Test_Consistency("Test of X in #9 floatArray::Abstract_Unary_Operator_Returning_IntArray");
#endif
 
  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
  // intArray & Temporary_Array.Build_Temporary_By_Example (X);
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( 
               new floatArray_Function_9 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
               (array_domain*) (&X.Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif
          Delete_If_Temporary ( X );
        }
 
  // printf ("Leaving Abstract Unary operator returning IntArray! \n");

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #9 floatArray::Abstract_Unary_Operator_Returning_IntArray");
#endif

     return Temporary_Array;
   }

// ******************************************************************
//                     ABSTRACT BINARY OPERATOR
// ******************************************************************
intArray &
floatArray::Abstract_Binary_Operator_Returning_IntArray (
     const floatArray & Lhs , const floatArray & Rhs ,
     MDI_float_Prototype_11 Operation_Array_Data ,
     MDI_float_Prototype_9  Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #10 floatArray::Abstract_Binary_Operator_Returning_IntArray (floatArray,floatArray) \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }

     Lhs.Test_Consistency("Test of Lhs in floatArray::Abstract_Binary_Operator_Returning_IntArray");
     Rhs.Test_Consistency("Test of Rhs in floatArray::Abstract_Binary_Operator_Returning_IntArray");
#endif

  // avoid compiler warning with this useless statement (should be optimized out by compiler)
     if (Operation_Array_Data_Accumulate_To_Existing_Operand);

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (Lhs));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( new floatArray_Function_11 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
                         Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
                         Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                         (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                         (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
                         (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) ,
                         Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
          Delete_If_Temporary ( Rhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #10 floatArray::Abstract_Binary_Operator_Returning_IntArray");
#endif
 
     return Temporary_Array;
   }

// ******************************************************************
//                     ABSTRACT BINARY OPERATOR
// ******************************************************************
intArray &
floatArray::Abstract_Binary_Operator_Returning_IntArray (
     const floatArray & Lhs , float x ,
     MDI_float_Prototype_12 Operation_Array_Data ,
     MDI_float_Prototype_13 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #11 floatArray::Abstract_Binary_Operator_Returning_IntArray (floatArray,float) \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }
 
     Lhs.Test_Consistency("Inside of floatArray::Abstract_Binary_Operator_Returning_IntArray (floatArray,float)");
#endif

  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (Lhs));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( new floatArray_Function_12 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #11 floatArray::Abstract_Binary_Operator_Returning_IntArray");
#endif

     return Temporary_Array;
   }

// ***********************************************************************
// ***********************************************************************
// Abstract operators required for support of floatArray::replace!
// ***********************************************************************
// ***********************************************************************

// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
floatArray & floatArray::Abstract_Modification_Operator ( const intArray & Lhs , const floatArray & Rhs ,
     MDI_float_Prototype_6 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #12 floatArray::Abstract_Modification_Operator (floatArray,floatArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #12 intArray::Abstract_Modification_Operator");
     Rhs.Test_Consistency("Test of Rhs in #12 floatArray::Abstract_Modification_Operator");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new floatArray_Function_6 ( Operation_Type , Operation_Array_Data , *this , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) 
	     (Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
              (array_domain*) (&Array_Descriptor.Array_Domain) , 
	      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
	      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
          Delete_If_Temporary ( Rhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #12 floatArray::Abstract_Modification_Operator");
#endif

     return *this;
   }

// ******************************************************************
//                        ABSTRACT OPERATOR
//               (used for replace member functions)
// ******************************************************************
floatArray & floatArray::Abstract_Modification_Operator ( const intArray & Lhs , float x ,
     MDI_float_Prototype_7 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #13 floatArray::Abstract_Modification_Operator (intArray,float) \n");
 
     Lhs.Test_Consistency("Inside of #13 floatArray::Abstract_Modification_Operator (intArray,float)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new floatArray_Function_7 ( Operation_Type , Operation_Array_Data , *this , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , (array_domain*) (&Array_Descriptor.Array_Domain) , 
	       (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
       // printf ("WARNING: Delete_If_Temporary ( Lhs ) not called in floatArray::Abstract_Modification_Operator \n");
       // P++ uses the referenceCounting in the A++ objects to allow this delete to occur
       // and decrement the referenceCount but not remove the object.
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #13 floatArray::Abstract_Modification_Operator");
#endif

     return *this;
   }

#if !defined(INTARRAY)
// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
floatArray & floatArray::Abstract_Modification_Operator ( const floatArray & Lhs , int x ,
     MDI_float_Prototype_8 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #14 floatArray::Abstract_Modification_Operator (floatArray,int) \n");
 
     Lhs.Test_Consistency("Inside of #14 floatArray::Abstract_Modification_Operator (floatArray,int)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new floatArray_Function_8 ( Operation_Type , Operation_Array_Data , *this , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , (array_domain*) (&Array_Descriptor.Array_Domain) , 
	       (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
        }
 
#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #14 floatArray::Abstract_Modification_Operator");
#endif

     return *this;
   }
#endif


// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
float floatArray::Abstract_Unary_Operator ( const floatArray & X ,
     MDI_float_Prototype_14 Operation_Array_Data , int Operation_Type )
   { 
     float Return_Value = 0;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #15 Abstract_Operator (floatArray) returning float \n");
 
     X.Test_Consistency("Test of X in #15 floatArray::Abstract_Operator (floatArray)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          int Avoid_Compiler_Warning = Operation_Type;
          printf ("ERROR: Misuse of Defered Evaluation -- Invalid use in reduction operator! \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          Return_Value = (*Operation_Array_Data)
	     ( X.Array_Descriptor.Array_Data , Mask_Array_Data ,
	       (array_domain*) (&X.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( X );
        }
 
     return Return_Value;
   }

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif



#define INTARRAY


#if !defined(USE_EXPRESSION_TEMPLATES)

// *****************************************************************
// *****************************************************************
//                    operator= functions
// *****************************************************************
// *****************************************************************

// **********************************************************
// The equals operator taking a scalar
// **********************************************************
intArray & 
intArray::operator= ( int x )
   {
  // This prevents the annoying "feature" of M++ where a scalar is assigned to an array and the 
  // array object is set to be a 1x1x1x1 array.

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray::operator=! (int) (scalar value = %f)\n",double(x));
#endif
 
     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          intArray_Function_5 *Execution_Object = new intArray_Function_5 
	     ( Assignment , MDI_i_Assign_Array_Equals_Scalar_Accumulate_To_Operand ,
	       *this , x );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif

 
       /* The function name that we call here is slightly effected by the fact that we use
       // a marco processor to generate the machine dependent functions a better name would have
       // been "Assign_Array_Data_Equals_Scalar" but this would have not made for a good
       // name (after macro expansion) for the other operators (like +,-,*, and /).
       */
#if EXECUTE_MDI_FUNCTIONS
          MDI_i_Assign_Array_Equals_Scalar_Accumulate_To_Operand ( 
                         Array_Descriptor.Array_Data , 
#ifndef FORTRAN_MDI
                         x , 
#else
                         &x , 
#endif
                      Mask_Array_Data ,
                      (array_domain*) (&Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
 
       // Now we have to delete the "this" object if it is a View! But we are the this object
       // so we have to be very carefull how this is done!
          Delete_Lhs_If_Temporary ( *this );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in intArray::operator=");
#endif

     return *this;
   }

// **********************************************************
// The other equals operator taking an array object
// **********************************************************
intArray & 
intArray::operator= ( const intArray & Rhs )
   {
  // This function is only called once for each Array statment!

#if COMPILE_DEBUG_STATEMENTS
     if ( (APP_DEBUG > 0) || (Diagnostic_Manager::getReferenceCountingReport() > 0) )
        {
          printf ("@@@@@ Inside of intArray::operator=(const intArray & Rhs) (id=%d) = (id=%d) \n",Array_ID(),Rhs.Array_ID());
          this->displayReferenceCounts("Lhs in intArray & operator=(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator=(intArray,intArray)");
       }

     Test_Consistency("Test Lhs in intArray::operator=");
     Rhs.Test_Consistency("Test Rhs in intArray::operator=");
#endif

     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (isTemporary() == TRUE)
        {
          printf ("ERROR: Assignment to a temporary makes little sence so it has been ");
          printf ("disallowed! \n");
          printf ("       If you really feel you need this sort of functionality then ");
          printf ("let me know ... Dan Quinlan \n");
          APP_ABORT();
       }
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Check to see if the operator()() is used otherwise we do the assignment!
       // The function Binary_Conformable returns false if the "where" mask is present!
          if ( Binary_Conformable (Rhs) && Rhs.isTemporary() )
             {
               Add_Defered_Expression ( new intArray_Function_Steal_Data ( Assignment , *this , Rhs ) );
             }
            else
             {
               Add_Defered_Expression ( new intArray_Function_3 ( Assignment , MDI_i_Assign_Array_Equals_Array_Accumulate_To_Operand , *this , Rhs ) );
             }
        }
       else
        {
       // BUG FIX (July 29 1993; dquinlan)!
       // These can't be reference variables since they are then not initialized 
       // properly so they are implemented as pointers to pointers to the array data!  

          int** Lhs_Data_Pointer = &(Array_Descriptor.Array_Data);
          int** Rhs_Data_Pointer = &(((intArray &)Rhs).Array_Descriptor.Array_Data);

          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


       // Check to see if one of the operator()() is used otherwise we do the 
       // assignment!  The function Binary_Conformable returns false if the "where" 
       // mask is present! This works independent of the reference counting (I think)!

          if ( (getRawDataReferenceCount() == getRawDataReferenceCountBase()) &&
                Binary_Conformable (Rhs) && 
                Rhs.isTemporary() &&
               !Rhs.isView() )
             {
            // Give back the original Array_Data memory (but only if it exists)!
            // But it should always exist so we don't really have to check unless it is 
	    // a Null array!

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Skip the assignment by stealing the data! \n");
#endif

            /*
            // ... bug fix (8/26/96, kdb) this was previously called later
            // after deleteing the Array_Descriptor but because the data
            // wasn't deleted, the reference count in
            // Array_Reference_Count_Array[Array_Id] was too high and so
            // the Array_ID wasn't put back onto the list ...
            */
               Delete_Array_Data ();

               if (isNullArray())
                  {
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
                         printf ("Lhs is a Null Array so copy descriptor before assignment! \n");
#endif

                 // Bug Fix (8/17/94) Even a Null array must have a properly computed array descriptor
                    APP_ASSERT(isTemporary() == FALSE);

                 // We know it is a temporary because we only try to steal temporaries
                 // so check to temporaries that are a view -- but we don't allow views
                 // so it CAN'T be a view.
                    APP_ASSERT(Rhs.isView() == FALSE);

                 // We have to reset the reference count since the operator= used in the Build_Temporary_By_Example function
                 // will get trapped in the test_Consistancy test if we don't.
                    resetRawDataReferenceCount();
                    Array_Descriptor.Build_Temporary_By_Example (Rhs.Array_Descriptor);

                 // This is redundent since the base of the temporary is already set to ZERO!
                 // setBase(0);
                    APP_ASSERT (getBase() == 0);

                 // This is no longer going to be a temporary so mark it as a non temporary
                 // Array_Descriptor.Array_Domain.Is_A_Temporary = FALSE;
                    setTemporary(FALSE);

                 // Double check to make sure it is not a view
                    APP_ASSERT( isView() == FALSE );
                  }

            // ... bug fix (8/26/96,kdb) see comment above ...
            // Delete_Array_Data ();
               *Lhs_Data_Pointer = NULL;

            // BUG FIX: (18/1/94) fixed reference counts.
            // Reset the reference count to zero as we steal the data because the 
            // new data should have a reference count of zero (not -1 as would be 
            // the case if it had been deleted)! setting it to ZERO means there are
            // NO (ZERO) external references to this data (only the one by this array 
            // object).

            // I don't want to provide a means to reset or assign specific reference 
            // counts (dangerous I think)

            // Steal the pointer (to the original array) from the Rhs's data
            // Note that these are REFERENCE variables!

               *Lhs_Data_Pointer = *Rhs_Data_Pointer;
            // ... change (8/21/96,kdb) Add eight view pointers ...
            // Array_View_Pointer = Array_Descriptor.Array_Data + Array_Descriptor->Scalar_Offset;

               POINTER_LIST_INITIALIZATION_MACRO;

            // Since the previous copy of the array data was deleted the reference count 
	    // of the Raw Data is not -1 (typically) so we have to reset it.  We could 
            // just increment it -- but error checking within the 
            // incrementRawDataReferenceCount would trap out the case where the
            // reference count of the raw data is less than ZERO.  So we have a special 
            // inline function to force the reset value to ZERO.

               resetRawDataReferenceCount();

            // We can set the correct one by using the fact that 
            // Rhs_Data_Pointer is a pointer to a pointer to the array data!

            // ... (bug fix, 6/27/96, kdb) don't set this pointer to null here
            // because otherwise the array won't be deleted.  Also increment
            // the reference count for Rhs so Array_Data won't be deleted ...
            // *Rhs_Data_Pointer = NULL;

            // printf ("In operator=: Calling Rhs.incrementRawDataReferenceCount() \n");
               Rhs.incrementRawDataReferenceCount();

            /*
            // ... the Array_Descriptor will be deleted but because the
	    // Array_Reference_Count_Array was just incremented the array ID
            // won't be put back in the list like it should be.  Before
            // deleting, force the array ID onto the stack.  The 
            // Array_Reference_Count_Array[Array_ID] will already be 0 after
            // the delete.  (This problem comes up because of the assumption
            // that the only time 2 arrays share Array_Data is when one is a
            // view of another and so has the same Array_ID) ...
            */

            // Bugfix (11/6/2000) Deleting the array should put it back onto 
            // the stack of array ID for future use (I think).
            // Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(Rhs.Array_ID());

            // get the array id of the Rhs before it is deleted
               int rhsArrayID = Rhs.Array_ID();

            // The Rhs will be deleted but until then we can't really call any
            // of it's member functions since there is no data associated with it.
            // printf ("In intArray::operator= calling Delete_If_Temporary ( Rhs ) \n");
               Delete_If_Temporary ( Rhs );

            // Bugfix (11/7/2000) 
            // Move to after the call to Delete_If_Temporary() since that function 
            // calls the Test_Consistency function which will report an error if the 
            // array ID is pushed onto the stack while it is a valid array ID still in use.
            // Bugfix (11/6/2000) Deleting the array should put it back onto 
            // the stack of array ID for future use (I think).
            // Bugfix (11/11/2000) accessing data after it is deleted
            // Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(Rhs.Array_ID());
               Rhs.Array_Descriptor.Array_Domain.Push_Array_ID(rhsArrayID);

            // Bugfix (12/6/2000) Also reset the reference count associated with this id value!
               Array_Domain_Type::Array_Reference_Count_Array [rhsArrayID] = getRawDataReferenceCountBase() - 1;

            // Now we have to delete the "this" object if it is a View! But we are the 
            // this object so we have to be very carefull how this is done!
            // printf ("In intArray::operator= calling Delete_Lhs_If_Temporary ( *this ) \n");
               Delete_Lhs_If_Temporary ( *this );
             }
            else // Do the assignment the hard way (element by element)!
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                    printf ("Do the assignment by calling the MDI function! \n");
#endif
            /* Call Assignment function
            // The function name that we call here is slightly effected by the fact that 
	    // we use a marco processor to generate the machine dependent functions a 
            // better name would have been "Assign_Array_Data_Equals_Scalar" but this 
            // would have not made for a good name (after macro expansion) for the other 
            // operators (like +,-,*, and /).
            */

            /* The array class must check for the case of self assignment,
            // if A=A then the assignement is skiped (using the logic below).
            // But if a region of A is assigned to another region of A then
            // because the overlap might not be computed correctly (we cannot
            // specify the order of evaluation in the MDI functions).
            */
               if (Array_ID() == Rhs.Array_ID())
                  {
                 // Possible case of A(I+1) = A(I) which would not evaluate correctly 
                 // because of the order of evaluation used in the MDI functions!
     
#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("CASE: Array_ID() == Rhs.Array_ID() \n");
	               }
#endif
                    if ( !(isContiguousData() && Rhs.isContiguousData()) )
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                            printf ("Make a copy \n");
#endif

                      // Rhs.displayReferenceCounts("In operator=(intArray) BEFORE COPY: Rhs");

                      // Make a copy
                         intArray Rhs_Copy = Rhs;

                      // Rhs.displayReferenceCounts("In operator=(intArray) AFTER COPY: Rhs");
                      // Rhs_Copy.displayReferenceCounts("In operator=(intArray) AFTER COPY: Rhs_Copy");

                      // Now just call the operator= again (since Lhs and Rhs have different 
	              // array ids)!
                         *this = Rhs_Copy;
                       }
                      else
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 1)
                              printf ("CASE: A(I) = A(I) so skip the assignment operation! \n");
#endif
                      // We can skip the assignment of A(I) = A(I) but we have to delete the 
	              // Rhs view!
                         Delete_If_Temporary ( Rhs );
                       }
                  }
                 else
                  {
                 // Bugfix (3/29/95) fixed null array operations between Lhs and Rhs Null 
	         // array objects.  This fix is required for P++ to work properly on 
	         // processors where neither the Lhs of Rhs have an representation on the 
                 // local processor.
                    if (isNullArray() && !isView())
                       {
                      // Bug fix (8/9/94) need to build new descriptor to the correct size 
	              // of the Rhs
                      // APP_ASSERT( Array_Descriptor != NULL );

                      // This is much simpler thant what we have below and it allows the 
	              // bases of the Rhs to be preserved in the Lhs (which was not 
	              // previously true).
                         redim (Rhs);
                      // ... change (8/21/96,kdb) add 8 view pointers ...
	                 POINTER_LIST_INITIALIZATION_MACRO;
                       }

                    APP_ASSERT(isTemporary() == FALSE);

#if EXECUTE_MDI_FUNCTIONS
                    MDI_i_Assign_Array_Equals_Array_Accumulate_To_Operand
                       ( *Lhs_Data_Pointer , *Rhs_Data_Pointer , Mask_Array_Data ,
                         (array_domain*) (&Array_Descriptor.Array_Domain) , 
                         (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("Calling Delete_If_Temporary(Rhs) \n");
	               }
#endif
                 // If A(I) = A(I-1) then the view Rhs was handled by the copy constructor!
                    Delete_If_Temporary ( Rhs );

#if COMPILE_DEBUG_STATEMENTS
                    if (APP_DEBUG > 1)
	               {
                         printf ("Calling Delete_Lhs_If_Temporary(*this) \n");
	               }
#endif
                 // Now we have to delete the "this" object if it is a View! But we are 
                 // the this object so we have to be very carefull how this is done!
                 // This function will be removed soon to streamline the operator= 
                 // member function
                    APP_ASSERT(isTemporary() == FALSE);
                    Delete_Lhs_If_Temporary ( *this );
                  }
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in intArray::operator=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 1)
        {
          printf ("Leaving operator= (intArray) \n");
        }
#endif
  // operator= does the assignment and then returns a intArray
     return *this;
   }

// ******************************************************************
// ******************************************************************
//  Abstract Operators (with masking support for "where" statements)
// ******************************************************************
// ******************************************************************

/* These operators support the efficient handling of the Temporaries and Views 
// which are a part of the Array class.  The details of the manipulation are
// isolated into a few abstract operators below which take function pointers
// to make calls specific to the requirements of operator+, operator-, operator*,
// operator/, and other related operators.  This greatly simplifies the 
// implementation of the actual operators for +,-,*, and /.  It also
// isolates the mantainence of the code!  The functions that are used
// as parameters define the machine dependent interface.
// Each input function is handed a pointer to the Mask used in the support of
// the "where" statement that is a part of this array class library.  It is 
// the job of the input function (the machine dependent function) to
// recognize if the Mask pointer is valid (in FORTRAN the Mask_Stride != 0 for a 
// valid mask) and then use the mask to correctly compute the specific fuction.
*/

// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
intArray &
intArray::Abstract_Unary_Operator (
   const intArray & X ,
   MDI_int_Prototype_0 Operation_Array_Data ,
   MDI_int_Prototype_1 Operation_Array_Data_Accumulate_To_Existing_Operand ,
   int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #1 intArray::Abstract_Unary_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif
 
     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     intArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand ( X , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY)
             {
               Temporary_Array.Add_Defered_Expression ( new intArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND);
               Temporary_Array.Add_Defered_Expression ( new intArray_Function_1 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , X ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY)
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) (
                    Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
                    (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
		    (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND);
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                    X.Array_Descriptor.Array_Data , Mask_Array_Data , 
		    (array_domain*) (&X.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif
            // Bugfix (12/4/2000) delete the temprary to be uniformally consistant with rest of code
               Delete_If_Temporary ( X );
             }
        } 

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #1 intArray::Abstract_Unary_Operator");

     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Temporary_Array.displayReferenceCounts("Temporary_Array in #1 intArray::Abstract_Unary_Operator");
        }
#endif

     return Temporary_Array;
   }



// Support for Conversion Operators
#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
intArray &
intArray::Abstract_int_Conversion_Operator ( 
   const intArray & X ,
   MDI_int_Prototype_convertTo_intArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & intArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new intArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the intArray::Abstract_int_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 intArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif

#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
floatArray &
intArray::Abstract_float_Conversion_Operator ( 
   const intArray & X ,
   MDI_int_Prototype_convertTo_floatArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & intArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     floatArray & Temporary_Array = *(floatArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new intArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the intArray::Abstract_float_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 intArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif

#if 1
// ******************************************************************
//                   ABSTRACT UNARY OPERATOR
// ******************************************************************
doubleArray &
intArray::Abstract_double_Conversion_Operator ( 
   const intArray & X ,
   MDI_int_Prototype_convertTo_doubleArray Operation_Array_Data ,
   int Operation_Type )
   {
  // This abstract operator is used for the conversion operators

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #2 Array & intArray::Abstract_Unary_Conversion_Operator! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ();
#endif

     doubleArray & Temporary_Array = *(doubleArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
       // Temporary_Array.Add_Defered_Expression ( new intArray_Function_0 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
          printf ("Defered evaluation commented out for the intArray::Abstract_double_Conversion_Operator() \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*)(&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*)(&X.Array_Descriptor.Array_Domain) , 
               Mask_Descriptor );
#endif
               Delete_If_Temporary ( X );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #2 intArray::Abstract_Unary_Operator");
#endif

     return Temporary_Array;
   }
#endif


// ******************************************************************
//                   ABSTRACT BINARY OPERATOR
// ******************************************************************
intArray &
intArray::Abstract_Binary_Operator ( const intArray & Lhs , const intArray & Rhs ,
   MDI_int_Prototype_2 Operation_Array_Data ,
   MDI_int_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
   int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #3 intArray::Abstract_Binary_Operator (intArray,intArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #3 intArray::Abstract_Binary_Operator");
     Rhs.Test_Consistency("Test of Rhs in #3 intArray::Abstract_Binary_Operator");

  // Lhs.view("Lhs in (AT START OF FUNCTION) intArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AT START OF FUNCTION) intArray::Abstract_Binary_Operator");

  // Lhs.displayReferenceCounts("Lhs at TOP of #3 intArray::Abstract_Binary_Operator()");
  // Rhs.displayReferenceCounts("Rhs at TOP of #3 intArray::Abstract_Binary_Operator()");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;

#if COMPILE_DEBUG_STATEMENTS
  // Lhs.view("Lhs in (BEFORE SETUP OF TEMPORARY ARRAY) intArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (BEFORE SETUP OF TEMPORARY ARRAY) intArray::Abstract_Binary_Operator");
#endif

     intArray & Temporary_Array = 
          Build_New_Array_Or_Reuse_Operand ( Lhs , Rhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

#if COMPILE_DEBUG_STATEMENTS
  // Lhs.view("Lhs in (AFTER SETUP OF TEMPORARY ARRAY) intArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AFTER SETUP OF TEMPORARY ARRAY) intArray::Abstract_Binary_Operator");
     Temporary_Array.Test_Consistency("Test of Temporary_Array in #3 intArray::Abstract_Binary_Operator");
  // Temporary_Array.view("Temporary_Array in intArray::Abstract_Binary_Operator");
  // Lhs.view("Lhs in (AFTER SETUP OF TEMPORARY ARRAY AND TEST_CONSISTENCY) intArray::Abstract_Binary_Operator");
  // Rhs.view("Rhs in (AFTER SETUP OF TEMPORARY ARRAY AND TEST_CONSISTENCY) intArray::Abstract_Binary_Operator");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new intArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
             }
            else
             {
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
                    Temporary_Array.Add_Defered_Expression ( new intArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
                    Temporary_Array.Add_Defered_Expression ( new intArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Rhs , Lhs ) );
                  }
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 1)
               printf ("Call machine dependent function! \n");

       // Temp code not valid for general use!
       // APP_ASSERT(Temporary_Array.Array_Data != NULL);
       // APP_ASSERT(Lhs.Array_Data != NULL);
       // APP_ASSERT(Rhs.Array_Data != NULL);
#endif

          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) 
		  (Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , 
		   Mask_Array_Data , 
		   (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
		   (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
                   (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
            // If we had to build the temporary using NEWLY_ALLOCATED_MEMORY then neither of the
            // input arrays where a temporary (which could be reused) -- so we don't have to
            // delete if temporary since we know that neither could be.  So the following
            // function calls can be eliminated.

            // printf ("Now call delete if temporary for Lhs and Rhs! \n");

            // Lhs.displayReferenceCounts("Lhs case NEWLY_ALLOCATED_MEMORY in #3 intArray::Abstract_Binary_Operator()");
            // Rhs.displayReferenceCounts("Rhs case NEWLY_ALLOCATED_MEMORY in #3 intArray::Abstract_Binary_Operator()");

               Delete_If_Temporary ( Lhs );
               Delete_If_Temporary ( Rhs );
             }
            else
             {
            // We use the same functions below but change the order 
            // of the Lhs and Rhs in the parameter list (and the order of the Base,Bound,etc)!
            // BUG FIX: (The first subtle error was that the Base,Bound,etc order was not swaped)
            // However we will leave the interface to the Machine_Dependent_Interface functions
            // such that these related functions are implemented once since we want to
            // limit the number of machine dependent functions (this enhances the portability).
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                     Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                     (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		     (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , 
		     Mask_Descriptor );
#endif
                 // Rhs.displayReferenceCounts("Rhs case MEMORY_FROM_LHS in #3 intArray::Abstract_Binary_Operator()");

                    Delete_If_Temporary ( Rhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS);
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                      Rhs.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                 // Lhs.displayReferenceCounts("Lhs case MEMORY_FROM_RHS in #3 intArray::Abstract_Binary_Operator()");

                 // Since we got the temporary from the Rhs the rule is that we first looked
                 // at the Lhs (and took it if it was a temporary) -- so the Lhs is NOT a 
                 // temporary.  So we can eliminate the function call to delete the Lhs.
                    Delete_If_Temporary ( Lhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Rhs );
                  }
             }
        } 

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #3 intArray::Abstract_Binary_Operator");
#endif
 
     return Temporary_Array;
   }

// ******************************************************************
//           ABSTRACT BINARY OPERATOR (NON COMMUTATIVE CASE)
// ******************************************************************
intArray &
intArray::Abstract_Binary_Operator_Non_Commutative ( 
     const intArray & Lhs , const intArray & Rhs ,
     MDI_int_Prototype_2 Operation_Array_Data ,
     MDI_int_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #4 intArray::Abstract_Binary_Operator_Non_Commutative (intArray,intArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #4 intArray::Abstract_Binary_Operator_Non_Commutative");
     Rhs.Test_Consistency("Test of Rhs in #4 intArray::Abstract_Binary_Operator_Non_Commutative");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     intArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Rhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Completed call to Build_New_Array_Or_Reuse_Operand() \n");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new intArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
             }
            else
             {
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
                    Temporary_Array.Add_Defered_Expression ( new intArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
                    Temporary_Array.Add_Defered_Expression ( new intArray_Function_2 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
                  }
             }
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Do Where_Statement processing \n");
#endif

          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Call machine dependent function! \n");
#endif

          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) (
                 Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data ,
                 Rhs.Array_Descriptor.Array_Data , Mask_Array_Data , 
		 (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                 (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		 (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
               Delete_If_Temporary ( Lhs );
               Delete_If_Temporary ( Rhs );
             }
            else
             {
            // We use the same functions below but change the order 
            // of the Lhs and Rhs in the parameter list (and the order of the Base,Bound,etc)!
            // BUG FIX: (The first subtle error was that the Base,Bound,etc order was not swaped)
            // However we will leave the interface to the Machine_Dependent_Interface functions
            // such that these related functions are implemented once since we want to
            // limit the number of machine dependent functions (this enhances the portability).
               if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_LHS) 
                  {
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                      Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Rhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
                  }
                 else
                  {
                    APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_RHS); 
#if EXECUTE_MDI_FUNCTIONS
                    (*Operation_Array_Data) (
                      Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
                      Rhs.Array_Descriptor.Array_Data , Mask_Array_Data , 
		      (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Lhs );

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Rhs );
                  }
             }
        } 
 
#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #4 intArray::Abstract_Binary_Operator_Non_Commutative");
#endif

     return Temporary_Array;
   }

// ******************************************************************
//                   ABSTRACT BINARY OPERATOR 
// ******************************************************************
intArray &
intArray::Abstract_Binary_Operator (
     const intArray & Lhs , int x ,
     MDI_int_Prototype_4 Operation_Array_Data ,
     MDI_int_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #5 intArray::Abstract_Binary_Operator (intArray,int) \n");

     Lhs.Test_Consistency("Test of Lhs in intArray::Abstract_Binary_Operator (intArray,int)");
#endif
 
     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     intArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new intArray_Function_4 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
               Temporary_Array.Add_Defered_Expression ( new intArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
                    x , 
#else
                    &x , 
#endif
                    Mask_Array_Data , 
		    (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
		    (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
                    Delete_If_Temporary ( Lhs );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) ( Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
                    x , 
#else
                    &x , 
#endif
                    Mask_Array_Data , (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		    Mask_Descriptor );
#endif

                 // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
                    Delete_If_Temporary ( Lhs );
             }
        } 
#if 0
#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #5 intArray::Abstract_Binary_Operator");

     Temporary_Array.displayReferenceCounts("Called inside of #5 intArray::Abstract_Binary_Operator");
#endif
#endif

     return Temporary_Array;
   }

#if 1
// ******************************************************************
//                   ABSTRACT BINARY OPERATOR 
// ******************************************************************
intArray &
intArray::Abstract_Binary_Operator_Non_Commutative (
     const intArray & Lhs , int x,
     MDI_int_Prototype_4 Operation_Array_Data ,
     MDI_int_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #6 intArray::Abstract_Binary_Operator_Non_Commutative (int,intArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #6 intArray::Abstract_Binary_Operator_Non_Commutative (int,intArray)");
#endif

     Memory_Source_Type Result_Is_Lhs_Or_Rhs_Or_New_Memory = Uninitialized_Source;
     intArray & Temporary_Array = Build_New_Array_Or_Reuse_Operand
                                         ( Lhs , Result_Is_Lhs_Or_Rhs_Or_New_Memory );

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          if (Result_Is_Lhs_Or_Rhs_Or_New_Memory == NEWLY_ALLOCATED_MEMORY) 
             {
               Temporary_Array.Add_Defered_Expression ( new intArray_Function_4 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
             }
            else
             {
               APP_ASSERT(Result_Is_Lhs_Or_Rhs_Or_New_Memory == MEMORY_FROM_OPERAND); 
               Temporary_Array.Add_Defered_Expression ( new intArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
             }
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , 
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) , 
               (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

       // Don't delete the Lhs if it's data will be reused
       // Delete_If_Temporary ( Lhs );

       // Bugfix (12/3/2000) we have to delete the Temporary in the abstract operator (to be consistent)
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #6 intArray::Abstract_Binary_Operator_Non_Commutative");
#endif
 
     return Temporary_Array;
   }
#endif

// ******************************************************************
//              ABSTRACT OPERATOR ( +=, -=, *=, /=, %= )
//           Note that operator= does not use this function
// ******************************************************************
void
intArray::Abstract_Operator_Operation_Equals (
     const intArray & Lhs , const intArray & Rhs ,
     MDI_int_Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #7 intArray::Abstract_Operator_Equals(intArray,intArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #7 intArray::Abstract_Operator_Equals (intArray,intArray)");
     Rhs.Test_Consistency("Test of Rhs in #7 intArray::Abstract_Operator_Equals (intArray,intArray)");
#endif

     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);
 
     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Lhs.Add_Defered_Expression ( new intArray_Function_3 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


       /* The array class much check for the case of self reference,
       // if A += A then the operation is not skipped as in assignement.
       // But if a region of A is assigned to another region of A then
       // because the overlap might not be computed correctly (we cannot
       // specify the order of evaluation in the MDI functions).
       */
          if (Lhs.Array_ID() == Rhs.Array_ID())
             {
            // Possible case of A(I+1) (+,-,*,/,%)= A(I) which would not evaluate correctly 
            // because of the order of evaluation used in the MDI functions!

            // Make a copy of the Rhs to avoid self-reference.
            // The copy constructor also takes care of deleting the Rhs if is was
            // a temporary or view (so it should not be referenced beyond this point).
               intArray Rhs_Copy = Rhs;

            // Now just call the abstract operation function again with the Rhs_Copy parameters!
               intArray::Abstract_Operator_Operation_Equals ( Lhs , Rhs_Copy ,
                    Operation_Array_Data_Accumulate_To_Existing_Operand , Operation_Type );
             }
            else
             {
#if EXECUTE_MDI_FUNCTIONS
               (*Operation_Array_Data_Accumulate_To_Existing_Operand) (
                Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
		(array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif

               Delete_If_Temporary ( Rhs );
               Delete_Lhs_If_Temporary ( Lhs );
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency("Test of Lhs (FINAL RESULT) in #7 intArray::Abstract_Operator_Equals");
#endif
   }

// ******************************************************************
//              ABSTRACT OPERATOR ( +=, -=, *=, /=, %= )
//           Note that operator= does not use this function
// ******************************************************************
void
intArray::Abstract_Operator_Operation_Equals (
     const intArray & Lhs , int x ,
     MDI_int_Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {   
  // This function is only called once for each Array statment!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #8 intArray::Abstract_Operator_Equals(intArray,int) \n");

     Lhs.Test_Consistency("Test of Lhs in #8 intArray::Abstract_Operator_Equals (intArray,int)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Lhs.Add_Defered_Expression ( new intArray_Function_5 ( Operation_Type , Operation_Array_Data_Accumulate_To_Existing_Operand , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif

 
#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data_Accumulate_To_Existing_Operand) ( Lhs.Array_Descriptor.Array_Data , 
#ifndef FORTRAN_MDI
                 x ,
#else
                 &x ,
#endif
                 Mask_Array_Data , (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
		 Mask_Descriptor );
#endif
          Delete_Lhs_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Lhs.Test_Consistency("Test of Lhs (FINAL RESULT) in #8 intArray::Abstract_Operator_Equals");
#endif
   }

// ***********************************************************************
// ***********************************************************************
//              ABSTRACT OPERATORS RETURNING intArray
// ***********************************************************************
// ***********************************************************************

// ******************************************************************
//           ABSTRACT UNARY OPERATOR (logical function NOT)
//       The following function is required in the "operator!"
// ******************************************************************
intArray &
intArray::Abstract_Unary_Operator_Returning_IntArray ( const intArray & X ,
     MDI_int_Prototype_9  Operation_Array_Data ,
     MDI_int_Prototype_10 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
  /* In this fucntion we want to reuse the temporary (if X is a temporary)
  // but only if it is the correct size (the size of Array<int>), so we check to 
  // see the size of the Array<TYPE> and either try to reuse the temporary
  // (same as in the other runctions returning Array<TYPE>) or build a 
  // new return type Array<int>!
  */

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #9 intArray::Abstract_Unary_Operator_Returning_IntArray! \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }

  // This is the only test we can do on the input!
     X.Test_Consistency("Test of X in #9 intArray::Abstract_Unary_Operator_Returning_IntArray");
#endif
 
  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
  // intArray & Temporary_Array.Build_Temporary_By_Example (X);
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (X));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( 
               new intArray_Function_9 ( Operation_Type , Operation_Array_Data , Temporary_Array , X ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
               Temporary_Array.Array_Descriptor.Array_Data , X.Array_Descriptor.Array_Data , Mask_Array_Data ,
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
               (array_domain*) (&X.Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif
          Delete_If_Temporary ( X );
        }
 
  // printf ("Leaving Abstract Unary operator returning IntArray! \n");

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #9 intArray::Abstract_Unary_Operator_Returning_IntArray");
#endif

     return Temporary_Array;
   }

// ******************************************************************
//                     ABSTRACT BINARY OPERATOR
// ******************************************************************
intArray &
intArray::Abstract_Binary_Operator_Returning_IntArray (
     const intArray & Lhs , const intArray & Rhs ,
     MDI_int_Prototype_11 Operation_Array_Data ,
     MDI_int_Prototype_9  Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #10 intArray::Abstract_Binary_Operator_Returning_IntArray (intArray,intArray) \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }

     Lhs.Test_Consistency("Test of Lhs in intArray::Abstract_Binary_Operator_Returning_IntArray");
     Rhs.Test_Consistency("Test of Rhs in intArray::Abstract_Binary_Operator_Returning_IntArray");
#endif

  // avoid compiler warning with this useless statement (should be optimized out by compiler)
     if (Operation_Array_Data_Accumulate_To_Existing_Operand);

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (Lhs));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( new intArray_Function_11 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) (
                         Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
                         Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
                         (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
                         (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
                         (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) ,
                         Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
          Delete_If_Temporary ( Rhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #10 intArray::Abstract_Binary_Operator_Returning_IntArray");
#endif
 
     return Temporary_Array;
   }

// ******************************************************************
//                     ABSTRACT BINARY OPERATOR
// ******************************************************************
intArray &
intArray::Abstract_Binary_Operator_Returning_IntArray (
     const intArray & Lhs , int x ,
     MDI_int_Prototype_12 Operation_Array_Data ,
     MDI_int_Prototype_13 Operation_Array_Data_Accumulate_To_Existing_Operand ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of #11 intArray::Abstract_Binary_Operator_Returning_IntArray (intArray,int) \n");
       // NOTE: that the second input functioncan't be used since it returns data from a doubleArray!
       // avoid compiler warning with this useless statement (should be optimized out by compiler)
       // printf ("Avoid_Compiler_Warning = %p \n",Operation_Array_Data_Accumulate_To_Existing_Operand);
        }
 
     Lhs.Test_Consistency("Inside of intArray::Abstract_Binary_Operator_Returning_IntArray (intArray,int)");
#endif

  // In the case of use in the intArray class we could check for temporaries instead
  // of just building a new temporary and and not trying to reuse an existing one
     intArray & Temporary_Array = *(intArray::Build_Temporary_By_Example (Lhs));

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Temporary_Array.Add_Defered_Expression ( new intArray_Function_12 ( Operation_Type , Operation_Array_Data , Temporary_Array , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Temporary_Array.Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data ,
               (array_domain*) (&Temporary_Array.Array_Descriptor.Array_Domain) ,
               (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) ,
               Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Temporary_Array.Test_Consistency("Test in #11 intArray::Abstract_Binary_Operator_Returning_IntArray");
#endif

     return Temporary_Array;
   }

// ***********************************************************************
// ***********************************************************************
// Abstract operators required for support of intArray::replace!
// ***********************************************************************
// ***********************************************************************

// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
intArray & intArray::Abstract_Modification_Operator ( const intArray & Lhs , const intArray & Rhs ,
     MDI_int_Prototype_6 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #12 intArray::Abstract_Modification_Operator (intArray,intArray) \n");

     Lhs.Test_Consistency("Test of Lhs in #12 intArray::Abstract_Modification_Operator");
     Rhs.Test_Consistency("Test of Rhs in #12 intArray::Abstract_Modification_Operator");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new intArray_Function_6 ( Operation_Type , Operation_Array_Data , *this , Lhs , Rhs ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) 
	     (Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , Rhs.Array_Descriptor.Array_Data , Mask_Array_Data ,
              (array_domain*) (&Array_Descriptor.Array_Domain) , 
	      (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , 
	      (array_domain*) (&Rhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
          Delete_If_Temporary ( Rhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #12 intArray::Abstract_Modification_Operator");
#endif

     return *this;
   }

// ******************************************************************
//                        ABSTRACT OPERATOR
//               (used for replace member functions)
// ******************************************************************
intArray & intArray::Abstract_Modification_Operator ( const intArray & Lhs , int x ,
     MDI_int_Prototype_7 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #13 intArray::Abstract_Modification_Operator (intArray,int) \n");
 
     Lhs.Test_Consistency("Inside of #13 intArray::Abstract_Modification_Operator (intArray,int)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new intArray_Function_7 ( Operation_Type , Operation_Array_Data , *this , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , (array_domain*) (&Array_Descriptor.Array_Domain) , 
	       (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
       // printf ("WARNING: Delete_If_Temporary ( Lhs ) not called in intArray::Abstract_Modification_Operator \n");
       // P++ uses the referenceCounting in the A++ objects to allow this delete to occur
       // and decrement the referenceCount but not remove the object.
          Delete_If_Temporary ( Lhs );
        }

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #13 intArray::Abstract_Modification_Operator");
#endif

     return *this;
   }

#if !defined(INTARRAY)
// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
intArray & intArray::Abstract_Modification_Operator ( const intArray & Lhs , int x ,
     MDI_int_Prototype_8 Operation_Array_Data ,
     int Operation_Type )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #14 intArray::Abstract_Modification_Operator (intArray,int) \n");
 
     Lhs.Test_Consistency("Inside of #14 intArray::Abstract_Modification_Operator (intArray,int)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          Add_Defered_Expression ( new intArray_Function_8 ( Operation_Type , Operation_Array_Data , *this , Lhs , x ) );
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.Lhs.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(Lhs.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    Lhs.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  Lhs.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          (*Operation_Array_Data) ( Array_Descriptor.Array_Data , Lhs.Array_Descriptor.Array_Data , 
#if !defined(FORTRAN_MDI)
               x , 
#else
               &x , 
#endif
               Mask_Array_Data , (array_domain*) (&Array_Descriptor.Array_Domain) , 
	       (array_domain*) (&Lhs.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( Lhs );
        }
 
#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Test in #14 intArray::Abstract_Modification_Operator");
#endif

     return *this;
   }
#endif


// ******************************************************************
//                        ABSTRACT OPERATOR
// ******************************************************************
int intArray::Abstract_Unary_Operator ( const intArray & X ,
     MDI_int_Prototype_14 Operation_Array_Data , int Operation_Type )
   { 
     int Return_Value = 0;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of #15 Abstract_Operator (intArray) returning int \n");
 
     X.Test_Consistency("Test of X in #15 intArray::Abstract_Operator (intArray)");
#endif

     if (Expression_Tree_Node_Type::DEFER_EXPRESSION_EVALUATION)
        {
          int Avoid_Compiler_Warning = Operation_Type;
          printf ("ERROR: Misuse of Defered Evaluation -- Invalid use in reduction operator! \n");
          APP_ABORT();
        }
       else
        {
          /* Notice that we assume that the mask might be a view!
       // For Machine dependent function implementations in FORTRAN,
       // Mask_Stride = 0 implies that the Mask_Array_Data is NULL!
       */
#ifndef FORTRAN_MDI
          int* Mask_Array_Data = NULL;
          array_domain* Mask_Descriptor = NULL;
#else
       // We can't pass a NULL pointer to the CM-5 FORTRAN!
          int* Mask_Array_Data = (int*) Array_Descriptor.X.Array_Data;
          array_domain* Mask_Descriptor = (array_domain*) &(X.Array_Descriptor.Array_Domain);
#endif

          if (Where_Statement_Support::Where_Statement_Mask != NULL)
             {
#if defined(SERIAL_APP)
            // error checking
#if COMPILE_DEBUG_STATEMENTS
            if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	    {
	       printf ("ERROR: Both Serial_Where_Statement_Mask and");
	       printf (" Where_Statement_Mask are nonnull.\n");
	       APP_ABORT();
	    }
#endif
            APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);

            // The serial array class does not have access to the data (this is a 
	    // permission probem I have to fix) 
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDataPointer();
               Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray->getDomainPointer();
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.SerialArray);
#else
            // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
            // Mask_Descriptor = (array_domain*) Where_Statement_Support::Where_Statement_Mask->getDomainPointer();
               Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*) &(Where_Statement_Support::Where_Statement_Mask->Array_Descriptor.Array_Domain);
            // Are the arrays the same size (otherwise issue error message and stop).
               if (Index::Index_Bounds_Checking)
                    X.Test_Conformability (*Where_Statement_Support::Where_Statement_Mask);
#endif

             }
#if defined(SERIAL_APP)
	     else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
	     {
               Mask_Array_Data = Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Data;
               Mask_Descriptor = (array_domain*)(&Where_Statement_Support::
		  Serial_Where_Statement_Mask->Array_Descriptor.Array_Domain);
               // Are the arrays the same size (otherwise issue error message and 
	       // stop).
               if (Index::Index_Bounds_Checking)
                  X.Test_Conformability 
		     (*Where_Statement_Support::Serial_Where_Statement_Mask);
	     }
#endif


#if EXECUTE_MDI_FUNCTIONS
          Return_Value = (*Operation_Array_Data)
	     ( X.Array_Descriptor.Array_Data , Mask_Array_Data ,
	       (array_domain*) (&X.Array_Descriptor.Array_Domain) , Mask_Descriptor );
#endif
          Delete_If_Temporary ( X );
        }
 
     return Return_Value;
   }

// End of !defined(USE_EXPRESSION_TEMPLATES)
#endif









/* *****************************************************************************
                           START OF NEW M4 MACRO 
   ***************************************************************************** */

