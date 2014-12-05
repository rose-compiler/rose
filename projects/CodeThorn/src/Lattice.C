#include "Lattice.h"

Lattice::Lattice() {
}

Lattice::~Lattice() {
}

void Lattice::toStream(std::ostream& os, VariableIdMapping* vim) {}

bool Lattice::approximatedBy(Lattice& other) {
  return false;
}

void Lattice::combine(Lattice& other){
}
