// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file MPIAbstraction/Recognizer.h
 *
 *  \brief Declares utility routines for recognizing and querying the
 *  use of MPI abstractions.
 */

#if !defined (MPI_ABSTRACTION_H)
#define MPI_ABSTRACTION_H //!< MPIAbstraction.h included.

#include <istream>
#include <C-API/Recognizer.h>

namespace MPIAbstraction
{
  //! Recognizes MPI abstractions at SgNodes.
  class Recognizer : public C_API::Recognizer
  {
  public:
    Recognizer (const std::string& api_file = std::string ("MPI.api"));
    Recognizer (std::istream& infile);
    virtual ~Recognizer (void) {}

    //! Returns a const reference to the MPI signatures.
    const C_API::SigMap_t& getSigs (void) const;

  private:
    C_API::SigMap_t mpi_sigs_; //!< Stores MPI abstractions.

    //! Read MPI signatures from the specified stream to 'mpi_sigs_'.
    void readSigs (std::istream& infile);
  }; // class Recognizer
} // namespace MPIAbstraction

#endif // !defined (MPI_ABSTRACTION_H)

// eof
