// Author: Marc Jasper, 2016.

#include "ParProAnalyzer.h"

using namespace CodeThorn;
using namespace CodeThorn;
using namespace std;

ParProAnalyzer::ParProAnalyzer():
_startTransitionAnnotation(""),
_transitionGraph(new ParProTransitionGraph()),
_eStateSet(ParProEStateSet(true)),
_numberOfThreadsToUse(1),
_approximation(COMPONENTS_NO_APPROX) {}

ParProAnalyzer::ParProAnalyzer(std::vector<Flow*> cfas):
_startTransitionAnnotation(""),
_transitionGraph(new ParProTransitionGraph()),
_eStateSet(ParProEStateSet(true)),
_numberOfThreadsToUse(1),
_approximation(COMPONENTS_NO_APPROX) {
  init(cfas);
}

ParProAnalyzer::ParProAnalyzer(std::vector<Flow*> cfas, boost::unordered_map<int, int>& cfgIdToStateIndex): 
_startTransitionAnnotation(""),
_transitionGraph(new ParProTransitionGraph()),
_eStateSet(ParProEStateSet(true)),
_numberOfThreadsToUse(1),
_approximation(COMPONENTS_NO_APPROX) {
  init(cfas, cfgIdToStateIndex);
}

void ParProAnalyzer::init(std::vector<Flow*> cfas) {
  _cfas = cfas;
  for (unsigned int i=0; i<_cfas.size(); i++) {
    _cfgIdToStateIndex.insert(pair<int, int>(i,i));
  }
  _artificalTerminationLabels = vector<Label>(cfas.size());
  //TODO : select a label ID that is guaranteed to not exist yet (instead of the maximum of size_t)
  size_t max_size = (size_t)-1; // hope that not all of the parallel CFGs contain the maximum node id
  for (unsigned int i=0; i<_cfas.size(); i++) {
    _artificalTerminationLabels[i] = Label(max_size);
  }
}

void ParProAnalyzer::init(std::vector<Flow*> cfas, boost::unordered_map<int, int>& cfgIdToStateIndex) {
  init(cfas);
  _cfgIdToStateIndex = cfgIdToStateIndex;
}

ParProEState ParProAnalyzer::setComponentToTerminationState(unsigned int i, const ParProEState* state) {
  ParProLabel newLabel = state->getLabel();
  newLabel[i] = _artificalTerminationLabels[i];
  return ParProEState(newLabel);
}

void ParProAnalyzer::initializeSolver() {
  // generate the initial global EState of the parallel program
  ParProLabel startLabel = ParProLabel(_cfas.size());
  for (unsigned int i=0; i<_cfas.size(); i++) {
    startLabel[i] = _cfas[i]->getStartLabel();
  }
  ParProEState startState(startLabel);
  // add the start state to the set of known states
  const ParProEState* startStatePtr= _eStateSet.processNewOrExisting(startState);
  // add the start state to the worklist
  worklist.push_back(startStatePtr);
  _transitionGraph->setStartState(startStatePtr);
}

