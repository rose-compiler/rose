#include "backstroke.h"
#include "ssa/staticSingleAssignment.h"
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

vector<SgFunctionDeclaration*> normalizeEvents(function<bool(SgFunctionDeclaration*)> is_event, SgProject* project)
{
	// Get the global scope.
	vector<SgFunctionDeclaration*> events;
	
	// Get every function declaration and identify if it's an event function.
	vector<SgFunctionDeclaration*> func_decls = BackstrokeUtility::querySubTree<SgFunctionDeclaration > (project);
	foreach (SgFunctionDeclaration* decl, func_decls)
	{
		//This ensures that we process every function only once
		if (decl != decl->get_definingDeclaration())
			continue;

		if (is_event(decl))
		{
			//Normalize this event function.
			BackstrokeNorm::normalizeEvent(decl);
			events.push_back(decl);
			
			cout << "Function " << decl->get_name().str() << " was normalized!\n" << endl;
		}
	}

	return events;
}


vector<ProcessedEvent>
reverseEvents(EventProcessor* event_processor,
		boost::function<bool(SgFunctionDeclaration*)> is_event,
		SgProject* project)
{
	ROSE_ASSERT(project);

	// Normalize all events then reverse them.
	timer analysisTimer;
	vector<SgFunctionDeclaration*> allEventMethods = normalizeEvents(is_event, project);
	printf("-- Timing: Normalization took %.2f seconds.\n", analysisTimer.elapsed());
    printf("Found %d event functions.\n", allEventMethods.size());
    fflush(stdout);

	AstTests::runAllTests(project);
    
	analysisTimer.restart();
	StaticSingleAssignment interproceduralSsa(project);
	interproceduralSsa.run(true);
	event_processor->setInterproceduralSsa(&interproceduralSsa);
	printf("-- Timing: Interprocedural SSA took %.2f seconds.\n", analysisTimer.elapsed());
    fflush(stdout);
    
    analysisTimer.restart();
	VariableRenaming var_renaming(project);
	var_renaming.run();
    event_processor->setVariableRenaming(&var_renaming);
	printf("-- Timing: Variable Renaming took %.2f seconds.\n", analysisTimer.elapsed());
    fflush(stdout);
    	
	// Get the global scope.
	SgGlobal* globalScope = SageInterface::getFirstGlobalScope(project);
	SageBuilder::pushScopeStack(isSgScopeStatement(globalScope));

	vector<ProcessedEvent> output;

	foreach(SgFunctionDeclaration* event, allEventMethods)
	{
		timer t;

		ProcessedEvent processed_event;
		processed_event.event = event;
		
		// Here reverse the event function into several versions.
		processed_event.fwd_rvs_events = event_processor->processEvent(event);
		ROSE_ASSERT(!processed_event.fwd_rvs_events.empty());

		reverse_foreach (const EventReversalResult& fwd_rvs_event, processed_event.fwd_rvs_events)
		{
			// Put the generated statement after the normalized event.
			SgFunctionDeclaration* originalEvent = isSgFunctionDeclaration(event->get_definingDeclaration());
			SageInterface::insertStatementAfter(originalEvent, fwd_rvs_event.commitMethod);
			SageInterface::insertStatementAfter(originalEvent, fwd_rvs_event.reverseEvent);
			SageInterface::insertStatementAfter(originalEvent, fwd_rvs_event.forwardEvent);
		}

		output.push_back(processed_event);
		
		cout << "Time used: " << t.elapsed() << endl;
		cout << "Event \"" << get_name(processed_event.event) << "\" as processed successfully!\n";
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
