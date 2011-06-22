/**
 * \file include/common/PolyhedricContainer.hpp
 * \brief Define class which contain Polyhedric Modelisation of a program.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _POLYHEDRIC_CONTAINER_HPP_
#define _POLYHEDRIC_CONTAINER_HPP_

#include "common/Exception.hpp"

#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <iomanip>
#include <utility>

#include "maths/PPLUtils.hpp"

namespace PolyhedricAnnotation {

template <class Function, class Expression, class VariableLBL>
class PolyhedralProgram;

template <class Function, class Expression, class VariableLBL>
class Domain;

template <class Function, class Expression, class VariableLBL>
class Scattering;

template <class Function, class Expression, class VariableLBL>
class DataAccess;

template <class Function, class Expression, class VariableLBL>
class DataDependencies;

/*********************/
/* PolyhedralProgram */
/*********************/

/**
 * \class PolyhedralProgram
 * \brief Linked to a SCoP, this class references all expressions (statements) in the SCoP and keep track of variables, iterators and globals.
 */
template <class Function, class Expression, class VariableLBL>
class PolyhedralProgram {
	protected:
		Function * p_function; //!< Function (SCoP) associated
		
		std::vector<VariableLBL> p_variables; //!< Variables ID map (arrays and scalars)
		
		std::vector<Expression *> p_expressions; //!< List of all expressions (statements) in the SCoP.
		
		/**
		 * Keep track of iterators and globals.\n
		 * Each iterator has an ID by "expression scope".\n
		 * Globals have only one ID refered with 'NULL' as Expression pointer.
		 */
		std::vector<VariableLBL> p_iterator_list;
		std::map<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > > p_var_map;
		
		static VariableLBL getByID(size_t id, const std::map<VariableLBL, size_t> & map);

	public:
		/**
		 * \brief Constructor
		 * \param function
		 */
		PolyhedralProgram(Function * function);
		
		/**
		 * \brief Adding an iterator to the list
		 * \param it VariableLBL pointer
		 */
		void addIterator(VariableLBL it);
		/**
		 * \param v a VariableLBL pointer
		 * \return true if 'v' is an iterator
		 */
		bool isIterator(VariableLBL v) const;
		
		const std::vector<VariableLBL> & getIterators() const;
		
		/**
		 * \param v VariableLBL pointer.
		 * \param e Expression pointer.
		 * \return ID of the variable in the scope of the expression.
		 */
		const size_t getVariableID(Expression * e, VariableLBL v);
		
		/**
		 * \brief Adding an expression to the list
		 * \param e Expression pointer
		 */
		void addExpression(Expression * e);
		/**
		 * \return the list of expression
		 */
		const std::vector<Expression *> & getExpressions() const;
		
		/**
		 * \param v VariableLBL pointer
		 * \return an ID for 'v' (newly or previously determine)
		 */
		size_t getVariableID(VariableLBL v);
		size_t getVariableID(VariableLBL v) const;
		/**
		 * \param id the ID of the variable we look for.
		 * \return the variable c_strpointer associate to 'id'
		 */
		VariableLBL getVariableByID(size_t id) const;
		size_t getNumberOfVariables() const;
		
		VariableLBL getIteratorById(Expression * e, size_t id) const;
		
		VariableLBL getGlobalById(size_t id) const;
		size_t getNumberOfGlobals() const;
		
		size_t getStatementID(Expression * e) const;
		Expression * getStatementById(size_t id) const;
		size_t getNumberOfStatement() const;
		
		Function * getFunction() const;
		Function * getBase() const;
		
		void finalize() const;
		
		void print(std::ostream & out);
		
	template <class Function_, class Expression_, class VariableLBL_>
	friend std::ostream & operator << (std::ostream & out, PolyhedralProgram<Function_, Expression_, VariableLBL_> & polyhedral_program);
};

/**
 * \brief To associate a PolyhedralProgram instance to a Function
 */
template <class Function, class Expression, class VariableLBL>
void setPolyhedralProgram(Function * function);

/**
 * \brief Retrieve the PolyhedralProgram instance associated to a function
 * \exception if no PolyhedralProgram instance is associated to the function
 */
template <class Function, class Expression, class VariableLBL>
PolyhedralProgram<Function, Expression, VariableLBL> & getPolyhedralProgram(const Function * function);


template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program);

/**********/
/* Domain */
/**********/

/**
 * \class Domain
 * \brief Store iteration domain of an expression (statement)
 */
template <class Function, class Expression, class VariableLBL>
class Domain {
	protected:
		PolyhedralProgram<Function, Expression, VariableLBL> & p_polyhedral_program; //!< Associated SCoP
		Expression * p_expression; //!< Associated expression (statement)
		
		size_t p_nbr_surr_loop; //!< Number of surrounding loops (iterators)
		
		Polyhedron p_domain; //!< Iteration domain
	
	public:
		Domain(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop);
		
		PolyhedralProgram<Function, Expression, VariableLBL> & getPolyhedralProgram() const;
		
		/**
		 * \param inequation is a linear expression: {(v_i, coef_i)} -> sum_i(coef_i * v_i)\n
		 * v_i = NULL is for the inhomogeneous term.
		 */
		void addInequation(const std::vector<std::pair<VariableLBL, int> > & inequation, bool positive);
		const Polyhedron & getDomainPolyhedron() const;
		
