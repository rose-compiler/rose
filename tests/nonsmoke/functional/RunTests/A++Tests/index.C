// P++ requires that either COMPILE_PPP or COMPILE_SERIAL_APP be defined
// where as A++ defines COMPILE_APP in config.h (P++ can't do that and
// still maintain simplicity using AutoMake)
// So if we don't see COMPILE_APP then we assume we are compiling for P++!
#if !defined(COMPILE_APP)
#define COMPILE_PPP
#endif

#include "A++.h"

/* **********************************************************************
   *************  Internal_Indirect_Addressing_Index class  *************
   ********************************************************************** 
*/

void
Internal_Indirect_Addressing_Index::Test_Consistency ( const char* Label ) const
   {
  // This function does error checking for the Index object at construction!

     APP_ASSERT ( (intArrayInStorage == FALSE) || (IndexInStorage == FALSE) || (intSerialArrayInStorage == FALSE) );

     Index_Data.Test_Consistency (Label);
     if (intArrayInStorage == TRUE)
        {
          APP_ASSERT (IndirectionArrayPointer != NULL);
          IndirectionArrayPointer->Test_Consistency (Label);
          APP_ASSERT (Scalar == -42000);
          APP_ASSERT (Index_Data.getMode() == Null_Index);
        }
       else
        {
          APP_ASSERT (intArrayInStorage == FALSE);
          APP_ASSERT (IndirectionArrayPointer == NULL);
        }

     if (intSerialArrayInStorage == TRUE)
        {
          APP_ASSERT (IndirectionSerialArrayPointer != NULL);
          IndirectionSerialArrayPointer->Test_Consistency (Label);
          APP_ASSERT (Scalar == -42000);
          APP_ASSERT (Index_Data.getMode() == Null_Index);
        }
       else
        {
          APP_ASSERT (intSerialArrayInStorage == FALSE);
          APP_ASSERT (IndirectionSerialArrayPointer == NULL);
        }

     if (Index_Data.getMode() != Null_Index)
        {
          Index_Data.Test_Consistency (Label);
          APP_ASSERT (intArrayInStorage == FALSE);
          APP_ASSERT (IndirectionArrayPointer == NULL);
          APP_ASSERT (intSerialArrayInStorage == FALSE);
          APP_ASSERT (IndirectionSerialArrayPointer == NULL);
          APP_ASSERT (Scalar == -42000);
        }
       else
        {
          APP_ASSERT (Index_Data.getMode() == Null_Index);
        }

   }

Internal_Indirect_Addressing_Index::~Internal_Indirect_Addressing_Index ()
   {
#if COMPILE_DEBUG_STATEMENTS
  if (APP_DEBUG > 1)
       printf ("Inside of ~Internal_Indirect_Addressing_Index() \n");
#endif

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from ~Internal_Indirect_Addressing_Index()");
#endif

#if 1
  // Comment out to test possible bug
     if (intArrayInStorage == TRUE)
        {
          APP_ASSERT (IndirectionArrayPointer != NULL);
          IndirectionArrayPointer->decrementReferenceCount();
          if (IndirectionArrayPointer->getReferenceCount() < intArray::getReferenceCountBase())
               delete IndirectionArrayPointer;
        }

     if (intSerialArrayInStorage == TRUE)
        {
          APP_ASSERT (IndirectionSerialArrayPointer != NULL);
          IndirectionSerialArrayPointer->decrementReferenceCount();
          if (IndirectionSerialArrayPointer->getReferenceCount() < intArray::getReferenceCountBase())
               delete IndirectionSerialArrayPointer;
        }
#else
     printf ("Commented out indirectionArrayPointer in Internal_Indirect_Addressing_Index destructor! \n");
#endif
   }

int
Internal_Indirect_Addressing_Index::operator() ( int i ) const
   {
  // Scalar indexing operator (used in indirect addressing support to fill in PGSLib data structures)

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::operator()");
#endif

     int returnValue = 0;
     if (intArrayInStorage == TRUE)
        {
       // return IndirectionArrayPointer->operator()(i);
          returnValue = (*IndirectionArrayPointer)(i);
        }
       else
        {
          if (IndexInStorage == TRUE)
             {
            // Comment out until we can build the int Internal_Index::operator()(int) operator!
               returnValue = Index_Data(i);
             }
            else
             {
            // Could still be a intSerialArray
               if (intSerialArrayInStorage == TRUE)
                  {
                    returnValue = (*IndirectionSerialArrayPointer)(i);
                  }
                 else
                  {
                 // Must be a scalar (no other option)
                    returnValue = Scalar;
                  }
             }
        }

     return returnValue;
   }

#if defined(PPP) || defined(SERIAL_APP)
Internal_Indirect_Addressing_Index* Internal_Indirect_Addressing_Index::
getPointerToLocalPart (const SerialArray_Domain_Type & X, int Axis) const
   {
#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::getPointerToLocalPart()");
#endif

     int Temp_Base   = 0;
     int Temp_Count  = 0;
     int Temp_Stride = 0;

     if (!X.Is_A_Null_Array && IndexInStorage)
        {
          return new Internal_Indirect_Addressing_Index (*Index_Data.getPointerToLocalPart(X,Axis));
        }
       else
        {
          return new Internal_Indirect_Addressing_Index (Temp_Base, Temp_Count, Temp_Stride);
        }
   }
#endif

