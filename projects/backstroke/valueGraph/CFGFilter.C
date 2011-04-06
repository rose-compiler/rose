#include "CFGFilter.h"

namespace Backstroke
{

using namespace std;

bool BackstrokeCFGNodeFilter::filterExpression(SgExpression* expr) const
{
	// If this expression has no side effect, and is not used as the condition of if,
	// selector of switch, etc., filter it off.

	// The comma expression will be broken then filtered.
	if (isSgCommaOpExp(expr))
		return false;

    // Logical and and or bring short circuit. Remove them from the CFG, and just show
    // their operands.
    if (isSgAndOp(expr) || isSgOrOp(expr))
        return false;

	// Find the right most expression in nested comma expressions.
	SgExpression* expr2 = expr;
	while (SgCommaOpExp* commaExpr = isSgCommaOpExp(expr2->get_parent()))
	{
		if (commaExpr->get_rhs_operand() == expr2)
			expr2 = commaExpr;
		else
			break;
	}

	SgNode* parent = expr2->get_parent()->get_parent();
	if (SgIfStmt* ifStmt = isSgIfStmt(parent))
	{
		if (expr2->get_parent() == ifStmt->get_conditional())
			return true;
	}
	else if(SgWhileStmt* whileStmt = isSgWhileStmt(parent))
	{
		if (expr2->get_parent() == whileStmt->get_condition())
			return true;
	}
	else if (SgDoWhileStmt* doWhileStmt = isSgDoWhileStmt(parent))
	{
		if (expr2->get_parent() == doWhileStmt->get_condition())
			return true;
	}
	else if (SgForStatement* forStmt = isSgForStatement(parent))
	{
		if (expr2->get_parent() == forStmt->get_test())
			return true;
	}
	else if (SgSwitchStatement* switchStmt = isSgSwitchStatement(parent))
	{
		if (expr2->get_parent() == switchStmt->get_item_selector())
			return true;
	}

	if (SageInterface::isAssignmentStatement(expr) ||
			isSgPlusPlusOp(expr) ||
			isSgMinusMinusOp(expr) ||
			isSgFunctionCallExp(expr) ||
            isSgGreaterThanOp(expr) ||
            isSgGreaterOrEqualOp(expr) ||
            isSgLessThanOp(expr) ||
            isSgLessOrEqualOp(expr) ||
            isSgEqualityOp(expr) ||
            isSgNotEqualOp(expr))
		return true;

	return false;
}

bool BackstrokeCFGNodeFilter::operator()(const VirtualCFG::CFGNode& cfgNode) const
{
	if (!cfgNode.isInteresting())
		return false;

	SgNode* node = cfgNode.getNode();

	if (SgExpression* expr = isSgExpression(node))
		return filterExpression(expr);

	if (isSgScopeStatement(node) && !isSgFunctionDefinition(node))
		return false;
	//if (isSgCommaOpExp(node->get_parent()) && !isSgCommaOpExp(node))
	//	return true;

	switch (node->variantT())
	{
		case V_SgInitializedName:
		case V_SgFunctionParameterList:
		case V_SgAssignInitializer:
		case V_SgCaseOptionStmt:
		case V_SgDefaultOptionStmt:
		//case V_SgFunctionRefExp:
		//case V_SgPntrArrRefExp:
		//case V_SgExprListExp:
		//case V_SgCastExp:
		//case V_SgForInitStatement:
		//case V_SgCommaOpExp:
		case V_SgExprStatement:
		case V_SgForInitStatement:
		case V_SgBreakStmt:
		case V_SgContinueStmt:
			return false;
		default:
			break;
	}

	return true;
}


} // end of Backstroke
