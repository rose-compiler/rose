#include "extractFromUseValueRestorer.h"
#include "utilities/cppDefinesAndNamespaces.h"
#include "eventProcessor.h"
#include <rose.h>

using namespace std;

vector<SgExpression*> ExtractFromUseValueRestorer::restoreVariable(VariableRenaming::VarName varName, 
		const VariableVersionTable& availableVariables, VariableRenaming::NumNodeRenameEntry definitions)
{
	VariableRenaming& variableRenamingAnalysis = *getEventHandler()->getVariableRenaming();
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

	//printf("\nLooking for uses of %s version ", VariableRenaming::keyToString(varName).c_str());
	//VariableRenaming::printRenameEntry(definitions);

	//We've collected all the use sites
	//Then process them to extract the variables!
	reverse_foreach (SgNode* useSite, useSites)
	{
		//printf("Line %d: %s: %s\n", useSite->get_file_info()->get_line(), useSite->class_name().c_str(), useSite->unparseToString().c_str());
		SgExpression* value = NULL;
		if (isSgAssignOp(useSite) || isSgAssignInitializer(useSite))
		{
			value = extractFromAssignOp(varName, availableVariables, useSite);
		}
		else if (isSgPlusPlusOp(useSite) || isSgMinusMinusOp(useSite))
		{
			value = extractFromIncrementOp(varName, availableVariables, useSite);
		}
		else if (isSgMinusAssignOp(useSite) || isSgPlusAssignOp(useSite) || isSgXorAssignOp(useSite))
		{
			value = extractFromUseAssignOp(varName, availableVariables, useSite);
		}

		if (value != NULL)
		{
			results.push_back(value);
		}
	}

	return results;
}

SgExpression* ExtractFromUseValueRestorer::extractFromAssignOp(VariableRenaming::VarName varName, 
		const VariableVersionTable& availableVariables,	SgNode* useSite)
{
	//If we're restoring x and we have the assignment a = x, we
	//can extract x from the value of a. An assign initializer is almost exactly like an assign op
	VariableRenaming& variableRenamingAnalysis = *getEventHandler()->getVariableRenaming();

	VariableRenaming::VarName lhsVar;
	SgExpression* useExpression;
	if (SgAssignOp* assignOpUse = isSgAssignOp(useSite))
	{
		useExpression = assignOpUse->get_rhs_operand();
		lhsVar = variableRenamingAnalysis.getVarName(assignOpUse->get_lhs_operand());
	}
	else if (SgAssignInitializer* assignInitializer = isSgAssignInitializer(useSite))
	{
		useExpression = assignInitializer->get_operand();
		SgInitializedName* declaredVar = isSgInitializedName(assignInitializer->get_parent());
		ROSE_ASSERT(declaredVar != NULL);
		lhsVar = variableRenamingAnalysis.getVarName(declaredVar);
	}
	else
	{
		ROSE_ASSERT(false);
	}

	//The left-hand side of the assign is more complex than just a variable. This should have been normalized...
	if (lhsVar == VariableRenaming::emptyName)
	{
		return NULL;
	}

	printf("Found suitable use for variable %s. Its value was saved in expression '%s' on line %d\n",
		VariableRenaming::keyToString(varName).c_str(), useExpression->unparseToString().c_str(),
		useSite->get_file_info()->get_line());

	//Ok, restore the lhs variable to the version used in the assignment.
	VariableRenaming::NumNodeRenameEntry lhsVersion = variableRenamingAnalysis.getReachingDefsAtNodeForName(useSite, lhsVar);
	printf("Recursively restoring variable '%s' to version ", VariableRenaming::keyToString(lhsVar).c_str());
	VariableRenaming::printRenameEntry(lhsVersion);
	printf(" on line %d\n", useSite->get_file_info()->get_line());

	SgExpression* useExpressionValue = getEventHandler()->restoreVariable(lhsVar, availableVariables, lhsVersion);
	if (useExpressionValue != NULL)
	{
		return restoreVariableFromExpression(varName, availableVariables, useExpression, useExpressionValue);
	}

	return NULL;
}


