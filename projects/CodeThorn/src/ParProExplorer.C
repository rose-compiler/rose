// Author: Marc Jasper, 2016.

#include "ParProExplorer.h"

using namespace SPRAY;
using namespace CodeThorn;
using namespace std;

ParProExplorer::ParProExplorer(vector<Flow>& cfgs, EdgeAnnotationMap& annotationMap):
_cfgs(cfgs),
_annotationMap(annotationMap),
_properties(new PropertyValueTable()),
_numVerified(0),
_numFalsified(0),
_componentSelection(PAR_PRO_COMPONENTS_ALL),
_randomSubsetMode(PAR_PRO_NUM_SUBSETS_NONE),
_numDifferentSubsets(-1),
_numRandomComponents(3),
_ltlMode(PAR_PRO_LTL_MODE_NONE),
_includeLtlResults(false),
_miningsPerSubsystem(50),
_numRequiredVerifiable(10),
_numRequiredFalsifiable(10),
_numberOfThreadsToUse(1), 
_visualize(false) {
  for (unsigned int i=0; i<_cfgs.size(); i++) {
    _cfgIdToStateIndex.insert(pair<int, int>(i,i));
  }
}

PropertyValueTable* ParProExplorer::ltlAnalysis(pair<ParProTransitionGraph*, SelectedCfgsAndIdMap*> stgAndSelectedComponents) {
  PropertyValueTable* result = NULL;
  if (_ltlMode == PAR_PRO_LTL_MODE_NONE) {
    return new PropertyValueTable();  // return an empty table because no LTL analysis was selected
  } else if (_ltlMode == PAR_PRO_LTL_MODE_CHECK) {
    SpotConnection spotConnection(_ltlInputFilename);
    bool withCounterexample = false;
    bool spuriousNoAnswers = false;
    spotConnection.checkLtlPropertiesParPro( *(stgAndSelectedComponents.first), withCounterexample, spuriousNoAnswers);
    result = spotConnection.getLtlResults();
  } else if (_ltlMode == PAR_PRO_LTL_MODE_MINE) { //TODO: allow for a different number of minimally required components than the number of components used to generate the STG
    if (stgAndSelectedComponents.second) {
      ParProLtlMiner miner((stgAndSelectedComponents.second)->first, _annotationMap);  //(stgAndSelectedComponents.second)->second);
      miner.setNumberOfMiningsPerSubsystem(_miningsPerSubsystem);
      result = miner.mineLtlProperties(_numRequiredVerifiable, _numRequiredFalsifiable, (stgAndSelectedComponents.second)->first.size());
    } else {
      boost::unordered_map<int, int> cfgIdToStateIndex;
      for (unsigned int i=0; i<_cfgs.size(); i++) {
	cfgIdToStateIndex[i] = i;
      }
      ParProLtlMiner miner(_cfgs, _annotationMap);  //cfgIdToStateIndex);
      miner.setNumberOfMiningsPerSubsystem(_miningsPerSubsystem);
      result = miner.mineLtlProperties(_numRequiredVerifiable, _numRequiredFalsifiable, _cfgs.size());
    }
  }
  return result;
}

void ParProExplorer::explore() {
  if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_INFINITE) {
    if (_ltlMode != PAR_PRO_LTL_MODE_MINE) {
      cout << "ERROR: An unlimited number of analyses using random subsets has been selected, but LTLs are not generated (choose mode --ltl-mode=mine)."<<endl;
      ROSE_ASSERT(0);
    }
    while(_numVerified < _numRequiredVerifiable || _numFalsified < _numRequiredFalsifiable) {
      pair<ParProTransitionGraph*, SelectedCfgsAndIdMap*> stgAndSelectedComponents = exploreOnce();
      _properties->append( *(ltlAnalysis(stgAndSelectedComponents)) );
    }
  } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_FINITE) {
    for (int i = 0; i < _numDifferentSubsets; i++) {
      pair<ParProTransitionGraph*, SelectedCfgsAndIdMap*> stgAndSelectedComponents = exploreOnce();
      _properties->append( *(ltlAnalysis(stgAndSelectedComponents)) );
      if ( _ltlMode == PAR_PRO_LTL_MODE_MINE
	   && (_numVerified >= _numRequiredVerifiable && _numFalsified >= _numRequiredFalsifiable) ) {
	break;
      }
    }
  } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_NONE) {
    pair<ParProTransitionGraph*, SelectedCfgsAndIdMap*> stgAndSelectedComponents = exploreOnce();
    ParProTransitionGraph* stgPointer = stgAndSelectedComponents.first;
    ROSE_ASSERT(stgPointer);
    _properties->append( *(ltlAnalysis(stgAndSelectedComponents)) );
  }
}

