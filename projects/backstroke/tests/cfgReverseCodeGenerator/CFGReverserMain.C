#include "rose.h"
#include <VariableRenaming.h>
#include "utilities/CPPDefinesAndNamespaces.h"
#include "normalizations/expNormalization.h"
#include "pluggableReverser/eventHandler.h"
#include "pluggableReverser/expressionHandler.h"
#include "pluggableReverser/statementHandler.h"
#include "pluggableReverser/straightlineStatementHandler.h"
#include "pluggableReverser/akgulStyleExpressionHandler.h"
#include "pluggableReverser/returnStatementHandler.h"
#include "pluggableReverser/variableDeclarationHandler.h"
#include "pluggableReverser/redefineValueRestorer.h"
#include "pluggableReverser/extractFromUseValueRestorer.h"


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
	EventHandler event_handler(NULL, &var_renaming);

	//Add the handlers in order of priority. The lower ones will be used only if higher ones do not produce results
	//Expression handlers:
	event_handler.addExpressionHandler(new IdentityExpressionHandler);
	event_handler.addExpressionHandler(new AkgulStyleExpressionHandler);
	event_handler.addExpressionHandler(new StoreAndRestoreExpressionHandler);

	//Statement handler
	event_handler.addStatementHandler(new ReturnStatementHandler);
	event_handler.addStatementHandler(new VariableDeclarationHandler);
	event_handler.addStatementHandler(new StraightlineStatementHandler);
	event_handler.addStatementHandler(new NullStatementHandler);

	//Variable value extraction handlers
	event_handler.addVariableValueRestorer(new RedefineValueRestorer);
	event_handler.addVariableValueRestorer(new ExtractFromUseValueRestorer);

	//Call the reverser and get the results
	SageBuilder::pushScopeStack(globalScope);
	vector<FuncDeclPair> forwardReversePairs = event_handler.processEvent(functionDeclaration);
	vector<SgVariableDeclaration*> generatedVariables = event_handler.getAllStackDeclarations();

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

	//Add the header file that includes functions called by the instrumented code
	string includes = "#include \"rctypes.h\"\n";
	SageInterface::addTextForUnparser(globalScope, includes, AstUnparseAttribute::e_before);

	//Unparse
	SageInterface::fixVariableReferences(globalScope);
	AstTests::runAllTests(project);
	return backend(project);
}
