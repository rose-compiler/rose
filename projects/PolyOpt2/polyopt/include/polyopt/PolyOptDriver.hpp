/*
 * PolyOptDriver.hpp: This file is part of the PolyOpt project.
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
 * @file: PolyOptDriver.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef POLYOPT_POLYOPT_DRIVER_HPP
# define POLYOPT_POLYOPT_DRIVER_HPP

#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#include <rose.h>

#include <polyopt/PolyRoseOptions.hpp>


/**
 * This unit contains the PolyOpt driver. To invoke it:
 *
 * // Initialize the options, by default all to false.
 * PolyRoseOptions polyoptions;
 * // To annotate with DDVs:
 * polyoptions.setComputeDDV(true);
 * // To annotate inner loops:
 * polyoptions.setAnnotateInnerLoops(true);
 * // To optimize the code with pluto (balanced fusion heuristic):
 * polyoptions.setPluto(true);
 * // To tile the code with pluto (balanced fusion heuristic + tiling):
 * polyoptions.setPlutoTile(true);
 * // To tile the code with PTile (balanced Pluto fusion heuristic + parametric tiling):
 * polyoptions.setCodegenUsePtile(true);
 * // To parallelize the code with pluto (balanced fusion heuristic):
 * polyoptions.setPlutoParralel(true);
 * // To pre-vectorize the code with pluto (balanced fusion heuristic):
 * polyoptions.setPlutoPrevector(true);
 * // To generate OpenMP pragmas as string annotations of the tree:
 * polyoptions.setGeneratePragmas(true);
 *
 * // Then invoke the driver. For working on a full project:
 * polyOptDriver(project, polyoptions);
 * // or on an arbitrary sub-tree:
 * polyOptDriver(node, polyoptions);
 *
 */


/**************************************************************************/
/********************** PolyOpt processing routines ***********************/
/**************************************************************************/


/**
 * PolyOpt entry point, for a SgProject processing.
 *
 */
int
PolyOptDriver(SgProject* project, PolyRoseOptions& polyoptions);


/**
 * PolyOpt entry point, for a SgNode processing.
 *
 */
int
PolyOptDriver(SgNode* root, PolyRoseOptions& polyoptions);



#endif
