/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Analyzer.h"
#include "CollectionOperators.h"
#include "CommandLineOptions.h"

string color(string);

Analyzer::Analyzer():startFunRoot(0),cfanalyzer(0) {
}

set<string> Analyzer::variableIdsToVariableNames(set<VariableId> s) {
  set<string> res;
  for(set<VariableId>::iterator i=s.begin();i!=s.end();++i) {
	res.insert((*i).variableName());
  }
  return res;
}
string Analyzer::nodeToString(SgNode* node) {
  string textual;
  if(node->attributeExists("info"))
	 textual=node->getAttribute("info")->toString()+":";
  return textual+SgNodeHelper::nodeToString(node);
}

Analyzer::~Analyzer() {
  // intentionally empty, nothing to free explicitly
}

void Analyzer::recordTransition(const EState* sourceState, Edge e, const EState* targetState) {
  transitionGraph.push_back(Transition(sourceState,e,targetState));
}

void Analyzer::printStatusMessage() {
  static long int lastNumEStates=displayDiff;
  if(isEmptyWorkList()) {
	//cout << "Empty Work List: empty."<<endl;
	return;
  }
  const EState* topWorkListeState=topWorkList();
  if(topWorkListeState) {
	//cout << "DEBUG: Working on: "<<topWorkList()->toString()<<endl;
  } else {
	cerr << "INTERNAL ERROR: null pointer in work list. Bailing out. "<<endl;
	exit(1);
  }
	// report we are alife
  if((eStateSet.size()-lastNumEStates)>=displayDiff) {
	lastNumEStates=eStateSet.size();
	cout<<color("white")<<"Number of states/estates/transitions: ";
	cout<<color("magenta")<<stateSet.size()
		<<color("white")<<"/"
		<<color("cyan")<<eStateSet.size()
		<<color("white")<<"/"
		<<color("blue")<<transitionGraph.size()
		<<color("white")<<"";
	cout<<endl;
  }
}

void Analyzer::addToWorkList(const EState* eState) { 
  if(!eState) {
	cerr<<"INTERNAL ERROR: null pointer added to work list."<<endl;
	exit(1);
  }
  eStateWorkList.push(eState); 
}


void Analyzer::runSolver1() {
  displayDiff=0;
  while(!isEmptyWorkList()) {
	printStatusMessage();
	displayDiff=1000;
	const EState* currentEStatePtr=popWorkList();
	assert(currentEStatePtr);
	Flow edgeSet=flow.outEdges(currentEStatePtr->label);
	//cerr << "DEBUG: edgeSet size:"<<edgeSet.size()<<endl;
	for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
	  //cerr << "DEBUG: Before transferFunction:"<<nodeToString(getLabeler()->getNode((*i).source)) <<" : eState="<<currentEStatePtr->toString()<<endl;
	  EState newEState=transferFunction(*i,currentEStatePtr);
	  if(newEState.label!=NO_ESTATE && (!newEState.constraints.deqConstraintExists()) &&(!isFailedAssertEState(&newEState))) {
		//cerr << "DEBUG: Adding to Worklist:"<<newEState.toString()<<endl;
		const EState* newEStatePtr=addToWorkListIfNew(newEState);
		recordTransition(currentEStatePtr,*i,newEStatePtr);
	  }
	  if(newEState.label!=NO_ESTATE && (!newEState.constraints.deqConstraintExists()) && (isFailedAssertEState(&newEState))) {
		// failed-assert end-state: do not add to work list but do add it to the transition graph
		const EState* newEStatePtr=processNewOrExistingEState(newEState);
		recordTransition(currentEStatePtr,*i,newEStatePtr);		
		if(boolOptions["report-failed-assert"]) {
		  cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
		}
	  }
	  if(newEState.label==NO_STATE) {
		//cerr << "DEBUG: NO_ESTATE (transition not recorded)"<<endl;
		//cerr << "INFO: found final state."<<endl;
	  }
	  //cerr << "DEBUG: After transferFunction: eState="<<newEState.toString()<<endl;
	}
  }
  displayDiff=0;
  printStatusMessage();
  cout << "analysis finished (worklist is empty)."<<endl;
}

