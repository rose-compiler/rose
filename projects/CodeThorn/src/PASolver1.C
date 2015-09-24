#ifndef DFSOLVER1_C
#define DFSOLVER1_C

#include "sage3basic.h"

#include "PASolver1.h"

SPRAY::PASolver1::PASolver1(WorkListSeq<Edge>& workList,
				  vector<Lattice*>& analyzerDataPreInfo,
				  vector<Lattice*>& analyzerDataPostInfo,
				  PropertyStateFactory& initialElementFactory,
				  Flow& flow,
				  DFTransferFunctions& transferFunctions
				  ):
  _workList(workList),
  _analyzerDataPreInfo(analyzerDataPreInfo),
  _analyzerDataPostInfo(analyzerDataPostInfo),
  _initialElementFactory(initialElementFactory),
  _flow(flow),
  _transferFunctions(transferFunctions),
  _trace(false)
{
}

void
SPRAY::PASolver1::computeCombinedPreInfo(Label lab,Lattice& info) {
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
SPRAY::PASolver1::computePostInfo(Label lab,Lattice& info) {
  _transferFunctions.transfer(lab,info);
}

// runs until worklist is empty
void
SPRAY::PASolver1::runSolver() {
  cout<<"INFO: solver 1 started."<<endl;
  ROSE_ASSERT(!_workList.isEmpty());
  while(!_workList.isEmpty()) {
    Edge edge=_workList.take();
    Label lab0=edge.source;
    Label lab1=edge.target;
    //if(_trace)
    //  cout<<"TRACE: computing edge "<<lab0<<"->"<<lab1<<endl;
    Lattice* info=_initialElementFactory.create();
    ROSE_ASSERT(info);
    info->combine(*_analyzerDataPreInfo[lab0.getId()]);
    if(info->isBot()) {
      if(_trace) {
        cout<<"TRACE: computing transfer function: "<<lab0<<":";info->toStream(cout,0);
        cout<<"-> cancel (because of bot)";
        cout<<endl;
      }
      // do nothing (non-reachable code)
    } else {
      if(_trace) {
        cout<<"TRACE: computing transfer function: "<<lab0<<":";info->toStream(cout,0);
        cout<<"->"<<lab1<<":";_analyzerDataPreInfo[lab1.getId()]->toStream(cout,0);
        cout<<endl;
      }
      _transferFunctions.transfer(edge,*info);
      ROSE_ASSERT(info);
      if(_trace) {
        cout<<"TRACE: transfer function result: "<<lab1<<":";
        ROSE_ASSERT(info);
        info->toStream(cout,0);
        cout<<endl;
      }
      
      bool isApproximatedBy=info->approximatedBy(*_analyzerDataPreInfo[lab1.getId()]);
      if(!isApproximatedBy) {
        if(_trace) {
          cout<<"TRACE: old df value : "<<lab1<<":";_analyzerDataPreInfo[lab1.getId()]->toStream(cout,0);
          cout<<endl;
        }
        if(_trace) {
          cout<<"TRACE: combined with: "<<lab1<<":";info->toStream(cout,0);
          cout<<endl;
        }

        _analyzerDataPreInfo[lab1.getId()]->combine(*info);
        
        if(_trace) {
          cout<<"TRACE: new df value : "<<lab1<<":";_analyzerDataPreInfo[lab1.getId()]->toStream(cout,0);
          cout<<endl;
        }
        
        Flow outEdges=_flow.outEdges(lab1);
        _workList.add(outEdges);
        if(_trace)
          cout<<"TRACE: adding to worklist: "<<outEdges.toString()<<endl;
      } else {
        // no new information was computed. Nothing to do.
        if(_trace)
          cout<<"TRACE: nop."<<endl;
      }
    }
    delete info;
  }
  cout<<"INFO: solver 1 finished."<<endl;
}

#endif