int
Internal_Indirect_Addressing_Index::getLength() const
   {
  // Return the lenght of the indirect index (depends on the type of index stored internally)

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::getLength()");
#endif

  // display("Inside of Internal_Indirect_Addressing_Index::getLength()");

     int returnValue = 0;
     if (IndexInStorage)
        {
          returnValue = Index_Data.getLength();
        }
       else
        {
          if (intArrayInStorage)
             {
               APP_ASSERT(IndirectionArrayPointer != NULL);
            // get the length of the indirection vector (which is 1D by definition)
               returnValue = IndirectionArrayPointer->getLength(0);
             }
            else
             {
            // Could still be a intSerialArray
               if (intSerialArrayInStorage == TRUE)
                  {
                    returnValue = IndirectionSerialArrayPointer->getLength(0);
                  }
                 else
                  {
                 // Must be a scalar (no other option)
                    returnValue = 1;
                  }
             }
        }

     return returnValue;
   }

int
Internal_Indirect_Addressing_Index::getBase() const
   {
  // Return the lenght of the indirect index (depends on the type of index stored internally)

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::getBase()");
#endif

  // display("Inside of Internal_Indirect_Addressing_Index::getLength()");

     int returnValue = 0;
     if (IndexInStorage)
        {
          returnValue = Index_Data.getBase();
        }
       else
        {
          if (intArrayInStorage)
             {
               APP_ASSERT(IndirectionArrayPointer != NULL);
            // get the length of the indirection vector (which is 1D by definition)
               returnValue = IndirectionArrayPointer->getBase(0);
             }
            else
             {
            // Could still be a intSerialArray
               if (intSerialArrayInStorage == TRUE)
                  {
                    returnValue = IndirectionSerialArrayPointer->getBase(0);
                  }
                 else
                  {
                 // Must be a scalar (no other option)
                    returnValue = Scalar;
                  }
             }
        }

     return returnValue;
   }

int
Internal_Indirect_Addressing_Index::getBound() const
   {
  // Return the lenght of the indirect index (depends on the type of index stored internally)

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::getBound()");
#endif

  // display("Inside of Internal_Indirect_Addressing_Index::getLength()");

     int returnValue = 0;
     if (IndexInStorage)
        {
          returnValue = Index_Data.getBound();
        }
       else
        {
          if (intArrayInStorage)
             {
               APP_ASSERT(IndirectionArrayPointer != NULL);
            // get the length of the indirection vector (which is 1D by definition)
               returnValue = IndirectionArrayPointer->getBound(0);
             }
            else
             {
            // Could still be a intSerialArray
               if (intSerialArrayInStorage == TRUE)
                  {
                    returnValue = IndirectionSerialArrayPointer->getBound(0);
                  }
                 else
                  {
                 // Must be a scalar (no other option)
                    returnValue = Scalar;
                  }
             }
        }

     return returnValue;
   }

int
Internal_Indirect_Addressing_Index::getStride() const
   {
  // Return the length of the indirect index (depends on the type of index stored internally)

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::getStride()");
#endif

  // display("Inside of Internal_Indirect_Addressing_Index::getLength()");

     int returnValue = 0;
     if (IndexInStorage)
        {
          returnValue = Index_Data.getStride();
        }
       else
        {
          if (intArrayInStorage)
             {
               APP_ASSERT(IndirectionArrayPointer != NULL);
            // get the length of the indirection vector (which is 1D by definition)
               returnValue = IndirectionArrayPointer->getStride(0);
             }
            else
             {
               if (intSerialArrayInStorage)
                  {
                    APP_ASSERT(IndirectionArrayPointer != NULL);
                 // get the length of the indirection vector (which is 1D by definition)
                    returnValue = IndirectionSerialArrayPointer->getStride(0);
                  }
                 else
                  {
                 // Case of a scalar
                    returnValue = 1;
                  }
            // Case of a scalar
             }
        }

     return returnValue;
   }

void
Internal_Indirect_Addressing_Index::display( const char* label ) const
   {
  // Return the lenght of the indirect index (depends on the type of index stored internally)

     printf ("Internal_Indirect_Addressing_Index::display(%s) \n",label);
     printf ("     IndexInStorage          = %s \n",(IndexInStorage)          ? "TRUE" : "FALSE");
     printf ("     intArrayInStorage       = %s \n",(intArrayInStorage)       ? "TRUE" : "FALSE");
     printf ("     intSerialArrayInStorage = %s \n",(intSerialArrayInStorage) ? "TRUE" : "FALSE");

     if (IndexInStorage)
        {
          Index_Data.display(label);
        }
       else
        {
          if (intArrayInStorage)
             {
               APP_ASSERT(IndirectionArrayPointer != NULL);
            // get the length of the indirection vector (which is 1D by definition)
               IndirectionArrayPointer->display(label);
             }
            else
             {
               if (intSerialArrayInStorage)
                  {
                    APP_ASSERT(IndirectionArrayPointer != NULL);
                 // get the length of the indirection vector (which is 1D by definition)
                    IndirectionSerialArrayPointer->display(label);
                  }
                 else
                  {
                 // Case of a scalar
                    printf ("Scalar Value = %d \n",Scalar);
                  }
             }
        }
   }