const EState* Analyzer::addToWorkListIfNew(EState eState) {
  EStateSet::ProcessingResult res=processEState(eState);
  if(res.first==false) {
	const EState* newStatePtr=res.second;
	assert(newStatePtr);
	addToWorkList(newStatePtr);
	return newStatePtr;
  } else {
	//cout << "DEBUG: State already exists. Not added:"<<eState.toString()<<endl;
	const EState* existingStatePtr=res.second;
	assert(existingStatePtr);
	return existingStatePtr;
  }
}

EState Analyzer::analyzeVariableDeclaration(SgVariableDeclaration* decl,EState currentEState, Label targetLabel) {
  //cout << "INFO1: we are at "<<astTermWithNullValuesToString(nextNodeToAnalyze1)<<endl;
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  if(initName0) {
	if(SgInitializedName* initName=isSgInitializedName(initName0)) {
	  SgSymbol* initDeclVar=initName->search_for_symbol_from_symbol_table();
	  VariableId initDeclVarId=VariableId(initDeclVar);
	  SgName initDeclVarName=initDeclVar->get_name();
	  string initDeclVarNameString=initDeclVarName.getString();
	  //cout << "INIT-DECLARATION: var:"<<initDeclVarNameString<<"=";
	  SgInitializer* initializer=initName->get_initializer();
	  ConstraintSet cset=currentEState.constraints;
	  if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
		//cout << "initalizer found:"<<endl;
		SgExpression* rhs=assignInitializer->get_operand_i();
		State newState=analyzeAssignRhs(*currentEState.state,initDeclVarId,rhs,cset);
		const State* newStatePtr=processNewOrExistingState(newState);
		assert(newStatePtr);
		return EState(targetLabel,newStatePtr,cset);
	  } else {
		//cout << "no initializer (OK)."<<endl;
		State newState=*currentEState.state;
		newState[initDeclVarId]=AType::Top();
		const State* newStatePtr=processNewOrExistingState(newState);
		return EState(targetLabel,newStatePtr,cset);
	  }
	} else {
	  cerr << "Error: in declaration (@initializedName) no variable found ... bailing out."<<endl;
	  exit(1);
	}
  } else {
	cerr << "Error: in declaration: no variable found ... bailing out."<<endl;
	exit(1);
  }
}

set<VariableId> Analyzer::determineVariableIdsOfVariableDeclarations(set<SgVariableDeclaration*> varDecls) {
  set<VariableId> resultSet;
  for(set<SgVariableDeclaration*>::iterator i=varDecls.begin();i!=varDecls.end();++i) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfVariableDeclaration(*i);
	if(sym) {
	  resultSet.insert(VariableId(sym));
	}
  }
  return resultSet;
}

set<VariableId> Analyzer::determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList) {
  set<VariableId> resultSet;
  for(SgInitializedNamePtrList::iterator i=namePtrList.begin();i!=namePtrList.end();++i) {
	SgSymbol* sym=SgNodeHelper::getSymbolOfInitializedName(*i);
	if(sym) {
	  resultSet.insert(VariableId(sym));
	}
  }
  return resultSet;
}

bool Analyzer::isAssertExpr(SgNode* node) {
  if(isSgExprStatement(node)) {
	node=SgNodeHelper::getExprStmtChild(node);
	if(isSgConditionalExp(node))
	  return true;
  }
  return false;
}

bool Analyzer::isFailedAssertEState(const EState* eState) {
  return eState->io.op==InputOutput::FAILED_ASSERT;
}

EState Analyzer::createFailedAssertEState(EState eState, Label target) {
  	  EState newEState=eState;
	  newEState.io.recordFailedAssert();
	  newEState.label=target;
	  return newEState;
}

const State* Analyzer::processNewState(State& s) {
  return stateSet.processNewState(s);
}

const State* Analyzer::processNewOrExistingState(State& s) {
  return stateSet.processNewOrExistingState(s);
}

const EState* Analyzer::processNewEState(EState& s) {
  return eStateSet.processNewEState(s);
}

const EState* Analyzer::processNewOrExistingEState(EState& s) {
  return eStateSet.processNewOrExistingEState(s);
}

EStateSet::ProcessingResult Analyzer::processEState(EState& s) {
  return eStateSet.processEState(s);
}


