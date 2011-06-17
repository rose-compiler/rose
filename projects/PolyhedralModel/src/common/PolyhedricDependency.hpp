/**
 * \file include/common/PolyhedricDependency.hpp
 * \brief Compute Data Dependencies of an annoted SCoP.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _POLYHEDRIC_DEPENDENCY_HPP_
#define _POLYHEDRIC_DEPENDENCY_HPP_

#include "common/Exception.hpp"

#include "common/PolyhedricContainer.hpp"

#include "utils/ChainedList.hpp"

#include "maths/PPLUtils.hpp"

#include "maths/Quast.hpp"

#include <vector>

template <class Expression>
std::ostream & operator << (std::ostream & out, const std::pair<Expression *, size_t> & p);

namespace PolyhedricDependency {

typedef enum {RaR, RaW, WaR, WaW} DependencyType;

/**
 * \brief Represent a dependency between two statements of a SCoP.
 */
template <class Function, class Expression, class VariableLBL>
class Dependency {
	protected:
		const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & p_polyhedral_program;
		size_t p_from;
		size_t p_to;
		size_t p_from_pos;
		size_t p_to_pos;
		DependencyType p_type;
		size_t p_variable;
		Polyhedron p_dependency;
		
	public:
		Dependency(
			const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program,
			Expression * from,
			Expression * to,
			size_t from_pos,
			size_t to_pos,
			DependencyType type,
			VariableLBL variable,
			Polyhedron & dependency
		);
		Dependency(
			const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program,
			size_t from,
			size_t to,
			size_t from_pos,
			size_t to_pos,
			DependencyType type,
			size_t variable,
			Polyhedron & dependency
		);
		
		/**
		 * \return the statement wich are dependent.
		 */
		size_t getFrom() const;
		
		/**
		 * \return the statement wich it depends.
		 */
		size_t getTo() const;
		
		/**
		 * \return the type of the dependency (RaR, RaW, WaR, WaW).
		 */
		DependencyType getType() const;
		
		/**
		 * \return the variable which is involve.
		 */
		size_t getVariable() const;
		
		/**
		 * \return the dependency polyhedron.
		 */
		Polyhedron getPolyhedron() const;
		
		void print(std::ostream & out);
};

/**
 * \brief Compute Read-after-Read dependency for a SCoP.
 * 
 * Those are the most time consuming, so have to be compute only if needed.
 */
template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeRaR(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program);

/**
 * \brief Compute Read-after-Write dependency for a SCoP.
 */
template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeRaW(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program);

/**
 * \brief Compute Write-after-Read dependency for a SCoP.
 */
template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeWaR(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program);

/**
 * \brief Compute Write-after-Write dependency for a SCoP.
 */
template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeWaW(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program);

/**
 * \brief Compute all dependency for a SCoP.
 */
template <class Function, class Expression, class VariableLBL>
std::vector<Dependency<Function, Expression, VariableLBL> *> * ComputeDependencies(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program);

#include "common/PolyhedricDependency.tpp"

}

#endif /* _POLYHEDRIC_DEPENDENCY_HPP_ */

