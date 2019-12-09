#ifndef LATTICE_H
#define LATTICE_H

#include "sage3basic.h"
#include <iostream>
#include <string>
#include "VariableIdMapping.h"

namespace CodeThorn {

class Lattice {
 public:
  Lattice();
  virtual bool isBot();
  virtual void toStream(std::ostream& os, CodeThorn::VariableIdMapping* vim);
  virtual std::string toString(CodeThorn::VariableIdMapping* vim=0);
  virtual bool approximatedBy(Lattice& other)=0;
  // schroder3 (2016-08-05): Asymmetric variant of approximatedBy. Default implementation
  //  calls the "normal" approximatedBy. The asymmetric variant is for example used to check
  //  whether the result of a transfer function of a backward edge is approximated by the
  //  the previous state.
  virtual bool approximatedByAsymmetric(Lattice& other);
  virtual void combine(Lattice& other);
  // schroder3 (2016-08-05): Asymmetric variant of combine. Default implementation
  //  calls the "normal" combine. The asymmetric variant is for example used if the
  //  result of a transfer function of a backward edge is combined with the previous
  //  state.
  virtual void combineAsymmetric(Lattice& other);
  virtual ~Lattice();
};

}

#endif
   
