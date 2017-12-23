#ifndef SOLVER10_H
#define SOLVER10_H


// CodeThorn includes
#include "IOSolver.h"
#include "PropertyValueTable.h"
#include "AnalysisParameters.h"

namespace CodeThorn {

  class PState;

/*! 
  * \author Marc Jasper
  * \date 2017.
  * \brief Black-box pattern search solver (used for the Reachability Track of RERS)
 */
  class Solver10 : public IOSolver {
  public:
    Solver10();
    void run();
    int getId();

  private:
    bool containsPatternTwoRepetitions(std::list<int>& sequence, int startIndex, int endIndex);
    bool computePStateAfterInputs(PState& pState, std::list<int>& inputs, int thread_id, std::list<int>* iOSequence=NULL);;
    std::list<int> inputsFromPatternTwoRepetitions(std::list<int> pattern2r);
    std::string convertToCeString(std::list<int>& ceAsIntegers, int maxInputVal);
    int pStateDepthFirstSearch(PState* startPState, int maxDepth, int thread_id, std::list<int>* partialTrace, int maxInputVal, int patternLength, int PatternIterations);

    void initSolverState();
    void initDiagnostics();

    // Search for all 100 RERS counterexamples by default (paths to reachable failing assertions)
    // (Allows to only search for a certain subsets of reachability properties, but currently not used)
    PropertyValueTable* _patternSearchAssertTable;
    ExplorationMode _patternSearchExplorationMode;

    static Sawyer::Message::Facility logger;
    static bool _diagnosticsInitialized;
  };

} // end of namespace CodeThorn

#endif
