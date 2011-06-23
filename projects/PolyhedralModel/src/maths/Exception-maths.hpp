/**
 * \file include/maths/Exception-maths.hpp
 * \brief Exception for maths.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _EXCEPTION_MATHS_HPP_
#define _EXCEPTION_MATHS_HPP_

#include "common/Exception.hpp"

namespace Exception {

class ExceptionMaths : public ExceptionBase {
	public:
		ExceptionMaths(std::string str);
		virtual ~ExceptionMaths();
		
		virtual void print(std::ostream & out) = 0;
};

class DimensionsError : public ExceptionMaths {
	public:
		DimensionsError(std::string str);
		virtual ~DimensionsError();
		
		virtual void print(std::ostream & out);
};

}

#endif /* _EXCEPTION_MATHS_HPP_ */

