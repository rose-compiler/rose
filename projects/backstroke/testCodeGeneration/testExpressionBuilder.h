#ifndef TESTEXPRESSIONBUILDER_H
#define	TESTEXPRESSIONBUILDER_H

#include "builderTypes.h"

//! This class is used to build expressions from other expression builders.
class ExpressionBuilderPool : public ExpressionBuilder
{
	std::vector<ExpressionBuilderPtr> exp_builders_;

public:
	void addExpressionBuilder(ExpressionBuilder* builder)
	{ addExpressionBuilder(ExpressionBuilderPtr(builder)); }

	void addExpressionBuilder(ExpressionBuilderPtr builder)
	{ exp_builders_.push_back(builder); }

	virtual void build();
};

class UnaryExpressionBuilder : public ExpressionBuilder
{
protected:
	SgExpression* operand_;

public:

	UnaryExpressionBuilder(SgExpression* operand)
	: operand_(operand)
	{
	}

	virtual void build();
};

class BinaryExpressionBuilder : public ExpressionBuilder
{
protected:
	SgExpression* lhs_operand_;
	SgExpression* rhs_operand_;

public:

	BinaryExpressionBuilder(SgExpression* lhs, SgExpression* rhs)
	: lhs_operand_(lhs), rhs_operand_(rhs)
	{
	}

	virtual void build();
};

class ConditionalExpressionBuilder : public ExpressionBuilder
{
};

class FunctionCallExpressionBuilder : public ExpressionBuilder
{
};

#endif	/* TESTEXPRESSIONBUILDER_H */