void ParProAnalyzer::runSolver() {
  int threadNum;
  int workers=_numberOfThreadsToUse;
  vector<bool> workVector(_numberOfThreadsToUse);
  set_finished(workVector,true);
  bool terminateEarly=false;
  omp_set_num_threads(workers);

  //cout <<"STATUS: Running solver p1 for parallel programs with "<<workers<<" threads."<<endl;
# pragma omp parallel shared(workVector) private(threadNum)
  {
    threadNum=omp_get_thread_num();
    while(!all_false(workVector)) {
      if(isEmptyWorkList()||isIncompleteStgReady()) {
#pragma omp critical
        {
          workVector[threadNum]=false;
        }
        continue;
      } else {
#pragma omp critical
        {
          if(terminateEarly)
            workVector[threadNum]=false;
          else
            workVector[threadNum]=true;
        }
      }
      // if we want to terminate early, we ensure to stop all threads and empty the worklist (e.g. verification error found).
      if(terminateEarly)
        continue;
      const ParProEState* currentEStatePtr=popWorkList();
      if(!currentEStatePtr) {
        assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
      } else {
        assert(currentEStatePtr);
	list<pair<Edge, ParProEState> > newEStateList = parProTransferFunction(currentEStatePtr);
	for (list<pair<Edge, ParProEState> >::iterator i=newEStateList.begin(); i!=newEStateList.end(); i++) {
	  ParProEStateSet::ProcessingResult pres = _eStateSet.process(i->second);
          const ParProEState* newEStatePtr = pres.second;
	  if (pres.first == true) {
	    addToWorkList(newEStatePtr);
	  }
	  _transitionGraph->add(ParProTransition(currentEStatePtr, i->first, newEStatePtr));
	}
      } // conditional: test if work is available
    } // while
  } // omp parallel
  if(isIncompleteStgReady()) {
    //cout << "STATUS: analysis finished (incomplete STG due to specified resource restriction)."<<endl;
    _transitionGraph->setIsComplete(false);
  } else {
    _transitionGraph->setIsComplete(true);
    //cout << "analysis finished (worklist is empty)."<<endl;
  }
  _transitionGraph->setIsPrecise(isPrecise());

  // TODO: remove this temporary test
  //cout << "DEBUG: _eStateSet size: " << _eStateSet.size() << endl;
  //  cout << "DEBUG: _eStateSet elements: " << endl;
  //  for (ParProEStateSet::iterator i=_eStateSet.begin(); i!=_eStateSet.end(); i++) {
  //    cout << "DEBUG: " << (*i)->toString() << endl;
  //  }
}

list<pair<Edge, ParProEState> > ParProAnalyzer::parProTransferFunction(const ParProEState* source) {
  list<pair<Edge, ParProEState> > result;
  ParProLabel sourceLabel = source->getLabel();
  // compute successor EStates based on the out edges of every CFG (one per parallel component)
  //  for (ParProLabel::iterator i=sourceLabel.begin(); i!=sourceLabel.end(); i++) {
  ROSE_ASSERT(_cfas.size() == sourceLabel.size());
  for (unsigned int i=0; i<_cfas.size(); i++) {
    if (_cfas[i]->contains(sourceLabel[i])) { // the artifical termination label will not be in the cfa, but has no outEdges anyways
      Flow outEdges = _cfas[i]->outEdges(sourceLabel[i]);
      for(Flow::iterator k=outEdges.begin(); k!=outEdges.end(); ++k) { 
	Edge e=*k;
	// TODO: combine "feasibleAccordingToGlobalState(...)" and "transfer(...)" to avoid 2nd lookup and iteration
	if (isPreciseTransition(e, source)) {
	  if (feasibleAccordingToGlobalState(e, source)) {
	    ParProEState target = transfer(source, e);
	    result.push_back(pair<Edge, ParProEState>(e, target));
	  }
	} else {
	  // we do not know whether or not the transition can be triggered
	  if (_approximation==COMPONENTS_OVER_APPROX) {
	    // we over-approximate the global system's behavior, therefore we generate the path where the tranistion is triggered...
	    ParProEState target = transfer(source, e);
	    result.push_back(pair<Edge, ParProEState>(e, target));
	    // ...but also include the case where the execution stops (none of these two cases is guaranteed to be part of the actual global behavior).
	    Edge terminationEdge = Edge(source->getLabel()[i], _artificalTerminationLabels[i]);
	    terminationEdge.setAnnotation("terminate (due to approximation)");
	    result.push_back(pair<Edge, ParProEState>(terminationEdge, setComponentToTerminationState(i, source)));
	  } else if (_approximation==COMPONENTS_UNDER_APPROX) {
	    // under-approximation here means to simply not include transitions that may or may not be feasible
	  } else {
	    cerr << "ERROR: some parallel CFGs are ignored and a synchronization tries to communicate with one of them, however no abstraction is selected." << endl;
	    ROSE_ASSERT(0);
	  }
	}
      }
    } // for each outgoing CFG edge of a particular parallel component's current label
  } // for each parallel component of the analyzed system
  return result;
}

bool ParProAnalyzer::isPreciseTransition(Edge e, const ParProEState* eState) {
  if (e.getAnnotation() == _startTransitionAnnotation) {
    return true; // unsynchronized transitions within the current STG computation are always precise
  }
  ParProLabel sourceLabel = eState->getLabel();
  EdgeAnnotationMap::iterator iter = _annotationToEdges.find(e.getAnnotation());
  ROSE_ASSERT(iter != _annotationToEdges.end()); // every annotation has to come from at least one CFG
  boost::unordered_map<int, std::list<Edge> > edgesByCfgId = iter->second;
  for (boost::unordered_map<int, std::list<Edge> >::iterator i=edgesByCfgId.begin(); i!=edgesByCfgId.end(); i++) {
    if (_cfgIdToStateIndex.find(i->first) == _cfgIdToStateIndex.end()) {
      return false;
    }
  }
  return true;
}

