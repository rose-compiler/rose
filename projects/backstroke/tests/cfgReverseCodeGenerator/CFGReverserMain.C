#include "rose.h"
#include <stdio.h>

#include "reverseComputation/eventReverser.h"
#include "utilities/CPPDefinesAndNamespaces.h"


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

	//Create a reverser for this function
	EventReverser reverser(functionDeclaration, NULL);

	//Call the reverser andget the results
	map<SgFunctionDeclaration*, FuncDeclPair> originalToGenerated = reverser.outputFunctions();
	vector<SgVariableDeclaration*> generatedVariables = reverser.getVarDeclarations();
	vector<SgAssignOp*> generatedVariableInitializations = reverser.getVarInitializers();

	//Insert all the generated functions right after the original function
	pair<SgFunctionDeclaration*, FuncDeclPair> originalAndInstrumented;
	foreach(originalAndInstrumented, originalToGenerated)
	{
		SgFunctionDeclaration* originalFunction = originalAndInstrumented.first;
		SgFunctionDeclaration* forward = originalAndInstrumented.second.first;
		SgFunctionDeclaration* reverse = originalAndInstrumented.second.second;
		SageInterface::insertStatementAfter(originalFunction, forward);
		SageInterface::insertStatementAfter(originalFunction, reverse);
	}

	//Insert all the necessary variable declarations
	foreach(SgVariableDeclaration* var, generatedVariables)
	{
		SageInterface::prependStatement(var, globalScope);
	}

	//Find main and insert the variable initializations in it
	SgFunctionDeclaration* mainDeclaration = SageInterface::findMain(project);
	mainDeclaration = isSgFunctionDeclaration(mainDeclaration->get_definingDeclaration());
	ROSE_ASSERT(mainDeclaration != NULL);
	SgFunctionDefinition* mainDefinition = mainDeclaration->get_definition();

	reverse_foreach(SgAssignOp* varInitOp, generatedVariableInitializations)
	{
		SageInterface::prependStatement(SageBuilder::buildExprStatement(varInitOp), mainDefinition->get_body());
	}

	//Unparse
	AstTests::runAllTests(project);
	backend(project);
}
