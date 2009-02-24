/*!
 *  \file RandomStmtSelector.cc
 *
 *  \brief Implements a routine that can select a random number of
 *  outlineable statements from a project.
 */
#include <rose.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>



#include <ASTtools.hh>
#include <Outliner.hh>
#include "RandomStmtSelector.hh"

// ========================================================================

typedef std::vector<SgFunctionDefinition *> FuncDefList_t;
typedef std::vector<SgStatement *> StmtVec_t;

// ========================================================================

using namespace std;

// ========================================================================

static
string
toString (const SgStatement* s, size_t id = 0)
{
  ROSE_ASSERT (s);
  const SgFunctionDeclaration* decl =
    SageInterface::getEnclosingFunctionDeclaration (const_cast<SgStatement*>(s));
  ROSE_ASSERT (decl);
  string func_tag (decl->get_qualified_name ().str ());

  stringstream str;
  if (id)
    str << "(" << id << ") ";
  str << func_tag << "()"
    << " -> "
    << ASTtools::toStringFileLoc (isSgLocatedNode (s))
    << " : <" << s->class_name () << ">";
  return str.str ();
}

// ========================================================================

static
void
collectFuncDefs (SgProject* proj, FuncDefList_t& funcs)
{
  // Traversal to collect function definitions within input files only.
  class FuncDefCollector : public AstSimpleProcessing
  {
  public:
    FuncDefCollector (FuncDefList_t& funcs) : funcs_ (funcs) {}
    virtual void visit (SgNode* n)
    {
      SgFunctionDefinition* f = isSgFunctionDefinition (n);
      if (f)
        funcs_.push_back (f);
    }
  private:
    FuncDefList_t& funcs_;
  };

  // Do collecting.
  FuncDefCollector collector (funcs);
  collector.traverseInputFiles (proj, preorder);
}

static
size_t
collectOutlineableStatements (SgBasicBlock* b, StmtVec_t& S)
{
  if (!b) return 0;

  // Traversal to count outlineable nodes.
  class CollectOutlineableTraversal : public AstSimpleProcessing
  {
  public:
    CollectOutlineableTraversal (StmtVec_t& S) : count_ (0), stmts_ (S) {}
    virtual void visit (SgNode* n)
    {
      SgStatement* s = isSgStatement (n);
      if (Outliner::isOutlineable (s))
        {
          if (SgProject::get_verbose () >= 3)
            cerr << '<' << s->class_name () << '>'
                 << ' ' << ASTtools::toStringFileLoc (s)
                 << " is outlineable."
                 << endl;

          count_++;
          stmts_.push_back (s);
        }
    }

    size_t count (void) const { return count_; }

  private:
    size_t count_;
    StmtVec_t& stmts_;
  };

  // Do collecting.
  CollectOutlineableTraversal collector (S);
  collector.traverse (b, preorder);
  return collector.count ();
}

static
size_t
collectOutlineableStatements (const FuncDefList_t& funcs,
                              StmtVec_t& stmts)
{
  size_t count = 0;
  for (FuncDefList_t::const_iterator i = funcs.begin ();
       i != funcs.end (); ++i)
    count += collectOutlineableStatements ((*i)->get_body (), stmts);
  return count;
}

// ========================================================================

void
RandomStmtSelector::selectOutlineable (SgProject* proj,
                                       size_t max_stmts,
                                       StmtSet_t& stmts)
{
  FuncDefList_t funcs;
  collectFuncDefs (proj, funcs);

  StmtVec_t all_stmts;
  size_t max_outline = collectOutlineableStatements (funcs, all_stmts);
  if (!max_outline)
    {
      cerr << "*** Warning: No statements to outline; exiting... ***" << endl;
      return;
    }
  else if (max_outline < max_stmts)
    {
      cerr << "*** Warning: Only "
           << max_outline
           << " statements may be outlined. ***"
           << endl;
      max_stmts = max_outline;
    }
  else
    cerr << "  [" << max_outline << " outlineable statements.]" << endl;

  size_t num_outlines = 0;
  size_t num_failures = 10 * max_outline; // Arbitrary limit on tries
  while (num_outlines < max_stmts && num_failures)
    {
      size_t s_id = lrand48 () % all_stmts.size ();
      SgStatement* s = all_stmts[s_id];
      ROSE_ASSERT (s);

      if (stmts.find (s) == stmts.end ())
        {
          cerr << "  " << toString (s, ++num_outlines) << endl;
          stmts.insert (s);
        }
      else
        --num_failures;
    }
  if (!num_failures)
    {
      cerr << "*** WARNING: Failure limit ("
           << num_failures
           << ") reached while trying to collect outlineable statements. ***"
           << endl;
    }
}

// eof
