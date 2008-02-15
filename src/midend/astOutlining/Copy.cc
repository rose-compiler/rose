/*!
 *  \file ASTtools/Copy.hh
 *
 *  \brief Implements a wrapper routine for making a deep-copy of a
 *  node.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */

#include <rose.h>
#include "Copy.hh"

// ========================================================================

using namespace std;

// ========================================================================

static SgTreeCopy g_treeCopy;

SgNode *
ASTtools::deepCopy (const SgNode* n)
{
  return n ? n->copy (g_treeCopy) : 0;
}

// eof
