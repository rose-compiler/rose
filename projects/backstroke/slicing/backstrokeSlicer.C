#include "backstrokeSlicer.h"
#include <boost/tuple/tuple.hpp>


#define foreach BOOST_FOREACH

namespace Backstroke
{

using namespace std;

bool CFGNodeFilterForSlicing::filterExpression(SgExpression* expr) const
{
	// If this expression has no side effect, and is not used as the condition of if,
	// selector of switch, etc., filter it off.

	// The comma expression will be broken then filtered.
	if (isSgCommaOpExp(expr))
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
			isSgFunctionCallExp(expr))
		return true;

	return false;
}

bool CFGNodeFilterForSlicing::operator()(const VirtualCFG::CFGNode& cfgNode) const
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

bool Slicer::isVariable(SgExpression* expr)
{
	return BackstrokeUtility::isVariableReference(expr);
#if 0
	if (isSgVarRefExp(expr))
		return true;
	if (isSgDotExp(expr) || isSgArrowExp(expr))
	{
		SgBinaryOp* binOp = isSgBinaryOp(expr);
		if (//isVariable(binOp->get_lhs_operand()) &&
			isVariable(binOp->get_rhs_operand()))
			return true;
	}
	if (isSgPntrArrRefExp(expr))
		return true;
	
	return false;
#endif
}

set<SgNode*> Slicer::getDirectDefs(SgNode* node)
{
	set<SgNode*> defs;

	if (SgVariableDeclaration* varDecl = isSgVariableDeclaration(node))
	{
		const SgInitializedNamePtrList& initNames = varDecl->get_variables();
		foreach (SgInitializedName* initName, initNames)
			defs.insert(initName);
		return defs;
	}

	if (SageInterface::isAssignmentStatement(node))
	{
		SgExpression* lhs = isSgBinaryOp(node)->get_lhs_operand();
		SgExpression* def = getLValue(lhs);
		defs.insert(def);
		return defs;
	}

	if (isSgPlusPlusOp(node) || isSgMinusMinusOp(node))
	{
		SgExpression* operand = isSgUnaryOp(node)->get_operand();
		SgExpression* def = getLValue(operand);
		defs.insert(def);
		return defs;
	}

	if (SgFunctionCallExp* funcCall = isSgFunctionCallExp(node))
	{
		// TODO  This part should be refined later.
		foreach (SgExpression* expr, funcCall->get_args()->get_expressions())
		{
			if (expr->isLValue())
			{
				SgExpression* def = getLValue(expr);
				defs.insert(def);
			}
		}
		return defs;
	}

	return defs;
}

set<SgExpression*> Slicer::getDirectUses(SgNode* node, bool useDef)
{
	set<SgExpression*> uses;

	SgExpression* expr = isSgExpression(node);
	if (expr == NULL)
	{
		// Variable declaration case.
		return uses;
	}

	ROSE_ASSERT(expr);
	
	// Put this check ahead of others.
	if (isVariable(expr))
	{
		uses.insert(expr);
		return uses;
	}

	if (isSgVariableDeclaration(node))
	{
		return uses;
	}

	if (SgBinaryOp* binOp = isSgBinaryOp(node))
	{
		SgExpression* lhs = binOp->get_lhs_operand();
		SgExpression* rhs = binOp->get_rhs_operand();

		if (SageInterface::isAssignmentStatement(node))
		{
			// TODO
			// FIXME Currently we don't consider the array case!!!
			// So there is no use on the lhs of assignment.
			if (isSgPntrArrRefExp(lhs))
				ROSE_ASSERT(!"Currently we don't consider the array case!!!");

			if (!useDef)
			{
				if (isSgAssignStatement(node))
				{
					return getDirectUses(rhs, true);
				}
				else
				{
					set<SgExpression*> lhsUses = getDirectUses(lhs, true);
					set<SgExpression*> rhsUses = getDirectUses(rhs, true);
					lhsUses.insert(rhsUses.begin(), rhsUses.end());
					return lhsUses;
				}
			}
			else
			{
				set<SgNode*> defs = getDirectDefs(binOp);
				foreach (SgNode* n, defs)
				{
					ROSE_ASSERT(isSgExpression(n));
					uses.insert(isSgExpression(n));
				}
				return uses;
			}
		}
		else
		{
			set<SgExpression*> lhsUses = getDirectUses(lhs, true);
			set<SgExpression*> rhsUses = getDirectUses(rhs, true);
			lhsUses.insert(rhsUses.begin(), rhsUses.end());
			return lhsUses;
		}
	}

	if (isSgPlusPlusOp(node) || isSgMinusMinusOp(node))
	{
		set<SgNode*> defs = getDirectDefs(node);
		foreach (SgNode* n, defs)
		{
			ROSE_ASSERT(isSgExpression(n));
			uses.insert(isSgExpression(n));
		}
		return uses;
	}

#if 0
	if (SgFunctionCallExp* funcCall = isSgFunctionCallExp(node))
	{
		// TODO  This part should be refined later.
		foreach (SgExpression* expr, funcCall->get_args()->get_expressions())
		{
			if (expr->isLValue())
			{
				SgExpression* def = getLValue(expr);
				uses.insert(getVarName(def));
			}
		}
		return uses;
	}
#endif

	return uses;
}

