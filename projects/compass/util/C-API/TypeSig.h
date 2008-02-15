// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file TypeSig.h
 *  \brief Declares a class for the constants of a C API.
 */

#if !defined (INC_C_API_TYPESIG_H)
#define INC_C_API_TYPESIG_H

#include <C-API/SigBase.h>

namespace C_API
{
  //! Stores information about an API-specific constant.
  class TypeSig : public SigBase
  {
  public:
    TypeSig (const std::string& name) : SigBase (name) {}
    virtual ~TypeSig (void) {}
    TypeSig (const TypeSig& sig) : SigBase (sig.getName ()) {}
  }; // class TypeSig
} // namespace C_API

#endif // !defined (INC_C_API_TYPESIG_H)

// eof
