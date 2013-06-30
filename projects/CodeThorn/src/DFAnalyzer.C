/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#ifndef MFANALYZER_C
#define MFANALYZER_C

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
VariableIdMapping* DFAnalyzer<LatticeType>::getVariableIdMapping() {
  return &_variableIdMapping;
}
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
  //	int cnt=cfanalyzer->reduceBlockBeginNodes(flow);
  //	cout << "INIT: CFG reduction OK. (eliminated "<<cnt<<" nodes)"<<endl;
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
  std::string funtofind=functionToStartAt;
  RoseAst completeast(root);
  startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) { 
    std::cerr << "Function '"<<funtofind<<"' not found.\n"; exit(1);
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
	set<VariableId> setOfUsedVars;
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
	cout<<"STATUS: Number of extremal labels: "<<_extremalLabels.size()<<endl;
  } else {
	// we may initialize alternatively with all labels
  }
}
// runs until worklist is empty
template<typename LatticeType>
void
DFAnalyzer<LatticeType>::solve() {
  while(!workList.isEmpty()) {
	Label lab=workList.take();
	// TODO;
	// compute set of predecessors Pred
	// apply combined(Pred)
	//_analyzerData[lab]=_analyzerData comb transfer(lab,combined(Pred));
	_analyzerData[lab]=transfer(lab,_analyzerData[lab]);
  }
}
// runs until worklist is empty
template<typename LatticeType>
void
DFAnalyzer<LatticeType>::run() {
  // initialize work list with extremal labels
  for(set<Label>::iterator i=_extremalLabels.begin();i!=_extremalLabels.end();++i) {
	workList.add(*i);
  }
  solve();
}

// default identity function
template<typename LatticeType>
LatticeType
DFAnalyzer<LatticeType>::transfer(Label lab, LatticeType element) {
  return element;
}

#endif
