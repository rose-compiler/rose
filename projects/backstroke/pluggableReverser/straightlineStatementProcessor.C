#include "straightlineStatementProcessor.h"

#include "utilities/CPPDefinesAndNamespaces.h"

#include "rose.h"
#include "statementProcessor.h"


vector<InstrumentedStatement> StraightlineStatementProcessor::process(const StatementPackage& stmtPkg)
{
    SgStatement* statement = stmtPkg.stmt;

	if (SgBasicBlock * basicBlock = isSgBasicBlock(statement))
	{
		return processBasicBlock(stmtPkg);
	}
	else if (SgExprStatement * expressionStatement = isSgExprStatement(statement))
	{
		return processExpressionStatement(stmtPkg);
	}
	else if (isSgReturnStmt(statement))
	{
		InstrumentedStatementVec results;
		results.push_back(InstrumentedStatement(SageInterface::copyStatement(statement), NULL, stmtPkg.var_table));
		return results;
	}

	return vector<InstrumentedStatement > ();
}


/** Process an expression statement by using the first expression handler returning a valid result. */
vector<InstrumentedStatement> StraightlineStatementProcessor::processExpressionStatement(const StatementPackage& stmtPkg)
{
    SgExprStatement* statement = isSgExprStatement(stmtPkg.stmt);
    ROSE_ASSERT(statement);
    
	vector<InstrumentedExpression> expressions = processExpression(statement->get_expression(), stmtPkg.var_table, false);

	//If none of the expression handlers could handle the code, we can't reverse it!
	ROSE_ASSERT(!expressions.empty());

	//This simple processor just takes the first valid reverse expression returned
	InstrumentedExpression& instrumentedExpression = expressions.front();

	SgStatement* forwardStatement = SageBuilder::buildExprStatement(instrumentedExpression.fwd_exp);
	SgStatement* reverseStatement = SageBuilder::buildExprStatement(instrumentedExpression.rvs_exp);

	vector<InstrumentedStatement> results;
	results.push_back(InstrumentedStatement(forwardStatement, reverseStatement, instrumentedExpression.var_table, instrumentedExpression.cost));

	return results;
}


vector<InstrumentedStatement> StraightlineStatementProcessor::processBasicBlock(const StatementPackage& stmtPkg)
{
    SgBasicBlock* basicBlock = isSgBasicBlock(stmtPkg.stmt);
    ROSE_ASSERT(basicBlock);
    
	SgBasicBlock* forwardBody = SageBuilder::buildBasicBlock();
	SgBasicBlock* reverseBody = SageBuilder::buildBasicBlock();

	vector<SgStatement*> scopeExitStores;
	vector<SgVariableDeclaration*> localVarDeclarations;

	reverse_foreach(SgStatement* s, basicBlock->get_statements())
	{
		// Put the declarations of local variables in the beginning of reverse
		// basic block and retrieve their values. Store values of all local
		// variables in the end of of forward basic block.
		// Also refer to the function "processVariableDeclaration"
		if (SgVariableDeclaration * variableDeclaration = isSgVariableDeclaration(s))
		{
			//Add a copy of the declaration to the forward body
			SgVariableDeclaration* forwardBodyDeclaration = isSgVariableDeclaration(SageInterface::copyStatement(variableDeclaration));
			forwardBody->prepend_statement(forwardBodyDeclaration);


			foreach(SgInitializedName* localVar, forwardBodyDeclaration->get_variables())
			{
				//Push(save) the variable at the bottom of the forward statement
				SgExpression* storeVarValue = pushVal(SageBuilder::buildVarRefExp(localVar, forwardBody), localVar->get_type());
				SgExprStatement* varSaveStatement = SageBuilder::buildExprStatement(storeVarValue);
				scopeExitStores.push_back(varSaveStatement);

				//In the reverse body, declare & pop the variable at the very top
				SgAssignInitializer* popInitializer = SageBuilder::buildAssignInitializer(popVal(localVar->get_type()));
				SgVariableDeclaration* reverseDeclaration = SageBuilder::buildVariableDeclaration(localVar->get_name(),
						localVar->get_type(), popInitializer);
				localVarDeclarations.push_back(reverseDeclaration);
			}

			//No need to do further processing for variable declarations
			continue;
		}

		//In this simple processor, we just take the first valid statement available
		vector<InstrumentedStatement> instrumentedStatements = processStatement(StatementPackage(s, stmtPkg.var_table));

		if (instrumentedStatements.empty())
		{
			fprintf(stderr, "Failed to process statement of type '%s' on line %d: %s\n", s->class_name().c_str(),
					s->get_file_info()->get_line(), s->unparseToString().c_str());
			exit(1);
		}

		InstrumentedStatement instrumentedStatement = instrumentedStatements.front();
		SgStatement* forwardStatement = instrumentedStatement.fwd_stmt;
		SgStatement* reverseStatement = instrumentedStatement.rvs_stmt;

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
	reverse_foreach(SgStatement* stmt, scopeExitStores)
	{
		forwardBody->append_statement(stmt);
	}

	//Restore all local variables upon returning into the scope
	reverse_foreach(SgStatement* stmt, localVarDeclarations)
	{
		reverseBody->prepend_statement(stmt);
	}

	InstrumentedStatement result(forwardBody, reverseBody, stmtPkg.var_table);

	vector<InstrumentedStatement> out;
	out.push_back(result);
	return out;
}
