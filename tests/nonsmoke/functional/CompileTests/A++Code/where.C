// OLD COMMENT
// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
// NEW COMMENT
// Compiling P++ requires that COMPILE_PPP be defined ANR that it is TRUE (value == 1)
// When not compiling P++ source code the value will be 0. But COMPILE_PPP is always
// defined except for compiling an A++ source file or application.
#if !defined(COMPILE_APP)
// define COMPILE_SERIAL_APP
#endif

#include "A++.h"

// I should have specified all the conditional compilation
// in this file to depend upon PPP instead of SERIAL_APP
// the following is a quick fix to this.
#if defined(PPP)
#define SERIAL_APP
#endif

// Support for where statement! 
#if defined(SERIAL_APP) 
intSerialArray* Where_Statement_Support::Serial_Where_Statement_Mask          = NULL;
intSerialArray* Where_Statement_Support::Serial_Previous_Where_Statement_Mask = NULL;
intSerialArray* Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History = NULL;
#endif
intArray* Where_Statement_Support::Where_Statement_Mask          = NULL;
intArray* Where_Statement_Support::Previous_Where_Statement_Mask = NULL;
intArray* Where_Statement_Support::Previous_Where_Statement_Mask_History = NULL;

int Where_Statement_Support::APP_Global_Where_Var = 0;
 

// *****************************************************************
// *****************************************************************
//                    Where related functions
// *****************************************************************
// *****************************************************************
 
// Should we support Masks which are views?  Then the stride might be > 1! 
// Currently we assume that the Where_Statement_Mask is not a temporary or a view!
// Discuss with Jeff the details of this implementation.

// We could have a stack of masks so that we could handle some sort of where_mask scope.
// We could have the mask have local scope and then implement their lifetimes markers
// using the destructor (so we know when it left scope without accessing it directly, which
// would be illegal (segment fault).

int
Where_Statement_Support::numberOfInternalArrays()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Where_Statement_Support::numberOfInternalArrays() \n");
#endif

     int numberOfOutstandingArrayObjects = 0;

     if (Where_Statement_Mask != NULL)
        {
          APP_ASSERT (Where_Statement_Mask->usesIndirectAddressing() == FALSE);
          numberOfOutstandingArrayObjects++;
        }
     if (Previous_Where_Statement_Mask != NULL)
        {
          APP_ASSERT (Previous_Where_Statement_Mask->usesIndirectAddressing() == FALSE);
          numberOfOutstandingArrayObjects++;
        }
     if (Previous_Where_Statement_Mask_History != NULL)
        {
          APP_ASSERT (Previous_Where_Statement_Mask_History->usesIndirectAddressing() == FALSE);
          numberOfOutstandingArrayObjects++;
        }

#if defined (SERIAL_APP)
  // We have to cleanup the intSerialArray where data as well.
  // This is not done in the code below but will be done in the
  // Where_Statement_Support::cleanup_after_Where() function
  // This code is only temporary.
     if (Serial_Where_Statement_Mask != NULL)
        {
          numberOfOutstandingArrayObjects++;
        }
     if (Serial_Previous_Where_Statement_Mask != NULL)
        {
          numberOfOutstandingArrayObjects++;
        }
     if (Serial_Previous_Where_Statement_Mask_History != NULL)
        {
          numberOfOutstandingArrayObjects++;
        }
#endif

  // printf ("Where_Statement_Support::numberOfInternalArrays() = %d \n",numberOfOutstandingArrayObjects);

     return numberOfOutstandingArrayObjects;
   }


