#include "extractFromUseValueRestorer.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "eventProcessor.h"
#include <rose.h>

vector<SgExpression*> ExtractFromUseValueRestorer::restoreVariable(VariableRenaming::VarName varName, const VariableVersionTable& availableVariables,
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
				continue;
			}

			printf("Found suitable use for variable %s. Its value was saved in expression '%s' on line %d\n",
				VariableRenaming::keyToString(varName).c_str(), useExpression->unparseToString().c_str(),
				useSite->get_file_info()->get_line());

			//Ok, restore the lhs variable to the version used in the assignment.
			VariableRenaming::NumNodeRenameEntry lhsVersion = variableRenamingAnalysis.getReachingDefsAtNodeForName(useSite, lhsVar);
			printf("Recursively restoring variable '%s' to version ", VariableRenaming::keyToString(lhsVar).c_str());
			VariableRenaming::printRenameEntry(lhsVersion);
			printf(" on line %d\n", useSite->get_file_info()->get_line());

			SgExpression* useExpressionValue = getEventProcessor()->restoreVariable(lhsVar, availableVariables, lhsVersion);
			if (useExpressionValue != NULL)
			{
				SgExpression* restoredVarValue = restoreVariableFromExpression(varName, availableVariables, useExpression, useExpressionValue);
				results.push_back(restoredVarValue);
			}
		}
		else if (isSgPlusPlusOp(useSite) || isSgMinusMinusOp(useSite))
		{
			SgUnaryOp* incOrDecrOp = isSgUnaryOp(useSite);

			//For floating point values, we can't extract the previous value by undoing the increment due to rounding
			if (!incOrDecrOp->get_operand()->get_type()->isIntegerType())
			{
				continue;
			}
			
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

SgExpression* ExtractFromUseValueRestorer::restoreVariableFromExpression(VariableRenaming::VarName variable,
			const VariableVersionTable& availableVariables, SgExpression* expression, SgExpression* expressionValue)
{
	VariableRenaming& varRenaming = *getEventProcessor()->getVariableRenaming();

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
		SgExpression* otherOperandValue = getEventProcessor()->restoreExpressionValue(otherOperand, availableVariables);
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
