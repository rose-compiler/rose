#include "CFGReverserProofOfConcept.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "utilities/Utilities.h"


/** Initialize the reverser for a given AST. */
CFGReverserProofofConcept::CFGReverserProofofConcept(SgProject* project) : defUseAnalysis(project), 
	variableRenamingAnalysis(project)
{
	defUseAnalysis.run();
	variableRenamingAnalysis.run();
}

ExpPair CFGReverserProofofConcept::ReverseExpression(SgExpression* expression)
{
	if (isSgAssignOp(expression))
	{
		SgAssignOp* assignOp = isSgAssignOp(expression);

		if (backstroke_util::IsVariableReference(assignOp->get_lhs_operand()))
		{
			vector<SgExpression*> results;
			bool success = handleAssignOp(assignOp, results);
			if (!success)
			{
				return NULL_EXP_PAIR;
			}
			
			SgExpression* reverseExpression = NULL;


			//Build a comma op from the expresion results
			foreach (SgExpression* exp, results)
			{
				if (reverseExpression == NULL)
				{
					reverseExpression = exp;
				}
				else
				{
					reverseExpression = SageBuilder::buildCommaOpExp(reverseExpression, exp);
				}
			}

			string reverseExpString = reverseExpression == NULL ? "NULL" : reverseExpression->unparseToString();
			printf("Line %d:  Reversing '%s' with the expression %s'\n\n", expression->get_file_info()->get_line(),
					expression->unparseToString().c_str(), reverseExpString.c_str());

			return ExpPair(SageInterface::copyExpression(assignOp), reverseExpression);
		}
	}

	return NULL_EXP_PAIR;
}


