/**
 * \file src/maths/biLinear.cpp
 * \brief Implementation of biLinear Expressions
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "maths/biLinear.hpp"

biLinearExpression::biLinearTerm::biLinearTerm(Integer coef_, size_t var_A_, size_t var_B_) :
	coef(coef_),
	var_A(var_A_),
	var_B(var_B_)
{}

biLinearExpression::biLinearTerm::biLinearTerm(const biLinearTerm & term) :
	coef(term.coef),
	var_A(term.var_A),
	var_B(term.var_B)
{}
				
bool biLinearExpression::biLinearTerm::sameVars(const biLinearTerm & term) const {
	return var_A == term.var_A && var_B == term.var_B;
}

bool biLinearExpression::biLinearTerm::useVars(size_t var_A_, size_t var_B_) const {
	return var_A == var_A_ && var_B == var_B_;
}
		
biLinearExpression::biLinearExpression(size_t dim_A, size_t dim_B) :
	p_dim_A(dim_A),
	p_dim_B(dim_B),
	p_expression()
{}

biLinearExpression::biLinearExpression(const biLinearExpression & exp) :
	p_dim_A(exp.p_dim_A),
	p_dim_B(exp.p_dim_B),
	p_expression(exp.p_expression)
{}
		
size_t biLinearExpression::getDimA() const {
	return p_dim_A;
}

size_t biLinearExpression::getDimB() const {
	return p_dim_B;
}

void biLinearExpression::add(size_t var_A, size_t var_B, Integer coef) {
	if (coef == 0)
		return;

	std::vector<biLinearTerm>::iterator it;
	for (it = p_expression.begin(); it != p_expression.end(); it++) {
		if (it->useVars(var_A, var_B)) {
			it->coef += coef;
			break;
		}
	}
	if (it == p_expression.end())
		p_expression.push_back(biLinearTerm(coef, var_A, var_B));
	else if (it->coef == 0)
		it = p_expression.erase(it);
}

void biLinearExpression::addA(size_t var_A, Integer coef) {
	add(var_A, p_dim_B, coef);
}

void biLinearExpression::addB(size_t var_B, Integer coef) {
	add(p_dim_A, var_B, coef);
}

void biLinearExpression::addConst(Integer coef) {
	add(p_dim_A, p_dim_B, coef);
}

Integer biLinearExpression::get(size_t var_A, size_t var_B) const {
	std::vector<biLinearTerm>::const_iterator it;
	for (it = p_expression.begin(); it != p_expression.end(); it++) {
		if (it->var_A == var_A && it->var_B == var_B)
		return it->coef;
	}
	return 0;
}

Integer biLinearExpression::getA(size_t var_A) const {
	return get(var_A, p_dim_B);
}

Integer biLinearExpression::getB(size_t var_B) const {
	return get(p_dim_A, var_B);
}

Integer biLinearExpression::getConst() const {
	return get(p_dim_A, p_dim_B);
}

biLinearExpression & biLinearExpression::operator = (const biLinearExpression & rhs) {
	this->p_dim_A = rhs.p_dim_A;
	this->p_dim_B = rhs.p_dim_B;
	this->p_expression = rhs.p_expression;
	return *this;
}

biLinearExpression biLinearExpression::operator + (const biLinearExpression & rhs) const {
	if (rhs.p_dim_A != this->p_dim_A || rhs.p_dim_B != this->p_dim_B)
		throw Exception::DimensionsError("biLinearExpression::operator + need bilinear expression being both A x B -> Z (same A and B).");
	biLinearExpression res(*this);
	
	std::vector<biLinearTerm>::const_iterator it1;
	std::vector<biLinearTerm>::iterator it2;
	for (it1 = rhs.p_expression.begin(); it1 != rhs.p_expression.end(); it1++) {
		for (it2 = res.p_expression.begin(); it2 != res.p_expression.end(); it2++) {
			if (it1->sameVars(*it2)) {
				it2->coef += it1->coef;
				break;
			}
		}
		if (it2 == res.p_expression.end())
			res.p_expression.push_back(*it1);
		else if (it2->coef == 0)
			it2 = res.p_expression.erase(it2);
	}
	
	return res;
}

biLinearExpression biLinearExpression::operator - (const biLinearExpression & rhs) const {
	if (rhs.p_dim_A != this->p_dim_A || rhs.p_dim_B != this->p_dim_B)
		throw Exception::DimensionsError("biLinearExpression::operator - need bilinear expression being both A x B -> Z (same A and B).");
	biLinearExpression res(*this);
	
	std::vector<biLinearTerm>::const_iterator it1;
	std::vector<biLinearTerm>::iterator it2;
	for (it1 = rhs.p_expression.begin(); it1 != rhs.p_expression.end(); it1++) {
		for (it2 = res.p_expression.begin(); it2 != res.p_expression.end(); it2++) {
			if (it1->sameVars(*it2)) {
				it2->coef -= it1->coef;
				break;
			}
		}
		if (it2 == res.p_expression.end())
			res.p_expression.push_back(biLinearTerm(-(it1->coef), it1->var_A, it1->var_B));
		else if (it2->coef == 0)
			it2 = res.p_expression.erase(it2);
	}
	
	return res;
}
		
biLinearExpression biLinearExpression::operator + (const Integer & rhs) const {
	biLinearExpression res(*this);
	
	res.addConst(rhs);
	
	return res;
}
		
biLinearExpression biLinearExpression::operator - (const Integer & rhs) const {
	biLinearExpression res(*this);
	
	res.addConst(-rhs);
	
	return res;
}

biLinearExpression biLinearExpression::biLinearExpression::operator - () const {
	biLinearExpression res(this->p_dim_A, this->p_dim_B);
	
	std::vector<biLinearTerm>::const_iterator it;
	for (it = p_expression.begin(); it != p_expression.end(); it++) {
		res.p_expression.push_back(biLinearTerm(-(it->coef), it->var_A, it->var_B));
	}
	
	return res;
}

biLinearExpression concatA(const biLinearExpression & a, const biLinearExpression & b) {
	if (a.p_dim_B != b.p_dim_B)
		Exception::DimensionsError("Concatenation of bilinear expression on first (A) space need second (B) space to be of same da.p_dim_B != b.p_dim_Bimension.");
	
	biLinearExpression res(a.p_dim_A + b.p_dim_A, a.p_dim_B);
	
	for (size_t i = 0; i <= a.p_dim_B; i++) {
		for (size_t j = 0; j < a.p_dim_A; j++)
			res.add(j, i, a.get(j,i));
		for (size_t j = 0; j < b.p_dim_A; j++)
			res.add(a.p_dim_A + j, i, b.get(j,i));
		res.add(a.p_dim_A + b.p_dim_A, i, a.get(a.p_dim_A, i) + b.get(b.p_dim_A,i));
	}
	
	return res;
}