PropertyValueTable* ParProExplorer::propertyValueTable() {
  return _properties;
}

pair<ParProTransitionGraph*, SelectedCfgsAndIdMap*> ParProExplorer::exploreOnce() {
  ParProTransitionGraph* transitionGraph = new ParProTransitionGraph();
  SelectedCfgsAndIdMap* components = NULL;
  ParProAnalyzer parProAnalyzer;
  if (_componentSelection == PAR_PRO_COMPONENTS_ALL) {
    parProAnalyzer.init(_cfgs);
  } else if (_componentSelection == PAR_PRO_COMPONENTS_SUBSET_FIXED) {
    boost::unordered_map<int, int> cfgIdToStateIndex;
    components = new SelectedCfgsAndIdMap(componentSubset(_fixedComponentIds));
    parProAnalyzer.init(components->first, components->second);
  } else if (_componentSelection == PAR_PRO_COMPONENTS_SUBSET_RANDOM) {
    set<int> randomIds = randomSetNonNegativeInts(_numRandomComponents, (((int)_cfgs.size()) - 1)); 
    components = new SelectedCfgsAndIdMap(componentSubset(randomIds));
    parProAnalyzer.init(components->first, components->second);
  }
  parProAnalyzer.setAnnotationMap(_annotationMap);
  parProAnalyzer.initializeSolver();
  parProAnalyzer.runSolver();
  transitionGraph = parProAnalyzer.getTransitionGraph();
  return pair<ParProTransitionGraph*, SelectedCfgsAndIdMap*>(transitionGraph, components);
}

set<int> ParProExplorer::randomSetNonNegativeInts(int size, int maxInt) {
  set<int> result;
  for (int i = 0; i < size; i++) {
    int val = rand() % (maxInt + 1);
    //draw a new number in case the randomly selected one has been drawn already
    while (result.find(val) != result.end()) {
      val = rand() % (maxInt + 1);
    }
    result.insert(val);
  }
  return result;
}

SelectedCfgsAndIdMap ParProExplorer::componentSubset(set<int> componentIds) {
  boost::unordered_map<int, int> cfgIdToStateIndex;
  int stateIndex = 0;
  vector<Flow> componentSubset(_fixedComponentIds.size());
  for (set<int>::iterator i=componentIds.begin(); i!=componentIds.end(); i++) {
    componentSubset[stateIndex] = _cfgs[*i];
    cfgIdToStateIndex[*i] = stateIndex;
    stateIndex++;
  }
  return SelectedCfgsAndIdMap(componentSubset, cfgIdToStateIndex);
}

string ParProExplorer::getLtlsAsPromelaCode() {
  return "";
#if 0
  SpotConnection spotConnectionForSpinOutput;
  stringstream propertiesSpinSyntax;
  int propertyId = 0;
  for (list<pair<string, PropertyValue> >::iterator i=_properties.begin(); i!=_properties.end(); i++) {
    propertiesSpinSyntax << "ltl p"<<propertyId<<"\t { "<<spotConnectionForSpinOutput.spinSyntax(i->first)<<" }";
    if (i->second == PROPERTY_VALUE_YES) {
      propertiesSpinSyntax << "\t /* true */" << endl;
    } else if (i->second == PROPERTY_VALUE_NO) {
      propertiesSpinSyntax << "\t /* false */" << endl;
    } else {
      cerr << "ERROR: reporting a mined property for which it is unknown whether or not it is valid." << endl;
      ROSE_ASSERT(0);
    }
    propertyId++;
  }
  return propertiesSpinSyntax.str();
#endif
}

string ParProExplorer::getLtlsAsString() {
  return "";
}
