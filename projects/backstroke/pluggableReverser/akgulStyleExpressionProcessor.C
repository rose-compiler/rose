#include "akgulStyleExpressionProcessor.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "utilities/Utilities.h"

#include <numeric>

/** Initialize the reverser for a given AST. */
AkgulStyleExpressionProcessor::AkgulStyleExpressionProcessor(SgProject* project) : variableRenamingAnalysis(project)
{
	variableRenamingAnalysis.run();
}


vector<ExpressionReversal> AkgulStyleExpressionProcessor::process(SgExpression* expression, const VariableVersionTable& var_table, bool isReverseValueUsed)
{
	if (isSgAssignOp(expression))
	{
		SgAssignOp* assignOp = isSgAssignOp(expression);

		if (backstroke_util::IsVariableReference(assignOp->get_lhs_operand()))
		{
			SgExpression* reverseExpression;
			if (handleAssignOp(assignOp, reverseExpression))
			{
				string reverseExpString = reverseExpression == NULL ? "NULL" : reverseExpression->unparseToString();
				printf("Line %d:  Reversing '%s' with the expression %s'\n\n", expression->get_file_info()->get_line(),
						expression->unparseToString().c_str(), reverseExpString.c_str());

				//Indicate in the variable version table that we have restored this variable and return
				VariableVersionTable newVarTable = var_table;
				newVarTable.reverseVersion(expression);
				SgExpression* forwardExp = SageInterface::copyExpression(assignOp);
				vector<ExpressionReversal> result;
				result.push_back(ExpressionReversal(forwardExp, reverseExpression, newVarTable));
				return result;
			}
		}
	}

	return vector<ExpressionReversal > ();
}


bool AkgulStyleExpressionProcessor::handleAssignOp(SgAssignOp* assignOp, SgExpression*& reverseExpression)
{
	reverseExpression = NULL;
	ROSE_ASSERT(assignOp != NULL);
	
	//Get the variable on the left side of the assign op
	VariableRenaming::VarName destroyedVarName;
	SgExpression* destroyedVarExpression;
	tie(destroyedVarName, destroyedVarExpression) = getReferredVariable(assignOp->get_lhs_operand());
	ROSE_ASSERT(destroyedVarName != VariableRenaming::emptyName);

	VariableRenaming::NumNodeRenameEntry reachingDefs = variableRenamingAnalysis.getReachingDefsAtNodeForName(assignOp->get_rhs_operand(), destroyedVarName);

	SgExpression* restoredValue = NULL;
	if (restoreVariable(destroyedVarName, assignOp, reachingDefs, restoredValue))
	{
		//Success! Let's build an assign op to restore the value
		if (restoredValue != NULL)
		{
			reverseExpression = SageBuilder::buildAssignOp(SageInterface::copyExpression(destroyedVarExpression), restoredValue);
		}
		return true;
	}

	return false;
}


/**
 *
 * @param variable name of the variable to be restored
 * @param referenceExpression an expression that can be used in the AST to refer to that variable
 * @param useSite location where the reverse expression will go
 * @param definitions the desired version of the variable
 * @param reverseExpressions side-effect free expression that evaluates to the desired version of the given variable
 * @return true on success, false on failure
 */
bool AkgulStyleExpressionProcessor::restoreVariable(VariableRenaming::VarName variable, SgNode* useSite,
	VariableRenaming::NumNodeRenameEntry definitions, SgExpression*& reverseExpression)
{
	//First, check if the variable needs restoring at all. If it has the desired version, there is nothing to do
	if (definitions == variableRenamingAnalysis.getReachingDefsAtNodeForName(useSite, variable))
	{
		reverseExpression = VariableRenaming::buildVariableReference(variable);
		return true;
	}

	if (extractFromUse(variable, useSite, definitions, reverseExpression))
	{
		return true;
	}
	else if (useReachingDefinition(variable, useSite, definitions, reverseExpression))
	{
		return true;
	}


	return false;
}


/** Given a variable reference and its previous definition node, produces and expression that
 * evaluates to the value of the variable by re-executing its definition.
 *
 * @param varRef reference to the variable being restored
 * @param reachingDefinition reaching definition which should be reexecuted
 * @param reverseExpressions expressions that should be executed to restore the value of the variable
 * @return  true on success, false on failure
 */
