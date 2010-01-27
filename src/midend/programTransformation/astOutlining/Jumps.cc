/*!
 *  \file ASTtools/Jumps.hh
 *
 *  \brief Implements routines to gather "jump" statements, including
 *  breaks, continues, gotos, and returns.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Jumps.hh"

// ========================================================================

using namespace std;

// ========================================================================

/*!
 *  Keeps track of different kinds of scopes as they are encountered
 * during a top-down traversal.
 */
struct MostRecentScope
{
  const SgStatement* loop_; // for, while, or do-while loops.
  const SgSwitchStatement* switch_; // switch statements.
  
  // Constructors
  MostRecentScope (void) : loop_ (0), switch_ (0) {}
  MostRecentScope (const MostRecentScope& x) : loop_ (x.loop_), switch_ (x.switch_) {}
  
  // Constructor that simultaneously initializes and updates
  MostRecentScope (const MostRecentScope& x, const SgStatement* s)
    : loop_ (x.loop_), switch_ (x.switch_)
  {
    update (s);
  }
  
  // Update this structure to reflect a new statement.
  bool update (const SgStatement* s)
  {
    bool did_update = false;
    if (s)
      switch (s->variantT ())
        {
        case V_SgForStatement:
        case V_SgWhileStmt:
        case V_SgDoWhileStmt:
          loop_ = s;
          did_update = true;
          break;
        case V_SgSwitchStatement:
          switch_ = isSgSwitchStatement (s);
          did_update = true;
          break;
        default: // do nothing
          break;
        }
    return did_update;
  }
};

/*!
 *  \brief Traversal to collect non-local jumps.
 *
 *  In particular, This sub-tree traversal begins at the given node,
 *  'root', and descends to collect the following statements:
 *
 *  - All return statements not enclosed by any function definition
 *  within 'root';
 *
 *  - All break statements not enclosed by any loop or switch;
 *
 *  - All continue statements not enclosed by any loop.
 */
class NonLocalJumpCollector : public AstTopDownProcessing<MostRecentScope>
{
public:
  NonLocalJumpCollector (ASTtools::JumpMap_t& J) : jumps_ (J), next_id_ (0) {}
  
  virtual MostRecentScope
  evaluateInheritedAttribute (SgNode* n, MostRecentScope s)
  {
    bool do_collect = false;
    ROSE_ASSERT (n);
    switch (n->variantT ())
      {
      case V_SgBreakStmt:
        do_collect = !s.loop_ && !s.switch_;
        break;
      case V_SgContinueStmt:
        do_collect = !s.loop_;
        break;
      case V_SgReturnStmt:
        do_collect = true;
        break;
      default: // Do not collect
        break;
      }
    if (do_collect)
      jumps_[isSgStatement (n)] = nextId ();
    return MostRecentScope (s, isSgStatement (n));
  }

protected:
  size_t nextId (void) { return ++next_id_; }

private:
  ASTtools::JumpMap_t& jumps_; // Where to store matching "jump" statements.
  size_t next_id_; // Next available jump ID number.
};

// ========================================================================

void
ASTtools::collectNonLocalJumps (const SgStatement* root, JumpMap_t& jumps)
{
  NonLocalJumpCollector collector (jumps);
  collector.traverse (const_cast<SgStatement *> (root), MostRecentScope ());
}

void
ASTtools::collectNonLocalGotos (const SgStatement* root, JumpMap_t& jumps)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t gotos = NodeQuery::querySubTree (const_cast<SgStatement *> (root),
                                              V_SgGotoStatement);
  NodeList_t labels = NodeQuery::querySubTree (const_cast<SgStatement *> (root),
                                               V_SgLabelStatement);

  // Convert label list into a somewhat faster lookup table.
  typedef set<const SgLabelStatement *> LabelSet_t;
  LabelSet_t label_set;
  for (NodeList_t::const_iterator i = labels.begin ();
       i != labels.end (); ++i)
    label_set.insert (isSgLabelStatement (*i));

  // Loop over all gotos, seeing if they match any local labels.
  size_t next_id = 1;
  for (NodeList_t::const_iterator i = gotos.begin (); i != gotos.end (); ++i)
    {
      const SgGotoStatement* g = isSgGotoStatement (*i);
      if (label_set.find (g->get_label ()) == label_set.end ()) // Non-local
        jumps[g] = next_id++;
    }
}

// eof
