/**
 *  \file Preprocess/Preprocess.cc
 *  \brief Outlining preprocessor.
 *
 *  This module implements functionality to transform an arbitrary
 *  outlineable statement into a canonical form that simplifies the
 *  core outlining algorithm.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <iostream>
#include <list>
#include <string>


#include "Preprocess.hh"
#include "Outliner.hh"

// =====================================================================

using namespace std;

// a lookup table to avoid inserting headers more than once for a file
static std::map<std::string, bool> fileHeaderMap; 

void Outliner::Preprocess::checkAndPatchUpOptions()
{
  // useStructureWrapper is a sub option of useParameterWrapper
  // Setting useStructureWrapper means useParameterWrapper should be true also
  if (useStructureWrapper && ! useParameterWrapper)
  {
    cout<<"Warning: Outliner::useParameterWrapper is automatically set to be true since useStructureWrapper is set."<<endl;
    useParameterWrapper = true;
  }

  //TODO  add other checks and patch up  
  
}


SgBasicBlock *
Outliner::Preprocess::preprocessOutlineTarget (SgStatement* s)
{
  checkAndPatchUpOptions();

  // insert a header to support outlining for auto tuning
  if (use_dlopen)
  {
    const string file_name = s->get_file_info()->get_filename();
    if (fileHeaderMap[file_name]!=true)
    {
   // DQ (3/19/2019): Suppress the output of the #include "autotuning_lib.h" since some tools will want 
   // to define there own supporting libraries and header files. The original behavior is the default.
   // SageInterface::insertHeader(AUTOTUNING_LIB_HEADER,PreprocessingInfo::after, false, s->get_scope());
      if (suppress_autotuning_header == false)
         {
           SageInterface::insertHeader(AUTOTUNING_LIB_HEADER,PreprocessingInfo::after, false, s->get_scope());
         }
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
    case V_SgBasicBlock: // already a block, nothing to do
      s_post = isSgBasicBlock(s);
      break;
    default:
      s_post = createBlock (s);
      break;
    }
  ROSE_ASSERT (s_post);

  // Extract preprocessing control structure.
  // We skip this during OpenMP lowering, it has some bug
  // Liao, 9/8/2009
  if (! SageInterface::getEnclosingFileNode(s)->get_openmp_lowering()) 
    s_post = transformPreprocIfs (s_post);

  // Convert 'this' expressions into references to a local pointer.
  s_post = transformThisExprs (s_post);

  // Transform non-local control flow.
  // We skip this during OpenMP lowering for two reasons:
  //  1. non-local jump within a parallel region is illegal
  //  2. The support for Fortran is lacking. 
  // TODO better solution is to add Fortran support and report illegal non-local jumps 
  // Liao, 1/10/2011
  if (! SageInterface::getEnclosingFileNode(s)->get_openmp_lowering()) 
    s_post = transformNonLocalControlFlow (s_post);

  // Make sure we duplicate any locally declared function prototypes.
  gatherNonLocalDecls (s_post);

  // Check return value before returning.
  ROSE_ASSERT (s_post);
  return s_post;
}

// eof
