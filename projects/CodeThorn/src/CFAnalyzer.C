/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "CFAnalyzer.h"
#include "Labeler.h"
#include "AstTerm.h"

using namespace CodeThorn;

Edge::Edge():source(0),target(0){
}
Edge::Edge(Label source0,Label target0):source(source0),target(target0){
  // _types is an empty set by default (we may want to use EDGE_UNKNOWN instead)
}
Edge::Edge(Label source0,EdgeType et,Label target0):source(source0),target(target0){
  _types.insert(et);
}
Edge::Edge(Label source0,set<EdgeType> tset,Label target0):source(source0),target(target0){
  _types=tset;
}

bool Edge::isType(EdgeType et) const {
  if(et==EDGE_UNKNOWN) {
	return _types.size()==0;
  } else {
	return _types.find(et)!=_types.end();
  }
}

set<EdgeType> Edge::types() const {
  return _types;
}

void Edge::addType(EdgeType et) {
  // perform some consistency checks
  bool ok=true;
  // for EDGE_PATH we allow any combination, otherwise we check

  if(!isType(EDGE_PATH) && !(et==EDGE_PATH) && !boolOptions["semantic-fold"] && !boolOptions["post-semantic-fold"]) {
	switch(et) {
	case EDGE_FORWARD: if(isType(EDGE_BACKWARD)) ok=false;break;
	case EDGE_BACKWARD: if(isType(EDGE_FORWARD)) ok=false;break;
	case EDGE_TRUE: if(isType(EDGE_FALSE)) ok=false;break;
	case EDGE_FALSE: if(isType(EDGE_TRUE)) ok=false;break;
	default:
	  ;// anything else is ok
	}
  }
  if(ok)
	_types.insert(et);
  else {
	cerr << "Error: inconsistent icfg-edge annotation: edge-annot: " << et << ", set: "<<toString() <<endl;
	exit(1);
  }
}

void Edge::addTypes(set<EdgeType> ets) {
  if(ets.find(EDGE_PATH)!=ets.end()) {
	addType(EDGE_PATH);
  }
  for(set<EdgeType>::iterator i=ets.begin();i!=ets.end();++i) {
	addType(*i);
  }
}

void Edge::removeType(EdgeType et) {
  set<EdgeType>::const_iterator iter =_types.find(et);
  if(iter!=_types.end()) {
	_types.erase(iter);
  }
}

string Edge::toString() const {
  stringstream ss;
  ss << "Edge"<<"("<<source<<","<<typesToString()<<","<<target<<")";
  return ss.str();
}
string Edge::toStringNoType() const {
  stringstream ss;
  ss << "("<<source<<","<<target<<")";
  return ss.str();
}

string Edge::typesToString() const {
  stringstream ss;
  ss<<"{";
  if(_types.size()==0) {
	ss<<typeToString(EDGE_UNKNOWN);
  } else {
	for(set<EdgeType>::iterator i=_types.begin();
		i!=_types.end();
		++i) {
	  if(i!=_types.begin())
		ss<< ", ";
	  ss<<typeToString(*i);
	}
  }
  ss<<"}";
  return ss.str();
}

string Edge::typeToString(EdgeType et) {
  switch(et) {
  case EDGE_UNKNOWN: return "unknown";
  case EDGE_FORWARD: return "forward"; // forward edges are obvious.
  case EDGE_BACKWARD: return "backward";
  case EDGE_TRUE: return "true";
  case EDGE_FALSE: return "false";
  case EDGE_LOCAL: return "local";
  case EDGE_EXTERNAL: return "external";
  case EDGE_CALL: return "call";
  case EDGE_CALLRETURN: return "callreturn";
  default:
	cerr<<"Error: Edge-type is not of 'enum EdgeType'.";
	exit(1);
  }
  return ""; // dead code. just to provide some return value to avoid false positive compiler warnings
}

string Edge::color() const {
  if(isType(EDGE_BACKWARD)) 
	return "blue";
  else if(isType(EDGE_TRUE)) 
	return "green";
  else if(isType(EDGE_FALSE)) 
	return "red";
  return "black";
}