#if defined (SERIAL_APP)
void
Where_Statement_Support::cleanup_after_Serial_Where()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Where_Statement_Support::cleanup_after_Serial_Where() \n");
#endif

  // We have to cleanup the intSerialArray where data as well.
  // This is not done in the code below but will be done in the
  // Where_Statement_Support::cleanup_after_Where() function
  // This code is only temporary.
     if (Serial_Where_Statement_Mask != NULL)
        {
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Serial_Where_Statement_Mask->decrementReferenceCount();
          if (Serial_Where_Statement_Mask->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Serial_Where_Statement_Mask;
          Serial_Where_Statement_Mask = NULL;
        }
     if (Serial_Previous_Where_Statement_Mask != NULL)
        {
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Serial_Previous_Where_Statement_Mask->decrementReferenceCount();
          if (Serial_Previous_Where_Statement_Mask->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Serial_Previous_Where_Statement_Mask;
          Serial_Previous_Where_Statement_Mask = NULL;
        }
     if (Serial_Previous_Where_Statement_Mask_History != NULL)
        {
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Serial_Previous_Where_Statement_Mask_History->decrementReferenceCount();
          if (Serial_Previous_Where_Statement_Mask_History->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Serial_Previous_Where_Statement_Mask_History;
          Serial_Previous_Where_Statement_Mask_History = NULL;
        }
   }
#endif

void Where_Statement_Support::cleanup_after_Where()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Where_Statement_Support::cleanup_after_Where() \n");
#endif
     
     if (Where_Statement_Mask != NULL)
        {
       // printf ("Deleting Where_Statement_Mask \n");
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Where_Statement_Mask->decrementReferenceCount();
          if (Where_Statement_Mask->getReferenceCount() < intArray::getReferenceCountBase())
               delete Where_Statement_Mask;
          Where_Statement_Mask = NULL;
        }
     if (Previous_Where_Statement_Mask != NULL)
        {
       // printf ("Deleting Previous_Where_Statement_Mask \n");
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Previous_Where_Statement_Mask->decrementReferenceCount();
          if (Previous_Where_Statement_Mask->getReferenceCount() < intArray::getReferenceCountBase())
               delete Previous_Where_Statement_Mask;
          Previous_Where_Statement_Mask = NULL;
        }
     if (Previous_Where_Statement_Mask_History != NULL)
        {
       // printf ("Deleting Previous_Where_Statement_Mask_History \n");
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Previous_Where_Statement_Mask_History->decrementReferenceCount();
          if (Previous_Where_Statement_Mask_History->getReferenceCount() < intArray::getReferenceCountBase())
               delete Previous_Where_Statement_Mask_History;
          Previous_Where_Statement_Mask_History = NULL;
        }
   }
 
//===================================================================
#if defined(SERIAL_APP)
int
where_function ( const intSerialArray & Mask )
   {
  // ... (9/20/96, kdb) fixed up intArray code below to make this
  // work with intSerialArray ...

  // printf ("Top of where_function (const intSerialArray & Mask)! \n");
  // printf ("Function not implemented yet! \n");
  // APP_ABORT();
  // return 0;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Top of where_function (const intSerialArray & Mask)! \n");
#endif

  // error checking
     if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
        {
          printf ("ERROR: In where: system Mask is already set (unset then set; likely a missing endwhere() (no nesting of where() is permited) \n");
          APP_ABORT();
        }
 
  // Clear out old previous mask to setup new where-elsewhere-... structure!
     if ( Where_Statement_Support::Serial_Previous_Where_Statement_Mask != NULL )
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Delete Serial_Previous_Where_Statement_Mask! \n");
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Where_Statement_Support::Serial_Previous_Where_Statement_Mask->decrementReferenceCount();
          if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask->getReferenceCount() < 
              intSerialArray::getReferenceCountBase())
               delete Where_Statement_Support::Serial_Previous_Where_Statement_Mask; 
          Where_Statement_Support::Serial_Previous_Where_Statement_Mask = NULL;
        }

  // Clear out old previous mask history to setup new where-elsewhere-... structure!
     if ( Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History != NULL )
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Delete Serial_Previous_Where_Statement_Mask_History! \n");
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History->decrementReferenceCount();
          if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History; 
          Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History = NULL;
        }

  // Bug fix 8/4/93 dquinlan -- Mask must be copied (unless it is a temporary)!
  // The result should be a mask that is not a temporary array object!
  // It is just as well that we make a copy in the case where a non-temporary 
  // mask is input since it avoids the mask changing if the user subsequently 
  // manipulated the input intArray. It makes the interface more robust!

     Where_Statement_Support::Serial_Where_Statement_Mask = new intSerialArray (Mask);
     APP_ASSERT (Where_Statement_Support::Serial_Where_Statement_Mask != NULL);

  // Mask.view("Mask in where_statement");
  // Where_Statement_Support::Serial_Where_Statement_Mask->view("Where_Statement_Support::Serial_Where_Statement_Mask");

