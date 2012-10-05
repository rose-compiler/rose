/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Analyzer.h"
#include "CollectionOperators.h"
#include "CommandLineOptions.h"
#include <fstream>

string color(string);

Analyzer::Analyzer():startFunRoot(0),cfanalyzer(0){
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
  transitionGraph.add(Transition(sourceState,e,targetState));
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
		<<color("white")<<"/"
		<<color("yellow")<<constraintSetMaintainer.size()
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

const EState* Analyzer::processNewOrExisting(Label label, State state, ConstraintSet cset) {
  const State* newStatePtr=processNewOrExisting(state);
  const ConstraintSet* newPtr=processNewOrExisting(cset);
  EState newEState=EState(label,newStatePtr,newPtr);
  const EState* newEStatePtr=eStateSet.processNewOrExisting(newEState);
  return newEStatePtr;
}

EState Analyzer::createEState(Label label, State state, ConstraintSet cset) {
  const State* newStatePtr=processNewOrExisting(state);
  const ConstraintSet* newConstraintSetPtr=processNewOrExisting(cset);
  EState eState=EState(label,newStatePtr,newConstraintSetPtr);
  return eState;
}
EState Analyzer::createEState(Label label, State state, ConstraintSet cset, InputOutput io) {
  EState eState=createEState(label,state,cset);
  eState.io=io;
  return eState;
}

bool Analyzer::isLTLrelevantLabel(Label label) {
  Labeler* lab=getLabeler();
  if(lab->isFunctionEntryLabel(label)
	 || lab->isFunctionExitLabel(label)
	 || lab->isFunctionCallLabel(label)
	 || lab->isFunctionCallReturnLabel(label)
	 ){
	return true;
  }
  if(SgNodeHelper::isLoopCond(lab->getNode(label))) {
	return true;
  }
  if(SgNodeHelper::isCond(lab->getNode(label))) {
	return true;
  }
#if 0
  if(lab->isBlockBeginLabel(label)||lab->isBlockEndLabel(label))
	return false;
  // assignment
  if(SgExprStatement* exprStmt=isSgExprStatement(getLabeler()->getNode(label))) {
	SgNode* node=SgNodeHelper::getExprStmtChild(exprStmt);
	if(isSgAssignOp(node)) {
	  if(isSgVarRefExp(SgNodeHelper::getLhs(node)) && isSgIntVal(SgNodeHelper::getRhs(node))) {
		cout << "DEBUG: ltl-irrelevant node: "<<SgNodeHelper::nodeToString(node)<<endl;
		return false;
	  }
	}
  }
#endif
  return false;
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
	  list<EState> newEStateList=transferFunction(*i,currentEStatePtr);
	  for(list<EState>::iterator nesListIter=newEStateList.begin();
		  nesListIter!=newEStateList.end();
		  ++nesListIter) {
		EState newEState=*nesListIter;
		if(newEState.label!=NO_ESTATE && (!newEState.constraints()->deqConstraintExists()) &&(!isFailedAssertEState(&newEState))) {
		  const EState* newEStatePtr=addToWorkListIfNew(newEState);
		  recordTransition(currentEStatePtr,*i,newEStatePtr);
		}
		if(newEState.label!=NO_ESTATE && (!newEState.constraints()->deqConstraintExists()) && (isFailedAssertEState(&newEState))) {
		  // failed-assert end-state: do not add to work list but do add it to the transition graph
		  const EState* newEStatePtr=processNewOrExisting(newEState);
		  recordTransition(currentEStatePtr,*i,newEStatePtr);		
		  if(boolOptions["report-failed-assert"]) {
			cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
		  }
		  if(_csv_assert_live_file.size()>0) {
			string name=labelNameOfAssertLabel(currentEStatePtr->label);
			if(name=="globalError")
			  name="error_60";
			name=name.substr(6,name.size()-6);
			std::ofstream fout;
			// csv_assert_live_file is the member-variable of analyzer
			fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
			assert (!fout.fail( ));
			fout << name << ",yes,9"<<endl;
			cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;

			fout.close(); 
		  }
		}
		if(newEState.label==NO_STATE) {
		  //cerr << "DEBUG: NO_ESTATE (transition not recorded)"<<endl;
		  //cerr << "INFO: found final state."<<endl;
		}
	  } // end of loop on transfer function return-estates
	}
  }
  displayDiff=0;
  printStatusMessage();
  cout << "analysis finished (worklist is empty)."<<endl;
}

