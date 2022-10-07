#ifndef COUNTEREXAMPLE_GENERATOR_H
#define COUNTEREXAMPLE_GENERATOR_H

#include "EState.h"

namespace CodeThorn {

  class TransitionGraph;
  class ExecutionTrace;

/**
 * \author Joshua Asplund
 * \date June 2017
 */
  class CounterexampleGenerator {
  public:
    enum TraceType {TRACE_TYPE_NONE, TRACE_TYPE_RERS_CE, TRACE_TYPE_SVCOMP_WITNESS};

    CounterexampleGenerator(TransitionGraph* stg);
    CounterexampleGenerator(TraceType type, TransitionGraph* stg);
    void setType(TraceType type) { _type = type; };
    static void initDiagnostics();
    std::list<ExecutionTrace*> createExecutionTraces();
/**
 * @brief Extracts an execution trace from the STG's start state leading to "target"
 * @details Actual type of returned element depends on" _type" member variable (factory pattern)
 */
    ExecutionTrace* traceLeadingTo(EStatePtr target);

  private:
/**
 * @brief Extracts an execution trace using a backwards breadth first search
 * @details This will search backwards from source towards target and will return a forward
 *    trace. That is, the order of the trace will follow execution order.
 * 
 * @param source The search will starting here, moving backwards along the transition graph
 * @param target The search will end when encountering this state
 * @return A trace starting at target and ending at source (newly allocated on the heap).
 */
    template <class T> 
    T* reverseTraceBreadthFirst(EStatePtr source, EStatePtr target);

/**
 * @brief Extracts an execution trace using Dijkstra's algorithm (searching backwards)
 * @details This will search backwards from source towards target and will return a forward
 *    trace. That is, the order of the trace will follow execution order.
 * 
 * @param source The search will starting here, moving backwards along the transition graph
 * @param target The search will end when encountering this state
 * @return A trace starting at target and ending at source (newly allocated on the heap).
 */
    template <class T>
    T* reverseTraceDijkstra(EStatePtr source, EStatePtr target);

  protected:
    static Sawyer::Message::Facility logger;

  private:
    TraceType _type = TRACE_TYPE_NONE;
    TransitionGraph* _stg = nullptr;

  };

}

#endif
