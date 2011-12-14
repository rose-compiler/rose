/*
 * PolyOpt.cpp: This file is part of the PolyOpt project.
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
 * @file: PolyOpt.cpp
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

#include <polyopt/PolyOpt.hpp>

#include <polyopt/ScopExtractor.hpp>
#include <polyopt/ClastToSage.hpp>
#include <polyopt/PastToSage.hpp>
#include <polyopt/PoCCOptimize.hpp>
#include <polyopt/PolyRoseOptions.hpp>
#include <polyopt/SageNodeAnnotation.hpp>


void optimizeSingleScop(scoplib_scop_p scoplibScop,
			int scopId,
			s_pocc_options_t* poptions,
			s_pocc_utils_options_t* puoptions,
			PolyRoseOptions& polyoptions);


/******************************************************************************/
/************************ PolyOpt Optimization routines ***********************/
/******************************************************************************/

/**
 * Transform a full Rose project using Polyhedral techniques.
 *
 *
 */
int PolyOptOptimizeProject(SgProject* project, PolyRoseOptions& polyoptions)
{
  int scopId;

  // 1- Retrieve pocc_options_t from PolyRoseOptions.
  s_pocc_options_t* poptions = polyoptions.buildPoccOptions ();

  s_pocc_utils_options_t* puoptions = pocc_utils_options_malloc ();
  if (poptions->letsee == 0 && poptions->pluto == 0)
    if (! poptions->quiet)
      std::cout <<
	"[PoCC] INFO: pass-thru compilation, no optimization enabled\n";


  // 2- Extract Scops from source tree.
  ScopExtractor extractor (project, polyoptions);
  std::vector<scoplib_scop_p> scops = extractor.getScoplibs();

  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Number of SCoPs: " << scops.size() << std::endl;
  std::vector<scoplib_scop_p>::iterator sage_scop_iterator;
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

  // 4- Compute scop-wide ddv, if required.
  if (polyoptions.getComputeDDV())
    PolyOptAnnotateProject(project, polyoptions);

  // 5- Annotate inner-most loops, if required.
  if (polyoptions.getAnnotateInnerLoops())
    PolyOptInnerLoopsAnnotateProject(project, polyoptions);

  // Translate parallel loops to parfor/vectorfor, and insert the
  // associated pragmas.
  /// LNP: Right now, it is done syntactically with annotations in the
  /// Sage, not by using new Sage node types for parallel
  /// loops/pragmas.
//   if (polyoptions.getGeneratePragmas())
//     PolyOptAnnotateProject(project, polyoptions);

  return EXIT_SUCCESS;
}


/**
 * Transform a sub-tree using Polyhedral techniques.
 *
 *
 */
int PolyOptOptimizeSubTree(SgNode* root, PolyRoseOptions& polyoptions)
{
  int scopId;

  // 1- Retrieve pocc_options_t from PolyRoseOptions.
  s_pocc_options_t* poptions = polyoptions.buildPoccOptions ();

  s_pocc_utils_options_t* puoptions = pocc_utils_options_malloc ();
  if (poptions->letsee == 0 && poptions->pluto == 0)
    if (! poptions->quiet)
      std::cout <<
	"[PoCC] INFO: pass-thru compilation, no optimization enabled\n";

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

  // 4- Annotate inner-most loops, if required.
  if (polyoptions.getAnnotateInnerLoops())
    PolyOptInnerLoopsAnnotateSubTree(root, polyoptions);


  return EXIT_SUCCESS;
}



/******************************************************************************/
/*************************** PolyOpt DDV computation **************************/
/******************************************************************************/


/**
 * Annotate the AST to reflect the schedule. Simple prefix visit.
 *
 */
