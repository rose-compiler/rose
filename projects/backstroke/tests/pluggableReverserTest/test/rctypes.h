#ifndef reverse_rctypes_h
#define reverse_rctypes_h

#include <stdlib.h>
#include <deque>
#include <queue>
#include <boost/any.hpp>

template <class T>
T push(std::deque<T>& data_stack, T val)
{
    data_stack.push_back(val);
    return val;
}

template <class T>
T pop(std::deque<T>& data_stack)
{
    T val = data_stack.back();
    data_stack.pop_back();
    return val;
}

//!Pop from the front of a stack. Used in fossil collection
template <class T>
void pop_front(std::deque<T>& data_stack)
{
    data_stack.pop_front();
}

template <class T>
T push(std::deque<boost::any>& data_stack, T val)
{
    data_stack.push_back(boost::any(val));
    return val;
}

template <class T>
T pop(std::deque<boost::any>& data_stack)
{
    T val = boost::any_cast<T>(data_stack.back());
    data_stack.pop_back();
    return val;
}

//!Pop from the front of a stack. Used in fossil collection
template <class T>
void pop_front(std::deque<boost::any>& data_stack)
{
	boost::any_cast<T>(data_stack.back());
    data_stack.pop_front();
}

/**********************************************************************************
 * The following random number generator functions are to make sure the event and forward event
 * functions received the same random numbers, in order to make test pass.
 **********************************************************************************/
static std::queue<int> random_numbers;

int rand_num()
{
    int num = rand();
    random_numbers.push(num);
    return num;
}

int rand_num_fwd()
{
    int num = random_numbers.front();
    random_numbers.pop();
    return num;
}


#endif
