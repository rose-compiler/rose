/**
 * \file include/common/ScopTree.hpp
 * \brief ScopTree definition, those class are use to translate Compiler IR to a more convenient representation.
 * \author Tristan Vanderbruggen
 * \version 0.1
 * The ScopTree associated to a function (or a scop) is proccess to compute Polyhedric Annotation (see include/common/PolyhedricContainer.hpp).
 */

#ifndef _SCOPTREE_HPP_
#define _SCOPTREE_HPP_

#include "common/PolyhedricContainer.hpp"
#include "common/Exception.hpp"

#include <vector>

namespace ScopTree_ {

/**
 * \class ScopTree
 * \brief Base class for SCoP pseudo-AST representation.
 */
template <class Function, class Expression, class VariableLBL>
class ScopTree {
	protected:
		size_t p_position; //!< Position inside the parent.
		ScopTree * p_parent; //!< Pointer on parent.
		std::vector<ScopTree<Function, Expression, VariableLBL> *> p_childs; //!< List of childs.

	public:
		/**
		 * \brief Constructor
		 * \param position Position inside the parent node (default = 0).
		 * \param parent Pointer on parent node (default = NULL).
		 */
		ScopTree(size_t position = 0, ScopTree<Function, Expression, VariableLBL> * parent = NULL);
		
		/**
		 * \brief Recursive function traversing SCoP tree.
		 * \param vect Path from root to parent node.
		 * \param nbr_surr_loop number of surrounding loops (ie: number of ScopLoop in 'vect').
		 */
		virtual void Traverse(std::vector<ScopTree<Function, Expression, VariableLBL> *> * vect = NULL, size_t nbr_surr_loop = 0);
		
		/**
		 * \return true: if current instance of ScopTree is a ScopRoot.
		 */
		virtual bool isRoot() const = 0;
		/**
		 * \return true: if current instance of ScopTree is a ScopLoop.
		 */
		virtual bool isLoop() const = 0;
		/**
		 * \return true: if current instance of ScopTree is a ScopConditionnal.
		 */
		virtual bool isConditinnal() const = 0;
		/**
		 * \return true: if current instance of ScopTree is a ScopStatement.
		 */
		virtual bool isStatement() const = 0;
		
		/**
		 * \return Parent of current node.
		 */
		ScopTree<Function, Expression, VariableLBL> * getParent() const;
		/**
		 * \return List of childs
		 */
		const std::vector<ScopTree<Function, Expression, VariableLBL> *> & getChilds() const;
		/**
		 * \return Position inside the parent.
		 */
		const size_t getPosition() const;
};

/**
 * \class ScopRoot
 * \brief This is the root of the SCoP tree.
 *
 * Usually, it's associated to a function definition.
 */
template <class Function, class Expression, class VariableLBL>
class ScopRoot : public ScopTree<Function, Expression, VariableLBL> {
	protected:
		Function * p_function; //!< Function associated to the SCoP.
	
	public:
		/**
		 * \brief Constructor
		 * \param function Function associated to the SCoP.
		 *
		 * Use default for super-constructor
		 */
		ScopRoot(Function * function);
		
		/**
		 * \return the function pointer associated with the SCoP
		 */
		Function * getFunction() const;
		
		/**
		 * \brief ScopRoot specialization of ScopTree::Traverse
		 * \param vect Need to be 'NULL'.
		 * \param nbr_surr_loop Need to be 0.
		 */
		virtual void Traverse(std::vector<ScopTree<Function, Expression, VariableLBL> *> * vect = NULL, size_t nbr_surr_loop = 0);
		
		virtual bool isRoot() const;
		virtual bool isLoop() const;
		virtual bool isConditinnal() const;
		virtual bool isStatement() const;
};

/**
 * \class ScopLoop
 * \brief Associated to SCoP loops.
 *
 * This class contains the associated iteration space and the iterator name.\n
 * Store linear expression in std::map<VariableLBL, int>: {(v_i, coef_i)} -> sum_i(coef_i * v_i).\n
 * Lower bound (lb) and upper bound (ub) aren't like lb < ub. It depend of the increment (inc). (eg: "for (i = 10; i >= 1; i--)" -> lb = 10, ub = 1, inc = -1)
 */
template <class Function, class Expression, class VariableLBL>
class ScopLoop : public ScopTree<Function, Expression, VariableLBL> {
	protected:
		VariableLBL p_iterator; //!< Iterator (induction variable) associated with the loop. ("i")
				
		std::map<VariableLBL, int>  p_lb; //!< Lower bound of the iteration domain. ("i >= lb")
		std::map<VariableLBL, int>  p_ub; //!< Upper bound of the iteration domain. ("i <= ub")
		int p_inc; //!< Increment used to traverse the domain. ("i += inc")

	public:
		/**
		 * \brief Constructor
		 * \param position Position inside the parent node.
		 * \param parent   Pointer on parent node.
		 * \param iterator Induction variable of the loop.
		 */
		ScopLoop(size_t position, ScopTree<Function, Expression, VariableLBL> * parent, VariableLBL iterator);
		
