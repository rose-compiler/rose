// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file ConstSig.h
 *  \brief Declares a class for the constants of a C API.
 */

#if !defined (INC_C_API_CONSTSIG_H)
#define INC_C_API_CONSTSIG_H

#include <C-API/SigBase.h>

namespace C_API
{
  //! Stores information about an API-specific constant.
  class ConstSig : public SigBase
  {
  public:
    ConstSig (const std::string& name) : SigBase (name) {}
    virtual ~ConstSig (void) {}
    ConstSig (const ConstSig& sig) : SigBase (sig.getName ()) {}
  }; // class ConstSig
} // namespace C_API

#endif // !defined (INC_C_API_CONSTSIG_H)

// eof
