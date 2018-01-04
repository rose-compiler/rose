/*
 * PolyOptDriver.cpp: This file is part of the PolyOpt project.
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
 * @file: PolyOptDriver.cpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#include <getopt.h>
#include <string.h>
#include <stdio.h>

#include <string>
#include <iostream>

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
#include <candl/candl.h>
#include <pocc/options.h>
#include <pocc/exec.h>
#include <pocc-utils/options.h>

#include <past/past.h>

#include <polyopt/PolyOptDriver.hpp>
#include <polyopt/PolyOpt.hpp>

#include <polyopt/ScopExtractor.hpp>
#include <polyopt/ClastToSage.hpp>
#include <polyopt/PastToSage.hpp>
#include <polyopt/PoCCOptimize.hpp>
#include <polyopt/PolyRoseOptions.hpp>
#include <polyopt/SageNodeAnnotation.hpp>


/**
 * Compute the DDV for a scop, and embed them in SgForStatement annotations.
 *
 */
void
computeDDVinScop(scoplib_scop_p scop, PolyRoseOptions& polyoptions);

static
int driverComputeDDV(SgNode* root, PolyRoseOptions& polyoptions)
{
  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Annotate polyhedral loops with DDV info" << std::endl;

  // 1- Extract Scops from tree.
  bool oldQuiet = polyoptions.getQuiet();
  polyoptions.setQuiet(true);
  ScopExtractor extractor (polyoptions);
  extractor.extractScops(root);
  std::vector<scoplib_scop_p> scops = extractor.getScoplibs();
  polyoptions.setQuiet(oldQuiet);
  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Create annotation for " << scops.size()
	      << " SCoP(s)" << std::endl;
  std::vector<scoplib_scop_p>::const_iterator sage_scop_iterator;
  // 2- Iterate on each scop:
  for (sage_scop_iterator = scops.begin (); sage_scop_iterator != scops.end ();
       ++sage_scop_iterator)
    {
      // Compute the DDVs for the scop.
      computeDDVinScop(*sage_scop_iterator, polyoptions);

      // Be clean.
      scoplib_scop_free (*sage_scop_iterator);
    }

  return EXIT_SUCCESS;
}

static
int driverAnnotateInnerLoops(SgNode* root, PolyRoseOptions& polyoptions)
{
  // 1- Retrieve pocc_options_t from PolyRoseOptions.
  s_pocc_options_t* poptions = polyoptions.buildPoccOptions ();
  s_pocc_utils_options_t* puoptions = pocc_utils_options_malloc ();

  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Annotate polyhedral inner loops with vectorizability info"
	      << std::endl;

  // 1- Extract all inner-most loops in the subtree.
  std::vector<SgNode*> innerLoops;
  SgNodePtrList forLoops = NodeQuery::querySubTree(root, V_SgForStatement);
  SgNodePtrList::const_iterator iter;
  for (iter = forLoops.begin(); iter != forLoops.end(); ++iter)
    {
      SgNodePtrList loops =
	NodeQuery::querySubTree(*iter, V_SgForStatement);
      if (loops.size() == 1)
	innerLoops.push_back(*iter);
    }

  std::vector<SgNode*>::iterator i;
  int scopId;
  int numInnerLoops = 0;
  for (i = innerLoops.begin(); i != innerLoops.end(); ++i)
    {
      // 3- Annotate all inner-most loop which are polyhedral.
      bool oldQuiet = polyoptions.getQuiet();
      polyoptions.setQuiet(true);
      ScopExtractor extractor (polyoptions);
      extractor.extractScops(*i);
      std::vector<scoplib_scop_p> scops = extractor.getScoplibs();
      polyoptions.setQuiet(oldQuiet);

      std::vector<scoplib_scop_p>::const_iterator sage_scop_iterator;
      // Iterate on each scop (should be exactly zero or one):
      for (sage_scop_iterator = scops.begin (), scopId = 1;
	   sage_scop_iterator != scops.end ();
	   ++sage_scop_iterator, ++scopId)
	{
	  SageForInfo* annot = new SageForInfo();
	  scoplib_scop_p scop = *sage_scop_iterator;
	  // Process it with PoCC:
	  annot->isParallel =
	    PoCCIsLoopParallel (scop, poptions, puoptions);
	  if (polyoptions.getScVerboseLevel() >=
	      PolyRoseOptions::VERBOSE_COMPREHENSIVE)
	    {
	      std::cout << "Loop is parallel? " << annot->isParallel;
	      std::cout << ((SgNode*)(scop->usr))->unparseToCompleteString()
			<< std::endl;
	    }
	  if (annot->isParallel)
	    {
	      annot->isStrideOne =
		PoCCIsLoopStrideOneAccesses(scop, poptions, puoptions);
	      if (polyoptions.getScVerboseLevel() >=
		  PolyRoseOptions::VERBOSE_FULL)
		{
		  std::cout << "Loop has stride-1? " << annot->isStrideOne;
		  std::cout << ((SgNode*)(scop->usr))->unparseToCompleteString()
			    << std::endl;
		}
	    }
	  else
	    annot->isStrideOne = 0;

	  SgNode* loop = (SgNode*) scop->usr;
	  std::vector<SgNode*> loops =
	    NodeQuery::querySubTree(loop, V_SgForStatement);
	  ROSE_ASSERT(loops.size() == 1);
          SgForStatement* forLoop = isSgForStatement(*(loops.begin()));
	  SageForInfo* oldannot =
	    (SageForInfo*) forLoop->getAttribute(SageForInfo::name());
	  if (oldannot)
	    delete oldannot;
	  /// LNP: loop-local annotation.
	  forLoop->setAttribute(SageForInfo::name(), annot);

	  // Be clean.
	  scoplib_scop_free (*sage_scop_iterator);

	  ++numInnerLoops;
	}
    }

  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Created vectorizability annotation for " << numInnerLoops
	      << " inner loop(s)" << std::endl;

  return EXIT_SUCCESS;
}

