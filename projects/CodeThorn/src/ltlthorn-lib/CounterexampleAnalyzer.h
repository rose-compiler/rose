
#ifndef CE_ANALYZER_H
#define CE_ANALYZER_H

#ifdef HAVE_SPOT

#include "sage3basic.h"

//CodeThorn includes
#include "IOAnalyzer.h"
#include "PropertyValueTable.h"
#include "EState.h"
#include "SpotConnection.h"
#include "Visualizer.h"

//BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"

namespace CodeThorn {

  enum CounterexampleType {CE_TYPE_REAL,CE_TYPE_SPURIOUS,CE_TYPE_UNKNOWN};
  enum IoType {IO_TYPE_UNKNOWN,IO_TYPE_INPUT,IO_TYPE_OUTPUT,IO_TYPE_ERROR};

  typedef std::pair<int, IoType> CeIoVal;
  typedef std::pair<list<CeIoVal> , std::list<CeIoVal> > PrefixAndCycle;
  typedef std::list<boost::unordered_set<const EState*> > StateSets;
  typedef boost::unordered_map<const EState*, list<int> > InputsAtEState;

  /*! 
   * \author Marc Jasper
   * \date 2014, 2015.
   */
  struct CEAnalysisResult {
    CounterexampleType analysisResult;
    Label spuriousTargetLabel;
  };

  /*! 
   * \author Marc Jasper
   * \date 2014, 2015.
   */
  struct CEAnalysisStep {
    CounterexampleType analysisResult;
    const EState* mostRecentStateRealTrace; 
    int spuriousIndexInCurrentPart;
    bool continueTracingOriginal;
    //special case for an output state followed by a failing assertion in the original program (leading to spurious input)
    bool assertionCausingSpuriousInput;
    Label failingAssertionInOriginal;
  };

  /*! 
   * \brief CEGPRA analysis and check of individual counterexamples.
   * \author Marc Jasper
   * \date 2014, 2015.
   */
  class CounterexampleAnalyzer {
  public:
    // initializing the CounterexampleAnalyzer, using "analyzer" to trace paths of the original program
    CounterexampleAnalyzer(IOAnalyzer* analyzer);
    CounterexampleAnalyzer(IOAnalyzer* analyzer, stringstream* csvOutput);
    // Check whether or not the "counterexample" is spurious. 
    // If "returnSpuriousLabel" is set to true: In the case of a spurious counterexample, the result includes a label that should not 
    //                                        be reachable according to the original program but is reachable in the counterexample
    // If "startState" != NULL it will be taken as the start state for tracing the original program's execution path.
    // "resetAnaylzerData" indicates whether or not previously discovered states should be deleted initially.
    CEAnalysisResult analyzeCounterexample(string counterexample, const EState* startState, bool returnSpuriousLabel, bool resetAnalyzerData);
    // CEGPRA: Iterative verification attempts using the cegar prefix mode for LTL. Iterates over all LTL properties, respects the limit for 
    // analyzed counterexamples for each individual property.
    PropertyValueTable* cegarPrefixAnalysisForLtl(SpotConnection& spotConnection, LtlRersMapping ltlRersMapping);
    // CEGPRA: Iterative verification attempts of a given ltl property. Prefix of the state space is explored using individual counterexample traces.
    PropertyValueTable* cegarPrefixAnalysisForLtl(int property, SpotConnection& spotConnection, LtlRersMapping ltlRersMapping);
    void setMaxCounterexamples(int max) { _maxCounterexamples=max; };
    
  private: 
    // parse a counterexample string
    PrefixAndCycle ceStringToInOutVals(std::string counterexample);
    // sets the analyzer's input sequence
    void setInputSequence(std::list<CeIoVal> sourceOfInputs);
    // checks if a certain sequence (part of the counterexample) can be executed on the original program. If the list of EState sets
    // "statesPerCycleIndex" is provided, then this function can identify cycles in the original program and thus identify real counterexamples.
    CEAnalysisStep getSpuriousTransition(std::list<CeIoVal> partOfCeTrace, TransitionGraph* originalTraceGraph, 
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
    void removeAndMarkErroneousBranches(TransitionGraph* model);
    // Returns a boolean vector indicating for which input alphabet value a successor of "eState" exists in "model".
    std::vector<bool> hasFollowingInputStates(std::vector<bool> v, const EState* eState, TransitionGraph* model) ;   
    // Retrieves a vector of input states that are successors of "eState" in "model". "eState" has to have one input
    // successor for all possible input values.
    std::vector<const EState*> getFollowingInputStates(std::vector<const EState*> v, const EState* startEState, TransitionGraph* model);
    // marks index in "v" as true iff the entry in "erroneousBranches" that corresponds to "eState" containts (index+1) in its mapping.

    //std::vector<bool> setErrorBranches(vector<bool> v, const EState* eState, InputsAtEState erroneousBranches);
    std::vector<bool> setErrorBranches(vector<bool> v, const EState* eState);
    std::list<pair<const EState*, int> > removeTraceLeadingToErrorState(const EState* errorState, TransitionGraph* stg);

    // Adds all output states in the prefix to "startAndOuputStatesPrefix" and returns the set.
    EStatePtrSet addAllPrefixOutputStates(EStatePtrSet& startAndOuputStatesPrefix, TransitionGraph* model);
    // identify states needed to disconnect the concrete prefix and the over-approximated part in the initial abstract model
    pair<EStatePtrSet, EStatePtrSet> getConcreteOutputAndAbstractInput(TransitionGraph* model);
    // sort the |<input_alphabet>| abstract input states according to their input value and insert them into the vector "v". 
    vector<const EState*> sortAbstractInputStates(vector<const EState*> v, EStatePtrSet abstractInputStates);
    // deprecated: now implemented as function of EState ("isRersTopified(...)");
    bool isPrefixState(const EState* state);
    // prints #transitions, details about states and #counterexamples analyzed
    void printStgSizeAndCeCount(TransitionGraph* model, int counterexampleCount, int property);

    //for debugging purposes
    std::string ceIoValToString(CeIoVal& ioVal);
    std::string ioErrTraceToString(list<pair<int, IoType> > trace);
    void writeDotGraphToDisk(std::string filename, Visualizer& visualizer);

    IOAnalyzer* _analyzer;
    stringstream* _csvOutput;
    int _maxCounterexamples;
    InputsAtEState _erroneousBranches;
  };

} // end of namespace CodeThorn

#endif // HAVE_SPOT

#endif
