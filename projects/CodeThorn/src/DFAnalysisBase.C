/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"
#include "DFAnalysisBase.h"
#include "AnalysisAbstractionLayer.h"
#include "ExtractFunctionArguments.h"
#include "FunctionNormalization.h"
//#include "Normalization.h"

using namespace CodeThorn;
using namespace std;

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
  _solver=new CodeThorn::PASolver1(_workList,
                      _analyzerDataPreInfo,
                      _analyzerDataPostInfo,
                      *getInitialElementFactory(),
                      *getFlow(),
                      *_transferFunctions);
}

Lattice* DFAnalysisBase::getPreInfo(Label lab) {
  return _analyzerDataPreInfo[lab.getId()];
}

Lattice* DFAnalysisBase::getPostInfo(Label lab) {
  return _analyzerDataPostInfo[lab.getId()];
}

void DFAnalysisBase::computeAllPreInfo() {
  if(!_preInfoIsValid) {
    _solver->runSolver();
    _preInfoIsValid=true;
    _postInfoIsValid=false;
  }
}

void DFAnalysisBase::computeAllPostInfo() {
  if(!_postInfoIsValid) {
    computeAllPreInfo();
    cout << "INFO: computing post data."<<endl;
    // compute set of used labels in ICFG.
    for(Labeler::iterator i=getLabeler()->begin();i!=getLabeler()->end();++i) {
      Label lab=*i;
      Lattice* info=getInitialElementFactory()->create();
      _solver->computeCombinedPreInfo(lab,*info);
      // TODO: invoke edge-based transfer function for each edge and
      // (i) combine results or (ii) provide set of results (one
      // result per edge)
      _transferFunctions->transfer(lab,*info);
      if(_analyzerDataPostInfo[lab.getId()]) {
        delete _analyzerDataPostInfo[lab.getId()];
      }
      _analyzerDataPostInfo[lab.getId()]=info;
    }
    _postInfoIsValid=true;
  }
}

PropertyStateFactory*
DFAnalysisBase::getInitialElementFactory() {
  ROSE_ASSERT(_transferFunctions);
  return _transferFunctions->getInitialElementFactory();
}

void DFAnalysisBase::setInitialElementFactory(PropertyStateFactory* pf) {
  ROSE_ASSERT(_transferFunctions);
  _transferFunctions->setInitialElementFactory(pf);
}

void DFAnalysisBase::setExtremalLabels(LabelSet extremalLabels) {
  _extremalLabels=extremalLabels;
}

void DFAnalysisBase::setForwardAnalysis() {
  _analysisType=DFAnalysisBase::FORWARD_ANALYSIS;
}

void DFAnalysisBase::setBackwardAnalysis() {
  _analysisType=DFAnalysisBase::BACKWARD_ANALYSIS;
}

bool DFAnalysisBase::isForwardAnalysis() {
  return _analysisType==DFAnalysisBase::FORWARD_ANALYSIS;
}

bool DFAnalysisBase::isBackwardAnalysis() {
  return _analysisType==DFAnalysisBase::BACKWARD_ANALYSIS;
}

bool DFAnalysisBase::getNoTopologicalSort() {
  return _no_topological_sort;
}

void DFAnalysisBase::setNoTopologicalSort(bool no_topological_sort) {
  _no_topological_sort = no_topological_sort;
}

void DFAnalysisBase::initializeExtremalValue(Lattice* element) {
  ROSE_ASSERT(_transferFunctions);
  _transferFunctions->initializeExtremalValue(*element);
}

Lattice* DFAnalysisBase::initializeGlobalVariables(SgProject* root) {
  ROSE_ASSERT(_transferFunctions);
  Lattice* elem=_transferFunctions->initializeGlobalVariables(root);
  _globalVariablesState=elem;
  return elem;
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

  ROSE_ASSERT(getInitialElementFactory());
  for(long l=0;l<getLabeler()->numberOfLabels();++l) {
    Lattice* le1=getInitialElementFactory()->create();
    _analyzerDataPreInfo.push_back(le1);
    Lattice* le2=getInitialElementFactory()->create();
    _analyzerDataPostInfo.push_back(le2);
  }
  cout << "STATUS: initialized monotone data flow analyzer for "<<_analyzerDataPreInfo.size()<< " labels."<<endl;

  cout << "INIT: initialized pre/post property states."<<endl;
  initializeSolver();
  cout << "STATUS: initialized solver."<<endl;
}

