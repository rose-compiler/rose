#include <backstroke.h>
#include <pluggableReverser/eventProcessor.h>
#include <pluggableReverser/expAndStmtHandlers.h>
#include <utilities/utilities.h>
#include <normalizations/expNormalization.h>
#include <boost/algorithm/string.hpp>
#include <string>
#include <boost/timer.hpp>

#include "utilities/cppDefinesAndNamespaces.h"

//The files are included from the generated code. We do this so they are used by some source file
#if 0
#include "test/rctypes.h"
#endif

using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;
using namespace BackstrokeUtility;

bool isEvent(SgFunctionDeclaration* func)
{
	string func_name = func->get_name();
	if (starts_with(func_name, "event") &&
			!ends_with(func_name, "reverse") &&
			!ends_with(func_name, "forward"))
		return true;
	return false;
}

void addHandlers(EventProcessor& event_processor)
{
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
}

void reverseEvents(SgProject* project)
{
	EventProcessor event_processor;
	addHandlers(event_processor);

	Backstroke::reverseEvents(&event_processor, isEvent, project);
	//return;

#if 0
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
		if (isEvent(decl))
		{
			//Normalize this event function.
			BackstrokeNorm::normalizeEvent(decl);
			cout << "Function " << decl->get_name().str() << " is normalized!\n" << endl;
		}
	}

	VariableRenaming var_renaming(project);
	var_renaming.run();


	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		if (!isEvent(decl))
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
	foreach(SgVariableDeclaration* decl, event_processor.getAllStackDeclarations())
	{
		prependStatement(decl);
	}

	popScopeStack();

	// Fix all variable references here.
	fixVariableReferences(global);
	AstTests::runAllTests(project);
	cout << "Test Done!\n";
#endif
}


int main(int argc, char * argv[])
{
	SgProject* project = frontend(argc, argv);
	reverseEvents(project);
	return backend(project);
}
