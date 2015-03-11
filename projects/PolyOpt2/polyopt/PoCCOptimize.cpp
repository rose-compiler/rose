/*
 * PoCCOptimize.cpp: This file is part of the PolyOpt project.
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
 * @file: PoCCOptimize.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#include <iostream>
#include <vector>

// LNP: To avoid redefined macro errors in pocc.
/// LNP: FIXME: Find a better solution!
#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_NAME
#undef PACKAGE_VERSION
#include <rose.h>
#include <AstTraversal.h>
#ifndef SCOPLIB_INT_T_IS_LONGLONG
# define SCOPLIB_INT_T_IS_LONGLONG
#endif
#include <scoplib/scop.h>
#ifndef CLOOG_INT_GMP
# define CLOOG_INT_GMP
#endif
#include <cloog/cloog.h>
#include <pragmatize/pragmatize.h>
#include <clastannotation/annotate.h>
#include <pocc/options.h>
#include <pocc-utils/options.h>
#include <pocc/driver-candl.h>
#include <pocc/driver-letsee.h>
#include <pocc/driver-pluto.h>
#include <pocc/driver-codegen.h>
#include <pocc/driver-clastops.h>
#include <candl/options.h>
#include <candl/candl.h>

#include <polyopt/SageNodeAnnotation.hpp>
#include <polyopt/SageTools.hpp>
#include <polyopt/PoCCOptimize.hpp>
#include <polyopt/PolyRoseOptions.hpp>

#include <clasttools/clast2past.h>

#include <ptile/PTile.hpp>
#include <ptile/options.h>
#include <ptile/pocc_driver.h>

#include <irconverter/past2scop.h>

#include <past/pprint.h>


static
CloogMatrix* convert_to_cloogmatrix(scoplib_matrix_p mat)
{
  CloogMatrix* ret = cloog_matrix_alloc (mat->NbRows, mat->NbColumns);

  int i, j;
  for (i = 0; i < mat->NbRows; ++i)
    for (j = 0; j < mat->NbColumns; ++j)
      cloog_int_set_si(ret->p[i][j], SCOPVAL_get_si(mat->p[i][j]));

  return ret;
}


/**
 * Create a CLooG schedule from a pluto scattering.
 * What a mess for tiling into the scattering!
 *
 */
static
scoplib_matrix_p cloogify_schedule(scoplib_matrix_p mat,
				   int nb_scatt, int nb_par)
{
  int i, j, k, l;
  int nb_ineq;
  int nb_eq;

  // Count the number of inequalities. Foolishly assume two
  // inequalities per tloop...
  for (i = 0, nb_ineq = 0, nb_eq = 0; i < mat->NbRows; ++i)
    {
      if (SCOPVAL_get_si(mat->p[i][0]) == 1)
	++nb_ineq;
      else
	++nb_eq;
    }
  int nb_tile_loops = nb_ineq / 2;
  nb_scatt -= nb_tile_loops;

  // Allocate new schedule. 'mat' already contains extra columns for
  // the tloop.
  scoplib_matrix_p ret =
    scoplib_matrix_malloc (nb_scatt + nb_ineq,
			   mat->NbColumns + nb_scatt);

  // -I for the scattering.
  for (i = 0; i < nb_scatt; ++i)
    SCOPVAL_set_si(ret->p[i][i + 1], -1);
  int neq = 0;
  // Copy the RHS of the schedule (that connects to actual iterations).
  for (i = 0; i < mat->NbRows; ++i)
    {
      if (SCOPVAL_get_si(mat->p[i][0]) == 0)
	{
	  SCOPVAL_set_si(ret->p[i][0], 0);
	  // Equality defining the schedule.
	  for (j = 1; j < mat->NbColumns; ++j)
	    SCOPVAL_set_si(ret->p[i][j + nb_scatt],
			   SCOPVAL_get_si(mat->p[i][j]));
	  ++neq;
	}
      else
	{
	  // Inequality defining the domain of the scattering.
	  SCOPVAL_set_si(ret->p[i][0], 1);
	  for (j = 0; j < neq; ++j)
	    if (SCOPVAL_get_si(mat->p[j][1 + (i - neq) / 2]) != 0)
	      break;
	  if (j < neq)
	    {
	      for (j = 1; j < mat->NbColumns; ++j)
		SCOPVAL_set_si(ret->p[i][j + nb_scatt],
			       SCOPVAL_get_si(mat->p[i][j]));
	    }
	  else
	    {
	      SCOPVAL_set_si(ret->p[i][0], 0);
	      SCOPVAL_set_si(ret->p[i][1 + (i - neq) / 2 + neq], -1);
	      ++i;
	    }
	}
    }

  return ret;
}


