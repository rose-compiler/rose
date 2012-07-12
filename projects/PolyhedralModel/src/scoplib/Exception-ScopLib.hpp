/**
 * \file include/scoplib/Exception-ScopLib.hpp
 * \brief Exception for ScopLib.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _EXCEPTION_SCOPLIB_HPP_
#define _EXCEPTION_SCOPLIB_HPP_

#include "common/Exception.hpp"

namespace Exception {

class ExceptionScopLib : public ExceptionBase {
	public:
		ExceptionScopLib(std::string str);
		virtual ~ExceptionScopLib();
		
		virtual void print(std::ostream & out);
};

class OutOfScopLibScope : public ExceptionScopLib {
	public:
		OutOfScopLibScope(std::string str);
		virtual ~OutOfScopLibScope();
		
		virtual void print(std::ostream & out);
};

class WrongDependencyType : public ExceptionScopLib {
	public:
		WrongDependencyType();
		virtual ~WrongDependencyType();
		
		virtual void print(std::ostream & out);
};

}

#endif /* _EXCEPTION_SCOPLIB_HPP_ */
