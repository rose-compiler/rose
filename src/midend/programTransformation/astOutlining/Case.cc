/*!
 *  \file CPreproc/Case.cc
 *
 *  \brief Implements one 'case' of a preprocessor '#if' directive.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */
#include <rose.h>
#include <string>

#include "If.hh"
#include "ASTtools.hh"

//! Returns the prefix of a string up to the first matching tag.
static std::string upTo (const std::string& s, const std::string& tag);

//! Returns everything after the first occurrence of the given tag.
static std::string allAfter (const std::string& s, const std::string& tag);

//! Trims whitespace from the beginning and end of a string.
static std::string trim (const std::string& str);

// ========================================================================

using namespace std;

// ========================================================================
CPreproc::If::Case::Case (void)
{
}

CPreproc::If::Case::Case (PreprocessingInfo* info, SgLocatedNode* node,
                          If* parent)
  : info_ (info), node_ (node), parent_ (parent)
{
}

CPreproc::If::Case::Case (const Case& c)
  : info_ (c.info_), node_ (c.node_), parent_ (c.parent_), kids_ (c.kids_)
{
}

// ========================================================================

bool
CPreproc::If::Case::isIf (void) const
{
  return info_
    && (info_->getTypeOfDirective ()
        == PreprocessingInfo::CpreprocessorIfDeclaration);
}

bool
CPreproc::If::Case::isIfdef (void) const
{
  return info_
    && (info_->getTypeOfDirective ()
        == PreprocessingInfo::CpreprocessorIfdefDeclaration);
}

bool
CPreproc::If::Case::isIfndef (void) const
{
  return info_
    && (info_->getTypeOfDirective ()
        == PreprocessingInfo::CpreprocessorIfndefDeclaration);
}

bool
CPreproc::If::Case::isElif (void) const
{
  return info_
    && (info_->getTypeOfDirective ()
        == PreprocessingInfo::CpreprocessorElifDeclaration);
}

bool
CPreproc::If::Case::isElse (void) const
{
  return info_
    && (info_->getTypeOfDirective ()
        == PreprocessingInfo::CpreprocessorElseDeclaration);
}

bool
CPreproc::If::Case::isEndif (void) const
{
  return info_
    && (info_->getTypeOfDirective ()
        == PreprocessingInfo::CpreprocessorEndifDeclaration);
}

// ========================================================================

string
CPreproc::If::Case::getRaw (void) const
{
  if (info_)
    return info_->getString ();
  return string ("");
}

string
CPreproc::If::Case::getDirective (void) const
{
  string tag;
  if (isIf ())
    tag = "if";
  else if (isIfdef ())
    tag = "ifdef";
  else if (isIfndef ())
    tag = "ifndef";
  else if (isElif ())
    tag = "elif";
  else if (isElse ())
    tag = "else";
  else if (isEndif ())
    tag = "endif";
  else // Not a valid directive!
    return string ("");

  return trim (upTo (getRaw (), tag));
}

string
CPreproc::If::Case::getCondition (void) const
{
  return trim (allAfter (getRaw (), getDirective ()));
}

SgLocatedNode *
CPreproc::If::Case::getNode (void)
{
  return node_;
}

const SgLocatedNode *
CPreproc::If::Case::getNode (void) const
{
  return node_;
}

PreprocessingInfo *
CPreproc::If::Case::getInfo (void)
{
  return info_;
}

const PreprocessingInfo *
CPreproc::If::Case::getInfo (void) const
{
  return info_;
}

// ========================================================================

CPreproc::If::Case::iterator
CPreproc::If::Case::begin (void)
{
  return kids_.begin ();
}

CPreproc::If::Case::const_iterator
CPreproc::If::Case::begin (void) const
{
  return kids_.begin ();
}

CPreproc::If::Case::reverse_iterator
CPreproc::If::Case::rbegin (void)
{
  return kids_.rbegin ();
}

CPreproc::If::Case::const_reverse_iterator
CPreproc::If::Case::rbegin (void) const
{
  return kids_.rbegin ();
}

CPreproc::If::Case::iterator
CPreproc::If::Case::end (void)
{
  return kids_.end ();
}

CPreproc::If::Case::const_iterator
CPreproc::If::Case::end (void) const
{
  return kids_.end ();
}

CPreproc::If::Case::reverse_iterator
CPreproc::If::Case::rend (void)
{
  return kids_.rend ();
}

CPreproc::If::Case::const_reverse_iterator
CPreproc::If::Case::rend (void) const
{
  return kids_.rend ();
}

//! Returns an indentation string for a given indent level.
// not elegant since it is copied from ExtractIfs.cc
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


void CPreproc::If::Case::dump(size_t level /*=0*/)
{
  const SgLocatedNode* n = getNode();
  cerr << indent (level)
     << getDirective ()
     << " : "
     << "'" << getCondition () << "'"
     << ' ' << ASTtools::toStringFileLoc (n)
     << endl;
  CPreproc::dump (begin (), end (), level+1);
}
// ========================================================================

void
CPreproc::If::Case::appendChild (If* new_kid)
{
  ROSE_ASSERT (!isEndif ());

  if (new_kid)
    {
      kids_.push_back (new_kid);
      new_kid->setParent (this);
    }
}

// ========================================================================

void
CPreproc::If::Case::setIf (If* new_parent)
{
  parent_ = new_parent;
}

CPreproc::If *
CPreproc::If::Case::getIf (void)
{
  return parent_;
}

const CPreproc::If *
CPreproc::If::Case::getIf (void) const
{
  return parent_;
}

// ========================================================================

static
string
upTo (const string& s, const string& tag)
{
  string::size_type pos = s.find (tag);
  if (pos != string::npos)
    return s.substr (0, pos+tag.length ());
  return string ("");
}

static
string
allAfter (const string& s, const string& tag)
{
  string::size_type pos = s.find (tag);
  if (pos != string::npos)
    return s.substr (pos+tag.length ());
  return string ("");
}

static
string
trim (const string& s)
{
  string s_new (s);
  string::size_type pos = s_new.find_last_not_of (" \t\n\r");
  if (pos != string::npos)
    {
      s_new.erase (pos + 1);
      pos = s_new.find_first_not_of (" \t\n\r");
      if(pos != string::npos)
        s_new.erase (0, pos);
    }
  else
    s_new.erase (s_new.begin (), s_new.end ());
  return s_new;
}

// eof
