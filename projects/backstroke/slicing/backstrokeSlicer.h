#ifndef BACKSTROKESLICER_H
#define	BACKSTROKESLICER_H

#include "backstrokePDG.h"
#include "backstrokeCFGNode.h"
#include <utilities/utilities.h>

namespace Backstroke
{

struct CFGNodeFilterForSlicing
{
	//! Returns if an expression should be filtered off.
	bool filterExpression(SgExpression* expr) const;

	//! Returns if a CFG node should be filtered off.
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const;
};

typedef CFG<CFGNodeFilterForSlicing> CFGForSlicing;

class Slicer
{
	//! The target function definition.
	SgFunctionDefinition* funcDef_;
	
	//! All criteria.
	std::set<SgNode*> criteria_;

	//! The slice contains AST nodes inside.
	std::set<SgNode*> slice_;

	//! The possible slice contains AST nodes inside. "Possible" means this node contains a node in the slice.
	std::set<SgNode*> possibleSlice_;

	//typedef Backstroke::FilteredCFG CFGForSlicing;

	typedef PDG<CFGForSlicing> PDGForSlicing;

	typedef std::vector<SgInitializedName*> VarName;

public:
	Slicer(SgFunctionDefinition* funcDef)
	: funcDef_(funcDef) {}

	void addCriterion(SgNode* node)
	{
		if (SgExpression* expr = isSgExpression(node))
		{
			if (isVariable(expr))
				criteria_.insert(expr);
		}
		/*
		//if (!isSgVarRefExp(node) && !isSgArrow)
		std::vector<SgExpression*> vars = BackstrokeUtility::getAllVariables(node);
		criterion_.insert(vars.begin(), vars.end());
		criterion_.insert(node);
		*/
	}

	SgFunctionDeclaration* slice();


	//! Returns if a AST node is a variable.
	static bool isVariable(SgExpression* expr);

	//! Returns the variable name for the given node.
	static VarName getVarName(SgNode* node)
	{ return VariableRenaming::getVarName(node); }

	//! Get all direct defs in the given node. Note that for the expression "a = ++b", only a is returned.
	static std::set<SgNode*> getDirectDefs(SgNode* node);

	//! Get all direct uses in the given node. Note that for the expression "a = (b += c)", only b is returned.
	//! The second parameter indicates if a direct def is considered as a use. It's used for recursively finding
	//! uses. For example, "a = (b = c)" has a direct use of "b". In the first call of this function, "useDef" should be
	//! set to false, so "a" is not a direct use. When this function is called recursively on the expression "b = c",
	//! "useDef" is set to true, then "b" is a direct use and "c" is not.
	static std::set<SgExpression*> getDirectUses(SgNode* node, bool useDef = false);

	//! Get the lvalue expression in the given node. For example, the lvalue for the expression (a, ++b) is b.
	static SgExpression* getLValue(SgExpression* expr);

private:
	//! Build the function only containing the slice.
	SgFunctionDeclaration* buildSlicedFunction();

	//! Copy a statement according to the slice.
	SgStatement* copySlicedStatement(SgStatement* stmt) const;

	//! Copy an expression according to the slice.
	SgExpression* copySlicedExpression(SgExpression* expr) const;

};

} // End of namespace Backstroke

#endif	/* BACKSTROKESLICER_H */

