/**
 * \file include/common/Schedule.hpp
 * \brief Schedule of an annoted SCoP.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _SCHEDULE_HPP_
#define _SCHEDULE_HPP_

#include "common/Exception.hpp"

#include "common/PolyhedricContainer.hpp"

#include <vector>

namespace Scheduling {

/**
 * \brief Represent a schedule of a SCoP
 */
template <class Function, class Expression, class VariableLBL>
class Schedule {
	protected:
		PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & p_polyhedral_program;
		
		size_t p_dim;
		std::map<Expression *, std::vector<LinearExpression> > p_schedule;
		size_t p_memory_size;
	public:
		Schedule(PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, size_t dim);
		
		size_t getDimension() const;
		
		std::vector<LinearExpression> & setSchedule(size_t expression);
		
		const std::vector<LinearExpression> & getSchedule(size_t expression) const;
		
		Function * getFunction() const;
		const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & getPolyhedralProgram() const;
		PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & getPolyhedralProgram();
		
		void print(std::ostream & out) const;
		void print(std::ostream & out, std::string indent) const;
		
		size_t memory_size() const;
		
		Schedule<Function, Expression, VariableLBL> & operator = (const Schedule<Function, Expression, VariableLBL> & s);
};

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, Schedule<Function, Expression, VariableLBL> & sched);
	
#include "common/Schedule.tpp"

}

#endif /* _SCHEDULE_HPP_ */

