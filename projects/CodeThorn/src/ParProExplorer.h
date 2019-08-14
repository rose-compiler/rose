
#ifndef PAR_PRO_EXPLORER_H
#define PAR_PRO_EXPLORER_H

#include "sage3basic.h"

// CodeThorn includes
#include "Flow.h"
#include "ParProEState.h"
#include "ParProTransitionGraph.h"
#include "DotGraphCfgFrontend.h"
#include "ParProAnalyzer.h"
#include "ParProLtlMiner.h"
#include "SpotConnection.h"
#include "Visualizer.h"

// CodeThorn includes

// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

using namespace CodeThorn;
using namespace CodeThorn;

namespace CodeThorn {

  typedef std::pair<std::vector<Flow*>, boost::unordered_map<int, int> > SelectedCfgsAndIdMap;

  enum ComponentSelection { PAR_PRO_COMPONENTS_ALL, PAR_PRO_COMPONENTS_SUBSET_FIXED, PAR_PRO_COMPONENTS_SUBSET_RANDOM };
  enum RandomSubsetMode { PAR_PRO_NUM_SUBSETS_NONE, PAR_PRO_NUM_SUBSETS_FINITE, PAR_PRO_NUM_SUBSETS_INFINITE };
  enum LtlMode { PAR_PRO_LTL_MODE_NONE, PAR_PRO_LTL_MODE_CHECK, PAR_PRO_LTL_MODE_MINE };

  /*!
   * \brief Explores a system of parallel process graphs by means of (abstract) state-space 
   exploration and/or LTL mining.
   * \author Marc Jasper
   * \date 2016, 2017.
   */
  class ParProExplorer {
  public:
    
    ParProExplorer(std::vector<Flow*>& cfas, EdgeAnnotationMap& annotationMap);
    // analyzes the behavior of the parallel program according to the selected options
    void explore();
    void computeStgApprox(ParallelSystem& system, ComponentApproximation approxMode);
    PropertyValueTable* propertyValueTable();

    void setComponentSelection(ComponentSelection componentSelection) { _componentSelection = componentSelection; }
    ComponentSelection componentSelection() { return _componentSelection; }
    void setFixedComponentSubsets(std::list<std::set<int> > fixedSubsets);
    void setRandomSubsetMode(RandomSubsetMode randomSubsetMode) { _randomSubsetMode = randomSubsetMode; }
    void setNumberDifferentComponentSubsets(int numDifferentSubsets) { _numDifferentSubsets = numDifferentSubsets; }
    void setNumberRandomComponents(int numRandomComponents) { _numRandomComponents = numRandomComponents; }
    void setStoreComputedSystems(bool keepSystems) { 
      _storeComputedSystems = keepSystems;
      _parProLtlMiner.setStoreComputedSystems(keepSystems); 
    }
    void setNumberOfComponentsForLtlAnnotations(unsigned int numComponentsLtl) { 
      _parProLtlMiner.setNumberOfComponentsForLtlAnnotations(numComponentsLtl); 
    }
    void setLtlMode(LtlMode ltlMode) { _ltlMode = ltlMode; }
    void setIncludeLtlResults(bool withResults) { _includeLtlResults = withResults; }
    void setLtlInputFilename(std::string ltlInputFilename) { _ltlInputFilename = ltlInputFilename; }
    void setNumMiningsPerSubset(int numMinings) { _parProLtlMiner.setNumberOfMiningsPerSubsystem(numMinings); }
    void setMinNumComponents(int minNumComponents) { _minNumComponents = minNumComponents; }
    void setNumRequiredVerifiable(int numVerifiable) { _numRequiredVerifiable = numVerifiable; }
    void setNumRequiredFalsifiable(int numFalsifiable) { _numRequiredFalsifiable = numFalsifiable; }
    void setNumberOfThreadsToUse(int n) { _numberOfThreadsToUse=n; }
    void setVisualize(bool viz) { _visualize = viz; }
    void setUseLtsMin(bool useLtsMin) { _useLtsMin = useLtsMin; }
    void setParallelCompositionOnly(bool compositionOnly) { _parallelCompositionOnly = compositionOnly; }

  private:
    ParallelSystem exploreOnce();
    PropertyValueTable* ltlAnalysis(ParallelSystem system);
    std::set<int> randomSetNonNegativeInts(int size, int maxInt);
    void recalculateNumVerifiedFalsified();
    void addToVisOutput(ParallelSystem& system, vector<Flow*>& dotGraphs, NumberGenerator& numGen);

    ParProLtlMiner _parProLtlMiner;

    std::vector<Flow*> _cfas;
    EdgeAnnotationMap _annotationMap;
    PropertyValueTable* _properties;
    int _numVerified;
    int _numFalsified;

    bool _storeComputedSystems;
    ComponentSelection _componentSelection;
    std::list<std::set<int> > _fixedComponentSubsets;
    std::list<std::set<int> >::iterator _currentFixedSubset;
    RandomSubsetMode _randomSubsetMode;
    int _numDifferentSubsets;
    int _numRandomComponents;
    LtlMode _ltlMode;
    bool _includeLtlResults;
    std::string _ltlInputFilename;
    int _miningsPerSubsystem;
    int _minNumComponents;
    int _numRequiredVerifiable;
    int _numRequiredFalsifiable;
    int _numberOfThreadsToUse;
    bool _visualize;
    bool _useLtsMin;
    bool _parallelCompositionOnly;
  };

} // end of namespace CodeThorn

#endif
