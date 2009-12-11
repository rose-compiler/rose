/*!
 *  \file ASTtools/StmtRewrite.hh
 *
 *  \brief Implements routines to support basic statement-level
 *  rewriting.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 *
 *  This module differs from the low-level rewrite mechanism in that
 *  it provides specific operations on particular kinds of statement
 *  nodes. However, it could be merged with the low-level rewrite at
 *  some point in the future.
 */

#if !defined(INC_ASTTOOLS_STMTREWRITE_HH)
//! ASTtools/StmtRewrite.hh included.
#define INC_ASTTOOLS_STMTREWRITE_HH

class SgStatement;
class SgBasicBlock;

namespace ASTtools
{
  //! Append a deep-copy of statement s onto the basic block b.
  void appendCopy (const SgStatement* s, SgBasicBlock* b);

  //! Append a deep-copy of statements from a to the basic block b.
  void appendStmtsCopy (const SgBasicBlock* a, SgBasicBlock* b);

  //! Replaces one statement with another.
  void replaceStatement (SgStatement* s_cur, SgStatement* s_new);

  //! Move statements from one basic-block to another.
  void moveStatements (SgBasicBlock* src, SgBasicBlock* target);

  /*!
   *  \brief Create an SgBasicBlock shell around an existing
   *  SgBasicBlock.
   *
   *  Given a basic block, B, this routine creates a new basic block,
   *  B', moves all statements in B to B', inserts B' into B, and
   *  returns B'.
   */
  SgBasicBlock* transformToBlockShell (SgBasicBlock* b_orig);

} // namespace ASTtools

#endif // !defined(INC_ASTTOOLS_STMTREWRITE_HH)

// eof