const EState* Analyzer::addToWorkListIfNew(EState eState) {
  EStateSet::ProcessingResult res=process(eState);
  if(res.first==true) {
	const EState* newEStatePtr=res.second;
	assert(newEStatePtr);
	addToWorkList(newEStatePtr);
	return newEStatePtr;
  } else {
	//cout << "DEBUG: State already exists. Not added:"<<eState.toString()<<endl;
	const EState* existingEStatePtr=res.second;
	assert(existingEStatePtr);
	return existingEStatePtr;
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
	  ConstraintSet cset=*currentEState.constraints();
	  if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
		//cout << "initalizer found:"<<endl;
		SgExpression* rhs=assignInitializer->get_operand_i();
		State newState=analyzeAssignRhs(*currentEState.state,initDeclVarId,rhs,cset);
		return createEState(targetLabel,newState,cset);
	  } else {
		//cout << "no initializer (OK)."<<endl;
		State newState=*currentEState.state;
		newState[initDeclVarId]=AType::Top();
		return createEState(targetLabel,newState,cset);
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

list<SgNode*> Analyzer::listOfAssertNodes(SgProject* root) {
  list<SgNode*> assertNodes;
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();
	  i!=funDefs.end();
	  ++i) {
	MyAst ast(*i);
	for(MyAst::iterator j=ast.begin();j!=ast.end();++j) {
	  if(isAssertExpr(*j)) {
		assertNodes.push_back(*j);
	  }
	}
  }
  return assertNodes;
}

list<pair<SgLabelStatement*,SgNode*> > Analyzer::listOfLabeledAssertNodes(SgProject* root) {
  list<pair<SgLabelStatement*,SgNode*> > assertNodes;
  list<SgFunctionDefinition*> funDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=funDefs.begin();
	  i!=funDefs.end();
	  ++i) {
	MyAst ast(*i);
	MyAst::iterator prev=ast.begin();
	for(MyAst::iterator j=ast.begin();j!=ast.end();++j) {
	  if(isAssertExpr(*j)) {
		if(prev!=j && isSgLabelStatement(*prev)) {
		  SgLabelStatement* labStmt=isSgLabelStatement(*prev);
		  assertNodes.push_back(make_pair(labStmt,*j));
		}
	  }
	  prev=j;
	}
  }
  return assertNodes;
}

const State* Analyzer::processNew(State& s) {
  return stateSet.processNew(s);
}

const State* Analyzer::processNewOrExisting(State& s) {
  return stateSet.processNewOrExisting(s);
}

const EState* Analyzer::processNew(EState& s) {
  return eStateSet.processNew(s);
}

const EState* Analyzer::processNewOrExisting(EState& s) {
  return eStateSet.processNewOrExisting(s);
}

const ConstraintSet* Analyzer::processNewOrExisting(ConstraintSet& cset) {
  return constraintSetMaintainer.processNewOrExisting(cset);
}

EStateSet::ProcessingResult Analyzer::process(EState& s) {
  return eStateSet.process(s);
}

list<EState> elistify(EState res) {
  //assert(res.state);
  //assert(res.constraints());
  list<EState> resList;
  resList.push_back(res);
  return resList;
}

