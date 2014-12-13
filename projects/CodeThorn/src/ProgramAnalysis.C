/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h"
#include "ProgramAnalysis.h"
#include "AnalysisAbstractionLayer.h"
#include "GeneralResultAttribute.h"

using namespace CodeThorn;

ProgramAnalysis::ProgramAnalysis():
  _labeler(0),
  _cfanalyzer(0),
  _numberOfLabels(0),
  _preInfoIsValid(false),
  _postInfoIsValid(false),
  _transferFunctions(0),
  _initialElementFactory(0),
  _analysisType(ProgramAnalysis::FORWARD_ANALYSIS)
{}

void ProgramAnalysis::initializeSolver() {
  ROSE_ASSERT(&_workList);
  ROSE_ASSERT(&_initialElementFactory);
  ROSE_ASSERT(&_analyzerDataPreInfo);
  ROSE_ASSERT(&_analyzerDataPostInfo);
  ROSE_ASSERT(&_flow);
  ROSE_ASSERT(&_transferFunctions);
  _solver=new PASolver1(_workList,
                      _analyzerDataPreInfo,
                      _analyzerDataPostInfo,
                      *_initialElementFactory,
                      _flow,
                      *_transferFunctions);
}

void ProgramAnalysis::initializeExtremalValue(Lattice* element) {
  // default identity function
}

Lattice* ProgramAnalysis::getPreInfo(Label lab) {
  return _analyzerDataPreInfo[lab.getId()];
}

Lattice* ProgramAnalysis::getPostInfo(Label lab) {
  return _analyzerDataPostInfo[lab.getId()];
}

void ProgramAnalysis::computeAllPreInfo() {
  if(!_preInfoIsValid) {
    _solver->runSolver();
    _preInfoIsValid=true;
    _postInfoIsValid=false;
  }
}

void ProgramAnalysis::computeAllPostInfo() {
  if(!_postInfoIsValid) {
    computeAllPreInfo();
    for(Labeler::iterator i=_labeler->begin();i!=_labeler->end();++i) {
      Label lab=*i;
      Lattice* info=_analyzerDataPostInfo[lab.getId()];
      _solver->computeCombinedPreInfo(lab,*info);
      _transferFunctions->transfer(lab,*info);
    }
    _postInfoIsValid=true;
  }
}

void ProgramAnalysis::setInitialElementFactory(PropertyStateFactory* pf) {
  _initialElementFactory=pf;
}

void ProgramAnalysis::setExtremalLabels(set<Label> extremalLabels) {
  _extremalLabels=extremalLabels;
}

void ProgramAnalysis::setForwardAnalysis() {
  _analysisType=ProgramAnalysis::FORWARD_ANALYSIS;
}

void ProgramAnalysis::setBackwardAnalysis() {
  _analysisType=ProgramAnalysis::BACKWARD_ANALYSIS;
}

bool ProgramAnalysis::isForwardAnalysis() {
  return _analysisType==ProgramAnalysis::FORWARD_ANALYSIS;
}

bool ProgramAnalysis::isBackwardAnalysis() {
  return _analysisType==ProgramAnalysis::BACKWARD_ANALYSIS;
}

void ProgramAnalysis::initializeGlobalVariables(SgProject* root) {
  ROSE_ASSERT(root);
  cout << "INFO: Initializing property state with global variables."<<endl;
  VariableIdSet globalVars=AnalysisAbstractionLayer::globalVariables(root,&_variableIdMapping);
  VariableIdSet usedVarsInFuncs=AnalysisAbstractionLayer::usedVariablesInsideFunctions(root,&_variableIdMapping);
  VariableIdSet usedGlobalVarIds=globalVars*usedVarsInFuncs;
  cout <<"INFO: global variables: "<<globalVars.size()<<endl;
  cout <<"INFO: used variables in functions: "<<usedVarsInFuncs.size()<<endl;
  cout <<"INFO: used global vars: "<<usedGlobalVarIds.size()<<endl;
  Lattice* elem=_initialElementFactory->create();
  list<SgVariableDeclaration*> usedGlobalVarDecls=SgNodeHelper::listOfGlobalVars(root);
  for(list<SgVariableDeclaration*>::iterator i=usedGlobalVarDecls.begin();i!=usedGlobalVarDecls.end();++i) {
    if(usedGlobalVarIds.find(_variableIdMapping.variableId(*i))!=usedGlobalVarIds.end()) {
      cout<<"DEBUG: transfer for global var "<<(*i)->unparseToString()<<endl;
      ROSE_ASSERT(_transferFunctions);
      _transferFunctions->transfer(_labeler->getLabel(*i),*elem);
    }
  }
  cout << "INIT: initial element: ";
  elem->toStream(cout,&_variableIdMapping);
  cout<<endl;
}


