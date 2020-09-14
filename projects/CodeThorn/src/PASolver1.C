#ifndef DFSOLVER1_C
#define DFSOLVER1_C

#include "sage3basic.h"

#include "PASolver1.h"
#include "TimeMeasurement.h"

using namespace std;

CodeThorn::PASolver1::PASolver1(WorkListSeq<Edge>& workList,
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
CodeThorn::PASolver1::computeCombinedPreInfo(Label lab,Lattice& info) {
  if(!_flow.contains(lab)) {
    // schroder3 (2016-07-07): If the label does not exist in the CFG, then
    //  it does not have predecessors and the given pre-info therefore does
    //  not change. This check is necessary if Flow::pred(Label) uses the
    //  Sawyer graph as underlying datastructure because Flow::pred then
    //  expects that the given label exists in the CFG.
    return;
  }
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
CodeThorn::PASolver1::computePostInfo(Label lab,Lattice& info) {
  _transferFunctions.transfer(lab,info);
}

// runs until worklist is empty
void
CodeThorn::PASolver1::runSolver() {
  constexpr uint64_t REPORT_INTERVAL = (1 << 12);
  
  TimeMeasurement solverTimer;
  uint64_t        nodeCounter = 0;
  double          splitTime   = 0;
  
  cout << "INFO: solver 1 started."<<endl;
  solverTimer.start();
  //ROSE_ASSERT(!_workList.isEmpty()); empty files (programs of zero length)
  while(!_workList.isEmpty()) {
    Edge edge=_workList.take();
    Label lab0=edge.source();
    Label lab1=edge.target();

#if OBSOLETE_CODE
    // schroder3 (2016-08-05): Set up the combine and approximatedBy member functions according
    //  to the edge type.
    void(Lattice::*combineMemFunc)(Lattice&);
    bool(Lattice::*approximatedByMemFunc)(Lattice&) const;
    if(edge.isType(EDGE_BACKWARD)) {
      combineMemFunc = &Lattice::combineAsymmetric;
      approximatedByMemFunc = &Lattice::approximatedByAsymmetric;
      if(_trace) {
        cout << "TRACE: BACKWARD edge" << endl;
      }
    }
    else {
      combineMemFunc = &Lattice::combine;
      approximatedByMemFunc = &Lattice::approximatedBy;
    }
#endif /* OBSOLETE_CODE */

    if(_trace)
    {
      cout<<"TRACE: computing edge "<<lab0<<"->"<<lab1<<endl;
      cout<<"  from: " << getLabeler().getNode(lab0)->unparseToString() 
          <<"    to: " << getLabeler().getNode(lab1)->unparseToString()
	  <<std::endl;
    }  
    Lattice* info=_initialElementFactory.create();
    ROSE_ASSERT(info);
    info->combine(*_analyzerDataPreInfo[lab0.getId()]);
    if(info->isBot()) {
      if(_trace) {
        cout<<"TRACE: computing transfer function: "<<lab0<<":";info->toStream(cout,0);
        cout<<" ==> cancel (because of bot)";
        cout<<endl;
      }
      // do nothing (non-reachable code)
    } else {
      if(_trace) {
        cout<<"TRACE: computing transfer function: "<<lab0<<":";info->toStream(cout,0);
        cout<<" ==> "<<lab1<<":";_analyzerDataPreInfo[lab1.getId()]->toStream(cout,0);
        cout<<endl;
      }
      _transferFunctions.transfer(edge,*info);
      if(_trace) {
        cout<<"TRACE: transfer function result: "<<lab1<<":";
        ROSE_ASSERT(info);
        info->toStream(cout,0);
        cout<<endl;
      }

      // schroder3 (2016-08-05): Check whether the combine below will change something.
      //~ bool isApproximatedBy=(info->*approximatedByMemFunc)(*_analyzerDataPreInfo[lab1.getId()]);
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
        for (Flow::iterator i=outEdges.begin(); i!=outEdges.end(); ++i) {
          _workList.add(*i);
        }
        if(_trace)
          cout<<"TRACE: adding to worklist: "<<outEdges.toString()<<endl;
      } else {
        // no new information was computed. Nothing to do.
        if(_trace)
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