EState Analyzer::transferFunction(Edge edge, const EState* eState) {
  assert(edge.source==eState->label);

  // we do not pass information on the local edge
  if(edge.type==EDGE_LOCAL) {
	EState noEState;
	noEState.label=NO_ESTATE;
	return noEState;
  }

  EState currentEState=*eState;
  State currentState=*currentEState.state;
  ConstraintSet cset=currentEState.constraints;
  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source);
  SgNode* targetNode=cfanalyzer->getNode(edge.target);
  assert(nextNodeToAnalyze1);

  // handle assert(0)
  if(isAssertExpr(nextNodeToAnalyze1)) {
	return createFailedAssertEState(currentEState,edge.target);
  }

  if(edge.type==EDGE_CALL) {
	// 1) obtain actual parameters from source
	// 2) obtain formal parameters from target
	// 3) eval each actual parameter and assign result to formal parameter in state
	// 4) create new eState

	// ad 1)
	SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source));
	assert(funCall);
	SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
	// ad 2)
	SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.target));
	SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
	assert(funDef);
	// ad 3)
	State newState=currentState;
	SgInitializedNamePtrList::iterator i=formalParameters.begin();
	SgExpressionPtrList::iterator j=actualParameters.begin();
	while(i!=formalParameters.end() || j!=actualParameters.end()) {
	  SgInitializedName* formalParameterName=*i;
	  VariableId formalParameterVarId=variableIdMapping.variableId(formalParameterName);
	  // VariableName varNameString=name->get_name();
	  SgExpression* actualParameterExpr=*j;

	  // check whether the actualy parameter is a single variable: In this case we can propagate the constraints of that variable to the formal parameter.
	  // pattern: call: f(x), callee: f(int y) => constraints of x are propagated to y
	  VariableId actualParameterVarId;
	  if(bool isActualParamterVar=ExprAnalyzer::variable(actualParameterExpr,actualParameterVarId)) {
		// propagate constraint from actualParamterVarId to formalParameterVarId
		cset.addEqVarVar(formalParameterVarId,actualParameterVarId);
	  }
	  // general case: the actual argument is an arbitrary expression (including a single variable)
	  SingleEvalResultConstInt evalResult=exprAnalyzer.evalConstInt(actualParameterExpr,currentEState);
	  newState[formalParameterVarId]=evalResult.value();
	  ++i;++j;
	}
	assert(i==formalParameters.end() && j==actualParameters.end()); // must hold if #fparams==#aparams (TODO: default values)
	// ad 4
	const State* newStatePtr=processNewOrExistingState(newState);
	return EState(edge.target,newStatePtr,cset);
  }

  // "return x;": add $return=eval() [but not for "return f();"]
  if(isSgReturnStmt(nextNodeToAnalyze1) && !SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
	SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);
	ConstraintSet cset=currentEState.constraints;
	State newState=analyzeAssignRhs(*(currentEState.state),
									variableIdMapping.createUniqueTemporaryVariableId(string("$return")),
									expr,
									cset);
	const State* newStatePtr=processNewOrExistingState(newState);
	return EState(edge.target,newStatePtr,cset);
  }

  // function exit node:
  if(getLabeler()->isFunctionExitLabel(edge.source)) {
	if(SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.source))) {
	  // 1) determine all local variables (including formal parameters) of function
	  // 2) delete all local variables from state
	  // 2a) remove variable from state
	  // 2b) remove all constraints concerning this variable
	  // 3) create new EState and return

	  // ad 1)
	  set<SgVariableDeclaration*> varDecls=SgNodeHelper::localVariableDeclarationsOfFunction(funDef);
	  // ad 2)
	  ConstraintSet cset=currentEState.constraints;
	  State newState=*(currentEState.state);
	  set<VariableId> localVars=determineVariableIdsOfVariableDeclarations(varDecls);
	  SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
	  set<VariableId> formalParams=determineVariableIdsOfSgInitializedNames(formalParamInitNames);
	  set<VariableId> vars=localVars+formalParams;
	  set<string> names=variableIdsToVariableNames(vars);

	  for(set<VariableId>::iterator i=vars.begin();i!=vars.end();++i) {
		VariableId varId=*i;
		newState.deleteVar(varId);
		cset.deleteConstraints(varId);
	  }
	  // ad 3)
	  const State* newStatePtr=processNewOrExistingState(newState);
	  return EState(edge.target,newStatePtr,cset);
	} else {
	  cerr << "FATAL ERROR: no function definition associated with function exit label."<<endl;
	  exit(1);
	}
  }

  if(getLabeler()->isFunctionCallReturnLabel(edge.source)) {
	// case 1: return f(); pass eState trough
	if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
	  EState newEState=currentEState;
	  newEState.label=edge.target;
	  return newEState;
	}
	// case 2: x=f(); bind variable x to value of $return
	if(SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(nextNodeToAnalyze1)) {
	  SgNode* lhs=SgNodeHelper::getLhs(SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1));
	  VariableId lhsVarId;
	  bool isLhsVar=ExprAnalyzer::variable(lhs,lhsVarId);
	  assert(isLhsVar); // must hold
	  State newState=*currentEState.state;
	  // we only create this variable here to be able to find an existing $return variable!
	  VariableId returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
	  AValue evalResult=newState[returnVarId].getValue();
	  newState[lhsVarId]=evalResult;

	  cset.duplicateConstConstraints(lhsVarId, returnVarId); // duplicate constraints of $return to lhsVar
	  newState.deleteVar(returnVarId); // remove $return from state
	  cset.deleteConstraints(returnVarId); // remove constraints of $return

	  const State* newStatePtr=processNewOrExistingState(newState);
	  return EState(edge.target,newStatePtr,cset);
	}
	// case 3: f(); remove $return from state (discard value)
	if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
	  State newState=*currentEState.state;
	  VariableId returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
	  newState.deleteVar(returnVarId);
	  cset.deleteConstraints(returnVarId); // remove constraints of $return
	  const State* newStatePtr=processNewOrExistingState(newState);
	  ConstraintSet cset=currentEState.constraints;
	  return EState(edge.target,newStatePtr,cset);

	}
  }
  if(edge.type==EDGE_EXTERNAL) {
	if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) ) {
	   string fName=SgNodeHelper::getFunctionName(funCall);
	   SgExpressionPtrList& actualParams=SgNodeHelper::getFunctionCallActualParameterList(funCall);
	   InputOutput newio;
	   if(fName=="scanf") {
		 if(actualParams.size()==2) {
		   SgAddressOfOp* addressOp=isSgAddressOfOp(actualParams[1]);
		   if(!addressOp) {
			 cerr<<"Error: unsupported scanf argument #2 (no address operator found). Currently scanf with exactly one variable of the form scanf(\"%d\",&v) is supported."<<endl;
			 exit(1);
		   }
		   SgVarRefExp* varRefExp=isSgVarRefExp(SgNodeHelper::getFirstChild(addressOp));
		   if(!varRefExp) {
			 cerr<<"Error: unsupported scanf argument #2 (no variable found). Currently scanf with exactly one variable of the form scanf(\"%d\",&v) is supported."<<endl;
			 exit(1);
		   }
		   // matched: SgAddressOfOp(SgVarRefExp())
		   SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
		   assert(sym);
		   VariableId varId=VariableId(sym);
		   newio.recordVariable(InputOutput::STDIN_VAR,varId);
		 } else {
		   cerr<<"Error: unsupported number of scanf arguments. Currently scanf with exactly one variable of the form scanf(\"%d\",&v) is supported."<<endl;
		   exit(1);
		 }
	   }
	   if(fName=="printf") {
		 if(actualParams.size()==2) {
		   SgVarRefExp* varRefExp=isSgVarRefExp(actualParams[1]);
		   if(!varRefExp) {
			 cerr<<"Error: unsupported print argument #2 (no variable found). Currently printf with exactly one variable of the form printf(\"...%d...\",v) is supported."<<endl;
			 exit(1);
		   }
		   SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
		   assert(sym);
		   VariableId varId=VariableId(sym);
		   newio.recordVariable(InputOutput::STDOUT_VAR,varId);
		   assert(newio.var==varId);
		   if(boolOptions["report-stdout"]) {
			 cout << "REPORT: stdout:"<<varId.toString()<<":"<<eState->toString()<<endl;
		   }

		 } else {
		   cerr<<"Error: unsupported number of printf arguments. Currently printf with exactly one variable of the form printf(\"...%d...\",v) is supported."<<endl;
		   exit(1);
		 }
	   }
	   if(fName=="fprintf") {
		 if(actualParams.size()==3) {
		   SgVarRefExp* varRefExp=isSgVarRefExp(actualParams[2]);
		   if(!varRefExp) {
			 cerr<<"Error: unsupported fprint argument #3 (no variable found). Currently printf with exactly one variable of the form fprintf(stream,\"...%d...\",v) is supported."<<endl;
			 exit(1);
		   }
		   SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
		   assert(sym);
		   VariableId varId=VariableId(sym);
		   newio.recordVariable(InputOutput::STDERR_VAR,varId);
		 } else {
		   cerr<<"Error: unsupported number of fprintf arguments. Currently printf with exactly one variable of the form fprintf(stream,\"...%d...\",v) is supported."<<endl;
		   exit(1);
		 }
	   }
	   // for all other external functions are use identity as transfer function
	   EState newEState=currentEState;
	   newEState.io=newio;
	   newEState.label=edge.target;
	   return newEState;
	}
  }
  // special case external call
  if(SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) 
	 ||edge.type==EDGE_EXTERNAL
	 ||edge.type==EDGE_CALLRETURN) {
	EState newEState=currentEState;
	newEState.label=edge.target;
	return newEState;
  }
  
  //cout << "INFO1: we are at "<<astTermWithNullValuesToString(nextNodeToAnalyze1)<<endl;
  if(SgVariableDeclaration* decl=isSgVariableDeclaration(nextNodeToAnalyze1)) {
	return analyzeVariableDeclaration(decl,currentEState, edge.target);
  }

  if(isSgExprStatement(nextNodeToAnalyze1)) {
	SgNode* nextNodeToAnalyze2=SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1);
	assert(nextNodeToAnalyze2);
	//cout << "INFO2: we are at "<<astTermWithNullValuesToString(nextNodeToAnalyze2)<<endl;
	if(edge.type==EDGE_TRUE || edge.type==EDGE_FALSE) {
	  SingleEvalResultConstInt evalResult=exprAnalyzer.evalConstInt(nextNodeToAnalyze2,currentEState);
	  if((evalResult.isTrue() && edge.type==EDGE_TRUE) || (evalResult.isFalse() && edge.type==EDGE_FALSE) || evalResult.isTop()) {
		// pass on EState
		EState newEState=evalResult.eState;
		newEState.label=edge.target;

		// merge with collected constraints of expr (exprConstraints), and invert for false branch
		if(edge.type==EDGE_TRUE) {
		  newEState.constraints=evalResult.eState.constraints+evalResult.exprConstraints;
		} else if(edge.type==EDGE_FALSE) {
		  ConstraintSet s1=evalResult.eState.constraints;
		  ConstraintSet s2=evalResult.exprConstraints.invertedConstraints();
		  newEState.constraints=s1+s2;
		}
		return newEState;
	  } else {
		// we determined not to be on an execution path, therefore return EState with NO_ESTATE
		EState noEState;
		noEState.label=NO_ESTATE;
		return noEState;
	  }

	}
	if(isSgConditionalExp(nextNodeToAnalyze2)) {
	  // we currently only handle ConditionalExpressions as used in asserts
	  ConstraintSet cset=currentEState.constraints;
	  State newState=*currentEState.state;
	  //SgNode* condition=SgNodeHelper::getCond(nextNodeToAnalyze2);
	  //SingleEvalResultConstInt evalResult=exprAnalyzer.evalConstInt(nextNodeToAnalyze2,currentEState);
	  // TODO: resolve condexp 
	  //Constraint c(Constraint::DEQ_VAR_CONST,string("$exit"), 1);
	  //cset.insert(c);
	  //stateSet.insert(newState);
	  const State* newStatePtr=processNewOrExistingState(newState);
	  return EState(edge.target,newStatePtr,cset);
	}

	if(isSgAssignOp(nextNodeToAnalyze2)) {
	  // analyze assignment and return new eState
	  // currentEState
	  ConstraintSet cset=currentEState.constraints;
	  State newState=analyzeAssignOp(*currentEState.state,nextNodeToAnalyze2,cset);
	  const State* newStatePtr=processNewOrExistingState(newState);
	  return EState(edge.target,newStatePtr,cset);
	}
  }
  // nothing to analyze, just create new eState (from same State) with target label of edge
  // can be same state if edge is a backedge to same cfg node
  EState newEState=currentEState;
  newEState.label=edge.target;
  return newEState;
}

