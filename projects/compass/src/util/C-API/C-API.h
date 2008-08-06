// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file C-API.h
 *  \brief Declares utility functions for describing a C-based API.
 */

#if !defined (INC_C_API_H)
#define INC_C_API_H //!< C-API.h included.

#include <map>
#include <string>
#include <istream>

#include <C-API/SigBase.h>
#include <C-API/FuncSig.h>
#include <C-API/ConstSig.h>
#include <C-API/TypeSig.h>

namespace C_API
{
  //! Defines a name-to-signature look-up table.
  typedef std::map<std::string, SigBase *> SigMap_t;

  /*!
   *  \brief Reads signature maps from a file.
   *  \returns 'true' on success, and populates 'sigs' with valid
   *  signatures.
   *
   *  The input file should be a sequence of lines, where each line is
   *  one of:
   *
   *    <CONST> name-of-constant
   *    <TYPE> name-of-type
   *    <SIG> name-of-function  name-of-arg-1  name-of-arg-2  ...  name-of-arg-n
   *
   *  Blank lines or lines beginning with '#' are ignored.  Leaves the
   *  file-pointer at the end of the file on return.
   *
   *  \pre 'sigs' is not NULL.
   */
  bool readSigs (std::istream& infile, SigMap_t* sigs);
}

#endif // !defined (INC_C_API_H)

// eof
