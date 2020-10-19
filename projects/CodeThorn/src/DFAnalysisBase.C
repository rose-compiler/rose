/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include "sage3basic.h"
#include "DFAnalysisBase.h"
#include "AstUtility.h"
#include "ExtractFunctionArguments.h"
#include "FunctionNormalization.h"
#include "PASolver1.h" 


using namespace std;

namespace CodeThorn 
{

  DFAnalysisBase::DFAnalysisBase()
  {
    // all data member initializers are specified in class definition
  }

  DFAnalysisBase::~DFAnalysisBase() {
    if(_pointerAnalysisEmptyImplementation)
      delete _pointerAnalysisEmptyImplementation;
    if(_programAbstractionLayer && _programAbstractionLayerOwner)
      delete _programAbstractionLayer;
  }

  void DFAnalysisBase::initializeSolver() {
    ROSE_ASSERT(&_workList);
    ROSE_ASSERT(getInitialElementFactory());
    ROSE_ASSERT(&_analyzerDataPreInfo);
    ROSE_ASSERT(&_analyzerDataPostInfo);
    ROSE_ASSERT(getFlow());
    ROSE_ASSERT(&_transferFunctions);
     
    _solver = new PASolver1( _workList,
                             _analyzerDataPreInfo,
                             _analyzerDataPostInfo,
                             *getInitialElementFactory(),
                             *getFlow(),
                             *_transferFunctions
                             );
  
    ROSE_ASSERT(_solver);
  }

  Lattice* DFAnalysisBase::getPreInfo(Label lab) {
    return _analyzerDataPreInfo.at(lab.getId());
  }

  Lattice* DFAnalysisBase::getPostInfo(Label lab) {
    return _analyzerDataPostInfo[lab.getId()];
  }

  void
  DFAnalysisBase::initializeAnalyzerDataInfo() {
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
  DFAnalysisBase::initialize(SgProject* root, ProgramAbstractionLayer* programAbstractionLayer) {
    cout << "INIT: establishing program abstraction layer." << endl;
    if(programAbstractionLayer) {
      ROSE_ASSERT(_programAbstractionLayer==nullptr);
      _programAbstractionLayer=programAbstractionLayer;
      _programAbstractionLayerOwner=false;
    } else {
      _programAbstractionLayer=new ProgramAbstractionLayer();
      _programAbstractionLayerOwner=true;
      _programAbstractionLayer->initialize(root);
    }
    _pointerAnalysisEmptyImplementation=new PointerAnalysisEmptyImplementation(getVariableIdMapping());
    _pointerAnalysisEmptyImplementation->initialize();
    _pointerAnalysisEmptyImplementation->run();
    cout << "INIT: Creating CFAnalysis."<<endl;

    // PP (07/15/19) moved flow generation to ProgramAbstractionLayer
    cout << "INIT: Requesting CFG."<<endl;
    _flow = _programAbstractionLayer->getFlow(isBackwardAnalysis());

    initializeAnalyzerDataInfo();
    cout << "STATUS: initialized monotone data flow analyzer for "<<_analyzerDataPreInfo.size()<< " labels."<<endl;
    cout << "INIT: initialized pre/post property states."<<endl;
    initializeSolver();
    cout << "STATUS: initialized solver."<<endl;
  }

  DFAstAttribute* DFAnalysisBase::createDFAstAttribute(Lattice* elem) {
    // elem ignored in default function
    return new DFAstAttribute();
  }


  // runs until worklist is empty
  void
  DFAnalysisBase::run() {
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
        Flow outEdges=_flow->outEdges(*i);
        for(Flow::iterator j=outEdges.begin();j!=outEdges.end();++j) {
          _workList.add(*j);
        }
      }
#if 0
      LabelSet initsucc=_flow.succ(*i);
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
        std::list<Edge> topologicalEdgeList = _flow->getTopologicalSortedEdgeList(startLabel);
        cout << "INFO: Using topologically sorted CFG as work list initialization." << endl;
        for(std::list<Edge>::const_iterator i = topologicalEdgeList.begin(); i != topologicalEdgeList.end(); ++i) {
          //cout << (*i).toString() << endl;
          _workList.add(*i);
        }
      } else {
        cout << "INFO: Using non-topologically sorted CFG with multiple function entries as work list initialization." << endl;
        for(set<Label>::iterator i=_extremalLabels.begin();i!=_extremalLabels.end();++i) {
          Flow outEdges=_flow->outEdges(*i);
          for(Flow::iterator i=outEdges.begin();i!=outEdges.end();++i) {
            _workList.add(*i);
          }
        }
      }
    }
    cout<<"INFO: work list size after initialization: "<<_workList.size()<<endl;
    solve();
  }

