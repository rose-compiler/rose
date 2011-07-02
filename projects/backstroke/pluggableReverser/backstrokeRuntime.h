#pragma once

#include <cstdlib>
#include <deque>
#include <queue>
#include <algorithm>
#include <boost/any.hpp>
#include <boost/foreach.hpp>
#include <cassert>

#define __BACKSTROKE

extern std::deque<boost::any> data_stack;

template <class T>
inline T pop_front()
{
    assert(!data_stack.empty());
    T val = boost::any_cast<T>(data_stack.front());
    data_stack.pop_front();
    return val;
}

template <class T>
inline T push(T val)
{
    data_stack.push_back(boost::any(val));
    return val;
}

template <class T>
inline T pop()
{
    assert(!data_stack.empty());
    T val = boost::any_cast<T>(data_stack.back());
    data_stack.pop_back();
    return val;
}

//! Create a deep copy of the specified object.
template<class T>
inline T* __clone__(T* obj)
{
    if (obj)
        return new T(*obj);
    else
        return NULL;
}

template<class T>
inline void __destroy__(T obj)
{
}

template<class T>
inline void __destroy__(T* obj)
{
    delete obj;
}

//! Used instead of the assignment operator. This might delete the rhs argument!
//! Note that T must be a subclass of U
template<class U, class T>
inline void __assign__(U*& lhs, T* rhs)
{
	if (rhs == NULL)
	{
		lhs = NULL;
	}
	else
	{
		if (lhs != NULL)
		{
			//Invoke the assignment operator
			assert(dynamic_cast<T*>(lhs) != NULL);
			*static_cast<T*>(lhs) = *rhs;
		}
		else
		{
			//We want to know when this happens
			assert(false);
			lhs = rhs;
		}
	}
}



