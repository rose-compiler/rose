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
	if (isSgAssignOp(expression))
	{
		SgAssignOp* assignOp = isSgAssignOp(expression);

		if (backstroke_util::IsVariableReference(assignOp->get_lhs_operand()))
		{
			SgExpression* reverseExpression;
			if (handleAssignOp(assignOp, varTable, reverseExpression))
			{
				string reverseExpString = reverseExpression == NULL ? "NULL" : reverseExpression->unparseToString();
				printf("Line %d:  Reversing '%s' with the expression %s'\n\n", expression->get_file_info()->get_line(),
						expression->unparseToString().c_str(), reverseExpString.c_str());

				//Indicate in the variable version table that we have restored this variable and return
				VariableVersionTable newVarTable = varTable;
				newVarTable.reverseVersion(getReferredVariable(assignOp->get_lhs_operand()).second);

				SgExpression* forwardExp = SageInterface::copyExpression(assignOp);
				EvaluationResult reversalInfo(newVarTable);
				ExpressionReversal reversalResult(forwardExp, reverseExpression);
				reversalInfo.setAttribute(new StoredExpressionReversal(reversalResult));
				
				vector<EvaluationResult> result;
				result.push_back(reversalInfo);
				return result;
			}
		}
	}

	return vector<EvaluationResult> ();
}

ExpressionReversal process(SgExpression* exp, const EvaluationResult& evaluationResult)
{
	StoredExpressionReversal* reversalResult = dynamic_cast<StoredExpressionReversal*>(evaluationResult.getAttribute());
	ROSE_ASSERT(reversalResult != NULL);

	return reversalResult->reversal;
}


bool AkgulStyleExpressionProcessor::handleAssignOp(SgAssignOp* assignOp, const VariableVersionTable& availableVariables, SgExpression*& reverseExpression)
{
	reverseExpression = NULL;
	ROSE_ASSERT(assignOp != NULL);
	
	//Get the variable on the left side of the assign op
	VariableRenaming::VarName destroyedVarName;
	SgExpression* destroyedVarExpression;
	tie(destroyedVarName, destroyedVarExpression) = getReferredVariable(assignOp->get_lhs_operand());
	ROSE_ASSERT(destroyedVarName != VariableRenaming::emptyName);

	VariableRenaming::NumNodeRenameEntry reachingDefs = getVariableRenaming()->getReachingDefsAtNodeForName(assignOp->get_rhs_operand(), destroyedVarName);

	vector<SgExpression*> restoredValues = restoreVariable(destroyedVarName, availableVariables, reachingDefs);
	if (!restoredValues.empty())
	{
		//Success! Let's build an assign op to restore the value
		reverseExpression = SageBuilder::buildAssignOp(SageInterface::copyExpression(destroyedVarExpression), restoredValues.front());
		return true;
	}

	return false;
}