// color: true/false has higher priority than forward/backward.
string Edge::toDot() const {
  stringstream ss;
  ss<<source<<"->"<<target;
  ss<<" [label=\""<<typesToString()<<"\"";
  ss<<" color="<<color()<<" ";
  ss<<"]";
  return ss.str();
}

string Edge::dotEdgeStyle() const {
  if(isType(EDGE_EXTERNAL)) 
	return "dotted";
  else if(isType(EDGE_CALL) || isType(EDGE_CALLRETURN)) 
	return "dashed";
  return "solid";
}
InterEdge::InterEdge(Label call, Label entry, Label exit, Label callReturn):
  call(call),
  entry(entry),
  exit(exit),
  callReturn(callReturn){
  }
string InterEdge::toString() const {
  stringstream ss;
  ss << "("<<call<<","<<entry<<","<<exit<<","<<callReturn<<")";
  return ss.str();
}

CFAnalyzer::CFAnalyzer(Labeler* l):labeler(l){
}

LabelSet CFAnalyzer::functionCallLabels(Flow& flow) {
  LabelSet resultSet;
  LabelSet nodeLabels;
  nodeLabels=flow.nodeLabels();
  for(LabelSet::iterator i=nodeLabels.begin();i!=nodeLabels.end();++i) {
	if(labeler->isFunctionCallLabel(*i))
	  resultSet.insert(*i);
  }
  return resultSet;
}

string InterFlow::toString() const {
  string res;
  res+="{";
  for(InterFlow::iterator i=begin();i!=end();++i) {
	if(i!=begin())
	  res+=",";
	res+=(*i).toString();
  }
  res+="}";
  return res;
}

bool CodeThorn::operator<(const InterEdge& e1, const InterEdge& e2) {
  if(e1.call!=e2.call) 
	return e1.call<e2.call;
  if(e1.entry!=e2.entry)
	return e1.entry<e2.entry;
  if(e1.exit!=e2.exit)
	return e1.exit<e2.exit;
  return e1.callReturn<e2.callReturn;
}

bool CodeThorn::operator==(const InterEdge& e1, const InterEdge& e2) {
  return e1.call==e2.call
    && e1.entry==e2.entry
    && e1.exit==e2.exit
	&& e1.callReturn==e2.callReturn
	;
}

bool CodeThorn::operator!=(const InterEdge& e1, const InterEdge& e2) {
  return !(e1==e2);
}

InterFlow CFAnalyzer::interFlow(Flow& flow) {
  // 1) for each call use AST information to find its corresponding called function
  // 2) create a set of <call,entry,exit,callreturn> edges
  InterFlow interFlow;
  LabelSet callLabs=functionCallLabels(flow);
  //cout << "calllabs: "<<callLabs.size()<<endl;
  for(LabelSet::iterator i=callLabs.begin();i!=callLabs.end();++i) {
	SgNode* callNode=getNode(*i);
	//info: callNode->get_args()
	SgFunctionCallExp *funCall=SgNodeHelper::Pattern::matchFunctionCall(callNode);
	if(!funCall) 
	  throw "Error: interFlow: unkown call exp (not a SgFunctionCallExp).";
	SgFunctionDefinition* funDef=SgNodeHelper::determineFunctionDefinition(funCall);
	Label callLabel,entryLabel,exitLabel,callReturnLabel;
	if(funDef==0) {
	  // we were not able to find the funDef in the AST
	  cout << "STATUS: External function ";
	  if(SgFunctionDeclaration* funDecl=funCall->getAssociatedFunctionDeclaration())
		cout << SgNodeHelper::getFunctionName(funDecl)<<"."<<endl;
	  else
		cout << "cannot be determined."<<endl;
	  callLabel=*i;
	  entryLabel=Labeler::NO_LABEL;
	  exitLabel=Labeler::NO_LABEL;
	  callReturnLabel=labeler->functionCallReturnLabel(callNode);
	} else {
	  callLabel=*i;
	  entryLabel=labeler->functionEntryLabel(funDef);
	  exitLabel=labeler->functionExitLabel(funDef);
	  callReturnLabel=labeler->functionCallReturnLabel(callNode);
	}
	interFlow.insert(InterEdge(callLabel,entryLabel,exitLabel,callReturnLabel));
	  
  }
  return interFlow;
}

