/*
 * PolyOptISLRepresentation.cpp: This file is part of the PolyOpt project.
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
 * @file: PolyOptISLRepresentation.cpp
 * @author: Louis-Noel Pouchet <pouchet@colostate.edu>
 */

#include <string.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include <fstream>

// LNP: To avoid redefined macro errors in pocc.
/// LNP: FIXME: Find a better solution!
#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#include <rose.h>
#include <Cxx_Grammar.h>
#include <AstTraversal.h>

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
#ifdef Q
# undef Q
#endif

#include <past/past.h>
#include <past/pprint.h>
#include <polyfeat/islinterface.h>
#include <isl/id.h>

#include <polyopt/PolyOptISLRepresentation.hpp>



PolyOptISLRepresentation::PolyOptISLRepresentation()
{
  scop_nb_arrays = 0;
  scop_nb_statements = 0;
  scop_reads = NULL;
  scop_writes = NULL;
  scop_scheds = NULL;
  stmt_accfunc_read = std::vector<isl_union_map*>();
  stmt_accfunc_write = std::vector<isl_union_map*>();
  stmt_read_domain = std::vector<isl_union_map*>();
  stmt_write_domain = std::vector<isl_union_map*>();
  stmt_schedule = std::vector<isl_map*>();
  stmt_iterdom = std::vector<isl_set*>();
  stmt_body = std::vector<std::string>();
  stmt_body_ir = std::vector<SgNode*>();
}

PolyOptISLRepresentation::~PolyOptISLRepresentation()
{
  // No deletion here, leaking instead.
}

static
int compute_array_dimensionality (scoplib_scop_p scop, int array_id)
{
  scoplib_statement_p s;
  int max_dim = 0;
  scoplib_matrix_p m;
  int cnt;
  for (s = scop->statement; s; s = s->next)
    for (m = s->read, cnt = 0; m && cnt < 2; m = s->write, cnt++)
      {
	int j;
	for (j = 0; j < m->NbRows; ++j)
	  if (SCOPVAL_get_si(m->p[j][0]) == array_id)
	    {
	      max_dim = 1; ++j;
	      while (j < m->NbRows && SCOPVAL_get_si(m->p[j][0]) == 0)
		{
		  ++max_dim;
		  ++j;
		}
	    }
      }

  return max_dim;
}


static
isl_space* build_isl_space(scoplib_scop_p scop, scoplib_statement_p s,
			                  int array_id, isl_ctx* ctxt)
{
  isl_space* space;
  int i;
  int dout = compute_array_dimensionality (scop, array_id);

  space = isl_space_alloc(ctxt, scop->nb_parameters, s->nb_iterators, dout);
  const char* name;
  // Set the names.
  for (i = 0; i < s->nb_iterators; ++i)
    {
      name = strdup (((SgVariableSymbol*)(s->iterators[i]))->get_name().str());
      space = isl_space_set_dim_name (space, isl_dim_in, i, name);
    }
  for (i = 0; i < scop->nb_parameters; ++i)
    {
      name = strdup (((SgVariableSymbol*)(scop->parameters[i]))->get_name().str());
      space = isl_space_set_dim_name (space, isl_dim_param, i, name);
    }
  char buffer[32];
  for (i = 0; i < dout; ++i)
    {
      sprintf (buffer, "o%d", i);
      name = strdup (buffer);
      space = isl_space_set_dim_name (space, isl_dim_out, i, name);
    }

  return space;
}

