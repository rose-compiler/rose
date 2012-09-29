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
  friend class Visualizer;

 public:
  
  Analyzer();
  ~Analyzer();

  void initAstNodeInfo(SgNode* node);

  static string nodeToString(SgNode* node);
  void initializeSolver1(std::string functionToStartAt,SgNode* root);

  State analyzeAssignRhs(State currentState,VariableId lhsVar, SgNode* rhs,ConstraintSet& cset);
  EState analyzeVariableDeclaration(SgVariableDeclaration* nextNodeToAnalyze1,EState currentEState, Label targetLabel);
  list<EState> transferFunction(Edge edge, const EState* eState);
  void addToWorkList(const EState* eState);
  const EState* addToWorkListIfNew(EState eState);
  void recordTransition(const EState* sourceState, Edge e, const EState* targetState);
  void printStatusMessage();
  bool isLTLrelevantLabel(Label label);
  const EState* takeFromWorkList() {
	if(eStateWorkList.size()==0)
	  return 0;
	const EState* co=eStateWorkList.top();
	eStateWorkList.pop();
	return co;
  }
 private:
  /*! if state exists in stateSet, a pointer to the existing state is returned otherwise 
	a new state is entered into stateSet and a pointer to it is returned.
  */
  const State* processNewState(State& s);
  const State* processNewOrExistingState(State& s);
  const EState* processNewEState(EState& s);
  const EState* processNewOrExistingEState(EState& s);
  EStateSet::ProcessingResult processEState(EState& s);
  const ConstraintSet* processNewOrExistingConstraintSet(ConstraintSet& cset);
  const EState* processNewOrExistingEState(Label label, State state, ConstraintSet cset);
  EState createEState(Label label, State state, ConstraintSet cset);
  EState createEState(Label label, State state, ConstraintSet cset, InputOutput io);

 public:
  bool isEmptyWorkList() { return eStateWorkList.size()==0;}
  const EState* topWorkList() { return eStateWorkList.top();}
  const EState* popWorkList() {
	const EState* s=topWorkList();
	eStateWorkList.pop();
	return s;
  }
  SgNode* getCond(SgNode* node);
  void generateAstNodeInfo(SgNode* node);

  //! requires init
  void runSolver1();

  //! The analyzer requires a CFAnalyzer to obtain the ICFG.
  void setCFAnalyzer(CFAnalyzer* cf) { cfanalyzer=cf; }
  CFAnalyzer* getCFAnalyzer() { return cfanalyzer; }

  // access  functions for computed information
  Labeler* getLabeler() { return cfanalyzer->getLabeler(); }
  Flow* getFlow() { return &flow; }
  StateSet* getStateSet() { return &stateSet; }
  EStateSet* getEStateSet() { return &eStateSet; }
  TransitionGraph* getTransitionGraph() { return &transitionGraph; }
  ConstraintSetMaintainer* getConstraintSetMaintainer() { return &constraintSetMaintainer; }
  //private: TODO
  Flow flow;
  SgNode* startFunRoot;
  CFAnalyzer* cfanalyzer;

  //! compute the VariableIds of variable declarations
  set<VariableId> determineVariableIdsOfVariableDeclarations(set<SgVariableDeclaration*> decls);
  //! compute the VariableIds of SgInitializedNamePtrList
  set<VariableId> determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList);

  set<string> variableIdsToVariableNames(set<VariableId>);

  bool isAssertExpr(SgNode* node);
  bool isFailedAssertEState(const EState* eState);
  //! adds a specific code to the io-info of an eState which is checked by isFailedAsserEState and determines a failed-assert eState. Note that the actual assert (and its label) is associated with the previous eState (this information can therefore be obtained from a transition-edge in the transition graph).
  EState createFailedAssertEState(const EState eState, Label target);
  //! list of all asserts in a program
  list<SgNode*> listOfAssertNodes(SgProject *root);
  //! rers-specific error_x: assert(0) version 
  list<pair<SgLabelStatement*,SgNode*> > listOfLabeledAssertNodes(SgProject *root);
  void initLabeledAssertNodes(SgProject* root) {
	_assertNodes=listOfLabeledAssertNodes(root);
  }
  string labelNameOfAssertLabel(Label lab) {
	string labelName;
	for(list<pair<SgLabelStatement*,SgNode*> >::iterator i=_assertNodes.begin();i!=_assertNodes.end();++i)
	  if(lab==getLabeler()->getLabel((*i).second))
		labelName=SgNodeHelper::getLabelName((*i).first);
	assert(labelName.size()>0);
	return labelName;
  }
  list<pair<SgLabelStatement*,SgNode*> > _assertNodes;
  string _csv_assert_live_file;
 private:
  ExprAnalyzer exprAnalyzer;
  VariableIdMapping variableIdMapping;
  EStateWorkList eStateWorkList;
  EStateSet eStateSet;
  StateSet stateSet;
  ConstraintSetMaintainer constraintSetMaintainer;
  TransitionGraph transitionGraph;
  set<const EState*> transitionSourceEStateSetOfLabel(Label lab);
  int displayDiff;
};

#endif
