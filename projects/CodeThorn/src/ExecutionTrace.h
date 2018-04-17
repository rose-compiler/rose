#ifndef EXECUTION_TRACE_H
#define EXECUTION_TRACE_H

namespace CodeThorn {

  class EState;

/**
 * @brief An ordered list of EStates.
 * 
 * \author Joshua Asplund
 * \date June 2017
 */
  class ExecutionTrace:
    public std::list<const EState*> {
  public:
    virtual ~ExecutionTrace() {};

  protected:
    template <class T>
    T onlyStatesSatisfyingTemplate(std::function<bool(const EState*)> predicate) const;
  };
}

// implementation of template function
#include "sage3basic.h"
#include "ExecutionTrace.h"
#include "EState.h"

using namespace CodeThorn;
using namespace std;

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
template <class T>
T ExecutionTrace::onlyStatesSatisfyingTemplate(std::function<bool(const EState*)> predicate) const {
  T newTrace;
  ExecutionTrace::const_iterator begin = this->begin();
  ExecutionTrace::const_iterator end = this->end();
  for (ExecutionTrace::const_iterator i = begin; i != end; i++ ) {
    if (predicate(*i)) {
      newTrace.push_back(*i);
    }
  }
  return newTrace;
}

#endif