Internal_Indirect_Addressing_Index &
Internal_Indirect_Addressing_Index::operator= (const Internal_Indirect_Addressing_Index & X)
   {
  // X.display("Inside of Internal_Indirect_Addressing_Index::operator= (X) (X)");

#if COMPILE_DEBUG_STATEMENTS
     X.Test_Consistency("Called from Internal_Indirect_Addressing_Index::operator=()");
#endif

     Scalar                        = X.Scalar;
     IndexInStorage                = X.IndexInStorage;
     Index_Data                    = X.Index_Data;
     intArrayInStorage             = X.intArrayInStorage;
     IndirectionArrayPointer       = X.IndirectionArrayPointer;
     intSerialArrayInStorage       = X.intSerialArrayInStorage;
     IndirectionSerialArrayPointer = X.IndirectionSerialArrayPointer;

  // display("Inside of Internal_Indirect_Addressing_Index::operator= (X) (THIS)");

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::operator=()");
#endif

     return *this;
   }


void
Internal_Indirect_Addressing_Index::setIndex( Internal_Index & X )
   {
  // Set the Internal_Index object within the Internal_Indirect_Addressing_Index object

#if COMPILE_DEBUG_STATEMENTS
     X.Test_Consistency("Called from Internal_Indirect_Addressing_Index::setIndex(Internal_Index &)");
#endif

     if (intArrayInStorage == TRUE)
        {
       // fixed call to delete so that it uses the reference counting mechanism
          APP_ASSERT(IndirectionArrayPointer != NULL);
          IndirectionArrayPointer->decrementReferenceCount();
          if (IndirectionArrayPointer->getReferenceCount() < intArray::getReferenceCountBase())
               delete IndirectionArrayPointer;
          IndirectionArrayPointer = NULL;
        }

     if (intSerialArrayInStorage == TRUE)
        {
       // fixed call to delete so that it uses the reference counting mechanism
          APP_ASSERT(IndirectionSerialArrayPointer != NULL);
          IndirectionSerialArrayPointer->decrementReferenceCount();
          if (IndirectionSerialArrayPointer->getReferenceCount() < intArray::getReferenceCountBase())
               delete IndirectionSerialArrayPointer;
          IndirectionSerialArrayPointer = NULL;
        }

     IndexInStorage                = TRUE;
     intArrayInStorage             = FALSE;
     IndirectionArrayPointer       = NULL;
     intSerialArrayInStorage       = FALSE;
     IndirectionSerialArrayPointer = NULL;
     Index_Data                    = X;

  // Set this to a value that should generate an error if used!
     Scalar                        = -42000;

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::setIndex(Internal_Index &)");
#endif
   }

void
Internal_Indirect_Addressing_Index::setIntArray( intArray* Xptr )
   {
  // Set the intArray object within the Internal_Indirect_Addressing_Index object

#if COMPILE_DEBUG_STATEMENTS
  if (APP_DEBUG > 0)
       printf ("Inside of Internal_Indirect_Addressing_Index::setIntArray( intArray* Xptr ) \n");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Xptr != NULL)
          Xptr->Test_Consistency("Called from Internal_Indirect_Addressing_Index::setIntArray(intArray*)");
#endif

     if (intArrayInStorage == TRUE)
        {
       // printf ("In Internal_Indirect_Addressing_Index::setIntArray( intArray* Xptr ): intArrayInStorage == TRUE: deleting reference to intArray \n");

       // fixed call to delete so that it uses the reference counting mechanism
          APP_ASSERT( IndirectionSerialArrayPointer == NULL );
          APP_ASSERT(IndirectionArrayPointer != NULL);
          IndirectionArrayPointer->decrementReferenceCount();
          if (IndirectionArrayPointer->getReferenceCount() < intArray::getReferenceCountBase())
             {
               delete IndirectionArrayPointer;
             }
            else
             {
            // printf ("NOT DELETED: new reference count = %d \n",IndirectionArrayPointer->getReferenceCount());
#if COMPILE_DEBUG_STATEMENTS
               if (Diagnostic_Manager::getReferenceCountingReport() > 0)
                  {
                 // This mechanism outputs reports which allow us to trace the reference counts
                    APP_ASSERT(IndirectionArrayPointer != NULL);
                    IndirectionArrayPointer->displayReferenceCounts();
                  }
               IndirectionArrayPointer->Test_Consistency ("Testing intArray indexing Internal_Indirect_Addressing_Index::setIntArray(intArray*)");
#endif
             }
          IndirectionArrayPointer = NULL;
        }
       else
        {
       // printf ("In Internal_Indirect_Addressing_Index::setIntArray( intArray* Xptr ): intArrayInStorage == FALSE \n");
        }
     
     if (intSerialArrayInStorage == TRUE)
        {
       // fixed call to delete so that it uses the reference counting mechanism
          APP_ASSERT(IndirectionSerialArrayPointer != NULL);
          IndirectionSerialArrayPointer->decrementReferenceCount();
          if (IndirectionSerialArrayPointer->getReferenceCount() < intArray::getReferenceCountBase())
               delete IndirectionSerialArrayPointer;
          IndirectionSerialArrayPointer = NULL;
        }

     IndexInStorage                = FALSE;
     intSerialArrayInStorage       = FALSE;
     IndirectionSerialArrayPointer = NULL;
     intArrayInStorage             = (Xptr != NULL) ? TRUE : FALSE;
     IndirectionArrayPointer       = Xptr;

     if (Xptr != NULL)
          Xptr->incrementReferenceCount();

     Index_Data                    = Index(0,0,1,Null_Index);

     APP_ASSERT (Index_Data.getMode() == Null_Index);

     Scalar                        = -42000;

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::setIntArray(intArray*)");
#endif
   }

