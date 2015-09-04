/*
 * PolyOpt.hpp: This file is part of the PolyOpt project.
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
 * @file: PolyOpt.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef POLYOPT_POLYOPT_HPP
# define POLYOPT_POLYOPT_HPP

#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#include <rose.h>

#include <polyopt/PolyRoseOptions.hpp>



/******************************************************************************/
/************************ PolyOpt Optimization routines ***********************/
/******************************************************************************/

/**
 * Transform a full Rose project using Polyhedral techniques.
 *
 *
 */
int PolyOptOptimizeProject(SgProject* project, PolyRoseOptions& polyoptions);

/**
 * Transform a sub-tree using Polyhedral techniques.
 *
 *
 */
int PolyOptOptimizeSubTree(SgNode* root, PolyRoseOptions& polyoptions);




/******************************************************************************/
/************************* PolyOpt Annotation routines ************************/
/******************************************************************************/


/**
 * Annotate inner-most loops of a whole ROSE project with
 * dependence/vectorization information (no transformation is
 * performed).
 *
 *
 */
int PolyOptInnerLoopsAnnotateProject(SgProject* project,
				     PolyRoseOptions& polyoptions);

/**
 * Annotate inner-most loops of a sub-tree with
 * dependence/vectorization information (no transformation is
 * performed).
 *
 *
 */
int PolyOptInnerLoopsAnnotateSubTree(SgNode* root,
				     PolyRoseOptions& polyoptions);


/**
 * Annotate a whole ROSE project with dependence/permutability
 * information (no transformation is performed).
 *
 *
 */
int PolyOptAnnotateProject(SgProject* project, PolyRoseOptions& polyoptions);

/**
 * Annotate a sub-tree with dependence/permutability information (no
 * transformation is performed).
 *
 *
 */
int PolyOptAnnotateSubTree(SgNode* root, PolyRoseOptions& polyoptions);


<<<<<<< HEAD
int PolyOptLoopTiling(SgForStatement* forStmt, int tileArg1, int tileArg2, int tileArg3);
=======
>>>>>>> 1b2ed609321865e886ba4d9a0bef7ea6b8ed4e2f
#endif
