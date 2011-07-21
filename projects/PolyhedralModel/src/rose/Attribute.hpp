/**
 * \file include/rose/Attribute.hpp
 * \brief Contains definition of AstAttribute (see Rose doc) specialization for Polyhedral Modelisation storage.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _ATTRIBUTE_HPP_
#define _ATTRIBUTE_HPP_

#include "rose/rose-utils.hpp"
#include "common/PolyhedricContainer.hpp"

namespace PolyhedricAnnotation {

/**
 * \brief To store PolyhedralProgram which is attach to a SCoP.
 */
template <class TplScope>
class PolyhedralProgramAttribute : public AstAttribute, public PolyhedralProgram<TplScope, SgExprStatement, RoseVariable> {
	public:
		PolyhedralProgramAttribute(TplScope * scope);
};

/**
 * \brief To store Domain which is attach to an Expression Statement (SgExprStatement).
 */
template <class TplScope>
class DomainAttribute : public AstAttribute, public Domain<TplScope, SgExprStatement, RoseVariable> {
	public:
		DomainAttribute(
			PolyhedralProgram<TplScope, SgExprStatement, RoseVariable> & polyhedral_program,
			SgExprStatement * expression,
			size_t nbr_iterators
		);
};

/**
 * \brief To store Scattering which is attach to an Expression Statement (SgExprStatement).
 */
template <class TplScope>
class ScatteringAttribute : public AstAttribute, public Scattering<TplScope, SgExprStatement, RoseVariable> {
	public:
		ScatteringAttribute(
			PolyhedralProgram<TplScope, SgExprStatement, RoseVariable> & polyhedral_program,
			SgExprStatement * expression,
			size_t nbr_iterators
		);
};

/**
 * \brief To store DataAccess which is attach to an Expression Statement (SgExprStatement).
 */
template <class TplScope>
class DataAccessAttribute : public AstAttribute, public DataAccess<TplScope, SgExprStatement, RoseVariable> {
	public:
		DataAccessAttribute(
			PolyhedralProgram<TplScope, SgExprStatement, RoseVariable> & polyhedral_program,
			SgExprStatement * expression,
			size_t nbr_iterators
		);
};

}

#include "rose/Attribute.tpp"

#endif /* _ATTRIBUTE_HPP_ */