static
std::vector<SgForStatement*>
mapForLoopToId(SgNode* root)
{
  class ScopVisitor : public AstPrePostOrderTraversal {
    public:
    // top-down parsing.
    virtual void preOrderVisit(SgNode* node) {
      SgForStatement* fornode = isSgForStatement(node);
      if (fornode)
	loopMap.push_back(fornode);
      else
	{
	  // Deal with the special case of statements not surrounded
	  // by any loop.
	  ScopStatementAnnotation* annot =
	    (ScopStatementAnnotation*)(node->getAttribute("ScopStmt"));
	  if (annot)
	    {
	      SgForStatement* encloseFor =
		SageInterface::getEnclosingNode<SgForStatement>(node);
	      if (encloseFor)
		{
		  std::vector<SgForStatement*>::const_iterator i;
		  for (i = loopMap.begin(); i != loopMap.end(); ++i)
		    if (*i == encloseFor)
		      break;
		  if (i != loopMap.end())
		    return;
		}
	      loopMap.push_back (NULL);
	    }
	}
    };
    virtual void postOrderVisit(SgNode* node) {};
    std::vector<SgForStatement*> loopMap;
  };

  ScopVisitor sv;

  sv.traverse(root);

  return sv.loopMap;
}


static
bool
check_ddv_exists(std::vector<PolyDDV>& refs, PolyDDV& ddv,
		 bool consider_only_pure_identical)
{
  std::vector<PolyDDV>::iterator i;
  for (i = refs.begin(); i != refs.end(); ++i)
    {
      if (consider_only_pure_identical)
	{
	  if ((*i).isPureIdentical(ddv))
	    return true;
	}
      else
	if ((*i).isIdenticalVector(ddv))
	  return true;
    }

  return false;
}


/**
 * Create Poly DDV from Candl ddv structure.
 *
 */
static
std::vector<PolyDDV>
create_ddv_from_candl(CandlDDV* cddv,
		      std::vector<SgForStatement*>& loop_ids,
		      bool remove_duplicates,
		      bool consider_only_pure_identical)
{
  CandlDDV* tmp;
  std::vector<PolyDDV> output;

  for (tmp = cddv; tmp; tmp = tmp->next)
    {
      PolyDDV ddv(tmp->length, loop_ids[tmp->loop_id - 1]);
      // Set the dependence type.
      switch (tmp->deptype)
	{
	case CANDL_RAW:
	  ddv.setRAW();
	  break;
	case CANDL_WAR:
	  ddv.setWAR();
	  break;
	case CANDL_WAW:
	  ddv.setWAW();
	  break;
	case CANDL_RAR:
	  ddv.setRAR();
	  break;
	case CANDL_UNSET:
	default:
	  break;
	}
      // Set the components of the vector.
      int i;
      for (i = 0; i < tmp->length; ++i)
	{
	  switch (tmp->data[i].type)
	    {
	    case candl_dv_eq:
	      ddv.setEq(i);
	      break;
	    case candl_dv_plus:
	      //ddv.setPlus(i);
	      ddv.setMinus(i);
	      break;
	    case candl_dv_minus:
	      //ddv.setMinus(i);
	      ddv.setPlus(i);
	      break;
	    case candl_dv_star:
	      ddv.setStar(i);
	      break;
	    case candl_dv_scalar:
	      ddv.setScalar(i, - tmp->data[i].value);
	      break;
	    default:
	      ROSE_ASSERT(0);
	    }
	}
      // Check if an identical ddv was already created.
      if (remove_duplicates)
	{
	  if (! check_ddv_exists(output, ddv, consider_only_pure_identical))
	    output.push_back(ddv);
	}
      else
	output.push_back(ddv);
    }

  return output;
}

static
std::vector<PolyDDV>
hullifyDDV(std::vector<PolyDDV>& ddvs)
{
  std::vector<PolyDDV> output;
  std::vector<PolyDDV>::iterator i = ddvs.begin();
  if (i != ddvs.end())
    {
      PolyDDV base = *i;
      ++i;
      for (; i != ddvs.end(); ++i)
	base.computeHull(*i);
      output.push_back(base);
    }

  return output;
}

