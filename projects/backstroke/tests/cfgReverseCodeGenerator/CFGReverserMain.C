#include <backstroke.h>
#include <VariableRenaming.h>
#include "utilities/cppDefinesAndNamespaces.h"
#include "normalizations/expNormalization.h"
#include "pluggableReverser/eventProcessor.h"
#include "pluggableReverser/expAndStmtHandlers.h"

using namespace std;

struct IsEvent
{
	SgScopeStatement* globalScope;

	IsEvent(SgScopeStatement* scope)
	: globalScope(scope) {}

	bool operator() (SgFunctionDeclaration* decl)
	{
		return decl->get_symbol_from_symbol_table() ==
				globalScope->lookup_function_symbol("reverseMe");
	}
};

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	AstTests::runAllTests(project);

	EventProcessor event_processor;

	//Add the handlers in order of priority. The lower ones will be used only if higher ones do not produce results
	//Expression handlers:
	event_processor.addExpressionHandler(new IdentityExpressionHandler);
	event_processor.addExpressionHandler(new AkgulStyleExpressionHandler);
	event_processor.addExpressionHandler(new StoreAndRestoreExpressionHandler);

	//Statement handler
	event_processor.addStatementHandler(new ReturnStatementHandler);
	event_processor.addStatementHandler(new VariableDeclarationHandler);
	event_processor.addStatementHandler(new StraightlineStatementHandler);
	event_processor.addStatementHandler(new NullStatementHandler);

	//Variable value extraction handlers
	event_processor.addVariableValueRestorer(new RedefineValueRestorer);
	event_processor.addVariableValueRestorer(new ExtractFromUseValueRestorer);

	SgScopeStatement* globalScope = isSgScopeStatement(SageInterface::getFirstGlobalScope(project));
	Backstroke::reverseEvents(&event_processor, IsEvent(globalScope), project);

	AstTests::runAllTests(project);
	return backend(project);
}
