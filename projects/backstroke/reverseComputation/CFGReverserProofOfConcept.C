#include "CFGReverserProofOfConcept.h"
#include "utilities/CPPDefinesAndNamespaces.h"
#include "utilities/Utilities.h"


/** Initialize the reverser for a given AST. */
CFGReverserProofofConcept::CFGReverserProofofConcept(SgProject* project) : defUseAnalysis(project)
{
	defUseAnalysis.run();
}


/**
 * @param body function body to reverse
 * @return first result is the forward body; second result is the reverse body
 */
tuple<SgBasicBlock*, SgBasicBlock*> CFGReverserProofofConcept::ReverseFunctionBody(SgBasicBlock* body)
{
	//We just assume all the statements are basic statements - no loop handling, conditional handling, etc
	reverse_foreach(SgStatement* statement, body->get_statements())
	{
		if (isSgForStatement(statement) || isSgWhileStmt(statement) || isSgDoWhileStmt(statement))
		{
			//Loops not supported yet!
			ROSE_ASSERT(false);
		}
		else if (isSgIfStmt(statement))
		{
			//Conditionals not supported yet
			ROSE_ASSERT(false);
		}
		else if (isSgExprStatement(statement))
		{
			//This is ok. Currently only handle AssignOp and CommaOp
			SgExpression* expression = isSgExprStatement(statement)->get_expression();
			ReverseExpression(expression);
		}

		else if (isSgReturnStmt(statement))
		{
			//We only support return statements as the very last statement of a function
			ROSE_ASSERT(body->get_statements().back() == statement);
			//Forward is the same, reverse is a no-op
		}
		else if (isSgVariableDeclaration(statement))
		{
			cout << "FOUND variable declaration\n";
		}
		else
		{
			//Whatever it is, it's not suported!
			cerr << statement->class_name() << " not supported\n";
			ROSE_ASSERT(false);
		}
	}

	tuple<SgBasicBlock*, SgBasicBlock*> result;
	return result;
}


ExpPair CFGReverserProofofConcept::ReverseExpression(SgExpression* expression)
{
	if (isSgAssignOp(expression))
	{
		SgAssignOp* assignOp = isSgAssignOp(expression);

		if (isSgVarRefExp(assignOp->get_lhs_operand()))
		{
			handleVarRef(isSgVarRefExp(assignOp->get_lhs_operand()));
		}
	}

	return EventReverser::NULL_EXP_PAIR;
}


void CFGReverserProofofConcept::handleVarRef(SgVarRefExp* varRef)
{
	ROSE_ASSERT(varRef != NULL);
	SgInitializedName* varDeclaration = varRef->get_symbol()->get_declaration();
	printf("\nGetting reaching definition for variable '%s' at line %d.\n", varDeclaration->get_qualified_name().str(),
			varRef->get_file_info()->get_line());

	vector<SgNode*> reachingDefinitions = defUseAnalysis.getDefFor(varRef, varDeclaration);

	//Right now we don't support loops and branching
	ROSE_ASSERT(reachingDefinitions.size() <= 1);

	if (reachingDefinitions.size() == 0)
	{
		printf("No reaching definitions found for \'%s\' on line %d\n", varRef->unparseToString().c_str(),
				varRef->get_file_info()->get_line());
	}
	else
	{
		SgNode* reachingDefinition = reachingDefinitions[0];

		printf("The reaching definition is %s: %s on line %d\n", reachingDefinition->class_name().c_str(),
				reachingDefinition->unparseToString().c_str(),
				reachingDefinition->get_file_info()->get_line());
	}

	//Get the uses of the overwritten variable, see if we can recover it from those uses
	vector<SgNode*> variableUses = defUseAnalysis.getUseFor(varRef, varDeclaration);
	for (vector<SgNode*>::const_iterator useIter = variableUses.begin(); useIter != variableUses.end(); useIter++)
	{
		SgNode* useNode = *useIter;
		SgStatement* enclosingStatement = SageInterface::getEnclosingStatement(useNode);
		printf("Use for %s found on line %d: %s\n", varDeclaration->get_qualified_name().str(),
				useNode->get_file_info()->get_line(), enclosingStatement->unparseToString().c_str());
	}
}