static
isl_map* build_access_function (scoplib_scop_p scop,
				                    scoplib_statement_p s,
				                    scoplib_matrix_p m,
						    isl_space* space,
						    isl_ctx* ctxt,
						    int* row_pos,
						    int array_id)
{
  isl_map* ret = NULL;
  int i;
  isl_val* tmp = isl_val_int_from_si (ctxt, 0);
  for (i = *row_pos; i < m->NbRows; ++i)
    {
      if (SCOPVAL_get_si(m->p[i][0]) == array_id)
	{
	  ret = isl_map_universe (isl_space_copy (space));
	  int pos = 0;
	  do
	    {
	      isl_local_space* ls =
		isl_local_space_from_space (isl_space_copy (space));
	      isl_constraint* cst = isl_equality_alloc (isl_local_space_copy (ls));

	      // Set input dimensions.
	      int k;
	      for (k = 0; k < s->nb_iterators; ++k)
		{
		  tmp = isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][k+1]));
		  cst = isl_constraint_set_coefficient_val (cst, isl_dim_in, k, isl_val_copy(tmp));
		}
	      for (k = 0; k < scop->nb_parameters; ++k)
		{
		  tmp =
		    isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][k+1+s->nb_iterators]));
		  cst =
		    isl_constraint_set_coefficient_val (cst, isl_dim_param, k,
							isl_val_copy (tmp));
		}
	      tmp = isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][m->NbColumns - 1]));
	      cst = isl_constraint_set_constant_val (cst, isl_val_copy(tmp));
	      // Set output dimension.
	      tmp = isl_val_set_si (tmp, -1);
	      isl_constraint_set_coefficient_val (cst, isl_dim_out, pos++, isl_val_copy (tmp));

	      // Insert constraint.
	      ret = isl_map_add_constraint (ret, cst);
	      ++i;
	    }
	  while (i < m->NbRows && SCOPVAL_get_si(m->p[i][0]) == 0);
	  *row_pos = i;
	  break;
       }
    }
  isl_val_free (tmp);

  return ret;
}



static
isl_set* build_iteration_domain (scoplib_scop_p scop,
				                   scoplib_statement_p s,
						    isl_space* space,
						    isl_ctx* ctxt)
{
  isl_set* ret = isl_set_universe (isl_space_domain (isl_space_copy (space)));
  int i;
  isl_val* tmp = isl_val_int_from_si (ctxt, 0);
  scoplib_matrix_p m = s->domain->elt;
  for (i = 0; i < m->NbRows; ++i)
    {
      isl_local_space* ls =
  	isl_local_space_from_space (isl_set_get_space (ret));
      isl_constraint* cst;
      if (SCOPVAL_get_si(m->p[i][0]) == 0)
  	cst = isl_equality_alloc (isl_local_space_copy (ls));
      else
  	cst = isl_inequality_alloc (isl_local_space_copy (ls));

      // Set input dimensions.
      int k;
      for (k = 0; k < s->nb_iterators; ++k)
  	{
  	  tmp = isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][k+1]));
  	  cst = isl_constraint_set_coefficient_val (cst, isl_dim_set, k, isl_val_copy (tmp));
  	}
      for (k = 0; k < scop->nb_parameters; ++k)
  	{
  	  tmp =
  	    isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][k+1+s->nb_iterators]));
  	  cst =
  	    isl_constraint_set_coefficient_val (cst, isl_dim_param, k,
  						isl_val_copy (tmp));
  	}
      tmp = isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][m->NbColumns - 1]));
      cst = isl_constraint_set_constant_val (cst, isl_val_copy (tmp));

      // Insert constraint.
      ret = isl_set_add_constraint (ret, cst);
    }
  isl_val_free (tmp);

  return ret;
}


