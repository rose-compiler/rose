#ifndef VISUALIZER
#define VISUALIZER

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Labeler.h"
#include "CFAnalysis.h"
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
  void setLabeler(SPRAY::Labeler* x);
  void setVariableIdMapping(SPRAY::VariableIdMapping* x);
  void setPStateSet(PStateSet* x);
  void setEStateSet(EStateSet* x);

 private:
  SPRAY::Labeler* labeler;
  SPRAY::VariableIdMapping* variableIdMapping;
  CodeThorn::PStateSet* pstateSet;
  CodeThorn::EStateSet* estateSet;
  vector<std::string> assertions;
};

class Visualizer {
 public:
  Visualizer();
  Visualizer(CodeThorn::Analyzer* analyzer);
  Visualizer(SPRAY::IOLabeler* l, SPRAY::VariableIdMapping* vim, SPRAY::Flow* f, CodeThorn::PStateSet* ss, CodeThorn::EStateSet* ess, CodeThorn::TransitionGraph* tg);
  void setOptionTransitionGraphDotHtmlNode(bool);
  void setVariableIdMapping(SPRAY::VariableIdMapping* x);
  void setLabeler(SPRAY::IOLabeler* x);
  void setFlow(SPRAY::Flow* x);
  void setPStateSet(CodeThorn::PStateSet* x);
  void setEStateSet(CodeThorn::EStateSet* x);
  void setTransitionGraph(CodeThorn::TransitionGraph* x);
  void createMappings();
  std::string pstateToString(const CodeThorn::PState* pstate);
  std::string pstateToDotString(const CodeThorn::PState* pstate);
  std::string estateToString(const CodeThorn::EState* estate);
  std::string estateToDotString(const CodeThorn::EState* estate);
  std::string transitionGraphDotHtmlNode(SPRAY::Label lab);
  std::string transitionGraphToDot();
  std::string transitionGraphWithIOToDot();
  // used for displaying abstract ("topified") transition graphs.
  std::string transitionGraphWithIOToDot(CodeThorn::EStatePtrSet displayedEStates, 
                                    bool uniteOutputFromAbstractStates, bool includeErrorStates, bool allignAbstractStates);
  std::string abstractTransitionGraphToDot(); // groups abstract states into a cluster (currently specific to Rers).
  std::string foldedTransitionGraphToDot();
  std::string estateIdStringWithTemporaries(const CodeThorn::EState* estate);
 private:
  SPRAY::IOLabeler* labeler;
  SPRAY::VariableIdMapping* variableIdMapping;
  SPRAY::Flow* flow;
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
