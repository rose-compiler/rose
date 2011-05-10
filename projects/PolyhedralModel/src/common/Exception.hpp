/**
 * \file include/common/Exception.hpp
 * \brief All exceptions use in 'common' are declared in this file.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <exception>
#include <ostream>

namespace Exception {

/**
 * \brief Base class for all exceptions in this project (not only the common part)
 */
class ExceptionBase {
	protected:
		std::string p_str;
	
	public:
		ExceptionBase(std::string str);
		virtual ~ExceptionBase();
		
		virtual void print(std::ostream & out) = 0;
};

std::ostream & operator <<(std::ostream & out, ExceptionBase & e);

class ExceptionFrontend : public ExceptionBase {
	public:
		ExceptionFrontend(std::string str);
		virtual ~ExceptionFrontend();
		
		virtual void print(std::ostream & out) = 0;
};

class NotImplementedYet : public ExceptionBase {
	public:
		NotImplementedYet(std::string str);
		virtual ~NotImplementedYet();
		
		virtual void print(std::ostream & out);
};

class MemoryOverflow : public ExceptionBase {
	public:
		MemoryOverflow();
		virtual ~MemoryOverflow();
		
		virtual void print(std::ostream & out);
};

/***********************/
/* Container exception */
/***********************/

class ContainerException : public ExceptionBase {
	public:
		ContainerException(std::string str);
		virtual ~ContainerException();
		
		virtual void print(std::ostream & out) = 0;
};

template <class Function, class Expression>
class UnknownExpression : public ContainerException {
	protected:
		Function * p_function;
		Expression * p_expression;
	
	public:
		UnknownExpression(Function * function, Expression * expression);
		virtual ~UnknownExpression();
		
		virtual void print(std::ostream & out);
};

template <class Expression, class VariableLBL>
class AmbiguousVariableDim : public ContainerException {
	protected:
		Expression * p_source;
		Expression * p_target;
		VariableLBL p_variable;
	
	public:
		AmbiguousVariableDim(Expression * source, Expression * target, VariableLBL variable);
		virtual ~AmbiguousVariableDim();
		
		virtual void print(std::ostream & out);
};

class UnknownVariableID : public ContainerException {
	protected:
	
	public:
		UnknownVariableID();
		virtual ~UnknownVariableID();
		
		virtual void print(std::ostream & out);		
};

class UnknownVariable : public ContainerException {
	protected:
	
	public:
		UnknownVariable();
		virtual ~UnknownVariable();
		
		virtual void print(std::ostream & out);		
};

/***********************/
/* ScopTree exceptions */
/***********************/

class ScopTreeException : public ExceptionBase {
	public:
		ScopTreeException(std::string str);
		virtual ~ScopTreeException();
		
		virtual void print(std::ostream & out) = 0;
};

class UnexpectedScopNode : public ScopTreeException {
	public:
		UnexpectedScopNode(std::string str);
		virtual ~UnexpectedScopNode();
		
		virtual void print(std::ostream & out);
};

class OutOfScopRestrainedDefinition : public ScopTreeException {
	public:
		OutOfScopRestrainedDefinition(std::string str);
		virtual ~OutOfScopRestrainedDefinition();
		
		virtual void print(std::ostream & out);
};

#include "common/Exception.tpp"

}

#endif /* _EXCEPTION_HPP_ */

