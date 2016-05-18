// Author: Marc Jasper, 2016.

#ifndef PAR_PRO_ANALYZER_H
#define PAR_PRO_ANALYZER_H

#include <omp.h>

#include "sage3basic.h"

// SPRAY includes
#include "Flow.h"
#include "StateRepresentations.h"
#include "ParProTransitionGraph.h"

// CodeThorn includes

// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

using namespace SPRAY;

namespace CodeThorn {

  typedef std::list<const ParProEState*> ParProWorklist;
  typedef boost::unordered_map<std::string, std::list<int> > EdgeAnnotationMap;

  // Analyzes the state space of a parallel program. 
  class ParProAnalyzer {
  public:
ParProAnalyzer(std::vector<Flow>& cfgs): _cfgs(cfgs),
                                         _transitionGraph(ParProTransitionGraph()), 
                                         _numberOfThreadsToUse(1) {}
    void initializeSolver();
    // run the state space exploration
    void runSolver();
    void setNumberOfThreadsToUse(int n) { _numberOfThreadsToUse=n; }
    int getNumberOfThreadsToUse() { return _numberOfThreadsToUse; }
    std::vector<Flow>* getCfgs() { return &_cfgs; }
    ParProTransitionGraph* getTransitionGraph() { return &_transitionGraph; }
    bool isPrecise();


  private:
    std::list<ParProEState> parProTransferFunction(const ParProEState*);
    ParProEState transfer(const ParProEState* eState, Edge e, unsigned int cfgId);
    bool feasibleAccordingToGlobalState(Edge e, const ParProEState*);
    bool isIncompleteStgReady();
    void addToWorkList(const ParProEState* estate);
    bool isEmptyWorkList();
    const ParProEState* popWorkList();
    void set_finished(vector<bool>& v, bool val);
    bool all_false(vector<bool>& v);
    std::vector<Flow> _cfgs;
    ParProTransitionGraph _transitionGraph;
    ParProEStateSet _eStateSet;
    ParProWorklist worklist;
    int _numberOfThreadsToUse;
    EdgeAnnotationMap _edgeAnnotation2CfgIds;
  };

} // end of namespace CodeThorn

#endif
