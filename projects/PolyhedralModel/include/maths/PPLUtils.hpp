/**
 * \file include/maths/PPLUtils.hpp
 * \brief Include PPL header and do some renaming for PPL's types.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _PPL_UTILS_HPP_
#define _PPL_UTILS_HPP_

#include <ppl.hh>

typedef Parma_Polyhedra_Library::Variable VariableID;
typedef Parma_Polyhedra_Library::Linear_Expression LinearExpression;
typedef Parma_Polyhedra_Library::Constraint Constraint;
typedef Parma_Polyhedra_Library::Constraint_System ConstraintSystem;
typedef Parma_Polyhedra_Library::C_Polyhedron Polyhedron;
typedef Parma_Polyhedra_Library::Coefficient Integer;
typedef Parma_Polyhedra_Library::Generator Generator;

#endif /* _PPL_UTILS_HPP_ */