void
ProgramAnalysis::initialize(SgProject* root) {
  cout << "INIT: Creating VariableIdMapping."<<endl;
  _variableIdMapping.computeVariableSymbolMapping(root);
  cout << "INIT: Creating Labeler."<<endl;
  _labeler= new Labeler(root);
  //cout << "INIT: Initializing ExprAnalyzer."<<endl;
  //exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  cout << "INIT: Creating CFAnalyzer."<<endl;
  _cfanalyzer=new CFAnalyzer(_labeler);
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
    int numReducedNodes=_cfanalyzer->reduceBlockBeginNodes(_flow);
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


#if 0
  std::string functionToStartAt="main";
  std::string funtofind=functionToStartAt;
  RoseAst completeast(root);
  SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) { 
    std::cerr << "Function '"<<funtofind<<"' not found.\n"; 
    exit(1);
  } else {
    // determine label of function
    Label startlab=_labeler->getLabel(startFunRoot);
    set<Label> elab;
    elab.insert(startlab);
    setExtremalLabels(elab);
    initializeTransferFunctions();
    initializeGlobalVariables(root);
    _analyzerDataPostInfo[startlab]=_initialElementFactory->create();
    cout << "STATUS: Initial info established at label "<<startlab<<endl;
  }
#endif
  
  // create empty state
#if 0
  PState emptyPState;
  const PState* emptyPStateStored=processNew(emptyPState);
  assert(emptyPStateStored);
  cout << "INIT: Empty state(stored): "<<emptyPStateStored->toString()<<endl;
  assert(cfanalyzer);

  Label startLabel=cfanalyzer->getLabel(startFunRoot);

  if(SgProject* project=isSgProject(root)) {
    cout << "STATUS: Number of global variables: ";
    list<SgVariableDeclaration*> globalVars=SgNodeHelper::listOfGlobalVars(project);
    cout << globalVars.size()<<endl;
    
    list<SgVarRefExp*> varRefExpList=SgNodeHelper::listOfUsedVarsInFunctions(project);
    // compute set of varIds (it is a set because we want multiple uses of the same var to be represented by one id)
    VariableIdMapping::VariableIdSet setOfUsedVars;
    for(list<SgVarRefExp*>::iterator i=varRefExpList.begin();i!=varRefExpList.end();++i) {
      setOfUsedVars.insert(variableIdMapping.variableId(*i));
    }
    cout << "STATUS: Number of used variables: "<<setOfUsedVars.size()<<endl;

    int filteredVars=0;
    for(list<SgVariableDeclaration*>::iterator i=globalVars.begin();i!=globalVars.end();++i) {
      if(setOfUsedVars.find(variableIdMapping.variableId(*i))!=setOfUsedVars.end()) {
        globalVarName2VarIdMapping[variableIdMapping.variableName(variableIdMapping.variableId(*i))]=variableIdMapping.variableId(*i);
        //estate=analyzeVariableDeclaration(*i,estate,estate.label());
      }
      else
        filteredVars++;
    }
    cout << "STATUS: Number of filtered variables for initial pstate: "<<filteredVars<<endl;
  } else {
    cout << "INIT: no global scope.";
  }    
#endif
}



void ProgramAnalysis::initializeTransferFunctions() {
  ROSE_ASSERT(_transferFunctions);
  ROSE_ASSERT(_labeler);
  _transferFunctions->setLabeler(_labeler);
  _transferFunctions->setVariableIdMapping(&_variableIdMapping);
}


