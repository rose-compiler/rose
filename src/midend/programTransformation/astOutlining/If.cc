/*!
 *  \file CPreproc/If.cc
 *
 *  \brief Implements a tree structure suitable for storing a
 *  collection of nested '#if' directives.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "PreprocessingInfo.hh"
#include "If.hh"

// ========================================================================

using namespace std;

// ========================================================================

CPreproc::If::If (void)
  : parent_ (0)
{
}

CPreproc::If::~If (void)
{
  // Frees memory associated with all cases.
  for (iterator i = begin (); i != end (); ++i)
    delete (*i);
}

/*! \brief
 *
 *  This routine will check that it is a valid case for an '#if'
 *  directive. Moreover, it checks that it is really an
 *  '#if/#ifdef/#ifndef' if there are no cases yet; if there are
 *  cases, it checks that the last case appended was NOT an '#endif'.
 */
CPreproc::If::Case *
CPreproc::If::appendCase (PreprocessingInfo* info, SgLocatedNode* node)
{
  // Check that we only begin our cases with an '#if/ifdef/ifndef'.
  if (cases_.empty () && !ASTtools::isIfDirectiveBegin (info))
    return 0;

  // Check that we are not appending after an '#endif'.
  Case* last_c = lastCase ();
  if (last_c && last_c->isEndif ())
    return 0;

  // Seems safe to add this case...
  Case* c = new Case (info, node, this);
  ROSE_ASSERT (c);
  cases_.push_back (c);
  return c;
}

CPreproc::If::Case *
CPreproc::If::firstCase (void)
{
  return (begin () == end ()) ? 0 : *begin ();
}

const CPreproc::If::Case *
CPreproc::If::firstCase (void) const
{
  return (begin () == end ()) ? 0 : *begin ();
}

CPreproc::If::Case *
CPreproc::If::lastCase (void)
{
  return (rbegin () == rend ()) ? 0 : *rbegin ();
}

const CPreproc::If::Case *
CPreproc::If::lastCase (void) const
{
  return (rbegin () == rend ()) ? 0 : *rbegin ();
}

CPreproc::If::Case *
CPreproc::If::getParent (void)
{
  return parent_;
}

const CPreproc::If::Case *
CPreproc::If::getParent (void) const
{
  return parent_;
}

void
CPreproc::If::setParent (If::Case* parent)
{
  parent_ = parent;
}

bool
CPreproc::If::isValid (void) const
{
  if (cases_.size () < 2)
    return false;

  // Check that the first is an '#if'.
  const_iterator i = begin ();
  ROSE_ASSERT (i != end ());
  if (!(*i)->isIf () && !(*i)->isIfdef () && !(*i)->isIfndef ())
    return false;

  // Check for zero or more '#elif' directives.
  while (++i != end ())
    {
      if ((*i)->isElif ())
        ;
      else  // !(*i)->isElif ()
        break;
    }

  // Check for possible '#else'.
  if (i != end () && (*i)->isElse ())
    ++i;

  // Must end with an '#endif'.
  if (i == end () || !(*i)->isEndif ())
    return false;

  if (++i != end ())
    return false;

  // Passed all tests.
  return true;
}

CPreproc::If::iterator
CPreproc::If::begin (void)
{
  return cases_.begin ();
}

CPreproc::If::const_iterator
CPreproc::If::begin (void) const
{
  return cases_.begin ();
}

CPreproc::If::reverse_iterator
CPreproc::If::rbegin (void)
{
  return cases_.rbegin ();
}

CPreproc::If::const_reverse_iterator
CPreproc::If::rbegin (void) const
{
  return cases_.rbegin ();
}

CPreproc::If::iterator
CPreproc::If::end (void)
{
  return cases_.end ();
}

CPreproc::If::const_iterator
CPreproc::If::end (void) const
{
  return cases_.end ();
}

CPreproc::If::reverse_iterator
CPreproc::If::rend (void)
{
  return cases_.rend ();
}

CPreproc::If::const_reverse_iterator
CPreproc::If::rend (void) const
{
  return cases_.rend ();
}

// eof
