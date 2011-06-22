/**
 * \file include/maths/PolyhedronExploration.hpp
 * \brief Set of function returning a list of point for a given polyhedron.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _POLYHEDRON_EXPLORATION_HPP_
#define _POLYHEDRON_EXPLORATION_HPP_

#include "maths/PPLUtils.hpp"

#include <vector>

/**
 * \param p the polyhedron
 * \param lower_bounds_ lower bound for each dimension
 * \param upper_bounds_ upper bound for each dimension
 * \return list of all points in the polyhedron respecting bounds
 *
 * \bug risk of memory overflow
 * \todo check if enougth memory is available
 */
std::vector<int*> * exhaustiveExploration(Polyhedron & p, const std::vector<int> & lower_bounds_, const std::vector<int> & upper_bounds_);

#endif /* _POLYHEDRON_EXPLORATION_HPP_ */

