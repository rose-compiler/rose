#ifndef ROSE_SCALAR_INDEXING_ACCUMULATOR_ATTRIBUTE_H
#define ROSE_SCALAR_INDEXING_ACCUMULATOR_ATTRIBUTE_H

// typedef int ArrayAssignmentStatementQueryAccumulatorType;

// include "scalarDataBase.h"
// #include "operandDataBase.h"

class ScalarIndexingStatementQueryAccumulatorType
   {
     public:
       // Store the operand data base here (one copy for the whole transformation)
       // ScalarIndexingOperandDataBaseType operandDataBase;
          OperandDataBaseType operandDataBase;

         ~ScalarIndexingStatementQueryAccumulatorType();
          ScalarIndexingStatementQueryAccumulatorType();

          ScalarIndexingStatementQueryAccumulatorType ( const ScalarIndexingStatementQueryAccumulatorType & X );
          ScalarIndexingStatementQueryAccumulatorType & operator= ( const ScalarIndexingStatementQueryAccumulatorType & X );
   };

// endif for ROSE_SCALAR_INDEXING_ACCUMULATOR_ATTRIBUTE_H
#endif
