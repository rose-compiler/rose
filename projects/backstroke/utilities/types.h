#ifndef REVERSE_COMPUTATION_TYPES
#define REVERSE_COMPUTATION_TYPES

#include <rose.h>
#include <utility>

struct EventReversalResult
{
	EventReversalResult(SgFunctionDeclaration* f, SgFunctionDeclaration* r, SgFunctionDeclaration* c)
			: forwardEvent(f), reverseEvent(r), commitMethod(c)
	{
	}

	SgFunctionDeclaration* forwardEvent;
	SgFunctionDeclaration* reverseEvent;
	SgFunctionDeclaration* commitMethod;
};


//! This structure contains all processed events inside.
struct ProcessedEvent
{
	SgFunctionDeclaration* event;
	std::vector<EventReversalResult> fwd_rvs_events;
};


#endif
