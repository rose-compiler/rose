#pragma once

#include "rose.h"

class StatementInheritedAttribute
{
public:
	/** Lowest statement in the AST which is a parent of the current AST node. */
	SgStatement* statement;

	StatementInheritedAttribute(SgStatement* s) : statement(s)
	{
	}

	StatementInheritedAttribute(const StatementInheritedAttribute& other) : statement(other.statement)
	{
	}
};

class ExtractFunctionArguments : public AstTopDownProcessing<StatementInheritedAttribute>
{
public:

    /** Perform the function argument extraction on all function calls in the given subtree of the AST. */
	void NormalizeTree(SgNode* tree);

	/** Update the scope based on the parent scope. */
	StatementInheritedAttribute evaluateInheritedAttribute(SgNode* astNode, StatementInheritedAttribute parentValue);
};