list<EState> Analyzer::transferFunction(Edge edge, const EState* eState) {
  assert(edge.source==eState->label);
  // we do not pass information on the local edge
  if(edge.type==EDGE_LOCAL) {
	EState noEState;
	noEState.label=NO_ESTATE;
	// TODO: return empty list?
	return elistify(noEState);
  }
  EState currentEState=*eState;
  State currentState=*currentEState.state;
  ConstraintSet cset=*currentEState.constraints();
  // 1. we handle the edge as outgoing edge
  SgNode* nextNodeToAnalyze1=cfanalyzer->getNode(edge.source);
  SgNode* targetNode=cfanalyzer->getNode(edge.target);
  assert(nextNodeToAnalyze1);
  // handle assert(0)
  if(isAssertExpr(nextNodeToAnalyze1)) {
	return elistify(createFailedAssertEState(currentEState,edge.target));
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
	  assert(formalParameterName);
	  VariableId formalParameterVarId=variableIdMapping.variableId(formalParameterName);
	  // VariableName varNameString=name->get_name();
	  SgExpression* actualParameterExpr=*j;
	  assert(actualParameterExpr);
	  // check whether the actualy parameter is a single variable: In this case we can propagate the constraints of that variable to the formal parameter.
	  // pattern: call: f(x), callee: f(int y) => constraints of x are propagated to y
	  VariableId actualParameterVarId;
	  assert(actualParameterExpr);
	  if(bool isActualParamterVar=ExprAnalyzer::variable(actualParameterExpr,actualParameterVarId)) {
		// propagate constraint from actualParamterVarId to formalParameterVarId
		cset.addAssignEqVarVar(formalParameterVarId,actualParameterVarId);
	  }
	  // general case: the actual argument is an arbitrary expression (including a single variable)
	  // we use for the third parameter "false": do not use constraints when extracting values.
	  // Consequently, formalparam=actualparam remains top, even if constraints are available, which
	  // would allow to extract a constant value (or a range (when relational constraints are added)).
	  list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalConstInt(actualParameterExpr,currentEState,false, true);
	  assert(evalResultList.size()>0);
	  list<SingleEvalResultConstInt>::iterator resultListIter=evalResultList.begin();
	  SingleEvalResultConstInt evalResult=*resultListIter;
	  if(evalResultList.size()>1) {
		cerr<<"Error: We currently do not support multi-state generating operators in function call parameters (yet)."<<endl;
		exit(1);
	  }
	  //TODO: investigate: this turns top variables into variables with concrete value in state - do we want this?
	  newState[formalParameterVarId]=evalResult.value();
	  ++i;++j;
	}
	assert(i==formalParameters.end() && j==actualParameters.end()); // must hold if #fparams==#aparams (TODO: default values)
	// ad 4
	return elistify(createEState(edge.target,newState,cset));
  }
  // "return x;": add $return=eval() [but not for "return f();"]
  if(isSgReturnStmt(nextNodeToAnalyze1) && !SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
	SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);
	ConstraintSet cset=*currentEState.constraints();
	State newState=analyzeAssignRhs(*(currentEState.state),
									variableIdMapping.createUniqueTemporaryVariableId(string("$return")),
									expr,
									cset);
	return elistify(createEState(edge.target,newState,cset));
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
	  ConstraintSet cset=*currentEState.constraints();
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
	  return elistify(createEState(edge.target,newState,cset));
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
	  return elistify(newEState);
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

	  cset.deleteAndMoveConstConstraints(returnVarId,lhsVarId); // duplicate constraints of $return to lhsVar
	  newState.deleteVar(returnVarId); // remove $return from state
	  cset.deleteConstraints(returnVarId); // remove constraints of $return

	  return elistify(createEState(edge.target,newState,cset));
	}
	// case 3: f(); remove $return from state (discard value)
	if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
	  State newState=*currentEState.state;
	  VariableId returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
	  newState.deleteVar(returnVarId);
	  cset.deleteConstraints(returnVarId); // remove constraints of $return
	  //ConstraintSet cset=*currentEState.constraints; ???
	  return elistify(createEState(edge.target,newState,cset));
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

		   // update state (remove all existing constraint on that variable and set it to top)
		   State newState=*currentEState.state;
		   ConstraintSet newCSet=*currentEState.constraints();
		   if(boolOptions["update-input-var"]) {
			 newState[varId]=AType::Top();
			 newCSet.deleteConstraints(varId);
		   }
		   newio.recordVariable(InputOutput::STDIN_VAR,varId);
		   return elistify(createEState(edge.target,newState,newCSet,newio));
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
	   // for all other external functions we use identity as transfer function
	   EState newEState=currentEState;
	   newEState.io=newio;
	   newEState.label=edge.target;
	   return elistify(newEState);
	}
  }
  // special case external call
  if(SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) 
	 ||edge.type==EDGE_EXTERNAL
	 ||edge.type==EDGE_CALLRETURN) {
	EState newEState=currentEState;
	newEState.label=edge.target;
	return elistify(newEState);
  }
  
  //cout << "INFO1: we are at "<<astTermWithNullValuesToString(nextNodeToAnalyze1)<<endl;
  if(SgVariableDeclaration* decl=isSgVariableDeclaration(nextNodeToAnalyze1)) {
	return elistify(analyzeVariableDeclaration(decl,currentEState, edge.target));
  }

  if(isSgExprStatement(nextNodeToAnalyze1)) {
	SgNode* nextNodeToAnalyze2=SgNodeHelper::getExprStmtChild(nextNodeToAnalyze1);
	assert(nextNodeToAnalyze2);
	Label newLabel;
	State newState;
	ConstraintSet newCSet;
	if(edge.type==EDGE_TRUE || edge.type==EDGE_FALSE) {
	  list<SingleEvalResultConstInt> evalResultList=exprAnalyzer.evalConstInt(nextNodeToAnalyze2,currentEState,true,true);
	  //assert(evalResultList.size()==1);
	  list<EState> newEStateList;
	  for(list<SingleEvalResultConstInt>::iterator i=evalResultList.begin();
		  i!=evalResultList.end();
		  ++i) {
		SingleEvalResultConstInt evalResult=*i;
		if((evalResult.isTrue() && edge.type==EDGE_TRUE) || (evalResult.isFalse() && edge.type==EDGE_FALSE) || evalResult.isTop()) {
		  // pass on EState
		  newLabel=edge.target;
		  newState=*evalResult.eState.state;
		  // merge with collected constraints of expr (exprConstraints), and invert for false branch
		  if(edge.type==EDGE_TRUE) {
			newCSet=*evalResult.eState.constraints()+evalResult.exprConstraints;
		  } else if(edge.type==EDGE_FALSE) {
			ConstraintSet s1=*evalResult.eState.constraints();
			//ConstraintSet s2=evalResult.exprConstraints.invertedConstraints();
			ConstraintSet s2=evalResult.exprConstraints;
			newCSet=s1+s2;
		  }
		} else {
		  // we determined not to be on an execution path, therefore return EState with NO_ESTATE
		  newLabel=NO_ESTATE;
		}
		// build LIST of results of newEState
		if(newLabel!=NO_ESTATE)
		  newEStateList.push_back(createEState(newLabel,newState,newCSet));
	  }
	  // return LIST
	  return newEStateList;
	}
	if(isSgConditionalExp(nextNodeToAnalyze2)) {
	  cerr<<"Error: found conditional expression outside assert. We do not support this form yet."<<endl;
	  exit(1);
	  // we currently only handle ConditionalExpressions as used in asserts (handled above)
	  ConstraintSet cset=*currentEState.constraints();
	  State newState=*currentEState.state;
	  //SgNode* condition=SgNodeHelper::getCond(nextNodeToAnalyze2);
	  //SingleEvalResultConstInt evalResult=exprAnalyzer.evalConstInt(nextNodeToAnalyze2,currentEState);
	  // TODO: resolve condexp 
	  //Constraint c(Constraint::DEQ_VAR_CONST,string("$exit"), 1);
	  //cset.insert(c);
	  //stateSet.insert(newState);
	  return elistify(createEState(edge.target,newState,cset));
	}

	if(isSgAssignOp(nextNodeToAnalyze2)) {
	  SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
	  SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
	  list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(rhs,currentEState,true,true);
	  list<EState> eStateList;
	  for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
		VariableId lhsVar;
		bool isLhsVar=ExprAnalyzer::variable(lhs,lhsVar);
		if(isLhsVar) {
		  EState eState=(*i).eState;
		  State newState=*eState.state;
		  ConstraintSet cset=*eState.constraints();
		  newState[lhsVar]=(*i).result;
		  if(!(*i).result.isTop())
			cset.deleteConstraints(lhsVar);
		  eStateList.push_back(createEState(edge.target,newState,cset));
		} else {
		  cerr << "Error: transferfunction:SgAssignOp: unrecognized expression on lhs."<<endl;
		  exit(1);
		}
	  }
	  return eStateList;
	}
  }
  // nothing to analyze, just create new eState (from same State) with target label of edge
  // can be same state if edge is a backedge to same cfg node
  EState newEState=currentEState;
  newEState.label=edge.target;
  return elistify(newEState);
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
  const State* emptyStateStored=processNew(emptyState);
  assert(emptyStateStored);
  cout << "INIT: Empty state(stored): "<<emptyStateStored->toString()<<endl;
  assert(cfanalyzer);
  ConstraintSet cset;
  const ConstraintSet* emptycsetstored=constraintSetMaintainer.processNewOrExisting(cset);
  EState eState(cfanalyzer->getLabel(startFunRoot),emptyStateStored,emptycsetstored);
  
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

  const EState* currentEState=processNew(eState);
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

// TODO: this function is obsolete (to delete)
// TODO: x=x eliminates constraints of x but it should not.
State Analyzer::analyzeAssignRhs(State currentState,VariableId lhsVar, SgNode* rhs, ConstraintSet& cset) {
  assert(isSgExpression(rhs));
  AValue rhsIntVal=AType::Top();
  bool isRhsIntVal=false;
  bool isRhsVar=false;

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
