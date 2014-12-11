/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef DFANALYZER_C
#define DFANALYZER_C

#include "sage3basic.h"
#include "DFAnalyzer.h"
#include "AnalysisAbstractionLayer.h"
#include "GeneralResultAttribute.h"

using namespace CodeThorn;

template<typename LatticeType>
DFAnalyzer<LatticeType>::DFAnalyzer():
  _labeler(0),
  _cfanalyzer(0),
  _numberOfLabels(0),
  _preInfoIsValid(false),
  _transferFunctions(0)
{}

template<typename LatticeType>
LatticeType DFAnalyzer<LatticeType>::getPreInfo(Label lab) {
  if(!_preInfoIsValid) {
    computeAllPreInfo();
    ROSE_ASSERT(_preInfoIsValid==true);
  }
  return *_analyzerDataPreInfo[lab];
}

template<typename LatticeType>
LatticeType DFAnalyzer<LatticeType>::getPostInfo(Label lab) {
  return *_analyzerData[lab];
}

template<typename LatticeType>
void DFAnalyzer<LatticeType>::computeAllPreInfo() {
  for(long lab=0;lab<_labeler->numberOfLabels();++lab) {
    LatticeType le;
    computePreInfo(lab,le);
    _analyzerDataPreInfo[lab]=le;
  }
  _preInfoIsValid=true;
}

template<typename LatticeType>
void DFAnalyzer<LatticeType>::setExtremalLabels(set<Label> extremalLabels) {
  _extremalLabels=extremalLabels;
}

template<typename LatticeType>
LatticeType DFAnalyzer<LatticeType>::initializeGlobalVariables(SgProject* root) {
  ROSE_ASSERT(root);
  cout << "INFO: Initializing property state with global variables."<<endl;
  VariableIdSet globalVars=AnalysisAbstractionLayer::globalVariables(root,&_variableIdMapping);
  VariableIdSet usedVarsInFuncs=AnalysisAbstractionLayer::usedVariablesInsideFunctions(root,&_variableIdMapping);
  VariableIdSet usedGlobalVarIds=globalVars*usedVarsInFuncs;
  cout <<"INFO: global variables: "<<globalVars.size()<<endl;
  cout <<"INFO: used variables in functions: "<<usedVarsInFuncs.size()<<endl;
  cout <<"INFO: used global vars: "<<usedGlobalVarIds.size()<<endl;
  LatticeType elem;
  list<SgVariableDeclaration*> usedGlobalVarDecls=SgNodeHelper::listOfGlobalVars(root);
  for(list<SgVariableDeclaration*>::iterator i=usedGlobalVarDecls.begin();i!=usedGlobalVarDecls.end();++i) {
    if(usedGlobalVarIds.find(_variableIdMapping.variableId(*i))!=usedGlobalVarIds.end())
      elem=transfer(_labeler->getLabel(*i),elem);
  }
  cout << "INIT: initial element: ";
  elem.toStream(cout,&_variableIdMapping);
  cout<<endl;
  _initialElement=elem;
  return elem;
}

