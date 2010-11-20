#ifndef TESTSTATEMENTBUILDER_H
#define	TESTSTATEMENTBUILDER_H

#include "builderTypes.h"

class ExpressionStatementBuilder : public StatementBuilder
{
protected:
	SgExpression* exp_;

public:
	virtual void build();
};

class IfStatementBuilder
{
};

class ForStatementBuilder
{
};

class WhileStatementBuilder
{
};

class DoWhileStatementBuilder
{
};

class SwitchStatementBuilder
{
};


#endif	/* TESTSTATEMENTBUILDER_H */

