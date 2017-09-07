#include "sage3basic.h"
#include "CounterexampleGenerator.h"
#include "Analyzer.h"
#include "Diagnostics.h"

#include <unordered_map>
#include <unordered_set>

#include "boost/heap/fibonacci_heap.hpp"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
ExecutionTrace ExecutionTrace::onlyIStates() const {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO();
  };
  return onlyStatesSatisfying(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
ExecutionTrace ExecutionTrace::onlyIOStates() const {
  function<bool(const EState*)> predicate = [](const EState* s) { 
    return s->io.isStdInIO() || s->io.isStdOutIO();
  };
  return onlyStatesSatisfying(predicate);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
ExecutionTrace ExecutionTrace::onlyStatesSatisfying(std::function<bool(const EState*)> predicate) const {
  ExecutionTrace newTrace;
  ExecutionTrace::const_iterator begin = this->begin();
  ExecutionTrace::const_iterator end = this->end();
  for (ExecutionTrace::const_iterator i = begin; i != end; i++ ) {
    if (predicate(*i)) {
      newTrace.push_back(*i);
    }
  }
  return newTrace;
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
string ExecutionTrace::toRersIString(Analyzer* analyzer) const {
  return toRersIOString(analyzer, false);
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
string ExecutionTrace::toRersIOString(Analyzer* analyzer) const {
  return toRersIOString(analyzer, true);
}

/*! 
 * \author Marc Jasper
 * \date 2014, 2017.
 */
string ExecutionTrace::toRersIOString(Analyzer* analyzer, bool withOutput) const {
  stringstream result; 
  result << "[";
  ExecutionTrace::const_iterator begin = this->begin();
  ExecutionTrace::const_iterator end = this->end();
  bool firstSymbol = true;
  int counter = 0;
  for (ExecutionTrace::const_iterator i = begin; i != end; i++ ) {
    if ( (*i)->io.isStdInIO() || (withOutput && (*i)->io.isStdOutIO()) ) {
      if (!firstSymbol) {
	result << ";";
      }
      const PState* pstate = (*i)->pstate();
      int inOutVal = pstate->readFromMemoryLocation((*i)->io.var).getIntValue();
      if ((*i)->io.isStdInIO()) {
	result << "i" << toRersChar(inOutVal);
	firstSymbol = false;
      } else if (withOutput && (*i)->io.isStdOutIO()) {
	result << "o" << toRersChar(inOutVal);
	firstSymbol = false;
      } 
    }
  }
  result << "]";
  return result.str();
}

/*! 
 * \author Marc Jasper
 * \date 2017.
 */
char ExecutionTrace::toRersChar(int value) const {
  return (char) (value + ((int) 'A') - 1);
}


//=============================================================================

Sawyer::Message::Facility CounterexampleGenerator::logger;

CounterexampleGenerator::CounterexampleGenerator(TransitionGraph* stg) : _stg(stg) {
}

void CounterexampleGenerator::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::CounterexampleGenerator", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

list<ExecutionTrace> CounterexampleGenerator::createExecutionTraces() {
  Analyzer* analyzer = _stg->getAnalyzer();
  list<FailedAssertion> assertions = analyzer->getFirstAssertionOccurences();
  list<ExecutionTrace> traces;
  for (auto i : assertions) {
    ExecutionTrace trace = reverseTraceBreadthFirst(i.second, _stg->getStartEState());
    traces.push_back(trace);
  }
  return traces;
}

/*! 
 * \author Marc Jasper
 * \date 2014.
 */
ExecutionTrace CounterexampleGenerator::reverseTraceBreadthFirst(const EState* source, const EState* target) {
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
      EStatePtrSet predsOfVertex = _stg->pred(vertex);
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

ExecutionTrace CounterexampleGenerator::reverseTraceDijkstra(const EState* source, const EState* target) {
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
  ExecutionTrace run;
  run.push_back(target);
  unordered_map <const EState*, const EState*>::iterator nextPred = predecessor.find(target);

  while (nextPred != predecessor.end()) {
    run.push_back(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }

  return run;
}