#if (defined(SERIAL_APP) || defined(PPP))
     // Only build this constructor if using P++
void
Internal_Indirect_Addressing_Index::setIntSerialArray( intSerialArray* Xptr )
   {
  // Set the intSerialArray object within the Internal_Indirect_Addressing_Index object

#if COMPILE_DEBUG_STATEMENTS
     if (Xptr != NULL)
          Xptr->Test_Consistency("Called from Internal_Indirect_Addressing_Index::setIntSerialArray(intSerialArray*)");
#endif

     if (intSerialArrayInStorage == TRUE )
        {
          APP_ASSERT( IndirectionArrayPointer == NULL );
          APP_ASSERT( IndirectionSerialArrayPointer != NULL );
          IndirectionSerialArrayPointer->decrementReferenceCount();
          if( IndirectionSerialArrayPointer->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
               delete IndirectionSerialArrayPointer;
             }
          IndirectionSerialArrayPointer = NULL;
        } // end of intSerialArrayInStorage == TRUE
  
     if (intArrayInStorage == TRUE)
        {
       // fixed call to delete so that it uses the reference counting mechanism
          APP_ASSERT(IndirectionArrayPointer != NULL);
          IndirectionArrayPointer->decrementReferenceCount();
          if (IndirectionArrayPointer->getReferenceCount() < intArray::getReferenceCountBase())
               delete IndirectionArrayPointer;
          IndirectionArrayPointer = NULL;
        }

     IndexInStorage                = FALSE;
     intSerialArrayInStorage       = (Xptr != NULL) ? TRUE : FALSE;
     IndirectionSerialArrayPointer = Xptr;

     if (Xptr != NULL)
          Xptr->incrementReferenceCount();

     intArrayInStorage             = FALSE;
     IndirectionArrayPointer       = NULL;
     Index_Data                    = Index(0,0,1,Null_Index);
     Scalar                        = -42000;

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index::setIntSerialArray(intSerialArray*)");
#endif
   }
#endif

Internal_Indirect_Addressing_Index::Internal_Indirect_Addressing_Index (const intArray & X) 
   : Scalar(-42000), Index_Data(0,0,1,Null_Index), 
     IndirectionArrayPointer(&((intArray &) X)), 
     intArrayInStorage(TRUE), IndexInStorage(FALSE),
     intSerialArrayInStorage(FALSE), 
     IndirectionSerialArrayPointer(NULL)
   {
     X.incrementReferenceCount();

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index constructor (intArray)");
#endif
   }

#if (defined(SERIAL_APP) || defined(PPP))
     // Only build this constructor if using P++
Internal_Indirect_Addressing_Index::Internal_Indirect_Addressing_Index (const intSerialArray & X) 
   : Scalar(-42000), Index_Data(0,0,1,Null_Index),
     IndirectionArrayPointer(NULL),
     intArrayInStorage(FALSE), IndexInStorage(FALSE),
     intSerialArrayInStorage(TRUE),
     IndirectionSerialArrayPointer(&((intSerialArray &) X))
   {
     X.incrementReferenceCount();

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index constructor (intSerialArray)");
#endif
   }
#endif

// Copy constructor
Internal_Indirect_Addressing_Index::Internal_Indirect_Addressing_Index (const Internal_Indirect_Addressing_Index & X)
   : Scalar(X.Scalar), Index_Data(X.Index_Data), 
     IndirectionArrayPointer(X.IndirectionArrayPointer),
     intArrayInStorage(X.intArrayInStorage),
     IndexInStorage(X.IndexInStorage),
     intSerialArrayInStorage(X.intSerialArrayInStorage),
     IndirectionSerialArrayPointer(X.IndirectionSerialArrayPointer)
   {

#if COMPILE_DEBUG_STATEMENTS
     Test_Consistency("Called from Internal_Indirect_Addressing_Index COPY constructor");
#endif
   }


/* **********************************************************************
   ***************************  Internal_Index class  ****************************
   ********************************************************************** 
*/

// The default is OFF (so users must turn it on explicitly to find errors)
bool Internal_Index::Index_Bounds_Checking = FALSE;

Internal_Index &
Internal_Index::operator+= ( int i )
   {
     Base  += i;
     Bound += i;
     return *this;
   }

Internal_Index &
Internal_Index::operator-= ( int i )
   {
     Base  -= i;
     Bound -= i;
     return *this;
   }

Internal_Index &
Internal_Index::operator*= ( int i )
   {
     Base   *= i;
     Bound   = (Base+Count)-1;
     Stride *= i;
     return *this;
   }

Internal_Index &
Internal_Index::operator/= ( int i )
   {
     Base   /= i;
     Bound   = (Base+Count)-1;
     Stride /= i;
     return *this;
   }