SgExpression* Slicer::getLValue(SgExpression* expr)
{
	if (isVariable(expr))
		return expr;
	
	// A function call may be a lvalue.
	if (isSgFunctionCallExp(expr) && expr->isLValue())
		return expr;

	if (SgCommaOpExp* commaExp = isSgCommaOpExp(expr))
		return getLValue(commaExp->get_rhs_operand());

	if (SageInterface::isAssignmentStatement(expr))
		return getLValue(isSgBinaryOp(expr)->get_lhs_operand());

	if (isSgPlusPlusOp(expr) || isSgMinusMinusOp(expr))
		return getLValue(isSgUnaryOp(expr)->get_operand());

	return NULL;
}

SgFunctionDeclaration* Slicer::slice()
{
	// First, build a PDG for the given function.
	PDGForSlicing pdg(funcDef_);

	// Second, find the corresponding nodes of criteria in the PDG.

	// A work list holds PDG node and the corresponding variable name pairs in the slice.
	stack<pair<PDGForSlicing::Vertex, VarName> > workList;

	boost::graph_traits<PDGForSlicing>::vertex_iterator first, last;

	for (boost::tie(first, last) = boost::vertices(pdg); first != last; ++first)
	{
		foreach(SgNode* node, criteria_)
		{
			// If this PDG node contains the criterion variable, add it to the worklist.
			if (pdg[*first]->getNode() == node ||
					SageInterface::isAncestor(pdg[*first]->getNode(), node))
			{
				workList.push(make_pair(*first, getVarName(node)));
				break;
			}
		}
	}

	set<PDGForSlicing::Vertex> verticesSlice;

	// Then start the backward slicing algorithm.
	while(!workList.empty())
	{
		PDGForSlicing::Vertex v;
		VarName varName;
		boost::tie(v, varName) = workList.top();
		workList.pop();

		verticesSlice.insert(v);

		boost::graph_traits<PDGForSlicing>::out_edge_iterator i, j;
		for (boost::tie(i, j) = boost::out_edges(v, pdg); i != j; ++i)
		{
			PDGForSlicing::Vertex tar = boost::target(*i, pdg);
			SgNode* node = pdg[tar]->getNode();

			// If the target of the edge is already in the slice, continue.
			if (verticesSlice.find(tar) != verticesSlice.end())
				continue;

			// The data dependence should have the same variable.
			if (pdg[*i].type == PDGEdge::DataDependence)
			{
				if (pdg[*i].ddEdge.varNames.count(varName) == 0)
					continue;

				// If the target node does not define varName directly, we won't add it to the worklist.
				// For example, a = ++b + c defines a directly, but not b. If b is data dependent on this
				// expression, we won't add c to the worklist.
				set<SgNode*> defs = getDirectDefs(node);
				bool flag = false;
				foreach (SgNode* def, defs)
				{
					if (getVarName(def) == varName)
					{
						flag = true;
						break;
					}
				}
				if (!flag)
					continue;

				// Note at this point, the node is a defining node.

				// Find all variables used in the target node.
				set<SgExpression*> vars = getDirectUses(node);
				foreach (SgExpression* var, vars)
				{
					// Add this variable into slice.
					slice_.insert(var);
					workList.push(make_pair(tar, getVarName(var)));
				}
				// In case the target node is a declaration.
				workList.push(make_pair(tar, varName));
			}			
			else
			{
				// Control dependence case.
				
				// Find all variables used in the target node.
				set<SgExpression*> vars = getDirectUses(node);
				foreach (SgExpression* var, vars)
				{
					// Add this variable into slice.
					slice_.insert(var);
					workList.push(make_pair(tar, getVarName(var)));
				}
			}
		}
	}

	cout << "Slice number:" << verticesSlice.size() << endl;

	foreach (PDGForSlicing::Vertex v, verticesSlice)
	{
		SgNode* node = pdg[v]->getNode();

		slice_.insert(node);

		// Add all parents and children to possible slice to make it easy to
		// check if a statement should be in the slice or not.
		while (node && !isSgFunctionDefinition(node))
		{
			possibleSlice_.insert(node);
			node = node->get_parent();
		}

		//vector<SgNode*> nodes = SageInterface::querySubTree<SgNode>(node, V_SgNode);
		//foreach (SgNode* n, nodes)
		//	possibleSlice_.insert(n);
	}

	cout << "Slice number:" << slice_.size() << endl;

	// The last step is building a function with only slice.
	return buildSlicedFunction();

}

