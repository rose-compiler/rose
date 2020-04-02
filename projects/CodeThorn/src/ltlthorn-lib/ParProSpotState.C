#include "sage3basic.h"
#include "rose_config.h"
#ifdef HAVE_SPOT

#include "ParProSpotState.h"

using namespace CodeThorn;

ParProSpotState::ParProSpotState(const ParProEState& e) : spot::state(), estate(e) {
}

int ParProSpotState::compare(const spot::state* other) const {
  const ParProSpotState* parameterState = dynamic_cast<const ParProSpotState*>(other);
  assert(parameterState);
  if (estate < parameterState->estate) { return -1;}
  else if (estate == parameterState->estate) { return 0;}
  else { return 1;}
}

size_t ParProSpotState::hash() const {
  ParProEStateHashFun hashFn;
  long hashVal = hashFn(const_cast<ParProEState*>(&estate));
  return (int) hashVal;
}

spot::state* ParProSpotState::clone() const {
  ParProSpotState *copy = new ParProSpotState(*this);
  return copy;
}

void ParProSpotState::destroy() const {
  this->~ParProSpotState();
}

std::string ParProSpotState::toString() const {
  return estate.toString();
}

const ParProEState& ParProSpotState::getEState() const {
  return estate;
}

#endif // end of "#ifdef HAVE_SPOT"