template<typename LatticeType>
void
DFAnalyzer<LatticeType>::initialize(SgProject* root) {
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
  for(long l=0;l<_labeler->numberOfLabels();++l) {
    LatticeType le;
    _analyzerDataPreInfo.push_back(le);
    _analyzerData.push_back(le);
  }
  cout << "INIT: Optimizing CFGs for label-out-info solver 1."<<endl;
  {
    size_t numDeletedEdges=_cfanalyzer->deleteFunctionCallLocalEdges(_flow);
    int numReducedNodes=_cfanalyzer->reduceBlockBeginNodes(_flow);
    cout << "INIT: Optimization finished (educed nodes: "<<numReducedNodes<<" deleted edges: "<<numDeletedEdges<<")"<<endl;
  }
  cout << "STATUS: initialized monotone data flow analyzer for "<<_analyzerData.size()<< " labels."<<endl;

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
    _analyzerData[startlab]=initializeGlobalVariables(root);
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


template<typename LatticeType>
void DFAnalyzer<LatticeType>::initializeTransferFunctions() {
  ROSE_ASSERT(_transferFunctions);
  ROSE_ASSERT(_labeler);
  _transferFunctions->setLabeler(_labeler);
  _transferFunctions->setVariableIdMapping(&_variableIdMapping);
}

template<typename LatticeType>
void
DFAnalyzer<LatticeType>::determineExtremalLabels(SgNode* startFunRoot=0) {
  if(startFunRoot) {
    Label startLabel=_cfanalyzer->getLabel(startFunRoot);
    _extremalLabels.insert(startLabel);
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
template<typename LatticeType>
void
DFAnalyzer<LatticeType>::solve() {
  DFSolver1<LatticeType> dfSolver1(_workList,_analyzerDataPreInfo,_analyzerData,_initialElement,_flow,*_transferFunctions) ;
  dfSolver1.runSolver();
  _preInfoIsValid=false;
}
template<typename LatticeType>

void
DFAnalyzer<LatticeType>::computePreInfo(Label lab,LatticeType& inInfo) {
  LabelSet pred=_flow.pred(lab);
  for(LabelSet::iterator i=pred.begin();i!=pred.end();++i) {
    inInfo.combine(_analyzerData[(*i).getId()]);
  }
}

template<typename LatticeType>
DFAstAttribute* DFAnalyzer<LatticeType>::createDFAstAttribute(LatticeType* elem) {
  // elem ignored in default function
  return new DFAstAttribute();
}

// runs until worklist is empty
template<typename LatticeType>
void
DFAnalyzer<LatticeType>::run() {
  // initialize work list with extremal labels
  for(set<Label>::iterator i=_extremalLabels.begin();i!=_extremalLabels.end();++i) {
    cout << "Initializing "<<*i<<" with ";
    _analyzerData[(*i).getId()]=transfer(*i,_initialElement);
    _analyzerData[(*i).getId()].toStream(cout,&_variableIdMapping);
    cout<<endl;
    LabelSet initsucc=_flow.succ(*i);
    for(LabelSet::iterator i=initsucc.begin();i!=initsucc.end();++i) {
      _workList.add(*i);
    }
  }
  solve();
}

// default identity function
template<typename LatticeType>
LatticeType
DFAnalyzer<LatticeType>::transfer(Label lab, LatticeType element) {
  ROSE_ASSERT(_transferFunctions);
  return _transferFunctions->transfer(lab,element);
}

template<typename LatticeType>
typename DFAnalyzer<LatticeType>::ResultAccess&
DFAnalyzer<LatticeType>::getResultAccess() {
  return _analyzerData;
}

#include <iostream>

#include "AstAnnotator.h"
#include <string>

using std::string;

#include <sstream>
#if 0
template<typename LatticeType>
void DFAnalyzer<LatticeType>::attachResultsToAst(string attributeName) {
  size_t lab=0;
  for(typename std::vector<LatticeType>::iterator i=_analyzerData.begin();
      i!=_analyzerData.end();
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
template<typename LatticeType>
CFAnalyzer* DFAnalyzer<LatticeType>::getCFAnalyzer() {
  return _cfanalyzer;
}

template<typename LatticeType>
Labeler* DFAnalyzer<LatticeType>::getLabeler() {
  return _labeler;
}

template<typename LatticeType>
VariableIdMapping* DFAnalyzer<LatticeType>::getVariableIdMapping() {
  return &_variableIdMapping;
}

#if 0
template<typename LatticeType>
CodeThorn::DFAnalyzer::iterator CodeThorn::DFAnalyzer<LatticeType>::begin() {
  return _analyzerData.begin();
}
  
template<typename LatticeType>
DFAnalyzer::iterator CodeThorn::DFAnalyzer<LatticeType>::end() {
  return _analyzerData.end();
}

template<typename LatticeType>
size_t DFAnalyzer<LatticeType>::size() {
  return _analyzerData.size();
}
#endif // begin/end

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename LatticeType>
void DFAnalyzer<LatticeType>::attachInfoToAst(string attributeName,bool inInfo) {
  if(inInfo && !_preInfoIsValid)
    computeAllPreInfo();
  LabelSet labelSet=_flow.nodeLabels();
  for(LabelSet::iterator i=labelSet.begin();
      i!=labelSet.end();
      ++i) {
    ROSE_ASSERT(*i<_analyzerData.size());
    // TODO: need to add a solution for nodes with multiple associated labels (e.g. function call)
    if(!_labeler->isFunctionExitLabel(*i) /* && !_labeler->isCallReturnLabel(lab)*/)
      if(*i >=0 ) {
        if(inInfo)
          _labeler->getNode(*i)->setAttribute(attributeName,createDFAstAttribute(&_analyzerDataPreInfo[(*i).getId()]));
        else
          _labeler->getNode(*i)->setAttribute(attributeName,createDFAstAttribute(&_analyzerData[(*i).getId()]));
      }
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename LatticeType>
void DFAnalyzer<LatticeType>::attachInInfoToAst(string attributeName) {
  if(!_preInfoIsValid)
    computeAllPreInfo();
  attachInfoToAst(attributeName,true);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
template<typename LatticeType>
void DFAnalyzer<LatticeType>::attachOutInfoToAst(string attributeName) {
  attachInfoToAst(attributeName,false);
}




#endif
