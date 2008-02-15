// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "arrayPreprocessor.h"

// include "arrayAssignmentStatementSynthesizedAttribute.h"
// #include "arrayAssignmentStatementAccumulatorAttribute.h"


ArrayAssignmentStatementQueryAccumulatorType::
~ArrayAssignmentStatementQueryAccumulatorType()
   {
     printf ("Calling destructor for ArrayAssignmentStatementQueryAccumulatorType class (no longer exiting ...) \n");
  // ROSE_ABORT();
   }

ArrayAssignmentStatementQueryAccumulatorType::
ArrayAssignmentStatementQueryAccumulatorType()
   {
     printf ("Calling constructor for ArrayAssignmentStatementQueryAccumulatorType class \n");
   }

ArrayAssignmentStatementQueryAccumulatorType::
ArrayAssignmentStatementQueryAccumulatorType ( const ArrayAssignmentStatementQueryAccumulatorType & X )
   {
  // Deep copy semantics
     ((ArrayAssignmentStatementQueryAccumulatorType*) this)->operator= (X);
   }

ArrayAssignmentStatementQueryAccumulatorType & 
ArrayAssignmentStatementQueryAccumulatorType::
operator= ( const ArrayAssignmentStatementQueryAccumulatorType & X )
   {
     operandDataBase = X.operandDataBase;

     return *this;
   }

