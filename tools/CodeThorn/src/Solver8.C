#include "sage3basic.h"
#include "Solver8.h"
#include "IOAnalyzer.h"

using namespace CodeThorn;
using namespace std;
using namespace Sawyer::Message;

Sawyer::Message::Facility Solver8::logger;
// initialize static member flag
bool Solver8::_diagnosticsInitialized = false;

Solver8::Solver8() {
  initDiagnostics();
}

int Solver8::getId() {
  return 8;
}

/*! 
 * \author Marc Jasper
 * \date 2014, 2015.
 */
void Solver8::run() {
  while(!_analyzer->isEmptyWorkList()) {
    EStatePtr currentEStatePtr;
    //solver 8
    ROSE_ASSERT(_analyzer->estateWorkListCurrent->size() == 1);
    if (!_analyzer->isEmptyWorkList()) {
      currentEStatePtr=_analyzer->popWorkList();
    } else {
      ROSE_ASSERT(0); // there should always be exactly one element in the worklist at this point
    }
    ROSE_ASSERT(currentEStatePtr);

    Flow edgeSet=_analyzer->getFlow()->outEdges(currentEStatePtr->label());
    for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
      Edge e=*i;
      list<EStatePtr> newEStateList;
      newEStateList=_analyzer->transferEdgeEState(e,currentEStatePtr);
      // solver 8: keep track of the input state where the input sequence ran out of elements (where solver8 stops)
      if (newEStateList.size()== 0) {
        if(e.isType(EDGE_EXTERNAL)) {
          SgNode* nextNodeToAnalyze1=_analyzer->getCFAnalyzer()->getNode(e.source());
          InputOutput newio;
	  CTIOLabeler* ctioLabeler=_analyzer->getIOLabeler();
	  ROSE_ASSERT(ctioLabeler);
          Label lab=ctioLabeler->getLabel(nextNodeToAnalyze1);
          VariableId varId;
          if(ctioLabeler->isStdInLabel(lab,&varId)) {
            _analyzer->_estateBeforeMissingInput = currentEStatePtr; //store the state where input was missing in member variable
          }
        }
      }
      // solver 8: only single traces allowed
      ROSE_ASSERT(newEStateList.size()<=1);
      for(list<EStatePtr>::iterator nesListIter=newEStateList.begin();
          nesListIter!=newEStateList.end();
          ++nesListIter) {
        // newEstate is passed by value (not created yet)
        EStatePtr newEStatePtr0=*nesListIter;
        ROSE_ASSERT(newEStatePtr0->label()!=Labeler::NO_LABEL);
        if((!_analyzer->isFailedAssertEState(newEStatePtr0))) {
          HSetMaintainer<EState,EStateHashFun,EStateEqualToPred>::ProcessingResult pres=_analyzer->process(newEStatePtr0);
          EStatePtr newEStatePtr=const_cast<EStatePtr>(pres.second);
          // maintain the most recent output state. It can be connected with _estateBeforeMissingInput to facilitate
          // further tracing of an STG that is reduced to input/output/error states.
          if (newEStatePtr->io.isStdOutIO()) {
            _analyzer->_latestOutputEState = newEStatePtr;
          }
          if (true)//simply continue analysing until the input sequence runs out
            _analyzer->addToWorkList(newEStatePtr);
          _analyzer->recordTransition(currentEStatePtr,e,newEStatePtr);
        }
        if((_analyzer->isFailedAssertEState(newEStatePtr0))) {
          // failed-assert end-state: do not add to work list but do add it to the transition graph
          EStatePtr newEStatePtr;
          newEStatePtr=_analyzer->processNewOrExisting(newEStatePtr0);
          _analyzer->_latestErrorEState = newEStatePtr;
          _analyzer->recordTransition(currentEStatePtr,e,newEStatePtr);
        }
      }  // all successor states of transfer function
    } // all outgoing edges in CFG
  } // while worklist is not empty
  //the result of the analysis is just a concrete trace on the original program
  _analyzer->transitionGraph.setIsPrecise(true);
  _analyzer->transitionGraph.setIsComplete(false);
}

void Solver8::initDiagnostics() {
  if (!_diagnosticsInitialized) {
    _diagnosticsInitialized = true;
    Solver::initDiagnostics(logger, getId());
  }
}