#if 0
  // ... (9/2/97,kdb) need to force this here because function has been disabled for serialArrays ...
     if (Mask.isTemporary())
        {
          Mask.decrementRawDataReferenceCount();
        }
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 2)
          Where_Statement_Support::Serial_Where_Statement_Mask->view("Inside of where -- WHERE MASK");
#endif

  // In the beginning the history Does Not Exist!
     Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History = NULL;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Bottom of where_function ()! \n");
#endif

  // Force first iteration through for loop used to implement where macro!
     return START_WHERE_MACRO;
   }
#endif
 
//===================================================================
int
where_function ( const intArray & Mask )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Top of where_function (const intArray & Mask)! \n");
#endif

  // error checking
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          printf ("ERROR: In where: system Mask is already set (unset then set; likely a missing endwhere() (no nesting of where() is permited) \n");
          APP_ABORT();
        }
 
  // Clear out old previous mask to setup new where-elsewhere-... structure!
     if ( Where_Statement_Support::Previous_Where_Statement_Mask != NULL )
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Delete Previous_Where_Statement_Mask! \n");
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Where_Statement_Support::Previous_Where_Statement_Mask->decrementReferenceCount();
          if (Where_Statement_Support::Previous_Where_Statement_Mask->getReferenceCount() < intArray::getReferenceCountBase())
               delete Where_Statement_Support::Previous_Where_Statement_Mask; 
          Where_Statement_Support::Previous_Where_Statement_Mask = NULL;
        }

  // Clear out old previous mask history to setup new where-elsewhere-... structure!
     if ( Where_Statement_Support::Previous_Where_Statement_Mask_History != NULL )
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Delete Previous_Where_Statement_Mask_History! \n");
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Where_Statement_Support::Previous_Where_Statement_Mask_History->decrementReferenceCount();
          if (Where_Statement_Support::Previous_Where_Statement_Mask_History->getReferenceCount() < intArray::getReferenceCountBase())
               delete Where_Statement_Support::Previous_Where_Statement_Mask_History; 
          Where_Statement_Support::Previous_Where_Statement_Mask_History = NULL;
        }

  // Bug fix 8/4/93 dquinlan -- Mask must be copied (unless it is a temporary)!
  // The result should be a mask that is not a temporary array object!
  // Where_Statement_Support::Where_Statement_Mask = &((intArray &) Mask);

  // It is just as well that we make a copy in the case where a non-temporary mask is
  // input since it avoids the mask changing if the user subsequently manipulated
  // the input intArray. It makes the interface more robust!
#if 0
     Where_Statement_Support::Where_Statement_Mask = new intSerialArray (Mask);
     APP_ASSERT (Where_Statement_Support::Where_Statement_Mask != NULL);
#endif

  // printf ("In where_function: Mask.Array_ID() = %d Mask.getReferenceCount() = %d \n",
  //      Mask.Array_ID(),Mask.getReferenceCount());
  // printf ("In where_function: Mask.getSerialArrayPointer()->Array_ID() = %d Mask.getSerialArrayPointer()->getReferenceCount() = %d \n",
  //      Mask.getSerialArrayPointer()->Array_ID(),Mask.getSerialArrayPointer()->getReferenceCount());
     Where_Statement_Support::Where_Statement_Mask = new intArray (Mask);
     APP_ASSERT (Where_Statement_Support::Where_Statement_Mask != NULL);

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 2)
          Where_Statement_Support::Where_Statement_Mask->view("Inside of where -- WHERE MASK");
#endif

  // In the beginning the history Does Not Exist!
     Where_Statement_Support::Previous_Where_Statement_Mask_History = NULL;

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Bottom of where_function ()! \n");
#endif

  // Force first iteration through for loop used to implement where macro!
     return START_WHERE_MACRO;
   }
 
//==================================================================

