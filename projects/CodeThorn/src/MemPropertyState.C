#include "sage3basic.h"

#include "MemPropertyState.h"
#include "SetAlgo.h"

CodeThorn::MemPropertyState::MemPropertyState() {
  //setBot();
}

CodeThorn::MemPropertyState::~MemPropertyState() {
  //setBot();
}

//void CodeThorn::MemPropertyState::toStream(ostream& os, CodeThorn::VariableIdMapping* vim) {
//}

std::string CodeThorn::MemPropertyState::toString(CodeThorn::VariableIdMapping* vim) {
  return "";
}

void CodeThorn::MemPropertyState::combine(Lattice& b) {
  MemPropertyState* other=dynamic_cast<MemPropertyState*>(&b);
  ROSE_ASSERT(other);
}

bool CodeThorn::MemPropertyState::approximatedBy(Lattice& b0) {
  MemPropertyState& b=dynamic_cast<MemPropertyState&>(b0);
  if(isBot()&&b.isBot())
    return true;
  if(isBot()) {
    return true;
  } else {
    if(b.isBot()) {
      return false;
    }
  }
  assert(!isBot()&&!b.isBot());
  // TODO
  return true;
}