		/**
		 * \brief ScopLoop specialization of ScopTree::Traverse
		 */
		virtual void Traverse(std::vector<ScopTree<Function, Expression, VariableLBL> *> * vect, size_t nbr_surr_loop);
		
		/**
		 * \return Iterator (induction variable) associated with the loop.
		 */
		VariableLBL getIterator();
		
		/**
		 * \brief Add a term in the lower bound linear expression.
		 * \param var  variable (iterator, global or "constant").
		 * \param coef integer coefficiant.
		 */
		void addLowerBoundTerm(VariableLBL var, int coef);
		/**
		 * \return Linear expression of the lower bound.
		 */
		std::map<VariableLBL, int> & getLowerBound();
		
		/**
		 * \brief Add a term in the upper bound linear expression.
		 * \param var  variable (iterator, global or "constant").
		 * \param coef integer coefficiant.
		 */
		void addUpperBoundTerm(VariableLBL var, int coef);
		/**
		 * \return Linear expression of the upper bound.
		 */
		std::map<VariableLBL, int> & getUpperBound();
		
		/**
		 * \brief Set increment.
		 * \param inc integer increment.
		 */
		void setIncrement(int inc);
		/**
		 * \return increment.
		 */
		int getIncrement();
		
		virtual bool isRoot() const;
		virtual bool isLoop() const;
		virtual bool isConditinnal() const;
		virtual bool isStatement() const;
};

/**
 * \class ScopConditionnal
 * \brief Associated to SCoP conditionnal.
 *
 * This class contains the associated conditions.\n\n
 * For " if c1 then S1 elseif c2 then S2 else S3 ", we have 3 ScopConditional block with c1, !c1 & c2, !c1 & !c2 as condition.\n\n
 * Store linear expression in std::map<VariableLBL, int>: {(v_i, coef_i)} -> sum_i(coef_i * v_i).\n\n
 * Multiple conditions: C1, ... , Cn give: if ( C1 && ... && Cn ) then ...
 */
template <class Function, class Expression, class VariableLBL>
class ScopConditionnal : public ScopTree<Function, Expression, VariableLBL> {
	protected:
		std::vector<std::pair<std::vector<std::pair<VariableLBL, int> >, bool> > p_conditions; //!< Set of Linear Expressions and type (ineq/eq)
	
	public:
		/**
		 * \brief Constructor
		 * \param position Position inside the parent node.
		 * \param parent   Pointer on parent node.
		 */
		ScopConditionnal(size_t position, ScopTree<Function, Expression, VariableLBL> * parent);
		
		/**
		 * \brief Add a condition linear expression.
		 * \param var  variable (iterator, global or "constant").
		 * \param coef integer coefficiant.
		 */
		void addCondition(const std::vector<std::pair<VariableLBL, int> > & lin_exp, bool equality);
		
		/**
		 * \return the number of conditions
		 */
		size_t getNumberOfCondition() const;
		
		/**
		 * \param id index of the requested condition
		 * \return a condition as {(v_i, coef_i)} -> sum_i(coef_i * v_i)
		 */
		const std::vector<std::pair<VariableLBL, int> > & getConditionLinearExpression(size_t id) const;
		
		/**
		 * \return false if inequality, true if equality
		 */
		bool getConditionType(size_t id) const;
		
		virtual bool isRoot() const;
		virtual bool isLoop() const;
		virtual bool isConditinnal() const;
		virtual bool isStatement() const;
};

/**
 * \class ScopStatement
 * \brief Associated to SCoP statement.
 *
 * SCoP statement are leaves of the tree and expression of the code.
 */
template <class Function, class Expression, class VariableLBL>
class ScopStatement : public ScopTree<Function, Expression, VariableLBL> {
	protected:
		Expression * p_expression; //!< Expression (piece of code) associated to the statement.

	public:
		/**
		 * \brief Constructor
		 * \param position   Position inside the parent node.
		 * \param parent     Pointer on parent node.
		 * \param expression Expression associated to the SCoP.
		 */
		ScopStatement(size_t position, ScopTree<Function, Expression, VariableLBL> * parent, Expression * expression);
		
		/**
		 * \brief ScopStatement specialization of ScopTree::Traverse
		 *
		 * As ScopStatement is leave specialization of ScopTree, this function is the end the recursion.\n
		 * This function create, from reading of param 'vect', PolyhedricAnnotation::Domain and PolyhedricAnnotation::Scattering of 'p_expression'.
		 */
		virtual void Traverse(std::vector<ScopTree<Function, Expression, VariableLBL> *> * vect, size_t nbr_surr_loop);
		
		virtual bool isRoot() const;
		virtual bool isLoop() const;
		virtual bool isConditinnal() const;
		virtual bool isStatement() const;
};

#include "common/ScopTree.tpp"

}

#endif /* _SCOPTREE_HPP_ */

