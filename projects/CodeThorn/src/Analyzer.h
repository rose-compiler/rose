#ifndef ANALYZER_H
#define ANALYZER_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <sstream>

#include "rose.h"

#include "AstTerm.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "MyAst.h"
#include "SgNodeHelper.h"
#include "ExprAnalyzer.h"
#include "StateRepresentation.h"

// we use INT_MIN, INT_MAX
#include "limits.h"

using namespace std;

#define DEBUGPRINT_STMT 0x1
#define DEBUGPRINT_STATE 0x2
#define DEBUGPRINT_STATEMOD 0x4
#define DEBUGPRINT_INFO 0x8

class AstNodeInfo : public AstAttribute {
 public:
 AstNodeInfo():label(0),initialLabel(0){}
  string toString() { stringstream ss;
	ss<<"\\n lab:"<<label<<" ";
	ss<<"init:"<<initialLabel<<" ";
	ss<<"final:"<<finalLabelsSet.toString();
	return ss.str(); 
  }
  void setLabel(Label l) { label=l; }
  void setInitialLabel(Label l) { initialLabel=l; }
  void setFinalLabels(LabelSet lset) { finalLabelsSet=lset; }
 private:
  Label label;
  Label initialLabel;
  LabelSet finalLabelsSet;
};

typedef stack<const EState*> EStateWorkList;

class Analyzer {

 public:
  
  Analyzer();
  ~Analyzer();

  void setCFAnalyzer(CFAnalyzer* cf) { cfanalyzer=cf; }
  CFAnalyzer* getCFAnalyzer() { return cfanalyzer; }

  void initAstNodeInfo(SgNode* node);
  //int label(SgNode* node);
  //CfgEdgeSet outEdges(SgNode* node);
  //CfgEdgeSet computeCfg(SgNode* node);

  static string nodeToString(SgNode* node);
  void initializeSolver1(std::string functionToStartAt,SgNode* root);
  //void computeEState(const EState* eState,SgNode* node);

  State analyzeAssignOp(State state,SgNode* node,ConstraintSet& cset);
  State analyzeAssignRhs(State currentState,VariableId lhsVar, SgNode* rhs,ConstraintSet& cset);
  EState analyzeVariableDeclaration(SgVariableDeclaration* nextNodeToAnalyze1,EState currentEState, Label targetLabel);
  EState transferFunction(Edge edge, const EState* eState);
  string stateToString(const State* state);
  string eStateToString(const EState* eState);
  string transitionGraphDotHtmlNode(Label l);
  string transitionGraphToDot();
  string foldedTransitionGraphToDot();
  void setOptionCompactStateString(bool opt) {optionCompactStateString=opt;}
  void addToWorkList(const EState* eState) { eStateWorkList.push(eState); }
  const EState* addToWorkListIfNew(EState eState);
  void recordTransition(const EState* sourceState, Edge e, const EState* targetState);
  void printStatusMessage();
  const EState* takeFromWorkList() {
	if(eStateWorkList.size()==0)
	  return 0;
	const EState* co=eStateWorkList.top();
	eStateWorkList.pop();
	return co;
  }
  bool isEmptyWorkList() { return eStateWorkList.size()==0;}
  const EState* topWorkList() { return eStateWorkList.top();}
  const EState* popWorkList() {
	const EState* s=topWorkList();
	eStateWorkList.pop();
	return s;
  }
  SgNode* getCond(SgNode* node);
  void generateAstNodeInfo(SgNode* node);

  // requires init
  void runSolver1();
  Labeler* getLabeler() { return cfanalyzer->getLabeler(); }
  Flow* getFlow() { return &flow; }
  TransitionGraph* getTransitionGraph() { return &transitionGraph; }

  //private: TODO
  Flow flow;
  SgNode* startFunRoot;
  CFAnalyzer* cfanalyzer;
  StateSet* getStateSet() { return &stateSet; }
  EStateSet* getEStateSet() { return &eStateSet; }
 private:
  ExprAnalyzer exprAnalyzer;
  VariableIdMapping variableIdMapping;
  EStateWorkList eStateWorkList;
  EStateSet eStateSet;
  StateSet stateSet;
  TransitionGraph transitionGraph;
  bool optionCompactStateString;
  set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
  bool optionStateNumber; // generate state number (not just the address)
  int displayDiff;
};

#endif

