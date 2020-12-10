/**
 *  \file NameGenerator.cc
 *  \brief Class to generate numbered names.
 */
// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.



#include <sstream>
#include "NameGenerator.hh"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

NameGenerator::NameGenerator (size_t first_id)
  : cur_id_ (first_id)
{
}

NameGenerator::NameGenerator (const string& pre,
                              size_t first_id,
                              const string& suf)
  : cur_id_ (first_id), prefix_ (pre), suffix_ (suf)
{
}

size_t
NameGenerator::id (void) const
{
  return cur_id_;
}

string
NameGenerator::prefix (void) const
{
  return prefix_;
}

string
NameGenerator::suffix (void) const
{
  return suffix_;
}

string
NameGenerator::current (void) const
{
  stringstream s;
  s << prefix () << id () << suffix ();
  return s.str ();
}

string
NameGenerator::next (void)
{
  ++cur_id_;
  return current ();
}

// eof
