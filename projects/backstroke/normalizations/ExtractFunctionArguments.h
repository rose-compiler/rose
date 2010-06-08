#pragma once

#include "rose.h"

typedef bool SynthetizedAttribute;

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

class ExtractFunctionArguments : public AstTopDownBottomUpProcessing<StatementInheritedAttribute, SynthetizedAttribute>
{
public:

    /** Perform the function argument extraction on all function calls in the given subtree of the AST. */
	void NormalizeTree(SgNode* tree);

	/** Update the scope based on the parent scope. This is a pre-order traversal. */
	StatementInheritedAttribute evaluateInheritedAttribute(SgNode* astNode, StatementInheritedAttribute parentValue);

	/** Perform the actual instrumentatation to extract the function arguments. This is a post-order traversal. */
	SynthetizedAttribute evaluateSynthesizedAttribute(SgNode* astNode, StatementInheritedAttribute parentValue, SynthesizedAttributesList);
};