static
isl_map* build_schedule (scoplib_scop_p scop,
				         scoplib_statement_p s,
			                 isl_space* sp,
			                 isl_ctx* ctxt)
{
  // Set up the space.
  isl_space* space_in  = isl_space_domain (isl_space_copy (sp));
  int dout = s->schedule->NbRows;
  isl_space* space_out = isl_space_set_alloc(ctxt, scop->nb_parameters, dout);
  int i;
  char buffer[32];
  char* name;
  for (i = 0; i < dout; ++i)
    {
      sprintf (buffer, "t%d", i);
      name = strdup (buffer);
      space_out = isl_space_set_dim_name (space_out, isl_dim_set, i, name);
    }
  for (i = 0; i < scop->nb_parameters; ++i)
    {
      name = strdup (((SgVariableSymbol*)(scop->parameters[i]))->get_name().str());
      space_out = isl_space_set_dim_name (space_out, isl_dim_param, i, name);
    }

  isl_space* space = isl_space_map_from_domain_and_range
    (isl_space_copy (space_in), isl_space_copy (space_out));
  isl_map* ret = isl_map_universe (isl_space_copy (space));
  isl_val* tmp = isl_val_int_from_si (ctxt, 0);
  scoplib_matrix_p m = s->schedule;
  for (i = 0; i < m->NbRows; ++i)
    {
      isl_local_space* ls =
  	isl_local_space_from_space (isl_space_copy (space));
      isl_constraint* cst;
      if (SCOPVAL_get_si(m->p[i][0]) == 0)
  	cst = isl_equality_alloc (isl_local_space_copy (ls));
      else
  	cst = isl_inequality_alloc (isl_local_space_copy (ls));

      // Set input dimensions.
      int k;
      for (k = 0; k < s->nb_iterators; ++k)
  	{
  	  tmp = isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][k+1]));
  	  cst = isl_constraint_set_coefficient_val (cst, isl_dim_in, k, isl_val_copy (tmp));
  	}
      for (k = 0; k < scop->nb_parameters; ++k)
  	{
  	  tmp =
  	    isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][k+1+s->nb_iterators]));
  	  cst =
  	    isl_constraint_set_coefficient_val (cst, isl_dim_param, k,
  						isl_val_copy (tmp));
  	}
      tmp = isl_val_set_si (tmp, SCOPVAL_get_si(m->p[i][m->NbColumns - 1]));
      cst = isl_constraint_set_constant_val (cst, isl_val_copy (tmp));

      // Set output dimension.
      tmp = isl_val_set_si (tmp, -1);
      isl_constraint_set_coefficient_val (cst, isl_dim_out, i, isl_val_copy (tmp));

      // Insert constraint.
      ret = isl_map_add_constraint (ret, cst);
    }
  isl_val_free (tmp);

  return ret;
}


/**
 * Converts a SCoP as extracted by PolyOpt's auto-scop detection
 * into ISL representation.
 *
 * bugs/limitations:
 *    (a) not robust to union of iteration domains in scoplib
 *    (b) code is leaking, need proper copy constructor that duplicates all
 *          ISL structures.
 */