void Analyzer::initializeSolver1(std::string functionToStartAt,SgNode* root) {
  std::string funtofind=functionToStartAt;
  MyAst completeast(root);
  startFunRoot=completeast.findFunctionByName(funtofind);
  if(startFunRoot==0) { 
    std::cerr << "Function '"<<funtofind<<"' not found.\n"; exit(1);
  }
  cout << "INIT: Initializing AST node info."<<endl;
  initAstNodeInfo(root);

  cout << "INIT: Creating Labeler."<<endl;
  Labeler* labeler= new Labeler(root);
  cout << "INIT: Creating CFAnalyzer."<<endl;
  cfanalyzer=new CFAnalyzer(labeler);
  cout << "INIT: Building CFG."<<endl;
  flow=cfanalyzer->flow(root);
  cout << "INIT: Intra-Flow OK. (size: " << flow.size() << " edges)"<<endl;
  InterFlow interFlow=cfanalyzer->interFlow(flow);
  cout << "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
  cfanalyzer->intraInterFlow(flow,interFlow);
  cout << "INIT: IntraInter-CFG OK. (size: " << flow.size() << " edges)"<<endl;

  // create empty state
  State emptyState;
  const State* emptyStateStored=processNewState(emptyState);
  assert(emptyStateStored);
  cout << "INIT: Empty state(stored): "<<emptyStateStored->toString()<<endl;
  assert(cfanalyzer);
  EState eState(cfanalyzer->getLabel(startFunRoot),emptyStateStored);
  
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
	  if(setOfUsedVars.find(variableIdMapping.variableId(*i))!=setOfUsedVars.end())
		eState=analyzeVariableDeclaration(*i,eState,eState.label);
	  else
		filteredVars++;
	}
	cout << "STATUS: Number of filtered variables for initial state: "<<filteredVars<<endl;
  } else {
	cout << "INIT: no global scope.";
  }	

  // TODO: delete global vars which are not used in the analyzed program (not necessary in PState)
  



  const EState* currentEState=processNewEState(eState);
  assert(currentEState);
  //cout << "INIT: "<<eStateSet.toString()<<endl;
  addToWorkList(currentEState);
  cout << "INIT: start state: "<<currentEState->toString()<<endl;
  cout << "INIT: finished."<<endl;
}

