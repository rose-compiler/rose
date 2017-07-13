#include "sage3basic.h"
#include "AnalyzerTools.h"
#include "Diagnostics.h"

#include <unordered_map>
#include <unordered_set>

#include "boost/heap/fibonacci_heap.hpp"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

ExecutionTrace ExecutionTrace::onlyIOStates() const {
  ExecutionTrace newTrace;
  ExecutionTrace::const_iterator begin = this->begin();
  ExecutionTrace::const_iterator end = this->end();
  for (ExecutionTrace::const_iterator i = begin; i != end; i++ ) {
    if ((*i)->io.isStdInIO() || (*i)->io.isStdOutIO()) {
      newTrace.push_back(*i);
    }
  }
  return newTrace;
}

string ExecutionTrace::toString() const {
  string result = "[";
  ExecutionTrace::const_iterator begin = this->begin();
  ExecutionTrace::const_iterator end = this->end();
  for (ExecutionTrace::const_iterator i = begin; i != end; i++ ) {
    if (i != begin) {
      result += ";";
    }
    if ((*i)->io.isStdInIO()) {
      result += "i";
    } else if ((*i)->io.isStdOutIO()) {
      result += "o";
    }
    result += (*i)->toString();
  }
  result += "]";
  return result;
}

string ExecutionTrace::toString(SPRAY::VariableIdMapping* variableIdMapping) const {
  string result = "[";
  ExecutionTrace::const_iterator begin = this->begin();
  ExecutionTrace::const_iterator end = this->end();
  for (ExecutionTrace::const_iterator i = begin; i != end; i++ ) {
    if (i != begin) {
      result += ";";
    }
    if ((*i)->io.isStdInIO()) {
      result += "i";
    } else if ((*i)->io.isStdOutIO()) {
      result += "o";
    }
    result += (*i)->toString(variableIdMapping);
  }
  result += "]";
  return result;
}



//=============================================================================

Sawyer::Message::Facility CounterexampleGenerator::logger;

void CounterexampleGenerator::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::CounterexampleGenerator", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

list<ExecutionTrace> CounterexampleGenerator::createExecutionTraces(Analyzer* analyzer) {
  TransitionGraph* graph = analyzer->getTransitionGraph();
  list<FailedAssertion> assertions = analyzer->getFirstAssertionOccurences();
  list<ExecutionTrace> traces;
  for (auto i : assertions) {
    ExecutionTrace trace = this->traceBreadthFirst(graph,i.second, graph->getStartEState());
    traces.push_back(trace);
  }
  return traces;
}

ExecutionTrace CounterexampleGenerator::traceBreadthFirst(TransitionGraph* transitionGraph, const EState* source, const EState* target) {
  // 1.) init: list wl , hashmap predecessor, hashset visited
  list<const EState*> worklist;
  worklist.push_back(source);
  boost::unordered_map <const EState*, const EState*> predecessor;
  boost::unordered_set<const EState*> visited;
  // 2.) while (elem in worklist) {s <-- pop wl; if (s not yet visited) {update predecessor map;
  //                                check if s==target: yes --> break, no --> add all pred to wl }}
  bool targetFound = false;
  while (worklist.size() > 0 && !targetFound) {
    const EState* vertex = worklist.front();
    worklist.pop_front();
    if (visited.find(vertex) == visited.end()) {  //avoid cycles
      visited.insert(vertex);
      EStatePtrSet predsOfVertex = transitionGraph->pred(vertex);
      for(EStatePtrSet::iterator i=predsOfVertex.begin();i!=predsOfVertex.end();++i) {
        predecessor.insert(pair<const EState*, const EState*>((*i), vertex));
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
  ExecutionTrace run;
  run.push_back(target);
  boost::unordered_map <const EState*, const EState*>::iterator nextPred = predecessor.find(target);

  while (nextPred != predecessor.end()) {
    run.push_back(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }

  return run;
}


ExecutionTrace CounterexampleGenerator::traceDijkstra(TransitionGraph* transitionGraph, const EState* source, const EState* target) {
  typedef std::pair<size_t, const EState*> entry;

  // 1.) init: list wl , hashmap predecessor, hashset visited
  boost::heap::fibonacci_heap<entry> heap;
  heap.push(std::make_pair(std::numeric_limits<size_t>::max(), source));
  unordered_set<const EState*> visited;
  unordered_map <const EState*, const EState*> predecessor;

  bool targetFound = false;
  while (heap.size() > 0 && !targetFound) {
    auto top = heap.top();
    const EState* current = top.second;
    size_t priority = top.first;
    heap.pop();
    if (visited.count(current) == 0) {
      visited.insert(current);
      EStatePtrSet predsOfVertex = transitionGraph->pred(current);
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
  ExecutionTrace run;
  run.push_back(target);
  unordered_map <const EState*, const EState*>::iterator nextPred = predecessor.find(target);

  while (nextPred != predecessor.end()) {
    run.push_back(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }

  return run;
}
