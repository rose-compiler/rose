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
PASolver1::computePreInfo(Label lab,Lattice& inInfo) {
  LabelSet pred=_flow.pred(lab);
  for(LabelSet::iterator i=pred.begin();i!=pred.end();++i) {
    inInfo.combine(*_analyzerDataPostInfo[(*i).getId()]);
  }
}

// runs until worklist is empty
void
PASolver1::runSolver() {
  cout<<"INFO: solver 1 started."<<endl;
  ROSE_ASSERT(!_workList.isEmpty());
  while(!_workList.isEmpty()) {
    cout<<"INFO: worklist size: "<<_workList.size()<<endl;
    Edge edge=_workList.take();
    Label lab0=edge.source;
    Label lab1=edge.target;
    Lattice* info=_initialElementFactory.create();
    //computePreInfo(lab0,*info);
    cout<<"DEBUG: P1: "<<_analyzerDataPreInfo[lab0.getId()]->toString()<<endl;
    info->combine(*_analyzerDataPreInfo[lab0.getId()]);
    cout<<"DEBUG: P2: "<<info->toString()<<endl;
    _transferFunctions.transfer(lab0,*info);
    cout<<"DEBUG: NewInfo: "<<"@"<<lab0<<": "<<info->toString()<<endl;

    bool isApproximatedBy=info->approximatedBy(*_analyzerDataPreInfo[lab1.getId()]);
    cout<<"DEBUG: TEST APPROX: "<<info->toString()<<cout<<","<<_analyzerDataPreInfo[lab1.getId()]->toString()<<": "<<isApproximatedBy<<endl;

    if(!isApproximatedBy) {
      cout<<"DEBUG: TEST APPROX: combining ..."<<endl;
      _analyzerDataPreInfo[lab1.getId()]->combine(*info);
      cout<<"DEBUG: new combined info: "<<_analyzerDataPreInfo[lab1.getId()]->toString()<<cout<<endl;
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