int elsewhere_function ()
{ 
   // ... (9/20/96) added functionality for SerialArrays ...
   // Must include functionality for SerialArray for P++ -- not implemented yet!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Top of elsewhere_function ()! \n");
#endif

   if (Where_Statement_Support::Previous_Where_Statement_Mask != NULL)
   {
      // error checking
#if defined(SERIAL_APP)
#if COMPILE_DEBUG_STATEMENTS
      if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask != NULL)
      {
	 printf ("ERROR: Both Serial_Previous_Where_Statement_Mask and \n");
	 printf ("  Previous_Where_Statement_Mask are nonnull in elsewhere.\n");
         APP_ABORT();
      }
#endif
      APP_ASSERT(Where_Statement_Support::Serial_Previous_Where_Statement_Mask 
		 == NULL);
#endif
      if (Where_Statement_Support::Previous_Where_Statement_Mask_History == NULL)
      {
          printf ("ERROR: In elsewhere: previous system Mask_History is NOT set (unset then set; likely a missing endwhere() \n");
          APP_ABORT();
      }

      // Save the where mask!
      intArray *Last_Mask         = 
	 Where_Statement_Support::Previous_Where_Statement_Mask;
      intArray *Last_Mask_History = 
	 Where_Statement_Support::Previous_Where_Statement_Mask_History;

#if COMPILE_DEBUG_STATEMENTS
      if (APP_DEBUG > 0)
      {
         Last_Mask->view("LAST MASK");
         Last_Mask_History->view("LAST MASK HISTORY");
      }
#endif

      // Turn off the where mask! (Should already be off!)
      if ( Where_Statement_Support::Where_Statement_Mask != NULL )
      {
         printf ("ERROR: Inside of elsewhere_function () -- Where_Statement_Support::Where_Statement_Mask != NULL \n");
         APP_ABORT();
      }

      // Reassign the pointer to turn ON the where mask! (pointer assignment)
#if defined(USE_EXPRESSION_TEMPLATES) && !defined(HAS_MEMBER_TEMPLATES)
      intArray X;
      X = !(*Last_Mask || *Last_Mask_History);
      Where_Statement_Support::Where_Statement_Mask = new intArray (X);
      APP_ASSERT (Where_Statement_Support::Where_Statement_Mask != NULL);
#else
      Where_Statement_Support::Where_Statement_Mask = 
#if 0
 // DQ (6/30/2005): these are conflicting with operators in g++ (turn them off for now!)
           new intArray (!(*Last_Mask || *Last_Mask_History));
#else
           NULL;
#endif
      APP_ASSERT (Where_Statement_Support::Where_Statement_Mask != NULL);
#endif

      // Reset the pointer to the previous Mask so we can avoid two pointers to 
      // the same mask.  This correctly setup the data for the endwhere function 
      // and then subsequent elsewhere operations can be cascaded.
      // Where_Statement_Support::Previous_Where_Statement_Mask = NULL;

      // Avoid a memory leak by clearing out the previous mask!
      if ( Where_Statement_Support::Previous_Where_Statement_Mask != NULL )
      {

#if COMPILE_DEBUG_STATEMENTS
         if (APP_DEBUG > 0)
           printf ("Delete Previous_Where_Statement_Mask! \n");
#endif
      // Added conventional mechanism for reference counting control
      // operator delete no longer decriments the referenceCount.
         Where_Statement_Support::Previous_Where_Statement_Mask->decrementReferenceCount();
         if (Where_Statement_Support::Previous_Where_Statement_Mask->getReferenceCount() < intArray::getReferenceCountBase())
              delete Where_Statement_Support::Previous_Where_Statement_Mask; 

      // Now we set the pointers to the deleted array to NULL, just for good habit!
         Where_Statement_Support::Previous_Where_Statement_Mask = NULL;
         Last_Mask = NULL; 
      }

#if COMPILE_DEBUG_STATEMENTS
      if (APP_DEBUG > 0)
         Where_Statement_Support::Where_Statement_Mask->display("AFTER MODIFICATION: WHERE MASK");
#endif

#if 0
      // Later we can support the above operation in the MDI functions for better 
      // efficiency!

      // Old call to MDI function supported input parameter!
      // Mask.display("In else_where() input Mask");
      // Where_Statement_Support::Where_Statement_Mask->display("BEFORE MODIFICATION: WHERE MASK");

      // *Where_Statement_Support::Where_Statement_Mask = 
      //      intArray::Abstract_Binary_Operator ( *Where_Statement_Support::Where_Statement_Mask , Mask ,
      //           MDI_i_Else_Where_NOT_X_AND_Y_Operator ,
      //           MDI_i_Else_Where_NOT_X_AND_Y_Operator_Accumulate_To_Operand );
      // Where_Statement_Support::Where_Statement_Mask->display("AFTER MODIFICATION: WHERE MASK");
#endif

   }
