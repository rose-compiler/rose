#include "akgulStyleExpressionProcessor.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "utilities/Utilities.h"
#include "pluggableReverser/eventProcessor.h"

#include <numeric>
#include <algorithm>

struct StoredExpressionReversal : public EvaluationResultAttribute
{
	StoredExpressionReversal(const ExpressionReversal& reversal) : reversal(reversal)
	{
		
	}
	
	ExpressionReversal reversal;
};

vector<EvaluationResult> AkgulStyleExpressionProcessor::evaluate(SgExpression* expression, const VariableVersionTable& varTable, bool isReverseValueUsed)
{
	if (backstroke_util::isAssignmentOp(expression))
	{
		SgBinaryOp* assignOp = isSgBinaryOp(expression);
		ROSE_ASSERT(assignOp != NULL && "All assignments should be binary ops");

		if (backstroke_util::IsVariableReference(assignOp->get_lhs_operand()))
		{
			//Get the variable on the left side of the assign op
			VariableRenaming::VarName destroyedVarName;
			SgExpression* destroyedVarExpression;
			tie(destroyedVarName, destroyedVarExpression) = getReferredVariable(assignOp->get_lhs_operand());
			ROSE_ASSERT(destroyedVarName != VariableRenaming::emptyName);

			//Get the version of the variable before it was overwritten
			VariableRenaming::NumNodeRenameEntry reachingDefs = getVariableRenaming()->getReachingDefsAtNodeForName(assignOp->get_rhs_operand(), destroyedVarName);

			//Call the variable value restorer plugins
			SgExpression* restoredValue = restoreVariable(destroyedVarName, varTable, reachingDefs);
			if (restoredValue != NULL)
			{
				//Success! Let's build an assign op to restore the value
				SgExpression* reverseExpression = SageBuilder::buildAssignOp(SageInterface::copyExpression(destroyedVarExpression), restoredValue);
				string reverseExpString = reverseExpression == NULL ? "NULL" : reverseExpression->unparseToString();
				printf("Line %d:  Reversing '%s' with the expression %s'\n\n", expression->get_file_info()->get_line(),
						expression->unparseToString().c_str(), reverseExpString.c_str());

				//Indicate in the variable version table that we have restored this variable and return
				VariableVersionTable newVarTable = varTable;
				newVarTable.reverseVersion(destroyedVarExpression);

				//Build the evaluation result and return it
				SgExpression* forwardExp = SageInterface::copyExpression(assignOp);
				ExpressionReversal reversalResult(forwardExp, reverseExpression);

				EvaluationResult reversalInfo(this, expression, newVarTable);
				reversalInfo.setAttribute(EvaluationResultAttributePtr(new StoredExpressionReversal(reversalResult)));

				vector<EvaluationResult> result;
				result.push_back(reversalInfo);
				return result;
			}
		}
	}

	return vector<EvaluationResult> ();
}

ExpressionReversal AkgulStyleExpressionProcessor::generateReverseAST(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	StoredExpressionReversal* reversalResult = dynamic_cast<StoredExpressionReversal*>(evaluationResult.getAttribute().get());
	ROSE_ASSERT(reversalResult != NULL);
	ROSE_ASSERT(evaluationResult.getExpressionHandler() == this);

	return reversalResult->reversal;
}


/** Returns the variable name referred by the expression. Also returns
 *  the AST expression for referring to that variable (using the variable renaming analysis).
  * Handles comma ops correctly. */
pair<VariableRenaming::VarName, SgExpression*> AkgulStyleExpressionProcessor::getReferredVariable(SgExpression* exp)
{
	if (SgCommaOpExp* commaOp = isSgCommaOpExp(exp))
	{
		return getReferredVariable(commaOp->get_rhs_operand());
	}

	return pair<VariableRenaming::VarName, SgExpression*>(VariableRenaming::getVarName(exp), exp);
}


multimap<int, SgExpression*> AkgulStyleExpressionProcessor::collectUsesForVariable(VariableRenaming::VarName name, SgNode* node)
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