SgFunctionDeclaration* Slicer::buildSlicedFunction()
{
	SgFunctionDeclaration* funcDecl = funcDef_->get_declaration();

	SgFunctionParameterList* paraList = SageBuilder::buildFunctionParameterList();
	foreach (SgInitializedName* initName, funcDecl->get_parameterList()->get_args())
	{
		paraList->append_arg(SageBuilder::buildInitializedName(initName->get_name(), initName->get_type()));
	}

	string funcName = funcDecl->get_name().str() + string("_sliced");
	SgFunctionDeclaration* declSliced =
			SageBuilder::buildDefiningFunctionDeclaration(
				funcName, funcDecl->get_orig_return_type(),
				paraList, SageInterface::getScope(funcDecl));
	SgFunctionDefinition* funcDefSliced = declSliced->get_definition();

	foreach (SgInitializedName* initName, declSliced->get_parameterList()->get_args())
	{
		initName->set_scope(funcDefSliced);
	}

	//SageInterface::replaceStatement(funcDefSliced->get_body(),
	//		isSgBasicBlock(SageInterface::copyStatement(funcDef_->get_body())));
	SageInterface::replaceStatement(funcDefSliced->get_body(),
			isSgBasicBlock(copySlicedStatement(funcDef_->get_body())));

	return declSliced;
}

