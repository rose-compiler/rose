// Author: Marc Jasper, 2016.

#include "ParProLtlMiner.h"

using namespace SPRAY;
using namespace std;


string ParProLtlMiner::randomLtlFormula(vector<string> atomicPropositions, int maxProductions) {
  int numberOfRules = 10; // Note: this has to reflect the number of different case statements in the swtich-case block below.
  int production;
  if (maxProductions == 0) {
    production = 9;
  } else {
    production = rand() % numberOfRules;
  }
  switch(production) {
    case 0 :  {  return ("( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) 
	               + " & " 
		       + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )");
	      }
    case 1 :  {  return ("( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) 
	               + " | " 
		       + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )");
	      } 
    case 2 :  {  return ("( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) 
	               + " -> " 
		       + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )");
	      }
    case 3 :  {  return "F( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )" ;
	      } 
    case 4 :  {  return "G( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )" ;
	      } 
    case 5 :  {  return ("( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) 
	               + " U " 
		       + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )");
	      }
    case 6 :  {  return ("( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) 
	               + " W " 
		       + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )");
	      }
    case 7 :  {  return ("( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) 
	               + " R " 
		       + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )");
	      } 
    case 8 :  {  return "!( " + randomLtlFormula(atomicPropositions, (maxProductions - 1)) + " )" ;
	      } 
    case 9 :  {  return "\"" + atomicPropositions[(rand() % atomicPropositions.size())] + "\"";
	      }
  }
  return "This string should never be returned but omits compiler warnings.";
}

bool ParProLtlMiner::isVerifiable(std::string ltlProperty, SelectedCfgsAndIdMap cfgsAndIdMap, EdgeAnnotationMap annotations) {
  return isExpectedResult(ltlProperty, cfgsAndIdMap, annotations, PROPERTY_VALUE_YES);
}

bool ParProLtlMiner::isFalsifiable(std::string ltlProperty, SelectedCfgsAndIdMap cfgsAndIdMap, EdgeAnnotationMap annotations) {
  return isExpectedResult(ltlProperty, cfgsAndIdMap, annotations, PROPERTY_VALUE_NO);
}

bool ParProLtlMiner::isExpectedResult(std::string ltlProperty, SelectedCfgsAndIdMap cfgsAndIdMap, EdgeAnnotationMap annotations, PropertyValue expectedResult) {
  ROSE_ASSERT(expectedResult != PROPERTY_VALUE_UNKNOWN);

  ParProAnalyzer parProAnalyzer(cfgsAndIdMap.first, cfgsAndIdMap.second);
  parProAnalyzer.setAnnotationMap(annotations);
  if (expectedResult == PROPERTY_VALUE_YES) {
    parProAnalyzer.setComponentApproximation(COMPONENTS_OVER_APPROX);
  } else {
    parProAnalyzer.setComponentApproximation(COMPONENTS_UNDER_APPROX);
  }
  parProAnalyzer.initializeSolver();
  parProAnalyzer.runSolver();

  list<string> ltlProperties;
  ltlProperties.push_back(ltlProperty);
  SpotConnection spotConnection(ltlProperties);
  ParProTransitionGraph* transitionGraph = parProAnalyzer.getTransitionGraph();
  spotConnection.checkLtlPropertiesParPro( *transitionGraph, false, false);
  PropertyValueTable* ltlResults = spotConnection.getLtlResults();

  if (expectedResult == PROPERTY_VALUE_YES) {
    return (ltlResults->getPropertyValue(0) == PROPERTY_VALUE_YES);
  } else {
    return (ltlResults->getPropertyValue(0) == PROPERTY_VALUE_NO);
  }
}

bool ParProLtlMiner::verifiableWithComponentSubset(std::string ltlProperty, SelectedCfgsAndIdMap cfgsAndIdMap, EdgeAnnotationMap annotations) {
  return expectedResultWithComponentSubset(ltlProperty, cfgsAndIdMap, annotations, PROPERTY_VALUE_YES);
}

bool ParProLtlMiner::falsifiableWithComponentSubset(std::string ltlProperty, SelectedCfgsAndIdMap cfgsAndIdMap, EdgeAnnotationMap annotations) {
  return expectedResultWithComponentSubset(ltlProperty, cfgsAndIdMap, annotations, PROPERTY_VALUE_NO);
}