void
computeDDVinScop(scoplib_scop_p scop, PolyRoseOptions& polyoptions)
{
  // 4- Process it:
  // 4.1- Compute dependence polyhedra
  CandlOptions* coptions = candl_options_malloc ();
  CandlProgram* cprogram = candl_program_convert_scop (scop, NULL);
  CandlDependence* cdeps = candl_dependence (cprogram, coptions);
  //candl_dependence_pprint (stdout, cdeps);
  /// FIXME: LNP: This may be enough?
  ///candl_dependence_isl_simplify(cdeps, cprogram);
  // 4.2- Map each loop in the scop to its candl id.
  std::vector<SgForStatement*> loopMap =
    mapForLoopToId((SgNode*)(scop)->usr);

  // 4.3- Traverse all loops, and create the DDVs.
  std::vector<SgForStatement*>::const_iterator i;
  int id;

  // Search for the smallest loop id. It should always be 0, but just to
  /// be sure...
  int smallestLoopId = -1;
  for (int c = 0; c < cprogram->nb_statements; ++c)
    for (int lid = 0; lid < cprogram->statement[c]->depth; ++lid)
      {
	int val = cprogram->statement[c]->index[lid];
	if (smallestLoopId == -1)
	  smallestLoopId = val;
	else
	  smallestLoopId = val < smallestLoopId ? val : smallestLoopId;
      }
  // Iterate on all loops.
  for (id = 0, i = loopMap.begin(); i != loopMap.end(); ++i, ++id)
    {
      // Skipping fake loops (ie, statement surrounded by no loop).
      if (*i == NULL)
	continue;

      if (polyoptions.getScVerboseLevel() >=
	  PolyRoseOptions::VERBOSE_COMPREHENSIVE)
	{
	  std::cout << "For loop: " << std::endl;
	  std::cout << (*i)->unparseToCompleteString() << std::endl;
	}
      CandlDDV* cddv = candl_ddv_extract_in_loop (cprogram, cdeps,
						  id + smallestLoopId);
      //candl_ddv_print (stdout, cddv);

      std::vector<PolyDDV> ddvs =
	create_ddv_from_candl(cddv, loopMap, true, true);

      candl_ddv_free (cddv);

      SageForInfo* annot =
	(SageForInfo*) (*i)->getAttribute(SageForInfo::name());
      if (annot == NULL)
	annot = new SageForInfo();
      if (polyoptions.getHullifyDDV())
	annot->ddvs = hullifyDDV(ddvs);
      else
	annot->ddvs = ddvs;
      (*i)->setAttribute(SageForInfo::name(), annot);

      if (polyoptions.getScVerboseLevel() >=
	  PolyRoseOptions::VERBOSE_COMPREHENSIVE)
	{
	  if (annot->ddvs.size())
	    {
	      std::cout << "Dependence vectors for the loop nest(s): " <<
		std::endl;
	      std::vector<PolyDDV>::const_iterator j;
	      for (j = annot->ddvs.begin(); j != annot->ddvs.end(); ++j)
		std::cout << *j << std::endl;
	    }
	}
    }
  candl_program_free (cprogram);
  candl_dependence_free (cdeps);
  candl_options_free (coptions);
}

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
				     PolyRoseOptions& polyoptions)
{
  // 1- Retrieve pocc_options_t from PolyRoseOptions.
  s_pocc_options_t* poptions = polyoptions.buildPoccOptions ();
  s_pocc_utils_options_t* puoptions = pocc_utils_options_malloc ();
  if (! poptions->quiet && ! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Annotate polyhedral inner loops with vectorizability info"
	      << std::endl;

  // 2- Extract all inner-most loops in the project.
  std::vector<SgNode*> innerLoops;
  SgFilePtrList& file_list = project->get_fileList();
  SgFilePtrList::const_iterator file_iter;
  // Iterate on all files of the project.
  for (file_iter = file_list.begin(); file_iter != file_list.end(); file_iter++)
    {
      SgSourceFile* file = isSgSourceFile(*file_iter);
      if (polyoptions.getScVerboseLevel())
	std::cout << "[Extr] File: " << file->getFileName() << std::endl;
      SgNodePtrList forLoops =
	NodeQuery::querySubTree(file, V_SgForStatement);
      SgNodePtrList::const_iterator iter;
      // Iterate on all for loops in a file.
      for (iter = forLoops.begin(); iter != forLoops.end(); ++iter)
	{
	  SgNodePtrList loops =
	    NodeQuery::querySubTree(*iter, V_SgForStatement);
	  if (loops.size() == 1)
	    innerLoops.push_back(*iter);
	}
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
	  ++numInnerLoops;
	  SageForInfo* annot = new SageForInfo();
	  scoplib_scop_p scop = *sage_scop_iterator;
	  // Process it with PoCC:
	  annot->isParallel =
	    PoCCIsLoopParallel (scop, poptions, puoptions);
	  if (polyoptions.getScVerboseLevel() >= PolyRoseOptions::VERBOSE_FULL)
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
	  forLoop->setAttribute(SageForInfo::name(), annot->copy());

	  // Compute the DDVs for the scop, if required.
	  if (polyoptions.getComputeDDV())
	    computeDDVinScop(*sage_scop_iterator, polyoptions);
	}
    }

  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Created vectorizability annotation for " << numInnerLoops
	      << " inner loop(s)" << std::endl;


  // Clear PoCC-wide options.
  /// LNP: FIXME: Reactivate this.
  //pip_close ();
  //pocc_options_free (poptions);

  return EXIT_SUCCESS;
}


