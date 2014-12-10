#include <iostream>
#include <sstream>
#include "Lattice.h"

Lattice::Lattice() {
}

Lattice::~Lattice() {
}

void Lattice::toStream(std::ostream& os, VariableIdMapping* vim) {
}

std::string Lattice::toString(VariableIdMapping* vim) {
  std::stringstream ss;
  toStream(ss,vim);
  return ss.str();
}

#if 0
bool Lattice::approximatedBy(Lattice& other) {
  return false;
}
#endif

void Lattice::combine(Lattice& other){
}

bool Lattice::isBot(){
  ROSE_ASSERT(false);
}
