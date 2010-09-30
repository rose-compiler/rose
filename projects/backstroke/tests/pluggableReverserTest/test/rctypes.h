#ifndef reverse_rctypes_h
#define reverse_rctypes_h

#include <stdlib.h>
#include <stack>
#include <queue>
#include <boost/any.hpp>

template <class T>
T push(std::stack<T>& data_stack, T val)
{
    data_stack.push(val);
    return val;
}

template <class T>
T pop(std::stack<T>& data_stack)
{
    T val = data_stack.top();
    data_stack.pop();
    return val;
}

template <class T>
T push(std::stack<boost::any>& data_stack, T val)
{
    data_stack.push(boost::any(val));
    return val;
}

template <class T>
T pop(std::stack<boost::any>& data_stack)
{
    T val = boost::any_cast<T>(data_stack.top());
    data_stack.pop();
    return val;
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
