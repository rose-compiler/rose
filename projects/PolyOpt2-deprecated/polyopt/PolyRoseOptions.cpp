/*
 * PolyRoseOptions.cpp: This file is part of the PolyOpt project.
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
 * @file: PolyRoseOptions.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <polyopt/PolyRoseOptions.hpp>

void
PolyRoseOptions::initialize()
{
  // Polyopt options.
  polyopt_generate_pragmas = false; // Default: do not generate
				    // omp/ivdep pragmas

  // Verbose option.
  pr_verbose = false;
  polyopt_quiet = false;

  // Scop Extraction options.
  sc_generic_scop_extractor = false;
  sc_scop_extractor_verbose_level = 0;
  sc_strict_scop_extractor = false;
  sc_unsafe_scop_extractor = true; // Default: unsafe scop extraction
				   // (assume no alias, no side
				   // effect.
  sc_approximate_scop_extractor = false;
  sc_f2c_scop_extractor = false;

  sc_allow_math_func = false;


  // Annotation functions.
  polyopt_annotate_only = false;
  polyopt_annotate_inner_loops = false;
  polyopt_compute_ddv = false;
  polyopt_hullify_ddv = false;


  // Dependence analysis.
  polyopt_scalar_privatization = false;

  // Generic optimization options.
  polyopt_fixed_tiling = false;
  polyopt_parametric_tiling = false;
  polyopt_parallel_only = false;

  // Codegen options.
  polyopt_codegen_use_past = true;
  polyopt_codegen_use_ptile = false;
  polyopt_insert_ptile_api = false;

  // PoCC generic options.
  pocc_input_file_name = NULL;
  pocc_trash = 0;
  pocc_verbose = 0;
  pocc_quiet = 0;
  pocc_clan_bounded_context = 0;
  pocc_inscop_fakepoccarray = 0;

  // Letsee options.
  pocc_letsee = false; // Run LetSee (default: no)
  pocc_letsee_space = LS_TYPE_FS;
  pocc_letsee_traversal = LS_HEURISTIC_EXHAUST;
  pocc_letsee_normspace = 0;
  pocc_letsee_scheme_m1 = NULL;
  pocc_letsee_prune_precut = 0;
  pocc_letsee_backtrack_multi = 0;
  pocc_letsee_rtries = 50;
  pocc_letsee_ilb = -1;
  pocc_letsee_iUb = 1;
  pocc_letsee_plb = -1;
  pocc_letsee_pUb = 1;
  pocc_letsee_clb = -1;
  pocc_letsee_cUb = 1;

  // Pluto options.
  pocc_pluto = false;
  pocc_pluto_parallel = 0;
  pocc_pluto_tile = 0;
  pocc_pluto_rar = 0;
  pocc_pluto_fuse = SMART_FUSE;
  pocc_pluto_unroll = 0;
  pocc_pluto_polyunroll = 0;
  pocc_pluto_bee = 0;
  pocc_pluto_prevector = 0;
  pocc_pluto_ufactor = 4;
  pocc_pluto_quiet = 0;
  pocc_pluto_context = 1;
  pocc_pluto_ft = -1;
  pocc_pluto_lt = -1;
  pocc_pluto_multipipe = 0;
  pocc_pluto_l2tile = 0;
  pocc_pluto_lastwriter = 0;
  pocc_pluto_scalpriv = 0;

  pocc_vectorizer = 0;
  pocc_storage_compaction = 0;
}

PolyRoseOptions::PolyRoseOptions()
{
  initialize ();
}

PolyRoseOptions::PolyRoseOptions(int argc, char** argv)
{
  initialize ();
  parse (argc, argv);
}


#define check_opt(x,y,z)			\
if (! strcmp (x, y)) {				\
  z;						\
  continue;					\
}

void
print_help ()
{
  fprintf (stderr, "Available options for PolyOpt: \n");
  fprintf (stderr, "--polyopt-help\n");
  fprintf (stderr, "Verbosity options: \n");
  fprintf (stderr, "--polyopt-verbose\n");
  fprintf (stderr, "--polyopt-quiet\n");
  fprintf (stderr, "Main optimization paths: \n");
  fprintf (stderr, "--polyopt-fixed-tiling\n");
  fprintf (stderr, "--polyopt-parametric-tiling\n");
  fprintf (stderr, "--polyopt-parallel-only\n");
  fprintf (stderr, "Scop extraction related options: \n");
  fprintf (stderr, "--polyopt-generic-scop-extractor\n");
  // fprintf (stderr, "--polyopt-strict-scop-extractor\n");
  // fprintf (stderr, "--polyopt-unsafe-scop-extractor\n");
  fprintf (stderr, "--polyopt-approximate-scop-extractor\n");
  fprintf (stderr, "--polyopt-safe-math-func\n");
  // fprintf (stderr, "--polyopt-f2c-scop-extractor\n");
  fprintf (stderr, "--polyopt-scop-extractor-verbose=1\n");
  fprintf (stderr, "--polyopt-scop-extractor-verbose=2\n");
  fprintf (stderr, "--polyopt-scop-extractor-verbose=3\n");
  fprintf (stderr, "--polyopt-scop-extractor-verbose=4\n");
  fprintf (stderr, "Main driver related options: \n");
  fprintf (stderr, "--polyopt-annotate-only\n");
  fprintf (stderr, "--polyopt-annotate-inner-loops\n");
  fprintf (stderr, "--polyopt-compute-ddv\n");
  fprintf (stderr, "--polyopt-hullify-ddv\n");
  fprintf (stderr, "--polyopt-generate-pragmas\n");
  fprintf (stderr, "Codegen related options: \n");
  fprintf (stderr, "--polyopt-codegen-use-past\n");
  fprintf (stderr, "--polyopt-codegen-use-clast\n");
  fprintf (stderr, "--polyopt-codegen-use-ptile\n");
  fprintf (stderr, "--polyopt-codegen-insert-ptile-api\n");
  fprintf (stderr, "Dependence analysis options: \n");
  fprintf (stderr, "--polyopt-scalar-privatization\n");
  fprintf (stderr, "PoCC related options: \n");
  fprintf (stderr, "--polyopt-pocc-verbose\n");
  fprintf (stderr, "--polyopt-pluto\n");
  fprintf (stderr, "--polyopt-pluto-tile\n");
  // fprintf (stderr, "--polyopt-pluto-l2tile\n");
  fprintf (stderr, "--polyopt-pluto-parallel\n");
  // fprintf (stderr, "--polyopt-pluto-unroll\n");
  // fprintf (stderr, "--polyopt-pluto-ufactor <value>\n");
  fprintf (stderr, "--polyopt-pluto-prevector\n");
  fprintf (stderr, "--polyopt-pluto-fuse-<maxfuse,smartfuse,nofuse>\n");
  fprintf (stderr, "--polyopt-pluto-rar\n");
  fprintf (stderr, "--polyopt-pluto-lastwriter\n");
  // fprintf (stderr, "--polyopt-pluto-multipipe\n");
  // fprintf (stderr, "--polyopt-pluto-scalpriv\n");
  // fprintf (stderr, "--polyopt-pluto-ft <value>\n");
  // fprintf (stderr, "--polyopt-pluto-lt <value>\n");
  // fprintf (stderr, "--polyopt-pluto-bee\n");
  // fprintf (stderr, "--polyopt-pluto-context\n");
  // fprintf (stderr, "--polyopt-letsee\n");
  // fprintf (stderr, "--polyopt-letsee-space <schedule,precut>\n");
  // fprintf (stderr, "--polyopt-letsee-traversal <exhaust,dh,random,m1,skip,ga>\n");
  // fprintf (stderr, "--polyopt-letsee-normspace\n");
  // fprintf (stderr, "--polyopt-letsee-prune-precut\n");
  // fprintf (stderr, "--polyopt-letsee-backtrack-multi\n");
  // fprintf (stderr, "--polyopt-letsee-rtries <value>\n");
  // fprintf (stderr, "--polyopt-letsee-ilb <value>\n");
  // fprintf (stderr, "--polyopt-letsee-iUb <value>\n");
  // fprintf (stderr, "--polyopt-letsee-plb <value>\n");
  // fprintf (stderr, "--polyopt-letsee-pUb <value>\n");
  // fprintf (stderr, "--polyopt-letsee-clb <value>\n");
  // fprintf (stderr, "--polyopt-letsee-cUb <value>\n");
  // fprintf (stderr, "--polyopt-vectorizer\n");
  // fprintf (stderr, "--polyopt-storage-compaction\n");

  exit (1);
}

void
PolyRoseOptions::parse(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
    {
      // Global Polyopt options.
      check_opt(argv[i], "--polyopt-verbose", pr_verbose = true);
      if (! strcmp (argv[i], "--polyopt-help"))
	print_help ();

      // Scop extraction options.
      check_opt(argv[i], "--polyopt-generic-scop-extractor",
		sc_generic_scop_extractor = true);
      check_opt(argv[i], "--polyopt-strict-scop-extractor",
		sc_strict_scop_extractor = true);
      check_opt(argv[i], "--polyopt-unsafe-scop-extractor",
		sc_unsafe_scop_extractor = true);
      sc_unsafe_scop_extractor = ! sc_strict_scop_extractor;
      check_opt(argv[i], "--polyopt-approximate-scop-extractor",
		sc_approximate_scop_extractor = true);
      check_opt(argv[i], "--polyopt-safe-math-func",
		sc_allow_math_func = true);
      check_opt(argv[i], "--polyopt-f2c-scop-extractor",
		sc_f2c_scop_extractor = true);
      check_opt(argv[i], "--polyopt-f2c-scop-extractor",
		sc_approximate_scop_extractor = true);
      check_opt(argv[i], "--polyopt-scop-extractor-verbose=1",
		sc_scop_extractor_verbose_level = 1);
      check_opt(argv[i], "--polyopt-scop-extractor-verbose=2",
		sc_scop_extractor_verbose_level = 2);
      check_opt(argv[i], "--polyopt-scop-extractor-verbose=3",
		sc_scop_extractor_verbose_level = 3);
      check_opt(argv[i], "--polyopt-scop-extractor-verbose=4",
		sc_scop_extractor_verbose_level = 4);
      check_opt(argv[i], "--polyopt-scop-extractor-verbose=5",
		sc_scop_extractor_verbose_level = 5);

      // Verbose option.
      check_opt(argv[i], "--polyopt-quiet",
		polyopt_quiet = true);

      // Dependence analysis.
      check_opt(argv[i], "--polyopt-scalar-privatization",
		polyopt_scalar_privatization = true);

      // Main driver option.
      check_opt(argv[i], "--polyopt-annotate-only",
		polyopt_annotate_only = true);
      check_opt(argv[i], "--polyopt-annotate-inner-loops",
		polyopt_annotate_inner_loops = true);
      check_opt(argv[i], "--polyopt-compute-ddv",
		polyopt_compute_ddv = true);
      check_opt(argv[i], "--polyopt-hullify-ddv",
		polyopt_hullify_ddv = true);
      check_opt(argv[i], "--polyopt-generate-pragmas",
		polyopt_generate_pragmas = true);


      // Main optimization primitives.
      check_opt(argv[i], "--polyopt-fixed-tiling",
		polyopt_fixed_tiling = true);
      check_opt(argv[i], "--polyopt-parametric-tiling",
		polyopt_parametric_tiling = true);
      check_opt(argv[i], "--polyopt-parallel-only",
		polyopt_parallel_only = true);

      // Codegen options.
      check_opt(argv[i], "--polyopt-codegen-use-past",
		polyopt_codegen_use_past = true);
      check_opt(argv[i], "--polyopt-codegen-use-clast",
		polyopt_codegen_use_past = false);

      check_opt(argv[i], "--polyopt-codegen-use-ptile",
		polyopt_codegen_use_ptile = true);
      check_opt(argv[i], "--polyopt-codegen-insert-ptile-api",
		polyopt_insert_ptile_api = true);

      // Global PoCC options.
      check_opt(argv[i], "--polyopt-pocc-verbose", pocc_verbose = true);

      check_opt(argv[i], "--polyopt-vectorizer", pocc_vectorizer = true);
      check_opt(argv[i], "--polyopt-storage-compaction",
		pocc_storage_compaction = true);

      // Pluto options.
      check_opt(argv[i], "--polyopt-pluto", pocc_pluto = true);
      check_opt(argv[i], "--polyopt-pluto-unroll", pocc_pluto_unroll = true);
      check_opt(argv[i], "--polyopt-pluto-ufactor",
		pocc_pluto_ufactor = atoi (argv[++i]));
      check_opt(argv[i], "--polyopt-pluto-parallel", pocc_pluto_parallel = true);
      check_opt(argv[i], "--polyopt-pluto-tile", pocc_pluto_tile = true);
      check_opt(argv[i], "--polyopt-pluto-rar", pocc_pluto_rar = true);


      check_opt(argv[i], "--polyopt-pluto-fuse-maxfuse", pocc_pluto_fuse = MAXIMAL_FUSE);
      check_opt(argv[i], "--polyopt-pluto-fuse-smartfuse", pocc_pluto_fuse = SMART_FUSE);
      check_opt(argv[i], "--polyopt-pluto-fuse-nofuse", pocc_pluto_fuse = NO_FUSE);
      // if (! strcmp (argv[i], "--polyopt-pluto-fuse"))
      // 	{
      // 	  ++i;
      // 	  if (! strcmp (argv[i], "maxfuse"))
      // 	    pocc_pluto_fuse = MAXIMAL_FUSE;
      // 	  else if (! strcmp (argv[i], "smartfuse"))
      // 	    pocc_pluto_fuse = SMART_FUSE;
      // 	  else if (! strcmp (argv[i], "nofuse"))
      // 	    pocc_pluto_fuse = NO_FUSE;
      // 	  pocc_pluto = 1;
      // 	}
      check_opt(argv[i], "--polyopt-pluto-polyunroll",
		pocc_pluto_polyunroll = true);
      check_opt(argv[i], "--polyopt-pluto-bee", pocc_pluto_bee = true);
      check_opt(argv[i], "--polyopt-pluto-prevector",
		pocc_pluto_prevector = true);
      check_opt(argv[i], "--polyopt-pluto-quiet", pocc_pluto_quiet = true);
      check_opt(argv[i], "--polyopt-pluto-context", pocc_pluto_context = true);
      check_opt(argv[i], "--polyopt-pluto-multipipe", pocc_pluto_multipipe = true);
      check_opt(argv[i], "--polyopt-pluto-l2tile", pocc_pluto_l2tile = true);
      check_opt(argv[i], "--polyopt-pluto-lastwriter",
		pocc_pluto_lastwriter = true);
      check_opt(argv[i], "--polyopt-pluto-scalpriv", pocc_pluto_scalpriv = true);
      check_opt(argv[i], "--polyopt-pluto-ft", pocc_pluto_ft = atoi (argv[++i]));
      check_opt(argv[i], "--polyopt-pluto-lt", pocc_pluto_lt = atoi (argv[++i]));

      // LetSee options.
      check_opt(argv[i], "--polyopt-letsee", pocc_letsee = true);
      if (! strcmp (argv[i], "--polyopt-letsee-traversal"))
	{
	  ++i;
	  if (! strcmp (argv[i], "exhaust"))
	    pocc_letsee_traversal = LS_HEURISTIC_EXHAUST;
	  else if (! strcmp (argv[i], "dh"))
	    pocc_letsee_traversal = LS_HEURISTIC_DH;
	  else if (! strcmp (argv[i], "random"))
	    pocc_letsee_traversal = LS_HEURISTIC_RANDOM;
	  else if (! strcmp (argv[i], "m1"))
	    pocc_letsee_traversal = LS_HEURISTIC_M1;
	  else if (! strcmp (argv[i], "skip"))
	    pocc_letsee_traversal = LS_HEURISTIC_SKIP;
	  //else if (! strcmp(opt_tab[POCC_OPT_LETSEE_TRAVERSAL], "ga"))
	  pocc_letsee = true;
	  continue;
	}
      if (! strcmp (argv[i], "--polyopt-letsee-space"))
	{
	  ++i;
	  if (! strcmp (argv[i], "precut"))
	    pocc_letsee_space = LS_TYPE_FS;
	  else if (! strcmp (argv[i], "schedule"))
	    pocc_letsee_space = LS_TYPE_MULTI;
	  pocc_letsee = true;
	  continue;
	}
      check_opt(argv[i], "--polyopt-letsee-normspace",
		pocc_letsee_normspace = true);
      check_opt(argv[i], "--polyopt-letsee-prune-precut",
		pocc_letsee_prune_precut = true);
      check_opt(argv[i], "--polyopt-letsee-backtrack-multi",
		pocc_letsee_backtrack_multi = true);
      check_opt(argv[i], "--polyopt-letsee-rtries",
		pocc_letsee_rtries = atoi (argv[++i]));
      check_opt(argv[i], "--polyopt-letsee-ilb",
		pocc_letsee_ilb = atoi (argv[++i]));
      check_opt(argv[i], "--polyopt-letsee-iUb",
		pocc_letsee_iUb = atoi (argv[++i]));
      check_opt(argv[i], "--polyopt-letsee-plb",
		pocc_letsee_plb = atoi (argv[++i]));
      check_opt(argv[i], "--polyopt-letsee-pUb",
		pocc_letsee_pUb = atoi (argv[++i]));
      check_opt(argv[i], "--polyopt-letsee-clb",
		pocc_letsee_clb = atoi (argv[++i]));
      check_opt(argv[i], "--polyopt-letsee-cUb",
		pocc_letsee_cUb = atoi (argv[++i]));
//       // Misc.
//       check_opt(argv[i], "--polyopt-compile-cmd",
// 		pocc_compile_cmd = strdup (argv[++i]));

      // If we are here, then we are parsing the filename now.
      pocc_input_file_name = strdup (argv[i]);
    }

  // Deal with generic options.
  if (polyopt_fixed_tiling)
    {
      polyopt_codegen_use_ptile = false;
      polyopt_insert_ptile_api = false;
      polyopt_generate_pragmas = true;
      pocc_pluto = true;
      pocc_pluto_parallel = true;
      pocc_pluto_tile = true;
      pocc_pluto_prevector = true;
    }
  if (polyopt_parametric_tiling)
    {
      polyopt_codegen_use_ptile = true;
      polyopt_insert_ptile_api = true;
      polyopt_generate_pragmas = true;
      pocc_pluto = true;
      pocc_pluto_parallel = true;
    }
  if (polyopt_parallel_only)
    {
      polyopt_codegen_use_ptile = false;
      polyopt_generate_pragmas = true;
      pocc_pluto = true;
      pocc_pluto_parallel = true;
    }
}


s_pocc_options_t*
PolyRoseOptions::buildPoccOptions()
{
  s_pocc_options_t* options = pocc_options_malloc ();

  // PolyRose Options
  options->trash = pocc_trash;
  options->verbose = pr_verbose || pocc_verbose;
  options->quiet = pocc_quiet | polyopt_quiet;
  options->input_file_name = pocc_input_file_name;
  options->clan_bounded_context = pocc_clan_bounded_context;
  options->inscop_fakepoccarray = pocc_inscop_fakepoccarray;
  options->names_are_strings = 0;

  // Codegen options.
  if (polyopt_codegen_use_ptile)
    {
      options->pluto = 1;
      options->pluto_parallel = 1;
      options->use_past = 1;
      polyopt_codegen_use_past = 1;
      pocc_pluto = true;
      pocc_pluto_parallel = false;
    }

  // Letsee options.
  options->letsee = pocc_letsee;
  options->letsee_space = pocc_letsee_space;
  options->letsee_traversal = pocc_letsee_traversal;
  options->letsee_normspace = pocc_letsee_normspace;
  options->letsee_scheme_m1 = pocc_letsee_scheme_m1;
  options->letsee_prune_precut = pocc_letsee_prune_precut;
  options->letsee_backtrack_multi = pocc_letsee_backtrack_multi;
  options->letsee_rtries = pocc_letsee_rtries;
  options->letsee_ilb = pocc_letsee_ilb;
  options->letsee_iUb = pocc_letsee_iUb;
  options->letsee_plb = pocc_letsee_plb;
  options->letsee_pUb = pocc_letsee_pUb;
  options->letsee_clb = pocc_letsee_clb;
  options->letsee_cUb = pocc_letsee_cUb;

  // Pluto options.
  if (pocc_pluto_parallel || pocc_pluto_tile
      || pocc_pluto_rar || pocc_pluto_fuse != SMART_FUSE
      || pocc_pluto_unroll
      || pocc_pluto_prevector)
    pocc_pluto = 1;
  if (polyopt_scalar_privatization)
    pocc_pluto_scalpriv = 1;
  options->pluto = pocc_pluto;
  options->pluto_parallel = pocc_pluto_parallel;
  options->pluto_tile = pocc_pluto_tile;
  options->pluto_rar = pocc_pluto_rar;
  options->pluto_fuse = pocc_pluto_fuse;
  options->pluto_unroll = pocc_pluto_unroll;
  options->pluto_polyunroll = pocc_pluto_polyunroll;
  options->pluto_bee = pocc_pluto_bee;
  options->pluto_prevector = pocc_pluto_prevector;
  options->pluto_ufactor = pocc_pluto_ufactor;
  options->pluto_quiet = pocc_pluto_quiet;
  options->pluto_context = pocc_pluto_context;
  options->pluto_ft = pocc_pluto_ft;
  options->pluto_lt = pocc_pluto_lt;
  options->pluto_multipipe = pocc_pluto_multipipe;
  options->pluto_l2tile = pocc_pluto_l2tile;
  options->pluto_lastwriter = pocc_pluto_lastwriter;
  options->pluto_scalpriv = pocc_pluto_scalpriv;

  // Vectorization options.
  options->vectorizer = pocc_vectorizer;

  // Storage compaction options.
  options->storage_compaction = pocc_storage_compaction;

  return options;
}

bool
PolyRoseOptions::getCodegenUsePAST()
{
  return polyopt_codegen_use_past;
}

bool
PolyRoseOptions::isVerbose()
{
  return pr_verbose;
}

int
PolyRoseOptions::getScVerboseLevel()
{
  return sc_scop_extractor_verbose_level;
}

bool
PolyRoseOptions::getScUnsafeExtractor()
{
  return sc_unsafe_scop_extractor;
}

bool
PolyRoseOptions::getScStrictExtractor()
{
  return sc_strict_scop_extractor;
}

bool
PolyRoseOptions::getScApproximateExtractor()
{
  return sc_approximate_scop_extractor;
}

bool
PolyRoseOptions::getScF2CExtractor()
{
  return sc_f2c_scop_extractor;
}

bool
PolyRoseOptions::getScGenericExtractor()
{
  return sc_generic_scop_extractor;
}

bool
PolyRoseOptions::getAnnotateOnly()
{
  return polyopt_annotate_only;
}

bool
PolyRoseOptions::getAnnotateInnerLoops()
{
  return polyopt_annotate_inner_loops;
}


void
PolyRoseOptions::setAnnotateInnerLoops(bool val)
{
  polyopt_annotate_inner_loops = val;
}

bool
PolyRoseOptions::getGeneratePragmas()
{
  return polyopt_generate_pragmas;
}

void
PolyRoseOptions::setGeneratePragmas(bool val)
{
  polyopt_generate_pragmas = val;
}

bool
PolyRoseOptions::getComputeDDV()
{
  return polyopt_compute_ddv;
}

void
PolyRoseOptions::setComputeDDV(bool val)
{
  polyopt_compute_ddv = val;
}

bool
PolyRoseOptions::getHullifyDDV()
{
  return polyopt_hullify_ddv;
}


void
PolyRoseOptions::setHullifyDDV(bool val)
{
  polyopt_hullify_ddv = val;
}

bool
PolyRoseOptions::getAllowMathFunc()
{
  return sc_allow_math_func;
}

void
PolyRoseOptions::setAllowMathFunc(bool val)
{
  sc_allow_math_func = val;
}


bool
PolyRoseOptions::getCodegenUsePtile()
{
  return polyopt_codegen_use_ptile;
}

void
PolyRoseOptions::setCodegenUsePtile(bool val)
{
  polyopt_codegen_use_ptile = val;
}


bool
PolyRoseOptions::getInsertPtileAPI()
{
  return polyopt_insert_ptile_api;
}

void
PolyRoseOptions::setInsertPtileAPI(bool val)
{
  polyopt_insert_ptile_api = val;
}

bool
PolyRoseOptions::getQuiet()
{
  return polyopt_quiet;
}


void
PolyRoseOptions::setQuiet(bool val)
{
  polyopt_quiet = val;
}


bool
PolyRoseOptions::getOptimize()
{
  return polyopt_optimize || pocc_pluto || pocc_pluto_tile ||
    pocc_pluto_parallel || pocc_letsee;
}


void
PolyRoseOptions::setOptimize(bool val)
{
  polyopt_optimize = val;
}

void
PolyRoseOptions::setPluto(bool val)
{
  pocc_pluto = val;
}

bool
PolyRoseOptions::getPluto()
{
  return pocc_pluto;
}

void
PolyRoseOptions::setPlutoTile(bool val)
{
  if (val)
    pocc_pluto = val;
  pocc_pluto_tile = val;
}

void
PolyRoseOptions::setPlutoParralel(bool val)
{
  if (val)
    pocc_pluto = val;
  pocc_pluto_parallel = val;
}

void
PolyRoseOptions::setPlutoPrevector(bool val)
{
  if (val)
    pocc_pluto = val;
  pocc_pluto_prevector = val;
}

void
PolyRoseOptions::setScalarPrivatization(bool val)
{
  polyopt_scalar_privatization = val;
}

bool
PolyRoseOptions::getScalarPrivatization()
{
  return polyopt_scalar_privatization;
}


