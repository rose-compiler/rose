#include "sage3basic.h"
#include "AnalyzerTools.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility CounterexampleGenerator::logger;

void CounterexampleGenerator::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::CounterexampleGenerator", rose::Diagnostics::destination);
    rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}


CounterexampleGenerator::CounterexampleGenerator(Analyzer* analyzer):
  analyzer(analyzer) { }

EStateWorkList CounterexampleGenerator::reverseInOutSequenceBreadthFirst(const EState* errorState, const EState* target, bool counterexampleWithOutput, bool filterIO) {
  TransitionGraph* transitionGraph = analyzer->getTransitionGraph();
  // 1.) init: list wl , hashset predecessor, hashset visited
  EStateWorkList worklist;
  worklist.push_back(errorState);
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
    logger[ERROR]<< "target state not connected to source while generating reversed trace source --> target." << endl;
    ROSE_ASSERT(0);
  }
  // 3.) reconstruct trace. filter list of only input ( & output) states and return it
  EStateWorkList run;
  run.push_front(target);
  boost::unordered_map <const EState*, const EState*>::iterator nextPred = predecessor.find(target);

  while (nextPred != predecessor.end()) {
    run.push_front(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }

  EStateWorkList result;
  if (filterIO) {
    return filterStdInOutOnly(run, counterexampleWithOutput);
  } else {
    return run;
  }
}

EStateWorkList CounterexampleGenerator::reverseInOutSequenceDijkstra(const EState* source, const EState* target, bool counterexampleWithOutput, bool filterIO) {
  EStatePtrSet states = analyzer->getTransitionGraph()->estateSet();
  boost::unordered_set<const EState*> worklist;
  map <const EState*, int> distance;
  map <const EState*, const EState*> predecessor;
  //initialize distances and worklist
  for(EStatePtrSet::iterator i=states.begin();i!=states.end();++i) {
    worklist.insert(*i);
    if ((*i) == source) {
      distance.insert(pair<const EState*, int>((*i), 0));
    } else {
      distance.insert(pair<const EState*, int>((*i), (std::numeric_limits<int>::max() - 1)));
    }
  }
  ROSE_ASSERT( distance.size() == worklist.size() );

  //process worklist
  while (worklist.size() > 0 ) {
    //extract vertex with shortest distance to source
    int minDist = std::numeric_limits<int>::max();
    const EState* vertex = NULL;
    for (map<const EState*, int>::iterator i=distance.begin(); i != distance.end(); ++i) {
      if ( (worklist.find(i->first) != worklist.end()) ) {
        if ( (i->second < minDist)) {
          minDist = i->second;
          vertex = i->first;
        }
      }
    }
    //check for all predecessors if a shorter path leading to them was found
    EStatePtrSet predsOfVertex = analyzer->getTransitionGraph()->pred(vertex);
    for(EStatePtrSet::iterator i=predsOfVertex.begin();i!=predsOfVertex.end();++i) {
      int altDist;
      if( (*i)->io.isStdInIO() ) {
        altDist = distance[vertex] + 1;
      } else {
        altDist = distance[vertex]; //we only count the input sequence length
      }
      if (altDist < distance[*i]) {
        distance[*i] = altDist;
        //update predecessor
        map <const EState*, const EState*>::iterator predListIndex = predecessor.find((*i));
        if (predListIndex != predecessor.end()) {
          predListIndex->second = vertex;
        } else {
          predecessor.insert(pair<const EState*, const EState*>((*i), vertex));
        }
        //optimization: stop if the target state was found
        if ((*i) == target) {break;}
      }
    }
    int worklistReducedBy = worklist.erase(vertex);
    ROSE_ASSERT(worklistReducedBy == 1);
  }

  //extract and return input run from source to target (backwards in the STG)
  EStateWorkList run;
  run.push_front(target);
  map <const EState*, const EState*>::iterator nextPred = predecessor.find(target);
  while (nextPred != predecessor.end()) {
    run.push_front(nextPred->second);
    nextPred = predecessor.find(nextPred->second);
  }
  assert ((*run.begin()) == source);
  if (filterIO) {
    return filterStdInOutOnly(run, counterexampleWithOutput);
  } else {
    return run;
  }
}

EStateWorkList CounterexampleGenerator::filterStdInOutOnly(EStateWorkList& states, bool counterexampleWithOutput) const {
  EStateWorkList result;
  for (EStateWorkList::iterator i = states.begin(); i != states.end(); i++ ) {
    if( (*i)->io.isStdInIO() || (counterexampleWithOutput && (*i)->io.isStdOutIO())) {
      result.push_back(*i);
    }
  }
  return result;
}

string CounterexampleGenerator::reversedInOutRunToString(EStateWorkList& run) {
  string result = "[";
  for (EStateWorkList::reverse_iterator i = run.rbegin(); i != run.rend(); i++ ) {
    if (i != run.rbegin()) {
      result += ";";
    }
    //get input or output value
    // PState* pstate = const_cast<PState*>( (*i)->pstate() );
    // int inOutVal;
    if ((*i)->io.isStdInIO()) {
      result += "i";
      result += (*i)->toString();
    } else if ((*i)->io.isStdOutIO()) {
      result += "o";
      result += (*i)->toString();
    } else {
      ROSE_ASSERT(0);  //function is supposed to handle list of stdIn and stdOut states only
    }
  }
  result += "]";
  return result;
}

string CounterexampleGenerator::reversedRunToString(EStateWorkList& run) {
  VariableIdMapping* variableIdMapping = analyzer->getVariableIdMapping();
  string result = "[";
  for (EStateWorkList::reverse_iterator i = run.rbegin(); i != run.rend(); i++ ) {
    if (i != run.rbegin()) {
      result += ";\n";
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

