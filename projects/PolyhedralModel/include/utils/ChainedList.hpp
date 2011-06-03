/**
 * \file include/utils/ChainedList.hpp
 * \brief Define a chained list used by QUAST to Dependencies translation... 
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _CHAINED_LIST_HPP_
#define _CHAINED_LIST_HPP_

template <class Type>
class ChainedList {
	protected:
		Type p_value;
		ChainedList<Type> * p_next;
		
	public:
		ChainedList(Type value) : p_value(value), p_next(NULL) {}
		
		void setNext(ChainedList<Type> * next) { p_next = next; }
		ChainedList<Type> * getNext() const { return p_next; }
		
		const Type & getValue() const { return p_value; }
};

#endif /* _CHAINED_LIST_HPP_ */