set<const EState*> Analyzer::transitionSourceEStateSetOfLabel(Label lab) {
  set<const EState*> eStateSet;
  for(TransitionGraph::iterator j=transitionGraph.begin();j!=transitionGraph.end();++j) {
	if((*j).source->label==lab)
	  eStateSet.insert((*j).source);
  }
  return eStateSet;
}

// TODO: x=x eliminates constraints of x but it should not.
State Analyzer::analyzeAssignRhs(State currentState,VariableId lhsVar, SgNode* rhs, ConstraintSet& cset) {
  assert(isSgExpression(rhs));
  bool isRhsIntVal=false;
  bool isRhsVar=false;
  AValue rhsIntVal=AType::Top();

  // TODO: -1 is OK, but not -(-1); yet.
  if(SgMinusOp* minusOp=isSgMinusOp(rhs)) {
	if(SgIntVal* intValNode=isSgIntVal(SgNodeHelper::getFirstChild(rhs))) {
	  // found integer on rhs
	  rhsIntVal=-((int)intValNode->get_value());
	  isRhsIntVal=true;
	}
  }

  // extracted info: isRhsIntVal:rhsIntVal 
  if(SgIntVal* intValNode=isSgIntVal(rhs)) {
	// found integer on rhs
	rhsIntVal=(int)intValNode->get_value();
	isRhsIntVal=true;
  }
  // allow single var on rhs
  if(SgVarRefExp* varRefExp=isSgVarRefExp(rhs)) {
	VariableId rhsVarId;
	isRhsVar=ExprAnalyzer::variable(rhs,rhsVarId);
	assert(isRhsVar);
	// x=y: constraint propagation for var1=var2 assignments
	// we do not perform this operation on assignments yet, as the constraint set could become inconsistent.
	//cset.addEqVarVar(lhsVar, rhsVarId);

	if(currentState.varExists(rhsVarId)) {
	  rhsIntVal=currentState[rhsVarId].getValue();
	} else {
	  cerr << "WARNING: access to variable "<<rhsVarId.longVariableName()<< "on rhs of assignment, but variable does not exist in state. Initializing with top."<<endl;
	  rhsIntVal=AType::Top();
	  isRhsIntVal=true;
	}
  }
  State newState=currentState;
  if(newState.varExists(lhsVar)) {
	if(!isRhsIntVal && !isRhsVar) {
	  rhsIntVal=AType::Top();
	}
	// we are using AValue here (and  operator== is overloaded for AValue==AValue)
	// for this comparison isTrue() is also false if any of the two operands is AType::Top()
	if( (newState[lhsVar].getValue()==rhsIntVal).isTrue() ) {
	  // update of existing variable with same value
	  // => no state change
	  return newState;
	} else {
	  // update of existing variable with new value
	  newState[lhsVar]=rhsIntVal;
	  if(!rhsIntVal.isTop() && !isRhsVar)
		cset.deleteConstraints(lhsVar);
	  return newState;
	}
  } else {
	// new variable with new value
	newState[lhsVar]=rhsIntVal;
	// no update of constraints because no constraints can exist for a new variable
	return newState;
  }
  // make sure, we only create/propagate contraints if a non-const value is assigned or if a variable is on the rhs.
  if(!rhsIntVal.isTop() && !isRhsVar)
	cset.deleteConstraints(lhsVar);
  return newState;
}

