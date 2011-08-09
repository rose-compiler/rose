#ifndef ROSE_ARRAY_ASSIGNMENT_STATEMENT_TRANSFORMATION_H
#define ROSE_ARRAY_ASSIGNMENT_STATEMENT_TRANSFORMATION_H

#include "arrayAssignmentStatementInheritedAttribute.h"
#include "arrayAssignmentStatementSynthesizedAttribute.h"
#include "arrayAssignmentStatementAccumulatorAttribute.h"

class ArrayAssignmentStatementTransformation
//: public SgBottomUpProcessing<ArrayAssignmentStatementQuerySynthesizedAttributeType>
: public SgTopDownBottomUpProcessing<ArrayAssignmentStatementQueryInheritedAttributeType,
ArrayAssignmentStatementQuerySynthesizedAttributeType>
{
public:
	// Example of support for accumulator attributes
	ArrayAssignmentStatementQueryAccumulatorType & accumulatorValue;

	// Access to the operand database (stores variable names and index names used with each variable)
	// OperandDataBaseType operandDataBase;

	~ArrayAssignmentStatementTransformation();

	// Note that the rewrite mechanism requires access to the input
	// command line which it gets from the SgProject object.
	// Uncommented 1 line below
	//ArrayAssignmentStatementTransformation( SgProject & project );
	ArrayAssignmentStatementTransformation();

	OperandDataBaseType & getOperandDataBase();

	// If this is static then it must take the SgProject pointer as an input parameter
	// We should decide upon a policy here:
	//      1) the SgProject could be specified in the constructor and then the
	//         transformation function would have to be non-static; OR
	//      2) The transformation function is static and the project is handed in
	//         as a parameter (better I think, since it is simpler for the user).
	//   static SynthesizedAttributeBaseClassType transformation
	static ArrayAssignmentStatementQuerySynthesizedAttributeType transformation
	( const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode );

	// Should return type be ArrayAssignmentStatementQuerySynthesizedAttributeType?
	friend ArrayAssignmentStatementQuerySynthesizedAttributeType expressionStatementTransformation
	( SgExprStatement* astNode,
			const ArrayAssignmentStatementQueryInheritedAttributeType & inheritedAttribute,
			const ArrayAssignmentStatementQuerySynthesizedAttributeType & innerLoopTransformation,
			const ArrayAssignmentStatementQueryAccumulatorType & accumulatedValue );

	void processArrayRefExp(SgVarRefExp* varRefExp,
			int dimension) ;

	SgExprStatement* createSecondLoop(SgExprStatement* exprStatement,
			ArrayAssignmentStatementQueryInheritedAttributeType & arrayAssignmentStatementQueryInheritedData,
			OperandDataBaseType & operandDataBase);

	ArrayAssignmentStatementQuerySynthesizedAttributeType expressionStatementTransformation(SgExprStatement* astNode,
			ArrayAssignmentStatementQueryInheritedAttributeType & arrayAssignmentStatementQueryInheritedData,
			const ArrayAssignmentStatementQuerySynthesizedAttributeType & innerLoopTransformation,
			OperandDataBaseType & operandDataBase);

	// Functions required by the global tree traversal mechanism
	ArrayAssignmentStatementQueryInheritedAttributeType evaluateInheritedAttribute (
			SgNode* astNode,
			ArrayAssignmentStatementQueryInheritedAttributeType inheritedValue );


	ArrayAssignmentStatementQuerySynthesizedAttributeType evaluateSynthesizedAttribute (
			SgNode* astNode,
			ArrayAssignmentStatementQueryInheritedAttributeType inheritedValue,
			SubTreeSynthesizedAttributes attributList );

	virtual void atTraversalStart();
	virtual void atTraversalEnd();

	static bool targetForTransformation( SgNode* astNode );

	static void createVariableDeclarations( SgExprStatement* exprStatement, int dimension );

private:
	vector<SgExpression*> nodeList;
	vector<int> dimensionList;
	// Make this inacessable within the interface (by defining it but making it private)
//        ArrayAssignmentStatementTransformation();

};

// endif for ROSE_ARRAY_ASSIGNMENT_STATEMENT_TRANSFORMATION_H
#endif

