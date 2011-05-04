/**
 * \file src/maths/Exception-maths.cpp
 * \brief Implementation of maths specific exceptions.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "maths/Exception-maths.hpp"

namespace Exception {

ExceptionMaths::ExceptionMaths(std::string str) :
	ExceptionBase(str)
{}

ExceptionMaths::~ExceptionMaths() {}

DimensionsError::DimensionsError(std::string str) :
	ExceptionMaths(str)
{}

DimensionsError::~DimensionsError() {}
		
void DimensionsError::print(std::ostream & out) {
	out << p_str << std::endl;
}

}

