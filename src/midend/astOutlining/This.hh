/*!
 *  \file ASTtools/This.hh
 *
 *  \brief Implements routines to assist in variable symbol analysis
 *  and manipulation.
 */

#if !defined(INC_ASTTOOLS_THIS_HH)
//! ASTtools/This.hh included.
#define INC_ASTTOOLS_THIS_HH

#include <set>

class SgThisExp;
class SgStatement;

namespace ASTtools
{
  //! Stores a list of 'this' expressions.
  typedef std::set<const SgThisExp *> ThisExprSet_t;

  //! Collect 'this' expressions.
  void collectThisExpressions (const SgStatement* root, ThisExprSet_t& exprs);

} // namespace ASTtools

#endif // !defined(INC_ASTTOOLS_THIS_HH)

// eof
