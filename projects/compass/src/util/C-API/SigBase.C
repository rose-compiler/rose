// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file SigBase.C
 *  \brief Implements the base class for the signatures of API entities.
 */

#include "SigBase.h"

using namespace std;

namespace C_API
{
  const string &
  SigBase::getName (void) const
  {
    return name_;
  }

  void
  SigBase::setName (const string& name)
  {
    name_ = name;
  }

  SigBase::SigBase (void)
  {
  }

  SigBase::SigBase (const string& name)
    : name_ (name)
  {
  }

  SigBase::~SigBase (void)
  {
  }
}

// eof
