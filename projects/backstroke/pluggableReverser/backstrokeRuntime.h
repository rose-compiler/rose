#pragma once

#include <cstdlib>
#include <deque>
#include <cassert>
#include <stdint.h>

#define __BACKSTROKE

//! Should be called at the very beginning of the forward event. Marks the current height of all the stacks.
void __initialize_forward_event();

//! Should be called at the very last point before the exit from a forward event. Saves information about the size &
//! contents of the rollback stack.
void __finalize_forward_event();

//! Read the information from the processing record at the bottom of the event processing stack and perform
//! commit actions - freeing stacks, deallocating memory, and commiting delayed output
void __commit();

//There are four data stacks, one for each primitive type that shares the same size.
extern std::deque<int8_t> rollback_stack_8;
extern std::deque<int16_t> rollback_stack_16;
extern std::deque<int32_t> rollback_stack_32;
extern std::deque<int64_t> rollback_stack_64;

//The function push(T) pushes variables on the appropriate stack based on their size.
//It is overloaded for all scalar types
template <typename T>
inline void __push(T val)
{
	assert(false && "Use the appropriate template specialization of push\n");
}

#define DECLARE_PUSH(type, stack_size) \
template<> \
inline void __push(type val) \
{ \
	assert(sizeof(type) == stack_size >> 3); \
	int##stack_size##_t* casted_val = reinterpret_cast<int##stack_size##_t*>(&val); \
	rollback_stack_ ## stack_size.push_back(*casted_val); \
} 

//Declare the push functions for all the basic integral types
DECLARE_PUSH(char, 8);
DECLARE_PUSH(short int, 16);
DECLARE_PUSH(int, 32);
DECLARE_PUSH(long int, 64);
DECLARE_PUSH(long long int, 64);
DECLARE_PUSH(unsigned char, 8);
DECLARE_PUSH(unsigned short int, 16);
DECLARE_PUSH(unsigned int, 32);
DECLARE_PUSH(unsigned long int, 64);
DECLARE_PUSH(unsigned long long int, 64);

//Declare push functions for the floating point types
DECLARE_PUSH(float, 32);
DECLARE_PUSH(double, 64);

//Declare push function for pointers
template<>
inline void __push(void* val)
{
	assert(sizeof(void*) == sizeof(int64_t));
	rollback_stack_64.push_back(reinterpret_cast<int64_t>(val));
}


//The function pop_back<T> pops a variable from the appropriate stack and returns it.
//The template type T is used to decide which stack to use
template <typename T>
inline T __pop_back()
{
	assert(false && "Use the appropriate template specialization.");
}

#define DECLARE_POP_BACK(type, stack_size) \
template <> \
inline type __pop_back() \
{ \
	assert(sizeof(type) == stack_size >> 3); \
	int##stack_size##_t val = rollback_stack_##stack_size.back(); \
	rollback_stack_##stack_size.pop_back(); \
	\
	type* castedVal = reinterpret_cast<type*>(&val); \
	\
	return *castedVal; \
}

//Declare the pop_back functions for all the basic integral types
DECLARE_POP_BACK(char, 8);
DECLARE_POP_BACK(unsigned char, 8);
DECLARE_POP_BACK(short int, 16);
DECLARE_POP_BACK(unsigned short int, 16);
DECLARE_POP_BACK(int, 32);
DECLARE_POP_BACK(unsigned int, 32);
DECLARE_POP_BACK(long int, 64);
DECLARE_POP_BACK(unsigned long int, 64);
DECLARE_POP_BACK(long long int, 64);
DECLARE_POP_BACK(unsigned long long int, 64);

//Declare the pop_back functions for the floating point types
DECLARE_POP_BACK(float, 32);
DECLARE_POP_BACK(double, 64);


