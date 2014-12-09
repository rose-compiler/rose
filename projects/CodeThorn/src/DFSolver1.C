#ifndef DFSOLVER1_C
#define DFSOLVER1_C

#include "sage3basic.h"

#include "DFSolver1.h"

template<typename LatticeType>
DFSolver1<LatticeType>::DFSolver1(WorkListSeq<Label>& workList,
				  vector<LatticeType>& analyzerDataPreInfo,
				  vector<LatticeType>& analyzerDataPostInfo,
				  LatticeType& initialElement,
				  Flow& flow,
				  DFTransferFunctions<LatticeType>& transferFunctions
				  ):
  _workList(workList),
  _analyzerDataPreInfo(analyzerDataPreInfo),
  _analyzerDataPostInfo(analyzerDataPostInfo),
  _initialElement(initialElement),
  _flow(flow),
  _transferFunctions(transferFunctions)
{
}

template<typename LatticeType>
void
DFSolver1<LatticeType>::computePreInfo(Label lab,LatticeType& inInfo) {
  LabelSet pred=_flow.pred(lab);
  for(LabelSet::iterator i=pred.begin();i!=pred.end();++i) {
    inInfo.combine(_analyzerDataPostInfo[(*i).getId()]);
  }
}

// runs until worklist is empty
template<typename LatticeType>
void
DFSolver1<LatticeType>::runSolver() {
  cout<<"INFO: solver (label-out-algorithm1) started."<<endl;
  ROSE_ASSERT(!_workList.isEmpty());
  while(!_workList.isEmpty()) {
    Label lab=_workList.take();
    //cout<<"INFO: worklist size: "<<_workList.size()<<endl;
    //_analyzerData[lab]=_analyzerData comb transfer(lab,combined(Pred));
    LatticeType inInfo;
    computePreInfo(lab,inInfo);
    
    LatticeType newInfo=_transferFunctions.transfer(lab,inInfo);
    //cout<<"NewInfo: ";newInfo.toStream(cout);cout<<endl;
    if(!newInfo.approximatedBy(_analyzerDataPostInfo[lab.getId()])) {
      _analyzerDataPostInfo[lab.getId()].combine(newInfo);
      LabelSet succ;
      succ=_flow.succ(lab);
      _workList.add(succ);
    } else {
      // no new information was computed. Nothing to do.
    }
  }
  cout<<"INFO: solver (label-out-algorithm1) finished."<<endl;
}

#endif