Label CFAnalyzer::getLabel(SgNode* node) {
  assert(labeler);
  return labeler->getLabel(node);
}

SgNode* CFAnalyzer::getNode(Label label) {
  assert(labeler);
  return labeler->getNode(label);
}

Labeler* CFAnalyzer::getLabeler() {
  return labeler;
}

Label CFAnalyzer::initialLabel(SgNode* node) {
  assert(node);

  // special case of incExpr in SgForStatement
  if(SgNodeHelper::isForIncExpr(node))
	return labeler->getLabel(node);

  // special case of function call
  if(SgNodeHelper::Pattern::matchFunctionCall(node))
	return labeler->getLabel(node);

  if(!labeler->isLabelRelevantNode(node)) {
	cerr << "Error: not label relevant node "<<node->sage_class_name()<<endl;
  }
  assert(labeler->isLabelRelevantNode(node));
  switch (node->variantT()) {
  case V_SgFunctionDeclaration:
  case V_SgNullStatement:
  case V_SgLabelStatement:
	return labeler->getLabel(node);
  case V_SgFunctionDefinition:
	return labeler->getLabel(node);
  case V_SgBreakStmt:
  case V_SgReturnStmt:
  case V_SgVariableDeclaration:
	  return labeler->getLabel(node);
  case V_SgExprStatement: {
	return labeler->getLabel(node);
  }
  case V_SgInitializedName:
	return labeler->getLabel(node);
  case V_SgIfStmt:
  case V_SgWhileStmt:
	node=SgNodeHelper::getCond(node);
	return labeler->getLabel(node);
  case V_SgDoWhileStmt:
	node=SgNodeHelper::getLoopBody(node);
	return labeler->getLabel(node);
  case V_SgBasicBlock:
   return labeler->getLabel(node);
   // TODO: for(emptyInitList;S;S) {}
  case V_SgForStatement: {
	SgStatementPtrList& stmtPtrList=SgNodeHelper::getForInitList(node);
	if(stmtPtrList.size()==0) {
	  // empty initializer list (hence, an initialization stmt cannot be initial stmt of for)
	  cout << "INFO: for-stmt: initializer-list is empty."<<endl;
	  cerr << "ERROR: we are bailing out. This case is not implemented yet."<<endl;
	  exit(1);
	}
	assert(stmtPtrList.size()>0);
	node=*stmtPtrList.begin();
	return labeler->getLabel(node);
  }
  default:
	cerr << "Error: Unknown node in CodeThorn::CFAnalyzer::initialLabel: "<<node->sage_class_name()<<endl; exit(1);
  }
}
 
