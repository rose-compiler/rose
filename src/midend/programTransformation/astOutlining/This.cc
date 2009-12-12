/*!
 *  \file ASTtools/This.hh
 *
 *  \brief Implements routines to assist in variable symbol analysis
 *  and manipulation.
 */

#include <rose.h>
#include "This.hh"

// ========================================================================

using namespace std;

// ========================================================================

void
ASTtools::collectThisExpressions (const SgStatement* root,
                                  ThisExprSet_t& exprs)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t E = NodeQuery::querySubTree (const_cast<SgStatement *> (root),
                                          V_SgThisExp);
  for (NodeList_t::const_iterator i = E.begin (); i != E.end (); ++i)
    {
      const SgThisExp* t = isSgThisExp (*i);
      ROSE_ASSERT (t);
      exprs.insert (t);
    }
}

// eof