SgStatement* Slicer::copySlicedStatement(SgStatement* stmt) const
{
	if (stmt == NULL)
		return NULL;
	
	if (possibleSlice_.find(stmt) == possibleSlice_.end())
		return NULL;
	
	switch (stmt->variantT())
	{
		case V_SgBasicBlock:
		{
			// Build a basic block no matter if it contains any slice.
			SgBasicBlock* basicBlock = isSgBasicBlock(stmt);
			SgBasicBlock* newBasicBlock = SageBuilder::buildBasicBlock();
			foreach (SgStatement* s, basicBlock->get_statements())
			{
				SgStatement* newStmt = copySlicedStatement(s);
				if (newStmt)
					newBasicBlock->append_statement(newStmt);
			}
			return newBasicBlock;
		}
			
		case V_SgExprStatement:
		{
			SgExprStatement* exprStmt = isSgExprStatement(stmt);
			SgExpression* newExpr = copySlicedExpression(exprStmt->get_expression());
			return SageBuilder::buildExprStatement(newExpr);
		}

		case V_SgVariableDeclaration:
		{
			return SageInterface::copyStatement(stmt);
		}

		case V_SgIfStmt:
		{
			SgIfStmt* ifStmt = isSgIfStmt(stmt);
			//if (slice_.find(ifStmt->get_conditional()) != slice_.end())
			{
				SgIfStmt* newIfStmt = SageBuilder::buildIfStmt(
						copySlicedStatement(ifStmt->get_conditional()),
						copySlicedStatement(ifStmt->get_true_body()),
						copySlicedStatement(ifStmt->get_false_body()));
				return newIfStmt;
			}
			break;
		}

		case V_SgForStatement:
		{
			SgForStatement* forStmt = isSgForStatement(stmt);
			//if (slice_.find(forStmt->get_test()) != slice_.end() ||
			//		slice_.find(forStmt->get_for_init_stmt()) != slice_.end())
			{
				SgStatement* newInitStmt = copySlicedStatement(forStmt->get_for_init_stmt());
				SgStatement* newTestStmt = copySlicedStatement(forStmt->get_test());

				// If newTestStmt is NULL, we will not copy the whole for statement, but only the initializing
				// statement. A case is like for(int i = ++a; i < N; ++i){}, in which we only add ++a to the slice.
				if (newTestStmt == NULL)
					return newInitStmt;

				SgExpression* newIncrExpr = copySlicedExpression(forStmt->get_increment());

				if (newIncrExpr == NULL)
					newIncrExpr = SageBuilder::buildNullExpression();
				
				ROSE_ASSERT(newInitStmt);
				ROSE_ASSERT(newTestStmt);
				ROSE_ASSERT(newIncrExpr);

				SgForStatement* newForStmt = SageBuilder::buildForStatement(
						newInitStmt, newTestStmt, newIncrExpr,
						copySlicedStatement(forStmt->get_loop_body()));
				return newForStmt;
			}
			break;
		}

		case V_SgForInitStatement:
		{
			SgForInitStatement* forInitStmt = isSgForInitStatement(stmt);
			const SgStatementPtrList& stmts = forInitStmt->get_init_stmt();
			if (stmts.size() == 1 && isSgExprStatement(stmts[0]))
				return copySlicedStatement(stmts[0]);

			// Since there is no buildForInitStatement function in SageBuilder, we first copy the statement
			// then remove and delete all nodes inside, then add sliced statements inside.
			SgForInitStatement* newForInitStmt = isSgForInitStatement(SageInterface::copyStatement(stmt));
			foreach (SgStatement* s, newForInitStmt->get_init_stmt())
				SageInterface::deepDelete(s);
			newForInitStmt->get_init_stmt().clear();

			foreach (SgStatement* s, stmts)
			{
				SgStatement* newStmt = copySlicedStatement(s);
				if (newStmt)
					newForInitStmt->append_init_stmt(newStmt);
			}
			return newForInitStmt;

			//SgStatement* newTestStmt = copySlicedStatement(forStmt->get_test());
			//return SageInterface::copyStatement(stmt);
		}

		case V_SgWhileStmt:
		{
			SgWhileStmt* whileStmt = isSgWhileStmt(stmt);
			//if (slice_.find(whileStmt->get_condition()) != slice_.end())
			{
				SgWhileStmt* newWhileStmt = SageBuilder::buildWhileStmt(
						copySlicedStatement(whileStmt->get_condition()),
						copySlicedStatement(whileStmt->get_body()));
				return newWhileStmt;
			}
			break;
		}

		case V_SgSwitchStatement:
		{
			SgSwitchStatement* switchStmt = isSgSwitchStatement(stmt);
			//if (slice_.find(switchStmt->get_item_selector()) != slice_.end())
			{
				return SageBuilder::buildSwitchStatement(
						copySlicedStatement(switchStmt->get_item_selector()),
						copySlicedStatement(switchStmt->get_body()));
			}
			break;
		}

		case V_SgCaseOptionStmt:
		{
			SgCaseOptionStmt* caseOptionStmt = isSgCaseOptionStmt(stmt);
			return SageBuilder::buildCaseOptionStmt(
					caseOptionStmt->get_key(),
					caseOptionStmt->get_body());
			break;
		}

		case V_SgDefaultOptionStmt:
		{
			SgDefaultOptionStmt* defaultOptionStmt = isSgDefaultOptionStmt(stmt);
			return SageBuilder::buildDefaultOptionStmt(defaultOptionStmt->get_body());
			break;
		}
			
		default:
			return NULL;
	}
	return NULL;
}

