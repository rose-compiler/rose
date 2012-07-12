/**
 * \file include/common/ScheduleSpace.hpp
 * \brief Compute Schedule Space of an annoted SCoP.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _SCHEDULE_SPACE_HPP_
#define _SCHEDULE_SPACE_HPP_

#include "common/Exception.hpp"

#include "common/PolyhedricDependency.hpp"
#include "common/Schedule.hpp"

#include "maths/Farkas.hpp"
#include "maths/PolyhedronExploration.hpp"

#include "system/Utils.hpp"

#include <vector>

namespace Scheduling {

/**
 * /brief To select an algorithm for Multidimensional schedule generation (if only a one-dimensional schedule is needed it use exactly the same algorithm).
 */
typedef enum {
	PBCC, //! < from Iterative Optimization in the Polyhedral Model: Part II, Multidimensional Time, by Louis-Noel Pouchet, Cedric Bastoul, Albert Cohen and John Cavazos.
	Feautrier //! < from Some efficient solutions to the affine scheduling problem, Part II, Multidimensional Time, by Paul Feutrier. NIY
} ScheduleSpaceAlgorithm;

typedef enum {
	Iterator,
	Global,
	Constant
} CoefType;

struct CoefInfo {
	size_t space_coef;
	size_t statement;
	CoefType type;
	size_t type_coef;
	size_t local_coef;
	
	CoefInfo(size_t space_coef_, size_t statement_, CoefType type_, size_t type_coef_, size_t local_coef_);
};

/**
 * /brief This class represent a set of valid schedule for a program. (all valid schedule if one-dimensional)
 */
template <class Function, class Expression, class VariableLBL>
class ScheduleSpace {
	protected:
		PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & p_polyhedral_program;
		const std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *> & p_dependencies;
		
		std::vector<Polyhedron> p_schedule_spaces;
		
		std::vector<CoefInfo> p_coef_info;
		
		size_t p_dim_space;
		
		void generateSchedule(
			std::vector<size_t> * selection,
			std::vector<std::vector<int*> *> & raw_sched,
			std::vector<Schedule<Function, Expression, VariableLBL> > & res
		);
		
		void computePBCC();
		
		bool isValid(int ** sched);
		void print(std::ostream & out, const Polyhedron * p) const;
	public:
		ScheduleSpace(
			PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, 
			const std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *> & dependencies,
			ScheduleSpaceAlgorithm method = PBCC
		);
		
		/**
		 * \brief Generate a list of all schedules given some bound on their coeficient.
		 * \param ilb iterators coefficient lower bound
		 * \param iub iterators coefficient upper bound
		 * \param glb globals coefficient lower bound
		 * \param gub globals coefficient upper bound
		 * \param clb constant coefficient lower bound
		 * \param cub constant coefficient upper bound
		 * \return list of schedules
		 * 
		 * \bug Need to check for available memory.
		 */
		std::vector<Schedule<Function, Expression, VariableLBL> > * generateScheduleList(int ilb, int iub, int glb, int gub, int clb, int cub);
		
		/**
		 * \brief Check if a given schedule is inside the space
		 */
		bool isValid(Schedule<Function, Expression, VariableLBL> & sched_);
		
		/**
		 * \return the dimension of contained schedules
		 */
		size_t getDimension() const;
		
		/**
		 * \return The number of coefficient per schedule
		 */
		size_t getSize() const;
		
		void print(std::ostream & out) const;
};

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, ScheduleSpace<Function, Expression, VariableLBL> & sched_space);
	
#include "common/ScheduleSpace.tpp"

}

#endif /* _SCHEDULE_SPACE_HPP_ */

