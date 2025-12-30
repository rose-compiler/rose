#include "sage3basic.h"
#include "CSDFAnalysisBase.h"
#include "AstUtility.h"
#include "ExtractFunctionArguments.h"
#include "FunctionNormalization.h"
#include "DFSolver1.h"


using namespace std;

namespace CodeThorn
{

  CSDFAnalysisBase::CSDFAnalysisBase()
  {
    // all data member initializers are specified in class definition
  }

  CSDFAnalysisBase::~CSDFAnalysisBase() {
    if(_pointerAnalysisEmptyImplementation)
      delete _pointerAnalysisEmptyImplementation;
    if(_programAbstractionLayer && _programAbstractionLayerOwner)
      delete _programAbstractionLayer;
  }

  void CSDFAnalysisBase::initializeSolver() {
    ROSE_ASSERT(getInitialElementFactory());
    ROSE_ASSERT(getFlow());

    _solver = new DFSolver1( _workList,
                             _analyzerDataPreInfo,
                             _analyzerDataPostInfo,
                             *getInitialElementFactory(),
                             *getFlow(),
                             *_transferFunctions
                             );

    ROSE_ASSERT(_solver);
  }

  void CSDFAnalysisBase::setPreInfo(Label, Context, Lattice*) {
  }

  Lattice* CSDFAnalysisBase::getPreInfo(Label lab, Context) {
    return _analyzerDataPreInfo.at(lab.getId());
  }

  Lattice* CSDFAnalysisBase::getPostInfo(Label lab, Context) {
    return _analyzerDataPostInfo[lab.getId()];
  }

  void CSDFAnalysisBase::setPostInfo(Label lab, Context context, Lattice* el) {
    if(getPostInfo(lab.getId(),context)) {
      delete _analyzerDataPostInfo[lab.getId()];
    }
    _analyzerDataPostInfo[lab.getId()]=el;
  }

  void CSDFAnalysisBase::computeAllPreInfo() {
    if(!_preInfoIsValid) {
      _solver->runSolver();
      _preInfoIsValid=true;
      _postInfoIsValid=false;
    }
  }

  void CSDFAnalysisBase::computeAllPostInfo() {
    if(!_postInfoIsValid) {
      computeAllPreInfo();
      // compute set of used labels in ICFG.
      for(Labeler::iterator i=getLabeler()->begin();i!=getLabeler()->end();++i) {
        Label lab=*i;
        Lattice* info=getInitialElementFactory()->create();
        _solver->computeCombinedPreInfo(lab,*info);
        // TODO: invoke edge-based transfer function for each edge and
        // (i) combine results or (ii) provide set of results (one
        // result per edge)
        _transferFunctions->transfer(lab,*info);
        Context context;
        setPostInfo(lab.getId(),context,info);
      }
      _postInfoIsValid=true;
    }
  }

  void
  CSDFAnalysisBase::initializeAnalyzerDataInfo() {
    Labeler*              labeler = getLabeler();
    PropertyStateFactory* factory = getInitialElementFactory();
    ROSE_ASSERT(factory && labeler);
    const size_t          numLabels = labeler->numberOfLabels();

    _analyzerDataPreInfo.reserve(numLabels);
    _analyzerDataPostInfo.reserve(numLabels);
    for(size_t l=0;l<numLabels;++l) {
      Lattice* le1=factory->create();
      _analyzerDataPreInfo.push_back(le1);
      Lattice* le2=factory->create();
      _analyzerDataPostInfo.push_back(le2);
    }
  }

