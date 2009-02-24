// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file Recognizer.C
 *
 *  \brief Implements utility routines for recognizing and querying
 *  MPI abstraction use.
 */
#include "rose.h"
#include <fstream>
#include "Recognizer.h"

// ========================================================================

using namespace std;

// ========================================================================

namespace MPIAbstraction
{
  Recognizer::Recognizer (const std::string& api_file)
  {
    ifstream infile (api_file.c_str ());
    if (infile.is_open ())
      readSigs (infile);
    setSigs (&mpi_sigs_);
  }

  Recognizer::Recognizer (std::istream& infile)
  {
    readSigs (infile);
    setSigs (&mpi_sigs_);
  }

  const C_API::SigMap_t &
  Recognizer::getSigs (void) const
  {
    return mpi_sigs_;
  }

  void
  Recognizer::readSigs (std::istream& infile)
  {
    C_API::readSigs (infile, &mpi_sigs_);
  }
} // namespace MPIAbstraction

// ========================================================================
// eof
