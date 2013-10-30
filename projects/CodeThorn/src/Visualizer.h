#ifndef VISUALIZER
#define VISUALIZER

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Labeler.h"
#include "CFAnalyzer.h"
#include "StateRepresentations.h"
#include "Analyzer.h"
#include "CommandLineOptions.h"

using CodeThorn::Analyzer;
using CodeThorn::PStateSet;
using CodeThorn::EStateSet;

class AssertionExtractor {
 public:
  AssertionExtractor(Analyzer* analyzer);
  void computeLabelVectorOfEStates();
  void annotateAst();
  void setLabeler(Labeler* x);
  void setVariableIdMapping(VariableIdMapping* x);
  void setPStateSet(PStateSet* x);
  void setEStateSet(EStateSet* x);

 private:
  Labeler* labeler;
  VariableIdMapping* variableIdMapping;
  CodeThorn::PStateSet* pstateSet;
  CodeThorn::EStateSet* estateSet;
  vector<string> assertions;
};

class Visualizer {
 public:
  Visualizer();
  Visualizer(CodeThorn::Analyzer* analyzer);
  Visualizer(Labeler* l, VariableIdMapping* vim, CodeThorn::Flow* f, CodeThorn::PStateSet* ss, CodeThorn::EStateSet* ess, CodeThorn::TransitionGraph* tg);
  void setOptionTransitionGraphDotHtmlNode(bool);
  void setVariableIdMapping(VariableIdMapping* x);
  void setLabeler(Labeler* x);
  void setFlow(CodeThorn::Flow* x);
  void setPStateSet(CodeThorn::PStateSet* x);
  void setEStateSet(CodeThorn::EStateSet* x);
  void setTransitionGraph(CodeThorn::TransitionGraph* x);
  void createMappings();
  string pstateToString(const CodeThorn::PState* pstate);
  string pstateToDotString(const CodeThorn::PState* pstate);
  string estateToString(const CodeThorn::EState* estate);
  string estateToDotString(const CodeThorn::EState* estate);
  string transitionGraphDotHtmlNode(Label lab);
  string transitionGraphToDot();
  string transitionGraphWithIOToDot();
  string foldedTransitionGraphToDot();
  string estateIdStringWithTemporaries(const CodeThorn::EState* estate);
 private:
  Labeler* labeler;
  VariableIdMapping* variableIdMapping;
  CodeThorn::Flow* flow;
  CodeThorn::PStateSet* pstateSet;
  CodeThorn::EStateSet* estateSet;
  CodeThorn::TransitionGraph* transitionGraph;
  
  bool tg1; // is true when we are visualizing transition graph 1, otherwise false.
  bool tg2; // is true when we are visualizing transition graph 2, otherwise false.
  bool optionPStateObjectAddress;
  bool optionPStateId;
  bool optionPStateProperties;
  bool optionEStateObjectAddress;
  bool optionEStateId;
  bool optionEStateProperties;
  bool optionTransitionGraphDotHtmlNode;

};

#endif
