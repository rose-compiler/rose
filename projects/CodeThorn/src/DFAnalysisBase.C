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

using namespace SPRAY;
using namespace std;

DFAnalysisBase::DFAnalysisBase():
  _labeler(0),
  _cfanalyzer(0),
  _numberOfLabels(0),
  _preInfoIsValid(false),
  _postInfoIsValid(false),
  _transferFunctions(0),
  _initialElementFactory(0),
  _analysisType(DFAnalysisBase::FORWARD_ANALYSIS),
  _pointerAnalysisInterface(0),
  _pointerAnalysisEmptyImplementation(0)
{}

DFAnalysisBase::~DFAnalysisBase() {
  if(_pointerAnalysisEmptyImplementation)
    delete _pointerAnalysisEmptyImplementation;
}
void DFAnalysisBase::initializeSolver() {
  ROSE_ASSERT(&_workList);
  ROSE_ASSERT(&_initialElementFactory);
  ROSE_ASSERT(&_analyzerDataPreInfo);
  ROSE_ASSERT(&_analyzerDataPostInfo);
  ROSE_ASSERT(&_flow);
  ROSE_ASSERT(&_transferFunctions);
  _solver=new SPRAY::PASolver1(_workList,
                      _analyzerDataPreInfo,
                      _analyzerDataPostInfo,
                      *_initialElementFactory,
                      _flow,
                      *_transferFunctions);
}

void DFAnalysisBase::initializeExtremalValue(Lattice* element) {
  // default identity function
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
    for(Labeler::iterator i=_labeler->begin();i!=_labeler->end();++i) {
      Label lab=*i;
      Lattice* info=_initialElementFactory->create();
      _solver->computeCombinedPreInfo(lab,*info);
      _transferFunctions->transfer(lab,*info);
      if(_analyzerDataPostInfo[lab.getId()]) {
	delete _analyzerDataPostInfo[lab.getId()];
      }
      _analyzerDataPostInfo[lab.getId()]=info;
    }
    _postInfoIsValid=true;
  }
}

void DFAnalysisBase::setInitialElementFactory(PropertyStateFactory* pf) {
  _initialElementFactory=pf;
}

