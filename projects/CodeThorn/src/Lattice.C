#include <iostream>
#include <sstream>
#include "Lattice.h"


CodeThorn::Lattice::~Lattice() {
}

void CodeThorn::Lattice::toStream(std::ostream& os, VariableIdMapping* vim) {
  os<< isBot() ? "bot" : "<lattice-element>";
}

std::string CodeThorn::Lattice::toString(VariableIdMapping* vim) {
  std::stringstream ss;
  toStream(ss,vim);
  return ss.str();
}

#if 0
bool CodeThorn::Lattice::approximatedBy(Lattice& other) {
  return false;
}

bool CodeThorn::Lattice::approximatedByAsymmetric(Lattice& other) const {
  return approximatedBy(other);
}

void CodeThorn::Lattice::combine(Lattice& other){
}

void CodeThorn::Lattice::combineAsymmetric(Lattice& other) {
  combine(other);
}

bool CodeThorn::Lattice::isBot() const {
  ROSE_ASSERT(false);
}
#endif

