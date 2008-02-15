// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "arrayPreprocessor.h"

// #include "scalarIndexingStatementAccumulatorAttribute.h"


ScalarIndexingStatementQueryAccumulatorType::
~ScalarIndexingStatementQueryAccumulatorType()
   {
   }

ScalarIndexingStatementQueryAccumulatorType::
ScalarIndexingStatementQueryAccumulatorType()
   {
   }

ScalarIndexingStatementQueryAccumulatorType::
ScalarIndexingStatementQueryAccumulatorType ( const ScalarIndexingStatementQueryAccumulatorType & X )
   {
  // Deep copy semantics
     ((ScalarIndexingStatementQueryAccumulatorType*) this)->operator= (X);
   }

ScalarIndexingStatementQueryAccumulatorType & 
ScalarIndexingStatementQueryAccumulatorType::
operator= ( const ScalarIndexingStatementQueryAccumulatorType & X )
   {
     operandDataBase = X.operandDataBase;

     return *this;
   }

