/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef DFANALYZER_C
#define DFANALYZER_C

#include "DFAnalyzer.h"

using namespace CodeThorn;

template<typename LatticeType>
void DFAnalyzer<LatticeType>::setExtremalLabels(set<Label> extremalLabels) {
  _extremalLabels=extremalLabels;
}
template<typename LatticeType>
DFAnalyzer<LatticeType>::DFAnalyzer()
  :
   _labeler(0),
   _cfanalyzer(0)
{}

template<typename LatticeType>
void
DFAnalyzer<LatticeType>::initialize(SgProject* root) {
  cout << "INIT: Creating VariableIdMapping."<<endl;
  _variableIdMapping.computeVariableSymbolMapping(root);
  cout << "INIT: Creating Labeler."<<endl;
  _labeler= new Labeler(root,getVariableIdMapping());
  //cout << "INIT: Initializing ExprAnalyzer."<<endl;
  //exprAnalyzer.setVariableIdMapping(getVariableIdMapping());
  cout << "INIT: Creating CFAnalyzer."<<endl;
  _cfanalyzer=new CFAnalyzer(_labeler);
  //cout<< "DEBUG: mappingLabelToNode: "<<endl<<getLabeler()->toString()<<endl;
  cout << "INIT: Building CFGs."<<endl;
  _flow=_cfanalyzer->flow(root);
  cout << "STATUS: Building CFGs finished."<<endl;
  //  if(boolOptions["reduce-cfg"]) {
  //    int cnt=cfanalyzer->reduceBlockBeginNodes(flow);
  //    cout << "INIT: CFG reduction OK. (eliminated "<<cnt<<" nodes)"<<endl;
  //}
  cout << "INIT: Intra-Flow OK. (size: " << _flow.size() << " edges)"<<endl;
  InterFlow interFlow=_cfanalyzer->interFlow(_flow);
  cout << "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
  _cfanalyzer->intraInterFlow(_flow,interFlow);
  cout << "INIT: IntraInter-CFG OK. (size: " << _flow.size() << " edges)"<<endl;
  for(long l=0;l<_labeler->numberOfLabels();++l) {
    LatticeType le;
    _analyzerData.push_back(le);
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
  cout<<"INFO: solver started."<<endl;
  ROSE_ASSERT(!_workList.isEmpty());
  while(!_workList.isEmpty()) {
    Label lab=_workList.take();
    //cout<<"INFO: worklist size: "<<_workList.size()<<endl;
    //_analyzerData[lab]=_analyzerData comb transfer(lab,combined(Pred));
    LabelSet pred=_flow.pred(lab);
    LatticeType inInfo;
    for(LabelSet::iterator i=pred.begin();i!=pred.end();++i) {
      inInfo.combine(_analyzerData[*i]);
    }
    
    LatticeType newInfo=transfer(lab,inInfo);
    //cout<<"NewInfo: ";newInfo.toStream(cout);cout<<endl;
    bool isLoopCondition=SgNodeHelper::isLoopCond(_labeler->getNode(lab));
    if(!newInfo.approximatedBy(_analyzerData[lab])) {
      _analyzerData[lab].combine(newInfo);
      // semantic propagation: if node[l] is a condition of a loop only propagate on the true branch
      LabelSet succ;
      {
        if(isLoopCondition) {
          succ=_flow.outEdgesOfType(lab,EDGE_TRUE).targetLabels();
        } else {
          succ=_flow.succ(lab);
        }
      }
      _workList.add(succ);
    } else {
      // no new information was computed. Nothing to do (except for the case it is a loop-condition of a loop for which we may have found a fix-point)
      if(isLoopCondition) {
        LabelSet succ=_flow.outEdgesOfType(lab,EDGE_FALSE).targetLabels();
        _workList.add(succ);
      } else {
        // nothing to add
      }
    }
  }
  cout<<"INFO: solver finished."<<endl;
}
// runs until worklist is empty
template<typename LatticeType>
void
DFAnalyzer<LatticeType>::run() {
  // initialize work list with extremal labels
  for(set<Label>::iterator i=_extremalLabels.begin();i!=_extremalLabels.end();++i) {
    _workList.add(*i);
  }
  solve();
}

// default identity function
template<typename LatticeType>
LatticeType
DFAnalyzer<LatticeType>::transfer(Label lab, LatticeType element) {
  return element;
}

template<typename LatticeType>
typename DFAnalyzer<LatticeType>::ResultAccess&
DFAnalyzer<LatticeType>::getResultAccess() {
  return _analyzerData;
}

#include <iostream>

#include "AttributeAnnotator.h"
#include <string>

using std::string;

class GeneralResultAttribute : public AnalysisResultAttribute {
public:
  GeneralResultAttribute(string postinfo) { _postinfo="// "+postinfo;}
  string getPreInfoString() { return ""; }
  string getPostInfoString() { return _postinfo; }
private:
  string _postinfo;
};

#include <sstream>

template<typename LatticeType>
void DFAnalyzer<LatticeType>::attachResultsToAst(string attributeName) {
  size_t lab=0;
  for(typename std::vector<LatticeType>::iterator i=_analyzerData.begin();
      i!=_analyzerData.end();
      ++i) {
    std::stringstream ss;
    (&(*i))->toStream(ss);
    //std::cout<<ss.str();
    // TODO: need to add a solution for nodes with multiple associated labels (e.g. functio call)
    _labeler->getNode(lab)->setAttribute(attributeName,new GeneralResultAttribute(ss.str()));
    lab++;
  }

}

template<typename LatticeType>
Labeler* DFAnalyzer<LatticeType>::getLabeler() {
  return _labeler;
}

template<typename LatticeType>
VariableIdMapping* DFAnalyzer<LatticeType>::getVariableIdMapping() {
  return &_variableIdMapping;
}

#endif