LabelSet CFAnalyzer::finalLabels(SgNode* node) {
  assert(node);
  assert(labeler->isLabelRelevantNode(node));
  LabelSet finalSet;

  // special case of incExpr in SgForStatement
  if(SgNodeHelper::isForIncExpr(node)) {
	finalSet.insert(labeler->getLabel(node));
	return finalSet;
  }

  // special case of function call
  if(SgNodeHelper::Pattern::matchFunctionCall(node)) {
	if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node)) {
	  finalSet.insert(labeler->functionCallReturnLabel(node)+1); // special case of function call in return-stmt
	} else {
	  finalSet.insert(labeler->functionCallReturnLabel(node));
	}
	return finalSet;
  }

  switch (node->variantT()) {
  // function declarations inside basic block
  case V_SgFunctionDeclaration:
	  finalSet.insert(labeler->getLabel(node));
	  return finalSet;
  case V_SgFunctionDefinition: {
	SgBasicBlock* body=isSgFunctionDefinition(node)->get_body();
	return finalLabels(body);
  }
  case V_SgBreakStmt:
	return finalSet;
  case V_SgReturnStmt:
	return finalSet;
  case V_SgNullStatement:
  case V_SgLabelStatement:
  case V_SgInitializedName:
  case V_SgVariableDeclaration:
	  finalSet.insert(labeler->getLabel(node));
	  return finalSet;
  case V_SgExprStatement: {
	finalSet.insert(labeler->getLabel(node));
	return finalSet;
  }

  case V_SgIfStmt: {
	SgNode* nodeTB=SgNodeHelper::getTrueBranch(node);
	LabelSet finalSetTB=finalLabels(nodeTB);
	finalSet+=finalSetTB;
	if(SgNode* nodeFB=SgNodeHelper::getFalseBranch(node)) {
	  LabelSet finalSetFB=finalLabels(nodeFB);
	  finalSet+=finalSetFB;
	} else {
	  // in case of an empty else branch the cond node becomes the final node
	  SgNode* condNode=SgNodeHelper::getCond(node);
	  finalSet.insert(labeler->getLabel(condNode));
	}
	return finalSet;
  }
  case V_SgForStatement:
  case V_SgDoWhileStmt:
  case V_SgWhileStmt: {
	SgNode* condNode=SgNodeHelper::getCond(node);
	finalSet.insert(labeler->getLabel(condNode));
    set<SgNode*> breakNodes=SgNodeHelper::LoopRelevantBreakStmtNodes(node);
	LabelSet lset=labeler->getLabelSet(breakNodes);
	finalSet+=lset;
	//cout << finalSet.toString() << endl;
	return finalSet;
  }
  case V_SgBasicBlock: {
	if(SgNodeHelper::numChildren(node)>0) {
	  SgNode* lastNode=SgNodeHelper::getLastOfBlock(node);
	  LabelSet s=finalLabels(lastNode);
	  finalSet+=s;
	} else {
	  // empty basic block
	  finalSet.insert(initialLabel(node));
	}
	return finalSet;
  }
  case V_SgFunctionCallExp:
	finalSet.insert(labeler->functionCallReturnLabel(node));
	return finalSet;

  default:
	cerr << "Error: Unknown node in CodeThorn::CFAnalyzer::finalLabels: "<<node->sage_class_name()<<endl; exit(1);
   }
}

bool CodeThorn::operator==(const Edge& e1, const Edge& e2) {
  assert(&e1);
  assert(&e2);
  return e1.source==e2.source && e1.typesCode()==e2.typesCode() && e1.target==e2.target;
}
bool CodeThorn::operator!=(const Edge& e1, const Edge& e2) {
  return !(e1==e2);
}
bool CodeThorn::operator<(const Edge& e1, const Edge& e2) {
  assert(&e1);
  assert(&e2);
  if(e1.source!=e2.source)
	return e1.source<e2.source;
  if(e1.target!=e2.target)
	return e1.target<e2.target;
  return e1.typesCode()<e2.typesCode();
}

long Edge::typesCode() const {
  long h=1;
  for(set<EdgeType>::iterator i=_types.begin();i!=_types.end();++i) {
	h+=(1<<*i);
  }
  return h;
}

long Edge::hash() const {
  return typesCode();
}



Flow::Flow():_dotOptionDisplayLabel(true),_dotOptionDisplayStmt(true){
}

string Flow::toString() {
  stringstream ss;
  ss<<"{";
  for(Flow::iterator i=begin();i!=end();++i) {
	if(i!=begin())
	  ss<<",";
	if(_stringNoType)
	  ss<<(*i).toStringNoType();
	else
	  ss<<(*i).toString();
  }
  ss<<"}";
  return ss.str();
}

Flow Flow::operator+(Flow& s2) {
  Flow result;
  result=*this;
  for(Flow::iterator i2=s2.begin();i2!=s2.end();++i2)
	result.insert(*i2);
  return result;
}
 
Flow& Flow::operator+=(Flow& s2) {
  for(Flow::iterator i2=s2.begin();i2!=s2.end();++i2)
	insert(*i2);
  return *this;
}

void Flow::setDotOptionDisplayLabel(bool opt) {
  _dotOptionDisplayLabel=opt;
}
void Flow::setDotOptionDisplayStmt(bool opt) {
  _dotOptionDisplayStmt=opt;
}

