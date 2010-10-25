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

vector<SgFunctionDeclaration*> normalizeEvents(
	boost::function<bool(SgFunctionDeclaration*)> is_event,
	SgProject* project)
{
	vector<SgFunctionDeclaration*> normalized_decls;
	// Get the global scope.
	SgGlobal* global = getFirstGlobalScope(project);
	
	// Get every function declaration and identify if it's an event function.
	vector<SgFunctionDeclaration*> func_decls = BackstrokeUtility::querySubTree<SgFunctionDeclaration > (global);
	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		if (is_event(decl))
		{
			//Normalize this event function.
			SgFunctionDeclaration* decl_normalized = copyStatement(decl);
			BackstrokeNorm::normalizeEvent(decl_normalized);
			decl_normalized->set_name(decl->get_name().str() + "_normalized");
			
			insertStatementAfter(decl, decl_normalized);
			normalized_decls.push_back(decl_normalized);
			cout << "Function " << decl->get_name().str() << " is normalized!\n" << endl;
		}
	}

	return normalized_decls;
}


map<SgFunctionDeclaration*, FuncDeclPairs>
reverseEvents(EventProcessor* event_processor,
		boost::function<bool(SgFunctionDeclaration*)> is_event,
		SgProject* project)
{
	ROSE_ASSERT(project);
	// Get the global scope.
	SgGlobal* global = getFirstGlobalScope(project);

	//generateGraphOfAST(project,"graph");
	//generateWholeGraphOfAST("graph");

	// Normalize all events then reverse them.
	vector<SgFunctionDeclaration*> normalized_events = normalizeEvents(is_event, project);

	VariableRenaming var_renaming(project);
	var_renaming.run();

	// Make sure a VariableRenaming object is set for out event processor.
	event_processor->setVariableRenaming(&var_renaming);

	pushScopeStack(isSgScopeStatement(global));
	
	map<SgFunctionDeclaration*, FuncDeclPairs> output;

#if 1
	vector<SgFunctionDeclaration*> func_decls = BackstrokeUtility::querySubTree<SgFunctionDeclaration > (global);
	foreach(SgFunctionDeclaration* decl, func_decls)
	{
		if (!is_event(decl))
			continue;

		timer t;

		//cout << decl->get_definition()->get_body()->get_statements().size() << endl;
		// Here reverse the event function into several versions.
		output[decl] = event_processor->processEvent(decl);

		cout << "Time used: " << t.elapsed() << endl;
		cout << "Event \"" << get_name(decl) << "\" is processed successfully!\n";

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
		prependStatement(decl, global);
	}

	// Prepend includes to test files.
	insertHeader("rctypes.h");

	popScopeStack();

	// Fix all variable references here.
	fixVariableReferences(global);
#endif

	return output;
}


} // namespace Backstroke