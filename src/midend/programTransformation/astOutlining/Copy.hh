/*!
 *  \file ASTtools/Copy.hh
 *
 *  \brief Implements a wrapper routine for making a deep-copy of a
 *  node.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 *
 *  This module differs from the low-level rewrite mechanism in that
 *  it provides specific operations on particular kinds of statement
 *  nodes. However, it could be merged with the low-level rewrite at
 *  some point in the future.
 */

#if !defined(INC_ASTTOOLS_COPY_HH)
//! ASTtools/Copy.hh included.
#define INC_ASTTOOLS_COPY_HH

namespace ASTtools
{
  //! Create a deep-copy of an SgNode
  ROSE_DLL_API SgNode* deepCopy (const SgNode* n);
}

#endif // !defined(INC_ASTTOOLS_COPY_HH)

// eof
