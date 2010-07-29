#include "rose.h"
#include <stdio.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <VariableRenaming.h>
#include "utilities/CPPDefinesAndNamespaces.h"
#include "normalizations/expNormalization.h"
#include "pluggableReverser/eventProcessor.h"
#include "pluggableReverser/expressionProcessor.h"
#include "pluggableReverser/statementProcessor.h"
#include "pluggableReverser/straightlineStatementProcessor.h"


int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	AstTests::runAllTests(project);

	//Find the function in global scope called "reverseMe"
	SgScopeStatement* globalScope = isSgScopeStatement(SageInterface::getFirstGlobalScope(project));
	SgFunctionSymbol* functionSymbol = globalScope->lookup_function_symbol("reverseMe");
	if (functionSymbol == NULL)
	{
		fprintf(stderr, "Please provide a function in global scope with the name \"reverseMe\"\n");
		exit(1);
	}
	SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
	functionDeclaration = isSgFunctionDeclaration(functionDeclaration->get_definingDeclaration());
	ROSE_ASSERT(functionDeclaration != NULL);

	//Normalize the function
	backstroke_norm::normalizeEvent(functionDeclaration);

	//Create a reverser for this function
    VariableRenaming var_renaming(project);
    var_renaming.run();
	EventProcessor event_processor(NULL, &var_renaming);
	
    event_processor.addExpressionProcessor(new StoreAndRestoreExpressionProcessor);
    event_processor.addExpressionProcessor(new ConstructiveExpressionProcessor);
    event_processor.addExpressionProcessor(new ConstructiveAssignmentProcessor);
	event_processor.addStatementProcessor(new StraightlineStatementProcessor);

	//Call the reverser and get the results
	SageBuilder::pushScopeStack(globalScope);
	vector<FuncDeclPair> forwardReversePairs = event_processor.processEvent(functionDeclaration);
	vector<SgVariableDeclaration*> generatedVariables = event_processor.getAllStackDeclarations();
	//vector<SgAssignOp*> generatedVariableInitializations = reverser.getVarInitializers();

	//Insert all the generated functions right after the original function
	foreach(FuncDeclPair originalAndInstrumented, forwardReversePairs)
	{
		SgFunctionDeclaration* forward = originalAndInstrumented.first;
		SgFunctionDeclaration* reverse = originalAndInstrumented.second;
		SageInterface::insertStatementAfter(functionDeclaration, reverse);
		SageInterface::insertStatementAfter(functionDeclaration, forward);
	}

	//Insert all the necessary variable declarations
	foreach(SgVariableDeclaration* var, generatedVariables)
	{
		SageInterface::prependStatement(var, globalScope);
	}

	//Find main and insert the variable initializations in it
	/*SgFunctionDeclaration* mainDeclaration = SageInterface::findMain(project);
	mainDeclaration = isSgFunctionDeclaration(mainDeclaration->get_definingDeclaration());
	ROSE_ASSERT(mainDeclaration != NULL);
	SgFunctionDefinition* mainDefinition = mainDeclaration->get_definition();

	reverse_foreach(SgAssignOp* varInitOp, generatedVariableInitializations)
	{
		SageInterface::prependStatement(SageBuilder::buildExprStatement(varInitOp), mainDefinition->get_body());
	}*/

	//Add the header file that includes functions called by the instrumented code
	string includes = "#include \"rctypes.h\"\n";
	SageInterface::addTextForUnparser(globalScope, includes, AstUnparseAttribute::e_before);

	//Unparse
	SageInterface::fixVariableReferences(globalScope);
	AstTests::runAllTests(project);
	backend(project);
}