		size_t getNumberOfIterators() const;
		size_t getNumberOfGlobals() const;
		
		void finalize();
		
		void print(std::ostream & out);
		
	template <class Function_, class Expression_, class VariableLBL_>
	friend std::ostream & operator << (std::ostream & out, Domain<Function_, Expression_, VariableLBL_> & domain);
};

/**
 * \brief To build a Domain instance to a function
 */
template <class Function, class Expression, class VariableLBL>
void setDomain(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop);

/**
 * \brief Retrieve the Domain instance associated to an expression
 * \exception if no Domain instance is associated to the expression
 */
template <class Function, class Expression, class VariableLBL>
Domain<Function, Expression, VariableLBL> & getDomain(const Expression * expression);


template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, Domain<Function, Expression, VariableLBL> & domain);

/**************/
/* Scattering */
/**************/

template <class Function, class Expression, class VariableLBL>
class Scattering {
	protected:
		PolyhedralProgram<Function, Expression, VariableLBL> & p_polyhedral_program;
		Expression * p_expression;
		
		size_t p_nbr_surr_loop; //!< Number of surrounding loops (iterators)
		
		// List of orderred linear expression [PPL] (scattering matrix)
		std::vector<LinearExpression> p_equations;
	
	public:
		Scattering(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop);
		
		PolyhedralProgram<Function, Expression, VariableLBL> & getPolyhedralProgram() const;
		
		void addEquation(std::vector<std::pair<VariableLBL, int> > & equation);
		const std::vector<LinearExpression> & getScattering() const;
		
		void print(std::ostream & out);
		
	
	template <class Function_, class Expression_, class VariableLBL_>
	friend std::ostream & operator << (std::ostream & out, Scattering<Function_, Expression_, VariableLBL_> & scattering);
};

/**
 * \brief To build a Scattering instance to an expression
 */
template <class Function, class Expression, class VariableLBL>
void setScattering(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop);

/**
 * \brief Retrieve the Scattering instance associated to an expression
 * \exception if no Scattering instance is associated to the expression
 */
template <class Function, class Expression, class VariableLBL>
Scattering<Function, Expression, VariableLBL> & getScattering(const Expression * expression);


template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, Scattering<Function, Expression, VariableLBL> & scattering);

/**************/
/* DataAccess */
/**************/

template <class Function, class Expression, class VariableLBL>
class DataAccess {
	protected:
		PolyhedralProgram<Function, Expression, VariableLBL> & p_polyhedral_program;
		Expression * p_expression;
		
		size_t p_nbr_surr_loop; //!< Number of surrounding loops (iterators)
		
		std::vector<std::pair<size_t, std::vector<LinearExpression> > > p_read;
		std::vector<std::pair<size_t, std::vector<LinearExpression> > > p_write;

	public:
		DataAccess(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop);
		
		PolyhedralProgram<Function, Expression, VariableLBL> & getPolyhedralProgram() const;
		
		/**
		 * \brief Declare a read access to a variable
		 * \param v the accessed variable
		 * \param access_vector the access vector:\n
		 * 	store as: {{(var_ij, coef_ij)}} = vect_i(sum_j(coef_ij * var_ij))\n
		 * 	eg: a[i][3*j+9] -> { { (1,i) } , { (3,j) , (9,NULL) } }\n
		 * 	As usual NULL is used for the inhomogeneous term.
		 */
		void addRead(VariableLBL v, const std::vector<std::vector<std::pair<VariableLBL, int> > > & access_vector);
		std::vector<std::vector<LinearExpression> > * getRead(VariableLBL v) const;
		/**
		 * \brief Declare a write access to a variable (see DataAccess::addRead for details).
		 * 
		 * Same than DataAccess::addRead.
		 */
		void addWrite(VariableLBL v, const std::vector<std::vector<std::pair<VariableLBL, int> > > & access_vector);
		std::vector<std::vector<LinearExpression> > * getWrite(VariableLBL v) const;
		
		void print(std::ostream & out);
		
		/**
		 * \brief internal use, for: ScopLib translator
		 */
		 const std::vector<std::pair<size_t, std::vector<LinearExpression> > > & getRead() const;
		 const std::vector<std::pair<size_t, std::vector<LinearExpression> > > & getWrite() const;
		
	friend class DataDependencies<Function, Expression, VariableLBL>;
	
	template <class Function_, class Expression_, class VariableLBL_>
	friend std::ostream & operator << (std::ostream & out, DataAccess<Function_, Expression_, VariableLBL_> & data_access);
};

/**
 * \brief To build a DataAccess instance to an expression
 */
template <class Function, class Expression, class VariableLBL>
void setDataAccess(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop);

/**
 * \brief Retrieve the DataAccess instance associated to an expression
 * \exception if no DataAccess instance is associated to the expression
 */
template <class Function, class Expression, class VariableLBL>
DataAccess<Function, Expression, VariableLBL> & getDataAccess(const Expression * expression);

template <class Function, class Expression, class VariableLBL>
void makeAccessAnnotation(Expression * expression, PolyhedricAnnotation::DataAccess<Function, Expression, VariableLBL> & data_access);

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, DataAccess<Function, Expression, VariableLBL> & data_access);

/****************************/
/* Templates implementation */
/****************************/

#include "common/PolyhedricContainer.tpp"

}

#endif /* _POLYHEDRIC_CONTAINER_HPP_ */

