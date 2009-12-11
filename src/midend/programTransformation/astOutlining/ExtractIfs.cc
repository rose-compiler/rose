/*!
 *  \file CPreproc/ExtractIfs.cc
 *
 *  \brief Implements an algorithm to extract all '#if' directives and
 *  their nesting structure from an SgProject.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */
#include <rose.h>
#include <iostream>
#include <string>
#include <stack>


#include "ASTtools.hh"
#include "IfDirectiveExtractor.hh"
#include "IfDirectiveContextFinder.hh"

// ========================================================================

using namespace std;

// ========================================================================

void
CPreproc::extractIfDirectives (SgNode* root, CPreproc::Ifs_t& ifs)
{
  IfDirectiveExtractor extractor (ifs);
  extractor.traverse (root);
}

// ========================================================================

void
CPreproc::findIfDirectiveContext (const SgLocatedNode* target,
                                  CPreproc::Ifs_t& ifs,
                                  CPreproc::If::Case*& top,
                                  CPreproc::If::Case*& bottom)
{
  SgGlobal* root =
    TransformationSupport::getGlobalScope (const_cast<SgLocatedNode *> (target));
  if (!root) return;

  IfDirectiveContextFinder finder (ifs, target);
  finder.traverse (root);

  top = finder.getContextTop ();
  bottom = finder.getContextBottom ();    
}

// ========================================================================

//! Returns an indentation string for a given indent level.
static
string
indent (size_t level)
{
  if (level > 1)
    return indent (level-1) + string ("  ");
  else if (level == 1)
    return string ("  ");

  // Default: empty string
  return string ("");
}

void
CPreproc::dump (CPreproc::Ifs_t::const_iterator b,
                CPreproc::Ifs_t::const_iterator e,
                size_t level)
{
  size_t count = 0;
  for (CPreproc::Ifs_t::const_iterator i = b; i != e; ++i)
    {
      cerr << indent (level) << "=== [" << ++count << "] ===" << endl;

      CPreproc::If::Cases_t::const_iterator c;
      for (c = (*i)->begin (); c != (*i)->end (); ++c)
        {
          const SgLocatedNode* n = (*c)->getNode ();
          cerr << indent (level)
               << (*c)->getDirective ()
               << " : "
               << "'" << (*c)->getCondition () << "'"
               << ' ' << ASTtools::toStringFileLoc (n)
               << endl;
          dump ((*c)->begin (), (*c)->end (), level+1);
        }
    }
}
void 
CPreproc::dump (const Ifs_t ifs)
{
   CPreproc::Ifs_t::const_iterator b,e;
   b=ifs.begin();
   e=ifs.end();
   dump(b,e);
}

// eof
