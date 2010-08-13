// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file FuncSig.C
 *  \brief Implements a class for the functions of a C API.
 */

#include "FuncSig.h"

using namespace std;

namespace C_API
{
  //=========================================================================
  FuncSig::FuncSig (const std::string& name, const Args_t& args)
    : SigBase (name), args_ (args)
  {
    buildMap ();
  }

  //=========================================================================
  FuncSig::FuncSig (const FuncSig& f)
    : SigBase (f.getName ()), args_ (f.args_)
  {
    buildMap ();
  }

  //=========================================================================
  size_t
  FuncSig::getNumArgs (void) const
  {
    return args_.size ();
  }

  //=========================================================================
  bool
  FuncSig::hasArgs (void) const
  {
    return !args_.empty ();
  }

  //=========================================================================
  FuncSig::const_iterator
  FuncSig::begin (void) const
  {
    return args_.begin ();
  }

  FuncSig::const_iterator
  FuncSig::end (void) const
  {
    return args_.end ();
  }

  //=========================================================================
  void
  FuncSig::buildMap (void)
  {
    args_map_.clear ();
    size_t arg_num = 0;
    for (Args_t::const_iterator arg = args_.begin ();
         arg != args_.end ();
         ++arg)
      {
        ++arg_num;
        args_map_[*arg] = arg_num;
      }
  }

  //=========================================================================
  size_t
  FuncSig::getArg (const std::string& name) const
  {
    ArgsMap_t::const_iterator i = args_map_.find (name);
    return (i != args_map_.end ()) ? i->second : 0;
  }
}

// eof