SgExpression* ExtractFromUseValueRestorer::extractFromIncrementOp(VariableRenaming::VarName varName,
		const VariableVersionTable& availableVariables,	SgNode* useSite)
{
	ROSE_ASSERT(isSgPlusPlusOp(useSite) || isSgMinusMinusOp(useSite));
	VariableRenaming& variableRenamingAnalysis = *getEventHandler()->getVariableRenaming();
	SgUnaryOp* incOrDecrOp = isSgUnaryOp(useSite);

	// For floating point values, we can't extract the previous value by undoing the increment due to rounding
	// For bool type, we also cannot do this.
	if (!incOrDecrOp->get_operand()->get_type()->isIntegerType() ||
			isSgTypeBool(incOrDecrOp->get_operand()->get_type()))
	{
		return NULL;
	}

	ROSE_ASSERT(VariableRenaming::getVarName(incOrDecrOp->get_operand()) == varName);

	//Ok, so we have something like a++. The initial version of a is 1 and the final version of a is 2. (for example)
	//We would like to recover version 1 given version two
	VariableRenaming::NumNodeRenameEntry versionAfterIncrement = variableRenamingAnalysis.getDefsAtNodeForName(incOrDecrOp, varName);
	printf("Recursively restoring variable '%s' to version ", VariableRenaming::keyToString(varName).c_str());
	VariableRenaming::printRenameEntry(versionAfterIncrement);

	SgExpression* valueAfterIncrement = getEventHandler()->restoreVariable(varName, availableVariables, versionAfterIncrement);
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

		return result;
	}

	return NULL;
}


SgExpression* ExtractFromUseValueRestorer:: extractFromUseAssignOp(VariableRenaming::VarName varName,
		const VariableVersionTable& availableVariables,	SgNode* useSite)
{
	//Assignments such as +=, -=, and |= are use sites of the lhs variable.
	//Let's say we have x += <exp>. If we know the new value of x and the value of <exp>, we can extract the previous
	//value of x
	ROSE_ASSERT(isSgMinusAssignOp(useSite) || isSgPlusAssignOp(useSite) || isSgXorAssignOp(useSite));
	VariableRenaming& variableRenamingAnalysis = *getEventHandler()->getVariableRenaming();
	SgBinaryOp* useAssignOp = isSgBinaryOp(useSite);

	// Both lhs and rhs operands should be integer types.
	// The lhs operand shall not be of bool type.
	if (isSgMinusAssignOp(useAssignOp) || isSgPlusAssignOp(useAssignOp))
	{
		if (!useAssignOp->get_type()->isIntegerType() ||
			!useAssignOp->get_rhs_operand()->get_type()->isIntegerType() ||
			isSgTypeBool(useAssignOp->get_lhs_operand()->get_type()))
		{
			//We can't extract values out of floating point types due to rounding
			return NULL;
		}
	}

	//The left-hand side should have been normalized to be a variable
	VariableRenaming::VarName lhsVariable = VariableRenaming::getVarName(useAssignOp->get_lhs_operand());
	ROSE_ASSERT(lhsVariable != VariableRenaming::emptyName);

	//First, find the version of the assigned variable after the assignment and restore it
	VariableRenaming::NumNodeRenameEntry versionAfterAssign =
			variableRenamingAnalysis.getDefsAtNodeForName(useAssignOp, lhsVariable);

	SgExpression* valueAfterAssign = getEventHandler()->restoreVariable(lhsVariable, availableVariables, versionAfterAssign);
	if (valueAfterAssign == NULL)
	{
		return NULL;
	}

	if (lhsVariable == varName)
	{
		//We have a situation such as x += <exp> and we would like to restore the value of x

		//Now, restore the other expression involved in the assignment
		SgExpression* rhsValue = getEventHandler()->restoreExpressionValue(useAssignOp->get_rhs_operand(), availableVariables);
		if (rhsValue == NULL)
		{
			SageInterface::deepDelete(valueAfterAssign);
			return NULL;
		}

		//Combine the lhs and rhs values to get the desired value of x
		SgExpression* desiredVal = NULL;
		switch (useAssignOp->variantT())
		{
			case V_SgPlusAssignOp:
				desiredVal = SageBuilder::buildSubtractOp(valueAfterAssign, rhsValue);
				break;
			case V_SgMinusAssignOp:
				desiredVal = SageBuilder::buildAddOp(valueAfterAssign, rhsValue);
				break;
			case V_SgXorAssignOp:
				desiredVal = SageBuilder::buildBitXorOp(valueAfterAssign, rhsValue);
				break;
			default:
				ROSE_ASSERT(false);
		}

		return desiredVal;
	}
	else if (!variableRenamingAnalysis.getUsesAtNodeForName(useAssignOp->get_rhs_operand(), varName).empty())
	{
		//We have a situation such as a += (x ^ 2) where we want to extract x. We have already restored the new value of a
		//We need to restore the old value of a, and the difference is the rhs (x ^ 2)
		VariableRenaming::NumNodeRenameEntry versionBeforeAssign =
				variableRenamingAnalysis.getUsesAtNodeForName(useAssignOp, lhsVariable);

		SgExpression* valueBeforeAssign = getEventHandler()->restoreVariable(lhsVariable, availableVariables, versionBeforeAssign);
		if (valueBeforeAssign == NULL)
		{
			SageInterface::deepDelete(valueBeforeAssign);
			return NULL;
		}

		SgExpression* rhsValue = NULL;
		switch (useAssignOp->variantT())
		{
			case V_SgPlusAssignOp:
				rhsValue = SageBuilder::buildSubtractOp(valueAfterAssign, valueBeforeAssign);
				break;
			case V_SgMinusAssignOp:
				rhsValue = SageBuilder::buildSubtractOp(valueBeforeAssign, valueAfterAssign);
				break;
			case V_SgXorAssignOp:
				rhsValue = SageBuilder::buildBitXorOp(valueBeforeAssign, valueAfterAssign);
				break;
			default:
				ROSE_ASSERT(false);
		}

		return restoreVariableFromExpression(varName, availableVariables, useAssignOp->get_rhs_operand(), rhsValue);
	}

	ROSE_ASSERT(false);
	return NULL;
}


