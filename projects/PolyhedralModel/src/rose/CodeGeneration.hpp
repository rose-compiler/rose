/**
 * \file include/rose/CodeGeneration.hpp
 * \brief Interface for Sage AST generation using Cloog.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _CODE_GENERATION_HPP_
#define _CODE_GENERATION_HPP_

#include "rose/rose-utils.hpp"
#include "common/PolyhedricContainer.hpp"
#include "common/Schedule.hpp"

#include "scoplib/Cloog.hpp"

namespace CodeGeneration {

template <class TplStatement>
SgBasicBlock * generateBasicBlockFromSchedule(
	PolyhedricAnnotation::PolyhedralProgram<TplStatement, SgExprStatement, RoseVariable> & polyhedral_program,
	Scheduling::Schedule<TplStatement, SgExprStatement, RoseVariable> & schedule,
	SgScopeStatement * scope
);

template <class TplStatement>
void generateSageFromClast(
	struct clast_stmt * s,
	SgScopeStatement * scope,
	PolyhedricAnnotation::PolyhedralProgram<TplStatement, SgExprStatement, RoseVariable> & polyhedral_program
);

template <class TplStatement>
SgExpression * ClastExprToSageExpr(
	struct clast_expr * e,
	SgScopeStatement * scope,
	PolyhedricAnnotation::PolyhedralProgram<TplStatement, SgExprStatement, RoseVariable> & polyhedral_program
);

}

#include "rose/CodeGeneration.tpp"

#endif  /* _CODE_GENERATION_HPP_ */

