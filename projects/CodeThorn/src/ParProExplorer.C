// Author: Marc Jasper, 2016.

#include "ParProExplorer.h"

using namespace SPRAY;
using namespace CodeThorn;
using namespace std;

ParProExplorer::ParProExplorer(vector<Flow>& cfgs, EdgeAnnotationMap& annotationMap):
_cfgs(cfgs),
_annotationMap(annotationMap),
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

void ParProExplorer::explore() {
  if (_componentSelection == PAR_PRO_COMPONENTS_ALL) {
    if (_ltlMode == PAR_PRO_LTL_MODE_CHECK) {
      ParProAnalyzer parProAnalyzer(_cfgs);
      parProAnalyzer.setAnnotationMap(_annotationMap);
      parProAnalyzer.initializeSolver();
      parProAnalyzer.runSolver();
      ParProTransitionGraph* transitionGraph = parProAnalyzer.getTransitionGraph();
      PropertyValueTable* ltlResults;
      SpotConnection spotConnection(_ltlInputFilename);
      cout << "STATUS: generating LTL results"<<endl;
      bool withCounterexample = false;
      bool spuriousNoAnswers = false;
      spotConnection.checkLtlPropertiesParPro( *transitionGraph, withCounterexample, spuriousNoAnswers);
      ltlResults = spotConnection.getLtlResults();
      ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexample);
      cout << "=============================================================="<<endl;
      ltlResults->printResultsStatistics();
      cout << "=============================================================="<<endl;
      delete ltlResults;
      ltlResults = NULL;
    } else if (_ltlMode == PAR_PRO_LTL_MODE_MINE) {
      ParProLtlMiner miner(_cfgs, _annotationMap);
      _properties = miner.mineLtlProperties(5,5,3);
    }
  }
}

string ParProExplorer::getLtlsAsPromelaCode() {
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
}

string ParProExplorer::getLtlsAsString() {
  return "";
}
