/**
 * \file include/maths/biLinear.hpp
 * \brief BiLinear Algebra.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _BILINEAR_HPP_
#define _BILINEAR_HPP_

#include "maths/PPLUtils.hpp"
#include "maths/Exception-maths.hpp"
 
#include <vector>

/**
 * \brief biLinear expression
 *
 * \todo Implement method for matrix based manipulation of bilinear expressions.\n\n
 * Can be usefull for low-complexity / high-memory-copy computation...\n
 * Based on 	Phi: E1 x E2 -> Z\n
 * as		Phi(a,b) = a.M.b + A.a + B.b + C\n
 * with 	M: dim(E1) x dim(E2) matrix\n
 * 		A: dim(E1) vector\n
 * 		B: dim(E2) vector\n
 * 		C: scalar\n\n
 * biLinearExpression(size_t dim_A, size_t dim_B, const Integer ** full_matrix);\n
 * void fillWithMatrix(const Integer ** full_matrix);\n
 * Integer ** getFullMatrix() const;\n\n
 * biLinearExpression(size_t dim_A, size_t dim_B, const Integer ** matrix, const Integer * vect_A, const Integer * vect_B, Integer C);\n
 * void fillWithMatrixAndVectors(Integer ** matrix, const Integer * vect_A, const Integer * vect_B, Integer C);\n\n
 * Integer ** getMatrix() const;\n
 * Integer * getVectA() const;\n
 * Integer * getVectB() const;
 */
class biLinearExpression {
	public:
		class biLinearTerm {
			public:
				Integer coef;
				size_t var_A;
				size_t var_B;
				
				biLinearTerm(Integer coef_, size_t var_A_, size_t var_B_);
				biLinearTerm(const biLinearTerm & term);
				
				bool sameVars(const biLinearTerm & term) const;
				bool useVars(size_t var_A_, size_t var_B_) const;
		};
	
	protected:
		size_t p_dim_A;
		size_t p_dim_B;
		
		std::vector<biLinearTerm> p_expression;
	
	public:
		biLinearExpression(size_t dim_A, size_t dim_B);
		biLinearExpression(const biLinearExpression & exp);
		
		size_t getDimA() const;
		size_t getDimB() const;
		
		void add(size_t var_A, size_t var_B, Integer coef);
		void addA(size_t var_A, Integer coef);
		void addB(size_t var_B, Integer coef);
		void addConst(Integer coef);
		
		Integer get(size_t var_A, size_t var_B) const;
		Integer getA(size_t var_A) const;
		Integer getB(size_t var_B) const;
		Integer getConst() const;
		
		biLinearExpression & operator = (const biLinearExpression & rhs);
		
		biLinearExpression operator + (const biLinearExpression & rhs) const;
		
		biLinearExpression operator - (const biLinearExpression & rhs) const;
		
		biLinearExpression operator + (const Integer & rhs) const;
		
		biLinearExpression operator - (const Integer & rhs) const;
		
		biLinearExpression operator - () const;
	
	friend biLinearExpression concatA(const biLinearExpression & a, const biLinearExpression & b);
};

biLinearExpression concatA(const biLinearExpression & a, const biLinearExpression & b);

#endif /* _BILINEAR_HPP_ */

