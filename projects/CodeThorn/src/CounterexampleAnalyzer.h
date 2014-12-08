
#ifndef CE_ANALYZER_H
#define CE_ANALYZER_H

// #include <cassert>

#include "sage3basic.h"

//CodeThorn includes
#include "Analyzer.h"
#include "PropertyValueTable.h"
#include "StateRepresentations.h"

//BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"

using namespace std;

namespace CodeThorn {

  enum CounterexampleType {CE_TYPE_REAL,CE_TYPE_SPURIOUS,CE_TYPE_UNKNOWN};
  enum IoType {IO_TYPE_UNKNOWN,IO_TYPE_INPUT,IO_TYPE_OUTPUT,IO_TYPE_ERROR};

  typedef pair<int, IoType> CeIoVal;
  typedef pair<list<CeIoVal> , list<CeIoVal> > PrefixAndCycle;
  typedef list<boost::unordered_set<const EState*> > StateSets;

  struct CEAnalysisResult {
    CounterexampleType analysisResult;
    Label spuriousTargetLabel;
  };

  struct CEAnalysisStep {
    CounterexampleType analysisResult;
    const EState* mostRecentStateRealTrace; 
    int spuriousIndexInCurrentPart;
  };

  // analyzes whether or not counterexamples are spurious.
  class CounterexampleAnalyzer {
  public:
    // initializing the CounterexampleAnalyzer, using "analyzer" to trace paths of the original program
    CounterexampleAnalyzer(Analyzer& analyzer);
    // Check whether or not the "counterexample" is spurious. In the case of a spurious counterexample, the result
    // includes a label that should not be reachable but is reachable in the counterexample
    CEAnalysisResult analyzeCounterexample(string counterexample);
    
  private: 
    // parse a counterexample string
    PrefixAndCycle ceStringToInOutVals(string counterexample);
    // sets the analyzer's input sequence
    void setInputSequence(list<CeIoVal> sourceOfInputs);
    // checks if a certain sequence (part of the counterexample) can be executed on the original program. If the list of EState sets
    // "statesPerCycleIndex" is provided, then this function can identify cycles in the original program and thus identify real counterexamples.
    CEAnalysisStep getSpuriousTransition(list<CeIoVal> partOfCeTrace, TransitionGraph* originalTraceGraph, 
                                         const EState* compareFollowingHere, StateSets* statesPerCycleIndex = NULL);
    // retrieve a label in the original program where spurious behavior occured first while analyzing the counterexample trace
    Label getFirstSpuriousLabel(TransitionGraph* analyzedModel, PrefixAndCycle counterexample, int numberOfSteps);
    // retrieve input/output/error information from a given estate
    CeIoVal eStateToCeIoVal(const EState* eState);
    // parses the InputOuput into the IoType format (enum of observable behavior)
    IoType getIoType(InputOutput io);

    Analyzer _analyzer;
  };

} // end of namespace CodeThorn

#endif
