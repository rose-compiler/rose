/**
 *  \file Preprocess/Preprocess.cc
 *  \brief Outlining preprocessor.
 *
 *  This module implements functionality to transform an arbitrary
 *  outlineable statement into a canonical form that simplifies the
 *  core outlining algorithm.
 */
#include <rose.h>
#include <iostream>
#include <list>
#include <string>


#include "Preprocess.hh"
#include "Outliner.hh"

// =====================================================================

using namespace std;

// a lookup table to avoid inserting headers more than once for a file
static std::map<std::string, bool> fileHeaderMap; 

SgBasicBlock *
Outliner::Preprocess::preprocessOutlineTarget (SgStatement* s)
{
  // insert a header to support outlining for auto tuning
  if (use_dlopen)
  {
    const string file_name = s->get_file_info()->get_filename();
    if (fileHeaderMap[file_name]!=true)
    {
      SageInterface::insertHeader(AUTOTUNING_LIB_HEADER,PreprocessingInfo::after, false, s->get_scope());
      fileHeaderMap[file_name]=true;
    }
  }  
  // Step 1: Make sure we outline an SgBasicBlock.
  SgBasicBlock* s_post = 0;
  ROSE_ASSERT (s);
  switch (s->variantT ())
    {
      // Liao, 4/14/2009
      // This branch does not make sense (will never reached) since Outliner::isOutlineable()
      // will exclude single variable declaration statement to be processed
    case V_SgVariableDeclaration:
      s_post = normalizeVarDecl (isSgVariableDeclaration (s));
      break;
    default:
      s_post = createBlock (s);
      break;
    }
  ROSE_ASSERT (s_post);

  // Extract preprocessing control structure.
  s_post = transformPreprocIfs (s_post);

  // Convert 'this' expressions into references to a local pointer.
  s_post = transformThisExprs (s_post);

  // Transform non-local control flow.
  s_post = transformNonLocalControlFlow (s_post);

  // Make sure we duplicate any locally declared function prototypes.
  gatherNonLocalDecls (s_post);

  // Check return value before returning.
  ROSE_ASSERT (s_post);
  return s_post;
}

// eof