  void
  CSDFAnalysisBase::initialize(CodeThornOptions& ctOpt, SgProject* root, ProgramAbstractionLayer* programAbstractionLayer) {
    //cout << "INIT: establishing program abstraction layer." << endl;
    if(programAbstractionLayer) {
      ROSE_ASSERT(_programAbstractionLayer==nullptr);
      _programAbstractionLayer=programAbstractionLayer;
      _programAbstractionLayerOwner=false;
    } else {
      _programAbstractionLayer=new ProgramAbstractionLayer();
      _programAbstractionLayerOwner=true;
      _programAbstractionLayer->initialize(ctOpt,root);
    }
    _pointerAnalysisEmptyImplementation=new PointerAnalysisEmptyImplementation(getVariableIdMapping());
    _pointerAnalysisEmptyImplementation->initialize();
    _pointerAnalysisEmptyImplementation->run();

    //cout << "STATUS: initializing solver."<<endl;
    initializeSolver();
    //cout << "STATUS: initializing monotone data flow analyzer data for "<<_analyzerDataPreInfo.size()<< " labels."<<endl;
    //cout << "INIT: initializing pre/post property states."<<endl;
    initializeAnalyzerDataInfo();
  }

  WorkListSeq<Edge>* CSDFAnalysisBase::getWorkList() {
    return &_workList;
  }

  // runs until worklist is empty
  void
  CSDFAnalysisBase::run() {
    ROSE_ASSERT(_globalVariablesState);
    // initialize work list with extremal labels
    cerr << "INFO: " << &_extremalLabels << " " << _extremalLabels.size() << std::endl;
    for(set<Label>::iterator i=_extremalLabels.begin();i!=_extremalLabels.end();++i) {
      ROSE_ASSERT(_analyzerDataPreInfo[(*i).getId()]!=0);
      cerr << "INFO: extremal-label-id = " << i->getId() << std::endl;
      initializeExtremalValue(_analyzerDataPreInfo[(*i).getId()]);
      // combine extremal value with global variables initialization state (computed by initializeGlobalVariables)
      _analyzerDataPreInfo[(*i).getId()]->combine(*_globalVariablesState);
      //cout<<"INFO: Initialized "<<*i<<" with ";
      //cout<<_analyzerDataPreInfo[(*i).getId()]->toString(getVariableIdMapping());
      //cout<<endl;
      // schroder3 (2016-08-16): Topological sorted CFG as worklist initialization is currently
      //  not supported for backward analyses. Add the extremal label's outgoing edges instead.
      if(_no_topological_sort || !isForwardAnalysis()) {
        Flow outEdges=getFlow()->outEdges(*i);
        for(Flow::iterator j=outEdges.begin();j!=outEdges.end();++j) {
          _workList.add(*j);
        }
      }
#if 0
      LabelSet initsucc=getFlow->succ(*i);
      for(LabelSet::iterator i=initsucc.begin();i!=initsucc.end();++i) {
        _workList.add(*i);
      }
#endif
    }

    // schroder3 (2016-08-16): Use the topological sorted CFG as worklist initialization. This avoids
    //  unnecessary computations that might occur (e.g. if the if-branch and else-branch
    //  do not have an equivalent number of nodes).
    if(!_no_topological_sort && isForwardAnalysis()) {
      if(_extremalLabels.size() == 1) {
        Label startLabel = *(_extremalLabels.begin());
        std::list<Edge> topologicalEdgeList = getFlow()->getTopologicalSortedEdgeList(startLabel);
        cout << "INFO: Using topologically sorted CFG as work list initialization." << endl;
        for(std::list<Edge>::const_iterator i = topologicalEdgeList.begin(); i != topologicalEdgeList.end(); ++i) {
          //cout << (*i).toString() << endl;
          _workList.add(*i);
        }
      } else {
        cout << "INFO: Using non-topologically sorted CFG with multiple function entries as work list initialization." << endl;
        for(set<Label>::iterator i=_extremalLabels.begin();i!=_extremalLabels.end();++i) {
          Flow outEdges=getFlow()->outEdges(*i);
          for(Flow::iterator i=outEdges.begin();i!=outEdges.end();++i) {
            _workList.add(*i);
          }
        }
      }
    }
    cout<<"INFO: work list size after initialization: "<<_workList.size()<<endl;
    solve();
  }

  // runs until worklist is empty
  void
  CSDFAnalysisBase::solve() {
    computeAllPreInfo();
    computeAllPostInfo();
  }

}