/**
 * Annotate inner-most loops of a sub-tree with
 * dependence/vectorization information (no transformation is
 * performed).
 *
 *
 */
int PolyOptInnerLoopsAnnotateSubTree(SgNode* root, PolyRoseOptions& polyoptions)
{
  // 1- Retrieve pocc_options_t from PolyRoseOptions.
  s_pocc_options_t* poptions = polyoptions.buildPoccOptions ();
  s_pocc_utils_options_t* puoptions = pocc_utils_options_malloc ();
  if (! poptions->quiet)
    std::cout << "[PoCC] INFO: pass-thru compilation, no optimization enabled"
	      << std::endl;

  // 2- Extract all inner-most loops in the subtree.
  std::vector<SgNode*> innerLoops;
  SgNodePtrList forLoops =
    NodeQuery::querySubTree(root, V_SgForStatement);
  SgNodePtrList::const_iterator iter;
  // Iterate on all for loops in a file.
  for (iter = forLoops.begin(); iter != forLoops.end(); ++iter)
    {
      SgNodePtrList loops =
	NodeQuery::querySubTree(*iter, V_SgForStatement);
      if (loops.size() == 1)
	innerLoops.push_back(*iter);
    }

  std::vector<SgNode*>::iterator i;
  int scopId;
  for (i = innerLoops.begin(); i != innerLoops.end(); ++i)
    {
      // 3- Annotate all inner-most loop which are polyhedral.
      ScopExtractor extractor (polyoptions);
      extractor.extractScops(*i);
      std::vector<scoplib_scop_p> scops = extractor.getScoplibs();

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

	  // Compute the DDVs for the scop, if required.
	  if (polyoptions.getComputeDDV())
	    computeDDVinScop(*sage_scop_iterator, polyoptions);
	}
    }

  // Clear PoCC-wide options.
  /// LNP: FIXME: Reactivate this.
  //pip_close ();
  //pocc_options_free (poptions);

  return EXIT_SUCCESS;
}




/**
 * Annotate a whole ROSE project with dependence/permutability
 * information (no transformation is performed).
 *
 *
 */
int PolyOptAnnotateProject(SgProject* project, PolyRoseOptions& polyoptions)
{
  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Annotate polyhedral loops with DDV info" << std::endl;

  int scopId;
  // 1- Extract Scops from source tree.
  bool oldQuiet = polyoptions.getQuiet();
  polyoptions.setQuiet(true);
  ScopExtractor extractor (project, polyoptions);
  std::vector<scoplib_scop_p> scops = extractor.getScoplibs();
  polyoptions.setQuiet(oldQuiet);

  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Create annotation for " << scops.size()
	      << " SCoP(s)" << std::endl;
  std::vector<scoplib_scop_p>::const_iterator sage_scop_iterator;
  // 3- Iterate on each scop:
  for (sage_scop_iterator = scops.begin (), scopId = 1;
       sage_scop_iterator != scops.end ();
       ++sage_scop_iterator, ++scopId)
    {
      // Compute the DDVs for the scop, if required.
      if (polyoptions.getComputeDDV())
	computeDDVinScop(*sage_scop_iterator, polyoptions);
    }

  return EXIT_SUCCESS;
}


/**
 * Annotate a sub-tree with dependence/permutability information (no
 * transformation is performed).
 *
 *
 */