static
CloogUnionDomain
*cloog_union_domain_set_name_nodup(CloogUnionDomain *ud,
				   enum cloog_dim_type type,
				   int index,
				   const char *name)
{
  int i;

  if (!ud)
    return ud;

  if (type != CLOOG_PARAM &&
      type != CLOOG_ITER &&
      type != CLOOG_SCAT)
    cloog_die("invalid dim type\n");

  if (index < 0 || index >= ud->n_name[type])
    cloog_die("index out of range\n");

  if (!ud->name[type]) {
    ud->name[type] = XMALLOC(char *, ud->n_name[type]);
    if (!ud->name[type])
      cloog_die("memory overflow.\n");
    for (i = 0; i < ud->n_name[type]; ++i)
      ud->name[type][i] = NULL;
  }

  free(ud->name[type][index]);
  ud->name[type][index] = (char*) name;
  if (!ud->name[type][index])
    cloog_die("memory overflow.\n");

  return ud;
}


/**
 * Code generation.
 *
 */
clast_stmt*
pocc_pace_codegen (scoplib_scop_p program,
		   s_pocc_options_t* poptions,
		   s_pocc_utils_options_t* puoptions,
		   bool isQuiet)
{
  CloogMatrix* cmat;
  CloogInput* input;
  struct clast_stmt* root;

  // Get the CloogOptons structure.
  CloogOptions* coptions = poptions->cloog_options;
  if (coptions == NULL)
    {
      CloogState* cstate = cloog_state_malloc ();
      poptions->cloog_options = coptions = cloog_options_malloc (cstate);
    }
  coptions->language = 'c';
  if (isQuiet)
    coptions->quiet = 1;

  // Convert context.
  cmat = convert_to_cloogmatrix (program->context);
  CloogDomain* context =
    cloog_domain_from_cloog_matrix (coptions->state, cmat,
				    program->nb_parameters);
  cloog_matrix_free (cmat);

  // Convert statements.
  CloogUnionDomain* ud = cloog_union_domain_alloc (program->nb_parameters);

  // Update parameter names.
  int i;
  for (i = 0; i < program->nb_parameters; ++i)
    ud = cloog_union_domain_set_name_nodup
      (ud, CLOOG_PARAM, i, (const char*) (program->parameters[i]));

  // Iterate on statements, and create one per component of the union
  // in a domain.
  scoplib_statement_p stm;
  CloogDomain* dom;
  CloogScattering* scat;
  scoplib_matrix_p mat;
  char** names;
  int nb_tile_loops;

  // Collect the maximal scattering dimensionality.
  int nb_scatt = 0;
  for (stm = program->statement; stm; stm = stm->next)
    {
      int nb_eq = 0;
      for (i = 0; i < stm->schedule->NbRows; ++i)
	if (SCOPVAL_get_si(stm->schedule->p[i][0]) == 0)
	  ++nb_eq;
      int cur_scatt = nb_eq + (stm->schedule->NbColumns -
			       stm->domain->elt->NbColumns);
      nb_scatt = nb_scatt > cur_scatt ? nb_scatt : cur_scatt;
    }

  for (stm = program->statement; stm; stm = stm->next)
    {
      scoplib_matrix_list_p smat;
      for (smat = stm->domain; smat; smat = smat->next)
	{
	  mat = (scoplib_matrix_p) smat->elt;
	  cmat = convert_to_cloogmatrix (mat);
	  dom = cloog_domain_from_cloog_matrix (coptions->state, cmat,
						program->nb_parameters);
	  cloog_matrix_free (cmat);
	  scoplib_matrix_p newsched =
	    cloogify_schedule ((scoplib_matrix_p) stm->schedule, nb_scatt,
			       program->nb_parameters);
	  int total_scatt_dims = newsched->NbColumns - mat->NbColumns;
	  cmat = convert_to_cloogmatrix (newsched);
	  scoplib_matrix_free (newsched);
	  scat = cloog_scattering_from_cloog_matrix (coptions->state, cmat,
						     total_scatt_dims,
						     program->nb_parameters);
	  ud = cloog_union_domain_add_domain (ud, NULL, dom, scat, NULL);
	  cloog_matrix_free (cmat);
	}
    }
  // Store the scattering names.
  int idx = 0;

  // 1- Find the list of symbols used in the SCoP.
  std::set<SgVariableSymbol*> usedSymbols;
  SgStatement* stmt = (SgStatement*)program->usr;
  if (stmt)
    {
      ScopRootAnnotation* annot =
	(ScopRootAnnotation*)(stmt->getAttribute("ScopRoot"));
      ROSE_ASSERT(annot);
      std::map<std::string, SgVariableSymbol*> fakeSymbolMap =
	annot->fakeSymbolMap;
      std::vector<SgNode*> readRefs;
      std::vector<SgNode*> writeRefs;
      bool collect =
	SageTools::collectReadWriteRefs(stmt, readRefs, writeRefs, 1);
      ROSE_ASSERT(collect);
      if (collect)
	{
	  std::set<SgVariableSymbol*> readSymbs =
	    SageTools::convertToSymbolSet(readRefs, fakeSymbolMap);
	  std::set<SgVariableSymbol*> writeSymbs =
	    SageTools::convertToSymbolSet(writeRefs, fakeSymbolMap);
	  set_union(readSymbs.begin(), readSymbs.end(),
		    writeSymbs.begin(), writeSymbs.end(),
		    std::inserter(usedSymbols, usedSymbols.begin()));
	}
    }

  for (i = 0; i < nb_scatt; ++i)
    {
      char buffer[16];
      bool isUsedSymbol;
      do
	{
	  // Ensure we will declare an unused symbol for the new loop
	  // iterators.
	  isUsedSymbol = false;
	  sprintf (buffer, "c%d", idx++);
	  std::set<SgVariableSymbol*>::const_iterator j;
	  for (j = usedSymbols.begin(); j != usedSymbols.end(); ++j)
	    if (*j && ! strcmp((*j)->get_name().str(), buffer))
	      {
		isUsedSymbol = true;
		break;
	      }
	}
      while (isUsedSymbol);
      ud = cloog_union_domain_set_name (ud, CLOOG_SCAT, i,
					buffer);
    }
  input = cloog_input_alloc (context, ud);

  // Generate the clast.
  root = cloog_clast_create_from_input (input, coptions);

  /// FIXME: reactivate this. input must be passed
  // cloog_input_free (input);

  // printf ("All done after cloog\n");
  // clast_pprint(stdout, root, 0, coptions);

  /// LNP: FIXME: We should not need to do this.
  cloog_options_free (coptions);
  poptions->cloog_options = NULL;

  return root;
}

