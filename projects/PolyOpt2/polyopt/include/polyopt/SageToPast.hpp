/*
 * SageToPast.hpp: This file is part of the PolyOpt project.
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
 * @file: SageToPast.hpp
 * @author: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 */

#ifndef PLUTO_ROSE_SAGE_TO_PAST_INC
# define PLUTO_ROSE_SAGE_TO_PAST_INC


// LNP: To avoid redefined macro errors in pocc.
/// LNP: FIXME: Find a better solution!
# undef PACKAGE_BUGREPORT
# undef PACKAGE_STRING
# undef PACKAGE_TARNAME
# undef PACKAGE_NAME
# undef PACKAGE_VERSION
# include <rose.h>
# ifndef SCOPLIB_INT_T_IS_LONGLONG
#  define SCOPLIB_INT_T_IS_LONGLONG
# endif
# include <scoplib/scop.h>
# ifndef CLOOG_INT_GMP
#  define CLOOG_INT_GMP
# endif
# include <cloog/cloog.h>
# include <pragmatize/pragmatize.h>
# include <past/past.h>
# include <past/symbols.h>

# include <polyopt/PolyRoseOptions.hpp>
# include <AstTraversal.h>

# include <stack>
# include <map>


/**
 * \brief Converter from Past to Sage.
 *
 * Converts a Past representation of generated code to an equivalent Sage
 * representation.
 */
class SageToPast : public AstPrePostOrderTraversal
{
public:
  SageToPast (PolyRoseOptions& polyoptions) : _polyoptions(polyoptions), AstPrePostOrderTraversal() {}

  virtual void preOrderVisit(SgNode* node) {}

  // Postfix algorithm for SCoP detection.
  virtual void postOrderVisit(SgNode* node);


  s_past_node_t* toPast (SgNode* root);

private:
  PolyRoseOptions _polyoptions;
  std::stack<void*> _past_stack;
  s_symbol_table_t* _past_symt;
  std::map<SgNode*,s_symbol_t*> _structMap;
};

#endif

