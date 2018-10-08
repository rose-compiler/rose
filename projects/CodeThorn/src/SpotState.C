#include "sage3basic.h"
#include "rose_config.h"
#ifdef HAVE_SPOT

#include "SpotState.h"

using namespace CodeThorn;

SpotState::SpotState(const EState& e) : spot::state(), estate(e) {
}

int SpotState::compare(const spot::state* other) const {
  const SpotState* parameterState = dynamic_cast<const SpotState*>(other);
  assert(parameterState);
  if (estate < parameterState->estate) { return -1;}
  else if (estate == parameterState->estate) { return 0;}
  else { return 1;}
}

size_t SpotState::hash() const {
  EStateHashFun hashFn;
  long hashVal = hashFn(const_cast<EState*>(&estate));
  return (int) hashVal;
}

spot::state* SpotState::clone() const {
  SpotState *copy = new SpotState(*this);
  return copy;
}

void SpotState::destroy() const {
  this->~SpotState();
}

std::string SpotState::toString() const {
  std::string result;
  Labeler labeler;
  result = labeler.labelToString(estate.label());
  return result;
}

const EState& SpotState::getEState() const {
  return estate;
}

#endif // end of "#ifdef HAVE_SPOT"