#if defined(SERIAL_APP)
   else if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask != NULL)
   {
      // error checking
      if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History 
	  == NULL)
      {
         printf ("ERROR: In elsewhere: previous system Mask_History is NOT set\n");
	 printf (" (unset then set; likely a missing endwhere() \n");
         APP_ABORT();
      }

      // Save the where mask!
      intSerialArray *Last_Mask         = 
	 Where_Statement_Support::Serial_Previous_Where_Statement_Mask;
      intSerialArray *Last_Mask_History = 
	 Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History;

#if COMPILE_DEBUG_STATEMENTS
      if (APP_DEBUG > 0)
      {
         Last_Mask->view("LAST MASK");
         Last_Mask_History->view("LAST MASK HISTORY");
      }
#endif

      // Turn off the where mask! (Should already be off!)
      if ( Where_Statement_Support::Serial_Where_Statement_Mask != NULL )
      {
         printf ("ERROR: Inside of elsewhere_function () -- ");
	 printf ("Where_Statement_Support::Serial_Where_Statement_Mask != NULL \n");
         APP_ABORT();
      }

      // Reassign the pointer to turn ON the where mask! (pointer assignment)
#if defined(USE_EXPRESSION_TEMPLATES) && !defined(HAS_MEMBER_TEMPLATES)
      intSerialArray Y;
      Y = !(*Last_Mask || *Last_Mask_History);
      Where_Statement_Support::Serial_Where_Statement_Mask = new intSerialArray (Y);
#else
      Where_Statement_Support::Serial_Where_Statement_Mask = 
	 new intSerialArray (!(*Last_Mask || *Last_Mask_History));
#endif
      APP_ASSERT (Where_Statement_Support::Serial_Where_Statement_Mask != NULL);

      // Reset the pointer to the previous Mask so we can avoid two pointers to 
      // the same mask.  This correctly setup the data for the endwhere function 
      // and then subsequent elsewhere operations can be cascaded.
      // Where_Statement_Support::Serial_Previous_Where_Statement_Mask = NULL;

      // Avoid a memory leak by clearing out the previous mask!
      if ( Where_Statement_Support::Serial_Previous_Where_Statement_Mask != NULL )
      {

#if COMPILE_DEBUG_STATEMENTS
         if (APP_DEBUG > 0)
           printf ("Delete Serial_Previous_Where_Statement_Mask! \n");
#endif
      // Added conventional mechanism for reference counting control
      // operator delete no longer decriments the referenceCount.
         Where_Statement_Support::Serial_Previous_Where_Statement_Mask->decrementReferenceCount();
         if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask->getReferenceCount() < intSerialArray::getReferenceCountBase())
              delete Where_Statement_Support::Serial_Previous_Where_Statement_Mask; 

         // Now we set the pointers to the deleted array to NULL, just for good 
	 // habit!
         Where_Statement_Support::Serial_Previous_Where_Statement_Mask = NULL;
         Last_Mask = NULL; 
      }

#if COMPILE_DEBUG_STATEMENTS
      if (APP_DEBUG > 0)
         Where_Statement_Support::Serial_Where_Statement_Mask->
	    display("AFTER MODIFICATION: WHERE MASK");
#endif

   }
#endif
   else
   {
      // Where_Statement_Support::Previous_Where_Statement_Mask == NULL and
      // Where_Statement_Support::Serial_Previous_Where_Statement_Mask == NULL 

      printf ("ERROR: In elsewhere: previous system Mask is NOT set");
      printf ("(unset then set; likely a missing endwhere() \n");
      APP_ABORT();
   }

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Bottom of elsewhere_function ()! \n");
#endif

  // Force first iteration through for loop used to implement where macro!
     return START_WHERE_MACRO;
} 

//=======================================================================