State Analyzer::analyzeAssignOp(State currentState,SgNode* node,ConstraintSet& cset) {
  if(node==0) {
	cerr << "WARNING: analyzeAssignOp: null pointer found (ignoring)"<<endl;
	return currentState;
  }
  State newState;
  assert(isSgAssignOp(node));
  bool isLhsVar=false;
  bool isRhsVar=false;
	
  VariableId lhsVar;

  SgNode* lhs=SgNodeHelper::getLhs(node);
  SgNode* rhs=SgNodeHelper::getRhs(node);
  isLhsVar=ExprAnalyzer::variable(lhs,lhsVar);
  if(isLhsVar) {
	newState=analyzeAssignRhs(currentState, lhsVar, rhs, cset);
	
	return newState;
  }
  cerr << "WARNING: analyzeAssignOp: unrecognized expression on lhs."<<endl;
  newState=currentState;
  return newState;
}

void Analyzer::initAstNodeInfo(SgNode* node) {
  MyAst ast(node);
  for(MyAst::iterator i=ast.begin();i!=ast.end();++i) {
	AstNodeInfo* attr=new AstNodeInfo();
	(*i)->addNewAttribute("info",attr);
  }
}

void Analyzer::generateAstNodeInfo(SgNode* node) {
  assert(node);
  if(!cfanalyzer) {
	cerr << "Error: DFAnalyzer: no cfanalyzer found."<<endl;
	exit(1);
  }
  MyAst ast(node);
  for(MyAst::iterator i=ast.begin().withoutNullValues();i!=ast.end();++i) {
	assert(*i);
	AstNodeInfo* attr=dynamic_cast<AstNodeInfo*>((*i)->getAttribute("info"));
	if(attr) {
	  if(cfanalyzer->getLabel(*i)>0) {
		attr->setLabel(cfanalyzer->getLabel(*i));
		attr->setInitialLabel(cfanalyzer->initialLabel(*i));
		attr->setFinalLabels(cfanalyzer->finalLabels(*i));
	  } else {
		(*i)->removeAttribute("info");
	  }
	} 
#if 0
	cout << "DEBUG:"<<(*i)->sage_class_name();
	if(attr) cout<<":"<<attr->toString();
	else cout<<": no attribute!"<<endl;
#endif
  }
}
