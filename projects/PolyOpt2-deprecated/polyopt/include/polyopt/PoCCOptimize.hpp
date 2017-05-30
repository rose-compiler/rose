/*
 * PoCCOptimize.hpp: This file is part of the PolyOpt project.
 *
 * PolyOpt: a Polyhedral Optimizer for the ROSE compiler
 *
 * Copyright (C) 2011 the Ohio State University
 *
 * This program can be redistributed and/or modified under the terms
 * of the license specified in the LICENSE.txt file at the root of the
 * project.
 *
 * Contact: P. Sadayappan <saday@cse.ohio-state.edu>
 *
 */
/**
 * @file: PoCCOptimize.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef POLYOPT_POCCOPTIMIZE_HPP
# define POLYOPT_POCCOPTIMIZE_HPP


#ifndef SCOPLIB_INT_T_IS_LONGLONG
# define SCOPLIB_INT_T_IS_LONGLONG
#endif
#include <scoplib/scop.h>
#ifndef CLOOG_INT_GMP
# define CLOOG_INT_GMP
#endif
#include <cloog/cloog.h>
#include <pocc/options.h>
#include <pocc-utils/options.h>
#include <pocc/driver-candl.h>
#include <pocc/driver-letsee.h>
#include <pocc/driver-pluto.h>
#include <pocc/driver-codegen.h>
#include <pocc/driver-clastops.h>
#include <past/past.h>
#include <polyopt/PolyRoseOptions.hpp>


clast_stmt* OptimizeSingleScopWithPocc (scoplib_scop_p scop,
					s_pocc_options_t* poptions,
					s_pocc_utils_options_t* puoptions,
					PolyRoseOptions& polyoptions);
s_past_node_t*
OptimizeSingleScopWithPoccPast (scoplib_scop_p scop,
				s_pocc_options_t* poptions,
				s_pocc_utils_options_t* puoptions,
				PolyRoseOptions& polyoptions);



/**
 * Check if a loop is parallel. The scop must represents only one
 * loop, with an arbitraty number of statements in it.
 *
 *
 */
bool PoCCIsLoopParallel (scoplib_scop_p scop,
			 s_pocc_options_t* poptions,
			 s_pocc_utils_options_t* puoptions);

/**
 * Check if all memory accesses in a loop have stride-1 property. The
 * scop must represents only one loop, with an arbitraty number of
 * statements in it.
 *
 *
 */
bool PoCCIsLoopStrideOneAccesses (scoplib_scop_p scop,
				  s_pocc_options_t* poptions,
				  s_pocc_utils_options_t* puoptions);


#endif