int elsewhere_function ( const intArray & Mask )
   { 
  // Must include functionality for SerialArray for P++ -- not implemented yet!

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Top of elsewhere_function (X)! \n");
#endif

  // error checking
     if (Where_Statement_Support::Previous_Where_Statement_Mask == NULL)
        {
          printf ("ERROR: In elsewhere: previous system Mask is NOT set (unset then set; likely a missing endwhere() \n");
          APP_ABORT();
        }

  // error checking
     if (Where_Statement_Support::Previous_Where_Statement_Mask_History == NULL)
        {
          printf ("ERROR: In elsewhere: previous system Mask_History is NOT set (unset then set; likely a missing endwhere() \n");
          APP_ABORT();
        }

  // Where_Statement_Support::Where_Statement_Mask->view("BEFORE MODIFICATION: WHERE MASK");

  // Create some shorter names for easier use in this function!
     intArray *Last_Mask         = Where_Statement_Support::Previous_Where_Statement_Mask;
     intArray *Last_Mask_History = Where_Statement_Support::Previous_Where_Statement_Mask_History;

  // Mask.view("BEFORE MODIFICATION: INPUT MASK");
  // Last_Mask->view("BEFORE MODIFICATION: WHERE MASK");
  // Last_Mask_History->view("BEFORE MODIFICATION: WHERE MASK HISTORY");

  // Turn off the where mask! (Should already be off!)  If it is ON this it would effect the 
  // computation of the new Where_Statement_Mask!
     if ( Where_Statement_Support::Where_Statement_Mask != NULL )
        {
          printf ("ERROR: Inside of elsewhere_function ( const intArray & Mask ) -- Where_Statement_Support::Where_Statement_Mask != NULL \n");
          APP_ABORT();
        }

  // Reassign the pointer to turn ON the where mask! (pointer assignment)
  // Where_Statement_Support::Where_Statement_Mask = &( !*Last_Mask && !*Last_Mask_History && Mask);
  // Now use DeMorgan's Law to remove one operation!
#if defined(USE_EXPRESSION_TEMPLATES) && !defined(HAS_MEMBER_TEMPLATES)
      intArray Y;
      Y = !(*Last_Mask || *Last_Mask_History);
     Where_Statement_Support::Where_Statement_Mask = new intArray (Y);
#else
#if 0
  // DQ (6/30/2005): these are conflicting with operators in g++ (turn them off for now!)
     Where_Statement_Support::Where_Statement_Mask = new intArray ( !( *Last_Mask || *Last_Mask_History ) && Mask );
#else
     Where_Statement_Support::Where_Statement_Mask = NULL;
#endif
#endif
     APP_ASSERT (Where_Statement_Support::Where_Statement_Mask != NULL);

  // We could likely use this for greater efficiency later (but is just avoids a
  // memory allocation and subsequent deallocation, not a copy, so it is not a big deal)!
  // *Last_Mask = ! ( *Last_Mask || *Last_Mask_History ) && Mask;
  // Where_Statement_Support::Where_Statement_Mask = Last_Mask;

  // Note: we can't reference Mask after the last statement since if it was a temporary it was
  // deleted in the evaluation of the array expression!  But this way we don't have to
  // worry about removing it either since its memory was correctly handled.

  // Reset the pointer to the previous Mask so we can avoid two pointer to the same 
  // mask.  This correctly setup the data for the endwhere function and then 
  // subsequent elsewhere operations can be cascaded.
  // Where_Statement_Support::Previous_Where_Statement_Mask = NULL;

  // Avoid a memory leak by clearing out the previous mask!
     if ( Where_Statement_Support::Previous_Where_Statement_Mask != NULL )
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Delete Previous_Where_Statement_Mask! \n");
#endif
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Where_Statement_Support::Previous_Where_Statement_Mask->decrementReferenceCount();
          if (Where_Statement_Support::Previous_Where_Statement_Mask->getReferenceCount() < intArray::getReferenceCountBase())
               delete Where_Statement_Support::Previous_Where_Statement_Mask; 

       // Now we set the pointers to the deleted array to NULL, just for good habit!
          Where_Statement_Support::Previous_Where_Statement_Mask = NULL;
          Last_Mask = NULL; 
        }

  // Where_Statement_Support::Where_Statement_Mask->display("AFTER MODIFICATION: WHERE MASK");

#if 0
  // Later we can support the above operation in the MDI functions for better efficiency!

  // Old call to MDI function supported input parameter!
  // Mask.display("In else_where() input Mask");
  // Where_Statement_Support::Where_Statement_Mask->display("BEFORE MODIFICATION: WHERE MASK");

  // *Where_Statement_Support::Where_Statement_Mask = 
  //      intArray::Abstract_Binary_Operator ( *Where_Statement_Support::Where_Statement_Mask , Mask ,
  //           MDI_i_Else_Where_NOT_X_AND_Y_Operator ,
  //           MDI_i_Else_Where_NOT_X_AND_Y_Operator_Accumulate_To_Operand );
  // Where_Statement_Support::Where_Statement_Mask->display("AFTER MODIFICATION: WHERE MASK");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Bottom of elsewhere_function (X)! \n");
