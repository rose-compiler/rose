#ifndef ANALYZER_TOOLS_H
#define ANALYZER_TOOLS_H

#include "Analyzer.h"

namespace CodeThorn {

/**
 * @brief An ordered list of EStates.
 * 
 * \author Joshua Asplund
 * \date June 2017
 */
  class ExecutionTrace:
    public list<const EState*> {
  private:

  public:
    ExecutionTrace() {};
    ~ExecutionTrace() {};

    std::string toString() const;
    std::string toString(SPRAY::VariableIdMapping* variableIdMapping) const;

  };

/**
 * \author Joshua Asplund
 * \date June 2017
 */
  class CounterexampleGenerator {
  public:
    CounterexampleGenerator();
    ~CounterexampleGenerator();
    static void initDiagnostics();

/**
 * @brief Extracts an execution trace using a breadth first search
 * @details This will search backwards from source towards target and will return a forward
 *    trace. That is, the order of the trace will follow execution order.
 * 
 * @param transitionGraph The transition graph describing program execution
 * @param source The search will starting here, moving backwards along the transition graph
 * @param target The search will end when encountering this state
 * @return A trace starting at target and ending at source.
 */
    ExecutionTrace traceBreadthFirst(TransitionGraph* transitionGraph, const EState* source, const EState* target);

  protected:
    static Sawyer::Message::Facility logger;

  };

}

#endif