bool CFGReverserProofofConcept::handleAssignOp(SgAssignOp* assignOp, std::vector<SgExpression*>& reverseExpressions)
{
	ROSE_ASSERT(assignOp != NULL);
	ROSE_ASSERT(reverseExpressions.empty());
	
	//Get the variable on the left side of the assign op
	VariableRenaming::varName destroyedVarName;
	SgExpression* destroyedVarExpression;
	tie(destroyedVarName, destroyedVarExpression) = getReferredVariable(assignOp->get_lhs_operand());
	ROSE_ASSERT(destroyedVarName != VariableRenaming::emptyName);

	printf("\nGetting reaching definition for variable '%s' at line %d.\n", variableRenamingAnalysis.keyToString(destroyedVarName).c_str(),
			assignOp->get_file_info()->get_line());

	//Retrieve the reaching definitions
	VariableRenaming::numNodeRenameEntry reachingDefs = variableRenamingAnalysis.getReachingDefsAtNodeForName(assignOp->get_lhs_operand(), destroyedVarName);

	if (reachingDefs.size() > 1)
	{
		//TODO: We don't support resolving multiple definitions yet.
		return false;
	}

	ROSE_ASSERT(reachingDefs.size() > 0 && "Why doesn't the variable have any reaching definitions?");


	pair<int, SgNode*> reachingDef = *reachingDefs.begin();
	SgNode* defNode = reachingDef.second;

	printf("The reaching definition is %s: %s on line %d\n", defNode->class_name().c_str(),
			defNode->unparseToString().c_str(),
			defNode->get_file_info()->get_line());

	//Get the uses of the overwritten variable, see if we can recover it from those uses
	/*vector<SgNode*> variableUses = defUseAnalysis.getUseFor(assignOp, varDeclaration);
	for (vector<SgNode*>::const_iterator useIter = variableUses.begin(); useIter != variableUses.end(); useIter++)
	{
		SgNode* useNode = *useIter;
		SgStatement* enclosingStatement = SageInterface::getEnclosingStatement(useNode);
		printf("Use for %s found on line %d: %s\n", varDeclaration->get_qualified_name().str(),
				useNode->get_file_info()->get_line(), enclosingStatement->unparseToString().c_str());
	}*/

	//Try the redefine technique
	if (useReachingDefinition(destroyedVarName, destroyedVarExpression, assignOp->get_rhs_operand(), defNode, reverseExpressions))
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
bool CFGReverserProofofConcept::useReachingDefinition(VariableRenaming::varName destroyedVarName, SgExpression* destroyedVarExp,
		SgNode* destroySite, SgNode* reachingDefinition, std::vector<SgExpression*>& reverseExpressions)
{
	ROSE_ASSERT(reverseExpressions.empty());

	//Find what expression was evaluated to assign the variable to its previous value
	SgExpression* definitionExpression;
	if (SgAssignOp* assignOp = isSgAssignOp(reachingDefinition))
	{
		//Check that this assign op is for the same variable
		ROSE_ASSERT(variableRenamingAnalysis.getVarName(assignOp->get_lhs_operand()) == destroyedVarName);

		definitionExpression = assignOp->get_rhs_operand();
	}
	else if (SgInitializedName* declaration = isSgInitializedName(reachingDefinition))
	{
		//If the previous definition is a declaration without an initializer, then the previous value
		//of the variable is undefined. Hence, no need to restore its value.
		if (declaration->get_initializer() == NULL)
		{
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
	VariableRenaming::numNodeRenameTable variablesInDefExpression = variableRenamingAnalysis.getUsesAtNode(definitionExpression);

	//Go through all the variables used in the definition expression and check if their values have changed since the def
	pair<VariableRenaming::varName, VariableRenaming::numNodeRenameEntry> nameDefinitionPair;
	bool variableModified = false;
	foreach (nameDefinitionPair, variablesInDefExpression)
	{
		VariableRenaming::numNodeRenameEntry varVersionAtDefinition = nameDefinitionPair.second;
		VariableRenaming::numNodeRenameEntry varVersionAtDestroySite = variableRenamingAnalysis.getReachingDefsAtNodeForName(destroySite, nameDefinitionPair.first);

		if (varVersionAtDefinition != varVersionAtDestroySite)
		{
			//Todo: use recusion here to possibly restore the modified variable
			variableModified = true;
		}
	}

	if (variableModified)
	{
		return false;
	}

	//Ok, all we need to do is re-execute the original initializer
	reverseExpressions.push_back(SageBuilder::buildAssignOp(destroyedVarExp, SageInterface::copyExpression(definitionExpression)));

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
	if (!getDefsForSubtree(variableRenamingAnalysis, expr).empty())
	{
		return true;
	}

	return false;
}


/** Returns the variable name referred by the expression. Also returns
 *  the AST expression for referring to that variable (using the variable renaming analysis).
  * Handles comma ops correctly. */
pair<VariableRenaming::varName, SgExpression*> CFGReverserProofofConcept::getReferredVariable(SgExpression* exp)
{
	if (SgCommaOpExp* commaOp = isSgCommaOpExp(exp))
	{
		return getReferredVariable(commaOp->get_rhs_operand());
	}

	return pair<VariableRenaming::varName, SgExpression*>(variableRenamingAnalysis.getVarName(exp), exp);
}


 /** Get name:num mapping for all variables defined in the given subtree.
  * Note that a variable may be renamed multiple times in the subtree
 *
 * This will return the combination of original and expanded defs on this node.
 *
 * ex. s.x = 5;  //This will return s.x and s  (s.x is original & s is expanded)
 *
 * @param root AST tree which will be searched for defs
 * @return A table mapping VarName->set(num, defNode) for every varName defined in the subtree
 */
map<VariableRenaming::varName, set<VariableRenaming::numNodeRenameEntry> > CFGReverserProofofConcept::getDefsForSubtree(VariableRenaming& varRenamingAnalysis, SgNode* root)
{
	class DefSearchTraversal : public AstSimpleProcessing
	{
	public:
		map<VariableRenaming::varName, set<VariableRenaming::numNodeRenameEntry> > result;
		VariableRenaming* varRenamingAnalysis;

		virtual void visit(SgNode* node)
		{
			//Look up defs at this particular node
			VariableRenaming::numNodeRenameTable defsAtNode = varRenamingAnalysis->getDefsAtNode(node);

			//Put them in the global table
			pair<VariableRenaming::varName, VariableRenaming::numNodeRenameEntry> varRenameEntryPair;
			foreach(varRenameEntryPair, defsAtNode)
			{
				VariableRenaming::varName variableName = varRenameEntryPair.first;
				result[variableName].insert(varRenameEntryPair.second);
			}
		}
	};

	DefSearchTraversal traversal;
	traversal.varRenamingAnalysis = &varRenamingAnalysis;
	traversal.traverse(root, preorder);

	return traversal.result;
}
