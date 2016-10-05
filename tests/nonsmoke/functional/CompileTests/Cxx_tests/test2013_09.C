#define _GLIBCXX__PTHREADS 1

#include "A++.h"

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
//             Add_Defered_Expression ( new doubleArray_Function_3 ( Assignment , MDI_d_Assign_Array_Equals_Array_Accumulate_To_Operand , *this , Rhs ) );
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









