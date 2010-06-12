#pragma once

#include "rose.h"

typedef bool SynthetizedAttribute;

class StatementInheritedAttribute
{
public:
	/** Lowest statement in the AST which is a parent of the current AST node.
	  * This is the location where we insert temporary variables. */
	SgStatement* tempVarInsertertLocation;

	/** This is set to the for-loop initializer statement if we're inside such
	  * a statement, else it is NULL. */
	SgForInitStatement* forInitializer;

	/** Parent for statment in the AST. Null if we're not in a for statement*/
	SgForStatement* forStatement;

	StatementInheritedAttribute(SgStatement* s) :
			tempVarInsertertLocation(s), forInitializer(NULL), forStatement(NULL)
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

private:

	/** Given the expression which is the argument to a function call, returns true if that
	  * expression should be pulled out into a temporary variable on a separate line.
	  * E.g. if the expression contains a function call, it needs to be normalized, while if it
	  * is a constant, there is no need to change it. */
	bool FunctionArgumentNeedsNormalization(SgExpression* argument);

	/** Returns true if any of the arguments of the given function call will need to
	  * be extracted. */
	bool FunctionCallNeedsNormalization(SgFunctionCallExp* functionCall);

	/** Returns true if any function calls in the given subtree will need to be
	  * instrumented. (to extract function arguments). */
	bool SubtreeNeedsNormalization(SgNode* top);

	/** Given an expression, generates a temporary variable whose initializer evaluates
	  * that expression.
	  * @param expression Expression which will be replaced by a variable
	  * @param scope scope in which the temporary variable will be generated
	  * @return declaration of the temporary variable. Its initializer evaluates the original expression. */
	static SgVariableDeclaration* CreateTempVariableForExpression(SgExpression* expression, SgScopeStatement* scope);

	/** Take a statement that is located somewhere inside the for loop and move it right before the
	  * for looop. If the statement is a variable declaration, the declaration is left in its original
	  * location to preserve its scope, and a new temporary variable is introduced. */
	void HoistStatementOutsideOfForLoop(SgForStatement* forLoop, SgStatement* statement);

	/** Generate a name that is unique in the current scope and any parent and children scopes.
	  * @param baseName the word to be included in the variable names. */
	static std::string GenerateUniqueVariableName(SgScopeStatement* scope, std::string baseName = "temp");
};
