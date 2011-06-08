/**
 * \file src/common/ScheduleSpace.cpp
 * \brief Implementation of Schedule Space (non-template).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "common/ScheduleSpace.hpp"

namespace Scheduling {

CoefInfo::CoefInfo(size_t space_coef_, size_t statement_, CoefType type_, size_t type_coef_, size_t local_coef_) :
	space_coef(space_coef_),
	statement(statement_),
	type(type_),
	type_coef(type_coef_),
	local_coef(local_coef_)
{}

}

