/**
 *  \file LiaOutliner.cc
 *  \brief An outlining implementation.
 */

#include <iostream>
#include <string>
#include <sstream>

#include <rose.h>

#include "NameGenerator.hh"
#include "LiaOutliner.hh"
#include "Preprocess.hh"
#include "Transform.hh"

// =====================================================================

using namespace std;

// =====================================================================

//! Factory for unique outlined function names.
static NameGenerator g_outlined_func_names ("OUT__", 0, "__");

//! Hash a string into an unsigned long integer.
static
unsigned long
hashStringToULong (const string& s)
{
  unsigned long sum = 0;
  for (size_t i = 0; i < s.length (); ++i)
    sum += (unsigned long)s[i];
  return sum;
}

string
LiaOutliner::generateFuncName (const SgStatement* stmt)
{
  // Generate a prefix.
  stringstream s;
  s << g_outlined_func_names.next ();

  // Use the statement's raw filename to come up with a file-specific
  // tag.
  const Sg_File_Info* info = stmt->get_startOfConstruct ();
  ROSE_ASSERT (info);
  s << hashStringToULong (info->get_raw_filename ()) << "__";

  return s.str ();
}

// =====================================================================

LiaOutliner::Result
LiaOutliner::outline (SgStatement* s)
{
  string func_name = generateFuncName (s);
  return outline (s, func_name);
}

LiaOutliner::Result
LiaOutliner::outline (SgStatement* s, const std::string& func_name)
{
  SgBasicBlock* s_post = preprocess (s);
  ROSE_ASSERT (s_post);
  return Transform::outlineBlock (s_post, func_name);
}

SgBasicBlock *
LiaOutliner::preprocess (SgStatement* s)
{
  ROSE_ASSERT (isOutlineable (s, SgProject::get_verbose () >= 1));
  SgBasicBlock* s_post = Preprocess::preprocessOutlineTarget (s);
  ROSE_ASSERT (s_post); 
  return s_post;
}

/* =====================================================================
 *  Container to store the results of one outlining transformation.
 */

LiaOutliner::Result::Result (void)
  : decl_ (0), call_ (0)
{
}

LiaOutliner::Result::Result (SgFunctionDeclaration* decl,
                             SgStatement* call)
  : decl_ (decl), call_ (call)
{
}

LiaOutliner::Result::Result (const Result& b)
  : decl_ (b.decl_), call_ (b.call_)
{
}

bool
LiaOutliner::Result::isValid (void) const
{
  return decl_ && call_;
}

// eof