int PolyOptAnnotateSubTree(SgNode* root, PolyRoseOptions& polyoptions)
{
  // 1- Extract Scops from source tree.
  int scopId;
  bool oldQuiet = polyoptions.getQuiet();
  polyoptions.setQuiet(true);
  ScopExtractor extractor (polyoptions);
  extractor.extractScops(root);
  std::vector<scoplib_scop_p> scops = extractor.getScoplibs();
  polyoptions.setQuiet(oldQuiet);

  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Number of SCoPs to annotate: " << scops.size() << std::endl;
  std::vector<scoplib_scop_p>::const_iterator sage_scop_iterator;
  // 3- Iterate on each scop:
  for (sage_scop_iterator = scops.begin (), scopId = 1;
       sage_scop_iterator != scops.end ();
       ++sage_scop_iterator, ++scopId)
    {
      // Compute the DDVs for the scop, if required.
      if (polyoptions.getComputeDDV())
	computeDDVinScop(*sage_scop_iterator, polyoptions);
    }

  return EXIT_SUCCESS;
}





/******************************************************************************/
/*************************** PolyOpt SCoP Processing **************************/
/******************************************************************************/


static
void
erasePlutoFiles()
{
  std::remove(".pragmas");
  std::remove(".unroll");
  std::remove(".vectorize");
}

/**
 * Helper. Process a single scop with PoCC.
 *
 */
void
optimizeSingleScop(scoplib_scop_p scoplibScop,
		   int scopId,
		   s_pocc_options_t* poptions,
		   s_pocc_utils_options_t* puoptions,
		   PolyRoseOptions& polyoptions)
{
  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Optimizing Scop #"<< scopId << std::endl;
  if (polyoptions.isVerbose() && 0)
    {
      std::cout << "--- Printing SCoP ---" << std::endl;
      scoplib_scop_print_dot_scop_options (stdout,
					   scoplibScop,
					   SCOPLIB_SCOP_PRINT_ARRAYSTAG);
      std::cout << "--- End SCoP ---" << std::endl;
    }
  if (polyoptions.getScVerboseLevel() >=
      PolyRoseOptions::VERBOSE_COMPREHENSIVE)
    std::cout << ((SgNode*)(scoplibScop->usr))->unparseToCompleteString()
	      << std::endl;
  if (polyoptions.getScVerboseLevel() >= PolyRoseOptions::VERBOSE_FULL)
    scoplib_scop_print (stdout, scoplibScop);

  // 1- Call PoCC optimizations.
  erasePlutoFiles();
  if (polyoptions.getCodegenUsePAST())
    {
      s_past_node_t* pastRoot =
	OptimizeSingleScopWithPoccPast (scoplibScop, poptions, puoptions,
					polyoptions);
      if (! polyoptions.getQuiet())
	std::cout << "[PolyOpt] Processing with the PAST back-end"
		  << std::endl;

      // 2- Translate back Clast to Sage.
      // 2.1- Translate.
      SgStatement* scopRoot = isSgStatement((SgNode*)(scoplibScop->usr));
      ROSE_ASSERT(scopRoot);
      SgScopeStatement* scope = scopRoot->get_scope();
      PastToSage sageScop (scope, pastRoot, scoplibScop, polyoptions, scopId);
      ScopRootAnnotation* annot = 
	(ScopRootAnnotation*)sageScop.getBasicBlock()->getAttribute("ScopRoot");
      if (! annot)
	annot = new ScopRootAnnotation();
      annot->scopId = scopId;
      sageScop.getBasicBlock()->setAttribute("ScopRoot", annot);

      // 2.2- Reinsert.
      SageInterface::replaceStatement(scopRoot, sageScop.getBasicBlock(), true);
      past_deep_free (pastRoot);

    }
  else
    {
      clast_stmt* clastRoot =
	OptimizeSingleScopWithPocc (scoplibScop, poptions, puoptions,
				    polyoptions);
      if (! polyoptions.getQuiet())
	std::cout << "[PolyOpt] Processing with the CLAST back-end"
		  << std::endl;

      // 2- Translate back Clast to Sage.
      // 2.1- Translate.
      SgStatement* scopRoot = isSgStatement((SgNode*)(scoplibScop->usr));
      ROSE_ASSERT(scopRoot);
      SgScopeStatement* scope = scopRoot->get_scope();
      ClastToSage sageScop (scope, clastRoot, scoplibScop, polyoptions);

      // 2.2- Reinsert.
      SageInterface::replaceStatement(scopRoot, sageScop.getBasicBlock(), true);
      //cloog_clast_free (clastRoot);
    }
  if (! polyoptions.getQuiet())
    std::cout << "[PolyOpt] Scop #"<< scopId << " successfully optimized"
	      << std::endl;
}
