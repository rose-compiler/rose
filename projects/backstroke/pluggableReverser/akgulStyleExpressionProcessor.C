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



/** Extract-from use technique (rudimentary) */
vector<SgExpression*> ExtractFromUseRestorer::restoreVariable(VariableRenaming::VarName varName, const VariableVersionTable& availableVariables,
		VariableRenaming::NumNodeRenameEntry definitions)
{
	VariableRenaming& variableRenamingAnalysis = *getEventProcessor()->getVariableRenaming();
	vector<SgExpression*> results;

	set<SgNode*> useSites;

	pair<int, SgNode*> versionDefPair;
	foreach(versionDefPair, definitions)
	{
		//Get all the uses of this version
		vector<SgNode*> usesForVersion = variableRenamingAnalysis.getAllUsesForDef(varName, versionDefPair.first);

		//There might be multiple versions we require. Only add the uses that have all the desired versions
		foreach(SgNode* potentialUseNode, usesForVersion)
		{
			VariableRenaming::NumNodeRenameEntry definitionsAtUse = variableRenamingAnalysis.getUsesAtNodeForName(potentialUseNode, varName);

			if (definitionsAtUse == definitions)
			{
				useSites.insert(potentialUseNode);
			}
		}
	}

	//printf("\nLooking for uses of %s:\n", VariableRenaming::keyToString(varName).c_str());

	//We've collected all the use sites!
	//Print them out to check if this is right
	//Then process them to extract the variables!
	reverse_foreach (SgNode* useSite, useSites)
	{
		//printf("Line %d: %s: %s\n", useSite->get_file_info()->get_line(), useSite->class_name().c_str(), useSite->unparseToString().c_str());
		//If we're restoring x and we have the assignment a = x, we
		//can extract x from the value of a. An assign initializer is almost exactly like an assign op
		if (isSgAssignOp(useSite) || isSgAssignInitializer(useSite))
		{
			VariableRenaming::VarName rhsVar, lhsVar;

			if (SgAssignOp* assignOpUse = isSgAssignOp(useSite))
			{
				rhsVar = variableRenamingAnalysis.getVarName(assignOpUse->get_rhs_operand());
				lhsVar = variableRenamingAnalysis.getVarName(assignOpUse->get_lhs_operand());
			}
			else if (SgAssignInitializer* assignInitializer = isSgAssignInitializer(useSite))
			{
				rhsVar = variableRenamingAnalysis.getVarName(assignInitializer->get_operand());
				SgInitializedName* declaredVar = isSgInitializedName(assignInitializer->get_parent());
				ROSE_ASSERT(declaredVar != NULL);
				lhsVar = variableRenamingAnalysis.getVarName(declaredVar);
			}
			else
			{
				ROSE_ASSERT(false);
			}

			//If this is a more complex op, such as a = x + y, we'll handle it in the future
			if (rhsVar != varName || lhsVar == VariableRenaming::emptyName)
			{
				continue;
			}

			printf("Found suitable use for variable %s. Its value was saved in variable %s on line %d\n",
				VariableRenaming::keyToString(varName).c_str(), VariableRenaming::keyToString(lhsVar).c_str(),
				useSite->get_file_info()->get_line());
			
			//Ok, restore the lhs variable to the version used in the assignment.
			//Then, that expression will hold the desired value of x
			VariableRenaming::NumNodeRenameEntry lhsVersion = variableRenamingAnalysis.getReachingDefsAtNodeForName(useSite, lhsVar);
			printf("Recursively restoring variable '%s' to version ", VariableRenaming::keyToString(lhsVar).c_str());
			VariableRenaming::printRenameEntry(lhsVersion);
			printf(" on line %d\n", useSite->get_file_info()->get_line());

			SgExpression* resultFromThisUse = getEventProcessor()->restoreVariable(lhsVar, availableVariables, lhsVersion);
			if (resultFromThisUse != NULL)
			{
				results.push_back(resultFromThisUse);
			}
		}
		else if (isSgPlusPlusOp(useSite) || isSgMinusMinusOp(useSite))
		{
			SgUnaryOp* incOrDecrOp = isSgUnaryOp(useSite);
			ROSE_ASSERT(VariableRenaming::getVarName(incOrDecrOp->get_operand()) == varName);

			//Ok, so we have something like a++. The initial version of a is 1 and the final version of a is 2. (for example)
			//We would like to recover version 1 given version two
			VariableRenaming::NumNodeRenameEntry versionAfterIncrement = variableRenamingAnalysis.getDefsAtNodeForName(incOrDecrOp, varName);
			printf("Recursively restoring variable '%s' to version ", VariableRenaming::keyToString(varName).c_str());
			VariableRenaming::printRenameEntry(versionAfterIncrement);

			SgExpression* valueAfterIncrement = getEventProcessor()->restoreVariable(varName, availableVariables, versionAfterIncrement);
			if (valueAfterIncrement != NULL)
			{
				//Success! Now all we have to do is subtract 1 to recover the value we want
				SgExpression* result;
				if (isSgPlusPlusOp(incOrDecrOp))
				{
					result = SageBuilder::buildSubtractOp(valueAfterIncrement, SageBuilder::buildIntVal(1));
				}
				else if (isSgMinusMinusOp(incOrDecrOp))
				{
					result = SageBuilder::buildAddOp(valueAfterIncrement, SageBuilder::buildIntVal(1));
				}
				else
				{
					ROSE_ASSERT(false);
				}

				results.push_back(result);
			}
		}
	}

	return results;
}