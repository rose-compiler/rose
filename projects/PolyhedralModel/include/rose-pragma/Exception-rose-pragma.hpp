/**
 * \file include/rose-pragma/Exception-rose-pragma.hpp
 * \brief Exceptions for the pragma based polyhedral modelisation
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _EXCEPTION_ROSE_PRAGMA_HPP_
#define _EXCEPTION_ROSE_PRAGMA_HPP_

#include "rose/rose-utils.hpp"
#include "rose/Exception-rose.hpp"

#include <string>

namespace Exception {

class ExceptionPragma : public ExceptionBase {	
	public:
		ExceptionPragma(std::string str);
		virtual ~ExceptionPragma();
		
		virtual void print(std::ostream & out) = 0;
};

class NotPolyhedricPragmaException : public ExceptionPragma {	
	public:
		NotPolyhedricPragmaException(std::string str);
		virtual ~NotPolyhedricPragmaException();
		
		virtual void print(std::ostream & out);
};

class InvalidPolyhedricPragmaException : public ExceptionPragma {	
	public:
		InvalidPolyhedricPragmaException(std::string str);
		virtual ~InvalidPolyhedricPragmaException();
		
		virtual void print(std::ostream & out);
};

class InvalidPolyhedricProgramException : public ExceptionPragma {
	protected:
		ExceptionRose & p_catched_exception;

	public:
		InvalidPolyhedricProgramException(std::string str, ExceptionRose & catched_exception);
		virtual ~InvalidPolyhedricProgramException();
		
		virtual void print(std::ostream & out);
};

}

#endif /* _EXCEPTION_ROSE_PRAGMA_HPP_ */

