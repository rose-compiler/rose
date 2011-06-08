/**
 * \file src/maths/Farkas.cpp
 * \brief Implementation of Farkas' Algorithm
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "maths/Farkas.hpp"

Polyhedron Farkas(const biLinearExpression & phi, const Polyhedron & positivity_domain) {
	if (phi.getDimB() != positivity_domain.space_dimension())
		throw Exception::DimensionsError("In Farkas Method, the dimension of the second (B) space of \"phi\" need to be the same than the dimension of the positivity domain.");

	size_t size = 0;
	const ConstraintSystem & cs = positivity_domain.minimized_constraints();
	ConstraintSystem::const_iterator it;
	for (it = cs.begin(); it != cs.end(); it++) size++;
	
	biLinearExpression farkas(size + 1, positivity_domain.space_dimension());
	
	farkas.addA(0, 1);
	size_t cnt = 1;
	for (it = cs.begin(); it != cs.end(); it++) {
		for (size_t i = 0; i < it->space_dimension(); i++)
			farkas.add(cnt, i, it->coefficient(VariableID(i)));
		farkas.addA(cnt, it->inhomogeneous_term());
		cnt++;
	}
	
	biLinearExpression ident = concatA(phi, -farkas);
	
	Polyhedron res(phi.getDimA() + size + 1);
	
	for (size_t i = 0; i <= it->space_dimension(); i++) {
		LinearExpression le;
		for (size_t j = 0; j < phi.getDimA() + size + 1; i++)
			le += ident.get(i, j) * VariableID(j);
		le += ident.getB(i);
		res.refine_with_constraint(le == 0);
	}
	
	res.refine_with_constraint(VariableID(phi.getDimA()) >= 0);
	cnt = phi.getDimA() + 1;
	for (it = cs.begin(); it != cs.end(); it++) {
		if (it->is_inequality())
			res.refine_with_constraint(VariableID(cnt) >= 0);
		cnt++;
	}
	
	res.unconstrain(Parma_Polyhedra_Library::Variables_Set(VariableID(phi.getDimA()), VariableID(phi.getDimA() + size)));
	
	return res;
}

void FarkasWS(const biLinearExpression & phi, const Polyhedron & positivity_domain, Polyhedron ** weakly, Polyhedron ** strongly) {
	if (phi.getDimB() != positivity_domain.space_dimension())
		throw Exception::DimensionsError("In Farkas Method, the dimension of the second (B) space of \"phi\" need to be the same than the dimension of the positivity domain.");

	size_t size = 0;
	const ConstraintSystem & cs = positivity_domain.minimized_constraints();
	ConstraintSystem::const_iterator it;
	for (it = cs.begin(); it != cs.end(); it++) size++;
	
	biLinearExpression farkas(size + 1, positivity_domain.space_dimension());
	
	farkas.addA(0, 1);
	size_t cnt = 1;
	for (it = cs.begin(); it != cs.end(); it++) {
		for (size_t i = 0; i < it->space_dimension(); i++)
			farkas.add(cnt, i, it->coefficient(VariableID(i)));
		farkas.addA(cnt, it->inhomogeneous_term());
		cnt++;
	}
	
	biLinearExpression ident = concatA(phi, -farkas);
	
	(*weakly) = new Polyhedron(phi.getDimA() + size + 1);
	
	(*weakly)->refine_with_constraint(VariableID(phi.getDimA()) >= 0);
	cnt = phi.getDimA() + 1;
	for (it = cs.begin(); it != cs.end(); it++) {
		if (it->is_inequality())
			(*weakly)->refine_with_constraint(VariableID(cnt) >= 0);
		cnt++;
	}
	
	for (size_t i = 0; i < phi.getDimB(); i++) {
		LinearExpression le;
		for (size_t j = 0; j < phi.getDimA() + size + 1; j++)
			le += ident.get(j, i) * VariableID(j);
		le += ident.getB(i);
		(*weakly)->refine_with_constraint(le == 0);
	}
	
	(*strongly) = new Polyhedron(**weakly);
	
	LinearExpression le;
	for (size_t j = 0; j < phi.getDimA() + size + 1; j++)
		le += ident.getA(j) * VariableID(j);
	le += ident.getConst();
	(*weakly)->refine_with_constraint(le == 0);
	(*strongly)->refine_with_constraint(le == 1);
	
	(*weakly)->unconstrain(Parma_Polyhedra_Library::Variables_Set(VariableID(phi.getDimA()), VariableID(phi.getDimA() + size)));
	(*weakly)->remove_space_dimensions(Parma_Polyhedra_Library::Variables_Set(VariableID(phi.getDimA()), VariableID(phi.getDimA() + size)));
	(*strongly)->unconstrain(Parma_Polyhedra_Library::Variables_Set(VariableID(phi.getDimA()), VariableID(phi.getDimA() + size)));
	(*strongly)->remove_space_dimensions(Parma_Polyhedra_Library::Variables_Set(VariableID(phi.getDimA()), VariableID(phi.getDimA() + size)));
}