static
void
substitute_name_in_expr (struct clast_expr* e,
	      char* original_name,
	      void* new_name)
{
  int i;

  if (!e)
    return;
  switch (e->type)
    {
    case clast_expr_name:
      {
	struct clast_name* n = (struct clast_name*) e;
	if (! strcmp (n->name, original_name))
	  n->name = (const char*) new_name;
	break;
      }
    case clast_expr_term:
      substitute_name_in_expr (((struct clast_term*)e)->var,
			       original_name, new_name);
      break;
    case clast_expr_red:
      {
	struct clast_reduction* r = (struct clast_reduction*) e;
	for (i = 0; i < r->n; ++i)
	  substitute_name_in_expr (r->elts[i], original_name, new_name);
	break;
      }
    case clast_expr_bin:
      substitute_name_in_expr (((struct clast_binary*) e)->LHS,
			       original_name, new_name);
      break;
    default:
      assert(0);
    }
}


static
void
substitute_name_in_clast (struct clast_stmt* s,
			  char* original_name,
			  void* new_name)
{
  for ( ; s; s = s->next)
    {
      if (CLAST_STMT_IS_A(s, stmt_root))
	continue;
      if (CLAST_STMT_IS_A(s, stmt_ass))
	substitute_name_in_expr (((struct clast_assignment *) s)->RHS,
		      original_name, new_name);
      else if (CLAST_STMT_IS_A(s, stmt_user))
	{
	  struct clast_stmt* i = ((struct clast_user_stmt*)s)->substitutions;
	  for ( ; i; i = i->next)
	    substitute_name_in_clast (i, original_name, new_name);
	}
      else if (CLAST_STMT_IS_A(s, stmt_for))
	{
	  struct clast_for* sfor = (struct clast_for*) s;
	  substitute_name_in_expr (sfor->LB, original_name, new_name);
	  substitute_name_in_expr (sfor->UB, original_name, new_name);
	  substitute_name_in_clast (sfor->body, original_name, new_name);
	}
      else if (CLAST_STMT_IS_A(s, stmt_guard))
	{
	  struct clast_guard* sguard = (struct clast_guard*) s;
	  int j;
	  for (j = 0; j < sguard->n; ++j)
	    {
	      struct clast_equation* seq = &sguard->eq[j];
	      substitute_name_in_expr (seq->RHS, original_name, new_name);
	      substitute_name_in_expr (seq->LHS, original_name, new_name);
	    }
	  substitute_name_in_clast (sguard->then, original_name, new_name);
	}
      else if (CLAST_STMT_IS_A(s, stmt_block))
	substitute_name_in_clast (((struct clast_block *)s)->body,
				  original_name, new_name);
      else
	assert(0);
    }
}


