/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Analyzer.h"
#include "CollectionOperators.h"
#include "CommandLineOptions.h"
#include <fstream>
#include <unistd.h>

string color(string);

Analyzer::Analyzer():startFunRoot(0),cfanalyzer(0),_displayDiff(10000),_numberOfThreadsToUse(1) {
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

void Analyzer::printStatusMessage(bool forceDisplay) {
  // forceDisplay currently only turns on or off
#if 0
  if(isEmptyWorkList()) {
	//cerr << "Status: work list empty."<<endl;
	return;
  }
#endif

  {
	// report we are alife
  stringstream ss;
  if(forceDisplay) {
	ss  <<color("white")<<"Number of states/estates/trans/csets: ";
	ss  <<color("magenta")<<pstateSet.size()
		<<color("white")<<"/"
		<<color("cyan")<<estateSet.size()
		<<color("white")<<"/"
		<<color("blue")<<transitionGraph.size()
		<<color("white")<<"/"
		<<color("yellow")<<constraintSetMaintainer.size()
		<<color("white")<<"";
	ss<<endl;
	cout<<ss.str();
  }
}
}

void Analyzer::addToWorkList(const EState* estate) { 
#pragma omp critical
  {
  if(!estate) {
	cerr<<"INTERNAL ERROR: null pointer added to work list."<<endl;
	exit(1);
  }
  estateWorkList.push(estate); 
  }
}

const EState* Analyzer::processNewOrExisting(Label label, PState pstate, ConstraintSet cset) {
  const EState* newEStatePtr;
  const PState* newPStatePtr=processNewOrExisting(pstate);
  const ConstraintSet* newCSetPtr=processNewOrExisting(cset);
  EState newEState=EState(label,newPStatePtr,newCSetPtr);
  newEStatePtr=estateSet.processNewOrExisting(newEState);
  return newEStatePtr;
}

EState Analyzer::createEState(Label label, PState pstate, ConstraintSet cset) {
  const PState* newPStatePtr=processNewOrExisting(pstate);
  const ConstraintSet* newConstraintSetPtr=processNewOrExisting(cset);
  EState estate=EState(label,newPStatePtr,newConstraintSetPtr);
  return estate;
}
EState Analyzer::createEState(Label label, PState pstate, ConstraintSet cset, InputOutput io) {
  EState estate=createEState(label,pstate,cset);
  estate.io=io;
  return estate;
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

// We want to avoid calling critical sections from critical sections.
// therefore all worklist functions do not use each other.
bool Analyzer::isEmptyWorkList() { 
  bool res;
  #pragma omp critical
  {
  bool res=estateWorkList.size()==0;
  }
  return res;
}
const EState* Analyzer::topWorkList() {
  const EState* estate=0;
#pragma omp critical
  {
	if(estateWorkList.size()>0)
	  estate=estateWorkList.top();
  }
  return estate;
}
const EState* Analyzer::popWorkList() {
  const EState* estate=0;
  #pragma omp critical
  {
	if(estateWorkList.size()>0)
	  estate=estateWorkList.top();
	if(estate)
	  estateWorkList.pop();
  }
  return estate;
}
const EState* Analyzer::takeFromWorkList() {
  const EState* co=0;
#pragma omp critical
  {
  if(estateWorkList.size()>0) {
	co=estateWorkList.top();
	estateWorkList.pop();
  }
  }
  return co;
}

void Analyzer::runSolver1() {
  int Statusprint=_displayDiff;
  long prevStateSetSize=_displayDiff+1; // force immediate report at start
  omp_set_num_threads(_numberOfThreadsToUse);
  omp_set_dynamic(1);
  int threadNum;
  const EState* workVector[256];
  if(_numberOfThreadsToUse>256) {
	throw "Error: number of threads exceeds 256.";
  }
  while(1)
  {
  int workers;
  for(workers=0;workers<_numberOfThreadsToUse;++workers) {
	if(!(workVector[workers]=popWorkList()))
	  break;
  }
  if(workers==0)
	break; // we are done

  if((_displayDiff && (estateSet.size()-prevStateSetSize)>_displayDiff)) {
	printStatusMessage(true);
	prevStateSetSize=estateSet.size();
  }
  #pragma omp parallel for private(threadNum),shared(workVector)
  for(int j=0;j<workers;++j) {
	threadNum=omp_get_thread_num();
	const EState* currentEStatePtr=workVector[j];
	if(!currentEStatePtr) {
	  cerr<<"Error: thread "<<threadNum<<" finished prematurely. Bailing out. "<<endl;
	  assert(threadNum>=0 && threadNum<=_numberOfThreadsToUse);
	  exit(1);
	}
	assert(currentEStatePtr);

	Flow edgeSet=flow.outEdges(currentEStatePtr->label);
	//cerr << "DEBUG: edgeSet size:"<<edgeSet.size()<<endl;
#if 1
	// we can simplify this by adding the proper function to Flow.
	Flow::iterator i=edgeSet.begin();
	int edgeNum=edgeSet.size();
	vector<const Edge*> edgeVec(edgeNum);
	for(int edge_i=0;edge_i<edgeNum;++edge_i) {
	  edgeVec[edge_i]=&(*i++);
	}
    #pragma omp parallel for
	for(int edge_i=0; edge_i<edgeNum;edge_i++) {
	  Edge e=*edgeVec[edge_i]; // *i
#else
	for(Flow::iterator i=edgeSet.begin();i!=edgeSet.end();++i) {
	  Edge e=*i;
#endif
	  list<EState> newEStateList;
 	  newEStateList=transferFunction(e,currentEStatePtr);
	  for(list<EState>::iterator nesListIter=newEStateList.begin();
		  nesListIter!=newEStateList.end();
		  ++nesListIter) {
		EState newEState=*nesListIter;
		if(newEState.label!=NO_ESTATE && (!newEState.constraints()->disequalityExists()) &&(!isFailedAssertEState(&newEState))) {
		  HSetMaintainer<EState,EStateHashFun>::ProcessingResult pres=estateSet.process(newEState);
		  const EState* newEStatePtr=pres.second;
		  if(pres.first==true)
			addToWorkList(newEStatePtr);			
		  recordTransition(currentEStatePtr,e,newEStatePtr);
		}
		if(newEState.label!=NO_ESTATE && (!newEState.constraints()->disequalityExists()) && (isFailedAssertEState(&newEState))) {
		  // failed-assert end-state: do not add to work list but do add it to the transition graph
		  const EState* newEStatePtr;
		  newEStatePtr=processNewOrExisting(newEState);
		  recordTransition(currentEStatePtr,e,newEStatePtr);		

		  if(boolOptions["report-failed-assert"]) {
          #pragma omp cricitcal
		  {
			cout << "REPORT: failed-assert: "<<newEStatePtr->toString()<<endl;
		  }
		  }
		  if(_csv_assert_live_file.size()>0) {
			string name=labelNameOfAssertLabel(currentEStatePtr->label);
			if(name=="globalError")
			  name="error_60";
			name=name.substr(6,name.size()-6);
			std::ofstream fout;
			// csv_assert_live_file is the member-variable of analyzer
          #pragma omp cricitcal
		  {
			fout.open(_csv_assert_live_file.c_str(),ios::app);    // open file for appending
			assert (!fout.fail( ));
			fout << name << ",yes,9"<<endl;
			//cout << "REACHABLE ASSERT FOUND: "<< name << ",yes,9"<<endl;

			fout.close(); 
		  }
		  } // if
		}
		if(newEState.label==NO_STATE) {
		  //cerr << "DEBUG: NO_ESTATE (transition not recorded)"<<endl;
		  //cerr << "INFO: found final state."<<endl;
		}
	  } // end of loop on transfer function return-estates
	} // edgeset-parallel for
	} // worklist-parallel for
  } // while
  printStatusMessage(true);
  cout << "analysis finished (worklist is empty)."<<endl;
}

const EState* Analyzer::addToWorkListIfNew(EState estate) {
  EStateSet::ProcessingResult res=process(estate);
  if(res.first==true) {
	const EState* newEStatePtr=res.second;
	assert(newEStatePtr);
	addToWorkList(newEStatePtr);
	return newEStatePtr;
  } else {
	//cout << "DEBUG: EState already exists. Not added:"<<estate.toString()<<endl;
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
		PState newPState=analyzeAssignRhs(*currentEState.pstate,initDeclVarId,rhs,cset);
		return createEState(targetLabel,newPState,cset);
	  } else {
		//cout << "no initializer (OK)."<<endl;
		PState newPState=*currentEState.pstate;
		newPState[initDeclVarId]=AType::Top();
		return createEState(targetLabel,newPState,cset);
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

bool Analyzer::isFailedAssertEState(const EState* estate) {
  return estate->io.op==InputOutput::FAILED_ASSERT;
}

EState Analyzer::createFailedAssertEState(EState estate, Label target) {
  	  EState newEState=estate;
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

const PState* Analyzer::processNew(PState& s) {
  return pstateSet.processNew(s);
}
const PState* Analyzer::processNewOrExisting(PState& s) {
  return pstateSet.processNewOrExisting(s);
}

const EState* Analyzer::processNew(EState& s) {
  return estateSet.processNew(s);
}

const EState* Analyzer::processNewOrExisting(EState& s) {
  return estateSet.processNewOrExisting(s);
}

const ConstraintSet* Analyzer::processNewOrExisting(ConstraintSet& cset) {
  return constraintSetMaintainer.processNewOrExisting(cset);
}

EStateSet::ProcessingResult Analyzer::process(EState& s) {
  return estateSet.process(s);
}

list<EState> elistify(EState res) {
  //assert(res.state);
  //assert(res.constraints());
  list<EState> resList;
  resList.push_back(res);
  return resList;
}

list<EState> Analyzer::transferFunction(Edge edge, const EState* estate) {
  assert(edge.source==estate->label);
  // we do not pass information on the local edge
  if(edge.type==EDGE_LOCAL) {
	EState noEState;
	noEState.label=NO_ESTATE;
	// TODO: investigate: with could also return an empty list here
	return elistify(noEState);
  }
  EState currentEState=*estate;
  PState currentPState=*currentEState.pstate;
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
	// 4) create new estate

	// ad 1)
	SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(getLabeler()->getNode(edge.source));
	assert(funCall);
	SgExpressionPtrList& actualParameters=SgNodeHelper::getFunctionCallActualParameterList(funCall);
	// ad 2)
	SgFunctionDefinition* funDef=isSgFunctionDefinition(getLabeler()->getNode(edge.target));
	SgInitializedNamePtrList& formalParameters=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
	assert(funDef);
	// ad 3)
	PState newPState=currentPState;
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
	  // above evalConstInt does not use constraints (par3==false). Therefore top vars remain top vars (which is what we want here)
	  newPState[formalParameterVarId]=evalResult.value();
	  ++i;++j;
	}
	// assert must hold if #formal-params==#actual-params (TODO: default values)
	assert(i==formalParameters.end() && j==actualParameters.end()); 
	// ad 4
	return elistify(createEState(edge.target,newPState,cset));
  }
  // "return x;": add $return=eval() [but not for "return f();"]
  if(isSgReturnStmt(nextNodeToAnalyze1) && !SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(nextNodeToAnalyze1)) {
	SgNode* expr=SgNodeHelper::getFirstChild(nextNodeToAnalyze1);
	ConstraintSet cset=*currentEState.constraints();
	PState newPState=analyzeAssignRhs(*(currentEState.pstate),
									variableIdMapping.createUniqueTemporaryVariableId(string("$return")),
									expr,
									cset);
	return elistify(createEState(edge.target,newPState,cset));
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
	  PState newPState=*(currentEState.pstate);
	  set<VariableId> localVars=determineVariableIdsOfVariableDeclarations(varDecls);
	  SgInitializedNamePtrList& formalParamInitNames=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
	  set<VariableId> formalParams=determineVariableIdsOfSgInitializedNames(formalParamInitNames);
	  set<VariableId> vars=localVars+formalParams;
	  set<string> names=variableIdsToVariableNames(vars);

	  for(set<VariableId>::iterator i=vars.begin();i!=vars.end();++i) {
		VariableId varId=*i;
		newPState.deleteVar(varId);
		cset.removeAllConstraintsOfVar(varId);
	  }
	  // ad 3)
	  return elistify(createEState(edge.target,newPState,cset));
	} else {
	  cerr << "FATAL ERROR: no function definition associated with function exit label."<<endl;
	  exit(1);
	}
  }
  if(getLabeler()->isFunctionCallReturnLabel(edge.source)) {
	// case 1: return f(); pass estate trough
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
	  PState newPState=*currentEState.pstate;
	  // we only create this variable here to be able to find an existing $return variable!
	  VariableId returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
	  AValue evalResult=newPState[returnVarId].getValue();
	  newPState[lhsVarId]=evalResult;

	  cset.addAssignEqVarVar(lhsVarId,returnVarId);

	  newPState.deleteVar(returnVarId); // remove $return from state
	  cset.removeAllConstraintsOfVar(returnVarId); // remove constraints of $return

	  return elistify(createEState(edge.target,newPState,cset));
	}
	// case 3: f(); remove $return from state (discard value)
	if(SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(nextNodeToAnalyze1)) {
	  PState newPState=*currentEState.pstate;
	  VariableId returnVarId=variableIdMapping.createUniqueTemporaryVariableId(string("$return"));
	  newPState.deleteVar(returnVarId);
	  cset.removeAllConstraintsOfVar(returnVarId); // remove constraints of $return
	  //ConstraintSet cset=*currentEState.constraints; ???
	  return elistify(createEState(edge.target,newPState,cset));
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
		   PState newPState=*currentEState.pstate;
		   ConstraintSet newCSet=*currentEState.constraints();
		   if(boolOptions["update-input-var"]) {
			 newCSet.removeAllConstraintsOfVar(varId);
			 newPState[varId]=AType::Top();
		   }
		   newio.recordVariable(InputOutput::STDIN_VAR,varId);
		   return elistify(createEState(edge.target,newPState,newCSet,newio));
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
			 cout << "REPORT: stdout:"<<varId.toString()<<":"<<estate->toString()<<endl;
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
	PState newPState;
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
		  newPState=*evalResult.estate.pstate;
		  // merge with collected constraints of expr (exprConstraints), and invert for false branch
		  if(edge.type==EDGE_TRUE) {
			newCSet=*evalResult.estate.constraints()+evalResult.exprConstraints;
		  } else if(edge.type==EDGE_FALSE) {
			ConstraintSet s1=*evalResult.estate.constraints();
			ConstraintSet s2=evalResult.exprConstraints;
			newCSet=s1+s2;
		  }
		} else {
		  // we determined not to be on an execution path, therefore return EState with NO_ESTATE
		  newLabel=NO_ESTATE;
		}
		// build LIST of results of newEState
		if(newLabel!=NO_ESTATE)
		  newEStateList.push_back(createEState(newLabel,newPState,newCSet));
	  }
	  // return LIST
	  return newEStateList;
	}
	if(isSgConditionalExp(nextNodeToAnalyze2)) {
	  cerr<<"Error: found conditional expression outside assert. We do not support this form yet."<<endl;
	  exit(1);
	  // we currently only handle ConditionalExpressions as used in asserts (handled above)
	  ConstraintSet cset=*currentEState.constraints();
	  PState newPState=*currentEState.pstate;
	  return elistify(createEState(edge.target,newPState,cset));
	}

	if(isSgAssignOp(nextNodeToAnalyze2)) {
	  SgNode* lhs=SgNodeHelper::getLhs(nextNodeToAnalyze2);
	  SgNode* rhs=SgNodeHelper::getRhs(nextNodeToAnalyze2);
	  list<SingleEvalResultConstInt> res=exprAnalyzer.evalConstInt(rhs,currentEState,true,true);
	  list<EState> estateList;
	  for(list<SingleEvalResultConstInt>::iterator i=res.begin();i!=res.end();++i) {
		VariableId lhsVar;
		bool isLhsVar=ExprAnalyzer::variable(lhs,lhsVar);
		if(isLhsVar) {
		  EState estate=(*i).estate;
		  PState newPState=*estate.pstate;
		  ConstraintSet cset=*estate.constraints();
		  newPState[lhsVar]=(*i).result;
		  if(!(*i).result.isTop())
			cset.removeAllConstraintsOfVar(lhsVar);
		  estateList.push_back(createEState(edge.target,newPState,cset));
		} else {
		  cerr << "Error: transferfunction:SgAssignOp: unrecognized expression on lhs."<<endl;
		  exit(1);
		}
	  }
	  return estateList;
	}
  }
  // nothing to analyze, just create new estate (from same State) with target label of edge
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
  cout << "INIT: Building CFGs."<<endl;
  flow=cfanalyzer->flow(root);
  if(boolOptions["reduce-cfg"]) {
	int cnt=cfanalyzer->reduceBlockBeginNodes(flow);
	cout << "INIT: CFG reduction OK. (eliminated "<<cnt<<" nodes)"<<endl;
  }
  cout << "INIT: Intra-Flow OK. (size: " << flow.size() << " edges)"<<endl;
  InterFlow interFlow=cfanalyzer->interFlow(flow);
  cout << "INIT: Inter-Flow OK. (size: " << interFlow.size()*2 << " edges)"<<endl;
  cfanalyzer->intraInterFlow(flow,interFlow);
  cout << "INIT: IntraInter-CFG OK. (size: " << flow.size() << " edges)"<<endl;

  // create empty state
  PState emptyPState;
  const PState* emptyPStateStored=processNew(emptyPState);
  assert(emptyPStateStored);
  cout << "INIT: Empty state(stored): "<<emptyPStateStored->toString()<<endl;
  assert(cfanalyzer);
  ConstraintSet cset;
  const ConstraintSet* emptycsetstored=constraintSetMaintainer.processNewOrExisting(cset);
  Label startLabel=cfanalyzer->getLabel(startFunRoot);
  transitionGraph.setStartLabel(startLabel);
  EState estate(startLabel,emptyPStateStored,emptycsetstored);
  
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
		estate=analyzeVariableDeclaration(*i,estate,estate.label);
	  else
		filteredVars++;
	}
	cout << "STATUS: Number of filtered variables for initial pstate: "<<filteredVars<<endl;
  } else {
	cout << "INIT: no global scope.";
  }	

  const EState* currentEState=processNew(estate);
  assert(currentEState);
  //cout << "INIT: "<<eStateSet.toString()<<endl;
  addToWorkList(currentEState);
  cout << "INIT: start state: "<<currentEState->toString()<<endl;
  cout << "INIT: finished."<<endl;
}

