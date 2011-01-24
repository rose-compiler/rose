#ifndef BACKSTROKECFGNODE_H
#define	BACKSTROKECFGNODE_H

#include <rose.h>
#include <backstrokeCFG.h>


namespace Backstroke
{


struct CFGNodeFilter
{
	// This function will be moved to a source file.
	bool operator()(const VirtualCFG::CFGNode& cfgNode) const
	{
		if (!cfgNode.isInteresting())
			return false;

		SgNode* node = cfgNode.getNode();

		if (isSgValueExp(node))
			return false;
		//if (isSgExpression(node) && isSgExprStatement(node->get_parent()))
		if (isSgExprStatement(node))
			return false;
		if (isSgScopeStatement(node) && !isSgFunctionDefinition(node))
			return false;
		if (isSgCommaOpExp(node->get_parent()) && !isSgCommaOpExp(node))
			return true;

		switch (node->variantT())
		{
			case V_SgVarRefExp:
			case V_SgInitializedName:
			case V_SgFunctionParameterList:
			case V_SgAssignInitializer:
			case V_SgFunctionRefExp:
			case V_SgPntrArrRefExp:
			case V_SgExprListExp:
			case V_SgCastExp:
			case V_SgForInitStatement:
			case V_SgCommaOpExp:
				return false;
			default:
				break;
		}

		return true;
	}
};

typedef CFG<CFGNodeFilter> CFGForSSA;



} // End of namespace Backstroke


#endif	/* BACKSTROKECFGNODE_H */