string Flow::toDot(Labeler* labeler) {
  stringstream ss;
  ss<<"digraph G {\n";
  LabelSet nlabs=nodeLabels();
  for(LabelSet::iterator i=nlabs.begin();i!=nlabs.end();++i) {
	if(_dotOptionDisplayLabel) {
	  ss << *i;
	  ss << " [label=\"";
	  ss << Labeler::labelToString(*i);
	  if(_dotOptionDisplayStmt)
		ss << ": ";
	}
	if(_dotOptionDisplayStmt) {
	  SgNode* node=labeler->getNode(*i);
	  if(labeler->isFunctionEntryLabel(*i))
		ss<<"Entry:";
	  if(labeler->isFunctionExitLabel(*i))
		ss<<"Exit:";
	  if(labeler->isFunctionCallLabel(*i))
		ss<<"Call:";
	  if(labeler->isFunctionCallReturnLabel(*i))
		ss<<"CallReturn:";
	  ss<<SgNodeHelper::nodeToString(node);
	}
	if(_dotOptionDisplayLabel||_dotOptionDisplayStmt)
	  ss << "\"";
	if(_dotOptionDisplayLabel||_dotOptionDisplayStmt) {
	  SgNode* node=labeler->getNode(*i);
	  if(SgNodeHelper::isCond(node)) {
		ss << " shape=oval style=filled color=yellow "; 
	  } else {
		ss << " shape=box ";
	  }
	  ss << "];\n";
	}
  }
  for(Flow::iterator i=begin();i!=end();++i) {
	Edge e=*i;
	ss<<e.toDot()<<";\n";
  }
  ss<<"}";
  return ss.str();
}

Flow Flow::inEdges(Label label) {
  Flow flow;
  for(Flow::iterator i=begin();i!=end();++i) {
	if((*i).target==label)
	  flow.insert(*i);
  }
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}

Flow Flow::outEdges(Label label) {
  Flow flow;
  for(Flow::iterator i=begin();i!=end();++i) {
	if((*i).source==label)
	  flow.insert(*i);
  }
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}

Flow Flow::outEdgesOfType(Label label, EdgeType edgeType) {
  Flow flow;
  for(Flow::iterator i=begin();i!=end();++i) {
	if((*i).source==label && (*i).isType(edgeType))
	  flow.insert(*i);
  }
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}


LabelSet Flow::nodeLabels() {
  LabelSet s;
  for(Flow::iterator i=begin();i!=end();++i) {
	Edge e=*i;
	s.insert(e.source);
	s.insert(e.target);
  }
  return s;
}

LabelSet Flow::sourceLabels() {
  LabelSet s;
  for(Flow::iterator i=begin();i!=end();++i) {
	Edge e=*i;
	s.insert(e.source);
  }
  return s;
}

LabelSet Flow::targetLabels() {
  LabelSet s;
  for(Flow::iterator i=begin();i!=end();++i) {
	Edge e=*i;
	s.insert(e.target);
  }
  return s;
}

LabelSet Flow::pred(Label label) {
  Flow flow=inEdges(label);
  return flow.sourceLabels();
}

LabelSet Flow::succ(Label label) {
  Flow flow=outEdges(label);
  return flow.targetLabels();
}

Flow CFAnalyzer::flow(SgNode* s1, SgNode* s2) {
  assert(s1);
  assert(s2);
  Flow flow12;
  Flow flow1=flow(s1);
  Flow flow2=flow(s2);
  flow12+=flow1;
  flow12+=flow2;
  LabelSet finalSets1=finalLabels(s1);
  Label initLabel2=initialLabel(s2);
  for(LabelSet::iterator i=finalSets1.begin();i!=finalSets1.end();++i) {
	Edge e(*i,initLabel2);
	if(SgNodeHelper::isCond(labeler->getNode(*i))) { // special case (all TRUE edges are created explicitly)
	  e.addType(EDGE_FALSE);
	}
	e.addType(EDGE_FORWARD);
	flow12.insert(e);
  }
  return flow12;
}