static
struct clast_stmt*
OptimizeSingleScopWithPoccGeneric (scoplib_scop_p scop,
				   s_pocc_options_t* poptions,
				   s_pocc_utils_options_t* puoptions,
				   std::vector<SgNode*>& sageParameters,
				   PolyRoseOptions& polyoptions,
				   std::set<int>& privateVars)
{
  if (! scop || scop->statement == NULL)
    {
      fprintf (stderr, "[PoCC] Possible parsing error: no statement in SCoP");
      exit (1);
    }
  // (1) Temporary conversion of Sage nodes (iterators, parameters,
  // etc.) into equivalent char* (for nice verbose output in pocc).
  std::vector<SgNode*> sageIterators;

  std::vector<SgNode*> sageArrays;
  std::vector<SgNode*> sageStatementBodies;
  int i;
  for (i = 0; i < scop->nb_parameters; ++i)
    {
      SgVariableSymbol* symbol =
	isSgVariableSymbol((SgNode*) scop->parameters[i]);
      sageParameters.push_back(symbol);
      scop->parameters[i] = strdup(symbol->get_name().getString().c_str());
    }
  for (i = 0; i < scop->nb_arrays; ++i)
    {
      SgVariableSymbol* symbol =
	isSgVariableSymbol((SgNode*) scop->arrays[i]);
      sageArrays.push_back(symbol);
      scop->arrays[i] = strdup(symbol->get_name().getString().c_str());
    }
  scoplib_statement_p stm;
  for (stm = scop->statement; stm; stm = stm->next)
    {
      for (i = 0; i < stm->nb_iterators; ++i)
	{
	  SgVariableSymbol* symbol =
	    isSgVariableSymbol((SgNode*) stm->iterators[i]);
	  sageIterators.push_back(symbol);
	  stm->iterators[i] = strdup(symbol->get_name().getString().c_str());
	}
      sageStatementBodies.push_back((SgNode*) stm->body);
      stm->body =
	strdup(((SgNode*) stm->body)->unparseToCompleteString().c_str());
    }

  // (2) If pass-thru, run candl.
  if (! poptions->letsee && ! poptions->pluto)
    pocc_driver_candl (scop, poptions, puoptions);

  // (2.b) Get privatized scalars.
  if (polyoptions.getScalarPrivatization())
    {
      CandlOptions* coptions = candl_options_malloc ();
      coptions->scalar_privatization = 1;
      CandlProgram* cprogram = candl_program_convert_scop (scop, NULL);
      CandlDependence* deps = candl_dependence (cprogram, coptions);
      for (int k = 0; cprogram->scalars_privatizable[k] != -1; k += 2)
	privateVars.insert(cprogram->scalars_privatizable[k]);

      candl_dependence_free (deps);
      candl_program_free (cprogram);
      candl_options_free (coptions);
    }

  // (3) Perform LetSee.
  if (poptions->letsee)
    pocc_driver_letsee (scop, poptions, puoptions);

  // (4) Perform PLuTo.
  // Don't do it if already performed through LetSee.
  /// LNP: This increases the "quality" of the transformation.
  poptions->pluto_bound_coefficients = 10;
  ///
  if (poptions->pluto && ! poptions->letsee)
    if (pocc_driver_pluto (scop, poptions, puoptions) == EXIT_FAILURE)
      exit (EXIT_FAILURE);

  // (5) Restore all Sage nodes BUT the parameters.
  for (i = 0; i < scop->nb_arrays; ++i)
    {
      free(scop->arrays[i]);
      scop->arrays[i] = (char*) sageArrays[i];
    }
  int count, j;
  for (stm = scop->statement, i = 0, count = 0; stm; stm = stm->next, ++i)
    {
      int nb_tiled_dim =
	stm->domain->elt->NbColumns - scop->context->NbColumns
	- stm->nb_iterators;
      char** old_iterators = stm->iterators;
      stm->iterators = XMALLOC(char*, stm->nb_iterators + nb_tiled_dim);
      for (j = 0; j < stm->nb_iterators; ++j)
	{
	  free(old_iterators[j]);
	  stm->iterators[j + nb_tiled_dim] =
		(char*) sageIterators[count++];
	}
      for (j = 0; j < nb_tiled_dim; ++j)
	stm->iterators[j] = NULL;
     stm->nb_iterators += nb_tiled_dim;

      free(stm->body);
      stm->body = (char*) sageStatementBodies[i];
    }

  // (6) Perform codgen.
  // Don't do it if already performed through LetSee.
  struct clast_stmt* root = NULL;
  if (poptions->codegen && ! poptions->letsee)
    root = pocc_pace_codegen (scop, poptions, puoptions, polyoptions.getQuiet());

  return root;
}


