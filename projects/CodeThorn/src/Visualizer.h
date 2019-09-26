#ifndef VISUALIZER
#define VISUALIZER

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Labeler.h"
#include "CFAnalysis.h"
#include "EState.h"
#include "ParProTransitionGraph.h"
#include "Analyzer.h"
#include "CommandLineOptions.h"
#include "ReadWriteData.h"

#include "rose_config.h"
#ifdef HAVE_SPOT
// SPOT include
#include "tgba/tgba.hh"
#endif

using CodeThorn::Analyzer;
using CodeThorn::PStateSet;
using CodeThorn::EStateSet;
using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

class AssertionExtractor {
 public:
  AssertionExtractor(Analyzer* analyzer);
  void computeLabelVectorOfEStates();
  void annotateAst();
  void setLabeler(CodeThorn::Labeler* x);
  void setVariableIdMapping(CodeThorn::VariableIdMapping* x);
  void setPStateSet(PStateSet* x);
  void setEStateSet(EStateSet* x);

 private:
  CodeThorn::Labeler* labeler;
  CodeThorn::VariableIdMapping* variableIdMapping;
  CodeThorn::PStateSet* pstateSet;
  CodeThorn::EStateSet* estateSet;
  vector<std::string> assertions;
};

class Visualizer {
 public:
  Visualizer();
  Visualizer(CodeThorn::Analyzer* analyzer);
  Visualizer(CodeThorn::IOLabeler* l, CodeThorn::VariableIdMapping* vim, CodeThorn::Flow* f, CodeThorn::PStateSet* ss, CodeThorn::EStateSet* ess, CodeThorn::TransitionGraph* tg);
  void setVariableIdMapping(CodeThorn::VariableIdMapping* x);
  void setLabeler(CodeThorn::IOLabeler* x);
  void setFlow(CodeThorn::Flow* x);
  void setPStateSet(CodeThorn::PStateSet* x);
  void setEStateSet(CodeThorn::EStateSet* x);
  void setTransitionGraph(CodeThorn::TransitionGraph* x);
  void createMappings();
  void setOptionTransitionGraphDotHtmlNode(bool);
  void setOptionMemorySubGraphs(bool flag);
  bool getOptionMemorySubGraphs();
  std::string cfasToDotSubgraphs(std::vector<Flow*> cfas);
  std::string pstateToString(const CodeThorn::PState* pstate);
  std::string pstateToDotString(const CodeThorn::PState* pstate);
  std::string estateToString(const CodeThorn::EState* estate);
  std::string estateToDotString(const CodeThorn::EState* estate);
  std::string transitionGraphDotHtmlNode(CodeThorn::Label lab);
  std::string transitionGraphToDot();
  std::string transitionGraphWithIOToDot();
  std::string parProTransitionGraphToDot(ParProTransitionGraph* parProTransitionGraph);
#ifdef HAVE_SPOT
  std::string spotTgbaToDot(spot::tgba& tgba);
#endif
  // used for displaying abstract ("topified") transition graphs.
  std::string transitionGraphWithIOToDot(CodeThorn::EStatePtrSet displayedEStates, 
                                    bool uniteOutputFromAbstractStates, bool includeErrorStates, bool allignAbstractStates);
  std::string abstractTransitionGraphToDot(); // groups abstract states into a cluster (currently specific to Rers).
  std::string foldedTransitionGraphToDot();
  std::string estateIdStringWithTemporaries(const CodeThorn::EState* estate);
  std::string visualizeReadWriteAccesses(IndexToReadWriteDataMap& indexToReadWriteDataMap, VariableIdMapping* variableIdMapping, 
					 ArrayElementAccessDataSet& readWriteRaces, ArrayElementAccessDataSet& writeWriteRaces, 
					 bool arrayElementsAsPoints, bool useClusters, bool prominentRaceWarnings);
  std::string dotEStateAddressString(const EState* estate);
  std::string dotEStateMemoryString(const EState* estate);
  void setMemorySubGraphsOption(bool flag);
  std::string dotClusterName(const EState* estate);
 private:
  CodeThorn::IOLabeler* labeler;
  CodeThorn::VariableIdMapping* variableIdMapping;
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
  bool optionMemorySubGraphs;
};

#endif
