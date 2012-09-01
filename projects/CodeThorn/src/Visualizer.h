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

class Visualizer {
 public:
  Visualizer();
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
  StateId stateId(const State* state);
  StateId stateId(const State state);
  EStateId eStateId(const EState* eState);
  EStateId eStateId(const EState eState);
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
  bool idMappingsAreConsistent;
};

#endif
