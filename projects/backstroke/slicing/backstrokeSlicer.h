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

typedef CFG<VirtualCFG::FilteredCFGNode<CFGNodeFilterForSlicing>,
			VirtualCFG::FilteredCFGEdge<CFGNodeFilterForSlicing> > CFGForSlicing;

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
		if (isVariable(node))
			criteria_.insert(node);
		/*
		//if (!isSgVarRefExp(node) && !isSgArrow)
		std::vector<SgExpression*> vars = BackstrokeUtility::getAllVariables(node);
		criterion_.insert(vars.begin(), vars.end());
		criterion_.insert(node);
		*/
	}

	SgFunctionDeclaration* slice();

private:
	//! Build the function only containing the slice.
	SgFunctionDeclaration* buildSlicedFunction();

	//! Copy a statement according to the slice.
	SgStatement* copySlicedStatement(SgStatement* stmt) const;

	//! Copy an expression according to the slice.
	SgExpression* copySlicedExpression(SgExpression* expr) const;

	//! Returns if a AST node is a variable.
	bool isVariable(SgNode* node) const;

	//! Returns the variable name for the given node.
	VarName getVarName(SgNode* node) const
	{ return VariableRenaming::getVarName(node); }

	//! Get all direct defs in the given node. Note that for the expression a = ++b, only a is returned.
	std::set<VarName> getDef(SgNode* node) const;

};

} // End of namespace Backstroke

#endif	/* BACKSTROKESLICER_H */

