#include "SgVariableDeclaration_Handler.h"
#include <rose.h>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

using namespace std;

StatementReversal SgVariableDeclaration_Handler::generateReverseAST(SgStatement* stmt, const EvaluationResult& evaluationResult)
{
  cerr<<"DEBUG: VarDecl_Handler::genRevAST"<<endl;
	ROSE_ASSERT(evaluationResult.getStatementHandler() == this && evaluationResult.getChildResults().size() <= 1);

	SgVariableDeclaration* varDeclaration = isSgVariableDeclaration(stmt);
	ROSE_ASSERT(varDeclaration != NULL);
	ROSE_ASSERT(varDeclaration->get_variables().size() == 1 && "Two or more variables in a variable declaration not supported yet");

	//If the variable initializer has no side effects, the reverse is null and the forward is just a copy of the declaration
	SgStatement* reverseStatement = NULL;
	SgVariableDeclaration* forwardStatement = isSgVariableDeclaration(SageInterface::copyStatement(stmt));
	ROSE_ASSERT(forwardStatement != NULL);
	
	//If the initializer of the variable had side effects, we should reverse them
	if (evaluationResult.getChildResults().size() == 1)
	{
		ExpressionReversal initializerReversal = evaluationResult.getChildResults().front().generateReverseExpression();
		if (initializerReversal.reverseExpression != NULL)
		{
			reverseStatement = SageBuilder::buildExprStatement(initializerReversal.reverseExpression);
			
			//Create a new version of the initializer, possibly instrumented with state saving
			SgExpression* originalInitializer = forwardStatement->get_variables().front()->get_initializer();
			ROSE_ASSERT(isSgAssignInitializer(originalInitializer) && "Only assign initializers currently supported");
			SgExpression* newInitializer = SageBuilder::buildAssignInitializer(initializerReversal.forwardExpression,
					forwardStatement->get_variables().front()->get_type());

			//Replace the original initializer with the new one
			SageInterface::replaceExpression(originalInitializer, newInitializer);
		}
	}

	// Remove the attached preprocessing info from this statement. This is to prevent the following case:
	//  #if 1
	//  int i;
	//  ...
	//  #endif
	// where we don't want to copy "#if 1" which may lead to error.
	forwardStatement->set_attachedPreprocessingInfoPtr(NULL);

	return StatementReversal(forwardStatement, reverseStatement);
}

EvaluationResult SgVariableDeclaration_Handler::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
	SgVariableDeclaration* varDeclaration = isSgVariableDeclaration(stmt);
	if (varDeclaration == NULL)
	{
		return EvaluationResult();
	}

	const SgInitializedNamePtrList & variables = varDeclaration->get_variables();
	if (variables.size() != 1)
	{
		fprintf(stderr, "The case in which multiple variables are in the same SgVariableDeclaration is not handled here. Fix!\n");
		exit(1);
	}

	//If the initializer of the variable has side effects, we should insert its reverse in the reverse code
	EvaluationResult variableDeclReversal;
	SgInitializer* initializer = variables[0]->get_initializer();
	if (initializer == NULL)
	{
		variableDeclReversal = EvaluationResult(this, stmt, var_table);
	}
	else if (SgAssignInitializer* assignInit = isSgAssignInitializer(initializer))
	{
		SgExpression* initializerExpression = assignInit->get_operand();
		EvaluationResult initializerReversal = evaluateExpression(initializerExpression, var_table, false);
		ROSE_ASSERT(initializerReversal.isValid());
		
		EvaluationResult declarationReversal(this, stmt, var_table);
		declarationReversal.addChildEvaluationResult(initializerReversal);
		variableDeclReversal = declarationReversal;
	}
	else if (SgConstructorInitializer* constructorInit = isSgConstructorInitializer(initializer))
	{
		ROSE_ASSERT(constructorInit && "Constructor initializer not handled yet. This is a type of function call");
	}
	else if (isSgAggregateInitializer(initializer))
	{
		printf("Aggregate initializer not supported yet\n");
		ROSE_ASSERT(false);
	}
	else
	{
		printf("Encountered unknown initializer type.");
		ROSE_ASSERT(false);
	}

	return variableDeclReversal;
}