int
Internal_Index::operator() ( int i ) const
   {
  // Scalar indexing operator (used in indirect addressing support to fill in PGSLib data structures)
  // enum Index_Mode_Type { Index_Triplet = 100, Null_Index = 101, All_Index = 102 };

     int returnValue = 0;
     if ( Index_Mode == Index_Triplet )
        {
       // (2/27/2000: DQ) The correct indexing of an Index or Range
       // object is (strangely enough) to return the input!

       // returnValue = (i*Stride)-Base;
       // returnValue = ((i/Stride)-(Base/Stride))*Stride + Base;

       // We need to make sure that the index values are valid elements of the Index or Range object
          APP_ASSERT ( (i >= Base) && (i <= Bound) );
          APP_ASSERT ( (i-Base) % Stride == 0 );

          returnValue = i;
        }
       else
        {
          if (Index_Mode == Null_Index)
             {
               printf ("Can't index an index of mode Null_Index (it does not make sense)! \n");
               APP_ABORT();
             }
            else
             {
               if (Index_Mode == All_Index)
                  {
                    printf ("Can't index an index of mode All_Index (it does not make sense)! \n");
                    APP_ABORT();
                  }
                 else
                  {
                    printf ("Can't figure out what mode of index this is in Internal_Index::operator() \n");
                    APP_ABORT();
                  }
             }
        }

     return returnValue;
   }


void
Internal_Index::Test_Consistency ( const char* Label ) const
   {
#if (EXTRA_ERROR_CHECKING == FALSE)
     printf ("A++ version (EXTRA_ERROR_CHECKING) was incorrectly built since Internal_Index::Test_Consistency (%s) was called! \n",Label);
     APP_ABORT();
#endif

     if (APP_DEBUG > 5)
          printf ("Inside of Internal_Index::Test_Consistency (%s) \n",Label);
     Consistency_Check(Label);
   }

void
Internal_Index::Consistency_Check( const char* Label ) const
   {
  // This function does error checking for the Index object at construction!

     if (Index_Mode == Index_Triplet)
        {
          if ( Stride == 0 )
             {
               printf ("ERROR in Internal_Index::Consistency_Check(%s) -- (Index_Mode == Index_Triplet && Stride == 0) this forces loops using Internal_Index to iterate forever! \n",Label);
               APP_ABORT();
             }

#if 1
       // Bill Henshaw uses magic range objects which are inconsistant so this 
       // error checking is temporarily disabled.
          if ( (Base > Bound) && (Stride > 0) )
             {
               printf ("ERROR in Internal_Index::Consistency_Check(%s) -- (Base=%d) > (Bound=%d) and (Stride=%d) in Internal_Index object where Index_Mode == Index_Triplet \n",Label,Base,Bound,Stride);
               printf ("Hint: use the larger as the Base and use negative Stride! \n");
               APP_ABORT();
             }
#endif
          if ( (Base < Bound) && (Stride < 0) )
             {
               printf ("ERROR in Internal_Index::Consistency_Check(%s) -- (Base=%d) < (Bound=%d) and (Stride=%d) in Internal_Index object where Index_Mode == Index_Triplet \n",Label,Base,Bound,Stride);
               printf ("Hint: use the larger as the Base and use negative Stride! \n");
               APP_ABORT();
             }

          if (Count == 0)
             {
               printf ("ERROR in Internal_Index::Consistency_Check(%s) -- Count == 0 is incorrect when Index_Mode == Index_Triplet (Index_Mode should be Null_Index)! \n",Label);
               APP_ABORT();
             }
        }
       else
        {
          if (Index_Mode == Null_Index)
             {
            // Is there a test for a Null_Index?
             }
            else
             {
               if (Index_Mode == All_Index)
                  {
                 // Is there a test for a All_Index?
                  }
                 else
                  {
                    printf ("ERROR: in Internal_Index::Consistency_Check(%s) -- Index_Mode unknown (Index_Mode = %d) \n",
                         Label,(int)Index_Mode);
                    APP_ABORT();
                  }
             }
        }

     if (Count < 0)
        {
          printf ("ERROR in Internal_Index::Consistency_Check(%s) -- Count < 0 is meaningless! \n",Label);
          APP_ABORT();
        }

     if (Stride == 0)
        {
          printf ("ERROR in Internal_Index::Consistency_Check(%s) -- Stride == 0 is meaningless! \n",Label);
          APP_ABORT();
        }

#if 1
  // We now permit the specification of negative stride Internal_Index objects.  But until we have checked
  // to see how they work with the rest of the A++/P++ code one must be carefull.
     if (Stride < 0)
        {
          printf ("(Sorry, not yet implemented!) in Internal_Index::Consistency_Check(%s) -- Stride < 0 \n",Label);
          APP_ABORT();
        }
#endif
   }

void
Internal_Index::setBoundsCheck ( On_Off_Type On_Off )
   {
     // This function does nothing since currently index
     // checking is turned on by a preprocessor statement!

     if ( (On_Off == ON) || (On_Off == On) || (On_Off == on) )
        {
          printf ("A++ Internal_Index bounds checking: ON \n");
          Index_Bounds_Checking = TRUE;
        }
       else
        {
          printf ("A++ Internal_Index bounds checking: OFF \n");
          Index_Bounds_Checking = FALSE;
        }
   }

void
Internal_Index::list( const char* Label ) const
   {
     printf ("Internal_Index::list() (mode=%s)  -- Label = %s \n",getModeString(),Label);

  // save the current state and turn off the output of node number prefix to all strings
     bool printfState = Communication_Manager::getPrefixParallelPrintf();
     Communication_Manager::setPrefixParallelPrintf(FALSE);

  // Note that the values of Base Bound etc are meaningless unless Index_Mode == Index_Triplet!
     if (Index_Mode == Index_Triplet)
        {
       // printf ("\n");
          printf ("Internal_Index Data: ");
          for (int i=Base; i <= Bound; i += Stride)
             {
               printf ("%d ",i);
             }
          printf ("\n");
        }
  // reset output of node number prefix to all strings
     Communication_Manager::setPrefixParallelPrintf(printfState);

  // If we have to time to display the values then
  // we should be able to check their correctness.
     Consistency_Check(Label);
   }

