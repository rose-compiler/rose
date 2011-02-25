#ifndef BACKSTROKE_H
#define	BACKSTROKE_H

#include <rose.h>
#include <boost/function.hpp>

#include "pluggableReverser/eventProcessor.h"

// Define foreach and reverse_foreach which ease coding very much.
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH


namespace Backstroke
{

using namespace std;
using namespace boost;

/**
* Normalize all expressions in each event in the given project.
*
* @param is_event A function or functor telling this function whether a function is an event.
* @param project A SgProject object which contains events.
 *@return all event functions found
*/
vector<SgFunctionDeclaration*> normalizeEvents(function<bool(SgFunctionDeclaration*)> is_event, SgProject* project);

/**
* Reverse all events in the given project. The generated forward and reverse functions are
* inserted after the corresponding event.
*
* @param event_processor An event processor object, which needs to be configured before being passed in.
* @param is_event A function or functor telling this function whether a function is an event.
* @param project A SgProject object which contains events.
* @return A map from event to its generated forward and reverse events pairs.
*/
vector<ProcessedEvent> reverseEvents(EventProcessor* event_processor, function<bool(SgFunctionDeclaration*)> is_event,
		SgProject* project);
}

#endif	/* BACKSTROKE_H */