static
void traverse_variable(s_past_node_t* node, void* data)
{
  if (past_node_is_a(node, past_variable))
    {
      void** vals = (void**) data;
      char* oldval = (char*) vals[0];
      void* newval = vals[1];
      PAST_DECLARE_TYPED(variable, v, node);
      if (v->symbol->is_char_data)
	{
	  if (! strcmp ((char*) v->symbol->data, oldval))
	    {
	      v->symbol->data = newval;
	      v->symbol->is_char_data = 0;
	    }
	}
    }
}


static
void
substitute_name_in_past (s_past_node_t* root,
			 char* oldval,
			 SgNode* newval)
{
  void* data[2];
  data[0] = oldval;
  data[1] = newval;
  past_visitor (root, traverse_variable, data, NULL, NULL);
}


static
void traverse_update_parfor (s_past_node_t* node, void* data)
{
  if (past_node_is_a(node, past_parfor))
    {
      std::vector<SgNode*>* pv = (std::vector<SgNode*>*)data;
      PAST_DECLARE_TYPED(parfor, pf, node);
      pf->usr = new std::vector<SgNode*>(*pv);
    }
}



static
void
update_for_with_privvar_in_past (s_past_node_t* root,
				 scoplib_scop_p scop,
				 std::set<int>& privateVars)
{
  std::vector<SgNode*> pv;
  for (std::set<int>::iterator i = privateVars.begin();
       i != privateVars.end(); ++i)
    pv.push_back((SgNode*)scop->arrays[*i - 1]);
  past_visitor (root, traverse_update_parfor, &pv, NULL, NULL);
}