int CFAnalyzer::reduceBlockBeginNodes(Flow& flow) {
  LabelSet labs=flow.nodeLabels();
  int cnt=0;
  for(LabelSet::iterator i=labs.begin();i!=labs.end();++i) {
	if(isSgBasicBlock(getNode(*i))) {
	  cnt++;
	  Flow inFlow=flow.inEdges(*i);
	  Flow outFlow=flow.outEdges(*i);

	  // multiple out-edges not supported yet
	  assert(outFlow.size()<=1); 

	  /* description of essential operations:
	   *   inedges: (n_i,b)
	   *   outedge: (b,n2) 
	   *   remove(n_i,b)
	   *   remove(b,n2)
	   *   insert(n1,n2)
	   */
	  for(Flow::iterator initer=inFlow.begin();initer!=inFlow.end();++initer) {
		Edge e1=*initer;
		Edge e2=*outFlow.begin();
		Edge newEdge=Edge(e1.source,e1.types(),e2.target);
		flow.erase(e1);
		flow.erase(e2);
		flow.insert(newEdge);
	  }
	}
  }
  return cnt;
}

void CFAnalyzer::intraInterFlow(Flow& flow, InterFlow& interFlow) {
  for(InterFlow::iterator i=interFlow.begin();i!=interFlow.end();++i) {
	if((*i).entry==Labeler::NO_LABEL && (*i).exit==Labeler::NO_LABEL) {
	  Edge externalEdge=Edge((*i).call,EDGE_EXTERNAL,(*i).callReturn);	  
	  flow.insert(externalEdge);
	} else {
	  Edge callEdge=Edge((*i).call,EDGE_CALL,(*i).entry);
	  Edge callReturnEdge=Edge((*i).exit,EDGE_CALLRETURN,(*i).callReturn);
	  Edge localEdge=Edge((*i).call,EDGE_LOCAL,(*i).callReturn);
	  flow.insert(callEdge);
	  flow.insert(callReturnEdge);
	  flow.insert(localEdge);
	}
  }
}

Flow CFAnalyzer::WhileAndDoWhileLoopFlow(SgNode* node, 
										 Flow edgeSet,
										 EdgeType edgeTypeParam1,
										 EdgeType edgeTypeParam2) {
  if(!(isSgWhileStmt(node) || isSgDoWhileStmt(node))) {
	throw "Error: WhileAndDoWhileLoopFlow: unsupported loop construct.";
  }
  SgNode* condNode=SgNodeHelper::getCond(node);
  Label condLabel=getLabel(condNode);
  SgNode* bodyNode=SgNodeHelper::getLoopBody(node);
  assert(bodyNode);
  Edge edge=Edge(condLabel,EDGE_TRUE,initialLabel(bodyNode));
  edge.addType(edgeTypeParam1);
  Flow flowB=flow(bodyNode);
  LabelSet finalSetB=finalLabels(bodyNode);
  edgeSet+=flowB;
  edgeSet.insert(edge);
  // back edges in while (forward edges in do-while)
  for(LabelSet::iterator i=finalSetB.begin();i!=finalSetB.end();++i) {
	Edge e;
	if(SgNodeHelper::isCond(labeler->getNode(*i))) {
	  e=Edge(*i,EDGE_FALSE,condLabel);
	  e.addType(edgeTypeParam2);
	} else {
	  e=Edge(*i,edgeTypeParam2,condLabel);
	}
	edgeSet.insert(e);
  }
  return edgeSet;
}

