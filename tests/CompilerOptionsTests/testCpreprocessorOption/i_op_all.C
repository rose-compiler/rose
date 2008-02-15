

#include "A++.h"
extern "C"
   {
/* machine.h is found in MDI/machine.h through a link in A++/inc lude and P++/inc lude */
#include "machine.h"

   }

void Delete_SerialArray ( const intArray    & parallelArray, intSerialArray*    serialArray, const Array_Conformability_Info_Type *Temporary_Array_Set );
void Delete_SerialArray ( const floatArray  & parallelArray, floatSerialArray*  serialArray, const Array_Conformability_Info_Type *Temporary_Array_Set );
void Delete_SerialArray ( const doubleArray & parallelArray, doubleSerialArray* serialArray, const Array_Conformability_Info_Type *Temporary_Array_Set );











#define INTARRAY
intArray &
operator+ ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("\n\n\n### Inside of operator+ (intArray,intArray) for intArray class: (id=%d) = (id=%d) \n",
               Lhs.Array_ID(),Rhs.Array_ID());
        }

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator+");
     Rhs.Test_Consistency ("Test Rhs in operator+");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs.isTemporary() = %s \n",(Lhs.isTemporary()) ? "TRUE" : "FALSE");
          Lhs.displayReferenceCounts("Lhs in intArray & operator+(intArray,intArray)");
          printf ("intArray: Rhs.isTemporary() = %s \n",(Rhs.isTemporary()) ? "TRUE" : "FALSE");
          Rhs.displayReferenceCounts("Rhs in intArray & operator+(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ( (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
               (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) )
             {
               printf ("Sorry, not implemented: can't mix indirect addressing using where statements and two array (binary) operators!\n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
                  (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator+(intArray,intArray)");
          printf ("intArray: Rhs_SerialArray->isTemporary() = %s \n",(Rhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & operator+(intArray,intArray)");
        }
#endif

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray + *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray + *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator+(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , 
               MDI_i_Add_Array_Plus_Array,
               MDI_i_Add_Array_Plus_Array_Accumulate_To_Operand , intArray::Plus );
#endif
   }


intArray &
intArray::operator-- ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside intArray::operator-- () for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator-- -- Prefix operator");
#endif

     (*this) -= 1;
     return *this;
   }
 
intArray &
intArray::operator-- ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside intArray::operator%s (int=%d) for intArray class! \n","--",x);

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator--(x=0) -- Postfix operator");
#endif

  // Postfix operator always passes zero as argument (strange but true -- See Stroustrup p594)
     APP_ASSERT( x == 0 );
     (*this) -= 1;
     return *this;
   }


#ifdef INTARRAY
/* There is no >>= operator and so the >> must be handled as a special case -- skip it for now */
intArray &
operator>> ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("\n\n\n### Inside of operator>> (intArray,intArray) for intArray class: (id=%d) = (id=%d) \n",
               Lhs.Array_ID(),Rhs.Array_ID());
        }

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator>>");
     Rhs.Test_Consistency ("Test Rhs in operator>>");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs.isTemporary() = %s \n",(Lhs.isTemporary()) ? "TRUE" : "FALSE");
          Lhs.displayReferenceCounts("Lhs in intArray & operator>>(intArray,intArray)");
          printf ("intArray: Rhs.isTemporary() = %s \n",(Rhs.isTemporary()) ? "TRUE" : "FALSE");
          Rhs.displayReferenceCounts("Rhs in intArray & operator>>(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ( (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
               (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) )
             {
               printf ("Sorry, not implemented: can't mix indirect addressing using where statements and two array (binary) operators!\n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
                  (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator>>(intArray,intArray)");
          printf ("intArray: Rhs_SerialArray->isTemporary() = %s \n",(Rhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & operator>>(intArray,intArray)");
        }
#endif

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray >> *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray >> *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator>>(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , 
               MDI_i_BIT_RSHIFT_Array_BitwiseRShift_Array,
               MDI_i_BIT_RSHIFT_Array_BitwiseRShift_Array_Accumulate_To_Operand , intArray::BitwiseRShift );
#endif
   }

intArray &
operator>> ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\n### Inside of operator>> (intArray,int) for intArray class: (id=%d) = scalar \n",Lhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator>>");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator>>(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
               APP_ASSERT(Lhs_SerialArray != NULL);
            // Lhs_SerialArray->displayReferenceCounts("AFTER PCE: *Lhs_SerialArray in intArray & operator>>(intArray,int)");
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator>>(intArray,int)");
        }
#endif

  // (11/27/2000) Added error checking (will not work with indirect addessing later!!!)
     APP_ASSERT(Temporary_Array_Set != NULL);

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator(Temporary_Array_Set,Lhs,Lhs_SerialArray,*Lhs_SerialArray >> x);
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in in intArray & operator>>(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_BIT_RSHIFT_Array_BitwiseRShift_Scalar,
               MDI_i_BIT_RSHIFT_Array_BitwiseRShift_Scalar_Accumulate_To_Operand , intArray::Scalar_BitwiseRShift );
#endif
   }

intArray &
operator>> ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator>> (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator>>");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator>>(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x >> *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator>>(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_BIT_RSHIFT_Scalar_BitwiseRShift_Array,
               MDI_i_BIT_RSHIFT_Scalar_BitwiseRShift_Array_Accumulate_To_Operand , intArray::Scalar_BitwiseRShift );
#endif
   }

#endif

#ifdef INTARRAY
intArray &
intArray::operator&= ( const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator&= (intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator&=");
     Rhs.Test_Consistency ("Test Rhs in intArray::operator&=");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator&=(intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::operator&=(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray  != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs, This_SerialArray, Rhs_SerialArray, *This_SerialArray &= *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#else
     intArray::Abstract_Operator_Operation_Equals ( *this , Rhs ,
               MDI_i_BIT_AND_Array_BitwiseAND_Array_Accumulate_To_Operand , intArray::BitwiseAND_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator&=(intArray)");
        }
#endif

     return *this;
   }

#endif

#ifdef INTARRAY
intArray &
intArray::operator|= ( const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator|= (intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator|=");
     Rhs.Test_Consistency ("Test Rhs in intArray::operator|=");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator|=(intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::operator|=(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray  != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs, This_SerialArray, Rhs_SerialArray, *This_SerialArray |= *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#else
     intArray::Abstract_Operator_Operation_Equals ( *this , Rhs ,
               MDI_i_BIT_OR_Array_BitwiseOR_Array_Accumulate_To_Operand , intArray::BitwiseOR_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator|=(intArray)");
        }
#endif

     return *this;
   }

#endif

#ifdef INTARRAY
intArray &
intArray::operator^= ( const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator^= (intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator^=");
     Rhs.Test_Consistency ("Test Rhs in intArray::operator^=");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator^=(intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::operator^=(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray  != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs, This_SerialArray, Rhs_SerialArray, *This_SerialArray ^= *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#else
     intArray::Abstract_Operator_Operation_Equals ( *this , Rhs ,
               MDI_i_BIT_XOR_Array_BitwiseXOR_Array_Accumulate_To_Operand , intArray::BitwiseXOR_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator^=(intArray)");
        }
#endif

     return *this;
   }

#endif

intArray &
intArray::convertTo_intArray () const
   {
// Used to implement the conversion functions between int float and double arrays

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of convertTo_intArray for intArray class! \n");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operatorconvertTo_intArray");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::convertTo_intArray()");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
	       Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this, This_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this, This_SerialArray);
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (
                    *this, This_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask,
                    Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	          ( *this, This_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask,
                    Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & intArray::convertTo_intArray () \n");
#endif
     intArray & Return_Value = intArray::Abstract_int_Conversion_Operator ( Temporary_Array_Set, *this, This_SerialArray, This_SerialArray->convertTo_intArray() );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;


  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & intArray::convertTo_intArray()");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_int_Conversion_Operator ( *this ,
                   MDI_i_Array_convertTo_intArray_Array_Accumulate_To_Operand ,
                   intArray::convertTo_intArrayFunction );
#endif
   }

floatArray &
intArray::convertTo_floatArray () const
   {
// Used to implement the conversion functions between int float and double arrays

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of convertTo_floatArray for intArray class! \n");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operatorconvertTo_floatArray");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in floatArray & intArray::convertTo_floatArray()");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
	       Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this, This_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this, This_SerialArray);
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (
                    *this, This_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask,
                    Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	          ( *this, This_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask,
                    Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & intArray::convertTo_floatArray () \n");
#endif
     floatArray & Return_Value = intArray::Abstract_float_Conversion_Operator ( Temporary_Array_Set, *this, This_SerialArray, This_SerialArray->convertTo_floatArray() );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;


  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in floatArray & intArray::convertTo_floatArray()");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_float_Conversion_Operator ( *this ,
                   MDI_i_Array_convertTo_floatArray_Array_Accumulate_To_Operand ,
                   intArray::convertTo_floatArrayFunction );
#endif
   }

doubleArray &
intArray::convertTo_doubleArray () const
   {
// Used to implement the conversion functions between int float and double arrays

#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of convertTo_doubleArray for intArray class! \n");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operatorconvertTo_doubleArray");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in doubleArray & intArray::convertTo_doubleArray()");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
	       Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this, This_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this, This_SerialArray);
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (
                    *this, This_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask,
                    Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	          ( *this, This_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask,
                    Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & intArray::convertTo_doubleArray () \n");
#endif
     doubleArray & Return_Value = intArray::Abstract_double_Conversion_Operator ( Temporary_Array_Set, *this, This_SerialArray, This_SerialArray->convertTo_doubleArray() );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;


  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in doubleArray & intArray::convertTo_doubleArray()");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_double_Conversion_Operator ( *this ,
                   MDI_i_Array_convertTo_doubleArray_Array_Accumulate_To_Operand ,
                   intArray::convertTo_doubleArrayFunction );
#endif
   }


intArray &
intArray::operator-= ( const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator-= (intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator-=");
     Rhs.Test_Consistency ("Test Rhs in intArray::operator-=");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator-=(intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::operator-=(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray  != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs, This_SerialArray, Rhs_SerialArray, *This_SerialArray -= *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#else
     intArray::Abstract_Operator_Operation_Equals ( *this , Rhs ,
               MDI_i_Subtract_Array_Minus_Array_Accumulate_To_Operand , intArray::Minus_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator-=(intArray)");
        }
#endif

     return *this;
   }


intArray &
intArray::operator-= ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator-= (int) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator-=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator-=(int)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, This_SerialArray, *This_SerialArray -= x );
     // ... don't need to use macro because Return_Value won't be Mask ...
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;

#else
     intArray::Abstract_Operator_Operation_Equals ( *this , x ,
        MDI_i_Subtract_Array_Minus_Scalar_Accumulate_To_Operand , intArray::Scalar_Minus_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator-=(int)");
        }
#endif

     return *this;
   }


intArray &
intArray::operator- () const
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of unary minus operator operator- for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operatoroperator-");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator-()");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, *this, This_SerialArray->operator-() );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = *this;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, *this, This_SerialArray, This_SerialArray->operator-() );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete This_SerialArray) in intArray & intArray::operator-()");
        }

  // This is the only test we can do on the output!
     Return_Value.Test_Consistency ("Test Return_Value (before delete This_SerialArray) in intArray::operatoroperator-");
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & intArray::operator-()");
        }

  // This is the only test we can do on the output!
     Return_Value.Test_Consistency ("Test Return_Value in intArray::operatoroperator-");
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( *this ,
                   MDI_i_Unary_Minus_Array ,
                   MDI_i_Unary_Minus_Array_Accumulate_To_Operand , intArray::Unary_Minus );
#endif
   } 


intArray &
operator* ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("\n\n\n### Inside of operator* (intArray,intArray) for intArray class: (id=%d) = (id=%d) \n",
               Lhs.Array_ID(),Rhs.Array_ID());
        }

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator*");
     Rhs.Test_Consistency ("Test Rhs in operator*");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs.isTemporary() = %s \n",(Lhs.isTemporary()) ? "TRUE" : "FALSE");
          Lhs.displayReferenceCounts("Lhs in intArray & operator*(intArray,intArray)");
          printf ("intArray: Rhs.isTemporary() = %s \n",(Rhs.isTemporary()) ? "TRUE" : "FALSE");
          Rhs.displayReferenceCounts("Rhs in intArray & operator*(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ( (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
               (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) )
             {
               printf ("Sorry, not implemented: can't mix indirect addressing using where statements and two array (binary) operators!\n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
                  (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator*(intArray,intArray)");
          printf ("intArray: Rhs_SerialArray->isTemporary() = %s \n",(Rhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & operator*(intArray,intArray)");
        }
#endif

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray * *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray * *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator*(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , 
               MDI_i_Multiply_Array_Times_Array,
               MDI_i_Multiply_Array_Times_Array_Accumulate_To_Operand , intArray::Times );
#endif
   }


intArray &
operator* ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\n### Inside of operator* (intArray,int) for intArray class: (id=%d) = scalar \n",Lhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator*");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator*(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
               APP_ASSERT(Lhs_SerialArray != NULL);
            // Lhs_SerialArray->displayReferenceCounts("AFTER PCE: *Lhs_SerialArray in intArray & operator*(intArray,int)");
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator*(intArray,int)");
        }
#endif

  // (11/27/2000) Added error checking (will not work with indirect addessing later!!!)
     APP_ASSERT(Temporary_Array_Set != NULL);

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator(Temporary_Array_Set,Lhs,Lhs_SerialArray,*Lhs_SerialArray * x);
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in in intArray & operator*(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_Multiply_Array_Times_Scalar,
               MDI_i_Multiply_Array_Times_Scalar_Accumulate_To_Operand , intArray::Scalar_Times );
#endif
   }


intArray &
operator* ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator* (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator*");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator*(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x * *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator*(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_Multiply_Scalar_Times_Array,
               MDI_i_Multiply_Scalar_Times_Array_Accumulate_To_Operand , intArray::Scalar_Times );
#endif
   }