bool AkgulStyleExpressionProcessor::useReachingDefinition(VariableRenaming::VarName destroyedVarName,
			SgNode* useSite, VariableRenaming::NumNodeRenameEntry definitions, SgExpression*& reverseExpression)
{
	reverseExpression = NULL;

	if (definitions.size() > 1)
	{
		//TODO: We don't support resolving multiple definitions yet.
		return false;
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
		ROSE_ASSERT(getReferredVariable(assignOp->get_lhs_operand()).first == destroyedVarName);

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
				return false;
			}

			reverseExpression = variableRenamingAnalysis.buildVariableReference(destroyedVarName);
			return true;
		}
		else if (SgAssignInitializer* assignInit = isSgAssignInitializer(declaration->get_initializer()))
		{
			definitionExpression = assignInit->get_operand();
		}
		else
		{
			//Todo: Handle constructor initializer, aggregate initializer, etc.
			return false;
		}
	}
	else
	{
		//Todo: Handle other types of reaching definitions, in addition to initialized name and assign op?
		return false;
	}

	//Ok, so the variable was previously defined by assignment to initExpression.
	//Now we need to see if we can re-execute that expression
	if (isModifyingExpression(definitionExpression))
	{
		return false;
	}

	//Ok, so the initializer expression has no side effects! We can just re-execute it to restore the variable.
	//However, the variables used in the initializer expression might have been changed between its location and
	//the current node
	VariableRenaming::NumNodeRenameTable variablesInDefExpression = variableRenamingAnalysis.getUsesAtNode(definitionExpression);

	//Go through all the variables used in the definition expression and check if their values have changed since the def
	pair<VariableRenaming::VarName, VariableRenaming::NumNodeRenameEntry> nameDefinitionPair;
	SgExpression* definitionExpressionCopy = SageInterface::copyExpression(definitionExpression);

	foreach(nameDefinitionPair, variablesInDefExpression)
	{
		VariableRenaming::NumNodeRenameEntry varVersionAtDefinition = nameDefinitionPair.second;
		VariableRenaming::NumNodeRenameEntry varVersionAtDestroySite = variableRenamingAnalysis.getReachingDefsAtNodeForName(useSite, nameDefinitionPair.first);

		if (varVersionAtDefinition != varVersionAtDestroySite)
		{
			printf("Recursively restoring variable '%s' to its value at line %d.\n", VariableRenaming::keyToString(nameDefinitionPair.first).c_str(),
					definitionExpression->get_file_info()->get_line());

			//See if we can recursively restore the variable so we can re-execute the definition
			SgExpression* restoredOldValue;
			if (restoreVariable(nameDefinitionPair.first, useSite, varVersionAtDefinition, restoredOldValue))
			{
				vector<SgExpression*> restoredVarReferences = findVarReferences(nameDefinitionPair.first, definitionExpressionCopy);

				foreach (SgExpression* restoredVarReference, restoredVarReferences)
				{
					printf("Replacing '%s' with '%s'\n", restoredVarReference->unparseToString().c_str(), restoredOldValue->unparseToString().c_str());

					//If the whole definition itself is a variable reference, eg (t = a), then we can't use SageInterface::replaceExpression
					//because the parent of the variable reference is null. Manually replace the definition expression with the restored value
					if (definitionExpressionCopy == restoredVarReference)
					{
						delete definitionExpressionCopy;
						definitionExpressionCopy = SageInterface::copyExpression(restoredOldValue);
						break;
					}

					SageInterface::replaceExpression(restoredVarReference, SageInterface::copyExpression(restoredOldValue));
				}

				delete restoredOldValue;
			}
			else
			{
				SageInterface::deepDelete(definitionExpressionCopy);
				return false;
			}
		}
	}

	//Ok, all we need to do is re-execute the original initializer
	reverseExpression = definitionExpressionCopy;

	return true;
}

/** Returns true if an expression calls any functions or modifies any variables. */
bool AkgulStyleExpressionProcessor::isModifyingExpression(SgExpression* expr)
{
	//TODO: Make this work with functions on the whitelist that we know are side-effect free
	//e.g. abs(), cos(), pow()
	if (!NodeQuery::querySubTree(expr, V_SgFunctionCallExp).empty())
	{
		return true;
	}

	//Use the variable renaming and see if there are any defs in this expression
	if (!variableRenamingAnalysis.getDefsForSubtree(expr).empty())
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

	return pair<VariableRenaming::VarName, SgExpression*>(variableRenamingAnalysis.getVarName(exp), exp);
}


bool AkgulStyleExpressionProcessor::extractFromUse(VariableRenaming::VarName varName, SgNode* destroySite, VariableRenaming::NumNodeRenameEntry defintions, SgExpression*& reverseExpression)
{
	reverseExpression = NULL;

	set<SgNode*> useSites;

	pair<int, SgNode*> versionDefPair;
	foreach(versionDefPair, defintions)
	{
		//Get all the uses of this version
		vector<SgNode*> usesForVersion = variableRenamingAnalysis.getAllUsesForDef(varName, versionDefPair.first);

		//There might be multiple versions we require. Only add the uses that have all the desired versions
		foreach(SgNode* potentialUseNode, usesForVersion)
		{
			VariableRenaming::NumNodeRenameEntry definitionsAtUse = variableRenamingAnalysis.getReachingDefsAtNodeForName(potentialUseNode, varName);

			if (definitionsAtUse == defintions)
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
		printf("Use for '%s' on line %d: %s: %s\n", VariableRenaming::keyToString(varName).c_str(), useSite->get_file_info()->get_line(),
				useSite->class_name().c_str(), useSite->unparseToString().c_str());

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

			//Ok, restore the lhs variable to the version used in the assignment.
			//Then, that expression will hold the desired value of x
			VariableRenaming::NumNodeRenameEntry lhsVersion = variableRenamingAnalysis.getReachingDefsAtNodeForName(useSite, lhsVar);
			if (restoreVariable(lhsVar, destroySite, lhsVersion, reverseExpression))
			{
				return true;
			}
		}
	}

	return false;
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
	traversal.variableRenamingAnalysis = &variableRenamingAnalysis;
	traversal.desiredVar = name;
	traversal.traverse(node);

	return traversal.result;
}


vector<SgExpression*> AkgulStyleExpressionProcessor::findVarReferences(VariableRenaming::VarName var, SgNode* root)
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
