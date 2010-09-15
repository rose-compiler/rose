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
