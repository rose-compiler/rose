#include "PropertyState.h"

PropertyState::PropertyState() {
}

void PropertyState::toStream(std::ostream& os, VariableIdMapping* vim) {}

bool PropertyState::approximatedBy(PropertyState& other) {
  return false;
}

void PropertyState::combine(PropertyState& other){
}

PropertyStateFactory::PropertyStateFactory() {
}

PropertyStateFactory::~PropertyStateFactory() {
}
