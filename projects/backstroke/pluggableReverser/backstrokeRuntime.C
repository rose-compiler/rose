#include "backstrokeRuntime.h"
#include "backstrokeRuntimePrivate.h"

// Definitions of all the stacks. They are extern everywhere else
std::deque<int8_t> rollback_stack_8;
std::deque<int16_t> rollback_stack_16;
std::deque<int32_t> rollback_stack_32;
std::deque<int64_t> rollback_stack_64;
std::deque<void*> deallocation_stack;
std::deque<EventProcessingRecord> event_processing_stack;


//These variables are only visible from this class
size_t stack8_marker = -1;
size_t stack16_marker = -1;
size_t stack32_marker = -1;
size_t stack64_marker = -1;
size_t deallocation_stack_marker = -1;

const size_t INVALID = (size_t)-1;

void __initialize_forward_event()
{
	//Make sure __initialize is not being called twice in a row
	assert(stack8_marker == INVALID && stack16_marker == INVALID && 
			stack32_marker == INVALID && stack64_marker == INVALID && deallocation_stack_marker == INVALID);
	
	//Save all the stack sizes
	stack8_marker = rollback_stack_8.size();
	stack16_marker = rollback_stack_16.size();
	stack32_marker = rollback_stack_32.size();
	stack64_marker = rollback_stack_64.size();
	deallocation_stack_marker = deallocation_stack.size();
}

void __finalize_forward_event()
{
	//Make sure __initialize was called first
	assert(stack8_marker != INVALID && stack16_marker != INVALID && stack32_marker != INVALID &&
			stack64_marker != INVALID && deallocation_stack_marker != INVALID);
	
	//Add a processing record the number of events pushed on each stack
	EventProcessingRecord record;
	record.stack8_pushes = rollback_stack_8.size() - stack8_marker;
	record.stack16_pushes = rollback_stack_16.size() - stack16_marker;
	record.stack32_pushes = rollback_stack_32.size() - stack32_marker;
	record.stack64_pushes = rollback_stack_64.size() - stack64_marker;
	record.deallocation_stack_pushes = deallocation_stack.size() - deallocation_stack_marker;
	
	//Make sure the stacks only grow during forward execution
	assert(record.stack8_pushes >= 0 && record.stack16_pushes >= 0 && record.stack32_pushes >=0 && record.stack64_pushes >= 0
			&& record.deallocation_stack_pushes >= 0);
	
	event_processing_stack.push_back(record);
	
	stack8_marker = INVALID;
	stack16_marker = INVALID;
	stack32_marker = INVALID;
	stack64_marker = INVALID;
	deallocation_stack_marker = INVALID;
}

void __commit()
{
	assert(!event_processing_stack.empty());
	const EventProcessingRecord& r = event_processing_stack.front();
	
	//Pop the rollback stacks
	rollback_stack_8.erase(rollback_stack_8.begin(), rollback_stack_8.begin() + r.stack8_pushes);
	rollback_stack_16.erase(rollback_stack_16.begin(), rollback_stack_16.begin() + r.stack16_pushes);
	rollback_stack_32.erase(rollback_stack_32.begin(), rollback_stack_32.begin() + r.stack32_pushes);
	rollback_stack_64.erase(rollback_stack_64.begin(), rollback_stack_64.begin() + r.stack64_pushes);

	//Deallocate memory
	for (size_t i = 0; i < r.deallocation_stack_pushes; ++i)
	{
		operator delete(deallocation_stack.front());
		deallocation_stack.pop_front();
	}
	
	//Remove the event processing record
	event_processing_stack.pop_front();
}

int main_test()
{
	__initialize_forward_event();
	__push<char>('1');
	__push<int>(3);
	__push<unsigned long>(4ul);
	__push<unsigned short>(5);
	__finalize_forward_event();
	
	//Test the commit function
	__commit();
	assert(rollback_stack_8.empty() && rollback_stack_16.empty() && rollback_stack_32.empty() && rollback_stack_64.empty());
	
	
	//Test push/pop
	__push<char>('1');
	__push<int>(3);
	__push<unsigned long>(4ul);
	__push<unsigned short>(5);
	
	assert(__pop_back<unsigned short>() == 5);
	assert(__pop_back<unsigned long>() == 4ul);
	assert(__pop_back<int>() == 3);
	assert(__pop_back<char>() == '1');
}