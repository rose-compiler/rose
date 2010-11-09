#include "testExpressionBuilder.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH

void ExpressionBuilderPool::build()
{
	foreach (ExpressionBuilderPtr builder, exp_builders_)
	{
		builder->build();
		vector<SgExpression*> res = builder->getGeneratedExpressions();
		results_.insert(results_.end(), res.begin(), res.end());
	}
}

void UnaryExpressionBuilder::build()
{
	results_.clear();

	SgType* type = operand_->get_type();

	if (isScalarType(type))
	{
		if (operand_->isLValue())
		{
			results_.push_back(buildPlusPlusOp(copyExpression(operand_), SgUnaryOp::prefix));
			results_.push_back(buildPlusPlusOp(copyExpression(operand_), SgUnaryOp::postfix));
			if (!isSgTypeBool(type))
			{
				results_.push_back(buildMinusMinusOp(copyExpression(operand_), SgUnaryOp::prefix));
				results_.push_back(buildMinusMinusOp(copyExpression(operand_), SgUnaryOp::postfix));
			}
		}
#if 1
		results_.push_back(buildUnaryExpression<SgNotOp>(copyExpression(operand_)));
		results_.push_back(buildUnaryExpression<SgMinusOp>(copyExpression(operand_)));
		//results_.push_back(buildUnaryExpression<SgUnaryAddOp>(copyExpression(operand_)));
		if (isStrictIntegerType(type))
			results_.push_back(buildUnaryExpression<SgBitComplementOp>(copyExpression(operand_)));
#endif
	}
}

void BinaryExpressionBuilder::build()
{
	SgType* lhs_type = lhs_operand_->get_type();
	SgType* rhs_type = rhs_operand_->get_type();

	if (lhs_operand_->isLValue())
	{
		results_.push_back(buildBinaryExpression<SgAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgPlusAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgMinusAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		results_.push_back(buildBinaryExpression<SgMultAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));

		if (isStrictIntegerType(lhs_type) && isStrictIntegerType(rhs_type))
		{
			//results_.push_back(buildBinaryExpression<SgModAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
			results_.push_back(buildBinaryExpression<SgIorAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
			results_.push_back(buildBinaryExpression<SgAndAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
			results_.push_back(buildBinaryExpression<SgXorAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
			results_.push_back(buildBinaryExpression<SgLshiftAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
			results_.push_back(buildBinaryExpression<SgRshiftAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
		}
	}

	// temporarily remove DivAssign to avoid exceptions
	//results_.push_back(buildBinaryExpression<SgDivAssignOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));

	//results_.push_back(buildBinaryExpression<SgAddOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgAndOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgOrOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgCommaOpExp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));
	results_.push_back(buildBinaryExpression<SgEqualityOp>(copyExpression(lhs_operand_), copyExpression(rhs_operand_)));


}
