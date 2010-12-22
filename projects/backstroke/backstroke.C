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

FuncDeclPairs normalizeEvents(
	boost::function<bool(SgFunctionDeclaration*)> is_event,
	SgProject* project)
{
	FuncDeclPairs normalized_decls;
	// Get the global scope.
	SgGlobal* global = getFirstGlobalScope(project);
	
	// Get every function declaration and identify if it's an event function.
	vector<SgFunctionDeclaration*> func_decls = BackstrokeUtility::querySubTree<SgFunctionDeclaration > (global);
	foreach (SgFunctionDeclaration* decl, func_decls)
	{
		//This ensures that we process every function only once
		if (decl != decl->get_firstNondefiningDeclaration())
			continue;

		if (is_event(decl))
		{
			//Normalize this event function.
			BackstrokeNorm::normalizeEvent(decl);
			normalized_decls.push_back(FuncDeclPair(decl, decl));
			
			cout << "Function " << decl->get_name().str() << " is normalized!\n" << endl;
		}
	}

	// Fix all variable references here.
	fixVariableReferences(global);

	return normalized_decls;
}


vector<ProcessedEvent>
reverseEvents(EventProcessor* event_processor,
		boost::function<bool(SgFunctionDeclaration*)> is_event,
		SgProject* project)
{
	ROSE_ASSERT(project);

	//generateGraphOfAST(project,"graph");
	//generateWholeGraphOfAST("graph");

	// Normalize all events then reverse them.
	FuncDeclPairs normalized_events = normalizeEvents(is_event, project);

	AstTests::runAllTests(project);
	//unparseProject(project);

	VariableRenaming var_renaming(project);
	var_renaming.run();
	// Make sure a VariableRenaming object is set for out event processor.
	event_processor->setVariableRenaming(&var_renaming);

	// Get the global scope.
	SgGlobal* globalScope = SageInterface::getFirstGlobalScope(project);
	SageBuilder::pushScopeStack(isSgScopeStatement(globalScope));

	vector<ProcessedEvent> output;

	foreach(const FuncDeclPair& event_pair, normalized_events)
	{
		timer t;

		ProcessedEvent processed_event;

		processed_event.event = event_pair.first;
		processed_event.normalized_event = event_pair.second;
		
		// Here reverse the event function into several versions.
		processed_event.fwd_rvs_events = event_processor->processEvent(processed_event.normalized_event);

		reverse_foreach (const FuncDeclPair& fwd_rvs_event, processed_event.fwd_rvs_events)
		{
			// Put the generated statement after the normalized event.
			SgFunctionDeclaration* originalEvent = 
					isSgFunctionDeclaration(processed_event.normalized_event->get_definingDeclaration());
			SageInterface::insertStatementAfter(originalEvent, fwd_rvs_event.second);
			SageInterface::insertStatementAfter(originalEvent, fwd_rvs_event.first);
		}

		output.push_back(processed_event);
		
		cout << "Time used: " << t.elapsed() << endl;
		cout << "Event \"" << get_name(processed_event.event) << "\" is processed successfully!\n";
	}

	// Declare all stack variables on top of the generated file.
	foreach(SgVariableDeclaration* decl, event_processor->getAllStackDeclarations())
	{
		prependStatement(decl, globalScope);
	}

	// Prepend includes to test files.
	insertHeader("rctypes.h", PreprocessingInfo::after, false, globalScope);

	SageBuilder::popScopeStack();

	// Fix all variable references here.
	SageInterface::fixVariableReferences(globalScope);

	return output;
}


} // namespace Backstroke