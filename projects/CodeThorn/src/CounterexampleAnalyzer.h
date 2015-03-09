// Author: Marc Jasper, 2014, 2015.

#ifndef CE_ANALYZER_H
#define CE_ANALYZER_H

// #include <cassert>

#include "sage3basic.h"

//CodeThorn includes
#include "Analyzer.h"
#include "PropertyValueTable.h"
#include "StateRepresentations.h"
#include "SpotConnection.h" 

//BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"

using namespace std;

namespace CodeThorn {

  enum CounterexampleType {CE_TYPE_REAL,CE_TYPE_SPURIOUS,CE_TYPE_UNKNOWN};
  enum IoType {IO_TYPE_UNKNOWN,IO_TYPE_INPUT,IO_TYPE_OUTPUT,IO_TYPE_ERROR};

  typedef pair<int, IoType> CeIoVal;
  typedef pair<list<CeIoVal> , list<CeIoVal> > PrefixAndCycle;
  typedef list<boost::unordered_set<const EState*> > StateSets;
  typedef boost::unordered_map<const EState*, list<int> > InputsAtEState;

  struct CEAnalysisResult {
    CounterexampleType analysisResult;
    Label spuriousTargetLabel;
  };

  struct CEAnalysisStep {
    CounterexampleType analysisResult;
    const EState* mostRecentStateRealTrace; 
    int spuriousIndexInCurrentPart;
    bool continueTracingOriginal;
    //special case for an output state followed by a failing assertion in the original program (leading to spurious input)
    bool assertionCausingSpuriousInput;
    Label failingAssertionInOriginal;
  };

  // Responsible for analyzing counterexamples and an analysis using a CEGAR approach.
  class CounterexampleAnalyzer {
  public:
    // initializing the CounterexampleAnalyzer, using "analyzer" to trace paths of the original program
    CounterexampleAnalyzer(Analyzer* analyzer);
    // Check whether or not the "counterexample" is spurious. 
    // If "returnSpuriousLabel" is set to true: In the case of a spurious counterexample, the result includes a label that should not 
    //                                        be reachable according to the original program but is reachable in the counterexample
    // If "startState" != NULL it will be taken as the start state for tracing the original program's execution path.
    // "resetAnaylzerData" indicates whether or not previously discovered states should be deleted initially.
    CEAnalysisResult analyzeCounterexample(string counterexample, const EState* startState, bool returnSpuriousLabel, bool resetAnalyzerData);
    // iterative verification attempts of a given ltl property. Prefix of the state space is explored using individual counterexample traces.
    PropertyValueTable* cegarPrefixAnalysisForLtl(int property, SpotConnection& spotConnection, set<int> ltlInAlphabet, set<int> ltlOutAlphabet);
    
  private: 
    // parse a counterexample string
    PrefixAndCycle ceStringToInOutVals(string counterexample);
    // sets the analyzer's input sequence
    void setInputSequence(list<CeIoVal> sourceOfInputs);
    // checks if a certain sequence (part of the counterexample) can be executed on the original program. If the list of EState sets
    // "statesPerCycleIndex" is provided, then this function can identify cycles in the original program and thus identify real counterexamples.
    CEAnalysisStep getSpuriousTransition(list<CeIoVal> partOfCeTrace, TransitionGraph* originalTraceGraph, 
                                         const EState* compareFollowingHere, StateSets* statesPerCycleIndex = NULL);
    // final evaluation of the result of the "getSpuriousTransition(...)" function
    void determineAnalysisStepResult(CEAnalysisStep& result, IoType mostRecentOriginalIoType, 
                                                const EState* currentState, TransitionGraph* originalTraceGraph, int index);
    // retrieve a label in the original program where spurious behavior of the counterexample occured first 
    // (according to observable input/output behavior)
    Label getFirstObservableSpuriousLabel(TransitionGraph* analyzedModel, PrefixAndCycle counterexample, int numberOfSteps);
    // retrieve input/output/error information from a given estate
    CeIoVal eStateToCeIoVal(const EState* eState);
    // parses the InputOuput into the IoType format (enum of observable behavior)
    IoType getIoType(InputOutput io);

    /// functions related to the cegar prefix mode.
    // updates the information about error branches regarding the latest addition to the traces stored in "model". 
    void removeAndMarkErroneousBranches(InputsAtEState* erroneousBranches, TransitionGraph* model);
    // Returns a boolean vector indicating for which input alphabet value a successor of "eState" exists in "model".
    vector<bool> hasFollowingInputStates(vector<bool> v, const EState* eState, TransitionGraph* model) ;   
    // Retrieves a vector of input states that are successors of "eState" in "model". "eState" has to have one input
    // successor for all possible input values.
    vector<const EState*> getFollowingInputStates(vector<const EState*> v, const EState* startEState, TransitionGraph* model);
    // marks index in "v" as true iff the entry in "erroneousBranches" that corresponds to "eState" containts (index+1) in its mapping.
    vector<bool> setErrorBranches(vector<bool> v, const EState* eState, InputsAtEState erroneousBranches);
    list<pair<const EState*, int> > removeTraceLeadingToErrorState(const EState* errorState, TransitionGraph* stg);
    // Adds all output states in the prefix to "startAndOuputStatesPrefix" and returns the set.
    EStatePtrSet addAllPrefixOutputStates(EStatePtrSet& startAndOuputStatesPrefix, TransitionGraph* model);
    //deprecated: now implemented as function of EState ("isRersTopified(...)");
    bool isPrefixState(const EState* state);

    //for debugging purposes
    string ceIoValToString(CeIoVal& ioVal);
    string ioErrTraceToString(list<pair<int, IoType> > trace);

    Analyzer* _analyzer;
  };

} // end of namespace CodeThorn

#endif
