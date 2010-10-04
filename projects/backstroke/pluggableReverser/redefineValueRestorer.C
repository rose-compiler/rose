#include "redefineValueRestorer.h"
#include "eventHandler.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "utilities/Utilities.h"
#include <rose.h>

vector<SgExpression*> RedefineValueRestorer::restoreVariable(VariableRenaming::VarName destroyedVarName, const VariableVersionTable& availableVariables,
		VariableRenaming::NumNodeRenameEntry definitions)
{
	vector<SgExpression*> results;

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
		return handleAssignOp(destroyedVarName, availableVariables, reachingDefinition);
	}
	else if (isSgPlusPlusOp(reachingDefinition) || isSgMinusMinusOp(reachingDefinition))
	{
		return handleIncrementDecrementDefinition(destroyedVarName, availableVariables, reachingDefinition);
	}
	else if (SageInterface::isAssignmentStatement(reachingDefinition))
	{
		return handleUseAssignDefinition(destroyedVarName, availableVariables, reachingDefinition);
	}
	else
	{
		//What other types of reaching definitions are there??
		printf("********** WARNING **********\n");
		printf("In RedefineValueRestorer: Encountered unhandled reaching definition of type %s\n",
			reachingDefinition->class_name().c_str());
		return results;
	}
}


//! Handle +=, -=, */, etc
vector<SgExpression*> RedefineValueRestorer::handleUseAssignDefinition(VariableRenaming::VarName destroyedVarName,
		const VariableVersionTable& availableVariables, SgNode* reachingDefinition)
{
	vector<SgExpression*> results;
	ROSE_ASSERT(SageInterface::isAssignmentStatement(reachingDefinition) && !isSgAssignOp(reachingDefinition));

	//These are all the assignments that also use the left value in addition to the right.
	//E.g. /=,  *=, &=, etc. We restore both the left and the right operands to their pre-assignment version
	SgBinaryOp* assignment = isSgBinaryOp(reachingDefinition);
	ROSE_ASSERT(VariableRenaming::getVarName(assignment->get_lhs_operand()) == destroyedVarName);

	//Find the version of the variable before the assignment
	VariableRenaming::NumNodeRenameEntry lhsVersion =
			getEventHandler()->getVariableRenaming()->getReachingDefsAtNodeForName(assignment->get_lhs_operand(), destroyedVarName);

	//Restore the left-hand side and the right-hand side
	SgExpression* lhsValue = getEventHandler()->restoreVariable(destroyedVarName, availableVariables, lhsVersion);
	SgExpression* rhsValue = getEventHandler()->restoreExpressionValue(assignment->get_rhs_operand(), availableVariables);

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

	return results;
}


vector<SgExpression*> RedefineValueRestorer::handleIncrementDecrementDefinition(VariableRenaming::VarName destroyedVarName,
			const VariableVersionTable& availableVariables, SgNode* reachingDefinition)
{
	ROSE_ASSERT(isSgPlusPlusOp(reachingDefinition) || isSgMinusMinusOp(reachingDefinition));
	SgUnaryOp* incrementOp = isSgUnaryOp(reachingDefinition);
	ROSE_ASSERT(incrementOp != NULL && VariableRenaming::getVarName(incrementOp->get_operand()) == destroyedVarName);
	vector<SgExpression*> results;

	//See what version of the variable was before it was incremented
	VariableRenaming::NumNodeRenameEntry versionIncremented = getEventHandler()->getVariableRenaming()->
			getUsesAtNodeForName(incrementOp->get_operand(), destroyedVarName);

	//Restore that version
	SgExpression* valueBeforeIncrement = getEventHandler()->restoreVariable(destroyedVarName, availableVariables, versionIncremented);
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

	return results;
}


vector<SgExpression*> RedefineValueRestorer::handleAssignOp(VariableRenaming::VarName destroyedVarName,
		const VariableVersionTable& availableVariables, SgNode* reachingDefinition)
{
	ROSE_ASSERT(isSgAssignOp(reachingDefinition) || isSgInitializedName(reachingDefinition));
	vector<SgExpression*> results;

	SgExpression* definitionExpression;
	if (SgAssignOp * assignOp = isSgAssignOp(reachingDefinition))
	{
		//Check that this assign op is for the same variable
		ROSE_ASSERT(VariableRenaming::getVarName(assignOp->get_lhs_operand()) == destroyedVarName);
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
	SgExpression* definitionExpressionValue = getEventHandler()->restoreExpressionValue(definitionExpression, availableVariables);
	if (definitionExpressionValue != NULL)
	{
		results.push_back(definitionExpressionValue);
	}

	return results;
}

