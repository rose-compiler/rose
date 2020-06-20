#include "sage3basic.h"

#include "MemPropertyState.h"
#include "SetAlgo.h"
#include <iostream>

using namespace std;

CodeThorn::MemPropertyState::MemPropertyState() {
  setBot(true);
}

CodeThorn::MemPropertyState::~MemPropertyState() {
}

void CodeThorn::MemPropertyState::setBot(bool flag) {
  _bot=flag;
}

bool CodeThorn::MemPropertyState::isBot() {
  return _bot;
}

//void CodeThorn::MemPropertyState::toStream(ostream& os, CodeThorn::VariableIdMapping* vim) {
//}

std::string CodeThorn::MemPropertyState::toString(CodeThorn::VariableIdMapping* vim) {
  return "";
}

void CodeThorn::MemPropertyState::combine(Lattice& b) {
  MemPropertyState* other=dynamic_cast<MemPropertyState*>(&b);
  ROSE_ASSERT(other);
  if(!isBot()||!other->isBot()) {
    setBot(false);
  }
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
