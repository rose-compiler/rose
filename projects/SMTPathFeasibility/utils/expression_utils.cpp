#include "expression_utils.h" 

void fixLongExp(SgExpression* se) {
	SageInterface::splitExpressionIntoBasicBlock(se);
	return;
}
