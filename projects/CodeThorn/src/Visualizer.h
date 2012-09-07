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

class Visualizer {
 public:
  Visualizer();
  Visualizer(Analyzer* analyzer);
  Visualizer(Labeler* l, Flow* f, StateSet* ss, EStateSet* ess, TransitionGraph* tg);
  void setOptionStateId(bool);
  void setOptionStateProperties(bool);
  void setOptionEStateLabel(bool);
  void setOptionEStateId(bool);
  void setOptionEStateProperties(bool);
  void setOptionTransitionGraphDotHtmlNode(bool);
  void setLabeler(Labeler* x);
  void setFlow(Flow* x);
  void setStateSet(StateSet* x);
  void setEStateSet(EStateSet* x);
  void setTransitionGraph(TransitionGraph* x);
  void createMappings();
  string stateToString(const State* state);
  string eStateToString(const EState* state);
  string transitionGraphDotHtmlNode(Label lab);
  string transitionGraphToDot();
  string foldedTransitionGraphToDot();
 private:
  Labeler* labeler;
  Flow* flow;
  StateSet* stateSet;
  EStateSet* eStateSet;
  TransitionGraph* transitionGraph;
  
  bool optionStateId;
  bool optionStateProperties;
  bool optionEStateLabel;
  bool optionEStateId;
  bool optionEStateProperties;
  bool optionTransitionGraphDotHtmlNode;
};

#endif