/**
 * Optimize a scop with PoCC. For internal use only.
 *
 */
void
optimizeSingleScop(scoplib_scop_p scoplibScop,
		   int scopId,
		   s_pocc_options_t* poptions,
		   s_pocc_utils_options_t* puoptions,
		   PolyRoseOptions& polyoptions);

static
int driverOptimizeWithPoCC(SgNode* root, PolyRoseOptions& polyoptions)
{
  int scopId;

  // 1- Retrieve pocc_options_t from PolyRoseOptions.
  s_pocc_options_t* poptions = polyoptions.buildPoccOptions ();
  s_pocc_utils_options_t* puoptions = pocc_utils_options_malloc ();
  if (poptions->letsee == 0 && poptions->pluto == 0 && poptions->ptile == 0)
    {
      if (! poptions->quiet)
	std::cout <<
	  "[PoCC] INFO: pass-thru compilation, no optimization enabled\n";
    }
  else
    {
      if (! poptions->quiet)
	std::cout <<
	  "[PoCC] INFO: Polyhedral Loop transformation engine activated\n";
    }
  // 2- Extract Scops from source tree.
  ScopExtractor extractor (polyoptions);
  extractor.extractScops(root);
  std::vector<scoplib_scop_p> scops = extractor.getScoplibs();

  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Number of SCoPs: " << scops.size() << std::endl;
  std::vector<scoplib_scop_p>::const_iterator sage_scop_iterator;
  // 3- Iterate on each scop:
  for (sage_scop_iterator = scops.begin (), scopId = 1;
       sage_scop_iterator != scops.end ();
       ++sage_scop_iterator, ++scopId)
    {
      //   4- Process it:
      // 4.1- Optimize the scop with PoCC
      // 4.2- Generate the new Sage AST with CLooG CLAST output
      // 4.3- Plug the new AST in place of the old one.
      optimizeSingleScop(*sage_scop_iterator, scopId, poptions,
			 puoptions, polyoptions);
    }

  // Clear PoCC-wide options.
  /// LNP: FIXME: Reactivate this.
  //pip_close ();
  pocc_options_free (poptions);

  return EXIT_SUCCESS;
}


/**
 * PolyOpt entry point, for a SgNode processing.
 *
 */
int
PolyOptDriver(SgNode* root, PolyRoseOptions& polyoptions)
{
  // 1- Optimize with pluto
  if (polyoptions.getOptimize())
    driverOptimizeWithPoCC(root, polyoptions);

  // 2- Annotate
  // 2a - compute ddvs.
  if (polyoptions.getComputeDDV())
    driverComputeDDV(root, polyoptions);

  // 2b - compute inner-loop vectorizability info.
  if (polyoptions.getAnnotateInnerLoops())
    driverAnnotateInnerLoops(root, polyoptions);

  // 3- Optimize with PTile.
  if (polyoptions.getCodegenUsePtile())
    {
      // Don't do pluto if already done, do pluto if already done.
      bool plutoPass = polyoptions.getPluto();
      polyoptions.setPluto(! plutoPass);
      driverOptimizeWithPoCC(root, polyoptions);
      polyoptions.setPluto(plutoPass);
    }

  return EXIT_SUCCESS;
}

/**
 * PolyOpt entry point, for a SgProject processing.
 *
 */
int
PolyOptDriver(SgProject* project, PolyRoseOptions& polyoptions)
{
  SgFilePtrList& file_list = project->get_fileList();
  SgFilePtrList::const_iterator file_iter;
  // Iterate on all files of the project.
  for (file_iter = file_list.begin(); file_iter != file_list.end(); file_iter++)
    {
      SgSourceFile* file = isSgSourceFile(*file_iter);
      if (polyoptions.getScVerboseLevel())
	std::cout << "[Extr] File: " << file->getFileName() << std::endl;
      SgNodePtrList funcDefnList =
	NodeQuery::querySubTree(file, V_SgFunctionDefinition);
      SgNodePtrList::const_iterator iter;
      // Iterate on all function defined in a file.
      for (iter = funcDefnList.begin(); iter != funcDefnList.end(); ++iter)
	{
	  SgFunctionDefinition *fun = isSgFunctionDefinition(*iter);
	  if (!fun)
	    {
	      std::cout << "[Extr] Warning: Expected SgFunctionDefinition in " <<
		file->getFileName() << std::endl;
	      continue; // with the next function definition
	    }
	  SgName name = fun->get_declaration()->get_name();
	  if (polyoptions.getScVerboseLevel())
	    std::cout << "[Extr] Function: " << name.getString() << std::endl;
	  SgBasicBlock* body = fun->get_body();

	  // Proceed with the function body.
	  PolyOptDriver(body, polyoptions);
	}
    }

  return EXIT_SUCCESS;
}