s_past_node_t*
OptimizeSingleScopWithPoccPast (scoplib_scop_p scop,
				s_pocc_options_t* poptions,
				s_pocc_utils_options_t* puoptions,
				PolyRoseOptions& polyoptions)
{
  std::vector<SgNode*> sageParameters;
  std::set<int> privateVars;
  clast_stmt* root =
    OptimizeSingleScopWithPoccGeneric (scop, poptions, puoptions,
				       sageParameters, polyoptions,
				       privateVars);

  s_past_node_t* proot = clast2past (root, 1);
  if (privateVars.size())
    {
      update_for_with_privvar_in_past (proot, scop, privateVars);
      ScopParentAnnotation* annot = new ScopParentAnnotation();
      for (std::set<int>::iterator i = privateVars.begin();
	   i != privateVars.end(); ++i)
	annot->privateVars.push_back((SgNode*)scop->arrays[*i - 1]);
      SgNode* node = (SgNode*)scop->usr;
      ROSE_ASSERT(node && node->get_parent());
      node = node->get_parent();
      node->setAttribute("PrivateVars", annot);
    }

  if (polyoptions.getCodegenUsePtile())
    {
      if (! polyoptions.getQuiet())
	std::cout << "[PolyOpt] Use Parametric Tiling module" << std::endl;
      // Set parent, just in case.
      past_set_parent (proot);

      // Extract scoplib representation.
      scoplib_scop_p control_scop = past2scop_control_only (proot, scop, 1);
      s_ptile_options_t* ptopts = ptile_options_malloc ();
      ptopts->fullTileSeparation = 0;
      ptopts->verbose_level = 1;
      ptopts->data_is_char = 1;
      ptopts->quiet = polyoptions.getQuiet();
      if (ptopts->quiet)
	ptopts->verbose_level = 0;
      if (! polyoptions.getQuiet())
	std::cout << "[PolyOpt] Start PTile" << std::endl;

      // Invoke PTile main driver. Will parametrically tile all tilable
      // components, by in-place modification of 'root'.
      ptile_pocc_driver (control_scop, proot, ptopts);

      // Be clean.
      scoplib_scop_shallow_free (control_scop);
      ptile_options_free (ptopts);
    }

  // (7) Substitute back the Sage nodes for parameters.
  // CLooG use some string functions (eg, strcmp), so it must be
  // done after codegen.
  for (int i = 0; i < scop->nb_parameters; ++i)
    {
      substitute_name_in_past (proot, scop->parameters[i], sageParameters[i]);
      free(scop->parameters[i]);
      scop->parameters[i] = (char*) sageParameters[i];
    }

  // (8) Perform CLAST annotation/translation, if required.
  if (poptions->clastannotation_pass)
    {
      if (! polyoptions.getQuiet())
	std::cerr << "[PolyOpt] PAST annotation pass not implemented yet"
		  << std::endl;
      assert(0);
    }

  return proot;
}


clast_stmt* OptimizeSingleScopWithPocc (scoplib_scop_p scop,
					s_pocc_options_t* poptions,
					s_pocc_utils_options_t* puoptions,
					PolyRoseOptions& polyoptions)
{
  std::vector<SgNode*> sageParameters;
  std::set<int> privateVars;
  struct clast_stmt* root =
    OptimizeSingleScopWithPoccGeneric (scop, poptions, puoptions,
				       sageParameters, polyoptions,
				       privateVars);
  // (7) Substitute back the Sage nodes for parameters.
  // CLooG use some string functions (eg, strcmp), so it must be
  // done after codegen.
  for (int i = 0; i < scop->nb_parameters; ++i)
    {
      substitute_name_in_clast (root, scop->parameters[i], sageParameters[i]);
      free(scop->parameters[i]);
      scop->parameters[i] = (char*) sageParameters[i];
    }

  // (8) Perform CLAST annotation/translation, if required.
  if (poptions->clastannotation_pass)
    clastannotation_annotate (root, scop);

  return root;
}