int
PolyOptISLRepresentation::convertScoplibToISL (scoplib_scop_p scop)
{
  int i;
  isl_union_map* all_reads = NULL;
  isl_union_map* all_writes = NULL;
  isl_union_map* all_scheds = NULL;
  isl_ctx* ctxt = isl_ctx_alloc();

  // 1. Prepare the arrays of unique names for statements and arrays.
  char buffer[32];
  int nb_statements;
  scoplib_statement_p s;
  for (nb_statements = 0, s = scop->statement; s; s = s->next, nb_statements++)
    ;
  char* stmt_names[nb_statements];
  for (i = 0; i < nb_statements; ++i)
    {
      sprintf (buffer, "S_%d", i);
      stmt_names[i] = strdup (buffer);
    }
  char* array_names[scop->nb_arrays];
  for (i = 0; i < scop->nb_arrays; ++i)
    array_names[i] =
      strdup (((SgVariableSymbol*)(scop->arrays[i]))->get_name().str());

  isl_union_map* umap;
  int stmt_id;
  for (s = scop->statement, stmt_id = 0; s; s = s->next, ++stmt_id)
    {
      isl_union_map* all_reads_stmt = NULL;
      isl_union_map* all_writes_stmt = NULL;
      isl_space* sp = NULL;
      for (i = 0; i < scop->nb_arrays; ++i)
	{
	  sp = build_isl_space (scop, s, i+1, ctxt);

	  // 1. Handle access matrices.
	  scoplib_matrix_p m;
	  int k;
	  for (k = 0, m = s->read, umap = all_reads_stmt; k < 2;
	       k++, m = s->write, umap = all_writes_stmt)
	    {
	      isl_map* acc_map = NULL;
	      int row_pos = 0;
	      do
		{
		  acc_map = build_access_function
		    (scop, s, m, sp, ctxt, &row_pos, i+1);
		  if (acc_map)
		    {
		      acc_map = isl_map_set_tuple_name
			(acc_map, isl_dim_in, stmt_names[stmt_id]);
		      acc_map = isl_map_set_tuple_name
			(acc_map, isl_dim_out, array_names[i]);
		      if (umap == NULL)
			umap = isl_union_map_from_map (isl_map_copy (acc_map));
		      else
			umap = isl_union_map_union
			  (umap, isl_union_map_from_map (isl_map_copy (acc_map)));
		      isl_map_free (acc_map);
		    }
		}
	      while (acc_map != NULL);
	      if (k == 0)
		all_reads_stmt = umap;
	      else
		all_writes_stmt = umap;
	    }
	}
      // Store the union of access functions of statement i.
      stmt_accfunc_read.push_back (all_reads_stmt);
      stmt_accfunc_write.push_back (all_writes_stmt);

      // 2. Handle iteration domains.
      isl_set* dom = build_iteration_domain (scop, s, sp, ctxt);
      dom = isl_set_set_tuple_name (dom, stmt_names[stmt_id]);
      if (all_reads_stmt != NULL)
	all_reads_stmt = isl_union_map_intersect_domain
	  (isl_union_map_copy (all_reads_stmt),
	   isl_union_set_from_set (isl_set_copy (dom)));
      if (all_writes_stmt != NULL)
	all_writes_stmt = isl_union_map_intersect_domain
	  (all_writes_stmt, isl_union_set_from_set (isl_set_copy (dom)));

      // Store the iteration domain of statement i.
      stmt_iterdom.push_back (dom);
      // Store the union of access functions of statement i after intersection by domain.
      stmt_read_domain.push_back (all_reads_stmt);
      stmt_write_domain.push_back (all_writes_stmt);

      // Unionize the result.
      if (all_reads == NULL)
	all_reads = isl_union_map_copy (all_reads_stmt);
      else
	all_reads = isl_union_map_union
	  (all_reads, isl_union_map_copy (all_reads_stmt));
      if (all_writes == NULL)
	all_writes = isl_union_map_copy (all_writes_stmt);
      else
	all_writes = isl_union_map_union
	  (all_writes, isl_union_map_copy (all_writes_stmt));
      // isl_union_map_free (all_reads_stmt);
      // isl_union_map_free (all_writes_stmt);

      // 3. Handle schedules.
      isl_map* sched = build_schedule (scop, s, sp, ctxt);
      sched = isl_map_set_tuple_name (sched, isl_dim_in, stmt_names[stmt_id]);
      if (all_scheds == NULL)
	all_scheds = isl_union_map_from_map (isl_map_copy (sched));
      else
	all_scheds = isl_union_map_union
	  (all_scheds, isl_union_map_from_map (isl_map_copy (sched)));
      // Store the schedule of statement i.
      stmt_schedule.push_back (sched);

      // 4. Finalize info about the statement.
      stmt_body.push_back (((SgNode*)(s->body))->unparseToCompleteString());
      stmt_body_ir.push_back ((SgNode*)(s->body));
    }

  // // Debug.
  // isl_printer* pr = isl_printer_to_file (ctxt, stdout);
  // std::cout << "UNION MAP READS" << std::endl;
  // isl_printer_print_union_map(pr, all_reads);
  // printf ("\n");
  // std::cout << "UNION MAP WRITES" << std::endl;
  // isl_printer_print_union_map(pr, all_writes);
  // printf ("\n");
  // std::cout << "UNION MAP SCHEDULES" << std::endl;
  // isl_printer_print_union_map(pr, all_scheds);
  // printf ("\n");
  for (std::vector<std::string>::iterator i = stmt_body.begin();
       i != stmt_body.end(); ++i)
    std::cout << "stmt body: " << *i << std::endl;
  // Finalize SCoP representation.
  scop_nb_arrays = scop->nb_arrays;
  scop_nb_statements = stmt_schedule.size();
  scop_reads = all_reads;
  scop_writes = all_writes;
  scop_scheds = all_scheds;

  return EXIT_SUCCESS;
}