SgExpression* ExtractFromUseValueRestorer::restoreVariableFromExpression(VariableRenaming::VarName variable,
			const VariableVersionTable& availableVariables, SgExpression* expression, SgExpression* expressionValue)
{
	VariableRenaming& varRenaming = *getEventHandler()->getVariableRenaming();

	if (VariableRenaming::getVarName(expression) != VariableRenaming::emptyName)
	{
		//We're trying to restore the value of x, and the expression itself is x. So, nothing to do
		if (VariableRenaming::getVarName(expression) == variable)
		{
			return expressionValue;
		}
		else
		{
			//TODO The expression is a variable, but it could be a struct that contains x.
			//Here we should build an expression to extract x from its parent
			SageInterface::deepDelete(expressionValue);
			return NULL;
		}
	}
	else if (isSgAddOp(expression) || isSgSubtractOp(expression) || isSgBitXorOp(expression))
	{
		//We know the value of b + x and we would like to restore the value of x. For this, we need to restore the value of b
		SgBinaryOp* binaryOp = isSgBinaryOp(expression);

		//For addition and subtraction, we can only extact values from integer types (not floating point due to rounding error)
		if ((isSgAddOp(expression) || isSgSubtractOp(expression)) && !expression->get_type()->isIntegerType())
		{
			SageInterface::deepDelete(expressionValue);
			return NULL;
		}

		//Find if x is used in the right-hand-side or the left-hand side
		bool usedLeft = !varRenaming.getUsesAtNodeForName(binaryOp->get_lhs_operand(), variable).empty();
		bool usedRight = !varRenaming.getUsesAtNodeForName(binaryOp->get_rhs_operand(), variable).empty();
		ROSE_ASSERT(usedLeft || usedRight);
		if (usedLeft && usedRight)
		{
			//We can't extract the value if it's used on both sides of the op
			SageInterface::deepDelete(expressionValue);
			return NULL;
		}

		//Whichever side the variable is in, we need to restore the value of the other one
		SgExpression* otherOperand = usedLeft ? binaryOp->get_rhs_operand() : binaryOp->get_lhs_operand();
		SgExpression* otherOperandValue = getEventHandler()->restoreExpressionValue(otherOperand, availableVariables);
		if (otherOperandValue == NULL)
		{
			SageInterface::deepDelete(expressionValue);
			return NULL;
		}

		SgExpression* usedExpressionValue;
		//Case 1: We have the value of (b + x) and we have the value of (b). Then, x = (b + x) - (b)
		if (isSgAddOp(binaryOp))
		{
			usedExpressionValue = SageBuilder::buildSubtractOp(expressionValue, otherOperandValue);
		}
		//Case 2: We have the value of (x - b) and we have the value of (b). Then, x = (x - b) + (b)
		else if (isSgSubtractOp(binaryOp) && usedLeft)
		{
			usedExpressionValue = SageBuilder::buildAddOp(expressionValue, otherOperandValue);
		}
		//Case 3: We have the value of (b - x) and we have the value of (b). Then x = b - (b - x)
		else if (isSgSubtractOp(binaryOp) && usedRight)
		{
			usedExpressionValue = SageBuilder::buildSubtractOp(otherOperandValue, expressionValue);
		}
		//Case 4: We have the value of (b ^ x) and we have the value of (b). Then x = (b ^ x) ^ b
		else if (isSgBitXorOp(binaryOp))
		{
			usedExpressionValue = SageBuilder::buildBitXorOp(expressionValue, otherOperandValue);
		}
		else
		{
			ROSE_ASSERT(false);
		}

		//If there is chained addition such as (a + b + x), we have to recurse to recover x
		SgExpression* usedExpression = usedLeft ? binaryOp->get_lhs_operand() : binaryOp->get_rhs_operand();
		return restoreVariableFromExpression(variable, availableVariables, usedExpression, usedExpressionValue);
	}

	SageInterface::deepDelete(expressionValue);
	return NULL;
}