/**
 * Check if a loop is parallel. The scop must represents only one
 * loop, with an arbitraty number of statements in it.
 *
 *
 */
bool PoCCIsLoopParallel (scoplib_scop_p scop,
			 s_pocc_options_t* poptions,
			 s_pocc_utils_options_t* puoptions)
{
  ROSE_ASSERT(scop->statement && scop->statement->nb_iterators == 1);

  // Build the CandlProgram associated to the scop.
  CandlOptions* coptions = candl_options_malloc ();
  CandlProgram* cprogram = candl_program_convert_scop (scop, NULL);
  CandlDependence* deps = candl_dependence (cprogram, coptions);

  // Find the first loop index.
  int loopId = -1;
  int i, j;
  for (i = 0; i < cprogram->nb_statements; ++i)
    for (j = 0; j < cprogram->statement[i]->depth; ++j)
      {
	int val = cprogram->statement[i]->index[j];
	if (loopId == -1)
	  loopId = val;
	else
	  loopId = loopId < val ? loopId : val;
      }

  CandlDependence* tmp;
  for (tmp = deps; tmp; tmp = tmp->next)
    if (candl_dependence_is_loop_carried (cprogram, tmp, loopId))
      break;

  // Be clean.
  candl_dependence_free (deps);
  candl_program_free (cprogram);
  candl_options_free (coptions);

  return tmp == NULL;
}


static
void
createAlignmentExpression(SgNode* ref, SageMemoryReferenceInfo* annot)
{
  // Useful only for arrays.
  SgPntrArrRefExp* aref = isSgPntrArrRefExp(ref);
  if (! aref)
    return;

  // 1- Take the base address
  SgPntrArrRefExp* last;
  SgPntrArrRefExp* atmp = aref;
  while ((last = isSgPntrArrRefExp(atmp->get_lhs_operand())))
    atmp = last;
  SgVarRefExp* baseArray = isSgVarRefExp(atmp->get_lhs_operand());

  // 2- Take the expression of the last array dimension.
  SgExpression* exp = isSgExpression(aref->get_rhs_operand());
  SgTreeCopy treeCopy;
  SgExpression* newexp = isSgExpression(exp->copy(treeCopy));

  // 3- Substitute the inner-most loop index by its lower bound.
  // Get the inner-most for loop.
  SgForStatement* fstmt = NULL;
  SgNode* tmp = aref;
  while (tmp && ! fstmt)
    {
      fstmt = isSgForStatement(tmp);
      tmp = tmp->get_parent();
    }
  ROSE_ASSERT(fstmt);
  // Get its iterator symbol.
  SgVariableSymbol* innerLoopIteratorSymbol =
    SageTools::getLoopIteratorSymbol(fstmt);
  // Get its lower bound.
  SgForInitStatement* lb = fstmt->get_for_init_stmt();
  SgStatementPtrList initList = lb->get_init_stmt();
  ROSE_ASSERT(initList.size() == 1); // unbounded loops are not handled.
  SgExpression* innerLoopLowerBound = NULL;
  SgExprStatement* expstmt = isSgExprStatement(*(initList.begin()));
  if (! expstmt)
    {
      /// Maybe C99 declaration.
      SgStatement* init = *(initList.begin());
      // Allow C99 iterator declaration in for loop init.
      SgVariableDeclaration* vd = isSgVariableDeclaration(init);
      if (vd)
	{
	  SgInitializedName* name = *(vd->get_variables().begin());
	  SgAssignInitializer* initializer =
	    isSgAssignInitializer(name->get_initializer());
	  if (initializer)
	    innerLoopLowerBound = initializer->get_operand();
	}
    }
  else
    {
      SgAssignOp* aop = isSgAssignOp(expstmt->get_expression());
      if (aop)
	innerLoopLowerBound = aop->get_rhs_operand();
    }
  // If the loop has no lower bound, or we were unable to retrieve it, do
  // nothing.
  if (innerLoopLowerBound)
    {
      // ROSE doesn't support query on expressions, only on
      // statements. Encapsulate.
      SgExprStatement* retexpstmt = SageBuilder::buildExprStatement(newexp);
      Rose_STL_Container<SgNode*> varNodes =
	NodeQuery::querySubTree(retexpstmt, V_SgVarRefExp);
      Rose_STL_Container<SgNode*>::iterator varIter;
      for(varIter = varNodes.begin(); varIter != varNodes.end(); ++varIter)
	{
	  SgVarRefExp* varRefExp = isSgVarRefExp(*varIter);
	  if(varRefExp->get_symbol() == innerLoopIteratorSymbol)
	    {
	      SgExpression* newExpr =
		isSgExpression(innerLoopLowerBound->copy(treeCopy));
	      ROSE_ASSERT(newExpr);
	      SageInterface::replaceExpression(varRefExp, newExpr, false);
	    }
	}

      // 4- Store the information.
      annot->isMaybeAligned = 1;
      annot->alignmentExpression =
	SageBuilder::buildAddOp(baseArray, retexpstmt->get_expression());
      delete(retexpstmt);
    }
}



