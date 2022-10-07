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
  void ReadWriteListener::readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc) {
  }
  void ReadWriteListener::writingToMemoryLocation(Label lab, PState* pstate, AbstractValue& memLoc, AbstractValue& newValue) {
  }
  // evalResult.value() holds AbstractValue of boolean value
  void ReadWriteListener::trueFalseEdgeEvaluation(Edge edge, SingleEvalResult& evalResult , EStatePtr estate) {
  }
  void ReadWriteListener::functionCallExternal(Edge edge, EStatePtr estate) {
  }
    void ReadWriteListener::setLabeler(Labeler* labeler) {
    _labeler=labeler;
  }

  Labeler* ReadWriteListener::getLabeler() {
    return _labeler;
  }
}