SgExpression* Slicer::copySlicedExpression(SgExpression* expr) const
{
	if (expr == NULL)
		return NULL;
#if 0

#if 0
	if (expr->isLValue())
	{
		if (slice_.find(expr) != slice_.end())
			return SageInterface::copyExpression(expr);
		else
			return NULL;
	}
	return SageInterface::copyExpression(expr);
#endif

	bool isInSlice = slice_.find(expr) != slice_.end();

	if (SageInterface::isAssignmentStatement(expr))
	{
		if (!isInSlice)
			return copySlicedExpression(isSgBinaryOp(expr)->get_lhs_operand());
	}

	if (isSgPlusPlusOp(node) || isSgMinusMinusOp(node))
	{
		SgUnaryOp* unaryExpr = isSgUnaryOp(expr);
		if (isInSlice)
		{
			SgUnaryOp* newExpr = isSgUnaryOp(SageInterface::copyExpression(expr));
			SageInterface::replaceExpression(newExpr->get_operand(), newOperand);
			return newExpr;
		}
	}

	if (SgFunctionCallExp* funcCall = isSgFunctionCallExp(node))
	{
		// TODO  This part should be refined later.
		foreach (SgExpression* expr, funcCall->get_args()->get_expressions())
		{
			if (expr->isLValue())
			{
				SgExpression* def = getLValue(expr);
				defs.insert(def);
			}
		}
		return defs;
	}

	

	if (isVariable(expr))
		return SageInterface::copyExpression(expr);

	if (possibleSlice_.find(expr) == possibleSlice_.end())
		return NULL;


	// TODO
	//ROSE_ASSERT(!"This part should be modified");
	if (SgUnaryOp* unaryExpr = isSgUnaryOp(expr))
	{
		SgExpression* newOperand = copySlicedExpression(unaryExpr->get_operand());
		if (isInSlice)
		{
			SgUnaryOp* newExpr = isSgUnaryOp(SageInterface::copyExpression(expr));
			SageInterface::replaceExpression(newExpr->get_operand(), newOperand);
			return newExpr;
		}
		else
			return newOperand;
	}
	else if (SgBinaryOp* binExpr = isSgBinaryOp(expr))
	{
		SgExpression* lhsExpr = copySlicedExpression(binExpr->get_lhs_operand());
		SgExpression* rhsExpr = copySlicedExpression(binExpr->get_rhs_operand());
		if (lhsExpr == NULL)
			return rhsExpr;
		if (rhsExpr == NULL)
			return lhsExpr;
		SgBinaryOp* newExpr = isSgBinaryOp(SageInterface::copyExpression(expr));
		SageInterface::replaceExpression(newExpr->get_lhs_operand(), lhsExpr);
		SageInterface::replaceExpression(newExpr->get_rhs_operand(), rhsExpr);
		return newExpr;
	}
#endif
	switch (expr->variantT())
	{
		case V_SgCommaOpExp:
		{
			SgCommaOpExp* commaExp = isSgCommaOpExp(expr);
			if (slice_.find(commaExp->get_lhs_operand()) != slice_.end() ||
					slice_.find(commaExp->get_rhs_operand()) != slice_.end())
			{
				SgExpression* lhsExpr = copySlicedExpression(commaExp->get_lhs_operand());
				SgExpression* rhsExpr = copySlicedExpression(commaExp->get_rhs_operand());
				if (lhsExpr == NULL)
					return rhsExpr;
				if (rhsExpr == NULL)
					return lhsExpr;
				return SageBuilder::buildCommaOpExp(lhsExpr, rhsExpr);
			}
			break;
		}

		default:
			//if (isInSlice)
				return SageInterface::copyExpression(expr);
			break;
	}
	return NULL;
}

} // End of namespace Backstroke

