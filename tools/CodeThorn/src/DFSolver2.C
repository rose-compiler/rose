#ifndef DFSOLVER2_C
#define DFSOLVER2_C

#include "sage3basic.h"

#include "DFSolver2.h"
#include "TimeMeasurement.h"

using namespace std;

CodeThorn::DFSolver2::DFSolver2() {
}

CodeThorn::DFSolver2::DFSolver2(DFAnalysisBase* dfAnalysisBase):
  _dfAnalysisBase(dfAnalysisBase)
{
}

void CodeThorn::DFSolver2::setDFAnalysis(DFAnalysisBase* dfAnalysisBase) {
  _dfAnalysisBase=dfAnalysisBase;
}

void
CodeThorn::DFSolver2::computeCombinedPreInfo(Label lab,Lattice& info) {
  if(!_dfAnalysisBase->getFlow()->contains(lab)) {
    // schroder3 (2016-07-07): If the label does not exist in the CFG, then
    //  it does not have predecessors and the given pre-info therefore does
    //  not change. This check is necessary if Flow::pred(Label) uses the
    //  Sawyer graph as underlying datastructure because Flow::pred then
    //  expects that the given label exists in the CFG.
    return;
  }
  LabelSet pred=_dfAnalysisBase->getFlow()->pred(lab);
  for(LabelSet::iterator i=pred.begin();i!=pred.end();++i) {
    Lattice* predInfo=_dfAnalysisBase->getInitialElementFactory()->create();
    predInfo->combine(*_dfAnalysisBase->getPreInfo(*i));
    computePostInfo(*i,*predInfo);
    info.combine(*predInfo);
    delete predInfo;
  }
}

void
CodeThorn::DFSolver2::computePostInfo(Label lab,Lattice& info) {
  _dfAnalysisBase->getTransferFunctions()->transfer(lab,info);
}

// runs until worklist is empty
void
CodeThorn::DFSolver2::runSolver() {
  constexpr uint64_t REPORT_INTERVAL = (1 << 12);
  
  TimeMeasurement solverTimer;
  uint64_t        nodeCounter = 0;
  double          splitTime   = 0;
  
  cout << "INFO: solver 1 started."<<endl;
  solverTimer.start();
  //ROSE_ASSERT(!_workList.isEmpty()); empty files (programs of zero length)
  WorkListSeq<Edge>& workListRef=*_dfAnalysisBase->getWorkList();
  while(!workListRef.isEmpty()) {
    Edge edge=workListRef.take();
    Label lab0=edge.source();
    Label lab1=edge.target();

    if(getTrace())
    {
      cout<<"TRACE: computing edge "<<lab0<<"->"<<lab1<<endl;
      cout<<"  from: " << _dfAnalysisBase->getLabeler()->getNode(lab0)->unparseToString() 
          <<"    to: " << _dfAnalysisBase->getLabeler()->getNode(lab1)->unparseToString()
	  <<std::endl;
    }  
    Lattice* info=_dfAnalysisBase->getInitialElementFactory()->create();
    ROSE_ASSERT(info);
    //info->combine(*_analyzerDataPreInfo[lab0.getId()]);
    info->combine(*_dfAnalysisBase->getPreInfo(lab0));
    if(info->isBot()) {
      if(getTrace()) {
        cout<<"TRACE: computing transfer function: "<<lab0<<":";info->toStream(cout,0);
        cout<<" ==> cancel (because of bot)";
        cout<<endl;
      }
      // do nothing (non-reachable code)
    } else {
      if(getTrace()) {
        cout<<"TRACE: computing transfer function: "<<lab0<<":";info->toStream(cout,0);
        cout<<" ==> "<<lab1<<":";_dfAnalysisBase->getPreInfo(lab1)->toStream(cout,0);
        cout<<endl;
      }
      _dfAnalysisBase->getTransferFunctions()->transfer(edge,*info);
      if(getTrace()) {
        cout<<"TRACE: transfer function result: "<<lab1<<":";
        ROSE_ASSERT(info);
        info->toStream(cout,0);
        cout<<endl;
      }

      bool isApproximatedBy=info->approximatedBy(*_dfAnalysisBase->getPreInfo(lab1));
      if(!isApproximatedBy) {
        if(getTrace()) {
          cout<<"TRACE: old df value : "<<lab1<<":";_dfAnalysisBase->getPreInfo(lab1)->toStream(cout,0);
          cout<<endl;
        }
        if(getTrace()) {
          cout<<"TRACE: combined with: "<<lab1<<":";info->toStream(cout,0);
          cout<<endl;
        }

        //_analyzerDataPreInfo[lab1.getId()]->combine(*info);
        _dfAnalysisBase->getPreInfo(lab1)->combine(*info);

        if(getTrace()) {
          cout<<"TRACE: new df value : "<<lab1<<":";_dfAnalysisBase->getPreInfo(lab1)->toStream(cout,0);
          cout<<endl;
        }

        Flow outEdges=_dfAnalysisBase->getFlow()->outEdges(lab1);
        for (Flow::iterator i=outEdges.begin(); i!=outEdges.end(); ++i) {
          workListRef.add(*i);
        }
        if(getTrace())
          cout<<"TRACE: adding to worklist: "<<outEdges.toString()<<endl;
      } else {
        // no new information was computed. Nothing to do.
        if(getTrace())
          cout<<"TRACE: noop."<<endl;
      }
    }
    delete info;
    
    if (((++nodeCounter) % REPORT_INTERVAL) == 0)
    {
      const double oldSplitTime = splitTime;
       
      splitTime = solverTimer.getTimeDurationAndKeepRunning().seconds();
      
      std::cerr << "INFO: " << static_cast<size_t>(REPORT_INTERVAL / (splitTime-oldSplitTime)) << " nodes/s - "
                << nodeCounter << '/' << splitTime << '.' 
		<< std::endl; 
    }
  }
  
  TimeDuration endTime = solverTimer.getTimeDurationAndStop();
  
  cout<<"INFO: solver 1 finished after " << static_cast<size_t>(endTime.milliSeconds()) << "ms."<<endl;
  cout<<"INFO: " << nodeCounter << " nodes analyzed (" << static_cast<size_t>(nodeCounter / endTime.seconds())
      <<" nodes/s)" << endl; 
}

#endif