bool ParProLtlMiner::expectedResultWithComponentSubset(std::string ltlProperty, SelectedCfgsAndIdMap cfgsAndIdMap, EdgeAnnotationMap annotations, PropertyValue expectedResult) {
  vector<Flow> cfgs = cfgsAndIdMap.first;
  boost::unordered_map<int, int> idMap = cfgsAndIdMap.second;
  for (unsigned int i = 0; i < cfgs.size(); i++) {
    // check if the ltl property is verifiable using all but the i'th CFG
    boost::unordered_map<int, int> cfgIdMapMinusOne;
    vector<Flow> cfgsMinusOne(cfgs.size() - 1);
    int n = 0;
    for (boost::unordered_map<int, int>::iterator k=idMap.begin(); k!=idMap.end(); k++) {
      if ((int) i != k->second) {
	cfgsMinusOne[n] = cfgs[k->second];
	cfgIdMapMinusOne[k->first] = n;
	n++;
      }
    }
    SelectedCfgsAndIdMap subsetCfgsAndIdMap(cfgsMinusOne, cfgIdMapMinusOne);
    if (isExpectedResult(ltlProperty, subsetCfgsAndIdMap, annotations, expectedResult)) {
      return true;
    }
  }
  return false;
}

list<pair<string, PropertyValue> > ParProLtlMiner::mineLtlProperties(int minNumVerifiable, int minNumFalsifiable, int minNumComponents) {
  srand(time(NULL));
  list<pair<string, PropertyValue> > ltlProperties;
  int verifiableCount = 0;
  int falsifiableCount = 0;
  while (verifiableCount < minNumVerifiable || falsifiableCount < minNumFalsifiable) {
    vector<Flow> selectedCfgs(minNumComponents);
    boost::unordered_map<int, int> cfgIdMap;
    set<string> annotationsSelectedCfgs;
    for (int i = 0; i < minNumComponents; i++) {
      int cfgId = rand() % _cfgs.size();
      //draw a new cfgId in case the randomly selected one has been drawn already
      while (cfgIdMap.find(cfgId) != cfgIdMap.end()) {
	cfgId = rand() % _cfgs.size();
      }
      selectedCfgs[i] = _cfgs[cfgId];
      cfgIdMap[cfgId] = i;
      set<string> annotations = _cfgs[cfgId].getAllAnnotations();
      annotationsSelectedCfgs.insert(annotations.begin(), annotations.end());
    }
    // do not consider the annotation to start one of the parallel components
    set<string>::iterator iter = annotationsSelectedCfgs.find("");
    if (iter != annotationsSelectedCfgs.end()) {
      annotationsSelectedCfgs.erase(iter);
    }
    vector<string> atomicPropositions;
    atomicPropositions.reserve(annotationsSelectedCfgs.size());
    copy(begin(annotationsSelectedCfgs), end(annotationsSelectedCfgs), back_inserter(atomicPropositions));
    for (unsigned int k = 0; k <= _numberOfMiningsPerSubsystem; k++) {
      string ltlProperty = randomLtlFormula(atomicPropositions, 2);
      if (isVerifiable(ltlProperty, SelectedCfgsAndIdMap(selectedCfgs, cfgIdMap), _annotations) ) {
	  //&& !(verifiableWithComponentSubset(ltlProperty, SelectedCfgsAndIdMap(selectedCfgs, cfgIdMap), _annotations)) ) {
	ltlProperties.push_back(pair<string, PropertyValue>(ltlProperty, PROPERTY_VALUE_YES));
	verifiableCount++;
      } else if (isFalsifiable(ltlProperty, SelectedCfgsAndIdMap(selectedCfgs, cfgIdMap), _annotations) ) {
		 //&& !(falsifiableWithComponentSubset(ltlProperty, SelectedCfgsAndIdMap(selectedCfgs, cfgIdMap), _annotations))) {
      	ltlProperties.push_back(pair<string, PropertyValue>(ltlProperty, PROPERTY_VALUE_NO));
      	falsifiableCount++;
      }
    }
  }
  cout << "DEBUG: LTL mining complete. Verifiable properties: " << verifiableCount << "  falsifiable properties: " << falsifiableCount << endl;
  return ltlProperties;
}