void
Internal_Index::display( const char* Label ) const
   { 
     printf ("Internal_Index::display()  -- Label = %s \n",Label);

  // save the current state and turn off the output of node number prefix to all strings
     bool printfState = Communication_Manager::getPrefixParallelPrintf();
     Communication_Manager::setPrefixParallelPrintf(FALSE);

  // printf ("\n");
  // printf ("Index Mode is %s \n",(Index_Mode == Index_Triplet) ? "INDEX_TRIPLET" : 
  //                               ((Index_Mode == All_Index) ? "ALL_INDEX" : "NULL_INDEX") );
     if (Index_Mode == Index_Triplet)
        {
          printf ("Index Mode is INDEX_TRIPLET \n");
        }
       else
        {
          if (Index_Mode == All_Index)
             {
               printf ("Index Mode is ALL_INDEX \n");
             }
            else
             {
               if (Index_Mode == Null_Index)
                  {
                    printf ("Index Mode is NULL_INDEX \n");
                  }
                 else
                  {
                    printf ("ERROR: Index_Mode not set! (Unknow value: Index_Mode = %d) \n",(int)Index_Mode);
                    APP_ABORT();
                  }
             }
        }

  // Note that the values of Base Bound etc are meaningless unless Index_Mode == Index_Triplet!
     if (Index_Mode == Index_Triplet)
        {
          printf ("Base   = %d  ",Base);
          printf ("Bound  = %d  ",Bound);
          printf ("Stride = %d  ",Stride);
          printf ("Count  = %d  ",Count);
          printf ("Length = %d \n",length());

       // printf ("\n");
          printf ("Internal_Index Data: ");
          for (int i=Base; i <= Bound; i += Stride)
             {
               printf ("%d ",i);
             }
          printf ("\n");
        }
       else
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
             {
               printf ("Base   = %d  ",Base);
               printf ("Bound  = %d  ",Bound);
               printf ("Stride = %d  ",Stride);
               printf ("Count  = %d  ",Count);
               printf ("Length = %d \n",length());
             }
#endif
        }

  // reset output of node number prefix to all strings
     Communication_Manager::setPrefixParallelPrintf(printfState);

     printf ("\n");

  // If we have to time to display the values then
  // we should be able to check their correctness.
     Consistency_Check(Label);
   }

char*
Internal_Index::getModeString() const
   {
     switch (Index_Mode)
        {
          case Index_Triplet:
               return "TRIPLET INDEX";
               break;
          case Null_Index:
               return "NULL INDEX";
               break;
          case All_Index:
               return "ALL INDEX";
               break;
          default:
               printf ("ERROR: in Index_Mode = %d \n",Index_Mode);
               APP_ABORT();
        }

     printf ("ERROR: in Index_Mode = %d \n",Index_Mode);
     APP_ABORT();
     return "ERROR";
   }


#if defined(PPP) || defined(SERIAL_APP)
void
Internal_Index::getLocalPartData ( 
      const SerialArray_Domain_Type & X , int Axis , 
      int & Output_Base, int & Output_Count, int & Output_Stride ) const
{
  // This code is incorrect for the case of stride != 1
  // ... change (9/1/96,kdb) modify code to work with new interpretation
  //  due to option A for scalar indexing and views of views.
  //  A large section of this code has been removed instead of commented
  //  out to simplify ...
  //int Local_Base  = X.Data_Base[Axis] + X.Base[Axis];
  //int Local_Bound = X.Data_Base[Axis] + X.Bound[Axis];
  int Local_Base  = X.User_Base[Axis];
  int Local_Bound = (X.Bound[Axis]-X.Base[Axis])/X.Stride[Axis] + Local_Base;
  int Local_Stride  = X.Stride[Axis];

#if COMPILE_DEBUG_STATEMENTS
  if (APP_DEBUG > 0)
       printf ("Inside of Internal_Index::getLocalPart \n");
#endif

  if (Index_Mode == Index_Triplet)
  {
     // ... change means X is treated like stride 1 even if it isn't ...

     // ... base_offset is the amount the Base is above the Local_Base.
     //  If Base is < Local_Base the Local_Base will be used for
     //  the new base and so the offset is 0.   ...
     int base_offset  = (Base  >= Local_Base)  ? (Base-Local_Base)  : (Stride-((Local_Base-Base)%Stride))%Stride;
     int bound_offset = (Bound <= Local_Bound) ? (Bound-Local_Base) : Local_Bound - ((Stride-((Bound-Local_Bound)%Stride))%Stride) - Local_Base;

     //int New_Base   = Local_Base + base_offset * Local_Stride; 
     //int New_Bound  = Local_Base + bound_offset * Local_Stride; 
     int New_Base   = Local_Base + base_offset; 
     int New_Bound  = Local_Base + bound_offset; 

     if (!(New_Base > Local_Bound) && !(New_Bound < Local_Base))
     {
     // int New_Stride = Stride * Local_Stride;
        int New_Stride = Stride;
        int New_Count  = ((bound_offset - base_offset) / Stride) + 1; 

#if COMPILE_DEBUG_STATEMENTS
        if (APP_DEBUG > 5)
        {
            printf ("Axis  = %d \n",Axis);
            printf ("New_Count  = %d \n",New_Count);
            printf ("New_Stride = %d \n",New_Stride);
            printf ("New_Base = %d New_Bound = %d  \n",New_Base,New_Bound);
            printf ("Local_Base = %d Local_Bound = %d  \n",Local_Base,Local_Bound);
            display("This Index");
            X.display ("X in Index::getLocalPart()");
            Internal_Index Example_Index (New_Base,New_Count,New_Stride);
            Example_Index.display("Example_Index");
        }

        if ((New_Base + ((New_Count-1)*New_Stride)) != New_Bound)
        {
           printf ("ERROR:(New_Base + ((New_Count-1)*New_Stride))!= New_Bound \n");
           printf ("New_Base   = %d \n",New_Base);
           printf ("New_Count  = %d \n",New_Count);
           printf ("New_Stride = %d \n",New_Stride);
           printf ("New_Bound  = %d \n",New_Bound);
        }
#endif
        APP_ASSERT ((New_Base + ((New_Count-1)*New_Stride)) == New_Bound);

        Output_Base   = New_Base;
        Output_Count  = New_Count;
        Output_Stride = New_Stride;
     }
     else
     {
	 // ... there is no intersection ...
         Output_Base   = 0;
         Output_Count  = 0;
         Output_Stride = 1;
     }
  }
  else if (Index_Mode == All_Index)
  {
     int New_Count = ((Local_Bound - Local_Base) / Local_Stride) + 1;
     Output_Base   = Local_Base;
     Output_Count  = New_Count;
     Output_Stride = Stride * Local_Stride;
  }
  else
  {
     APP_ASSERT(Index_Mode == Null_Index);
  // Local part of a Null_Index is a Null_Index
     Output_Base   = 0;
     Output_Count  = 0;
     Output_Stride = 1;
  }
}