void DFAnalysisBase::setExtremalLabels(set<Label> extremalLabels) {
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

// outdated
Lattice* DFAnalysisBase::initializeGlobalVariables(SgProject* root) {
  ROSE_ASSERT(root);
  cout << "INFO: Initializing property state with global variables."<<endl;
  VariableIdSet globalVars=AnalysisAbstractionLayer::globalVariables(root,&_variableIdMapping);
  VariableIdSet usedVarsInFuncs=AnalysisAbstractionLayer::usedVariablesInsideFunctions(root,&_variableIdMapping);
  VariableIdSet usedVarsInGlobalVarsInitializers=AnalysisAbstractionLayer::usedVariablesInGlobalVariableInitializers(root,&_variableIdMapping);
  VariableIdSet usedGlobalVarIds=globalVars*usedVarsInFuncs; //+usedVarsInGlobalVarsInitializers;;
  usedGlobalVarIds.insert(usedVarsInGlobalVarsInitializers.begin(),
			  usedVarsInGlobalVarsInitializers.end());
  cout <<"INFO: number of global variables: "<<globalVars.size()<<endl;
  //  cout <<"INFO: used variables in functions: "<<usedVarsInFuncs.size()<<endl;
  //cout <<"INFO: used global vars: "<<usedGlobalVarIds.size()<<endl;
  Lattice* elem=_initialElementFactory->create();
  list<SgVariableDeclaration*> globalVarDecls=SgNodeHelper::listOfGlobalVars(root);
  for(list<SgVariableDeclaration*>::iterator i=globalVarDecls.begin();i!=globalVarDecls.end();++i) {
    if(usedGlobalVarIds.find(_variableIdMapping.variableId(*i))!=usedGlobalVarIds.end()) {
      //cout<<"DEBUG: transfer for global var @"<<_labeler->getLabel(*i)<<" : "<<(*i)->unparseToString()<<endl;
      ROSE_ASSERT(_transferFunctions);
      _transferFunctions->transfer(_labeler->getLabel(*i),*elem);
    } else {
      cout<<"INFO: filtered from initial state: "<<(*i)->unparseToString()<<endl;
    }
  }
  cout << "INIT: initial element: ";
  elem->toStream(cout,&_variableIdMapping);
  cout<<endl;
  _globalVariablesState=elem;
  return elem;
}

void
DFAnalysisBase::normalizeProgram(SgProject* root) {
    cout<<"STATUS: Normalizing program."<<endl;
  ExtractFunctionArguments efa;
  if(!efa.IsNormalized(root)) {
    cout<<"STATUS: Normalizing function call arguments."<<endl;
    efa.NormalizeTree(root,true);
  }
  FunctionCallNormalization fn;
  cout<<"STATUS: Normalizing function calls in expressions."<<endl;
  fn.visit(root);
}

void
DFAnalysisBase::initialize(SgProject* root, bool variableIdForEachArrayElement/* = false*/) {
  cout << "INIT: Creating VariableIdMapping." << endl;
  if (variableIdForEachArrayElement)
    _variableIdMapping.setModeVariableIdForEachArrayElement(true);
  _variableIdMapping.computeVariableSymbolMapping(root);
  _pointerAnalysisEmptyImplementation=new PointerAnalysisEmptyImplementation(&_variableIdMapping);
  _pointerAnalysisEmptyImplementation->initialize();
  _pointerAnalysisEmptyImplementation->run();
  cout << "INIT: Creating Labeler."<<endl;
  _labeler= new Labeler(root);
  //cout << "INIT: Initializing ExprAnalyzer."<<endl;
  //exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  cout << "INIT: Creating CFAnalysis."<<endl;
  _cfanalyzer=new CFAnalysis(_labeler);
  //cout<< "DEBUG: mappingLabelToLabelProperty: "<<endl<<getLabeler()->toString()<<endl;
  cout << "INIT: Building CFG for each function."<<endl;
  _flow=_cfanalyzer->flow(root);
  cout << "STATUS: Building CFGs finished."<<endl;
  cout << "INIT: Intra-Flow OK. (size: " << _flow.size() << " edges)"<<endl;
  InterFlow interFlow=_cfanalyzer->interFlow(_flow);
  cout << "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
  _cfanalyzer->intraInterFlow(_flow,interFlow);
  cout << "INIT: IntraInter-CFG OK. (size: " << _flow.size() << " edges)"<<endl;
  cout << "INIT: Optimizing CFGs for label-out-info solver 1."<<endl;
  {
    size_t numDeletedEdges=_cfanalyzer->deleteFunctionCallLocalEdges(_flow);
    int numReducedNodes=0; //_cfanalyzer->reduceBlockBeginNodes(_flow);
    cout << "INIT: Optimization finished (educed nodes: "<<numReducedNodes<<" deleted edges: "<<numDeletedEdges<<")"<<endl;
  }

  ROSE_ASSERT(_initialElementFactory);
  for(long l=0;l<_labeler->numberOfLabels();++l) {
    Lattice* le1=_initialElementFactory->create();
    _analyzerDataPreInfo.push_back(le1);
    Lattice* le2=_initialElementFactory->create();
    _analyzerDataPostInfo.push_back(le2);
  }
  cout << "STATUS: initialized monotone data flow analyzer for "<<_analyzerDataPreInfo.size()<< " labels."<<endl;

  cout << "INIT: initialized pre/post property states."<<endl;
  if(isBackwardAnalysis()) {
    _flow=_flow.reverseFlow();
    cout << "INIT: established reverse flow for backward analysis."<<endl;
  }

  initializeSolver();
  cout << "STATUS: initialized solver."<<endl;
}

void DFAnalysisBase::initializeTransferFunctions() {
  ROSE_ASSERT(_transferFunctions);
  ROSE_ASSERT(_labeler);
  _transferFunctions->setLabeler(_labeler);
  _transferFunctions->setVariableIdMapping(&_variableIdMapping);
  if(_pointerAnalysisInterface==0)
    _transferFunctions->setPointerAnalysis(_pointerAnalysisEmptyImplementation);
  else
    _transferFunctions->setPointerAnalysis(_pointerAnalysisInterface);
}

void DFAnalysisBase::setPointerAnalysis(PointerAnalysisInterface* pa) {
  _pointerAnalysisInterface=pa;
}

void
DFAnalysisBase::determineExtremalLabels(SgNode* startFunRoot=0) {
  if(startFunRoot) {
    if(isForwardAnalysis()) {
      Label startLabel=_cfanalyzer->getLabel(startFunRoot);
      _extremalLabels.insert(startLabel);
    } else if(isBackwardAnalysis()) {
      if(isSgFunctionDefinition(startFunRoot)) {
        Label startLabel=_cfanalyzer->getLabel(startFunRoot);
        // TODO: temporary hack (requires get-methods for different types of labels
        // or a list of all labels that are associated with a node)
        int startLabelId=startLabel.getId();
        // exit-label = entry-label + 1
        Label endLabel(startLabelId+1);
        _extremalLabels.insert(endLabel);
      } else {
        cerr<<"Error: backward analysis only supported for start at a function exit label."<<endl;
        exit(1);
      }
    }
  } else {
    // naive way of initializing all labels
    for(long i=0;i<_labeler->numberOfLabels();++i) {
      Label lab=i;
      _extremalLabels.insert(lab);
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
  // initialize work list with extremal labels
  for(set<Label>::iterator i=_extremalLabels.begin();i!=_extremalLabels.end();++i) {
    ROSE_ASSERT(_analyzerDataPreInfo[(*i).getId()]!=0);
    initializeExtremalValue(_analyzerDataPreInfo[(*i).getId()]);
    // combine extremal value with global variables initialization state (computed by initializeGlobalVariables)
    _analyzerDataPreInfo[(*i).getId()]->combine(*_globalVariablesState);
    cout<<"INFO: Initialized "<<*i<<" with ";
    cout<<_analyzerDataPreInfo[(*i).getId()]->toString(&_variableIdMapping);
    cout<<endl;
    Flow outEdges=_flow.outEdges(*i);
    for(Flow::iterator j=outEdges.begin();j!=outEdges.end();++j) {
      _workList.add(*j);
    }
#if 0
    LabelSet initsucc=_flow.succ(*i);
    for(LabelSet::iterator i=initsucc.begin();i!=initsucc.end();++i) {
      _workList.add(*i);
    }
#endif
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
  return _cfanalyzer;
}


Labeler* DFAnalysisBase::getLabeler() {
  return _labeler;
}


VariableIdMapping* DFAnalysisBase::getVariableIdMapping() {
  return &_variableIdMapping;
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
  LabelSet labelSet=_flow.nodeLabels();
  for(LabelSet::iterator i=labelSet.begin();
      i!=labelSet.end();
      ++i) {
    ROSE_ASSERT(*i<_analyzerDataPreInfo.size());
    ROSE_ASSERT(*i<_analyzerDataPostInfo.size());
    // TODO: need to add a solution for nodes with multiple associated labels (e.g. function call)
    if(*i >=0 ) {
      Label lab=*i;
      SgNode* node=_labeler->getNode(*i);
      Lattice* info=0;
      if(inInfo) {
        if(isForwardAnalysis()) {
          info=getPreInfo(lab);
          if(_labeler->isSecondLabelOfMultiLabeledNode(lab)) {
            continue;
          }
        } else if(isBackwardAnalysis()) {
          if(_labeler->isSecondLabelOfMultiLabeledNode(lab)) {
            continue;
          }
          info=getPostInfo(lab);
        } else {
          cerr<<"Error: Ast-annotation: unsupported analysis mode."<<endl;
          exit(1);
        }
      } else {
        if(isForwardAnalysis()) {
          if(_labeler->isFirstLabelOfMultiLabeledNode(lab)) {
            continue;
          }
          info=getPostInfo(lab);
        } else if(isBackwardAnalysis()) {
          if(_labeler->isFirstLabelOfMultiLabeledNode(lab)) {
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

