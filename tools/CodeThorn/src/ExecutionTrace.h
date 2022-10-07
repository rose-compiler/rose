#ifndef EXECUTION_TRACE_H
#define EXECUTION_TRACE_H

#include "sage3basic.h"
#include "EState.h"

namespace CodeThorn {

  class EState;
  class ExecutionTrace:
    public std::list<EStatePtr> {
  public:
    virtual ~ExecutionTrace() {};

  protected:
    template <class T>
    T onlyStatesSatisfyingTemplate(std::function<bool(EStatePtr)> predicate) const;
  };
}

// implementation of template function
template <class T>
T CodeThorn::ExecutionTrace::onlyStatesSatisfyingTemplate(std::function<bool(EStatePtr)> predicate) const {
  T newTrace;
  CodeThorn::ExecutionTrace::const_iterator begin = this->begin();
  CodeThorn::ExecutionTrace::const_iterator end = this->end();
  for (CodeThorn::ExecutionTrace::const_iterator i = begin; i != end; i++ ) {
    if (predicate(*i)) {
      newTrace.push_back(*i);
    }
  }
  return newTrace;
}

#endif