set<const EState*> Analyzer::transitionSourceEStateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=transitionGraph.begin();j!=transitionGraph.end();++j) {
	if((*j).source->label==lab)
	  estateSet.insert((*j).source);
  }
  return estateSet;
}

// TODO: this function is obsolete (to delete)
// TODO: x=x eliminates constraints of x but it should not.
PState Analyzer::analyzeAssignRhs(PState currentPState,VariableId lhsVar, SgNode* rhs, ConstraintSet& cset) {
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

	if(currentPState.varExists(rhsVarId)) {
	  rhsIntVal=currentPState[rhsVarId].getValue();
	} else {
	  cerr << "WARNING: access to variable "<<rhsVarId.longVariableName()<< "on rhs of assignment, but variable does not exist in state. Initializing with top."<<endl;
	  rhsIntVal=AType::Top();
	  isRhsIntVal=true;
	}
  }
  PState newPState=currentPState;
  if(newPState.varExists(lhsVar)) {
	if(!isRhsIntVal && !isRhsVar) {
	  rhsIntVal=AType::Top();
	}
	// we are using AValue here (and  operator== is overloaded for AValue==AValue)
	// for this comparison isTrue() is also false if any of the two operands is AType::Top()
	if( (newPState[lhsVar].getValue()==rhsIntVal).isTrue() ) {
	  // update of existing variable with same value
	  // => no state change
	  return newPState;
	} else {
	  // update of existing variable with new value
	  newPState[lhsVar]=rhsIntVal;
	  if(!rhsIntVal.isTop() && !isRhsVar)
		cset.removeAllConstraintsOfVar(lhsVar);
	  return newPState;
	}
  } else {
	// new variable with new value
	newPState[lhsVar]=rhsIntVal;
	// no update of constraints because no constraints can exist for a new variable
	return newPState;
  }
  // make sure, we only create/propagate contraints if a non-const value is assigned or if a variable is on the rhs.
  if(!rhsIntVal.isTop() && !isRhsVar)
	cset.removeAllConstraintsOfVar(lhsVar);
  return newPState;
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
