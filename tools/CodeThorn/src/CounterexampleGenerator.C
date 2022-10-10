#include "sage3basic.h"
#include "CounterexampleGenerator.h"
#include "RersCounterexample.h"
#include "SvcompWitness.h"
#include "CTAnalysis.h"
#include "Rose/Diagnostics.h"

#include <unordered_map>
#include <unordered_set>

#include "boost/heap/fibonacci_heap.hpp"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility CounterexampleGenerator::logger;

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
CounterexampleGenerator::CounterexampleGenerator(TransitionGraph* stg) : 
  _stg(stg) {
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
CounterexampleGenerator::CounterexampleGenerator(TraceType type, TransitionGraph* stg) : 
  _type(type),
  _stg(stg) {
}

void CounterexampleGenerator::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::CounterexampleGenerator", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

list<ExecutionTrace*> CounterexampleGenerator::createExecutionTraces() {
  CTAnalysis* analyzer = _stg->getAnalyzer();
  list<FailedAssertion> assertions = analyzer->getFirstAssertionOccurences();
  list<ExecutionTrace*> traces;
  for (auto i : assertions) {
    traces.push_back(traceLeadingTo(i.second));
  }
  return traces;
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
ExecutionTrace* CounterexampleGenerator::traceLeadingTo(EStatePtr target) {
  if (_type == TRACE_TYPE_RERS_CE) {
    return reverseTraceBreadthFirst<RersCounterexample>(target, _stg->getStartEState());
  } else if (_type == TRACE_TYPE_SVCOMP_WITNESS) {
    return reverseTraceBreadthFirst<SvcompWitness>(target, _stg->getStartEState());
  } else {
    throw CodeThorn::Exception("Unsupported trace type when generating a trace leading to target EState.");
  } 
}

/*! 
 * \author Marc Jasper
 * \date 2014.
 */
template <class T>
T* CounterexampleGenerator::reverseTraceBreadthFirst(EStatePtr source, EStatePtr target) {
  // 1.) init: list wl , hashmap predecessor, hashset visited
  list<EStatePtr> worklist;
  worklist.push_back(source);
  std::unordered_map <EStatePtr, EStatePtr> predecessor;
  std::unordered_set<EStatePtr> visited;
  // 2.) while (elem in worklist) {s <-- pop wl; if (s not yet visited) {update predecessor map;
  //                                check if s==target: yes --> break, no --> add all pred to wl }}
  bool targetFound = false;
  while (worklist.size() > 0 && !targetFound) {
    EStatePtr vertex = worklist.front();
    worklist.pop_front();
    if (visited.find(vertex) == visited.end()) {  //avoid cycles
      visited.insert(vertex);
      EStatePtrSet predsOfVertex = _stg->pred(vertex);
      for(EStatePtrSet::iterator i=predsOfVertex.begin();i!=predsOfVertex.end();++i) {
        predecessor.insert(pair<EStatePtr, EStatePtr>((*i), vertex));
        if ((*i) == target) {
          targetFound=true;
          break;
        } else {
          worklist.push_back((*i));
        }
      }
    }
  }
  if (!targetFound) {
    logger[ERROR]<< "Target state not connected to source" << endl;
    ROSE_ASSERT(0);
  }

  // 3.) reconstruct trace.
  T* run = new T;
  run->push_back(target);
  std::unordered_map <EStatePtr, EStatePtr>::iterator nextPred = predecessor.find(target);

  while (nextPred != predecessor.end()) {
    run->push_back(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }

  return run;
}

template<class T>
T* CounterexampleGenerator::reverseTraceDijkstra(EStatePtr source, EStatePtr target) {
  typedef std::pair<size_t, EStatePtr> entry;

  // 1.) init: list wl , hashmap predecessor, hashset visited
  boost::heap::fibonacci_heap<entry> heap;
  heap.push(std::make_pair(std::numeric_limits<size_t>::max(), source));
  unordered_set<EStatePtr> visited;
  unordered_map <EStatePtr, EStatePtr> predecessor;

  bool targetFound = false;
  while (heap.size() > 0 && !targetFound) {
    auto top = heap.top();
    EStatePtr current = top.second;
    size_t priority = top.first;
    heap.pop();
    if (visited.count(current) == 0) {
      visited.insert(current);
      EStatePtrSet predsOfVertex = _stg->pred(current);
      for (auto pred : predsOfVertex) {
        predecessor.insert(make_pair(pred, current));
        if (pred == target) {
          targetFound = true;
          break;
        } else {
          heap.push(make_pair(priority-1, pred));
        }
      }
    }
  }
  if (!targetFound) {
    logger[ERROR]<< "Target state not connected to source" << endl;
    ROSE_ASSERT(0);
  }

  // 3.) reconstruct trace.
  T* run = new T;
  run->push_back(target);
  unordered_map <EStatePtr, EStatePtr>::iterator nextPred = predecessor.find(target);

  while (nextPred != predecessor.end()) {
    run->push_back(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }

  return run;
}
