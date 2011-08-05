#ifndef ROSE_ARRAY_ACCUMULATOR_ATTRIBUTE_H
#define ROSE_ARRAY_ACCUMULATOR_ATTRIBUTE_H

// typedef int ArrayAssignmentStatementQueryAccumulatorType;
// #include "operandDataBase.h"

class ArrayAssignmentStatementQueryAccumulatorType
{
public:
	// Store the operand data base here (one copy for the whole transformation)
	OperandDataBaseType operandDataBase;

	~ArrayAssignmentStatementQueryAccumulatorType();
	ArrayAssignmentStatementQueryAccumulatorType();

	ArrayAssignmentStatementQueryAccumulatorType (
			const ArrayAssignmentStatementQueryAccumulatorType & X );
	ArrayAssignmentStatementQueryAccumulatorType & operator= (
			const ArrayAssignmentStatementQueryAccumulatorType & X );
};

// endif for ROSE_ARRAY_ACCUMULATOR_ATTRIBUTE_H
#endif
