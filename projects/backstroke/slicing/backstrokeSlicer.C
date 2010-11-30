#include "backstrokeSlicer.h"
#include <boost/tuple/tuple.hpp>


#define foreach BOOST_FOREACH

namespace Backstroke
{
	
SgFunctionDeclaration* Slicer::slice()
{
	// First, build a PDG for the given function.
	PDGForSlicing pdg(funcDef_);

	// Second, find the corresponding nodes of criteria in the PDG.

	std::stack<PDGForSlicing::Vertex> workList;

	boost::graph_traits<PDGForSlicing>::vertex_iterator first, last;

	for (boost::tie(first, last) = boost::vertices(pdg); first != last; ++first)
	{
		foreach(SgNode* node, criterion_)
		{
			if (pdg[*first]->getNode() == node)
			{
				workList.push(*first);
				break;
			}
		}
	}

	std::set<PDGForSlicing::Vertex> verticesSlice;

	// Then start the backward slicing algorithm.
	while(!workList.empty())
	{
		PDGForSlicing::Vertex v = workList.top();
		workList.pop();

		verticesSlice.insert(v);

		boost::graph_traits<PDGForSlicing>::out_edge_iterator i, j;
		for (boost::tie(i, j) = boost::out_edges(v, pdg); i != j; ++i)
		{
			PDGForSlicing::Vertex tar = boost::target(*i, pdg);
			if (verticesSlice.find(tar) == verticesSlice.end())
				workList.push(tar);
		}
	}

	foreach (PDGForSlicing::Vertex v, verticesSlice)
		slices_.insert(pdg[v]->getNode());

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

	std::string funcName = funcDecl->get_name().str() + std::string("_sliced");
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

SgStatement* Slicer::copySlicedStatement(SgStatement* stmt, SgScopeStatement* scope)
{
	if (stmt == NULL)
		return NULL;
	
	bool isInSlice = slices_.find(stmt) != slices_.end();
	
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
			if (isInSlice)
				return SageInterface::copyStatement(stmt);
			break;

		case V_SgVariableDeclaration:
			if (isInSlice)
				return SageInterface::copyStatement(stmt);
			break;

		case V_SgIfStmt:
		{
			SgIfStmt* ifStmt = isSgIfStmt(stmt);
			if (slices_.find(ifStmt->get_conditional()) != slices_.end())
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
			if (slices_.find(forStmt->get_test()) != slices_.end() ||
					slices_.find(forStmt->get_for_init_stmt()) != slices_.end())
			{
				SgStatement* newInitStmt = copySlicedStatement(forStmt->get_for_init_stmt());
				SgStatement* newTestStmt = copySlicedStatement(forStmt->get_test());
				SgExpression* incrExpr = copySlicedExpression(forStmt->get_increment());

				if (incrExpr == NULL)
					incrExpr = SageBuilder::buildNullExpression();
				
				SgForStatement* newForStmt = SageBuilder::buildForStatement(
						newInitStmt, newTestStmt, incrExpr,
						copySlicedStatement(forStmt->get_loop_body()));
				return newForStmt;
			}
			break;
		}

		case V_SgForInitStatement:
		{
			if (isInSlice)
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
			break;
		}

		case V_SgWhileStmt:
		{
			SgWhileStmt* whileStmt = isSgWhileStmt(stmt);
			if (slices_.find(whileStmt->get_condition()) != slices_.end())
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
			if (slices_.find(switchStmt->get_item_selector()) != slices_.end())
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

SgExpression* Slicer::copySlicedExpression(SgExpression* expr)
{
	if (expr == NULL)
		return NULL;

	bool isInSlice = slices_.find(expr) != slices_.end();

	switch (expr->variantT())
	{
		case V_SgCommaOpExp:
		{
			SgCommaOpExp* commaExp = isSgCommaOpExp(expr);
			if (slices_.find(commaExp->get_lhs_operand()) != slices_.end() ||
					slices_.find(commaExp->get_rhs_operand()) != slices_.end())
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
			if (isInSlice)
				return SageInterface::copyExpression(expr);
			break;
	}
	return NULL;
}

} // End of namespace Backstroke