Flow CFAnalyzer::flow(SgNode* node) {
  assert(node);

  Flow edgeSet;
  if(node==0)
	return edgeSet;
  if(isSgFunctionDeclaration(node)) {
	return edgeSet;
  }

  if(isSgProject(node)||isSgFileList(node)||isSgGlobal(node)||isSgSourceFile(node)) {
	RoseAst ast(node);
	Flow tmpEdgeSet;
	// search for all functions and union flow for all functions
	for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	  if(isSgFunctionDefinition(*i)) {
		i.skipChildrenOnForward();
		cout << "STATUS: Generating flow for function "<<SgNodeHelper::getFunctionName(*i)<<endl;
		tmpEdgeSet=flow(*i);
		edgeSet+=tmpEdgeSet;
	  }
	}
	return edgeSet;
  }
  
  // special case of function call pattern
  if(SgNodeHelper::Pattern::matchFunctionCall(node)) {
	// we add the 'local' edge when intraInter flow is computed (it may also be an 'external' edge)
#if 0
	Label callLabel=labeler->functionCallLabel(node);
	Label callReturnLabel=labeler->functionCallReturnLabel(node);
	edgeSet.insert(Edge(callLabel,EDGE_LOCAL,callReturnLabel));
#endif
	// add special case edge for callReturn to returnNode SgReturnStmt(SgFunctionCallExp) 
	// edge: SgFunctionCallExp.callReturn->init(SgReturnStmt)
	if(SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node)) {
	  Label callReturnLabel=labeler->functionCallReturnLabel(node);
	  Label returnStmtLabel=labeler->functionCallReturnLabel(node)+1;
	  edgeSet.insert(Edge(callReturnLabel,EDGE_FORWARD,returnStmtLabel));
	}
	return edgeSet;
  }

  switch (node->variantT()) {
  case V_SgFunctionDefinition: {
	SgBasicBlock* body=isSgFunctionDefinition(node)->get_body();
	Edge edge=Edge(labeler->functionEntryLabel(node),EDGE_FORWARD,initialLabel(body));
	edgeSet.insert(edge);
	Flow bodyFlow=flow(body);
	edgeSet+=bodyFlow;
	/* add explicitly edge from last stmt of function to exit-node
	   not that a return at the end of a function is *not* represented in a ROSE AST.
	   if return does not exist, we need to add an explicit edge from end of function to exit
	   since we create this edge with the computed final node a branch with returns in all branches is
	   maintained properly.
	   this edge is identical if we have some other branches. Because we maintain the edges as an edge-set
	   this operation is always OK.
	 */
	LabelSet funFinalLabels=finalLabels(node);
	for(LabelSet::iterator i=funFinalLabels.begin();i!=funFinalLabels.end();++i) {
	  Edge explicitEdge=Edge(*i,EDGE_FORWARD,labeler->functionExitLabel(node));
	  if(SgNodeHelper::isLoopCond(labeler->getNode(*i))) {
		explicitEdge.addType(EDGE_FALSE);
	  }
	  edgeSet.insert(explicitEdge);
	}
	return edgeSet;
  }
  case V_SgReturnStmt: {
	SgNode* funcDef=SgNodeHelper::correspondingSgFunctionDefinition(node);
	if(!funcDef)
	  cerr << "Error: No corresponding function for ReturnStmt found."<<endl;
	assert(isSgFunctionDefinition(funcDef));
	Edge edge=Edge(getLabel(node),EDGE_FORWARD,labeler->functionExitLabel(funcDef));
	edgeSet.insert(edge);
	return edgeSet;
  }
  case V_SgBreakStmt:
  case V_SgInitializedName:
  case V_SgVariableDeclaration:
  case V_SgNullStatement:
  case V_SgLabelStatement:
  case V_SgExprStatement:
	return edgeSet;
  case V_SgIfStmt: {
	SgNode* nodeC=SgNodeHelper::getCond(node);
	Label condLabel=getLabel(nodeC);
	SgNode* nodeTB=SgNodeHelper::getTrueBranch(node);
	Edge edgeTB=Edge(condLabel,EDGE_TRUE,initialLabel(nodeTB));
	edgeTB.addType(EDGE_FORWARD);
	edgeSet.insert(edgeTB);
	Flow flowTB=flow(nodeTB);
	edgeSet+=flowTB;
	if(SgNode* nodeFB=SgNodeHelper::getFalseBranch(node)) {
	  Flow flowFB=flow(nodeFB);
	  Edge edgeFB=Edge(condLabel,EDGE_FALSE,initialLabel(nodeFB));
	  edgeFB.addType(EDGE_FORWARD);
	  edgeSet.insert(edgeFB);
	  edgeSet+=flowFB;
	}
	return edgeSet;
  }
  case V_SgWhileStmt: 
	return WhileAndDoWhileLoopFlow(node,edgeSet,EDGE_FORWARD,EDGE_BACKWARD);
  case V_SgDoWhileStmt: 
	return WhileAndDoWhileLoopFlow(node,edgeSet,EDGE_BACKWARD,EDGE_FORWARD);
  case V_SgBasicBlock: {
	size_t len=node->get_numberOfTraversalSuccessors();
	if(len==0) {
	  return edgeSet;
	} else {
	  if(len==1) {
		SgNode* onlyStmt=node->get_traversalSuccessorByIndex(0);
		Flow onlyFlow=flow(onlyStmt);
		edgeSet+=onlyFlow;
	  } else {
		for(size_t i=0;i<len-1;++i) {
		  SgNode* childNode1=node->get_traversalSuccessorByIndex(i);
		  SgNode* childNode2=node->get_traversalSuccessorByIndex(i+1);
		  Flow flow12=flow(childNode1,childNode2);
		  edgeSet+=flow12;
		}
	  }
	}
	SgNode* firstStmt=node->get_traversalSuccessorByIndex(0);
	Edge edge=Edge(getLabel(node),EDGE_FORWARD,initialLabel(firstStmt));
	edgeSet.insert(edge);
	return edgeSet;
  }

  case V_SgForStatement: {
	SgStatementPtrList& stmtPtrList=SgNodeHelper::getForInitList(node);
	int len=stmtPtrList.size();
	if(len==0) {
	  // empty initializer list (hence, an initialization stmt cannot be initial stmt of for)
	  cout << "INFO: for-stmt: initializer-list is empty."<<endl;
	  cerr << "Error: empty for-stmt initializer not supported yet."<<endl;
	  exit(1);
	}
	assert(len>0);
	SgNode* lastNode=0;
	if(len==1) {
	  SgNode* onlyStmt=*stmtPtrList.begin();
	  Flow onlyFlow=flow(onlyStmt);
	  edgeSet+=onlyFlow;
	  lastNode=onlyStmt;
	} else {
	  assert(stmtPtrList.size()>=2);
	  for(SgStatementPtrList::iterator i=stmtPtrList.begin();
		  i!=stmtPtrList.end();
		  ++i) {
		SgNode* node1=*i;
		SgStatementPtrList::iterator i2=i;
		i2++;
		SgNode* node2=*i2;
		Flow flow12=flow(node1,node2);
		edgeSet+=flow12;
		i2++;
		if(i2==stmtPtrList.end()) {
		  lastNode=node2;
		  break;
		}		
	  }
	}
	SgNode* condNode=SgNodeHelper::getCond(node);
	if(!condNode)
	  throw "Error: for-loop: empty condition not supported yet.";
	Flow flowInitToCond=flow(lastNode,condNode);
	edgeSet+=flowInitToCond;
	Label condLabel=getLabel(condNode);
	SgNode* bodyNode=SgNodeHelper::getLoopBody(node);
	assert(bodyNode);
	Edge edge=Edge(condLabel,EDGE_TRUE,initialLabel(bodyNode));
	edge.addType(EDGE_FORWARD);
	Flow flowB=flow(bodyNode);
	LabelSet finalSetB=finalLabels(bodyNode);
	edgeSet+=flowB;
	edgeSet.insert(edge);
	// back edges
	for(LabelSet::iterator i=finalSetB.begin();i!=finalSetB.end();++i) {
	  SgExpression* incExp=SgNodeHelper::getForIncExpr(node);
	  if(!incExp)
		throw "Error: for-loop: empty incExpr not supported yet.";
	  assert(incExp);
	  Label incExpLabel=getLabel(incExp);
	  assert(incExpLabel!=Labeler::NO_LABEL);
	  Edge e1,e2;
	  if(SgNodeHelper::isCond(labeler->getNode(*i))) {
		e1=Edge(*i,EDGE_FALSE,incExpLabel);
		e1.addType(EDGE_FORWARD);
		e2=Edge(incExpLabel,EDGE_BACKWARD,condLabel);
	  } else {
		e1=Edge(*i,EDGE_FORWARD,incExpLabel);
		e2=Edge(incExpLabel,EDGE_BACKWARD,condLabel);
	  }
	  edgeSet.insert(e1);
	  edgeSet.insert(e2);
	}
	return edgeSet;
  }
	
  default:
	cerr << "Error: Unknown node in CodeThorn::CFAnalyzer::flow: "<<node->sage_class_name()<<endl; 
	cerr << "Problemnode: "<<node->unparseToString()<<endl;
	exit(1);
  }
}
