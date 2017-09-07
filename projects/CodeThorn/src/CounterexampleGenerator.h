#ifndef COUNTEREXAMPLE_GENERATOR_H
#define COUNTEREXAMPLE_GENERATOR_H

#include "VariableIdMapping.h"

namespace CodeThorn {

  class EState;
  class TransitionGraph;
  class Analyzer;

/**
 * @brief An ordered list of EStates.
 * 
 * \author Joshua Asplund
 * \date June 2017
 */
  class ExecutionTrace:
    public std::list<const EState*> {

  public:
    std::string toRersIString(Analyzer* analyzer) const;
    std::string toRersIOString(Analyzer* analyzer) const;

    ExecutionTrace onlyIStates() const;
    ExecutionTrace onlyIOStates() const;
    ExecutionTrace onlyStatesSatisfying(std::function<bool(const EState*)> predicate) const;

  private:
    std::string toRersIOString(Analyzer* analyzer, bool withOutput) const;
    char toRersChar(int value) const;
  };

/**
 * \author Joshua Asplund
 * \date June 2017
 */
  class CounterexampleGenerator {
  public:
    CounterexampleGenerator(TransitionGraph* stg);
    static void initDiagnostics();

    std::list<ExecutionTrace> createExecutionTraces();

/**
 * @brief Extracts an execution trace using a backwards breadth first search
 * @details This will search backwards from source towards target and will return a forward
 *    trace. That is, the order of the trace will follow execution order.
 * 
 * @param source The search will starting here, moving backwards along the transition graph
 * @param target The search will end when encountering this state
 * @return A trace starting at target and ending at source.
 */
    ExecutionTrace reverseTraceBreadthFirst(const EState* source, const EState* target);

/**
 * @brief Extracts an execution trace using Dijkstra's algorithm (searching backwards)
 * @details This will search backwards from source towards target and will return a forward
 *    trace. That is, the order of the trace will follow execution order.
 * 
 * @param source The search will starting here, moving backwards along the transition graph
 * @param target The search will end when encountering this state
 * @return A trace starting at target and ending at source.
 */
    ExecutionTrace reverseTraceDijkstra(const EState* source, const EState* target);

  protected:
    static Sawyer::Message::Facility logger;

  private:
    TransitionGraph* _stg = nullptr;

  };

}

#endif
