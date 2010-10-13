#include "backstroke.h"
#include <utilities/utilities.h>
#include <pluggableReverser/eventProcessor.h>
#include <normalizations/expNormalization.h>
#include <boost/timer.hpp>


namespace Backstroke
{

using namespace std;
using namespace boost;
using namespace SageInterface;
using namespace SageBuilder;


map<SgFunctionDeclaration*, FuncDeclPairs>
reverseEvents(EventProcessor* event_processor,
		boost::function<bool(SgFunctionDeclaration*)> is_event,
		SgProject* project)
{
	// Get the global scope.
	SgGlobal* global = getFirstGlobalScope(project);

	pushScopeStack(isSgScopeStatement(global));

	// Get every function declaration and identify if it's an event function.
	vector<SgFunctionDeclaration*> func_decls = BackstrokeUtility::querySubTree<SgFunctionDeclaration > (global);
	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		if (is_event(decl))
		{
			//Normalize this event function.
			BackstrokeNorm::normalizeEvent(decl);
			cout << "Function " << decl->get_name().str() << " is normalized!\n" << endl;
		}
	}

	VariableRenaming var_renaming(project);
	var_renaming.run();

	// Make sure a VariableRenaming object is set for out event processor.
	event_processor->setVariableRenaming(&var_renaming);

	map<SgFunctionDeclaration*, FuncDeclPairs> output;
	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		if (!is_event(decl))
			continue;

		timer t;
		// Here reverse the event function into several versions.
		output[decl] = event_processor->processEvent(decl);

		cout << "Time used: " << t.elapsed() << endl;
		cout << "Event is processed successfully!\n";

		reverse_foreach (const FuncDeclPair& func_decl_pair, output[decl])
		{
			// Put the generated statement after the original event.
			insertStatementAfter(decl, func_decl_pair.second);
			insertStatementAfter(decl, func_decl_pair.first);
		}
	}

	// Declare all stack variables on top of the generated file.
	foreach(SgVariableDeclaration* decl, event_processor->getAllStackDeclarations())
	{
		prependStatement(decl);
	}

	// Prepend includes to test files.
	insertHeader("rctypes.h");

	popScopeStack();

	// Fix all variable references here.
	fixVariableReferences(global);

	return output;
}


} // namespace Backstroke