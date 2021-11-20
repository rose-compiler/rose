#ifndef READ_WRITE_LISTENER_H
#define READ_WRITE_LISTENER_H

#include <map>
#include "BoolLattice.h"
#include "Labeler.h"
#include "EState.h"

namespace CodeThorn {
  class SingleEvalResult;
  
  class ReadWriteListener {

  public:
    ReadWriteListener();
    virtual ~ReadWriteListener();

    // result is value after reading from memLoc in pstate at label lab
    virtual void readingFromMemoryLocation(Label lab, PStatePtr pstate, AbstractValue& memLoc);

    // pstate is state at label lab before writing newValue to
    // memLoc. (*pstate).writeToMemoryLocation(memloc,result) gives
    // state after write
    virtual void writingToMemoryLocation(Label lab, PState* pstate, AbstractValue& memLoc, AbstractValue& newValue);

    // evalResult.value() holds AbstractValue of boolean value
    virtual void trueFalseEdgeEvaluation(Edge edge, SingleEvalResult& evalResult , EStatePtr estate);

    virtual void functionCallExternal(Edge edge, EStatePtr estate);

  };
}

#endif
