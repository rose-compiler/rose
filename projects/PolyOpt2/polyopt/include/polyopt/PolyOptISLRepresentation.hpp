/*
 * PolyOptISLRepresentation.hpp: This file is part of the PolyOpt project.
 *
 * PolyOpt: a Polyhedral Optimizer for the ROSE compiler
 *
 * Copyright (C) 2017 the Ohio State University
 *
 * This program can be redistributed and/or modified under the terms
 * of the license specified in the LICENSE.txt file at the root of the
 * project.
 *
 * Contact: P. Sadayappan <saday@cse.ohio-state.edu>
 *
 */
/**
 * @file: PolyOptISLRepresentation.hpp
 * @author: Louis-Noel Pouchet <pouchet@colostate.edu>
 */

#ifndef POLYOPT_POLYOPTISLREPRESENTATION_HPP
# define POLYOPT_POLYOPTISLREPRESENTATION_HPP

#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#include <rose.h>

#ifndef SCOPLIB_INT_T_IS_LONGLONG
# define SCOPLIB_INT_T_IS_LONGLONG
#endif
#include <scoplib/scop.h>
#ifndef CLOOG_INT_GMP
# define CLOOG_INT_GMP
#endif
#include <cloog/cloog.h>
#ifndef LINEAR_VALUE_IS_LONGLONG
# define LINEAR_VALUE_IS_LONGLONG
#endif
/// Stupid PolyLib which #def Q if(...) inside...
#ifdef Q
# undef Q
#endif
#include <isl/map.h>
#include <isl/set.h>

#include <polyopt/PolyRoseOptions.hpp>



class PolyOptISLRepresentation {
public:
  PolyOptISLRepresentation();
  // PolyOptISLRepresentation(PolyOptISLRepresentation& r);
  ~PolyOptISLRepresentation();

  // SCoP representation. One entry per statement in each vector below.
  int scop_nb_arrays;
  int scop_nb_statements;
  // Access functions, no domain info.
  std::vector<isl_union_map*> stmt_accfunc_read;
  std::vector<isl_union_map*> stmt_accfunc_write;
  // Access functions intersected with iteration domain
  std::vector<isl_union_map*> stmt_read_domain;
  std::vector<isl_union_map*> stmt_write_domain;
  // Schedule
  std::vector<isl_map*> stmt_schedule;
  // Iteration domain
  std::vector<isl_set*> stmt_iterdom;
  // Statement body (text representation).
  std::vector<std::string> stmt_body;
  // Union of all access functions intersected with domain.
  isl_union_map* scop_reads;
  // Union of all access functions intersected with domain.
  isl_union_map* scop_writes;
  // Union of all schedules.
  isl_union_map* scop_scheds;

  // Convert a SCoP produced by PolyOpt into ISL format as above.
  int convertScoplibToISL (scoplib_scop_p scop);
};



#endif
