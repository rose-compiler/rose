/*!
 *  \file CPreproc/IfDirectiveContextFinder.cc
 *
 *  \brief Implements a module to determine the '#if' directive
 *  context surrounding a specified target node.
 */
#include "rose.h"
#include "IfDirectiveContextFinder.hh"

// ========================================================================

using namespace std;

// ========================================================================

CPreproc::IfDirectiveContextFinder::IfDirectiveContextFinder (CPreproc::Ifs_t& ifs,
                                                              const SgLocatedNode* target)
  : IfDirectiveExtractor (ifs), target_ (target), top_ (0), bottom_ (0)
{
}

void
CPreproc::IfDirectiveContextFinder::visitTopDown (SgNode* n)
{
  IfDirectiveExtractor::visitTopDown (n);
  if (isSgLocatedNode (n) == target_)
    top_ = getCurrentContext ();
}

void
CPreproc::IfDirectiveContextFinder::visitBottomUp (SgNode* n)
{
  if (isSgLocatedNode (n) == target_)
    bottom_ = getCurrentContext ();
  IfDirectiveExtractor::visitBottomUp (n);
}

CPreproc::If::Case *
CPreproc::IfDirectiveContextFinder::getContextTop (void)
{
  return top_;
}

const CPreproc::If::Case *
CPreproc::IfDirectiveContextFinder::getContextTop (void) const
{
  return top_;
}

CPreproc::If::Case *
CPreproc::IfDirectiveContextFinder::getContextBottom (void)
{
  return bottom_;
}

const CPreproc::If::Case*
CPreproc::IfDirectiveContextFinder::getContextBottom (void) const
{
  return bottom_;
}

// eof
