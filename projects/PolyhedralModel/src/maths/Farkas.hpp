/**
 * \file include/maths/Farkas.hpp
 * \brief Implementation of Farkas Algorithm.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _FARKAS_HPP_
#define _FARKAS_HPP_

#include "maths/biLinear.hpp"

/**
 * \brief Classic Farkas' Algorithm
 */
Polyhedron Farkas(const biLinearExpression & phi, const Polyhedron & positivity_domain);

/**
 * \brief Compute Farkas' Algorithm for phi >= 0 and phi > 0
 */
void FarkasWS(const biLinearExpression & phi, const Polyhedron & positivity_domain, Polyhedron ** weakly, Polyhedron ** strongly);

#endif /* _FARKAS_HPP_ */

