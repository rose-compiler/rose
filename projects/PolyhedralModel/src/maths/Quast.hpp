/**
 * \file include/maths/Quast.hpp
 * \brief QUAST (QUasi-Affine Selection Tree) implementation.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include <iostream>
#include "maths/PPLUtils.hpp"
#include "maths/Exception-maths.hpp"

#ifndef _QUAST_HPP_
#define _QUAST_HPP_

template <class Type>
bool isBottom(Type p);

template <class Type>
Type getBottom();

bool operator == (const std::vector<Constraint> & v1, const std::vector<Constraint> & v2);

bool operator == (const std::vector<LinearExpression> & v1, const std::vector<LinearExpression> & v2);

std::vector<Constraint> * conditions(
	const ConstraintSystem & dt,
	const ConstraintSystem & df,
	const std::vector<LinearExpression> & rel,
	size_t st, size_t sf, size_t sg 
);

std::vector<LinearExpression> * maxlex(Polyhedron & p, std::vector<bool> & sign, size_t d1, size_t d2, size_t g);

/**/

template <class Type>
class QUAST;

/**/

template <class Type>
QUAST<Type> * computeMaxLex(Type from, const std::vector<LinearExpression> & f_from, Type to, const std::vector<LinearExpression> & f_to);

template <class Type>
QUAST<Type> * computeMax(Type from, const std::vector<LinearExpression> & f_from, Type to, const std::vector<LinearExpression> & f_to, size_t depth);

template <class Type>
std::vector<LinearExpression> * computeRelation(Type obj, std::vector<LinearExpression> & arg);

/**/

template <class Type>
QUAST<Type> * maxQuast(QUAST<Type> * q1, QUAST<Type> * q2);

template <class Type>
QUAST<Type> * simplify(QUAST<Type> * q, Polyhedron * p = NULL);

/**/

template <class Type>
QUAST<Type> * makeEmpty(Type source);

template <class Type>
QUAST<Type> * makeBranch(Type source, std::vector<Constraint> & cs, QUAST<Type> * q1, QUAST<Type> * q2);

template <class Type>
QUAST<Type> * makeLeaf(Type source, Type leaf, std::vector<LinearExpression> & iterators);

/**
 * \brief QUAST means Quasi-Affine Selection Tree.
 *
 * Here, I implement a Affine Selection Tree (Quasi is for congruence support).
 */

template <class Type>
class QUAST {
	protected:
		Type p_source;
		
		std::vector<Constraint> p_conditions;
		QUAST<Type> * p_then;
		QUAST<Type> * p_else;
		
		bool p_is_leaf;
		Type p_leaf;
		std::vector<LinearExpression> p_relation;
		
		QUAST(Type source);
		QUAST(const QUAST<Type> & quast);
		
	public:
		~QUAST();
		
		Type Source() const;
		
		const std::vector<Constraint> & Condition() const;
		
		QUAST<Type> * Then() const;
		QUAST<Type> * Else() const;
		
		bool isLeaf() const;
		
		Type Leaf() const;
		const std::vector<LinearExpression> & Relation() const;
		
		void print(std::ostream & out, std::string indent) const;
		
	template <class Type_>
	friend bool operator == (const QUAST<Type_> & q1, const QUAST<Type_> & q2);
		
	template <class Type_>
	friend QUAST<Type_> * maxQuast(QUAST<Type_> * q1, QUAST<Type_> * q2);
	
	template <class Type_>
	friend QUAST<Type_> * simplify(QUAST<Type_> * q, Polyhedron * p);

	template <class Type_>
	friend QUAST<Type_> * makeEmpty(Type_ source);

	template <class Type_>
	friend QUAST<Type_> * makeBranch(Type_ source, std::vector<Constraint> & cs, QUAST<Type_> * q1, QUAST<Type_> * q2);

	template <class Type_>
	friend QUAST<Type_> * makeLeaf(Type_ source, Type_ leaf, std::vector<LinearExpression> & iterators);
};

template <class Type>
bool operator == (const QUAST<Type> & q1, const QUAST<Type> & q2);

/**/

#include "maths/Quast.tpp"

#endif /* _QUAST_HPP_ */

