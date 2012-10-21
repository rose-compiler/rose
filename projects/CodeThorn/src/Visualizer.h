#ifndef VISUALIZER
#define VISUALIZER

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Labeler.h"
#include "CFAnalyzer.h"
#include "StateRepresentation.h"
#include "Analyzer.h"
#include "CommandLineOptions.h"

class Visualizer {
 public:
  Visualizer();
  Visualizer(Analyzer* analyzer);
  Visualizer(Labeler* l, Flow* f, PStateSet* ss, EStateSet* ess, TransitionGraph* tg);
  void setOptionTransitionGraphDotHtmlNode(bool);
  void setLabeler(Labeler* x);
  void setFlow(Flow* x);
  void setPStateSet(PStateSet* x);
  void setEStateSet(EStateSet* x);
  void setTransitionGraph(TransitionGraph* x);
  void createMappings();
  string pstateToString(const PState* pstate);
  string pstateToDotString(const PState* pstate);
  string eStateToString(const EState* estate);
  string eStateToDotString(const EState* estate);
  string transitionGraphDotHtmlNode(Label lab);
  string transitionGraphToDot();
  string foldedTransitionGraphToDot();
 private:
  Labeler* labeler;
  Flow* flow;
  PStateSet* pstateSet;
  EStateSet* eStateSet;
  TransitionGraph* transitionGraph;
  
  bool optionPStateObjectAddress;
  bool optionPStateId;
  bool optionPStateProperties;
  bool optionEStateObjectAddress;
  bool optionEStateId;
  bool optionEStateProperties;
  bool optionTransitionGraphDotHtmlNode;

  bool tg1; // is true when we are visualizing transition graph 1, otherwise false.
  bool tg2; // is true when we are visualizing transition graph 2, otherwise false.
};

#endif
