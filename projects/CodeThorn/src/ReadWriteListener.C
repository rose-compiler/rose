#include "sage3basic.h"
#include <map>
#include <iomanip>
#include "ReadWriteListener.h"

using namespace std;

namespace CodeThorn {

  ReadWriteListener::ReadWriteListener() {
  }
  ReadWriteListener::~ReadWriteListener() {
  }
  void ReadWriteListener::readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& result) {
  }
  void ReadWriteListener::writingToMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc, AbstractValue& newValue) {
  }
  // evalResult.value() holds AbstractValue of boolean value
  void ReadWriteListener::trueFalseEdgeEvaluation(Edge edge, SingleEvalResult& evalResult , const EState* estate) {
  }
  void ReadWriteListener::functionCallExternal(Edge edge, const EState* estate) {
  }

}