#endif

  // Force first iteration through for loop used to implement where macro!
     return START_WHERE_MACRO;
   } 
 
//=======================================================================

#if defined (SERIAL_APP)
int elsewhere_function ( const intSerialArray & Mask )
{ 
   // ... (9/20/96, kdb) fixed up version using intArray to make this
   //  work ...

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Top of elsewhere_function (X)! \n");
#endif

   // error checking
   if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask == NULL)
   {
      printf ("ERROR: In elsewhere: previous system Mask is NOT set (unset then set; likely a missing endwhere() \n");
      APP_ABORT();
   }

   // error checking
   if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History 
       == NULL)
   {
      printf ("ERROR: In elsewhere: previous system Mask_History is NOT set (unset then set; likely a missing endwhere() \n");
      APP_ABORT();
   }

   // Create some shorter names for easier use in this function!
   intSerialArray *Last_Mask         = Where_Statement_Support::
      Serial_Previous_Where_Statement_Mask;
   intSerialArray *Last_Mask_History = Where_Statement_Support::
      Serial_Previous_Where_Statement_Mask_History;

   // Turn off the where mask! (Should already be off!)  If it is ON this it 
   // would effect the computation of the new Where_Statement_Mask!
   if ( Where_Statement_Support::Serial_Where_Statement_Mask != NULL )
   {
      printf ("ERROR: Inside of elsewhere_function ( const intSerialArray & Mask ) -- Where_Statement_Support::Serial_Where_Statement_Mask != NULL \n");
      APP_ABORT();
   }

   // Reassign the pointer to turn ON the where mask! (pointer assignment)
   Where_Statement_Support::Serial_Where_Statement_Mask = 
      new intSerialArray ( !( *Last_Mask || *Last_Mask_History ) && Mask );
   APP_ASSERT (Where_Statement_Support::Serial_Where_Statement_Mask != NULL);

   // Note: we can't reference Mask after the last statement since if it was a 
   // temporary it was deleted in the evaluation of the array expression!  But 
   // this way we don't have to worry about removing it either since its memory 
   // was correctly handled.

   // Avoid a memory leak by clearing out the previous mask!
   if ( Where_Statement_Support::Serial_Previous_Where_Statement_Mask != NULL )
   {
#if COMPILE_DEBUG_STATEMENTS
      if (APP_DEBUG > 0)
         printf ("Delete Serial_Previous_Where_Statement_Mask! \n");
#endif
   // Added conventional mechanism for reference counting control
   // operator delete no longer decriments the referenceCount.
      Where_Statement_Support::Serial_Previous_Where_Statement_Mask->decrementReferenceCount();
      if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask->getReferenceCount() < intSerialArray::getReferenceCountBase())
           delete Where_Statement_Support::Serial_Previous_Where_Statement_Mask; 

   // Now we set the pointers to the deleted array to NULL, just for good habit!
      Where_Statement_Support::Serial_Previous_Where_Statement_Mask = NULL;
      Last_Mask = NULL; 
   }

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Bottom of elsewhere_function (X)! \n");
#endif

   // Force first iteration through for loop used to implement where macro!
   return START_WHERE_MACRO;
} 
 
#endif

//=======================================================================

