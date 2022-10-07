#ifndef VISUALIZER
#define VISUALIZER

#include "Labeler.h"
#include "CFAnalysis.h"
#include "EState.h"
#include "CTAnalysis.h"
#include "CommandLineOptions.h"
#include "ReadWriteData.h"

//#include "rose_config.h"
//#ifdef HAVE_SPOT
// SPOT include
//#include "tgba/tgba.hh"
//#endif

using CodeThorn::CTAnalysis;
using CodeThorn::PStateSet;
using CodeThorn::EStateSet;
using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

class AssertionExtractor {
 public:
  AssertionExtractor(CTAnalysis* analyzer);
  void computeLabelVectorOfEStates();
  void annotateAst();
  void setLabeler(CodeThorn::Labeler* x);
  void setVariableIdMapping(CodeThorn::VariableIdMapping* x);
  void setEStateSet(EStateSet* x);

 private:
  CodeThorn::Labeler* labeler;
  CodeThorn::VariableIdMapping* variableIdMapping;
  CodeThorn::EStateSet* estateSet;
  vector<std::string> assertions;
};

namespace spot {
  class tgba;
}

// required for Visualizer::spotTgbaToDot when compiling without SPOT
#ifndef HAVE_SPOT
namespace spot {
  class tgba;
}
#endif

class Visualizer {
 public:
  Visualizer();
  Visualizer(CodeThorn::CTAnalysis* analyzer);
  void setVariableIdMapping(CodeThorn::VariableIdMapping* x);
  void setLabeler(CodeThorn::Labeler* x);
  void setFlow(CodeThorn::Flow* x);
  void setEStateSet(CodeThorn::EStateSet* x);
  void setTransitionGraph(CodeThorn::TransitionGraph* x);
  void createMappings();
  void setOptionTransitionGraphDotHtmlNode(bool);
  void setOptionMemorySubGraphs(bool flag);
  bool getOptionMemorySubGraphs();
  std::string estateToString(EStatePtr estate);
  std::string estateToDotString(EStatePtr estate);
  std::string transitionGraphDotHtmlNode(CodeThorn::Label lab);
  std::string transitionGraphToDot();
  std::string transitionGraphWithIOToDot();

  // used for displaying abstract ("topified") transition graphs.
  std::string transitionGraphWithIOToDot(CodeThorn::EStatePtrSet displayedEStates, 
                                    bool uniteOutputFromAbstractStates, bool includeErrorStates, bool allignAbstractStates);
  std::string abstractTransitionGraphToDot(); // groups abstract states into a cluster (currently specific to Rers).
  std::string foldedTransitionGraphToDot();
  std::string estateIdStringWithTemporaries(EStatePtr estate);
  std::string visualizeReadWriteAccesses(IndexToReadWriteDataMap& indexToReadWriteDataMap, VariableIdMapping* variableIdMapping, 
					 ArrayElementAccessDataSet& readWriteRaces, ArrayElementAccessDataSet& writeWriteRaces, 
					 bool arrayElementsAsPoints, bool useClusters, bool prominentRaceWarnings);
  std::string dotEStateAddressString(EStatePtr estate);
  std::string dotEStateMemoryString(EStatePtr estate);
  void setMemorySubGraphsOption(bool flag);
  std::string dotClusterName(EStatePtr estate);
 private:
  CodeThorn::Labeler* labeler;
  CodeThorn::VariableIdMapping* variableIdMapping;
  CodeThorn::Flow* flow;
  CodeThorn::EStateSet* estateSet;
  CodeThorn::TransitionGraph* transitionGraph;
  
  bool tg1; // is true when we are visualizing transition graph 1, otherwise false.
  bool tg2; // is true when we are visualizing transition graph 2, otherwise false.
  //bool optionPStateObjectAddress;
  //bool optionPStateId;
  //bool optionPStateProperties;
  //bool optionEStateObjectAddress;
  //bool optionEStateId;
  //bool optionEStateProperties;
  bool optionTransitionGraphDotHtmlNode;
  CodeThornOptions _ctOpt;
  bool optionMemorySubGraphs;
};

#endif