void
ProgramAnalysis::determineExtremalLabels(SgNode* startFunRoot=0) {
  if(startFunRoot) {
    if(isForwardAnalysis()) {
      Label startLabel=_cfanalyzer->getLabel(startFunRoot);
      _extremalLabels.insert(startLabel);
    } else if(isBackwardAnalysis()) {
      Label startLabel=_cfanalyzer->getLabel(startFunRoot);
      // TODO: temporary hack (requires get-methods for different types of labels
      // or a list of all labels that are associated with a node)
      int startLabelId=startLabel.getId();
      Label endLabel(startLabelId+1);
      _extremalLabels.insert(endLabel);
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
ProgramAnalysis::solve() {
  computeAllPreInfo();
  computeAllPostInfo();
}

DFAstAttribute* ProgramAnalysis::createDFAstAttribute(Lattice* elem) {
  // elem ignored in default function
  return new DFAstAttribute();
}

// runs until worklist is empty

void
ProgramAnalysis::run() {
  // initialize work list with extremal labels
  for(set<Label>::iterator i=_extremalLabels.begin();i!=_extremalLabels.end();++i) {
    ROSE_ASSERT(_analyzerDataPreInfo[(*i).getId()]!=0);
    initializeExtremalValue(_analyzerDataPreInfo[(*i).getId()]);
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

ProgramAnalysis::ResultAccess&
ProgramAnalysis::getResultAccess() {
  return _analyzerDataPreInfo;
}

#include <iostream>

#include "AstAnnotator.h"
#include <string>

using std::string;

#include <sstream>
#if 0

void ProgramAnalysis::attachResultsToAst(string attributeName) {
  size_t lab=0;
  for(std::vector::iterator i=_analyzerDataPreInfo.begin();
      i!=_analyzerDataPreInfo.end();
      ++i) {
    std::stringstream ss;
    (&(*i))->toStream(ss,&_variableIdMapping);
    //std::cout<<ss.str();
    // TODO: need to add a solution for nodes with multiple associated labels (e.g. functio call)
    _labeler->getNode(lab)->setAttribute(attributeName,new GeneralResultAttribute(ss.str()));
    lab++;
  }

}
#endif

CFAnalyzer* ProgramAnalysis::getCFAnalyzer() {
  return _cfanalyzer;
}


Labeler* ProgramAnalysis::getLabeler() {
  return _labeler;
}


VariableIdMapping* ProgramAnalysis::getVariableIdMapping() {
  return &_variableIdMapping;
}

#if 0

CodeThorn::ProgramAnalysis::iterator CodeThorn::ProgramAnalysis::begin() {
  return _analyzerDataPostInfo.begin();
}
  

ProgramAnalysis::iterator CodeThorn::ProgramAnalysis::end() {
  return _analyzerDataPostInfo.end();
}


size_t ProgramAnalysis::size() {
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
void ProgramAnalysis::attachInfoToAst(string attributeName,bool inInfo) {
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
      if(inInfo) {
        if(isForwardAnalysis()) {
          _labeler->getNode(*i)->setAttribute(attributeName,createDFAstAttribute(getPreInfo(lab)));
        } else if(isBackwardAnalysis()) {
          _labeler->getNode(*i)->setAttribute(attributeName,createDFAstAttribute(getPostInfo(lab)));
        } else {
          cerr<<"Error: Ast-annotation: unsupported analysis mode."<<endl;
          exit(1);
        }
      } else {
        if(isForwardAnalysis()) {
          _labeler->getNode(*i)->setAttribute(attributeName,createDFAstAttribute(getPostInfo(lab)));
        } else if(isBackwardAnalysis()) {
          _labeler->getNode(*i)->setAttribute(attributeName,createDFAstAttribute(getPreInfo(lab)));
        } else {
          cerr<<"Error: Ast-annotation: unsupported analysis mode."<<endl;
          exit(1);
        }
      }
    }
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */

void ProgramAnalysis::attachInInfoToAst(string attributeName) {
  attachInfoToAst(attributeName,true);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */

void ProgramAnalysis::attachOutInfoToAst(string attributeName) {
  attachInfoToAst(attributeName,false);
}