intArray &
intArray::operator*= ( const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator*= (intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator*=");
     Rhs.Test_Consistency ("Test Rhs in intArray::operator*=");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator*=(intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::operator*=(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray  != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs, This_SerialArray, Rhs_SerialArray, *This_SerialArray *= *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#else
     intArray::Abstract_Operator_Operation_Equals ( *this , Rhs ,
               MDI_i_Multiply_Array_Times_Array_Accumulate_To_Operand , intArray::Times_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator*=(intArray)");
        }
#endif

     return *this;
   }


intArray &
intArray::operator*= ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator*= (int) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator*=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator*=(int)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, This_SerialArray, *This_SerialArray *= x );
     // ... don't need to use macro because Return_Value won't be Mask ...
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;

#else
     intArray::Abstract_Operator_Operation_Equals ( *this , x ,
        MDI_i_Multiply_Array_Times_Scalar_Accumulate_To_Operand , intArray::Scalar_Times_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator*=(int)");
        }
#endif

     return *this;
   }


intArray &
operator/ ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator/ (intArray,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator/");
     Rhs.Test_Consistency ("Test Rhs in operator/");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator/(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator/(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray / *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, 
	  Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray / *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator/(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Lhs , Rhs , 
               MDI_i_Divide_Array_Divided_By_Array,
               MDI_i_Divide_Array_Divided_By_Array_Accumulate_To_Operand , intArray::Divided_By );
#endif
   }


intArray &
operator+ ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\n### Inside of operator+ (intArray,int) for intArray class: (id=%d) = scalar \n",Lhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator+");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator+(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
               APP_ASSERT(Lhs_SerialArray != NULL);
            // Lhs_SerialArray->displayReferenceCounts("AFTER PCE: *Lhs_SerialArray in intArray & operator+(intArray,int)");
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator+(intArray,int)");
        }
#endif

  // (11/27/2000) Added error checking (will not work with indirect addessing later!!!)
     APP_ASSERT(Temporary_Array_Set != NULL);

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator(Temporary_Array_Set,Lhs,Lhs_SerialArray,*Lhs_SerialArray + x);
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in in intArray & operator+(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_Add_Array_Plus_Scalar,
               MDI_i_Add_Array_Plus_Scalar_Accumulate_To_Operand , intArray::Scalar_Plus );
#endif
   }


intArray &
operator/ ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator/ (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator/");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator/(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray     != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray / x );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator/(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_Divide_Array_Divided_By_Scalar,
               MDI_i_Divide_Array_Divided_By_Scalar_Accumulate_To_Operand , intArray::Scalar_Divided_By );
#endif
   }


intArray &
operator/ ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator/ (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator/");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator/(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x / *Rhs_SerialArray );
  // return intArray::Abstract_Binary_Operator ( Temporary_Array_Set, Rhs, x / *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator/(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_Divide_Scalar_Divided_By_Array,
               MDI_i_Divide_Scalar_Divided_By_Array_Accumulate_To_Operand , intArray::Scalar_Divided_By );
#endif
   }


intArray &
intArray::operator/= ( const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator/= (intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator/=");
     Rhs.Test_Consistency ("Test Rhs in intArray::operator/=");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator/=(intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::operator/=(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray  != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs, This_SerialArray, Rhs_SerialArray, *This_SerialArray /= *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#else
     intArray::Abstract_Operator_Operation_Equals ( *this , Rhs ,
               MDI_i_Divide_Array_Divided_By_Array_Accumulate_To_Operand , intArray::Divided_By_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator/=(intArray)");
        }
#endif

     return *this;
   }


intArray &
intArray::operator/= ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator/= (int) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator/=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator/=(int)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, This_SerialArray, *This_SerialArray /= x );
     // ... don't need to use macro because Return_Value won't be Mask ...
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;

#else
     intArray::Abstract_Operator_Operation_Equals ( *this , x ,
        MDI_i_Divide_Array_Divided_By_Scalar_Accumulate_To_Operand , intArray::Scalar_Divided_By_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator/=(int)");
        }
#endif

     return *this;
   }


intArray &
operator% ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("\n\n\n### Inside of operator% (intArray,intArray) for intArray class: (id=%d) = (id=%d) \n",
               Lhs.Array_ID(),Rhs.Array_ID());
        }

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator%");
     Rhs.Test_Consistency ("Test Rhs in operator%");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs.isTemporary() = %s \n",(Lhs.isTemporary()) ? "TRUE" : "FALSE");
          Lhs.displayReferenceCounts("Lhs in intArray & operator%(intArray,intArray)");
          printf ("intArray: Rhs.isTemporary() = %s \n",(Rhs.isTemporary()) ? "TRUE" : "FALSE");
          Rhs.displayReferenceCounts("Rhs in intArray & operator%(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ( (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
               (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) )
             {
               printf ("Sorry, not implemented: can't mix indirect addressing using where statements and two array (binary) operators!\n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
                  (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator%(intArray,intArray)");
          printf ("intArray: Rhs_SerialArray->isTemporary() = %s \n",(Rhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & operator%(intArray,intArray)");
        }
#endif

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray % *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray % *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator%(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , 
               MDI_i_Fmod_Array_Modulo_Array,
               MDI_i_Fmod_Array_Modulo_Array_Accumulate_To_Operand , intArray::Modulo );
#endif
   }


intArray &
operator% ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\n### Inside of operator% (intArray,int) for intArray class: (id=%d) = scalar \n",Lhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator%");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator%(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
               APP_ASSERT(Lhs_SerialArray != NULL);
            // Lhs_SerialArray->displayReferenceCounts("AFTER PCE: *Lhs_SerialArray in intArray & operator%(intArray,int)");
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator%(intArray,int)");
        }
#endif

  // (11/27/2000) Added error checking (will not work with indirect addessing later!!!)
     APP_ASSERT(Temporary_Array_Set != NULL);

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator(Temporary_Array_Set,Lhs,Lhs_SerialArray,*Lhs_SerialArray % x);
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in in intArray & operator%(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_Fmod_Array_Modulo_Scalar,
               MDI_i_Fmod_Array_Modulo_Scalar_Accumulate_To_Operand , intArray::Scalar_Modulo );
#endif
   }


intArray &
operator% ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator% (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator%");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator%(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x % *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator%(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_Fmod_Scalar_Modulo_Array,
               MDI_i_Fmod_Scalar_Modulo_Array_Accumulate_To_Operand , intArray::Scalar_Modulo );
#endif
   }


