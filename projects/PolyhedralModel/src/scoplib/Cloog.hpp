/**
 * \file include/scoplib/Cloog.hpp
 * \brief An interface for Cloog: Code generation from linear schedules.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _CLOOG_HPP_
#define _CLOOG_HPP_

#include "scoplib/Exception-ScopLib.hpp"

#include "common/Schedule.hpp"

#include "cloog/cloog.h"

namespace ScopLib {

template <class Function, class Expression, class VariableLBL>
size_t generateCloogProgramFromSchedule(
	const Scheduling::Schedule<Function, Expression, VariableLBL> & schedule,
	CloogState * state,
	CloogOptions * options,
	CloogProgram * program
);
	
#include "scoplib/Cloog.tpp"

}

#endif /* _CLOOG_HPP_ */

