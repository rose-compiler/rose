#include "sage3basic.h"
#include "AnalyzerTools.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;



string ExecutionTrace::toString() const {
  string result = "[";
  ExecutionTrace::const_iterator begin = this->begin();
  ExecutionTrace::const_iterator end = this->end();
  for (ExecutionTrace::const_iterator i = this->begin(); i != end; i++ ) {
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
  for (ExecutionTrace::const_iterator i = this->begin(); i != end; i++ ) {
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
    logger = Sawyer::Message::Facility("CodeThorn::CounterexampleGenerator", rose::Diagnostics::destination);
    rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
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