Internal_Index
Internal_Index::getLocalPart ( const SerialArray_Domain_Type & X , int Axis ) const
   {
     int Temp_Base   = 0;
     int Temp_Count  = 0;
     int Temp_Stride = 0;

     getLocalPartData (X,Axis,Temp_Base,Temp_Count,Temp_Stride);

     return Internal_Index (Temp_Base,Temp_Count,Temp_Stride);
   }

Internal_Index*
Internal_Index::getPointerToLocalPart ( const SerialArray_Domain_Type & X , int Axis ) const
   {
     int Temp_Base   = 0;
     int Temp_Count  = 0;
     int Temp_Stride = 1;

     // ... QUESTION: should X ever be NULL here? ...
     if (!X.Is_A_Null_Array)
          getLocalPartData (X,Axis,Temp_Base,Temp_Count,Temp_Stride);
     //else
	//printf("WARNING: X is NULL in getPointerToLocalPart. \n");

     return new Internal_Index (Temp_Base,Temp_Count,Temp_Stride);
   }

Internal_Index
Internal_Index::operator() ( const Internal_Index & X ) const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of Internal_Index::operator(const Internal_Index & X) \n");
#endif

  // APP_ASSERT(Index_Mode != Null_Index);
  // int Temp_Stride = (Index_Mode == All_Index) ? X.Stride : Stride * X.Stride;
  // return Index ( Base + X.Base, X.Count, Stride * X.Stride );
  // APP_ASSERT (X.Index_Mode != All_Index);

     if (Index_Mode == Index_Triplet)
        {
#if COMPILE_DEBUG_STATEMENTS
          if (APP_DEBUG > 0)
               printf ("Index_Mode == Index_Triplet \n");
#endif
          if (X.Index_Mode == Index_Triplet)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("X.Index_Mode == Index_Triplet \n");
#endif
            // Use the new indexing (absolute indexing)
            // int Intersection_Base   = (Base > X.Base) ? Base : X.Base;
            // int Intersection_Bound  = (Bound < X.Bound) ? Bound : X.Bound;
            // int Intersection_Stride = Stride * X.Stride;
            // int Intersection_Size   = ((Intersection_Bound - Intersection_Base) / Intersection_Stride) + 1;

            // ... (8/29/96,kdb)bug fix (this doesn't work with strides > 2 ...
            /*
               int Intersection_Base   = ( Base  > X.Base  ) ?  Base  + ((Base-X.Base) % X.Stride)   : X.Base;
               int Intersection_Bound  = ( Bound < X.Bound ) ?  Bound - ((X.Bound-Bound) % X.Stride) : X.Bound;
               int Intersection_Stride = ((X.Stride - Stride) + 1) * Stride;
               int Intersection_Size   = ((Intersection_Bound - Intersection_Base) / Intersection_Stride) + 1;
            */

               // ... find first value greater or equal to Base that is
	       //  specified by X and find its distance from Base ...
               int base_offset = (X.Base >= Base) ? (X.Base-Base) :
		  ( (X.Stride-((Base-X.Base)%X.Stride))%X.Stride );

               // ... find last value specified by X that is less than Bound
	       //   and then its offset from Base ... 
               int bound_in_intersect = (X.Bound <= Bound) ? X.Bound :
		  Bound - ( (X.Stride-((X.Bound-Bound)%X.Stride))%X.Stride );
               int bound_offset = bound_in_intersect-Base;

               int Intersection_Base   = Base + (base_offset) * Stride;
               int Intersection_Bound  = Base + (bound_offset) * Stride;
               int Intersection_Stride = X.Stride * Stride;
               int Intersection_Size   = (Intersection_Bound-Intersection_Base)/Intersection_Stride + 1;

#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 1)
                  {
                    display("Internal_Index::operator() -- this");
                    X.display("Internal_Index::operator() -- X");
                    printf ("base_offset = %d bound_in_intersect = %d  bound_offset = %d \n",
                         base_offset,bound_in_intersect,bound_offset);
		    
                    printf ("Intersection_Base = %d  Intersection_Bound = %d  Intersection_Size = %d  Intersection_Stride = %d \n",
                         Intersection_Base,Intersection_Bound,Intersection_Size,Intersection_Stride);
                  }
            // APP_ASSERT (Intersection_Size >= 1);
#endif
               if (Intersection_Size >= 1)
                  {
                    APP_ASSERT (Intersection_Base + ((Intersection_Size-1) * Intersection_Stride) == Intersection_Bound);
                    return Internal_Index ( Intersection_Base, Intersection_Size, Intersection_Stride );
                  }
                 else
                  {
                 // return a NULL Index object! 
                    return Internal_Index (Intersection_Base, 0, 1);
                  }
             }
            else
             {
               if (X.Index_Mode == All_Index)
                  {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("X.Index_Mode == All_Index \n");
#endif
                 // All of a Index_Triplet is that Index_Triplet
                    return *this;
                  }
                 else
                  {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("X.Index_Mode == Null_Index \n");
#endif
                 // Null_Index of anything is a Null_Index
                    APP_ASSERT (X.Index_Mode == Null_Index);
                    return X;
                  }
             }
        }
       else
        {
          if (Index_Mode == All_Index)
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Index_Mode == All_Index \n");
#endif
               if (X.Index_Mode == Index_Triplet)
                  {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("X.Index_Mode == Index_Triplet \n");
#endif
                 // Indexing of an All_Index is input Index object
                    return X;
                  }
                 else
                  {
                    if (X.Index_Mode == All_Index)
                       {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("X.Index_Mode == All_Index \n");
#endif
                      // Since both Index objects are All_Index mode we can return either one
                      // All_Index of an All_Index is an All_Index
                         return X;
                       }
                      else
                       {
#if COMPILE_DEBUG_STATEMENTS
                         if (APP_DEBUG > 0)
                              printf ("X.Index_Mode == Null_Index \n");
#endif
                      // Case of Null Index Null_Index of and All_Index is a Null_Index
                         APP_ASSERT (X.Index_Mode == Null_Index);
                         return X;
                       }
                  }
             }
            else
             {
#if COMPILE_DEBUG_STATEMENTS
               if (APP_DEBUG > 0)
                    printf ("Index_Mode == Null_Index \n");
#endif
               APP_ASSERT (Index_Mode == Null_Index);
            // Any indexing of a Null_Array is a Null_Array
               return *this;
             }
        }
   }