/** Returns true if an expression calls any functions or modifies any variables. */
bool RedefineValueRestorer::isModifyingExpression(SgExpression* expr, VariableRenaming* variableRenamingAnalysis)
{
	//TODO: Make this work with functions on the whitelist that we know are side-effect free
	//e.g. abs(), cos(), pow()
	if (!NodeQuery::querySubTree(expr, V_SgFunctionCallExp).empty())
	{
		return true;
	}

	//Use the variable renaming and see if there are any defs in this expression
	if (!variableRenamingAnalysis->getDefsForSubtree(expr).empty())
	{
		return true;
	}

	return false;
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


vector<SgExpression*> RedefineValueRestorer::findVarReferences(VariableRenaming::VarName var, SgNode* root)
{
	class SearchTraversal : public AstTopDownProcessing<bool>
	{
	public:
		VariableRenaming::VarName desiredVar;
		vector<SgExpression*> result;

		virtual bool evaluateInheritedAttribute(SgNode* node, bool isParentReference)
		{
			if (isParentReference)
			{
				return true;
			}

			if (VariableRenaming::getVarName(node) == desiredVar)
			{
				ROSE_ASSERT(isSgExpression(node)); //The variable name should always be attached to an expression
				result.push_back(isSgExpression(node));
				return true;
			}
			else
			{
				return false;
			}
		}
	};

	SearchTraversal traversal;
	traversal.desiredVar = var;
	traversal.traverse(root, false);
	return traversal.result;
}

/** Implement redefine technique */
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
	SgExpression* definitionExpression;
	if (SgAssignOp* assignOp = isSgAssignOp(reachingDefinition))
	{
		//Check that this assign op is for the same variable
		ROSE_ASSERT(AkgulStyleExpressionProcessor::getReferredVariable(assignOp->get_lhs_operand()).first == destroyedVarName);

		definitionExpression = assignOp->get_rhs_operand();
	}
	else if (SgInitializedName* declaration = isSgInitializedName(reachingDefinition))
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
		else if (SgAssignInitializer* assignInit = isSgAssignInitializer(declaration->get_initializer()))
		{
			definitionExpression = assignInit->get_operand();
		}
		else
		{
			//Todo: Handle constructor initializer, aggregate initializer, etc.
			return results;
		}
	}
	else
	{
		//Todo: Handle other types of reaching definitions, in addition to initialized name and assign op?
		return results;
	}

	//Ok, so the variable was previously defined by assignment to initExpression.
	//Now we need to see if we can re-execute that expression
	if (isModifyingExpression(definitionExpression, getEventProcessor()->getVariableRenaming()))
	{
		return results;
	}

	//Ok, so the initializer expression has no side effects! We can just re-execute it to restore the variable.
	//However, the variables used in the initializer expression might have been changed between its location and
	//the current node
	VariableRenaming& variableRenamingAnalysis = *getEventProcessor()->getVariableRenaming();
	VariableRenaming::NumNodeRenameTable variablesInDefExpression = variableRenamingAnalysis.getOriginalUsesAtNode(definitionExpression);

	//Go through all the variables used in the definition expression and check if their values have changed since the def
	pair<VariableRenaming::VarName, VariableRenaming::NumNodeRenameEntry> nameDefinitionPair;
	SgExpression* definitionExpressionCopy = SageInterface::copyExpression(definitionExpression);

	foreach(nameDefinitionPair, variablesInDefExpression)
	{
		VariableRenaming::NumNodeRenameEntry varVersionAtDefinition = nameDefinitionPair.second;

		if (!availableVariables.matchesVersion(nameDefinitionPair.first, varVersionAtDefinition))
		{
			printf("Recursively restoring variable '%s' to its value at line %d.\n", VariableRenaming::keyToString(nameDefinitionPair.first).c_str(),
					definitionExpression->get_file_info()->get_line());

			//See if we can recursively restore the variable so we can re-execute the definition
			vector<SgExpression*> recursivelyRestoredValues = getEventProcessor()->restoreVariable(nameDefinitionPair.first, availableVariables, varVersionAtDefinition);
			if (!recursivelyRestoredValues.empty())
			{
				SgExpression* restoredOldValue = recursivelyRestoredValues.front();
				vector<SgExpression*> restoredVarReferences = findVarReferences(nameDefinitionPair.first, definitionExpressionCopy);

				foreach (SgExpression* restoredVarReference, restoredVarReferences)
				{
					printf("Replacing '%s' with '%s'\n", restoredVarReference->unparseToString().c_str(), restoredOldValue->unparseToString().c_str());

					//If the whole definition itself is a variable reference, eg (t = a), then we can't use SageInterface::replaceExpression
					//because the parent of the variable reference is null. Manually replace the definition expression with the restored value
					if (definitionExpressionCopy == restoredVarReference)
					{
						SageInterface::deepDelete(definitionExpressionCopy);
						definitionExpressionCopy = SageInterface::copyExpression(restoredOldValue);
						break;
					}

					SageInterface::replaceExpression(restoredVarReference, SageInterface::copyExpression(restoredOldValue));
				}

				SageInterface::deepDelete(restoredOldValue);
			}
			else
			{
				SageInterface::deepDelete(definitionExpressionCopy);
				return results;
			}
		}
	}

	//Ok, all we need to do is re-execute the original initializer
	results.push_back(definitionExpressionCopy);

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
			VariableRenaming::NumNodeRenameEntry definitionsAtUse = variableRenamingAnalysis.getReachingDefsAtNodeForName(potentialUseNode, varName);

			if (definitionsAtUse == definitions)
			{
				useSites.insert(potentialUseNode);
			}
		}
	}

	//We've collected all the use sites!
	//Print them out to check if this is right
	//Then process them to extract the variables!
	reverse_foreach (SgNode* useSite, useSites)
	{
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
			printf("Recursively restoring variable '%s' to its version in line %d\n", VariableRenaming::keyToString(lhsVar).c_str(),
				useSite->get_file_info()->get_line());
			results = getEventProcessor()->restoreVariable(lhsVar, availableVariables, lhsVersion);
			if (!results.empty())
			{
				return results;
			}
		}
	}

	return results;
}