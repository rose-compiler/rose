/**
 * \file include/rose/Parser.hpp
 * \brief Generic interface to generate Polyhedric Modelisation (if it's possible) of Sage AST.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include "rose/rose-utils.hpp"
#include "common/ScopTree.hpp"
#include "rose/Exception-rose.hpp"

#include <map>
#include <utility>

namespace PolyhedricAnnotation {


template <class TplStatement>
size_t Parse(TplStatement * attach_to, SgStatement * stmt) throw (Exception::ExceptionBase);

/**
 * \brief Traverse a stmt to generate a ScopTree
 */
template <class TplStatement>
size_t Traverse(
	SgStatement * stmt,
	ScopTree_::ScopTree<TplStatement, SgExprStatement, RoseVariable> * tree,
	size_t pos,
	TplStatement * base
) throw (Exception::ExceptionBase);

/* Data access parsing needed templates */

template <class TplStatement>
void makeAccessAnnotationSage(
	SgExprStatement * expression,
	PolyhedricAnnotation::DataAccess<TplStatement, SgExprStatement, RoseVariable> & data_access
);

template <class TplStatement>
void setRead(
	SgExpression * exp,
	PolyhedricAnnotation::DataAccess<TplStatement, SgExprStatement, RoseVariable> & data_access
);

template <class TplStatement>
void setWrite(
	SgExpression * exp,
	PolyhedricAnnotation::DataAccess<TplStatement, SgExprStatement, RoseVariable> & data_access
);

/**
 * \brief Translate a Sage expression to a vector of term (coef * var)
 */
void translateLinearExpression(SgExpression * lin_exp, std::vector<std::pair<RoseVariable, int> > & res, bool neg = false);

/**
 * \brief Translate a Sage expression to a RoseVariable
 */
RoseVariable * translateVarRef(SgExpression * var_ref);

RoseVariable getIterator(SgForStatement * for_stmt);

int getIncrement(SgForStatement * for_stmt, const RoseVariable & iterator);

RoseVariable * parseArray(SgPntrArrRefExp * array, std::vector<std::vector<std::pair<RoseVariable, int> > > & av);

}

#include "rose/Parser.tpp"

#endif /* _PARSER_HPP_ */