void
Internal_Index::adjustBase  ( int x )
   {
  // This function adjusts the base by positive or negative amounts of the stride
  // if (Index_Mode != Index_Triplet)
  //      display("WARNING: Index_Mode != Index_Triplet");
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Internal_Index::adjustBase (%d) \n",x);
          display("THIS in Internal_Index::adjustBase()");
        }
#endif

  // This function must work on Null_Array Index objects too
  // APP_ASSERT(Index_Mode == Index_Triplet);
     if (Index_Mode == Index_Triplet)
        {
       // Bugfix (2/1/96) Must explicitly convert to Null_Index if Count <= 0.
       // Base  += x * Stride;
       // Count -= x * Stride;
       // ... bug fix (10/11/96,kdb) x shouldn't be multiplied by stride
       //  for Count ...
       //   if (Count - x * Stride > 0)
          if (Count - x > 0)
             {
               Base  += x * Stride;
               Count -= x;
               //Count -= x * Stride;
             }
            else
             {
               Index_Mode = Null_Index;
               Base       =  0;
               Count      =  0;
               Stride     =  1;
               Bound      = -1;
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     Consistency_Check("Called from Internal_Index::adjustBase(int)");
#endif
   }

// put into header file later
void
Internal_Index::adjustBound ( int x )
   {
  // This function adjusts the bound by positive or negative amounts of the stride
  // if (Index_Mode != Index_Triplet)
  //      display("WARNING: Index_Mode != Index_Triplet");
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("Inside of Internal_Index::adjustBound (%d) \n",x);
          display("THIS in Internal_Index::adjustBound()");
        }
#endif

  // This function must work on Null_Array Index objects too
  // APP_ASSERT(Index_Mode == Index_Triplet);
     if (Index_Mode == Index_Triplet)
        {
       // Bugfix (2/1/96) Must explicitly convert to Null_Index if Count <= 0.
       // Count += x * Stride;
       // Bound += x * Stride;
       // ... bug fix (10/11/96,kdb) x shouldn't be multiplied by stride
       //  for Count ...
       //   if (Count + x * Stride > 0)
          if (Count + x > 0)
             {
               //Count += x * Stride;
               Count += x;
               Bound += x * Stride;
             }
            else
             {
               Index_Mode = Null_Index;
               Base       =  0;
               Count      =  0;
               Stride     =  1;
               Bound      = -1;
             }
        }

#if COMPILE_DEBUG_STATEMENTS
     Consistency_Check("Called from Internal_Index::adjustBound(int)");
#endif
   }

#endif


