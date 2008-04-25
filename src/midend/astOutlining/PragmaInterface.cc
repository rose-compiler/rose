/**
 *  \file PragmaInterface.cc
 *  \brief A source-level, pragma-based interface to the outliner.
 *
 *  This module implements a high-level wrapper that permits a user to
 *  insert "#pragma rose_outline" statements into the body of the
 *  source, thereby directing what to outline at the source-level.
 *
 *  \todo Extend this interface.
 */

#include <string>
#include <rose.h>

#include "Outliner.hh"
#include "ASTtools.hh"
#include "PreprocessingInfo.hh"

//! Simplest outlining directives, applied to a single statement.
static const std::string PRAGMA_OUTLINE ("rose_outline");

//! Stores a list of valid outlining pragma directives.
typedef Rose_STL_Container<SgPragmaDeclaration *> PragmaList_t;

// =====================================================================

using namespace std;

// =====================================================================

/*!
 *  \brief Check whether the specified pragma is an outlining
 *  directive.
 *
 *  This routine checks whether the specified pragma is an outlining
 *  directive, and if so, returns the statement that should be
 *  outlined. Returns NULL if the pragma is not an outlining directive
 *  or no such statement exists.
 */
static
SgStatement *
processPragma (SgPragmaDeclaration* decl)
{
  if (!decl || !decl->get_pragma ())
    return 0;

  string pragmaString = decl->get_pragma ()->get_pragma ();
  if (pragmaString != PRAGMA_OUTLINE) // Not an outlining pragma.
    return 0;
    
  // Get statement to outline
  return const_cast<SgStatement *> (ASTtools::findNextStatement (decl));
}

/* =====================================================================
 *  Main routine to outline a single statement immediately following
 *  an outline directive (pragma).
 */

Outliner::Result
Outliner::outline (SgPragmaDeclaration* decl)
{
  SgStatement* s = processPragma (decl);
  if (!s)
    return Result ();

  // Generate outlined function, removing 's' from the tree.
  string name = generateFuncName (s);
  Result result = outline (s, name);
  ROSE_ASSERT (result.isValid ());

  // Remove pragma
  ASTtools::moveBeforePreprocInfo (decl, result.call_);
  ASTtools::moveAfterPreprocInfo (decl, result.call_);
  LowLevelRewrite::remove (decl);
  return result;
}

SgBasicBlock *
Outliner::preprocess (SgPragmaDeclaration* decl)
{
  SgStatement* s = processPragma (decl);
  if (s)
    return preprocess (s);
  else
    return 0;
}

// =====================================================================

/*!
 *  \brief Collects all outlining pragmas.
 *
 * This routine scans the given project for all outlining pragmas, and
 * returns them in the order in which they should be processed.
 *
 * The ordering is important because neither preorder nor postorder
 * tree traversals yield the desired bottomup processing for outlining
 * pragmas. To see why, consider the following code example:
 *
 * \code
 * #pragma rose_outline
 * {
 *   #pragma rose_outline
 *   statement1;
 * }
 * \endcode
 *
 * The corresponding AST is:
 *
 *   SgBasicBlock1
 *     /      \
 *    /        \
 * SgPragma1  SgBasicBlock2
 *              /      \
 *          SgPragma2  SgStatement1
 *
 * The standard traversal orders are:
 *
 * - Preorder: bb1, pragma1, bb2, pragma2, stmt1
 * - Postorder: pragma1, pragma2, stmt1,bb2, bb1
 *
 * In both cases, pragma1 is always visited before pragma2.
 *
 * The routine obtains a "correct" ordering by using the default
 * preorder AST query and then reversing the results.  In this we, we
 * obtain the ordering:
 *
 * - stmt1, pragma2, bb2,pragma1, bb1
 */
static
size_t
collectPragmas (SgProject* proj, PragmaList_t& pragmas)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t raw_list = NodeQuery::querySubTree (proj, V_SgPragmaDeclaration);
  size_t count = 0;
  for (NodeList_t::reverse_iterator i = raw_list.rbegin ();
       i != raw_list.rend (); ++i)
    {
      SgPragmaDeclaration* decl = isSgPragmaDeclaration (*i);
      if (processPragma (decl))
        {
          pragmas.push_back (decl);
          ++count;
        }
    }
  return count;
}

size_t
Outliner::outlineAll (SgProject* project)
{
  size_t num_outlined = 0;
  PragmaList_t pragmas;
  if (collectPragmas (project, pragmas))
    {
      for (PragmaList_t::iterator i = pragmas.begin ();
           i != pragmas.end (); ++i)
        if (outline (*i).isValid ())
          ++num_outlined;
    }
  return num_outlined;
}

size_t
Outliner::preprocessAll (SgProject* proj)
{
  size_t count = 0;
  PragmaList_t pragmas;
  if (collectPragmas (proj, pragmas))
    {
      for (PragmaList_t::iterator i = pragmas.begin ();
           i != pragmas.end (); ++i)
        if (preprocess (*i))
          ++count;
    }
  return count;
}

// eof
