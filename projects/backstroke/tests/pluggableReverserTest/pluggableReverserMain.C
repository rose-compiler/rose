#include <pluggableReverser/eventProcessor.h>
#include <pluggableReverser/expAndStmtHandlers.h>
#include <utilities/utilities.h>
#include <normalizations/expNormalization.h>
#include <boost/algorithm/string.hpp>
#include <string>
#include <boost/timer.hpp>

#include "utilities/cppDefinesAndNamespaces.h"


using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;
using namespace BackstrokeUtility;


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
	vector<SgFunctionDeclaration*> func_decls = BackstrokeUtility::querySubTree<SgFunctionDeclaration > (global);
	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		string func_name = decl->get_name();
		if (!starts_with(func_name, "event") ||	ends_with(func_name, "reverse") || ends_with(func_name, "forward"))
			continue;

		//Normalize this event function.
		cout << "Function " << decl->get_name().str() << " is normalized!\n" << endl;
		BackstrokeNorm::normalizeEvent(decl);
	}

	VariableRenaming var_renaming(project);
	var_renaming.run();

	EventProcessor event_processor(NULL, &var_renaming);

	// Add all expression handlers to the expression pool.
	//event_processor.addExpressionHandler(new NullExpressionHandler);
	event_processor.addExpressionHandler(new IdentityExpressionHandler);
	event_processor.addExpressionHandler(new StoreAndRestoreExpressionHandler);
	//event_processor.addExpressionHandler(new AkgulStyleExpressionHandler);

	// Add all statement handlers to the statement pool.
	event_processor.addStatementHandler(new CombinatorialExprStatementHandler);
	event_processor.addStatementHandler(new VariableDeclarationHandler);
	event_processor.addStatementHandler(new CombinatorialBasicBlockHandler);
	event_processor.addStatementHandler(new IfStatementHandler);
	event_processor.addStatementHandler(new WhileStatementHandler);
	event_processor.addStatementHandler(new ReturnStatementHandler);
	event_processor.addStatementHandler(new StateSavingStatementHandler);
	//event_processor.addStatementHandler(new NullStatementHandler);

	//Variable value extraction handlers
	event_processor.addVariableValueRestorer(new RedefineValueRestorer);
	event_processor.addVariableValueRestorer(new ExtractFromUseValueRestorer);

	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		string func_name = decl->get_name();
		if (!starts_with(func_name, "event") || ends_with(func_name, "reverse") || ends_with(func_name, "forward"))
			continue;

		timer t;
		// Here reverse the event function into several versions.
		FuncDeclPairs output = event_processor.processEvent(decl);

		cout << "Time used: " << t.elapsed() << endl;
		cout << "Event is processed successfully!\n";

		reverse_foreach (FuncDeclPair& func_decl_pair, output)
		{
			// Put the generated statement after the original event.
			insertStatement(decl, func_decl_pair.second, false);
			insertStatement(decl, func_decl_pair.first, false);
		}
	}

	// Declare all stack variables on top of the generated file.
	vector<SgVariableDeclaration*> stack_decls = event_processor.getAllStackDeclarations();
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
