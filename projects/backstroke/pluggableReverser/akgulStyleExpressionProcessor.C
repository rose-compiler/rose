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


/** Implement redefine technique */
vector<SgExpression*> RedefineValueRestorer::restoreVariable(VariableRenaming::VarName destroyedVarName, const VariableVersionTable& availableVariables,
		VariableRenaming::NumNodeRenameEntry definitions)
{
	vector<SgExpression*> results;
	VariableRenaming& varRenaming = *getEventProcessor()->getVariableRenaming();

	if (definitions.size() > 1)
	{
		//TODO: We don't support resolving multiple definitions yet.
		return results;
	}
	ROSE_ASSERT(definitions.size() > 0 && "Why doesn't the variable have any reaching definitions?");

	SgNode* reachingDefinition = definitions.begin()->second;

	printf("The reaching definition for %s is %s: %s on line %d\n", VariableRenaming::keyToString(destroyedVarName).c_str(), reachingDefinition->class_name().c_str(),
			reachingDefinition->unparseToString().c_str(),
			reachingDefinition->get_file_info()->get_line());

	//Find what expression was evaluated to assign the variable to its previous value
	if (isSgAssignOp(reachingDefinition) || isSgInitializedName(reachingDefinition))
	{
		SgExpression* definitionExpression;
		if (SgAssignOp * assignOp = isSgAssignOp(reachingDefinition))
		{
			//Check that this assign op is for the same variable
			ROSE_ASSERT(AkgulStyleExpressionProcessor::getReferredVariable(assignOp->get_lhs_operand()).first == destroyedVarName);

			definitionExpression = assignOp->get_rhs_operand();
		}
		else if (SgInitializedName * declaration = isSgInitializedName(reachingDefinition))
		{
			//FIXME: The initialized name might be for the parent of this variable.
			//For example, the reaching defintion of s.x might be s = foo(). In this case,
			//we have to restore s, then append the .x accessor to it...

			//If the previous definition is a declaration without an initializer and the declaration is not
			//in a function parameter list, then the previous value of the variable is undefined and we don't
			//need to restore it
			if (declaration->get_initializer() == NULL)
			{
				if (isSgFunctionParameterList(declaration->get_parent()))
				{
					//The variable has a well-defined previous value but we can't re-execute its defintion
					return results;
				}

				results.push_back(VariableRenaming::buildVariableReference(destroyedVarName));
				return results;
			}
			else if (SgAssignInitializer * assignInit = isSgAssignInitializer(declaration->get_initializer()))
			{
				definitionExpression = assignInit->get_operand();
			}
			else
			{
				//Todo: Handle constructor initializer, aggregate initializer, etc.
				return results;
			}
		}

		//Ok, so the variable was previously defined by assignment to initExpression.
		//Now we need to see if we can re-execute that expression
		SgExpression* definitionExpressionValue = getEventProcessor()->restoreExpressionValue(definitionExpression, availableVariables);
		if (definitionExpressionValue != NULL)
		{
			results.push_back(definitionExpressionValue);
		}
	}
	else if (isSgPlusPlusOp(reachingDefinition) || isSgMinusMinusOp(reachingDefinition))
	{
		SgUnaryOp* incrementOp = isSgUnaryOp(reachingDefinition);
		ROSE_ASSERT(incrementOp != NULL && VariableRenaming::getVarName(incrementOp->get_operand()) == destroyedVarName);

		//See what version of the variable was used
		VariableRenaming::NumNodeRenameEntry versionIncremented = varRenaming.getUsesAtNodeForName(incrementOp->get_operand(), destroyedVarName);

		//Restore that version
		SgExpression* valueBeforeIncrement = getEventProcessor()->restoreVariable(destroyedVarName, availableVariables, versionIncremented);
		if (valueBeforeIncrement != NULL)
		{
			SgExpression* restoredValue;
			if (isSgPlusPlusOp(incrementOp))
			{
				restoredValue = SageBuilder::buildAddOp(valueBeforeIncrement, SageBuilder::buildIntVal(1));
			}
			else if (isSgMinusMinusOp(incrementOp))
			{
				restoredValue = SageBuilder::buildSubtractOp(valueBeforeIncrement, SageBuilder::buildIntVal(1));
			}
			else
			{
				ROSE_ASSERT(false);
			}

			results.push_back(restoredValue);
		}
	}
	else if (backstroke_util::isAssignmentOp(reachingDefinition))
	{
		//These are all the assignments that also use the left value in addition to the right.
		//E.g. /=,  *=, &=, etc. We restore bot the left and the right operands to their pre-assignment version
		SgBinaryOp* assignment = isSgBinaryOp(reachingDefinition);
		ROSE_ASSERT(VariableRenaming::getVarName(assignment->get_lhs_operand()) == destroyedVarName);

		//Find the version of the variable before the assignment
		VariableRenaming::NumNodeRenameEntry lhsVersion =
				varRenaming.getReachingDefsAtNodeForName(assignment->get_lhs_operand(), destroyedVarName);
		printf("The lhs version was \n");
		VariableRenaming::printRenameEntry(lhsVersion);

		//Restore the left-hand side and the right-hand side
		SgExpression* lhsValue = getEventProcessor()->restoreVariable(destroyedVarName, availableVariables, lhsVersion);
		SgExpression* rhsValue = getEventProcessor()->restoreExpressionValue(assignment->get_rhs_operand(), availableVariables);

		//Combine the lhs and rhs according to the assignment type to restore the value
		if (lhsValue != NULL && rhsValue != NULL)
		{
			switch (assignment->variantT())
			{
				case V_SgPlusAssignOp:
					results.push_back(SageBuilder::buildAddOp(lhsValue, rhsValue));
					break;
				case V_SgMinusAssignOp:
					results.push_back(SageBuilder::buildSubtractOp(lhsValue, rhsValue));
					break;
				case V_SgMultAssignOp:
					results.push_back(SageBuilder::buildMultiplyOp(lhsValue, rhsValue));
					break;
				case V_SgDivAssignOp:
					results.push_back(SageBuilder::buildDivideOp(lhsValue, rhsValue));
					break;
				case V_SgModAssignOp:
					results.push_back(SageBuilder::buildModOp(lhsValue, rhsValue));
					break;
				case V_SgIorAssignOp:
					results.push_back(SageBuilder::buildBitOrOp(lhsValue, rhsValue));
					break;
				case V_SgAndAssignOp:
					results.push_back(SageBuilder::buildBitAndOp(lhsValue, rhsValue));
					break;
				case V_SgXorAssignOp:
					results.push_back(SageBuilder::buildBitXorOp(lhsValue, rhsValue));
					break;
				case V_SgLshiftAssignOp:
					results.push_back(SageBuilder::buildLshiftOp(lhsValue, rhsValue));
					break;
				case V_SgRshiftAssignOp:
					results.push_back(SageBuilder::buildRshiftOp(lhsValue, rhsValue));
					break;
				default:
					ROSE_ASSERT(false);
			}
		}
		else
		{
			//One of the restorations did not succeed, delete the trees
			if (lhsValue != NULL)
			{
				SageInterface::deepDelete(lhsValue);
			}
			if (rhsValue != NULL)
			{
				SageInterface::deepDelete(rhsValue);
			}
		}
	}
	else
	{
		//What other types of reaching definitions are there??
		printf("********** WARNING **********\n");
		printf("In RedefineValueRestorer: Encountered unhandled reaching definition of type %s\n",
			reachingDefinition->class_name().c_str());
		return results;
	}

	return results;
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