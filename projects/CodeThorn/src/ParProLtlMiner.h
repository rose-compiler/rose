// Author: Marc Jasper, 2016.

#ifndef PAR_PRO_LTL_MINER_H
#define PAR_PRO_LTL_MINER_H

#include "sage3basic.h"

// SPRAY includes
#include "StateRepresentations.h"

// CodeThorn includes
#include "SpotConnection.h"
#include "ParProAnalyzer.h"
#include "PropertyValueTable.h"

// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

using namespace CodeThorn;

namespace CodeThorn {

  typedef std::pair<std::vector<Flow>, boost::unordered_map<int, int> > SelectedCfgsAndIdMap;

  class ParProLtlMiner {
  public:
  ParProLtlMiner() : _numberOfMiningsPerSubsystem(10) {}
    ParProLtlMiner(std::vector<Flow>& cfgs, EdgeAnnotationMap& annotations) : 
      _numberOfMiningsPerSubsystem(10),
      _cfgs(cfgs),
      _annotations(annotations) {}

    list<pair<string, PropertyValue> > mineLtlProperties(int minNumVerifiable, int minNumFalsifiable, int minNumComponents);
    bool isExpectedResult(std::string ltlProperty, SelectedCfgsAndIdMap cfgsAndIdMap, EdgeAnnotationMap annotations, PropertyValue expectedResult);
    bool isVerifiable(std::string ltlProperty, SelectedCfgsAndIdMap selectedCfgsAndIdMap, EdgeAnnotationMap annotations);
    bool isFalsifiable(std::string ltlProperty, SelectedCfgsAndIdMap selectedCfgsAndIdMap, EdgeAnnotationMap annotations);
    bool expectedResultWithComponentSubset(std::string ltlProperty, SelectedCfgsAndIdMap cfgsAndIdMap, 
					   EdgeAnnotationMap annotations, PropertyValue expectedResult);
    bool falsifiableWithComponentSubset(std::string ltlProperty, SelectedCfgsAndIdMap selectedCfgsAndIdMap, EdgeAnnotationMap annotations);
    bool verifiableWithComponentSubset(std::string ltlProperty, SelectedCfgsAndIdMap selectedCfgsAndIdMap, EdgeAnnotationMap annotations);
    void setNumberOfMiningsPerSubsystem(unsigned int minings) { _numberOfMiningsPerSubsystem = minings; }
    std::string randomLtlFormula(std::vector<std::string> atomicPropositions, int maxProductions); //TODO: move to private functions

  private:
    // std::string randomLtlFormula(std::vector<std::string> atomicPropositions, int maxProductions);
    
    unsigned int _numberOfMiningsPerSubsystem;
    std::vector<Flow> _cfgs; 
    EdgeAnnotationMap _annotations;
  };

} // end of namespace CodeThorn

#endif
