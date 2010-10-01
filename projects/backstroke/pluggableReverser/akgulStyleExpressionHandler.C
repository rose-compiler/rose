#include "akgulStyleExpressionHandler.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "utilities/Utilities.h"

#include <numeric>
#include <algorithm>

vector<EvaluationResult> AkgulStyleExpressionHandler::evaluate(SgExpression* expression, const VariableVersionTable& varTable,
		bool isReverseValueUsed)
{
	VariableRenaming::VarName destroyedVarName;
	SgExpression* destroyedVarExpression = NULL;
	VariableRenaming::NumNodeRenameEntry reachingDefs;

	//First, find out what variable was destroyed and what are its reaching definitions
	if (backstroke_util::isAssignmentOp(expression))
	{
		SgBinaryOp* assignOp = isSgBinaryOp(expression);
		ROSE_ASSERT(assignOp != NULL && "All assignments should be binary ops");

		if (VariableRenaming::getVarName(assignOp->get_lhs_operand()) != VariableRenaming::emptyName)
		{
			//Get the variable on the left side of the assign op
			tie(destroyedVarName, destroyedVarExpression) = getReferredVariable(assignOp->get_lhs_operand());
			ROSE_ASSERT(destroyedVarName != VariableRenaming::emptyName);

			//Get the version of the variable before it was overwritten
			reachingDefs = getVariableRenaming()->getReachingDefsAtNodeForName(assignOp->get_rhs_operand(), destroyedVarName);

		}
	}
	else if (isSgPlusPlusOp(expression) || isSgMinusMinusOp(expression))
	{
		SgUnaryOp* unaryOp = isSgUnaryOp(expression);

		//It would be extra work to make postfix increments return the correct value in the reverse code, so skip that case
		if (!isReverseValueUsed || (unaryOp->get_mode() == SgUnaryOp::prefix))
		{
			//Get the variable that was incremented
			tie(destroyedVarName, destroyedVarExpression) = getReferredVariable(unaryOp->get_operand());
			ROSE_ASSERT(destroyedVarName != VariableRenaming::emptyName);

			//Get the version of the variable before it was incremented
			reachingDefs = getVariableRenaming()->getUsesAtNodeForName(unaryOp, destroyedVarName);
		}
	}

	//Now we know the reaching definitions of the previous value of the variable. Restore the previous value and assign it
	if (destroyedVarExpression != NULL)
	{
		SgExpression* restoredValue = restoreVariable(destroyedVarName, varTable, reachingDefs);
		if (restoredValue != NULL)
		{
			//Success! Let's build an assign op to restore the value
			SgExpression* reverseExpression = SageBuilder::buildAssignOp(SageInterface::copyExpression(destroyedVarExpression),
					restoredValue);
			string reverseExpString = reverseExpression == NULL ? "NULL" : reverseExpression->unparseToString();
			printf("Line %d:  Reversing '%s' with the expression %s'\n\n", expression->get_file_info()->get_line(),
					expression->unparseToString().c_str(), reverseExpString.c_str());

			//Indicate in the variable version table that we have restored this variable
			VariableVersionTable newVarTable = varTable;
			newVarTable.reverseVersion(destroyedVarExpression);

			//Build the evaluation result and return it
			SgExpression* forwardExp = SageInterface::copyExpression(expression);
			ExpressionReversal reversalResult(forwardExp, reverseExpression);

			EvaluationResult reversalInfo(this, expression, newVarTable);
			//reversalInfo.setAttribute(EvaluationResultAttributePtr(new StoredExpressionReversal(reversalResult)));
			reversalInfo.setAttribute(reversalResult);

			vector<EvaluationResult> result;
			result.push_back(reversalInfo);
			return result;
		}
	}

	return vector<EvaluationResult>();
}

ExpressionReversal AkgulStyleExpressionHandler::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	ROSE_ASSERT(evaluationResult.getExpressionHandler() == this);
	return evaluationResult.getAttribute<ExpressionReversal>();
}


/** Returns the variable name referred by the expression. Also returns
 *  the AST expression for referring to that variable (using the variable renaming analysis).
  * Handles comma ops correctly. */
pair<VariableRenaming::VarName, SgExpression*> AkgulStyleExpressionHandler::getReferredVariable(SgExpression* exp)
{
	if (SgCommaOpExp* commaOp = isSgCommaOpExp(exp))
	{
		return getReferredVariable(commaOp->get_rhs_operand());
	}

	return pair<VariableRenaming::VarName, SgExpression*>(VariableRenaming::getVarName(exp), exp);
}


multimap<int, SgExpression*> AkgulStyleExpressionHandler::collectUsesForVariable(VariableRenaming::VarName name, SgNode* node)
{
	class CollectUses : public AstBottomUpProcessing<bool>
	{
	public:
		multimap<int, SgExpression*> result;
		VariableRenaming* variableRenamingAnalysis;
		VariableRenaming::VarName desiredVar;

		virtual bool evaluateSynthesizedAttribute(SgNode* astNode, SynthesizedAttributesList childAttributes)
		{
			bool childrenHaveUses = accumulate(childAttributes.begin(), childAttributes.end(), false, logical_or<bool>());
			if (childrenHaveUses)
			{
				//We return true if the lowest-level use is below us
				return true;
			}

			//No uses at lower-level nodes. Look for uses here
			VariableRenaming::NumNodeRenameEntry versionToUseMap = variableRenamingAnalysis->getUsesAtNodeForName(astNode, desiredVar);

			foreach(VariableRenaming::NumNodeRenameEntry::value_type versionUsePair, versionToUseMap)
			{
				ROSE_ASSERT(isSgExpression(astNode)); //The lowest level use should always be an expression
				int version = versionUsePair.first;
				result.insert(make_pair(version, isSgExpression(astNode)));
			}

			return versionToUseMap.size() > 0;
		}
	};

	CollectUses traversal;
	traversal.variableRenamingAnalysis = getVariableRenaming();
	traversal.desiredVar = name;
	traversal.traverse(node);

	return traversal.result;
}