int endwhere_function ()
{
   // ... (9/20/96,kdb) added functionality for SerialArrays by repeating and
   //  modifying previous code ...
   // Must include functionality for SerialArray for P++ -- not implemented yet!

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Top of endwhere_function! \n");
#endif

   if (Where_Statement_Support::Where_Statement_Mask != NULL)
   {
      // error checking
#if defined (SERIAL_APP)
#if COMPILE_DEBUG_STATEMENTS
      if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
      {
	 printf ("ERROR: both Serial_Where_Statement_Mask and\n");
	 printf ("   Where_Statement_Mask are nonnull in endwhere.\n");
	 APP_ABORT();
      }
#endif
      APP_ASSERT(Where_Statement_Support::Serial_Where_Statement_Mask == NULL);
#endif
      if (Where_Statement_Support::Previous_Where_Statement_Mask != NULL)
      {
         printf ("ERROR: In endwhere: system Previous_Mask is NOT NULL! \n");
         APP_ABORT();
      } 
         
      Where_Statement_Support::Previous_Where_Statement_Mask = 
         Where_Statement_Support::Where_Statement_Mask;
      Where_Statement_Support::Where_Statement_Mask = NULL;

#if COMPILE_DEBUG_STATEMENTS
      // Where_Statement_Support::Previous_Where_Statement_Mask->view
      //("In endwhere_function: Previous_Where_Statement_Mask");
#endif

      // In the beginning the history Does Not Exist!
      if (Where_Statement_Support::Previous_Where_Statement_Mask_History == NULL)
      {
         // So the first Mask becomes the history!

#if COMPILE_DEBUG_STATEMENTS
         if (APP_DEBUG > 0)
            printf ("Where_Statement_Support::Previous_Where_Statement_Mask_History == NULL \n");
#endif

         // Make sure that if the original Mask input to "where" was a temporary 
	 // it is made persistant!
         Where_Statement_Support::Previous_Where_Statement_Mask_History = 
            new intArray ( *Where_Statement_Support::Previous_Where_Statement_Mask);
         APP_ASSERT (Where_Statement_Support::Previous_Where_Statement_Mask_History != NULL);
      }
      else
      {
         // The non-zero elements in either mask is the new mask history!
         // This is what prevents any mask element that is non-zero from being
         // used over again!

#if COMPILE_DEBUG_STATEMENTS
         if (APP_DEBUG > 0)
            printf ("Where_Statement_Support::Previous_Where_Statement_Mask_History != NULL \n");
#endif

         *Where_Statement_Support::Previous_Where_Statement_Mask_History = 
#if 0
      // DQ (6/30/2005): these are conflicting with operators in g++ (turn them off for now!)
             *Where_Statement_Support::Previous_Where_Statement_Mask_History || 
             *Where_Statement_Support::Previous_Where_Statement_Mask;
#else
             NULL;
#endif
      }
   } 
#if defined (SERIAL_APP)
   else if (Where_Statement_Support::Serial_Where_Statement_Mask != NULL)
   {
      // error checking
      if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask != NULL)
      {
         printf ("ERROR: In endwhere: system Previous_Mask is NOT NULL! \n");
         APP_ABORT();
      } 
         
      Where_Statement_Support::Serial_Previous_Where_Statement_Mask = 
         Where_Statement_Support::Serial_Where_Statement_Mask;
      Where_Statement_Support::Serial_Where_Statement_Mask = NULL;

      // In the beginning the history Does Not Exist!
      if (Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History 
	  == NULL)
      {
         // So the first Mask becomes the history!

#if COMPILE_DEBUG_STATEMENTS
         if (APP_DEBUG > 0)
            printf ("Serial_Previous_Where_Statement_Mask_History == NULL \n");
#endif

         // Make sure that if the original Mask input to "where" was a temporary 
	 // it is made persistant!
         Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History = 
            new intSerialArray 
	       ( *Where_Statement_Support::Serial_Previous_Where_Statement_Mask);
         APP_ASSERT (Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History != NULL);
      }
      else
      {
         // The non-zero elements in either mask is the new mask history!
         // This is what prevents any mask element that is non-zero from being
         // used over again!

#if COMPILE_DEBUG_STATEMENTS
         if (APP_DEBUG > 0)
            printf ("Serial_Previous_Where_Statement_Mask_History != NULL \n");
#endif

         *Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History = 
           *Where_Statement_Support::Serial_Previous_Where_Statement_Mask_History|| 
           *Where_Statement_Support::Serial_Previous_Where_Statement_Mask;
      }
   }
#endif
   else
   {
      // Where_Statement_Support::Where_Statement_Mask == NULL and
      // Where_Statement_Support::Serial_Where_Statement_Mask == NULL

      printf ("ERROR: In endwhere: system Mask is NOT set");
      printf ("(unset then set; likely a missing endwhere() \n");
      APP_ABORT();
   } 

#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      printf ("Bottom of endwhere_function! \n");
#endif

   // We return a value to kill an subsequent iterations of the for
   // loop that we use as a macro to implement the where function.
   return STOP_WHERE_MACRO;
} 
//=======================================================================


