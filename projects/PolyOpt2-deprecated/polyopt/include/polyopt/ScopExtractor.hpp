/*
 * ScopExtractor.hpp: This file is part of the PolyOpt project.
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
 * @file: ScopExtractor.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#if !defined(PLUTO_ROSE_SCOP_EXTRACTOR_INC)
#define PLUTO_ROSE_SCOP_EXTRACTOR_INC 1

# include <polyopt/PolyRoseOptions.hpp>
# ifndef SCOPLIB_INT_T_IS_LONGLONG
#  define SCOPLIB_INT_T_IS_LONGLONG
# endif
# include <scoplib/scop.h>

/**
 * \brief Sage AST analyzer for extracting SCoPs.
 *
 * This class implements a Sage AST analyzer that identifies AST sub-trees that
 * constitute a SCoP.
 */
class ScopExtractor
{
public:

  /**
   * \brief Constructor
   *
   * Perform scop detection on the whole project after instantiating
   * the class.
   *
   * \param project     The Sage AST project to analyze.
   * \param polyoptions The project options.
   *
   */
  ScopExtractor(SgProject* project, PolyRoseOptions& polyoptions);

  /**
   * \brief Constructor
   *
   * Instantiate the object but do not perform scop extraction.
   *
   * \param project     The Sage AST project to analyze.
   * \param polyoptions The project options.
   *
   */
  ScopExtractor(PolyRoseOptions& polyopts);

  ~ScopExtractor();

  /**
   * Extract scops in a particular sub-tree.
   *
   */
  void extractScops(SgNode* root);


  /**
   * Accessor.
   *
   */
  std::vector<scoplib_scop_p>& getScoplibs();

  /**
   * Accessor.
   *
   */
  std::vector<SgNode*>& getScopRoots();

private:
  bool assertNodeIsCandidate(SgProject* project, SgNode* root);
  bool assertFunctionIsCandidate(SgProject* project, SgNode* root);
  void inspectBottomUpFunctionBody(SgProject* project, SgNode* root);

  // Attributes:
  PolyRoseOptions polyoptions;
  bool isVerbose;

  std::vector<scoplib_scop_p> _scoplibs;
  std::vector<SgNode*> _scopRoots;
};

#endif