#include <iostream>
#include "AstAnnotator.h"
#include <string>

  using std::string;

#include <sstream>

  /*!
   * \author Markus Schordan
   * \date 2012.
   */

  void DFAnalysisBase::attachInfoToAst(string attributeName,bool inInfo) {
    computeAllPreInfo();
    computeAllPostInfo();
    LabelSet labelSet=_flow->nodeLabels();
    for(LabelSet::iterator i=labelSet.begin();
        i!=labelSet.end();
        ++i) {
      ROSE_ASSERT(*i<_analyzerDataPreInfo.size());
      ROSE_ASSERT(*i<_analyzerDataPostInfo.size());
      // TODO: need to add a solution for nodes with multiple associated labels (e.g. function call)
      if(*i >=0 ) {
        Label lab=*i;
        SgNode* node=getLabeler()->getNode(*i);
        Lattice* info=0;
        if(inInfo) {
          if(isForwardAnalysis()) {
            info=getPreInfo(lab);
            if(getLabeler()->isSecondLabelOfMultiLabeledNode(lab)) {
              continue;
            }
          } else if(isBackwardAnalysis()) {
            if(getLabeler()->isSecondLabelOfMultiLabeledNode(lab)) {
              continue;
            }
            info=getPostInfo(lab);
          } else {
            cerr<<"Error: Ast-annotation: unsupported analysis mode."<<endl;
            exit(1);
          }
        } else {
          if(isForwardAnalysis()) {
            if(getLabeler()->isFirstLabelOfMultiLabeledNode(lab)) {
              continue;
            }
            info=getPostInfo(lab);
          } else if(isBackwardAnalysis()) {
            if(getLabeler()->isFirstLabelOfMultiLabeledNode(lab)) {
              continue;
            }
            info=getPreInfo(lab);
          } else {
            cerr<<"Error: Ast-annotation: unsupported analysis mode."<<endl;
            exit(1);
          }

        }
        ROSE_ASSERT(info!=0);
        node->setAttribute(attributeName,createDFAstAttribute(info));
      }
    }
  }

  /*!
   * \author Markus Schordan
   * \date 2012.
   */

  void DFAnalysisBase::attachInInfoToAst(string attributeName) {
    attachInfoToAst(attributeName,true);
  }

  /*!
   * \author Markus Schordan
   * \date 2012.
   */

  void DFAnalysisBase::attachOutInfoToAst(string attributeName) {
    attachInfoToAst(attributeName,false);
  }

  /*!
   * \author Markus Schordan
   * \date 2018.
   */

  void DFAnalysisBase::setSkipUnknownFunctionCalls(bool defer) {
    _skipSelectedFunctionCalls=defer;
    if(_transferFunctions) {
      _transferFunctions->setSkipUnknownFunctionCalls(defer);
    }
  }

  void DFAnalysisBase::setPostInfo(Label lab,Lattice* el) {
    if(getPostInfo(lab.getId())) {
      delete _analyzerDataPostInfo[lab.getId()];
    }
    _analyzerDataPostInfo[lab.getId()]=el;
  }
  
}

