#include <backstroke.h>
#include <VariableRenaming.h>
#include "utilities/cppDefinesAndNamespaces.h"
#include "normalizations/expNormalization.h"
#include "pluggableReverser/eventProcessor.h"
#include "pluggableReverser/expAndStmtHandlers.h"

using namespace std;

struct IsEvent
{
	bool operator() (SgFunctionDeclaration* decl)
	{
		string funcName = decl->get_name();
			
		//All event handler functions are named "Handle"
		if (funcName != "invertMe")
			return false;
		
        return true;
	}
};


struct VariableReversalFilter : public IVariableFilter
{
	virtual bool isVariableInteresting(const VariableRenaming::VarName& var) const
	{
		return true;
	}
};

int main(int argc, char** argv)
{
	//Add the preinclude option
	vector<string> commandArguments(argv, argv + argc);
	commandArguments.push_back("-include");
	commandArguments.push_back("backstrokeRuntime.h");

	SgProject* project = frontend(commandArguments);
	AstTests::runAllTests(project);

	VariableReversalFilter varFilter;
	EventProcessor event_processor(&varFilter);

	//Add the handlers in order of priority. The lower ones will be used only if higher ones do not produce results
	//Expression handlers:
	event_processor.addExpressionHandler(new IdentityExpressionHandler);
	event_processor.addExpressionHandler(new StoreAndRestoreExpressionHandler);

	//Statement handler
	event_processor.addStatementHandler(new ReturnStatementHandler);
	event_processor.addStatementHandler(new VariableDeclarationHandler);
	event_processor.addStatementHandler(new StraightlineStatementHandler);
	event_processor.addStatementHandler(new NullStatementHandler);

	Backstroke::reverseEvents(&event_processor, IsEvent(), project);

	AstTests::runAllTests(project);
	return backend(project);
}
