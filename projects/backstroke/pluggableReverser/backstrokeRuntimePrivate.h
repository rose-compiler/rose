#pragma once

#include <unistd.h>
#include <deque>

// This file is NOT to be included by transformed programs. It is here simply to assist in implementing the runtime.

//! Stores infomation necessary for the reverse/commig processing of one event.
//! A record gets created for each event function, but not for non-event functions.
struct EventProcessingRecord
{
	size_t stack8_pushes;
	size_t stack16_pushes;
	size_t stack32_pushes;
	size_t stack64_pushes;
	size_t deallocation_stack_pushes;
	
	EventProcessingRecord() : 
		stack8_pushes(-1),
		stack16_pushes(-1),
		stack32_pushes(-1),
		stack64_pushes(-1),
		deallocation_stack_pushes(-1)
	{ }
};


//! This stack contains exactly one EventProcessingRecord object for each event processed.
//! Processing the forward function causes the a new processing record to be appended to the back of the stack
extern std::deque<EventProcessingRecord> event_processing_stack;