#include <pluggableReverser/eventHandler.h>
#include <pluggableReverser/expressionHandler.h>
#include <pluggableReverser/statementHandler.h>
#include <pluggableReverser/ifStatementHandler.h>
#include <pluggableReverser/whileStatementHandler.h>
#include <pluggableReverser/stateSavingStatementHandler.h>
#include <pluggableReverser/akgulStyleExpressionHandler.h>
#include "pluggableReverser/variableDeclarationHandler.h"
#include <utilities/Utilities.h>
#include <normalizations/expNormalization.h>
#include <boost/algorithm/string.hpp>
#include <string>
#include <boost/timer.hpp>

#include "utilities/CPPDefinesAndNamespaces.h"
#include "pluggableReverser/returnStatementHandler.h"
#include "pluggableReverser/akgulStyleExpressionHandler.h"
#include "pluggableReverser/redefineValueRestorer.h"
#include "pluggableReverser/extractFromUseValueRestorer.h"



using namespace SageInterface;
using namespace SageBuilder;
using namespace backstroke_util;


int main(int argc, char * argv[])
{
	vector<string> args(argv, argv + argc);
	SgProject* project = frontend(args);

	SgGlobal* global = getFirstGlobalScope(project);

	// Prepend includes to test files.
	string includes = "#include <rctypes.h>\n"
			"#include <stdio.h>\n"
			"#include <stdlib.h>\n"
			"#include <time.h>\n"
			"#include <assert.h>\n"
			"#include <memory.h>\n";
	addTextForUnparser(global, includes, AstUnparseAttribute::e_before);

	pushScopeStack(isSgScopeStatement(global));

	// Get every function declaration and identify if it's an event function.
	vector<SgFunctionDeclaration*> func_decls = backstroke_util::querySubTree<SgFunctionDeclaration > (global);
	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		string func_name = decl->get_name();
		if (!starts_with(func_name, "event") ||	ends_with(func_name, "reverse") || ends_with(func_name, "forward"))
			continue;

		//Normalize this event function.
		backstroke_norm::normalizeEvent(decl);
	}

	VariableRenaming var_renaming(project);
	var_renaming.run();

	EventHandler event_handler(NULL, &var_renaming);

	// Add all expression handlers to the expression pool.
	//event_handler.addExpressionHandler(new NullExpressionHandler);
	event_handler.addExpressionHandler(new IdentityExpressionHandler);
	event_handler.addExpressionHandler(new StoreAndRestoreExpressionHandler);
	//event_handler.addExpressionHandler(new AkgulStyleExpressionHandler);

	// Add all statement handlers to the statement pool.
	event_handler.addStatementHandler(new CombinatorialExprStatementHandler);
	event_handler.addStatementHandler(new VariableDeclarationHandler);
	event_handler.addStatementHandler(new CombinatorialBasicBlockHandler);
	event_handler.addStatementHandler(new IfStatementHandler);
	event_handler.addStatementHandler(new WhileStatementHandler);
	event_handler.addStatementHandler(new ReturnStatementHandler);
	event_handler.addStatementHandler(new StateSavingStatementHandler);
	//event_handler.addStatementHandler(new NullStatementHandler);

	//Variable value extraction handlers
	event_handler.addVariableValueRestorer(new RedefineValueRestorer);
	event_handler.addVariableValueRestorer(new ExtractFromUseValueRestorer);

	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		string func_name = decl->get_name();
		if (!starts_with(func_name, "event") || ends_with(func_name, "reverse") || ends_with(func_name, "forward"))
			continue;

		timer t;
		// Here reverse the event function into several versions.
		FuncDeclPairs output = event_handler.processEvent(decl);

		cout << "Time used: " << t.elapsed() << endl;
		cout << "Event is processed successfully!\n";

		foreach(FuncDeclPair& func_decl_pair, output)
		{
			appendStatement(func_decl_pair.first);
			appendStatement(func_decl_pair.second);
		}
	}

	// Declare all stack variables on top of the generated file.
	vector<SgVariableDeclaration*> stack_decls = event_handler.getAllStackDeclarations();
	foreach(SgVariableDeclaration* decl, stack_decls)
	{
		prependStatement(decl);
	}

	popScopeStack();

	// Fix all variable references here.
	cout << "VarRef fixed: " <<	fixVariableReferences(global) << endl;
	AstTests::runAllTests(project);
	cout << "Test Done!\n";

	return backend(project);
}