bool ParProAnalyzer::feasibleAccordingToGlobalState(Edge e, const ParProEState* eState) {
  if (e.getAnnotation() == _startTransitionAnnotation) {
    return true; // unsynchronized transitions within the current STG computation are always precise
  }
  ParProLabel sourceLabel = eState->getLabel();
  EdgeAnnotationMap::iterator iter = _annotationToEdges.find(e.getAnnotation());
  ROSE_ASSERT(iter != _annotationToEdges.end()); // every annotation has to come from at least one CFG
  boost::unordered_map<int, std::list<Edge> > edgesByCfgId = iter->second;
  for (boost::unordered_map<int, std::list<Edge> >::iterator i=edgesByCfgId.begin(); i!=edgesByCfgId.end(); ++i) {
    bool foundAnEdgeInCfg = false;
    for (list<Edge>::iterator k=i->second.begin(); k!=i->second.end(); k++) {
      if (sourceLabel[_cfgIdToStateIndex[i->first]] == k->source()) {
	foundAnEdgeInCfg = true;
	break;
      }
    }
    if (!foundAnEdgeInCfg) {
      return false;
    } 
  }
  return true;
}

ParProEState ParProAnalyzer::transfer(const ParProEState* eState, Edge e) {
  ParProLabel targetLabel = eState->getLabel();
  EdgeAnnotationMap::iterator iter = _annotationToEdges.find(e.getAnnotation());
  ROSE_ASSERT(iter != _annotationToEdges.end()); // every annotation has to come from at least one CFG
  boost::unordered_map<int, std::list<Edge> > edgesByCfgId = iter->second;
  for (boost::unordered_map<int, std::list<Edge> >::iterator i=edgesByCfgId.begin(); i!=edgesByCfgId.end(); ++i) {
    // only follow transitions in those automata for which the state is actually stored (and therefore not abstracted)
    // (a synchronized transition might involve several automata, some of which may be ignored (abstraction))
    if (_cfgIdToStateIndex.find(i->first) != _cfgIdToStateIndex.end()) {
      for (list<Edge>::iterator k=i->second.begin(); k!=i->second.end(); k++) {
	if (targetLabel[_cfgIdToStateIndex[i->first]] == k->source()) {
	  targetLabel[_cfgIdToStateIndex[i->first]] = k->target();
	  break;
	}
      }
    }
  }
  return ParProEState(targetLabel);
}

void ParProAnalyzer::addToWorkList(const ParProEState* estate) { 
#pragma omp critical(ESTATEWL)
  {
    if(!estate) {
      cerr<<"INTERNAL ERROR: null pointer added to work list."<<endl;
      exit(1);
    }
    worklist.push_back(estate);
  }
}

const ParProEState* ParProAnalyzer::popWorkList() {
  const ParProEState* estate = NULL;
  #pragma omp critical(ESTATEWL)
  {
    if(!worklist.empty())
      estate = *worklist.begin();
    if(estate) {
      worklist.pop_front();
    }
  }
  return estate;
}

bool ParProAnalyzer::isEmptyWorkList() { 
  bool res;
#pragma omp critical(ESTATEWL)
  {
    res=worklist.empty();
  }
  return res;
}

bool ParProAnalyzer::isPrecise() { return true; }

bool ParProAnalyzer::isIncompleteStgReady() { return false; }

 void ParProAnalyzer::set_finished(vector<bool>& v, bool val) {
  ROSE_ASSERT(v.size()>0);
  for(vector<bool>::iterator i=v.begin();i!=v.end();++i) {
    *i=val;
  }
}

bool ParProAnalyzer::all_false(vector<bool>& v) {
  ROSE_ASSERT(v.size()>0);
  bool res=false;
#pragma omp critical
  {
  for(vector<bool>::iterator i=v.begin();i!=v.end();++i) {
    res=res||(*i);
  }
  }
  return !res;
}
