#ifndef BACKSTROKESLICER_H
#define	BACKSTROKESLICER_H

#include "backstrokePDG.h"
#include "backstrokeCFGNode.h"
#include <utilities/utilities.h>

namespace Backstroke
{

struct CFGNodeFilterForSlicing
{
	// This function will be moved to a source file.
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{
		if (!cfgNode.isInteresting())
			return false;

		SgNode* node = cfgNode.getNode();

		if (isSgValueExp(node))
			return false;
		if (isSgExpression(node))
		{
			if (isSgExprStatement(node->get_parent()))
				return false;
			// If this expression has no side effect, filter it off.
			if (!SageInterface::isAssignmentStatement(node) && !isSgFunctionCallExp(node))
				return false;
		}
		if (isSgScopeStatement(node) && !isSgFunctionDefinition(node))
			return false;
		if (isSgCommaOpExp(node->get_parent()) && !isSgCommaOpExp(node))
			return true;

		switch (node->variantT())
		{
			case V_SgVarRefExp:
			{
				SgNode* parent = node->get_parent()->get_parent();
				if (isSgWhileStmt(parent) || 
						isSgForStatement(parent) ||
						isSgIfStmt(parent) ||
						isSgSwitchStatement(parent) ||
						isSgDoWhileStmt(parent))
					return true;
				else
					return false;
			}
			case V_SgInitializedName:
			case V_SgFunctionParameterList:
			case V_SgAssignInitializer:
			case V_SgFunctionRefExp:
			//case V_SgPntrArrRefExp:
			case V_SgExprListExp:
			case V_SgCastExp:
			//case V_SgForInitStatement:
			case V_SgCommaOpExp:
				return false;
			default:
				break;
		}

		return true;
	}
};

typedef CFG<VirtualCFG::FilteredCFGNode<CFGNodeFilterForSlicing>,
			VirtualCFG::FilteredCFGEdge<CFGNodeFilterForSlicing> > CFGForSlicing;

class Slicer
{
	//! The target function definition.
	SgFunctionDefinition* funcDef_;
	
	//! All criterion.
	std::set<SgNode*> criterion_;

	//! All slices.
	std::set<SgNode*> slices_;

	//typedef Backstroke::FilteredCFG CFGForSlicing;

	typedef PDG<CFGForSlicing> PDGForSlicing;

public:
	Slicer(SgFunctionDefinition* funcDef)
	: funcDef_(funcDef) {}

	void addCriterion(SgNode* node)
	{
		std::vector<SgExpression*> vars = BackstrokeUtility::getAllVariables(node);
		criterion_.insert(vars.begin(), vars.end());
		criterion_.insert(node);
	}

	SgFunctionDeclaration* slice();

private:
	SgFunctionDeclaration* buildSlicedFunction();
	SgStatement* copySlicedStatement(SgStatement* stmt, SgScopeStatement* scope = NULL);
	SgExpression* copySlicedExpression(SgExpression* expr);

};

} // End of namespace Backstroke

#endif	/* BACKSTROKESLICER_H */

