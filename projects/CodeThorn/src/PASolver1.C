#ifndef DFSOLVER1_C
#define DFSOLVER1_C

#include "sage3basic.h"

#include "PASolver1.h"

PASolver1::PASolver1(WorkListSeq<Edge>& workList,
				  vector<Lattice*>& analyzerDataPreInfo,
				  vector<Lattice*>& analyzerDataPostInfo,
				  PropertyStateFactory& initialElementFactory,
				  Flow& flow,
				  PATransferFunctions& transferFunctions
				  ):
  _workList(workList),
  _analyzerDataPreInfo(analyzerDataPreInfo),
  _analyzerDataPostInfo(analyzerDataPostInfo),
  _initialElementFactory(initialElementFactory),
  _flow(flow),
  _transferFunctions(transferFunctions)
{
}

void
PASolver1::computeCombinedPreInfo(Label lab,Lattice& info) {
  LabelSet pred=_flow.pred(lab);
  for(LabelSet::iterator i=pred.begin();i!=pred.end();++i) {
    Lattice* predInfo=_initialElementFactory.create();
    predInfo->combine(*_analyzerDataPreInfo[(*i).getId()]); // clone info of predecessor
    computePostInfo(*i,*predInfo);
    info.combine(*predInfo);
    delete predInfo;
  }
}

void
PASolver1::computePostInfo(Label lab,Lattice& info) {
  _transferFunctions.transfer(lab,info);
}

// runs until worklist is empty
void
PASolver1::runSolver() {
  cout<<"INFO: solver 1 started."<<endl;
  ROSE_ASSERT(!_workList.isEmpty());
  while(!_workList.isEmpty()) {
    Edge edge=_workList.take();
    Label lab0=edge.source;
    Label lab1=edge.target;
    Lattice* info=_initialElementFactory.create();
#if 0
    info->combine(*_analyzerDataPreInfo[lab0.getId()]);
#else
    computeCombinedPreInfo(lab0,*info);
#endif
    //_transferFunctions.transfer(lab0,*info);
    bool isApproximatedBy=info->approximatedBy(*_analyzerDataPreInfo[lab1.getId()]);
    if(!isApproximatedBy) {
      _analyzerDataPreInfo[lab1.getId()]->combine(*info);
      Flow outEdges=_flow.outEdges(lab1);
      _workList.add(outEdges);
    } else {
      // no new information was computed. Nothing to do.
    }
    delete info;
  }
  cout<<"INFO: solver 1 finished."<<endl;
}

#endif
