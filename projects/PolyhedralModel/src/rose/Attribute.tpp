/**
 * \file src/rose/Attribute.tpp
 * \brief Implementation of Sage Attribute for Polyhedric Modelization storage.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

namespace PolyhedricAnnotation {

template <class TplScope>
PolyhedralProgramAttribute<TplScope>::PolyhedralProgramAttribute(TplScope * scope) :
	AstAttribute(),
	PolyhedralProgram<TplScope, SgExprStatement, RoseVariable>(scope)
{
	scope->addNewAttribute("PolyhedricAnnotation::PolyhedralProgram", this);
}

template <class TplScope>
DomainAttribute<TplScope>::DomainAttribute(
	PolyhedralProgram<TplScope, SgExprStatement, RoseVariable> & polyhedral_program,
	SgExprStatement * expression,
	size_t nbr_iterators
) :
	AstAttribute(),
	Domain<TplScope, SgExprStatement, RoseVariable>(polyhedral_program, expression, nbr_iterators)
{
	expression->addNewAttribute("PolyhedricAnnotation::Domain", this);
}

template <class TplScope>
ScatteringAttribute<TplScope>::ScatteringAttribute(
	PolyhedralProgram<TplScope, SgExprStatement, RoseVariable> & polyhedral_program,
	SgExprStatement * expression,
	size_t nbr_iterators
) :
	AstAttribute(),
	Scattering<TplScope, SgExprStatement, RoseVariable>(polyhedral_program, expression, nbr_iterators)
{
	expression->addNewAttribute("PolyhedricAnnotation::Scattering", this);
}

template <class TplScope>
DataAccessAttribute<TplScope>::DataAccessAttribute(
	PolyhedralProgram<TplScope, SgExprStatement, RoseVariable> & polyhedral_program,
	SgExprStatement * expression,
	size_t nbr_iterators
) :
	AstAttribute(),
	DataAccess<TplScope, SgExprStatement, RoseVariable>(polyhedral_program, expression, nbr_iterators)
{
	expression->addNewAttribute("PolyhedricAnnotation::DataAccess", this);
}

}

