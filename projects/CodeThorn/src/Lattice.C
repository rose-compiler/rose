#include <iostream>
#include <sstream>
#include "Lattice.h"

SPRAY::Lattice::Lattice() {
}

SPRAY::Lattice::~Lattice() {
}

void SPRAY::Lattice::toStream(std::ostream& os, VariableIdMapping* vim) {
  os<<"<lattice-element>";
}

std::string SPRAY::Lattice::toString(VariableIdMapping* vim) {
  std::stringstream ss;
  toStream(ss,vim);
  return ss.str();
}

#if 0
bool SPRAY::Lattice::approximatedBy(Lattice& other) {
  return false;
}
#endif

void SPRAY::Lattice::combine(Lattice& other){
}

bool SPRAY::Lattice::isBot(){
  ROSE_ASSERT(false);
}
