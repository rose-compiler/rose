// Author: Marc Jasper, 2016.

#include "ParProAnalyzer.h"
#include "StateRepresentations.h"

using namespace SPRAY;
using namespace CodeThorn;
using namespace std;

void ParProAnalyzer::initializeSolver() {
  // generate the initial global EState of the parallel program
  ParProLabel startLabel = ParProLabel(_cfgs.size());
  for (unsigned int i=0; i<_cfgs.size(); i++) {
    startLabel[i] = _cfgs[i].getStartLabel();
  }
  ParProEState startState(startLabel);
  // add the start state to the set of known states
  const ParProEState* startStatePtr= _eStateSet.processNewOrExisting(startState);
  // add the start state to the worklist
  worklist.push_back(startStatePtr);
}

void ParProAnalyzer::runSolver() {
  int threadNum;
  int workers=_numberOfThreadsToUse;
  vector<bool> workVector(_numberOfThreadsToUse);
  set_finished(workVector,true);
  bool terminateEarly=false;
  omp_set_num_threads(workers);

  cout <<"STATUS: Running solver p1 for parallel programs with "<<workers<<" threads."<<endl;
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
	list<ParProEState> newEStateList = parProTransferFunction(currentEStatePtr);
	for (list<ParProEState>::iterator i=newEStateList.begin(); i!=newEStateList.end(); i++) {
	  ParProEStateSet::ProcessingResult pres = _eStateSet.process(*i);
          const ParProEState* newEStatePtr = pres.second;
	  if (pres.first == true) {
	    addToWorkList(newEStatePtr);
	  }
	  // TODO: record transition in the STG here
	}
      } // conditional: test if work is available
    } // while
  } // omp parallel
  if(isIncompleteStgReady()) {
    cout << "STATUS: analysis finished (incomplete STG due to specified resource restriction)."<<endl;
    _transitionGraph.setIsComplete(false);
  } else {
    _transitionGraph.setIsComplete(true);
    cout << "analysis finished (worklist is empty)."<<endl;
  }
  _transitionGraph.setIsPrecise(isPrecise());

  // TODO: remove this temporary test
  cout << "DEBUG: _eStateSet size: " << _eStateSet.size() << endl;
  cout << "DEBUG: _eStateSet elements: " << endl;
  for (ParProEStateSet::iterator i=_eStateSet.begin(); i!=_eStateSet.end(); i++) {
    cout << "DEBUG: " << (*i)->toString() << endl;
  }
}

list<ParProEState> ParProAnalyzer::parProTransferFunction(const ParProEState* source) {
  list<ParProEState> result;
  ParProLabel sourceLabel = source->getLabel();
  // compute successor EStates based on the out edges of every CFG (one per parallel component)
  //  for (ParProLabel::iterator i=sourceLabel.begin(); i!=sourceLabel.end(); i++) {
  ROSE_ASSERT(_cfgs.size() == sourceLabel.size());
  for (unsigned int i=0; i<_cfgs.size(); i++) {
    Flow outEdges = _cfgs[i].outEdges(sourceLabel[i]);
    for(Flow::iterator k=outEdges.begin(); k!=outEdges.end(); k++) { 
      Edge e=*k;
      if (feasibleAccordingToGlobalState(e, source)) {
	ParProEState target = transfer(source, e, i);
	result.push_back(target);
      }
    } // for each outgoing CFG edge of a particular parallel component's current label
  } // for each parallel component of the analyzed system
  return result;
}

// TODO: implement evaluation based on communication/synchronization between parallel components
bool ParProAnalyzer::feasibleAccordingToGlobalState(Edge e, const ParProEState*) { return true; }

// TODO: implement semantics of CFG edges
ParProEState ParProAnalyzer::transfer(const ParProEState* eState, Edge e, unsigned int cfgId) {
  ParProLabel targetLabel = eState->getLabel();
  targetLabel[cfgId] = e.target;
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
