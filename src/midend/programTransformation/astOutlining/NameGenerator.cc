/**
 *  \file NameGenerator.cc
 *  \brief Class to generate numbered names.
 */
// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.



#include <sstream>
#include "NameGenerator.hh"
#include <ROSE_ASSERT.h>

using namespace std;

NameGenerator::NameGenerator (size_t first_id)
{
// all other should also start with this first_id  
  first_id_ = first_id;
  cur_id_[""]=first_id_;
}

NameGenerator::NameGenerator (const string& pre,
                              size_t first_id,
                              const string& suf)
  : prefix_ (pre), suffix_ (suf)
{
  first_id_ = first_id;
  cur_id_[""]=first_id_;
}

size_t
NameGenerator::id (std::string full_file_name) 
{
  // First time to get id? 
  if (cur_id_.count(full_file_name)==0)
    cur_id_[full_file_name] = first_id_;
  return cur_id_[full_file_name];
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
NameGenerator::current (std::string full_file_name) 
{
  stringstream s;
  s << prefix () << id (full_file_name) << suffix ();
  return s.str ();
}

string
NameGenerator::next (std::string full_file_name)
{
  // First time to get id? 
  if (cur_id_.count(full_file_name)==0)
    cur_id_[full_file_name] = first_id_;
  cur_id_[full_file_name]++;
  return current (full_file_name);
}

// eof
