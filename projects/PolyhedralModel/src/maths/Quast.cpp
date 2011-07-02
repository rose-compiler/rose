/**
 * \file src/maths/Quast.cpp
 * \brief Implementation of Quast (non-template)
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "maths/PPLUtils.hpp"

#include <vector>

std::vector<LinearExpression> * maxlex(Polyhedron & p, std::vector<bool> & sign, size_t d1, size_t d2, size_t g) {
	std::vector<LinearExpression> * res = new std::vector<LinearExpression>(d1);
	
	if (d1==0) return res;
	
	std::vector<std::pair<bool, LinearExpression> > cs(2 * d1);
	
	const ConstraintSystem & cs_ = p.minimized_constraints();
	ConstraintSystem::const_iterator it;
	LinearExpression le(0);
	
	for (it = cs_.begin(); it != cs_.end(); it++) {
		for (size_t i = 0; i < d1; i++) {
			if (it->coefficient(VariableID(i)) != 0) { // == 1 or -1
				le = LinearExpression::zero();
				le += it->inhomogeneous_term();
				for (size_t j = 0; j < d1 + d2 + g; j++) {
					if (j != i)
						le += it->coefficient(VariableID(j)) * VariableID(j);
				}
				if (it->is_equality()) {
					cs[2 * i] =     std::pair<bool, LinearExpression>(true, - it->coefficient(VariableID(i)) * le);
					cs[2 * i + 1] = std::pair<bool, LinearExpression>(true, - it->coefficient(VariableID(i)) * le);
					break;
				}
				else if (it->coefficient(VariableID(i)) == 1) {
					if (!cs[2 * i].first) {
						cs[2 * i] = std::pair<bool, LinearExpression>(true, -le);
						break;
					}
				}
				else {
					if (!cs[2 * i + 1].first) {
						cs[2 * i + 1] = std::pair<bool, LinearExpression>(true, le);
						break;
					}
				}
			}
		}
	}
/*
	for (size_t i = 0; i < 2*d1; i++) {
		std::cout << cs[i].first << "\t";
		for (int j = 0; j < d1+d2+g; j++)
			std::cout << cs[i].second.coefficient(VariableID(j)) << "\t";
		std::cout << cs[i].second.inhomogeneous_term() << std::endl;
		std::cout << std::endl;
	}
*/
	for (size_t i = 0; i < d1-1; i++) {
		for (size_t j = i + 1; j < d1; j++) {
			if (cs[2 * j].second.coefficient(VariableID(i)) != 0)
				cs[2 * j].second += cs[2 * j].second.coefficient(VariableID(i)) * cs[2 * i].second - cs[2 * j].second.coefficient(VariableID(i)) * VariableID(i);
			if (cs[2 * j + 1].second.coefficient(VariableID(i)) != 0)
				cs[2 * j + 1].second += cs[2 * j + 1].second.coefficient(VariableID(i)) * cs[2 * i + 1].second - cs[2 * j + 1].second.coefficient(VariableID(i)) * VariableID(i);
		}
	}
	for (size_t i = d1 - 1; i > 0; i--) {
		for (int j = i - 1; j >= 0; j--) {
			if (cs[2 * j].second.coefficient(VariableID(i)) != 0)
				cs[2 * j].second += cs[2 * j].second.coefficient(VariableID(i)) * cs[2 * i].second - cs[2 * j].second.coefficient(VariableID(i)) * VariableID(i);
			if (cs[2 * j + 1].second.coefficient(VariableID(i)) != 0)
				cs[2 * j + 1].second += cs[2 * j + 1].second.coefficient(VariableID(i)) * cs[2 * i + 1].second - cs[2 * j + 1].second.coefficient(VariableID(i)) * VariableID(i);
		}
	}
	
	for (size_t i = 0; i < d1; i++)
		(*res)[i] = cs[2 * i + (sign[i] ? 1 : 0)].second;
	
	std::vector<LinearExpression> * tmp = new std::vector<LinearExpression>(d1);
	
	for (size_t i = 0; i < d1; i++) {
		(*tmp)[i] +=  (*res)[i].inhomogeneous_term();
		for (size_t j = 0; j < d2 + g; j++)
			(*tmp)[i] +=  (*res)[i].coefficient(VariableID(d1 + j)) * VariableID(j);
	}
	
	delete res;
	
	return tmp;
}

std::vector<Constraint> * conditions(
	const ConstraintSystem & dt,
	const ConstraintSystem & df,
	const std::vector<LinearExpression> & rel,
	size_t st, size_t sf, size_t sg 
) {
	std::vector<Constraint> * res = new std::vector<Constraint>();
	ConstraintSystem::const_iterator itc_cs;
	for (itc_cs = dt.begin(); itc_cs != dt.end(); itc_cs++) {
		LinearExpression le(0);
		le += itc_cs->inhomogeneous_term();
		for (int i = 0; (i < st) && (i < itc_cs->space_dimension()); i++) {
			le += itc_cs->coefficient(VariableID(i)) * rel[i];
		}
		for (int i = 0; (i < sg) && (st + i < itc_cs->space_dimension()); i++) {
			le += itc_cs->coefficient(VariableID(st + i)) * VariableID(sf + i);
		}
		if (itc_cs->is_equality())
			res->push_back(le == 0);
		else
			res->push_back(le >= 0);
	}
	Polyhedron p(sf + sg);
	for (itc_cs = df.begin(); itc_cs != df.end(); itc_cs++)
		p.refine_with_constraint(*itc_cs);
	
	std::vector<Constraint>::iterator it_cs = res->begin();
	while (it_cs != res->end()) {
		if (it_cs->is_tautological()) {
			it_cs = res->erase(it_cs);
			break;
		}
		if (it_cs->is_inconsistent()) {
			delete res;
			return NULL;
		}
		
		Polyhedron p_(p);
		p_.refine_with_constraint(*it_cs);
		if (p_.contains(p))
			it_cs = res->erase(it_cs);
		else
			it_cs++;
	}
	
	return res;
}

bool operator == (const std::vector<LinearExpression> & v1, const std::vector<LinearExpression> & v2) {
	if (v1.size() != v2.size())
		return false;
	bool res = true;
	for (int i = 0; i < v1.size(); i++) {
		res = (v1[i] - v2[i]).is_zero();
		if (!res) break;
	}
	return res;
}

bool operator == (const std::vector<Constraint> & v1, const std::vector<Constraint> & v2) {
	if (v1.size() != v2.size())
		return false;
	bool res = true;
	for (int i = 0; i < v1.size(); i++) {
		res = (v1[i] == v2[i]);
		if (!res) break;
	}
	return res;
}

