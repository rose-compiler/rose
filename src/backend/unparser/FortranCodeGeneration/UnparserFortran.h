#ifndef UNPARSER_FORTRAN_H
#define UNPARSER_FORTRAN_H

#include "UnparserDelegate.h"

//----------------------------------------------------------------

struct UnparserFortran : public UnparseDelegate {

  UnparserFortran() = default;

  virtual ~UnparserFortran() {
    std::cerr << "UnparserFortran destructor called\n";
  };

  // Entry point to unparser from Rose.
  virtual bool unparse_statement(SgStatement* stmt, SgUnparse_Info& info, UnparseFormat& out) override;

};

#endif // UNPARSER_FORTRAN_H
