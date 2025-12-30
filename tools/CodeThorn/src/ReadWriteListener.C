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
  void ReadWriteListener::readingFromMemoryLocation(Label, PStatePtr, AbstractValue&) {
  }
  void ReadWriteListener::writingToMemoryLocation(Label, PState*, AbstractValue&, AbstractValue&) {
  }
  // evalResult.value() holds AbstractValue of boolean value
  void ReadWriteListener::trueFalseEdgeEvaluation(Edge, SingleEvalResult&, EStatePtr) {
  }
  void ReadWriteListener::functionCallExternal(Edge, EStatePtr) {
  }
    void ReadWriteListener::setLabeler(Labeler* labeler) {
    _labeler=labeler;
  }

  Labeler* ReadWriteListener::getLabeler() {
    return _labeler;
  }
}
