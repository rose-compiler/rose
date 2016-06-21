// Author: Marc Jasper, 2016.

#ifndef PAR_PRO_ANALYZER_H
#define PAR_PRO_ANALYZER_H

#include <omp.h>

#include "sage3basic.h"

// SPRAY includes
#include "Flow.h"
#include "StateRepresentations.h"
#include "ParProTransitionGraph.h"
#include "DotGraphCfgFrontend.h"

// CodeThorn includes

// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

using namespace SPRAY;
using namespace CodeThorn;

namespace CodeThorn {

  typedef std::list<const ParProEState*> ParProWorklist;
  enum ComponentApproximation { COMPONENTS_OVER_APPROX, COMPONENTS_UNDER_APPROX, COMPONENTS_NO_APPROX };

  // Analyzes the state space of a parallel program. 
  class ParProAnalyzer {
  public:
    ParProAnalyzer();
    ParProAnalyzer(std::vector<Flow>& cfgs);
    ParProAnalyzer(std::vector<Flow>& cfgs, boost::unordered_map<int, int>& cfgIdToStateIndex);
    void init(std::vector<Flow>& cfgs);
    void init(std::vector<Flow>& cfgs, boost::unordered_map<int, int>& cfgIdToStateIndex);
    void initializeSolver();
    // run the state space exploration
    void runSolver();
    void setNumberOfThreadsToUse(int n) { _numberOfThreadsToUse=n; }
    int getNumberOfThreadsToUse() { return _numberOfThreadsToUse; }
    void setAnnotationMap(EdgeAnnotationMap& emap) { _annotationToEdges = emap; }
    void setComponentApproximation(ComponentApproximation approxMode) { _approximation = approxMode; }
    std::vector<Flow>* getCfgs() { return &_cfgs; }
    ParProTransitionGraph* getTransitionGraph() { return _transitionGraph; }
    bool isPrecise();

  private:
    std::list<std::pair<Edge, ParProEState> > parProTransferFunction(const ParProEState*);
    ParProEState transfer(const ParProEState* eState, Edge e);
    bool feasibleAccordingToGlobalState(Edge e, const ParProEState*);
    bool isPreciseTransition(Edge e, const ParProEState* eState);
    ParProEState getTerminationState();
    bool isIncompleteStgReady();
    void addToWorkList(const ParProEState* estate);
    bool isEmptyWorkList();
    const ParProEState* popWorkList();
    void set_finished(vector<bool>& v, bool val);
    bool all_false(vector<bool>& v);

    std::vector<Flow> _cfgs;
    boost::unordered_map<int, int> _cfgIdToStateIndex;
    std::string _startTransitionAnnotation;
    ParProTransitionGraph* _transitionGraph;
    ParProEStateSet _eStateSet;
    ParProWorklist worklist;
    int _numberOfThreadsToUse;
    EdgeAnnotationMap _annotationToEdges;
    ComponentApproximation _approximation;
    vector<Label> _artificalTerminationLabels;
  };

} // end of namespace CodeThorn

#endif
