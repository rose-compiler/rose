#include "straightlineStatementHandler.h"

#include "utilities/cppDefinesAndNamespaces.h"

#include "rose.h"
#include "statementHandler.h"
#include "pluggableReverser/eventProcessor.h"

using namespace std;

vector<EvaluationResult> StraightlineStatementHandler::evaluate(SgStatement* statement, const VariableVersionTable& var_table)
{
	if (SgBasicBlock * basicBlock = isSgBasicBlock(statement))
	{
		return evaluateBasicBlock(basicBlock, var_table);
	}
	else if (SgExprStatement * expressionStatement = isSgExprStatement(statement))
	{
		return evaluateExpressionStatement(expressionStatement, var_table);
	}

	return vector<EvaluationResult> ();
}


/** Process an expression statement by using the first expression handler returning a valid result. */
vector<EvaluationResult> StraightlineStatementHandler::evaluateExpressionStatement(SgExprStatement* statement, const VariableVersionTable& var_table)
{
    ROSE_ASSERT(statement);
    
	vector<EvaluationResult> expressions = evaluateExpression(statement->get_expression(), var_table, false);

	//If none of the expression handlers could handle the code, we can't reverse it!
	ROSE_ASSERT(!expressions.empty());

	//This simple handler just takes the first valid reverse expression returned
	EvaluationResult& expressionReversalOption = expressions.front();
	ExpressionReversal expressionReversal = expressionReversalOption.generateReverseExpression();
	SgStatement* forwardStatement = NULL;
	if (expressionReversal.fwd_exp != NULL)
	{
		forwardStatement = SageBuilder::buildExprStatement(expressionReversal.fwd_exp);
	}
	SgStatement* reverseStatement = NULL;
	if (expressionReversal.rvs_exp != NULL)
	{
		reverseStatement = SageBuilder::buildExprStatement(expressionReversal.rvs_exp);
	}

	//We just do all the work in the evaluation step and save it as an attribute
	EvaluationResult statementResult(this, statement, expressionReversalOption.getVarTable(), expressionReversalOption.getCost());
	StatementReversal statementReversal(forwardStatement, reverseStatement);
	//statementResult.setAttribute(EvaluationResultAttributePtr(new StoredStatementReversal(statementReversal)));
	statementResult.setAttribute(statementReversal);

	vector<EvaluationResult> result;
	result.push_back(statementResult);

	return result;
}

StatementReversal StraightlineStatementHandler::generateReverseAST(SgStatement* statement, const EvaluationResult& reversal)
{
	ROSE_ASSERT(reversal.getStatementHandler() == this);
	return reversal.getAttribute<StatementReversal>();
}


vector<EvaluationResult> StraightlineStatementHandler::evaluateBasicBlock(SgBasicBlock* basicBlock, const VariableVersionTable& var_table)
{
    ROSE_ASSERT(basicBlock);
    
	SgBasicBlock* forwardBody = SageBuilder::buildBasicBlock();
	SgBasicBlock* reverseBody = SageBuilder::buildBasicBlock();

	vector<SgStatement*> scopeExitStores;
	vector<SgVariableDeclaration*> localVarDeclarations;

	VariableVersionTable currentVariableVersions = var_table;
	SimpleCostModel totalCost;

	//First pass: handle declarations of local variables
	foreach(SgStatement* statement, basicBlock->get_statements())
	{
		// Put the declarations of local variables in the beginning of reverse
		// basic block and retrieve their values. Store values of all local
		// variables in the end of of forward basic block.
		// Also refer to the function "processVariableDeclaration"
		if (SgVariableDeclaration * variableDeclaration = isSgVariableDeclaration(statement))
		{
			foreach(SgInitializedName* localVar, variableDeclaration->get_variables())
			{
				//First, check if we can restore the variable without savings its value
				VariableRenaming::VarName varName;
				varName.push_back(localVar);
				SgFunctionDefinition* enclosingFunction = SageInterface::getEnclosingFunctionDefinition(basicBlock);
				VariableRenaming::NumNodeRenameEntry definitions = getVariableRenaming()->getReachingDefsAtFunctionEndForName(enclosingFunction, varName);

				SgExpression* restoredValue = restoreVariable(varName, currentVariableVersions, definitions);
				SgAssignInitializer* reverseVarInitializer;
				if (restoredValue != NULL)
				{
					reverseVarInitializer = SageBuilder::buildAssignInitializer(restoredValue);
				}
				else
				{
					//Push(save) the variable at the bottom of the forward statement
					SgExpression* storeVarValue = pushVal(SageBuilder::buildVarRefExp(localVar, forwardBody), localVar->get_type());
					SgExprStatement* varSaveStatement = SageBuilder::buildExprStatement(storeVarValue);
					scopeExitStores.push_back(varSaveStatement);

					//In the reverse body, declare & pop the variable at the very top
					reverseVarInitializer = SageBuilder::buildAssignInitializer(popVal(localVar->get_type()));

					//Note that we have stored one variable
					totalCost.increaseStoreCount(1);
				}

				SgVariableDeclaration* reverseDeclaration = SageBuilder::buildVariableDeclaration(localVar->get_name(),
						localVar->get_type(), reverseVarInitializer);
				localVarDeclarations.push_back(reverseDeclaration);

				//Update the variable version table to indicate that this variable has been restored
				currentVariableVersions.setLastVersion(localVar);
			}
		}
	}

	//Second pass: reverse all the statements
	reverse_foreach(SgStatement* s, basicBlock->get_statements())
	{
		//In this simple handler, we just take the first valid statement available
		vector<EvaluationResult> possibleStatements = evaluateStatement(s, currentVariableVersions);
		if (possibleStatements.empty())
		{
			fprintf(stderr, "Failed to process statement of type '%s' on line %d: %s\n", s->class_name().c_str(),
					s->get_file_info()->get_line(), s->unparseToString().c_str());
			exit(1);
		}

		StatementReversal instrumentedStatement = possibleStatements.front().generateReverseStatement();
		totalCost += possibleStatements.front().getCost();
		SgStatement* forwardStatement = instrumentedStatement.fwd_stmt;
		SgStatement* reverseStatement = instrumentedStatement.rvs_stmt;
		currentVariableVersions = possibleStatements.front().getVarTable();

		//The return statement should go at the very end of the forward statement
		//after the variables that exit scope have been stored
		if (isSgReturnStmt(forwardStatement))
		{
			scopeExitStores.push_back(forwardStatement);
		}
		else if (forwardStatement != NULL)
		{
			forwardBody->prepend_statement(forwardStatement);
		}

		//Insert the reverse statement
		if (isSgReturnStmt(reverseStatement))
		{
			//The reverse of a return statement is a null statement since the return statement
			//can only come at the end of a function. Hence, this case should never occur.
			ROSE_ASSERT(false);
		}
		else if (reverseStatement != NULL)
		{
			reverseBody->append_statement(reverseStatement);
		}
	}

	//Before exiting scope, store all local variables
	foreach(SgStatement* stmt, scopeExitStores)
	{
		forwardBody->append_statement(stmt);
	}

	//Restore all local variables upon returning into the scope
	reverse_foreach(SgStatement* stmt, localVarDeclarations)
	{
		reverseBody->prepend_statement(stmt);
	}

	//We actually did both cost evaluation and code generation. Store the result as an attribute
	StatementReversal result(forwardBody, reverseBody);
	EvaluationResult costAndStuff(this, basicBlock, currentVariableVersions, totalCost);
	costAndStuff.setAttribute(result);

	vector<EvaluationResult> out;
	out.push_back(costAndStuff);
	return out;
}