/**
 * Check if all memory accesses in a loop have stride-1 property. The
 * scop must represents only one loop, with an arbitraty number of
 * statements in it.
 *
 *
 */
bool PoCCIsLoopStrideOneAccesses (scoplib_scop_p scop,
				  s_pocc_options_t* poptions,
				  s_pocc_utils_options_t* puoptions)
{
  bool ret = true;

  // 1- Iterate on all statements
  scoplib_statement_p stm;
  for (stm = scop->statement; stm; stm = stm->next)
    {
      SgNode* ref;
      ScopStatementAnnotation* stmAnnot = (ScopStatementAnnotation*)
	((SgNode*)(stm->body))->getAttribute("ScopStmt");
      ROSE_ASSERT(stmAnnot);
      // 2- Iterate on all access functions in a statement.
      int k;
      scoplib_matrix_p mat;
      for (k = 0, mat = stm->read; k < 2; ++k, mat = stm->write)
	{
	  int count = 0;
	  // Ensure the stride-1 property: the loop iterator appears
	  // only for the last array dimension.
	  int i;
	  for (i = 0; i < mat->NbRows; ++i)
	    {
	      if (SCOPVAL_get_si(mat->p[i][0]) != 0)
		{
		  if (k == 0)
		    ref = stmAnnot->readAffineRefs[count++];
		  else
		    ref = stmAnnot->writeAffineRefs[count++];
		  SageMemoryReferenceInfo* annot =
		    new SageMemoryReferenceInfo();
		  // Reading a new array access.
		  int val = SCOPVAL_get_si(mat->p[i][1]);
		  bool isStrideOne = true;
		  while (i + 1 < mat->NbRows &&
			 SCOPVAL_get_si(mat->p[i + 1][0]) == 0)
		    {
		      if (val != 0)
			{
			  isStrideOne = false;
			  break;
			}
		      val = SCOPVAL_get_si(mat->p[i + 1][1]);
		      ++i;
		    }

		  if (isStrideOne)
		    {
		      if (val == 1)
			annot->isStrideOne = 1;
		      else if (val == 0)
			annot->isStrideZero = 1;
		      else
			isStrideOne = false;
		      if (isStrideOne)
			createAlignmentExpression(ref, annot);
		    }

		  ref->setAttribute(SageMemoryReferenceInfo::name(),
				    annot);
		  ret = ret && isStrideOne;
		}
	    }
	}
    }

  return ret;
}
