/*!
 *  \file ASTtools/Jumps.hh
 *
 *  \brief Implements routines to gather "jump" statements, including
 *  breaks, continues, gotos, and returns.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */

#if !defined(INC_ASTTOOLS_JUMPS_HH)
//! ASTtools/Jumps.hh included.
#define INC_ASTTOOLS_JUMPS_HH

#include <map>

class SgStatement;

namespace ASTtools
{
  //! Store a list of non-local "jumps" (continues, breaks) and ids.
  typedef std::map<const SgStatement *, size_t> JumpMap_t;

  //! Collect non-local "jumps" within a statement.
  void collectNonLocalJumps (const SgStatement* root, JumpMap_t& jumps);

  //! Collect non-local "goto" statements.
  void collectNonLocalGotos (const SgStatement* root, JumpMap_t& jumps);

} // namespace ASTtools

#endif // !defined(INC_ASTTOOLS_JUMPS_HH)

// eof
