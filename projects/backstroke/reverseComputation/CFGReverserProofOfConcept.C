#include "CFGReverserProofOfConcept.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "utilities/Utilities.h"

#include <numeric>

/** Initialize the reverser for a given AST. */
CFGReverserProofofConcept::CFGReverserProofofConcept(SgProject* project) : variableRenamingAnalysis(project)
{
	variableRenamingAnalysis.run();
}

ExpPair CFGReverserProofofConcept::ReverseExpression(SgExpression* expression)
{
	if (isSgAssignOp(expression))
	{
		SgAssignOp* assignOp = isSgAssignOp(expression);

		if (backstroke_util::IsVariableReference(assignOp->get_lhs_operand()))
		{
			SgExpression* reverseExpression;
			bool success = handleAssignOp(assignOp, reverseExpression);
			if (!success)
			{
				return NULL_EXP_PAIR;
			}
			
			string reverseExpString = reverseExpression == NULL ? "NULL" : reverseExpression->unparseToString();
			printf("Line %d:  Reversing '%s' with the expression %s'\n\n", expression->get_file_info()->get_line(),
					expression->unparseToString().c_str(), reverseExpString.c_str());

			return ExpPair(SageInterface::copyExpression(assignOp), reverseExpression);
		}
	}

	return NULL_EXP_PAIR;
}


bool CFGReverserProofofConcept::handleAssignOp(SgAssignOp* assignOp, SgExpression*& reverseExpression)
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
bool CFGReverserProofofConcept::restoreVariable(VariableRenaming::VarName variable, SgNode* useSite,
	VariableRenaming::NumNodeRenameEntry definitions, SgExpression*& reverseExpression)
{
	//Try the redefine technique
	if (useReachingDefinition(variable, useSite, definitions, reverseExpression))
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
bool CFGReverserProofofConcept::useReachingDefinition(VariableRenaming::VarName destroyedVarName,
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
			//FIXME: What is the initializer is inside a class declaration? The class might have a constructor...
			reverseExpression = buildVariableReference(destroyedVarName);
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
			//See if we can recursively restore the variable so we can re-execute the definition
			SgExpression* restoredOldValue;
			if (restoreVariable(nameDefinitionPair.first, definitionExpression, varVersionAtDefinition, restoredOldValue))
			{
				vector<SgExpression*> restoredVarReferences = findVarReferences(nameDefinitionPair.first, definitionExpressionCopy);

				foreach (SgExpression* restoredVarReference, restoredVarReferences)
				{
					printf("Replacing '%s' with '%s'\n", restoredVarReference->unparseToString().c_str(), restoredOldValue->unparseToString().c_str());
					SageInterface::replaceExpression(restoredVarReference, SageInterface::copyExpression(restoredOldValue));
				}

				delete restoredOldValue;
			}
			else
			{
				//Should I use deepdelete?
				delete definitionExpressionCopy;
				return false;
			}
		}
	}

	//Ok, all we need to do is re-execute the original initializer
	reverseExpression = definitionExpressionCopy;

	return true;
}

/** Returns true if an expression calls any functions or modifies any variables. */
bool CFGReverserProofofConcept::isModifyingExpression(SgExpression* expr)
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
pair<VariableRenaming::VarName, SgExpression*> CFGReverserProofofConcept::getReferredVariable(SgExpression* exp)
{
	if (SgCommaOpExp* commaOp = isSgCommaOpExp(exp))
	{
		return getReferredVariable(commaOp->get_rhs_operand());
	}

	return pair<VariableRenaming::VarName, SgExpression*>(variableRenamingAnalysis.getVarName(exp), exp);
}

multimap<int, SgExpression*> CFGReverserProofofConcept::collectUsesForVariable(VariableRenaming::VarName name, SgNode* node)
{
	class CollectUses : public AstBottomUpProcessing<bool>
	{
	public:
		multimap<int, SgExpression*> result;
		VariableRenaming* variableRenamingAnalysis;
		VariableRenaming::VarName desiredVar;

		virtual bool evaluateSynthesizedAttribute(SgNode* astNode, SynthesizedAttributesList childAttributes)
		{
			bool childrenHaveDefs = accumulate(childAttributes.begin(), childAttributes.end(), false, logical_or<bool>());
			if (childrenHaveDefs)
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

SgExpression* CFGReverserProofofConcept::buildVariableReference(VariableRenaming::VarName var, SgScopeStatement* scope)
{
	ROSE_ASSERT(var.size() > 0);

	SgExpression* varsSoFar = SageBuilder::buildVarRefExp(var.front(), scope);

	for (size_t i = 0; i < var.size(); i++)
	{
		SgInitializedName* initName = var[i];
		if (initName == var.back())
		{
			break;
		}

		SgVarRefExp* nextVar = SageBuilder::buildVarRefExp(var[i+1], scope);

		if (isSgPointerType(initName->get_type()))
		{
			varsSoFar = SageBuilder::buildArrowExp(varsSoFar, nextVar);
		}
		else
		{
			varsSoFar = SageBuilder::buildDotExp(varsSoFar, nextVar);
		}
	}

	return varsSoFar;
}

vector<SgExpression*> CFGReverserProofofConcept::findVarReferences(VariableRenaming::VarName var, SgNode* root)
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