intArray &
intArray::operator%= ( const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator%= (intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator%=");
     Rhs.Test_Consistency ("Test Rhs in intArray::operator%=");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator%=(intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::operator%=(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray  != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs, This_SerialArray, Rhs_SerialArray, *This_SerialArray %= *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#else
     intArray::Abstract_Operator_Operation_Equals ( *this , Rhs ,
               MDI_i_Fmod_Array_Modulo_Array_Accumulate_To_Operand , intArray::Modulo_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator%=(intArray)");
        }
#endif

     return *this;
   }


intArray &
intArray::operator%= ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator%= (int) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator%=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator%=(int)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, This_SerialArray, *This_SerialArray %= x );
     // ... don't need to use macro because Return_Value won't be Mask ...
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;

#else
     intArray::Abstract_Operator_Operation_Equals ( *this , x ,
        MDI_i_Fmod_Array_Modulo_Scalar_Accumulate_To_Operand , intArray::Scalar_Modulo_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator%=(int)");
        }
#endif

     return *this;
   }


#ifndef INTARRAY
intArray &
cos ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of cos for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in cos ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & cos(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, cos(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, cos(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & cos(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Cos_Array ,
                   MDI_i_Cos_Array_Accumulate_To_Operand , intArray::cos_Function );
#endif
   } 

#endif

intArray &
operator+ ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator+ (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator+");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator+(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x + *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator+(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_Add_Scalar_Plus_Array,
               MDI_i_Add_Scalar_Plus_Array_Accumulate_To_Operand , intArray::Scalar_Plus );
#endif
   }


#ifndef INTARRAY
intArray &
sin ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of sin for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in sin ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & sin(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, sin(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, sin(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & sin(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Sin_Array ,
                   MDI_i_Sin_Array_Accumulate_To_Operand , intArray::sin_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
tan ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of tan for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in tan ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & tan(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, tan(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, tan(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & tan(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Tan_Array ,
                   MDI_i_Tan_Array_Accumulate_To_Operand , intArray::tan_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
acos ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of acos for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in acos ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & acos(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, acos(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, acos(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & acos(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Arc_Cos_Array ,
                   MDI_i_Arc_Cos_Array_Accumulate_To_Operand , intArray::acos_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
asin ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of asin for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in asin ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & asin(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, asin(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, asin(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & asin(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Arc_Sin_Array ,
                   MDI_i_Arc_Sin_Array_Accumulate_To_Operand , intArray::asin_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
atan ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of atan for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in atan ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & atan(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, atan(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, atan(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & atan(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Arc_Tan_Array ,
                   MDI_i_Arc_Tan_Array_Accumulate_To_Operand , intArray::atan_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
cosh ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of cosh for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in cosh ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & cosh(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, cosh(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, cosh(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & cosh(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Cosh_Array ,
                   MDI_i_Cosh_Array_Accumulate_To_Operand , intArray::cosh_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
sinh ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of sinh for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in sinh ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & sinh(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, sinh(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, sinh(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & sinh(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Sinh_Array ,
                   MDI_i_Sinh_Array_Accumulate_To_Operand , intArray::sinh_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
tanh ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of tanh for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in tanh ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & tanh(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, tanh(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, tanh(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & tanh(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Tanh_Array ,
                   MDI_i_Tanh_Array_Accumulate_To_Operand , intArray::tanh_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
acosh ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of acosh for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in acosh ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & acosh(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, acosh(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, acosh(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & acosh(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Arc_Cosh_Array ,
                   MDI_i_Arc_Cosh_Array_Accumulate_To_Operand , intArray::acosh_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
asinh ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of asinh for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in asinh ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & asinh(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, asinh(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, asinh(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & asinh(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Arc_Sinh_Array ,
                   MDI_i_Arc_Sinh_Array_Accumulate_To_Operand , intArray::asinh_Function );
#endif
   } 

#endif

intArray &
intArray::operator++ ()
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside intArray::operator++ () for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator++ -- Prefix operator");
#endif

     (*this) += 1;
     return *this;
   }
 
intArray &
intArray::operator++ ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside intArray::operator%s (int=%d) for intArray class! \n","++",x);

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator++(x=0) -- Postfix operator");
#endif

  // Postfix operator always passes zero as argument (strange but true -- See Stroustrup p594)
     APP_ASSERT( x == 0 );
     (*this) += 1;
     return *this;
   }


#ifndef INTARRAY
intArray &
atanh ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of atanh for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in atanh ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & atanh(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, atanh(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, atanh(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & atanh(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Arc_Tanh_Array ,
                   MDI_i_Arc_Tanh_Array_Accumulate_To_Operand , intArray::atanh_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
log ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of log for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in log ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & log(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, log(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, log(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & log(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Log_Array ,
                   MDI_i_Log_Array_Accumulate_To_Operand , intArray::log_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
log10 ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of log10 for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in log10 ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & log10(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, log10(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, log10(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & log10(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Log10_Array ,
                   MDI_i_Log10_Array_Accumulate_To_Operand , intArray::log10_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
exp ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of exp for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in exp ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & exp(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, exp(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, exp(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & exp(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Exp_Array ,
                   MDI_i_Exp_Array_Accumulate_To_Operand , intArray::exp_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
sqrt ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of sqrt for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in sqrt ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & sqrt(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, sqrt(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, sqrt(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & sqrt(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Sqrt_Array ,
                   MDI_i_Sqrt_Array_Accumulate_To_Operand , intArray::sqrt_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
fabs ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of fabs for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in fabs ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & fabs(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, fabs(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, fabs(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & fabs(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Fabs_Array ,
                   MDI_i_Fabs_Array_Accumulate_To_Operand , intArray::fabs_Function );
#endif
   } 

intArray &
abs ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of abs for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in abs ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & abs(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, abs(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, abs(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & abs(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Fabs_Array ,
                   MDI_i_Fabs_Array_Accumulate_To_Operand , intArray::abs_Function );
#endif
   } 

#else
intArray &
abs ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of abs for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in abs ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & abs(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, abs(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, abs(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & abs(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Abs_Array ,
                   MDI_i_Abs_Array_Accumulate_To_Operand , intArray::abs_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
ceil ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of ceil for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in ceil ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & ceil(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, ceil(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, ceil(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & ceil(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Ceil_Array ,
                   MDI_i_Ceil_Array_Accumulate_To_Operand , intArray::ceil_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
floor ( const intArray & X )
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of floor for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in floor ( const intArray & X )");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & floor(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (X, X_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, X, floor(*X_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = X;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, floor(*X_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & floor(intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X ,
                   MDI_i_Floor_Array ,
                   MDI_i_Floor_Array_Accumulate_To_Operand , intArray::floor_Function );
#endif
   } 

#endif

#ifndef INTARRAY
intArray &
intArray::replace ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::replace (intArray,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::replace (intArray,intArray)");
     Lhs.Test_Consistency ("Test Lhs in intArray::replace (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in intArray::replace (intArray,intArray)");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & intArray::replace(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::replace(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray  *This_SerialArray = NULL;
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray  *Rhs_SerialArray = NULL;
     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	 (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
     {
	puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
        APP_ABORT();
     }
     else
     {
        Temporary_Array_Set = 
            intArray::Parallel_Conformability_Enforcement 
	       ( *this, This_SerialArray, Lhs, Lhs_SerialArray, 
	          Rhs  , Rhs_SerialArray );
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray    != NULL);
     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool lhsIsTemporary  = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (lhsIsTemporary == TRUE)  || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE)  || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Lhs, Rhs,
               This_SerialArray, Lhs_SerialArray, Rhs_SerialArray,
               This_SerialArray->replace (*Lhs_SerialArray, *Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & intArray::replace(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Modification_Operator ( Lhs , Rhs , MDI_i_If_Array_Use_Array , intArray::replace_Function );
#endif
   }

#endif

#ifndef INTARRAY
intArray &
intArray::replace ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray::replace (intArray,x) for intArray class! \n");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::replace (intArray,int)");
     Lhs.Test_Consistency ("Test Lhs in intArray::replace (intArray,int)");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & intArray::replace(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray  *This_SerialArray = NULL;
     intSerialArray *Lhs_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	 (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
     {
        Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this, This_SerialArray, Lhs, Lhs_SerialArray );
     }
     else
     {
        Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this, This_SerialArray, Lhs, Lhs_SerialArray );
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray    != NULL);
     APP_ASSERT(Lhs_SerialArray     != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool lhsIsTemporary  = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (lhsIsTemporary  == TRUE) || (lhsIsTemporary  == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & intArray::replace ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Lhs,
               This_SerialArray, Lhs_SerialArray, This_SerialArray->replace (*Lhs_SerialArray, x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & intArray::replace(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Modification_Operator ( Lhs , x , MDI_i_If_Array_Use_Scalar , intArray::Scalar_replace_Function );
#endif
   }

#endif

intArray &
intArray::operator+= ( const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator+= (intArray) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator+=");
     Rhs.Test_Consistency ("Test Rhs in intArray::operator+=");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator+=(intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::operator+=(intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( *this , This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray  != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs, This_SerialArray, Rhs_SerialArray, *This_SerialArray += *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#else
     intArray::Abstract_Operator_Operation_Equals ( *this , Rhs ,
               MDI_i_Add_Array_Plus_Array_Accumulate_To_Operand , intArray::Plus_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator+=(intArray)");
        }
#endif

     return *this;
   }


#ifndef INTARRAY
intArray &
intArray::replace ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of intArray::replace (x,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::replace (int,intArray)");
     Rhs.Test_Consistency ("Test Rhs in intArray::replace (int,intArray)");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & intArray::replace(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray  = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement( *this, This_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this, This_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = 
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray    != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();
     bool rhsIsTemporary  = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary  == TRUE) || (rhsIsTemporary  == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, Rhs,
               This_SerialArray, Rhs_SerialArray,
               This_SerialArray->replace (x, *Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & intArray::replace(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Modification_Operator ( Rhs , x , MDI_i_If_Scalar_Use_Array , Scalar_replace_Function );
#endif
   }

#endif

#ifndef INTARRAY
intArray &
fmod ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of fmod (intArray,intArray) for intArray class! Lhs:rc=%d Rhs:rc=%d ",
               Lhs.getRawDataReferenceCount(),Rhs.getRawDataReferenceCount());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in fmod");
     Rhs.Test_Consistency ("Test Rhs in fmod");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & fmod(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & fmod(intArray,intArray)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs and Rhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
	       puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
                     Rhs, Rhs_SerialArray );
             }

          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

  // New test (8/5/2000)
     APP_ASSERT(Temporary_Array_Set != NULL);
  // Temporary_Array_Set->display("Check to see what sort of communication model was used");

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & fmod(intArray,intArray)");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & fmod(intArray,intArray)");
        }
#endif

  // Inputs to intArray::Abstract_Binary_Operator:
  //     1. Temporary_Array_Set is attached to the intArray temporary returned by Abstract_Binary_Operator
  //     2. Lhs is used to get the Lhs partition information (PARTI parallel descriptor) and array reuse
  //     3. Rhs is used to get the Rhs partition information (PARTI parallel descriptor) in case the Lhs was 
  //        a NULL array (no data and no defined partitioning (i.e. no PARTI parallel descriptor)) and array reuse
  //     4. The intSerialArray which is to be put into the intArray temporary returned by Abstract_Binary_Operator
  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, fmod(*Lhs_SerialArray,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, fmod(*Lhs_SerialArray,*Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // if (Lhs_SerialArray != Return_Value.getSerialArrayPointer())
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & fmod(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Lhs , Rhs , MDI_i_Fmod_Array_Modulo_Array, MDI_i_Fmod_Array_Modulo_Array_Accumulate_To_Operand , intArray::fmod_Function );
#endif
   }



intArray &
mod ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of mod (intArray,intArray) for intArray class! Lhs:rc=%d Rhs:rc=%d ",
               Lhs.getRawDataReferenceCount(),Rhs.getRawDataReferenceCount());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in mod");
     Rhs.Test_Consistency ("Test Rhs in mod");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & mod(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & mod(intArray,intArray)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs and Rhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
	       puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
                     Rhs, Rhs_SerialArray );
             }

          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

  // New test (8/5/2000)
     APP_ASSERT(Temporary_Array_Set != NULL);
  // Temporary_Array_Set->display("Check to see what sort of communication model was used");

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & mod(intArray,intArray)");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & mod(intArray,intArray)");
        }
#endif

  // Inputs to intArray::Abstract_Binary_Operator:
  //     1. Temporary_Array_Set is attached to the intArray temporary returned by Abstract_Binary_Operator
  //     2. Lhs is used to get the Lhs partition information (PARTI parallel descriptor) and array reuse
  //     3. Rhs is used to get the Rhs partition information (PARTI parallel descriptor) in case the Lhs was 
  //        a NULL array (no data and no defined partitioning (i.e. no PARTI parallel descriptor)) and array reuse
  //     4. The intSerialArray which is to be put into the intArray temporary returned by Abstract_Binary_Operator
  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, mod(*Lhs_SerialArray,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, mod(*Lhs_SerialArray,*Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // if (Lhs_SerialArray != Return_Value.getSerialArrayPointer())
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & mod(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Lhs , Rhs , MDI_i_Fmod_Array_Modulo_Array, MDI_i_Fmod_Array_Modulo_Array_Accumulate_To_Operand , intArray::mod_Function );
#endif
   }



#else
intArray &
mod ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of mod (intArray,intArray) for intArray class! Lhs:rc=%d Rhs:rc=%d ",
               Lhs.getRawDataReferenceCount(),Rhs.getRawDataReferenceCount());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in mod");
     Rhs.Test_Consistency ("Test Rhs in mod");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & mod(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & mod(intArray,intArray)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs and Rhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
	       puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
                     Rhs, Rhs_SerialArray );
             }

          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

  // New test (8/5/2000)
     APP_ASSERT(Temporary_Array_Set != NULL);
  // Temporary_Array_Set->display("Check to see what sort of communication model was used");

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & mod(intArray,intArray)");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & mod(intArray,intArray)");
        }
#endif

  // Inputs to intArray::Abstract_Binary_Operator:
  //     1. Temporary_Array_Set is attached to the intArray temporary returned by Abstract_Binary_Operator
  //     2. Lhs is used to get the Lhs partition information (PARTI parallel descriptor) and array reuse
  //     3. Rhs is used to get the Rhs partition information (PARTI parallel descriptor) in case the Lhs was 
  //        a NULL array (no data and no defined partitioning (i.e. no PARTI parallel descriptor)) and array reuse
  //     4. The intSerialArray which is to be put into the intArray temporary returned by Abstract_Binary_Operator
  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, mod(*Lhs_SerialArray,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, mod(*Lhs_SerialArray,*Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // if (Lhs_SerialArray != Return_Value.getSerialArrayPointer())
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & mod(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Lhs , Rhs , MDI_i_Fmod_Array_Modulo_Array, MDI_i_Fmod_Array_Modulo_Array_Accumulate_To_Operand , intArray::mod_Function );
#endif
   }



#endif

#ifndef INTARRAY
intArray &
fmod ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\nInside of fmod (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in fmod");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & fmod(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
              (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}

        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & fmod(int,intArray)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, fmod(x,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, fmod(x,*Rhs_SerialArray) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete macro) in intArray & fmod(int,intArray)");
        }
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & fmod(int,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Rhs , x ,
               MDI_i_Fmod_Scalar_Modulo_Array,
               MDI_i_Fmod_Scalar_Modulo_Array_Accumulate_To_Operand , intArray::Scalar_fmod_Function );
#endif
   }



intArray &
mod ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\nInside of mod (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in mod");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & mod(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
              (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}

        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & mod(int,intArray)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, mod(x,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, mod(x,*Rhs_SerialArray) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete macro) in intArray & mod(int,intArray)");
        }
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & mod(int,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Rhs , x ,
               MDI_i_Fmod_Scalar_Modulo_Array,
               MDI_i_Fmod_Scalar_Modulo_Array_Accumulate_To_Operand , intArray::Scalar_mod_Function );
#endif
   }



#else
intArray &
mod ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\nInside of mod (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in mod");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & mod(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
              (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}

        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & mod(int,intArray)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, mod(x,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, mod(x,*Rhs_SerialArray) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete macro) in intArray & mod(int,intArray)");
        }
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & mod(int,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Rhs , x ,
               MDI_i_Fmod_Scalar_Modulo_Array,
               MDI_i_Fmod_Scalar_Modulo_Array_Accumulate_To_Operand , intArray::Scalar_mod_Function );
#endif
   }



#endif

#ifndef INTARRAY
intArray &
fmod ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of fmod (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in fmod");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts 
          Lhs.displayReferenceCounts("Lhs in intArray & fmod(intArray,int)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & fmod(intArray,int)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, fmod(*Lhs_SerialArray,x) );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & fmod ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, fmod(*Lhs_SerialArray,x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & fmod(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x ,
               MDI_i_Fmod_Array_Modulo_Scalar,
               MDI_i_Fmod_Array_Modulo_Scalar_Accumulate_To_Operand , intArray::Scalar_fmod_Function );
#endif
   }

intArray &
mod ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of mod (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in mod");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts 
          Lhs.displayReferenceCounts("Lhs in intArray & mod(intArray,int)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & mod(intArray,int)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, mod(*Lhs_SerialArray,x) );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & mod ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, mod(*Lhs_SerialArray,x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & mod(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x ,
               MDI_i_Fmod_Array_Modulo_Scalar,
               MDI_i_Fmod_Array_Modulo_Scalar_Accumulate_To_Operand , intArray::Scalar_mod_Function );
#endif
   }

#else
intArray &
mod ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of mod (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in mod");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts 
          Lhs.displayReferenceCounts("Lhs in intArray & mod(intArray,int)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & mod(intArray,int)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, mod(*Lhs_SerialArray,x) );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & mod ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, mod(*Lhs_SerialArray,x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & mod(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x ,
               MDI_i_Fmod_Array_Modulo_Scalar,
               MDI_i_Fmod_Array_Modulo_Scalar_Accumulate_To_Operand , intArray::Scalar_mod_Function );
#endif
   }

#endif

intArray &
pow ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of pow (intArray,intArray) for intArray class! Lhs:rc=%d Rhs:rc=%d ",
               Lhs.getRawDataReferenceCount(),Rhs.getRawDataReferenceCount());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in pow");
     Rhs.Test_Consistency ("Test Rhs in pow");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & pow(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & pow(intArray,intArray)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs and Rhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
	       puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
                     Rhs, Rhs_SerialArray );
             }

          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

  // New test (8/5/2000)
     APP_ASSERT(Temporary_Array_Set != NULL);
  // Temporary_Array_Set->display("Check to see what sort of communication model was used");

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & pow(intArray,intArray)");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & pow(intArray,intArray)");
        }
#endif

  // Inputs to intArray::Abstract_Binary_Operator:
  //     1. Temporary_Array_Set is attached to the intArray temporary returned by Abstract_Binary_Operator
  //     2. Lhs is used to get the Lhs partition information (PARTI parallel descriptor) and array reuse
  //     3. Rhs is used to get the Rhs partition information (PARTI parallel descriptor) in case the Lhs was 
  //        a NULL array (no data and no defined partitioning (i.e. no PARTI parallel descriptor)) and array reuse
  //     4. The intSerialArray which is to be put into the intArray temporary returned by Abstract_Binary_Operator
  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, pow(*Lhs_SerialArray,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, pow(*Lhs_SerialArray,*Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // if (Lhs_SerialArray != Return_Value.getSerialArrayPointer())
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & pow(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Lhs , Rhs , MDI_i_Pow_Array_Raised_To_Array, MDI_i_Pow_Array_Raised_To_Array_Accumulate_To_Operand , intArray::pow_Function );
#endif
   }




intArray &
pow ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\nInside of pow (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in pow");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & pow(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
              (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}

        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & pow(int,intArray)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, pow(x,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, pow(x,*Rhs_SerialArray) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete macro) in intArray & pow(int,intArray)");
        }
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & pow(int,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Rhs , x ,
               MDI_i_Pow_Scalar_Raised_To_Array,
               MDI_i_Pow_Scalar_Raised_To_Array_Accumulate_To_Operand , intArray::Scalar_pow_Function );
#endif
   }




intArray &
pow ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of pow (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in pow");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts 
          Lhs.displayReferenceCounts("Lhs in intArray & pow(intArray,int)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & pow(intArray,int)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, pow(*Lhs_SerialArray,x) );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & pow ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, pow(*Lhs_SerialArray,x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & pow(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x ,
               MDI_i_Pow_Array_Raised_To_Scalar,
               MDI_i_Pow_Array_Raised_To_Scalar_Accumulate_To_Operand , intArray::Scalar_pow_Function );
#endif
   }


intArray &
sign ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of sign (intArray,intArray) for intArray class! Lhs:rc=%d Rhs:rc=%d ",
               Lhs.getRawDataReferenceCount(),Rhs.getRawDataReferenceCount());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in sign");
     Rhs.Test_Consistency ("Test Rhs in sign");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & sign(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & sign(intArray,intArray)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs and Rhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
	       puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
                     Rhs, Rhs_SerialArray );
             }

          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

  // New test (8/5/2000)
     APP_ASSERT(Temporary_Array_Set != NULL);
  // Temporary_Array_Set->display("Check to see what sort of communication model was used");

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & sign(intArray,intArray)");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & sign(intArray,intArray)");
        }
#endif

  // Inputs to intArray::Abstract_Binary_Operator:
  //     1. Temporary_Array_Set is attached to the intArray temporary returned by Abstract_Binary_Operator
  //     2. Lhs is used to get the Lhs partition information (PARTI parallel descriptor) and array reuse
  //     3. Rhs is used to get the Rhs partition information (PARTI parallel descriptor) in case the Lhs was 
  //        a NULL array (no data and no defined partitioning (i.e. no PARTI parallel descriptor)) and array reuse
  //     4. The intSerialArray which is to be put into the intArray temporary returned by Abstract_Binary_Operator
  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, sign(*Lhs_SerialArray,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, sign(*Lhs_SerialArray,*Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // if (Lhs_SerialArray != Return_Value.getSerialArrayPointer())
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & sign(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Lhs , Rhs , MDI_i_Sign_Array_Of_Array, MDI_i_Sign_Array_Of_Array_Accumulate_To_Operand , intArray::sign_Function );
#endif
   }




intArray &
sign ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\nInside of sign (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in sign");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & sign(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
              (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}

        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & sign(int,intArray)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, sign(x,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, sign(x,*Rhs_SerialArray) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete macro) in intArray & sign(int,intArray)");
        }
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & sign(int,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Rhs , x ,
               MDI_i_Sign_Scalar_Of_Array,
               MDI_i_Sign_Scalar_Of_Array_Accumulate_To_Operand , intArray::Scalar_sign_Function );
#endif
   }




intArray &
sign ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of sign (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in sign");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts 
          Lhs.displayReferenceCounts("Lhs in intArray & sign(intArray,int)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & sign(intArray,int)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, sign(*Lhs_SerialArray,x) );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & sign ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, sign(*Lhs_SerialArray,x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & sign(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x ,
               MDI_i_Sign_Array_Of_Scalar,
               MDI_i_Sign_Array_Of_Scalar_Accumulate_To_Operand , intArray::Scalar_sign_Function );
#endif
   }


intArray &
intArray::operator+= ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator+= (int) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator+=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator+=(int)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, This_SerialArray, *This_SerialArray += x );
     // ... don't need to use macro because Return_Value won't be Mask ...
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;

#else
     intArray::Abstract_Operator_Operation_Equals ( *this , x ,
        MDI_i_Add_Array_Plus_Scalar_Accumulate_To_Operand , intArray::Scalar_Plus_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator+=(int)");
        }
#endif

     return *this;
   }


// Most C++ compliers support a unary plus operator
intArray &
intArray::operator+ () const
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of unary plus intArray::operator+ () for intArray class! \n");
#endif

  // return *this;
     return (intArray &)(*this);
   }


intArray &
min ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of min (intArray,intArray) for intArray class! Lhs:rc=%d Rhs:rc=%d ",
               Lhs.getRawDataReferenceCount(),Rhs.getRawDataReferenceCount());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in min");
     Rhs.Test_Consistency ("Test Rhs in min");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & min(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & min(intArray,intArray)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs and Rhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
	       puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
                     Rhs, Rhs_SerialArray );
             }

          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

  // New test (8/5/2000)
     APP_ASSERT(Temporary_Array_Set != NULL);
  // Temporary_Array_Set->display("Check to see what sort of communication model was used");

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & min(intArray,intArray)");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & min(intArray,intArray)");
        }
#endif

  // Inputs to intArray::Abstract_Binary_Operator:
  //     1. Temporary_Array_Set is attached to the intArray temporary returned by Abstract_Binary_Operator
  //     2. Lhs is used to get the Lhs partition information (PARTI parallel descriptor) and array reuse
  //     3. Rhs is used to get the Rhs partition information (PARTI parallel descriptor) in case the Lhs was 
  //        a NULL array (no data and no defined partitioning (i.e. no PARTI parallel descriptor)) and array reuse
  //     4. The intSerialArray which is to be put into the intArray temporary returned by Abstract_Binary_Operator
  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, min(*Lhs_SerialArray,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, min(*Lhs_SerialArray,*Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // if (Lhs_SerialArray != Return_Value.getSerialArrayPointer())
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & min(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , MDI_i_Min_Array_And_Array, MDI_i_Min_Array_And_Array_Accumulate_To_Operand , intArray::min_Function );
#endif
   }


intArray &
min ( const intArray & X , const intArray & Y , const intArray & Z )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of min (intArray,intArray,intArray) for class! X:rc=%d Y:rc=%d Z:rc=%d ",
               X.getRawDataReferenceCount(),Y.getRawDataReferenceCount(),Z.getRawDataReferenceCount());
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
        {
          X.Test_Conformability (Y);
          X.Test_Conformability (Z);
        }
     
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & min (intArray,intArray,intArray)");
          Y.displayReferenceCounts("Y in intArray & min (intArray,intArray,intArray)");
          Z.displayReferenceCounts("Z in intArray & min (intArray,intArray,intArray)");
        }
#endif

     return min ( X , min ( Y , Z ) );
   }


intArray &
min ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\nInside of min (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in min");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & min(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
              (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}

        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & min(int,intArray)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, min(x,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, min(x,*Rhs_SerialArray) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete macro) in intArray & min(int,intArray)");
        }
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & min(int,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator ( Rhs , x ,
               MDI_i_Min_Scalar_And_Array,
               MDI_i_Min_Scalar_And_Array_Accumulate_To_Operand , intArray::Scalar_min_Function );
#endif
   }


intArray &
min ( int x , const intArray & Y , const intArray & Z )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of min (int,intArray,intArray) for class! Y:rc=%d Z:rc=%d ",
               Y.getRawDataReferenceCount(),Z.getRawDataReferenceCount());
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Y.Test_Conformability (Z);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Y.displayReferenceCounts("Y in intArray & min (int,intArray,intArray)");
          Z.displayReferenceCounts("Z in intArray & min (int,intArray,intArray)");
        }
#endif

     return min ( x , min ( Y , Z ) );
   }


intArray &
min ( const intArray & X , int y , const intArray & Z )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of min (intArray,int,intArray) for class! X:rc=%d Z:rc=%d ",
               X.getRawDataReferenceCount(),Z.getRawDataReferenceCount());
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          X.Test_Conformability (Z);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & min (int,intArray,intArray)");
          Z.displayReferenceCounts("Z in intArray & min (int,intArray,intArray)");
        }
#endif

     return min ( y , min ( X , Z ) );
   }


intArray &
min ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of min (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in min");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts 
          Lhs.displayReferenceCounts("Lhs in intArray & min(intArray,int)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & min(intArray,int)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, min(*Lhs_SerialArray,x) );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & min ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, min(*Lhs_SerialArray,x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & min(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x ,
               MDI_i_Min_Array_And_Scalar,
               MDI_i_Min_Array_And_Scalar_Accumulate_To_Operand , intArray::Scalar_min_Function );
#endif
   }


intArray &
min ( const intArray & X , const intArray & Y , int z )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of min (intArray,intArray,int) for class! X:rc=%d Y:rc=%d ",
               X.getRawDataReferenceCount(),Y.getRawDataReferenceCount());
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          X.Test_Conformability (Y);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & min (intArray,intArray,int)");
          Y.displayReferenceCounts("Y in intArray & min (intArray,intArray,int)");
        }
#endif

     return min ( min ( X , Y ) , z );
   }


int
min ( const intArray & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of min (intArray) returning int for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in min (const intArray & X)");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in int min (intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	          (X, X_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (X, X_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
        {
          Temporary_Array_Set = new Array_Conformability_Info_Type();
        }

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // We need to specify the type of operation so that the reduction operation between processors can be handled correctly
#if defined(MEMORY_LEAK_TEST)
     int Return_Value = 0;
#else
     int Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, min (*X_SerialArray) , intArray::min_Function );
  // return intArray::Abstract_Reduction_Operator ( Temporary_Array_Set, X, min (*X_SerialArray) , intArray::min_Function );
#endif

  // Delete the serial array unless it would have been absorbed by the serialArray in function
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
       {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X , MDI_i_Min_Array_Returning_Scalar , intArray::min_Function );
#endif
   }


intArray &
max ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of max (intArray,intArray) for intArray class! Lhs:rc=%d Rhs:rc=%d ",
               Lhs.getRawDataReferenceCount(),Rhs.getRawDataReferenceCount());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in max");
     Rhs.Test_Consistency ("Test Rhs in max");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & max(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & max(intArray,intArray)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs and Rhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
	       puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
                     Rhs, Rhs_SerialArray );
             }

          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

  // New test (8/5/2000)
     APP_ASSERT(Temporary_Array_Set != NULL);
  // Temporary_Array_Set->display("Check to see what sort of communication model was used");

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & max(intArray,intArray)");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & max(intArray,intArray)");
        }
#endif

  // Inputs to intArray::Abstract_Binary_Operator:
  //     1. Temporary_Array_Set is attached to the intArray temporary returned by Abstract_Binary_Operator
  //     2. Lhs is used to get the Lhs partition information (PARTI parallel descriptor) and array reuse
  //     3. Rhs is used to get the Rhs partition information (PARTI parallel descriptor) in case the Lhs was 
  //        a NULL array (no data and no defined partitioning (i.e. no PARTI parallel descriptor)) and array reuse
  //     4. The intSerialArray which is to be put into the intArray temporary returned by Abstract_Binary_Operator
  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, max(*Lhs_SerialArray,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, max(*Lhs_SerialArray,*Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // if (Lhs_SerialArray != Return_Value.getSerialArrayPointer())
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & max(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , MDI_i_Max_Array_And_Array, MDI_i_Max_Array_And_Array_Accumulate_To_Operand , intArray::max_Function );
#endif
   }


intArray &
max ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\nInside of max (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in max");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & max(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
              (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}

        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & max(int,intArray)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, max(x,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, max(x,*Rhs_SerialArray) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete macro) in intArray & max(int,intArray)");
        }
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & max(int,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator ( Rhs , x ,
               MDI_i_Max_Scalar_And_Array,
               MDI_i_Max_Scalar_And_Array_Accumulate_To_Operand , intArray::Scalar_max_Function );
#endif
   }


intArray &
max ( int x , const intArray & Y , const intArray & Z )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of max (int,intArray,intArray) for intArray class!");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Y.Test_Conformability (Z);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Y.displayReferenceCounts("Y in intArray & max (int,intArray,intArray)");
          Z.displayReferenceCounts("Z in intArray & max (int,intArray,intArray)");
        }
#endif

     return max ( x , max ( Y , Z ) );
   }


intArray &
max ( const intArray & X , int y , const intArray & Z )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of max (intArray,int,intArray) for intArray class!");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          X.Test_Conformability (Z);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & max (int,intArray,intArray)");
          Z.displayReferenceCounts("Z in intArray & max (int,intArray,intArray)");
        }
#endif

     return max ( y , max ( X , Z ) );
   }


intArray &
max ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of max (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in max");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts 
          Lhs.displayReferenceCounts("Lhs in intArray & max(intArray,int)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & max(intArray,int)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, max(*Lhs_SerialArray,x) );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & max ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, max(*Lhs_SerialArray,x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & max(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x ,
               MDI_i_Max_Array_And_Scalar,
               MDI_i_Max_Array_And_Scalar_Accumulate_To_Operand , intArray::Scalar_max_Function );
#endif
   }


intArray &
max ( const intArray & X , const intArray & Y , int z )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of max (intArray,intArray,int) for intArray class! \n");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          X.Test_Conformability (Y);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & max (intArray,intArray,int)");
          Y.displayReferenceCounts("Y in intArray & max (intArray,intArray,int)");
        }
#endif

     return max ( max ( X , Y ) , z );
   }


int
max ( const intArray & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of max (intArray) returning int for intArray class! \n");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in max (const intArray & X)");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in int max (intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
             }
	    else
	     {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (X, X_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	            (X, X_SerialArray, 
                     *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	     }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
        {
       // printf ("Building the Array_Set in the max operator \n");
          Temporary_Array_Set =	new Array_Conformability_Info_Type();
        }

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray       != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     int Return_Value = 0;
#else
     int Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, max (*X_SerialArray) , intArray::max_Function );
#endif

  // Delete the serial array unless it would have been absorbed by the serialArray in function
     if (xIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X , MDI_i_Max_Array_Returning_Scalar , intArray::max_Function );
#endif
   }


intArray &
max ( const intArray & X , const intArray & Y , const intArray & Z )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of max (intArray,intArray,intArray) for intArray class! \n");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
        {
          X.Test_Conformability (Y);
          X.Test_Conformability (Z);
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in intArray & max (intArray,intArray,intArray)");
          Y.displayReferenceCounts("Y in intArray & max (intArray,intArray,intArray)");
          Z.displayReferenceCounts("Z in intArray & max (intArray,intArray,intArray)");
        }
#endif

     return max ( X , max ( Y , Z ) );
   }


int
sum ( const intArray & X )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of sum (const intArray) returning int for intArray class!");

  // This is the only test we can do on the input!
     X.Test_Consistency ("Test X in sum (const intArray & X)");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          X.displayReferenceCounts("X in int sum (intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *X_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( X, X_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( X, X_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (X.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (X, X_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
	    else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (X, X_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(X_SerialArray != NULL);

     bool xIsTemporary = X_SerialArray->isTemporary();

     APP_ASSERT ( (xIsTemporary == TRUE) || (xIsTemporary == FALSE) );

  // This bug is fixed in the intArray::Parallel_Conformability_Enforcement function by restricting the
  // view of the serial array returned to the non ghost boundary portion of the array.
  // If we did that then A = -A would require message passing to update the ghost boundaries.
  // So have to fix it here more directly.
  // Bugfix (2/7/96) P++ must avoid counting the ghost boundaries when performing reduction operations!
     Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_Pointer_List;
     for (int i=0; i < MAX_ARRAY_DIMENSION; i++)
        {
       // Index_Pointer_List[i] = &(X.Array_Descriptor.Array_Domain.Local_Mask_Index[i]);
       // This is all a lot more complex if the stride is not the unit stride! 
       // So for now we avoid this case.
          int Ghost_Boundary_Width = X.Array_Descriptor.Array_Domain.InternalGhostCellWidth[i];
          int Local_Base  = X.getLocalBase(i);
          int Local_Bound = X.getLocalBound(i);

       // Left and right edges do not  i n c l u d e  a ghost boundary!
          if (X.Array_Descriptor.Array_Domain.isLeftPartition(i) == FALSE)
             Local_Base += Ghost_Boundary_Width;
          if (X.Array_Descriptor.Array_Domain.isRightPartition(i) == FALSE)
             Local_Bound -= Ghost_Boundary_Width;
       // APP_ASSERT(Local_Base <= Local_Bound);
       // ... (12/27/96,kdb) only valid value might be on a ghost cell so make a NULL INDEX in this case ...
          if (Local_Base <= Local_Bound)
               Index_Pointer_List[i] = new Range (Local_Base,Local_Bound);
            else
               Index_Pointer_List[i] = new Internal_Index (Local_Base,0);
#if 0
          APP_ASSERT (X.Array_Descriptor.Array_Domain.Stride[i] == 1);
#else
       // Now take the intersection of this with the local mask
          (*Index_Pointer_List[i]) = (*Index_Pointer_List[i])(X.Array_Descriptor.Array_Domain.Local_Mask_Index[i]);
#endif
        }

#if defined(MEMORY_LEAK_TEST)
     int Return_Value = 0;
#else
  // Note that we hand the sum operator a view and this means we have
  // to delete the X_SerialArray explicitly (unlike other operators)
     int Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, X, X_SerialArray, sum ((*X_SerialArray)(Index_Pointer_List)) , intArray::sum_Function );
#endif

     for (int k=0; k < MAX_ARRAY_DIMENSION; k++)
        {
          if (Index_Pointer_List[k] != NULL)
             {
               delete Index_Pointer_List[k];
               Index_Pointer_List[k] = NULL;
             }
        }

  // Note that a view is handed into the sum operator (so this is not dependent upon the value of xIsTemporary)
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): X_SerialArray->getReferenceCount() = %d \n",
       //      X_SerialArray->getReferenceCount());

       // Must delete the X_SerialArray if it was taken directly from the X array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (X_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          X_SerialArray->decrementReferenceCount();
          if (X_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete X_SerialArray;
             }
          X_SerialArray = NULL;


  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( X , MDI_i_Sum_Array_Returning_Scalar , intArray::sum_Function );
#endif
   }


intArray &
intArray::operator! ()
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator! for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operatoroperator!");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator!()");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
	       Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this, This_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this, This_SerialArray);
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (
                    *this, This_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask,
                    Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	          ( *this, This_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask,
                    Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL) Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray    != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, *this, This_SerialArray->operator!() );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & intArray::operator! ()");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, *this, This_SerialArray, This_SerialArray->operator!() );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }
     
  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & intArray::operator!()");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator_Returning_IntArray ( *this ,
                   MDI_i_NOT_Array_Returning_IntArray ,
                   MDI_i_NOT_Array_Accumulate_To_Operand_Returning_IntArray , 
                   intArray::Not );
#endif
   }


intArray &
operator- ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator- (intArray,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator-");
     Rhs.Test_Consistency ("Test Rhs in operator-");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator-(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator-(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	    (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
        {
	   puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
           APP_ABORT();
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
	       Rhs, Rhs_SerialArray );
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray - *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, 
	  Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray - *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator-(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Lhs , Rhs , 
               MDI_i_Subtract_Array_Minus_Array,
               MDI_i_Subtract_Array_Minus_Array_Accumulate_To_Operand , intArray::Minus );
#endif
   }


intArray &
operator< ( const intArray & Lhs , const intArray & Rhs )
   {   
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator< (intArray(id=%d),intArray(id=%d)) for intArray class!",
               Lhs.Array_ID(),Rhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator< (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in operator< (intArray,intArray)");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator<(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator<(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

  // printf ("Checking if WHERE statement is used before calling Array::Parallel_Conformability_Enforcement from operator< \n");

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
       // printf ("Checking if indirect addressing is used before calling Array::Parallel_Conformability_Enforcement from operator< \n");
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               printf ("ERROR: can't mix indirect addressing with 2 arrays and where. \n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);
     APP_ASSERT(Temporary_Array_Set != NULL);

#if 0
     Lhs.displayReferenceCounts ("Lhs after PCE in operator< (intArray,intArray)");
     Rhs.displayReferenceCounts ("Rhs after PCE in operator< (intArray,intArray)");
     Lhs_SerialArray->displayReferenceCounts ("Lhs_SerialArray after PCE in operator< (intArray,intArray)");
     Rhs_SerialArray->displayReferenceCounts ("Rhs_SerialArray after PCE in operator< (intArray,intArray)");
#endif

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator< ( const intArray & Lhs , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray < *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


  // Since the Rhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in operator< (intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , Rhs ,
               MDI_i_LT_Array_LT_Array,
               MDI_i_LT_Array_LT_Array_Accumulate_To_Operand , intArray::LT );
#endif
   }   


intArray &
operator< ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator< (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator< (intArray,int)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator<(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, *Lhs_SerialArray < x );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & operator< ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray < x );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator<(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , x ,
               MDI_i_LT_Array_LT_Scalar,
               MDI_i_LT_Array_LT_Scalar_Accumulate_To_Operand , intArray::Scalar_LT );
#endif
   }


intArray &
operator< ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator< (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator< (int,intArray)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator<(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator< ( int x , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x < *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator<(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Rhs , x ,
               MDI_i_LT_Scalar_LT_Array,
               MDI_i_LT_Scalar_LT_Array_Accumulate_To_Operand , intArray::Scalar_LT );
#endif
   }


intArray &
operator> ( const intArray & Lhs , const intArray & Rhs )
   {   
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator> (intArray(id=%d),intArray(id=%d)) for intArray class!",
               Lhs.Array_ID(),Rhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator> (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in operator> (intArray,intArray)");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator>(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator>(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

  // printf ("Checking if WHERE statement is used before calling Array::Parallel_Conformability_Enforcement from operator> \n");

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
       // printf ("Checking if indirect addressing is used before calling Array::Parallel_Conformability_Enforcement from operator> \n");
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               printf ("ERROR: can't mix indirect addressing with 2 arrays and where. \n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);
     APP_ASSERT(Temporary_Array_Set != NULL);

#if 0
     Lhs.displayReferenceCounts ("Lhs after PCE in operator> (intArray,intArray)");
     Rhs.displayReferenceCounts ("Rhs after PCE in operator> (intArray,intArray)");
     Lhs_SerialArray->displayReferenceCounts ("Lhs_SerialArray after PCE in operator> (intArray,intArray)");
     Rhs_SerialArray->displayReferenceCounts ("Rhs_SerialArray after PCE in operator> (intArray,intArray)");
#endif

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator> ( const intArray & Lhs , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray > *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


  // Since the Rhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in operator> (intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , Rhs ,
               MDI_i_GT_Array_GT_Array,
               MDI_i_GT_Array_GT_Array_Accumulate_To_Operand , intArray::GT );
#endif
   }   


intArray &
operator> ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator> (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator> (intArray,int)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator>(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, *Lhs_SerialArray > x );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & operator> ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray > x );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator>(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , x ,
               MDI_i_GT_Array_GT_Scalar,
               MDI_i_GT_Array_GT_Scalar_Accumulate_To_Operand , intArray::Scalar_GT );
#endif
   }


intArray &
operator> ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator> (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator> (int,intArray)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator>(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator> ( int x , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x > *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator>(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Rhs , x ,
               MDI_i_GT_Scalar_GT_Array,
               MDI_i_GT_Scalar_GT_Array_Accumulate_To_Operand , intArray::Scalar_GT );
#endif
   }


intArray &
operator<= ( const intArray & Lhs , const intArray & Rhs )
   {   
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator<= (intArray(id=%d),intArray(id=%d)) for intArray class!",
               Lhs.Array_ID(),Rhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator<= (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in operator<= (intArray,intArray)");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator<=(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator<=(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

  // printf ("Checking if WHERE statement is used before calling Array::Parallel_Conformability_Enforcement from operator<= \n");

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
       // printf ("Checking if indirect addressing is used before calling Array::Parallel_Conformability_Enforcement from operator<= \n");
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               printf ("ERROR: can't mix indirect addressing with 2 arrays and where. \n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);
     APP_ASSERT(Temporary_Array_Set != NULL);

#if 0
     Lhs.displayReferenceCounts ("Lhs after PCE in operator<= (intArray,intArray)");
     Rhs.displayReferenceCounts ("Rhs after PCE in operator<= (intArray,intArray)");
     Lhs_SerialArray->displayReferenceCounts ("Lhs_SerialArray after PCE in operator<= (intArray,intArray)");
     Rhs_SerialArray->displayReferenceCounts ("Rhs_SerialArray after PCE in operator<= (intArray,intArray)");
#endif

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator<= ( const intArray & Lhs , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray <= *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


  // Since the Rhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in operator<= (intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , Rhs ,
               MDI_i_LTEQ_Array_LTEQ_Array,
               MDI_i_LTEQ_Array_LTEQ_Array_Accumulate_To_Operand , intArray::LTEQ );
#endif
   }   


intArray &
operator<= ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator<= (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator<= (intArray,int)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator<=(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, *Lhs_SerialArray <= x );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & operator<= ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray <= x );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator<=(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , x ,
               MDI_i_LTEQ_Array_LTEQ_Scalar,
               MDI_i_LTEQ_Array_LTEQ_Scalar_Accumulate_To_Operand , intArray::Scalar_LTEQ );
#endif
   }


intArray &
operator<= ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator<= (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator<= (int,intArray)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator<=(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator<= ( int x , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x <= *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator<=(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Rhs , x ,
               MDI_i_LTEQ_Scalar_LTEQ_Array,
               MDI_i_LTEQ_Scalar_LTEQ_Array_Accumulate_To_Operand , intArray::Scalar_LTEQ );
#endif
   }


intArray &
operator>= ( const intArray & Lhs , const intArray & Rhs )
   {   
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator>= (intArray(id=%d),intArray(id=%d)) for intArray class!",
               Lhs.Array_ID(),Rhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator>= (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in operator>= (intArray,intArray)");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator>=(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator>=(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

  // printf ("Checking if WHERE statement is used before calling Array::Parallel_Conformability_Enforcement from operator>= \n");

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
       // printf ("Checking if indirect addressing is used before calling Array::Parallel_Conformability_Enforcement from operator>= \n");
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               printf ("ERROR: can't mix indirect addressing with 2 arrays and where. \n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);
     APP_ASSERT(Temporary_Array_Set != NULL);

#if 0
     Lhs.displayReferenceCounts ("Lhs after PCE in operator>= (intArray,intArray)");
     Rhs.displayReferenceCounts ("Rhs after PCE in operator>= (intArray,intArray)");
     Lhs_SerialArray->displayReferenceCounts ("Lhs_SerialArray after PCE in operator>= (intArray,intArray)");
     Rhs_SerialArray->displayReferenceCounts ("Rhs_SerialArray after PCE in operator>= (intArray,intArray)");
#endif

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator>= ( const intArray & Lhs , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray >= *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


  // Since the Rhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in operator>= (intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , Rhs ,
               MDI_i_GTEQ_Array_GTEQ_Array,
               MDI_i_GTEQ_Array_GTEQ_Array_Accumulate_To_Operand , intArray::GTEQ );
#endif
   }   


intArray &
operator- ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator- (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator-");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator-(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray     != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray - x );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator-(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_Subtract_Array_Minus_Scalar,
               MDI_i_Subtract_Array_Minus_Scalar_Accumulate_To_Operand , intArray::Scalar_Minus );
#endif
   }


intArray &
operator>= ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator>= (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator>= (intArray,int)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator>=(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, *Lhs_SerialArray >= x );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & operator>= ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray >= x );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator>=(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , x ,
               MDI_i_GTEQ_Array_GTEQ_Scalar,
               MDI_i_GTEQ_Array_GTEQ_Scalar_Accumulate_To_Operand , intArray::Scalar_GTEQ );
#endif
   }


intArray &
operator>= ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator>= (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator>= (int,intArray)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator>=(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator>= ( int x , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x >= *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator>=(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Rhs , x ,
               MDI_i_GTEQ_Scalar_GTEQ_Array,
               MDI_i_GTEQ_Scalar_GTEQ_Array_Accumulate_To_Operand , intArray::Scalar_GTEQ );
#endif
   }


intArray &
operator== ( const intArray & Lhs , const intArray & Rhs )
   {   
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator== (intArray(id=%d),intArray(id=%d)) for intArray class!",
               Lhs.Array_ID(),Rhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator== (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in operator== (intArray,intArray)");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator==(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator==(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

  // printf ("Checking if WHERE statement is used before calling Array::Parallel_Conformability_Enforcement from operator== \n");

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
       // printf ("Checking if indirect addressing is used before calling Array::Parallel_Conformability_Enforcement from operator== \n");
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               printf ("ERROR: can't mix indirect addressing with 2 arrays and where. \n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);
     APP_ASSERT(Temporary_Array_Set != NULL);

#if 0
     Lhs.displayReferenceCounts ("Lhs after PCE in operator== (intArray,intArray)");
     Rhs.displayReferenceCounts ("Rhs after PCE in operator== (intArray,intArray)");
     Lhs_SerialArray->displayReferenceCounts ("Lhs_SerialArray after PCE in operator== (intArray,intArray)");
     Rhs_SerialArray->displayReferenceCounts ("Rhs_SerialArray after PCE in operator== (intArray,intArray)");
#endif

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator== ( const intArray & Lhs , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray == *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


  // Since the Rhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in operator== (intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , Rhs ,
               MDI_i_EQ_Array_EQ_Array,
               MDI_i_EQ_Array_EQ_Array_Accumulate_To_Operand , intArray::EQ );
#endif
   }   


intArray &
operator== ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator== (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator== (intArray,int)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator==(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, *Lhs_SerialArray == x );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & operator== ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray == x );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator==(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , x ,
               MDI_i_EQ_Array_EQ_Scalar,
               MDI_i_EQ_Array_EQ_Scalar_Accumulate_To_Operand , intArray::Scalar_EQ );
#endif
   }


intArray &
operator== ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator== (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator== (int,intArray)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator==(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator== ( int x , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x == *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator==(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Rhs , x ,
               MDI_i_EQ_Scalar_EQ_Array,
               MDI_i_EQ_Scalar_EQ_Array_Accumulate_To_Operand , intArray::Scalar_EQ );
#endif
   }


intArray &
operator!= ( const intArray & Lhs , const intArray & Rhs )
   {   
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator!= (intArray(id=%d),intArray(id=%d)) for intArray class!",
               Lhs.Array_ID(),Rhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator!= (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in operator!= (intArray,intArray)");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator!=(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator!=(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

  // printf ("Checking if WHERE statement is used before calling Array::Parallel_Conformability_Enforcement from operator!= \n");

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
       // printf ("Checking if indirect addressing is used before calling Array::Parallel_Conformability_Enforcement from operator!= \n");
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               printf ("ERROR: can't mix indirect addressing with 2 arrays and where. \n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);
     APP_ASSERT(Temporary_Array_Set != NULL);

#if 0
     Lhs.displayReferenceCounts ("Lhs after PCE in operator!= (intArray,intArray)");
     Rhs.displayReferenceCounts ("Rhs after PCE in operator!= (intArray,intArray)");
     Lhs_SerialArray->displayReferenceCounts ("Lhs_SerialArray after PCE in operator!= (intArray,intArray)");
     Rhs_SerialArray->displayReferenceCounts ("Rhs_SerialArray after PCE in operator!= (intArray,intArray)");
#endif

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator!= ( const intArray & Lhs , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray != *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


  // Since the Rhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in operator!= (intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , Rhs ,
               MDI_i_NOT_EQ_Array_NOT_EQ_Array,
               MDI_i_NOT_EQ_Array_NOT_EQ_Array_Accumulate_To_Operand , intArray::NOT_EQ );
#endif
   }   


intArray &
operator!= ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator!= (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator!= (intArray,int)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator!=(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, *Lhs_SerialArray != x );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & operator!= ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray != x );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator!=(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , x ,
               MDI_i_NOT_EQ_Array_NOT_EQ_Scalar,
               MDI_i_NOT_EQ_Array_NOT_EQ_Scalar_Accumulate_To_Operand , intArray::Scalar_NOT_EQ );
#endif
   }


intArray &
operator!= ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator!= (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator!= (int,intArray)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator!=(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator!= ( int x , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x != *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator!=(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Rhs , x ,
               MDI_i_NOT_EQ_Scalar_NOT_EQ_Array,
               MDI_i_NOT_EQ_Scalar_NOT_EQ_Array_Accumulate_To_Operand , intArray::Scalar_NOT_EQ );
#endif
   }


intArray &
operator&& ( const intArray & Lhs , const intArray & Rhs )
   {   
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator&& (intArray(id=%d),intArray(id=%d)) for intArray class!",
               Lhs.Array_ID(),Rhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator&& (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in operator&& (intArray,intArray)");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator&&(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator&&(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

  // printf ("Checking if WHERE statement is used before calling Array::Parallel_Conformability_Enforcement from operator&& \n");

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
       // printf ("Checking if indirect addressing is used before calling Array::Parallel_Conformability_Enforcement from operator&& \n");
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               printf ("ERROR: can't mix indirect addressing with 2 arrays and where. \n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);
     APP_ASSERT(Temporary_Array_Set != NULL);

#if 0
     Lhs.displayReferenceCounts ("Lhs after PCE in operator&& (intArray,intArray)");
     Rhs.displayReferenceCounts ("Rhs after PCE in operator&& (intArray,intArray)");
     Lhs_SerialArray->displayReferenceCounts ("Lhs_SerialArray after PCE in operator&& (intArray,intArray)");
     Rhs_SerialArray->displayReferenceCounts ("Rhs_SerialArray after PCE in operator&& (intArray,intArray)");
#endif

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator&& ( const intArray & Lhs , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray && *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


  // Since the Rhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in operator&& (intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , Rhs ,
               MDI_i_AND_Array_AND_Array,
               MDI_i_AND_Array_AND_Array_Accumulate_To_Operand , intArray::AND );
#endif
   }   


intArray &
operator&& ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator&& (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator&& (intArray,int)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator&&(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, *Lhs_SerialArray && x );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & operator&& ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray && x );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator&&(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , x ,
               MDI_i_AND_Array_AND_Scalar,
               MDI_i_AND_Array_AND_Scalar_Accumulate_To_Operand , intArray::Scalar_AND );
#endif
   }


intArray &
operator- ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator- (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator-");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator-(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x - *Rhs_SerialArray );
  // return intArray::Abstract_Binary_Operator ( Temporary_Array_Set, Rhs, x - *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator-(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_Subtract_Scalar_Minus_Array,
               MDI_i_Subtract_Scalar_Minus_Array_Accumulate_To_Operand , intArray::Scalar_Minus );
#endif
   }


intArray &
operator&& ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator&& (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator&& (int,intArray)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator&&(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator&& ( int x , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x && *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator&&(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Rhs , x ,
               MDI_i_AND_Scalar_AND_Array,
               MDI_i_AND_Scalar_AND_Array_Accumulate_To_Operand , intArray::Scalar_AND );
#endif
   }


intArray &
operator|| ( const intArray & Lhs , const intArray & Rhs )
   {   
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of operator|| (intArray(id=%d),intArray(id=%d)) for intArray class!",
               Lhs.Array_ID(),Rhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator|| (intArray,intArray)");
     Rhs.Test_Consistency ("Test Rhs in operator|| (intArray,intArray)");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator||(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & operator||(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

  // printf ("Checking if WHERE statement is used before calling Array::Parallel_Conformability_Enforcement from operator|| \n");

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
       // printf ("Checking if indirect addressing is used before calling Array::Parallel_Conformability_Enforcement from operator|| \n");
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)|| 
              (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               printf ("ERROR: can't mix indirect addressing with 2 arrays and where. \n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Lhs_SerialArray     != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);
     APP_ASSERT(Temporary_Array_Set != NULL);

#if 0
     Lhs.displayReferenceCounts ("Lhs after PCE in operator|| (intArray,intArray)");
     Rhs.displayReferenceCounts ("Rhs after PCE in operator|| (intArray,intArray)");
     Lhs_SerialArray->displayReferenceCounts ("Lhs_SerialArray after PCE in operator|| (intArray,intArray)");
     Rhs_SerialArray->displayReferenceCounts ("Rhs_SerialArray after PCE in operator|| (intArray,intArray)");
#endif

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator|| ( const intArray & Lhs , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray || *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;


  // Since the Rhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in operator|| (intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , Rhs ,
               MDI_i_OR_Array_OR_Array,
               MDI_i_OR_Array_OR_Array_Accumulate_To_Operand , intArray::OR );
#endif
   }   


intArray &
operator|| ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator|| (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator|| (intArray,int)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator||(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, *Lhs_SerialArray || x );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & operator|| ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, *Lhs_SerialArray || x );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

#else
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator||(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Lhs , x ,
               MDI_i_OR_Array_OR_Scalar,
               MDI_i_OR_Array_OR_Scalar_Accumulate_To_Operand , intArray::Scalar_OR );
#endif
   }


intArray &
operator|| ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator|| (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator|| (int,intArray)");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator||(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     printf ("Can't do MEMORY_LEAK_TEST in intArray & operator|| ( int x , const intArray & Rhs ) \n");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x || *Rhs_SerialArray );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
#ifndef INTARRAY
  // Since the Lhs is a different type than the Return Value it could not be reused!
     // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

#else
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }
#endif

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator||(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator_Returning_IntArray ( Rhs , x ,
               MDI_i_OR_Scalar_OR_Array,
               MDI_i_OR_Scalar_OR_Array_Accumulate_To_Operand , intArray::Scalar_OR );
#endif
   }


// Sum along axis friend function!
intArray &
sum ( const intArray & inputArray , int Axis )
{
/*
// ... (Bug Fix, kdb, 7/1/96) Code was previuosly hardwired for 4
//  dimensions, this has been changed to an arbitrary number ...
*/

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          inputArray.displayReferenceCounts("X in intArray & sum (intArray,int)");
        }
#endif

//  We want to preserve the interface being for cost array objects but we
// need to use a non-const representation to make this work with the current MDI layer
// so we will cast away cost to make this work.
   intArray & X = (intArray &) inputArray;

#if defined(MEMORY_LEAK_TEST)
   puts ("Can't do MEMORY_LEAK_TEST in intArray & sum ( const intArray & X , int Axis )");
#endif

//==============================================================
#if COMPILE_DEBUG_STATEMENTS
   if (APP_DEBUG > 0)
      puts ("Inside of intArray & sum ( const intArray & X , int Axis ) for intArray class!");
#endif
//==============================================================

#if defined(PPP)
   // In P++ this function is only implemented for a single processor 
   // case (for now)!
   APP_ASSERT (Communication_Manager::Number_Of_Processors == 1);
#endif

//==============================================================
#if COMPILE_DEBUG_STATEMENTS
   // This is the only test we can do on the input!
   X.Test_Consistency ("Test X in sum (const intArray & X, int Axis)");
#endif
//==============================================================

   // Build result array (it will be marked as a temporary before we 
   // return)
   intArray* Result = NULL;

   if (X.Array_Descriptor.Array_Domain.Is_A_Temporary)
   {
      // We could reuse the temporary by taking a view of it (the view 
      // would be of one lower dimension) This avoids any 
      // initialization of the result and the return vaules are 
      // computed in place.

//==============================================================
#if COMPILE_DEBUG_STATEMENTS
      if (APP_DEBUG > 0) puts ("Input is a temporary!");
#endif
//==============================================================
 
      int Base   [MAX_ARRAY_DIMENSION];
      int Length [MAX_ARRAY_DIMENSION];
      int Stride [MAX_ARRAY_DIMENSION];

      int i;
      for (i=0;i<MAX_ARRAY_DIMENSION;i++)
      {
         Base[i]   = X.Array_Descriptor.Array_Domain.Base[i]+
            X.Array_Descriptor.Array_Domain.Data_Base[i];
         Length[i] = (X.Array_Descriptor.Array_Domain.Bound[i]-
            X.Array_Descriptor.Array_Domain.Base[i]) + 1;
         Stride[i] = X.Array_Descriptor.Array_Domain.Stride[i];
      }

      // Increment reference count of data we are building a view of!
      // This allows the temporary to be reused (thus accumulating the 
      // result into the temporary)
      // Note that the we have to delete the temporary at the end of 
      // the function in order to decrement the reference count on the 
      // data.  And since we reuse the temporary we have to increment 
      // (and then decrement) the base along the axis of sumation to 
      // avoid adding the first row to itself.  A nasty detail!

      // Array_Descriptor_Type::Array_Reference_Count_Array 
      //    [X.Array_Descriptor.Array_Domain.Array_ID]++;

      X.incrementRawDataReferenceCount();

//==============================================================
#if COMPILE_DEBUG_STATEMENTS
      if (APP_DEBUG > 0)
        puts ("Do axis specific allocation of temporary!");
#endif
//==============================================================

      // Dimension the array with the specified Axis collapsed
      for (i=0; i<MAX_ARRAY_DIMENSION;i++)
         if (i == Axis)
	 {
	   Length[i] = 1;
	   Stride[i] = 1;
	 }

      Index_Pointer_Array_MAX_ARRAY_DIMENSION_Type Index_List;
      Index Index_Array[MAX_ARRAY_DIMENSION];

      for (i=0; i<MAX_ARRAY_DIMENSION;i++)
      {
         Index_Array[i] = Index (Base[i],Length[i],Stride[i]);
         Index_List[i] = &Index_Array[i];
      }

#if defined(PPP)
      // Must build a view of X.SerialArray to make it consistant 
      // with the P++ descriptor

      // SerialArray_Descriptor_Type::Array_Reference_Count_Array 
      //   [X.SerialArray->Array_Descriptor.Array_Domain.Array_ID]++;

       X.incrementRawDataReferenceCount();
       intSerialArray *View = new intSerialArray 
         ( X.Array_Descriptor.SerialArray->Array_Descriptor.Array_Data , 
	   X.Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain,Index_List);
       Result = new intArray 
         (View , X.Array_Descriptor.Array_Domain,Index_List);
#else
      Result = new intArray 
         (X.Array_Descriptor.Array_Data , 
	  X.Array_Descriptor.Array_Domain,Index_List);
#endif

     // Modify the Input descriptor to avoid the sum of the first 
     // value along the chosen axis to itself. This allows the MDI 
     // function to accumulate the result into itself thus reusing 
     // the temporary.  But to make this work we have to skip the 
     // sumation of the
     // first row into itself.  A very nasty detail!
     // This should be a issue for any threaded computation (THREAD SAFETY ALERT)

     X.Array_Descriptor.Array_Domain.Base[Axis] += 
	X.Array_Descriptor.Array_Domain.Stride[Axis];

//==============================================================
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        puts ("DONE with axis specific allocation of temporary!");
#endif
//==============================================================
   }
   else
   {
      Integer_Array_MAX_ARRAY_DIMENSION_Type Integer_List;
      int i;
      for (i=0;i<MAX_ARRAY_DIMENSION;i++)
         Integer_List[i] = X.getLength (i);

      Integer_List[Axis] = 1;

      // Input is not a temporary so we have to build a return array (of 
      // the correct dimensions)
      // Dimension the array with the specified Axis collapsed

      Integer_List[Axis] = 1;
      Result = new intArray (Integer_List);
   }

#if defined(PPP)
  // Skip this for now since it is sort of complex
  // puts ("P++ sum function (sum along an axis) not completely implemented yet (this function is more complex in P++ and will be done last)!");
  // APP_ABORT();
  // Use avoid compiler warning
  // int Avoid_Compiler_Warning = Axis;
  // Use avoid compiler warning
  // return (intArray &) X;

  // This could be more efficent!
     APP_ASSERT(Result != NULL);
     APP_ASSERT(Result->Array_Descriptor.SerialArray != NULL);
     //APP_ASSERT(Result->Array_Descriptor != NULL);
     APP_ASSERT(X.Array_Descriptor.SerialArray != NULL);
     //APP_ASSERT(X.Array_Descriptor != NULL);

  // Result->view("Result->view (BEFORE SUM)");
  // Result->SerialArray->view("Result->SerialArray->view");
  // if (X.Array_Descriptor.Array_Domain.Is_A_Temporary == TRUE)
  //      *(Result->SerialArray) = sum ( *(X.SerialArray) , Axis );

  // puts ("Call intSerialArray sum along axis function!");
  // Mark as a NON-temporary to avoid temporary handling which would absorb the temporary
  // This could be done more efficently by testing for a temporary and skipping the
  // call to the assignment operator! Later!
  // Result->Array_Descriptor.Array_Domain.Is_A_Temporary = FALSE;
     Result->Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain.
	Is_A_Temporary = FALSE;

     APP_ASSERT(Result->Array_Descriptor.SerialArray->Array_Descriptor.
		Array_Domain.Is_A_Temporary == FALSE);
     *(Result->Array_Descriptor.SerialArray) = 
	sum ( *(X.Array_Descriptor.SerialArray) , Axis );
  // Result->view("Result->view (AFTER SUM)");

  // puts ("Returning from intArray sum along axis!");

  // Mark as a temporary
     Result->Array_Descriptor.Array_Domain.Is_A_Temporary = TRUE;
     Result->Array_Descriptor.SerialArray->Array_Descriptor.Array_Domain.
	Is_A_Temporary = TRUE;

#if COMPILE_DEBUG_STATEMENTS
  // This is the only test we can do on the input!
     Result->Test_Consistency ("Test Result in sum (const intArray & X, int Axis)");
#endif

     return *Result;
  // End of P++ code
#else
  // Start of A++ code

  // Variables to hold data obtainted from inlined access functions
     int *Mask_Array_Data   = NULL;
     array_domain *Mask_Descriptor   = NULL;

  // Check for Where Mask
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          puts ("In sum(intiArray,int): Where Mask usage not implemented (it is largely meaningless in this case)!");
          APP_ABORT();
       // Mask_Array_Data = Where_Statement_Support::Where_Statement_Mask->Array_Data;
       // Mask_Descriptor = (int*) Where_Statement_Support::Where_Statement_Mask->Array_Descriptor;
        }

     APP_ASSERT(Result->Array_Descriptor.Array_Data       != NULL);
     APP_ASSERT(X.Array_Descriptor.Array_Data             != NULL);
     //APP_ASSERT(Result->Array_Descriptor                != NULL);
     //APP_ASSERT(X.Array_Descriptor                      != NULL);

  // If the input was not a temporary then a temporary was created (which must be initialized to zero)
     if (X.Array_Descriptor.Array_Domain.Is_A_Temporary == FALSE)
        {
       // Initialize the just allocated temporary to ZERO.  Because this could be a 3D array
       // (one dimension less than the input) this operation is not trivial.  To simplify this
       // we call the MDI function for assignment of a scalar toan array directly.  This would be 
       // more efficient than calling the A++ operator= though this function could be made more
       // efficient if we were to do the initialization in the MDI_i_Sum_Array_Along_Axis function
       // but that would be more complex so I will skip that for now.
          MDI_i_Assign_Array_Equals_Scalar_Accumulate_To_Operand 
	     ( Result->Array_Descriptor.Array_Data , 0 , Mask_Array_Data ,
              (array_domain*) &(Result->Array_Descriptor.Array_Domain) , 
	      Mask_Descriptor );
        }

  // Hand off to the MDI layer for more efficent computation
     MDI_i_Sum_Array_Along_Axis 
	( Axis, Result->Array_Descriptor.Array_Data, X.Array_Descriptor.Array_Data, 
	  Mask_Array_Data, (array_domain*) &(Result->Array_Descriptor.Array_Domain), 
	  (array_domain*) &(X.Array_Descriptor.Array_Domain) , Mask_Descriptor );

  // Bug fix (9/11/94) we reuse the temporary!
  // Delete the input array object if it was a temporary
     if (X.Array_Descriptor.Array_Domain.Is_A_Temporary == TRUE)
        {
       // Above the descriptor was modified to avoid sumation of the first element along
       // the choosen axis with itself.  So now we have to undo it just to make sure that
       // even if the descriptor is referenced somewhere else we have have returned it to
       // its correct state.  I guess we could avoid the fixup if the descriptor's reference
       // count implied it had no additional references but this is more elegant.
          X.Array_Descriptor.Array_Domain.Base[Axis] =- X.Array_Descriptor.Array_Domain.Stride[Axis];

       // Now we have to delete the input since it was a temporary 
       // (this is part of the temporary management that A++ does)
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          X.decrementReferenceCount();
          if (X.getReferenceCount() < intArray::getReferenceCountBase())
               delete &((intArray &) X);
        }

  // puts ("Returning from intSerialArray sum along axis!");

  // Mark as a temporary
     Result->Array_Descriptor.Array_Domain.Is_A_Temporary = TRUE;

#if COMPILE_DEBUG_STATEMENTS
  // This is the only test we can do on the input!
     Result->Test_Consistency ("Test Result in sum (const intArray & X, int Axis)");
#endif

     return *Result;
#endif
   }


#ifndef INTARRAY
intArray &
atan2 ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of atan2 (intArray,intArray) for intArray class! Lhs:rc=%d Rhs:rc=%d ",
               Lhs.getRawDataReferenceCount(),Rhs.getRawDataReferenceCount());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in atan2");
     Rhs.Test_Consistency ("Test Rhs in atan2");
#endif
 
  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & atan2(intArray,intArray)");
          Rhs.displayReferenceCounts("Rhs in intArray & atan2(intArray,intArray)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs and Rhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement (Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
	       puts ("ERROR: can't mix indirect addressing with 2 arrays and where.");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, 
                    *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray,
                     Rhs, Rhs_SerialArray );
             }

          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

  // New test (8/5/2000)
     APP_ASSERT(Temporary_Array_Set != NULL);
  // Temporary_Array_Set->display("Check to see what sort of communication model was used");

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & atan2(intArray,intArray)");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & atan2(intArray,intArray)");
        }
#endif

  // Inputs to intArray::Abstract_Binary_Operator:
  //     1. Temporary_Array_Set is attached to the intArray temporary returned by Abstract_Binary_Operator
  //     2. Lhs is used to get the Lhs partition information (PARTI parallel descriptor) and array reuse
  //     3. Rhs is used to get the Rhs partition information (PARTI parallel descriptor) in case the Lhs was 
  //        a NULL array (no data and no defined partitioning (i.e. no PARTI parallel descriptor)) and array reuse
  //     4. The intSerialArray which is to be put into the intArray temporary returned by Abstract_Binary_Operator
  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, atan2(*Lhs_SerialArray,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, atan2(*Lhs_SerialArray,*Rhs_SerialArray) );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
  // if (Lhs_SerialArray != Return_Value.getSerialArrayPointer())
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & atan2(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Lhs , Rhs , MDI_i_Arc_Tan2_Array_ArcTan2_Array, MDI_i_Arc_Tan2_Array_ArcTan2_Array_Accumulate_To_Operand , intArray::atan2_Function );
#endif
   }



intArray &
atan2 ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\nInside of atan2 (int,intArray) for intArray class! \n");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in atan2");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & atan2(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray *Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...
     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
              (Rhs, Rhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}

        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray     != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & atan2(int,intArray)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, atan2(x,*Rhs_SerialArray) );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, atan2(x,*Rhs_SerialArray) );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete macro) in intArray & atan2(int,intArray)");
        }
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & atan2(int,intArray)");
        }
#endif

     return Return_Value;
#else
  // This function can be either Abstract_Binary_Operator or Abstract_Binary_Operator_Non_Commutative
     return intArray::Abstract_Binary_Operator_Non_Commutative ( Rhs , x ,
               MDI_i_Arc_Tan2_Scalar_ArcTan2_Array,
               MDI_i_Arc_Tan2_Scalar_ArcTan2_Array_Accumulate_To_Operand , intArray::Scalar_atan2_Function );
#endif
   }



intArray &
atan2 ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("Inside of atan2 (intArray,int) for intArray class!");

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in atan2");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts 
          Lhs.displayReferenceCounts("Lhs in intArray & atan2(intArray,int)");
        }
#endif

#if defined(PPP)
  // Pointers to views of Lhs serial arrays which allow a conformable operation
     intSerialArray *Lhs_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE)
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Lhs, Lhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & atan2(intArray,int)");
        }
#endif

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, atan2(*Lhs_SerialArray,x) );
#if defined(MEMORY_LEAK_TEST)
     puts ("Can't do MEMORY_LEAK_TEST in intArray & atan2 ( const intArray & Lhs , int x )");
#endif
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Lhs_SerialArray, atan2(*Lhs_SerialArray,x) );

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray;
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & atan2(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x ,
               MDI_i_Arc_Tan2_Array_ArcTan2_Scalar,
               MDI_i_Arc_Tan2_Array_ArcTan2_Scalar_Accumulate_To_Operand , intArray::Scalar_atan2_Function );
#endif
   }

#endif

#ifdef INTARRAY
intArray &
operator& ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("\n\n\n### Inside of operator& (intArray,intArray) for intArray class: (id=%d) = (id=%d) \n",
               Lhs.Array_ID(),Rhs.Array_ID());
        }

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator&");
     Rhs.Test_Consistency ("Test Rhs in operator&");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs.isTemporary() = %s \n",(Lhs.isTemporary()) ? "TRUE" : "FALSE");
          Lhs.displayReferenceCounts("Lhs in intArray & operator&(intArray,intArray)");
          printf ("intArray: Rhs.isTemporary() = %s \n",(Rhs.isTemporary()) ? "TRUE" : "FALSE");
          Rhs.displayReferenceCounts("Rhs in intArray & operator&(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ( (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
               (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) )
             {
               printf ("Sorry, not implemented: can't mix indirect addressing using where statements and two array (binary) operators!\n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
                  (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator&(intArray,intArray)");
          printf ("intArray: Rhs_SerialArray->isTemporary() = %s \n",(Rhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & operator&(intArray,intArray)");
        }
#endif

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray & *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray & *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator&(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , 
               MDI_i_BIT_AND_Array_BitwiseAND_Array,
               MDI_i_BIT_AND_Array_BitwiseAND_Array_Accumulate_To_Operand , intArray::BitwiseAND );
#endif
   }

intArray &
operator& ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\n### Inside of operator& (intArray,int) for intArray class: (id=%d) = scalar \n",Lhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator&");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator&(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
               APP_ASSERT(Lhs_SerialArray != NULL);
            // Lhs_SerialArray->displayReferenceCounts("AFTER PCE: *Lhs_SerialArray in intArray & operator&(intArray,int)");
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator&(intArray,int)");
        }
#endif

  // (11/27/2000) Added error checking (will not work with indirect addessing later!!!)
     APP_ASSERT(Temporary_Array_Set != NULL);

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator(Temporary_Array_Set,Lhs,Lhs_SerialArray,*Lhs_SerialArray & x);
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in in intArray & operator&(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_BIT_AND_Array_BitwiseAND_Scalar,
               MDI_i_BIT_AND_Array_BitwiseAND_Scalar_Accumulate_To_Operand , intArray::Scalar_BitwiseAND );
#endif
   }

intArray &
operator& ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator& (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator&");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator&(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x & *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator&(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_BIT_AND_Scalar_BitwiseAND_Array,
               MDI_i_BIT_AND_Scalar_BitwiseAND_Array_Accumulate_To_Operand , intArray::Scalar_BitwiseAND );
#endif
   }

intArray &
intArray::operator&= ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator&= (int) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator&=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator&=(int)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, This_SerialArray, *This_SerialArray &= x );
     // ... don't need to use macro because Return_Value won't be Mask ...
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;

#else
     intArray::Abstract_Operator_Operation_Equals ( *this , x ,
        MDI_i_BIT_AND_Array_BitwiseAND_Scalar_Accumulate_To_Operand , intArray::Scalar_BitwiseAND_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator&=(int)");
        }
#endif

     return *this;
   }

#endif

#ifdef INTARRAY
intArray &
operator| ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("\n\n\n### Inside of operator| (intArray,intArray) for intArray class: (id=%d) = (id=%d) \n",
               Lhs.Array_ID(),Rhs.Array_ID());
        }

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator|");
     Rhs.Test_Consistency ("Test Rhs in operator|");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs.isTemporary() = %s \n",(Lhs.isTemporary()) ? "TRUE" : "FALSE");
          Lhs.displayReferenceCounts("Lhs in intArray & operator|(intArray,intArray)");
          printf ("intArray: Rhs.isTemporary() = %s \n",(Rhs.isTemporary()) ? "TRUE" : "FALSE");
          Rhs.displayReferenceCounts("Rhs in intArray & operator|(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ( (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
               (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) )
             {
               printf ("Sorry, not implemented: can't mix indirect addressing using where statements and two array (binary) operators!\n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
                  (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator|(intArray,intArray)");
          printf ("intArray: Rhs_SerialArray->isTemporary() = %s \n",(Rhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & operator|(intArray,intArray)");
        }
#endif

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray | *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray | *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator|(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , 
               MDI_i_BIT_OR_Array_BitwiseOR_Array,
               MDI_i_BIT_OR_Array_BitwiseOR_Array_Accumulate_To_Operand , intArray::BitwiseOR );
#endif
   }

intArray &
operator| ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\n### Inside of operator| (intArray,int) for intArray class: (id=%d) = scalar \n",Lhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator|");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator|(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
               APP_ASSERT(Lhs_SerialArray != NULL);
            // Lhs_SerialArray->displayReferenceCounts("AFTER PCE: *Lhs_SerialArray in intArray & operator|(intArray,int)");
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator|(intArray,int)");
        }
#endif

  // (11/27/2000) Added error checking (will not work with indirect addessing later!!!)
     APP_ASSERT(Temporary_Array_Set != NULL);

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator(Temporary_Array_Set,Lhs,Lhs_SerialArray,*Lhs_SerialArray | x);
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in in intArray & operator|(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_BIT_OR_Array_BitwiseOR_Scalar,
               MDI_i_BIT_OR_Array_BitwiseOR_Scalar_Accumulate_To_Operand , intArray::Scalar_BitwiseOR );
#endif
   }

intArray &
operator| ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator| (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator|");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator|(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x | *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator|(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_BIT_OR_Scalar_BitwiseOR_Array,
               MDI_i_BIT_OR_Scalar_BitwiseOR_Array_Accumulate_To_Operand , intArray::Scalar_BitwiseOR );
#endif
   }

intArray &
intArray::operator|= ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator|= (int) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator|=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator|=(int)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, This_SerialArray, *This_SerialArray |= x );
     // ... don't need to use macro because Return_Value won't be Mask ...
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;

#else
     intArray::Abstract_Operator_Operation_Equals ( *this , x ,
        MDI_i_BIT_OR_Array_BitwiseOR_Scalar_Accumulate_To_Operand , intArray::Scalar_BitwiseOR_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator|=(int)");
        }
#endif

     return *this;
   }

#endif

#ifdef INTARRAY
intArray &
operator^ ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("\n\n\n### Inside of operator^ (intArray,intArray) for intArray class: (id=%d) = (id=%d) \n",
               Lhs.Array_ID(),Rhs.Array_ID());
        }

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator^");
     Rhs.Test_Consistency ("Test Rhs in operator^");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs.isTemporary() = %s \n",(Lhs.isTemporary()) ? "TRUE" : "FALSE");
          Lhs.displayReferenceCounts("Lhs in intArray & operator^(intArray,intArray)");
          printf ("intArray: Rhs.isTemporary() = %s \n",(Rhs.isTemporary()) ? "TRUE" : "FALSE");
          Rhs.displayReferenceCounts("Rhs in intArray & operator^(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ( (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
               (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) )
             {
               printf ("Sorry, not implemented: can't mix indirect addressing using where statements and two array (binary) operators!\n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
                  (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator^(intArray,intArray)");
          printf ("intArray: Rhs_SerialArray->isTemporary() = %s \n",(Rhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & operator^(intArray,intArray)");
        }
#endif

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray ^ *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray ^ *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator^(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , 
               MDI_i_BIT_XOR_Array_BitwiseXOR_Array,
               MDI_i_BIT_XOR_Array_BitwiseXOR_Array_Accumulate_To_Operand , intArray::BitwiseXOR );
#endif
   }

intArray &
operator^ ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\n### Inside of operator^ (intArray,int) for intArray class: (id=%d) = scalar \n",Lhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator^");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator^(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
               APP_ASSERT(Lhs_SerialArray != NULL);
            // Lhs_SerialArray->displayReferenceCounts("AFTER PCE: *Lhs_SerialArray in intArray & operator^(intArray,int)");
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator^(intArray,int)");
        }
#endif

  // (11/27/2000) Added error checking (will not work with indirect addessing later!!!)
     APP_ASSERT(Temporary_Array_Set != NULL);

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator(Temporary_Array_Set,Lhs,Lhs_SerialArray,*Lhs_SerialArray ^ x);
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in in intArray & operator^(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_BIT_XOR_Array_BitwiseXOR_Scalar,
               MDI_i_BIT_XOR_Array_BitwiseXOR_Scalar_Accumulate_To_Operand , intArray::Scalar_BitwiseXOR );
#endif
   }

intArray &
operator^ ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator^ (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator^");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator^(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x ^ *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator^(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_BIT_XOR_Scalar_BitwiseXOR_Array,
               MDI_i_BIT_XOR_Scalar_BitwiseXOR_Array_Accumulate_To_Operand , intArray::Scalar_BitwiseXOR );
#endif
   }

intArray &
intArray::operator^= ( int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of intArray::operator^= (int) for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operator^=");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator^=(int)");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

#if !defined(MEMORY_LEAK_TEST)
     intArray::Abstract_Modification_Operator ( Temporary_Array_Set, *this, This_SerialArray, *This_SerialArray ^= x );
     // ... don't need to use macro because Return_Value won't be Mask ...
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;

#else
     intArray::Abstract_Operator_Operation_Equals ( *this , x ,
        MDI_i_BIT_XOR_Array_BitwiseXOR_Scalar_Accumulate_To_Operand , intArray::Scalar_BitwiseXOR_Equals );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS (return value) in intArray & intArray::operator^=(int)");
        }
#endif

     return *this;
   }

#endif

#ifdef INTARRAY
/* There is no <<= operator and so the << must be handled as a special case -- skip it for now */
intArray &
operator<< ( const intArray & Lhs , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
        {
          printf ("\n\n\n### Inside of operator<< (intArray,intArray) for intArray class: (id=%d) = (id=%d) \n",
               Lhs.Array_ID(),Rhs.Array_ID());
        }

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator<<");
     Rhs.Test_Consistency ("Test Rhs in operator<<");
#endif

  // Are the arrays the same size (otherwise issue error message and stop).
     if (Index::Index_Bounds_Checking)
          Lhs.Test_Conformability (Rhs);

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs.isTemporary() = %s \n",(Lhs.isTemporary()) ? "TRUE" : "FALSE");
          Lhs.displayReferenceCounts("Lhs in intArray & operator<<(intArray,intArray)");
          printf ("intArray: Rhs.isTemporary() = %s \n",(Rhs.isTemporary()) ? "TRUE" : "FALSE");
          Rhs.displayReferenceCounts("Rhs in intArray & operator<<(intArray,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;
     intSerialArray* Rhs_SerialArray = NULL;

     intSerialArray* Mask_SerialArray = NULL;
     intSerialArray* Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if ((Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
	      (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE))
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray, Rhs, Rhs_SerialArray );
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if ( (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) ||
               (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) )
             {
               printf ("Sorry, not implemented: can't mix indirect addressing using where statements and two array (binary) operators!\n");
               APP_ABORT();
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement
                  (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray, Rhs, Rhs_SerialArray );
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator<<(intArray,intArray)");
          printf ("intArray: Rhs_SerialArray->isTemporary() = %s \n",(Rhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Rhs_SerialArray->displayReferenceCounts("Rhs_SerialArray in intArray & operator<<(intArray,intArray)");
        }
#endif

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();
     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );
     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, Lhs, Rhs, *Lhs_SerialArray << *Rhs_SerialArray );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator 
	( Temporary_Array_Set, Lhs, Rhs, Lhs_SerialArray, Rhs_SerialArray, *Lhs_SerialArray << *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator<<(intArray,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , Rhs , 
               MDI_i_BIT_LSHIFT_Array_BitwiseLShift_Array,
               MDI_i_BIT_LSHIFT_Array_BitwiseLShift_Array_Accumulate_To_Operand , intArray::BitwiseLShift );
#endif
   }

intArray &
operator<< ( const intArray & Lhs , int x )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          printf ("\n\n\n### Inside of operator<< (intArray,int) for intArray class: (id=%d) = scalar \n",Lhs.Array_ID());

  // This is the only test we can do on the input!
     Lhs.Test_Consistency ("Test Lhs in operator<<");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Lhs.displayReferenceCounts("Lhs in intArray & operator<<(intArray,int)");
        }
#endif

#if defined(PPP)
     intSerialArray* Lhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
        {
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( Lhs, Lhs_SerialArray );
               APP_ASSERT(Lhs_SerialArray != NULL);
            // Lhs_SerialArray->displayReferenceCounts("AFTER PCE: *Lhs_SerialArray in intArray & operator<<(intArray,int)");
             }
        }
       else
        {
          Old_Mask_SerialArray = Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
          if (Lhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
             {
               Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
            else
             {
               Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
                    (Lhs, Lhs_SerialArray, *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
             }
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Mask_SerialArray;
        }

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          printf ("intArray: Lhs_SerialArray->isTemporary() = %s \n",(Lhs_SerialArray->isTemporary()) ? "TRUE" : "FALSE");
          Lhs_SerialArray->displayReferenceCounts("Lhs_SerialArray in intArray & operator<<(intArray,int)");
        }
#endif

  // (11/27/2000) Added error checking (will not work with indirect addessing later!!!)
     APP_ASSERT(Temporary_Array_Set != NULL);

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set = new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Lhs_SerialArray != NULL);

     bool lhsIsTemporary = Lhs_SerialArray->isTemporary();

     APP_ASSERT ( (lhsIsTemporary == TRUE) || (lhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Lhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator(Temporary_Array_Set,Lhs,Lhs_SerialArray,*Lhs_SerialArray << x);
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (lhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Lhs_SerialArray->getReferenceCount() = %d \n",
       //      Lhs_SerialArray->getReferenceCount());

       // Must delete the Lhs_SerialArray if it was taken directly from the Lhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Lhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Lhs_SerialArray->decrementReferenceCount();
          if (Lhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Lhs_SerialArray;
             }
          Lhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
          *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in in intArray & operator<<(intArray,int)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Lhs , x , 
               MDI_i_BIT_LSHIFT_Array_BitwiseLShift_Scalar,
               MDI_i_BIT_LSHIFT_Array_BitwiseLShift_Scalar_Accumulate_To_Operand , intArray::Scalar_BitwiseLShift );
#endif
   }

intArray &
operator<< ( int x , const intArray & Rhs )
   {
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of operator<< (int,intArray) for intArray class!");

  // This is the only test we can do on the input!
     Rhs.Test_Consistency ("Test Rhs in operator<<");
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Rhs.displayReferenceCounts("Rhs in intArray & operator<<(int,intArray)");
        }
#endif

#if defined(PPP)
     intSerialArray* Rhs_SerialArray = NULL;

  // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray     = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL;
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      ( Rhs, Rhs_SerialArray );
        }
	else
	{
           Temporary_Array_Set = 
	      intArray::Parallel_Conformability_Enforcement ( Rhs, Rhs_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Rhs.Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (Rhs, Rhs_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }
     if (Temporary_Array_Set == NULL) Temporary_Array_Set =
	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(Rhs_SerialArray != NULL);

     bool rhsIsTemporary = Rhs_SerialArray->isTemporary();

     APP_ASSERT ( (rhsIsTemporary == TRUE) || (rhsIsTemporary == FALSE) );

#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = Rhs;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, Rhs, Rhs_SerialArray, x << *Rhs_SerialArray );
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (rhsIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): Rhs_SerialArray->getReferenceCount() = %d \n",
       //      Rhs_SerialArray->getReferenceCount());

       // Must delete the Rhs_SerialArray if it was taken directly from the Rhs array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (Rhs_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          Rhs_SerialArray->decrementReferenceCount();
          if (Rhs_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete Rhs_SerialArray;
             }
          Rhs_SerialArray = NULL;

        }

  // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & operator<<(int,intArray)");
        }
#endif

     return Return_Value;
#else
     return intArray::Abstract_Binary_Operator ( Rhs , x , 
               MDI_i_BIT_LSHIFT_Scalar_BitwiseLShift_Array,
               MDI_i_BIT_LSHIFT_Scalar_BitwiseLShift_Array_Accumulate_To_Operand , intArray::Scalar_BitwiseLShift );
#endif
   }


intArray &
intArray::operator~ () const
   { 
#if COMPILE_DEBUG_STATEMENTS
     if (APP_DEBUG > 0)
          puts ("Inside of unary minus operator operator~ for intArray class!");

  // This is the only test we can do on the input!
     Test_Consistency ("Test *this in intArray::operatoroperator~");
#endif
 
#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          displayReferenceCounts("THIS in intArray & intArray::operator~()");
        }
#endif

#if defined(PPP)
     intSerialArray *This_SerialArray = NULL;

     // ... bug fix (4/10/97, kdb) must make serial where mask conformable ...

     intSerialArray *Mask_SerialArray = NULL;
     intSerialArray *Old_Mask_SerialArray = NULL;

     Array_Conformability_Info_Type *Temporary_Array_Set = NULL; 
     if (Where_Statement_Support::Where_Statement_Mask == NULL)
     {
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
	   Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement ( *this , This_SerialArray );
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement ( *this , This_SerialArray );
	}
     }
     else
     {
	Old_Mask_SerialArray = 
           Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointer();
        if (Array_Descriptor.Array_Domain.Uses_Indirect_Addressing == TRUE) 
        {
           Temporary_Array_Set = intArray::Parallel_Indirect_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
        }
	else
	{
           Temporary_Array_Set = intArray::Parallel_Conformability_Enforcement 
	      (*this, This_SerialArray, 
               *Where_Statement_Support::Where_Statement_Mask, Mask_SerialArray);
	}
        *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() =
	   Mask_SerialArray;
     }

     if (Temporary_Array_Set == NULL)
          Temporary_Array_Set =	new Array_Conformability_Info_Type();

     APP_ASSERT(Temporary_Array_Set != NULL);
     APP_ASSERT(This_SerialArray != NULL);

     bool thisIsTemporary = This_SerialArray->isTemporary();

     APP_ASSERT ( (thisIsTemporary == TRUE) || (thisIsTemporary == FALSE) );

  // return intArray::Abstract_Operator ( Temporary_Array_Set, *this, This_SerialArray->operator~() );
#if defined(MEMORY_LEAK_TEST)
     intArray & Return_Value = *this;
#else
     intArray & Return_Value = intArray::Abstract_Operator ( Temporary_Array_Set, *this, This_SerialArray, This_SerialArray->operator~() );
#endif

#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value (before delete This_SerialArray) in intArray & intArray::operator~()");
        }

  // This is the only test we can do on the output!
     Return_Value.Test_Consistency ("Test Return_Value (before delete This_SerialArray) in intArray::operatoroperator~");
#endif

  // Check for reuse of serialArray object in return value (do not delete it if it was reused)
     if (thisIsTemporary == FALSE)
        {
          // Only delete the serial array data when the Overlap update model is used
       // printf ("In Macro Delete SerialArray (before decrement): This_SerialArray->getReferenceCount() = %d \n",
       //      This_SerialArray->getReferenceCount());

       // Must delete the This_SerialArray if it was taken directly from the This array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          APP_ASSERT (This_SerialArray->getReferenceCount() >= intSerialArray::getReferenceCountBase());
          This_SerialArray->decrementReferenceCount();
          if (This_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete This_SerialArray;
             }
          This_SerialArray = NULL;

        }

     // ... don't need to use macro because Return_Value won't be Mask ...
     if (Where_Statement_Support::Where_Statement_Mask != NULL)
        {
         *Where_Statement_Support::Where_Statement_Mask->getSerialArrayPointerLoc() = Old_Mask_SerialArray; 
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          Mask_SerialArray->decrementReferenceCount();
          if (Mask_SerialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
               delete Mask_SerialArray;
          Mask_SerialArray = NULL;
        }

     // Delete the Temporary_Array_Set
  // printf ("In MACRO in operator.C: Temporary_Array_Set->getReferenceCount() = %d \n",Temporary_Array_Set->getReferenceCount());
     APP_ASSERT (Temporary_Array_Set->getReferenceCount() >= Array_Conformability_Info_Type::getReferenceCountBase());
     Temporary_Array_Set->decrementReferenceCount();
     if (Temporary_Array_Set->getReferenceCount() < Array_Conformability_Info_Type::getReferenceCountBase())
        {
       // printf ("COMMENTED OUT CALL TO DELETE: Deleting the Temporary_Array_Set in Macro Delete Temporary_Array_Set \n");
          delete Temporary_Array_Set;
        }
     Temporary_Array_Set = NULL;


#if COMPILE_DEBUG_STATEMENTS
     if (Diagnostic_Manager::getReferenceCountingReport() > 0)
        {
       // This mechanism outputs reports which allow us to trace the reference counts
          Return_Value.displayReferenceCounts("Return_Value in intArray & intArray::operator~()");
        }

  // This is the only test we can do on the output!
     Return_Value.Test_Consistency ("Test Return_Value in intArray::operatoroperator~");
#endif

     return Return_Value;
#else
     return intArray::Abstract_Unary_Operator ( *this ,
                   MDI_i_BIT_COMPLEMENT_Array ,
                   MDI_i_BIT_COMPLEMENT_Array_Accumulate_To_Operand , intArray::Unary_Minus );
#endif
   } 

#endif



















/*
// This code would simplify the macro to a function call
define(Macro_Delete_X_SerialArray,    Delete_SerialArray(X,X_SerialArray,Temporary_Array_Set);)
define(Macro_Delete_This_SerialArray, Delete_SerialArray((*this),This_SerialArray,Temporary_Array_Set);)
define(Macro_Delete_Lhs_SerialArray,  Delete_SerialArray(Lhs,Lhs_SerialArray,Temporary_Array_Set);)
define(Macro_Delete_Rhs_SerialArray,  Delete_SerialArray(Rhs,Rhs_SerialArray,Temporary_Array_Set);)

void
Delete_PCE_SerialArray (
     const $4Array & parallelArray,
     $4SerialArray* serialArray,
     const Array_Conformability_Info_Type *Temporary_Array_Set )
   {
     APP_ASSERT(Temporary_Array_Set != NULL);
     if (Temporary_Array_Set->Full_VSG_Update_Required == FALSE || TRUE)
        {
       // Only delete the serial array data when the Overlap update model is used

       // printf ("In Macro Delete SerialArray (before decrement): $1_SerialArray->getReferenceCount() = %d \n",
       //      $1_SerialArray->getReferenceCount());

       // Handle case where PADRE is used
#if defined(USE_PADRE)
       // when using PADRE we have to clear the use of the SerialArray_Domain before
       // deleting the SerialArray object.
          parallelArray.setLocalDomainInPADRE_Descriptor(NULL);
#endif

       // Can't reference Return_Value in all functions
       // APP_ASSERT ($1_SerialArray != NULL);
       // APP_ASSERT (Return_Value.Array_Descriptor.SerialArray != NULL);
       // APP_ASSERT (Return_Value.Array_Descriptor.SerialArray->Array_ID() != $1_SerialArray->Array_ID());

       // Must delete the $1_SerialArray if it was taken directly from the $1 array!
       // Added conventional mechanism for reference counting control
       // operator delete no longer decriments the referenceCount.
          serialArray->decrementReferenceCount();
          if (serialArray->getReferenceCount() < intSerialArray::getReferenceCountBase())
             {
            // printf ("Deleting the serial array in Macro Delete SerialArray \n");
               delete serialArray;
             }
          serialArray = NULL;
        }
   }
*/




 

 





 

 





















 









































