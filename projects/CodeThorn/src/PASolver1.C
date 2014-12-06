#ifndef DFSOLVER1_C
#define DFSOLVER1_C

#include "sage3basic.h"

#include "PASolver1.h"

PASolver1::PASolver1(WorkListSeq<Label>& workList,
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
    inInfo.combine(*_analyzerDataPostInfo[*i]);
  }
}

// runs until worklist is empty
void
PASolver1::runSolver() {
  cout<<"INFO: solver (label-out-algorithm1) started."<<endl;
  ROSE_ASSERT(!_workList.isEmpty());
  while(!_workList.isEmpty()) {
    Label lab=_workList.take();
    //cout<<"INFO: worklist size: "<<_workList.size()<<endl;
    //_analyzerData[lab]=_analyzerData comb transfer(lab,combined(Pred));
    // TODO: std::auto_ptr<Lattice*>/std::unique_ptr<Lattice*>
    Lattice* info=_initialElementFactory.create();
    computePreInfo(lab,*info);
    
    _transferFunctions.transfer(lab,*info);
    //cout<<"NewInfo: ";newInfo.toStream(cout);cout<<endl;
    if(!info->approximatedBy(*_analyzerDataPostInfo[lab])) {
      _analyzerDataPostInfo[lab]->combine(*info);
      LabelSet succ;
      succ=_flow.succ(lab);
      _workList.add(succ);
    } else {
      // no new information was computed. Nothing to do.
    }
    delete info;
  }
  cout<<"INFO: solver (label-out-algorithm1) finished."<<endl;
}

#endif