void DFAnalysisBase::initializeTransferFunctions() {
  ROSE_ASSERT(_transferFunctions);
  ROSE_ASSERT(getLabeler());
  _transferFunctions->setProgramAbstractionLayer(_programAbstractionLayer);
  if(_pointerAnalysisInterface==0)
    _transferFunctions->setPointerAnalysis(_pointerAnalysisEmptyImplementation);
  else
    _transferFunctions->setPointerAnalysis(_pointerAnalysisInterface);
  _transferFunctions->addParameterPassingVariables();
}

void DFAnalysisBase::setPointerAnalysis(PointerAnalysisInterface* pa) {
  _pointerAnalysisInterface=pa;
}

CodeThorn::PointerAnalysisInterface* DFAnalysisBase::getPointerAnalysis() {
  return _pointerAnalysisInterface;
}

void
DFAnalysisBase::determineExtremalLabels(SgNode* startFunRoot,bool onlySingleStartLabel) {
  if(startFunRoot) {
    CFAnalysis* cfanalyzer = getCFAnalyzer();

    if(isForwardAnalysis()) {
      Label startLabel=cfanalyzer->getLabel(startFunRoot);
      _extremalLabels.insert(startLabel);
    } else if(isBackwardAnalysis()) {
      if(isSgFunctionDefinition(startFunRoot)) {
        Label startLabel=cfanalyzer->getLabel(startFunRoot);
        Label endLabel=cfanalyzer->correspondingFunctionExitLabel(startLabel);
        _extremalLabels.insert(endLabel);
      } else {
        cerr<<"Error: backward analysis only supported for start at function exit label."<<endl;
        exit(1);
      }
    }
  } else {
    if(!onlySingleStartLabel) {
      Labeler* labeler=getLabeler();
      long numLabels=labeler->numberOfLabels();
      // naive way of initializing all labels
      for(long i=0;i<numLabels;++i) {
        Label lab=i;
        // only add function entry labels as extremal labels
        if(isForwardAnalysis()) {
          if(labeler->isFunctionEntryLabel(i)) {
            _extremalLabels.insert(lab);
          }
        } else {
          ROSE_ASSERT(isBackwardAnalysis());
          if(labeler->isFunctionExitLabel(i)) {
            _extremalLabels.insert(lab);
          }
        }
      }
    } else {
      // keep _extremalLabels an empty set if no start function is
      // determined and only a single start label is requested.
      // _extremalLabels remains empty. Analysis will not be run.
    }
  }
  cout<<"STATUS: Number of extremal labels: "<<_extremalLabels.size()<<endl;
}



// runs until worklist is empty

void
DFAnalysisBase::solve() {
  computeAllPreInfo();
  computeAllPostInfo();
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

// default identity function

DFAnalysisBase::ResultAccess&
DFAnalysisBase::getResultAccess() {
  return _analyzerDataPreInfo;
}

#include <iostream>

#include "AstAnnotator.h"
#include <string>

using std::string;

#include <sstream>

CFAnalysis* DFAnalysisBase::getCFAnalyzer() {
  ROSE_ASSERT(_programAbstractionLayer);

  return _programAbstractionLayer->getCFAnalyzer();
}


Labeler* DFAnalysisBase::getLabeler() {
  return _programAbstractionLayer->getLabeler();
}


VariableIdMappingExtended* DFAnalysisBase::getVariableIdMapping() {
  return _programAbstractionLayer->getVariableIdMapping();
}

FunctionIdMapping* DFAnalysisBase::getFunctionIdMapping() {
  return _programAbstractionLayer->getFunctionIdMapping();
}

#if 0

CodeThorn::DFAnalysisBase::iterator CodeThorn::DFAnalysisBase::begin() {
  return _analyzerDataPostInfo.begin();
}


DFAnalysisBase::iterator CodeThorn::DFAnalysisBase::end() {
  return _analyzerDataPostInfo.end();
}


size_t DFAnalysisBase::size() {
  return _analyzerDataPostInfo.size();
}
#endif // begin/end

/*!
  * \author Markus Schordan
  * \date 2012.
 */

// parameter 2: ingoing information: true, outgoing=false
/* TODO: nodes with multiple associated labels need to be attached multiple attributes (or a list of attributes)
   e.g. FunctionEntry/FunctionExit are associated with SgFunctionDefinition (should be a vector of attributes at each node)
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

