// Author: Marc Jasper, 2016.

#include "ParProLtlMiner.h"
#include "ParProExplorer.h"
#include "PropertyValueTable.h"
#include "SpotConnection.h"
#include "Miscellaneous2.h"

using namespace CodeThorn;
using namespace CodeThorn;
using namespace boost;
using namespace std;

ParallelSystem::ParallelSystem() :
_stg(NULL),
_stgOverApprox(NULL), 
_stgUnderApprox(NULL) {
}

void ParallelSystem::deleteStgs() {
  if (_stg)
    _stg->deleteStates();
    delete _stg;
  if (_stgOverApprox)
    _stgOverApprox->deleteStates();
    delete _stgOverApprox;
  if (_stgUnderApprox)
    _stgUnderApprox->deleteStates();
    delete _stgUnderApprox;
}

set<int> ParallelSystem::getComponentIds() const {
  set<int> result;
  for (map<int, Flow*>::const_iterator i=_components.begin(); i!=_components.end(); ++i) {
    result.insert((*i).first);
  }
  return result;
}

set<string> ParallelSystem::getAnnotations() const {
  set<string> result;
  for (map<int, Flow*>::const_iterator i=_components.begin(); i!=_components.end(); ++i) {
    set<string> annotations = (*i).second->getAllAnnotations();
    result.insert(annotations.begin(), annotations.end());
  }
  // TODO: improve representation so that the empty string does not need to be treated special
  set<string>::iterator iter = result.find("");
  if (iter != result.end()) {
    result.erase(iter);
  }
  return result;
}

set<string> ParallelSystem::getAnnotations(list<int> componentIds) {
  set<string> result;
  for (list<int>::iterator i=componentIds.begin(); i!=componentIds.end(); ++i) {
    set<string> annotations = _components[*i]->getAllAnnotations();
    result.insert(annotations.begin(), annotations.end());
  }
  // TODO: improve representation so that the empty string does not need to be treated special
  set<string>::iterator iter = result.find("");
  if (iter != result.end()) {
    result.erase(iter);
  }
  return result;
}

void ParallelSystem::addComponent(int id, Flow* cfa) {
  _components.insert(pair<int, Flow*>(id, cfa));
}

string ParallelSystem::toString() const {
  stringstream ss;
  ss << "ids: ";
  for (map<int, Flow*>::const_iterator i=_components.begin(); i!=_components.end(); ++i) {
    if (i != _components.begin()) {
      ss << ",";
    }
    ss << (*i).first;
  }
  return ss.str();
}

EdgeAnnotationMap ParallelSystem::edgeAnnotationMap() {
  EdgeAnnotationMap result;
  for (map<int, Flow*>::const_iterator i=_components.begin(); i!=_components.end(); ++i) {
    Flow* component = (*i).second;
    for (Flow::iterator k=component->begin(); k!= component->end(); ++k) {
      string annotation = (*k).getAnnotation();
      if (result.find(annotation) == result.end()) {
	boost::unordered_map<int, list<Edge> > newMap;
	result[annotation] = newMap;
      }
      boost::unordered_map<int, list<Edge> > occurrences = result[annotation];
      if (occurrences.find((*i).first) == occurrences.end()) {
	list<Edge> newList;
	occurrences[(*i).first] = newList;
      }
      list<Edge> newList = occurrences[(*i).first];
      newList.push_back(*k);
      occurrences[(*i).first] = newList;
      result[annotation] = occurrences;
    }
  }
  return result;
}

// define order for ParallelSystems
bool CodeThorn::operator<(const ParallelSystem& p1, const ParallelSystem& p2) { 
  if (p1.size()!= p2.size()) {
    return p1.size() < p2.size();
  }
  for (map<int, Flow*>::iterator i1=p1.components().begin(), i2=p2.components().begin(); 
       i1!=p1.components().end(); 
       (++i1, ++i2)) {
    if ((*i1).first != (*i2).first) {
      return (*i1).first < (*i2).first;
    }
  }
return false;
}

bool CodeThorn::operator==(const ParallelSystem& p1, const ParallelSystem& p2) {
  if (p1.size()!= p2.size()) {
    return false;
  }
  for (map<int, Flow*>::iterator i1=p1.components().begin(), i2=p2.components().begin(); 
       i1!=p1.components().end(); 
       (++i1, ++i2)) {
    if ((*i1).first != (*i2).first) {
      return false;
    }
  }
  return true;
}

bool CodeThorn::operator!=(const ParallelSystem& p1, const ParallelSystem& p2) {
  return !(p1==p2);
}

pair<string, string> ParProLtlMiner::randomLtlFormula(set<string> atomicPropositions) {
  return randomTemporalFormula(atomicPropositions, 1);
}

pair<string, string> ParProLtlMiner::randomTemporalFormula(set<string>& atomicPropositions, int temporalDepth, bool withOuterParens) {
  ROSE_ASSERT(temporalDepth >= 1 &&  temporalDepth <= 2);  // currently using fixed parameters, other case should not occur
  string formula = "";
  string description = "";
  int production = randomIntInRange(pair<int,int>(0,3));
  int numberDisOrConjuncted;
  if (temporalDepth > 1) {
    numberDisOrConjuncted = 1;
  } else {
    numberDisOrConjuncted = randomIntInRange(pair<int,int>(1,2));
  }
  switch(production) {
  case 0 :  {  // G( A and A …)
    bool useInnerParens = numberDisOrConjuncted == 1 ? false : true;
    formula += "G";
    formula += "(";
    description = "(it generally holds that ";
    for (int i = 0; i < numberDisOrConjuncted; ++i) {
      if (i != 0) {
	formula += " & ";
        description += " and ";
      }
      pair<string,string> frequentEvent = randomFrequentEventFormula(atomicPropositions, temporalDepth, useInnerParens);
      formula += frequentEvent.first;
      description += frequentEvent.second;
    }
    formula += ")";
    description += ")";
    break;
  }
  case 1 :  {  // F( X and X…) 
    formula += "F(";
    description = "(the following will finally happen: ";
    pair<string,string> rareEvent = randomRareEventFormula(atomicPropositions, temporalDepth, false);
    formula += rareEvent.first;
    description += rareEvent.second;
    formula += ")";
    description += ")";
    break;
  }
  case 2 :  {  // ( A U ( b or c …) )
    if (withOuterParens) {
      formula = "(";
      description += "(";
    }
    pair<string,string> frequentEvent = randomFrequentEventFormula(atomicPropositions, temporalDepth);
    formula += frequentEvent.first;
    description += frequentEvent.second;
    formula += " U ";
    description += " holds until ";
    if (numberDisOrConjuncted > 1) {
      formula += "(";
    }
    for (int i = 0; i < numberDisOrConjuncted; ++i) {
      if (i != 0) {
	formula += " | ";
        description += " or ";
      }
      string atomicProposition = randomAtomicProposition(atomicPropositions);
      formula += atomicProposition;
      description += "\"" + atomicProposition + "\"";
    }
    if (numberDisOrConjuncted > 1) {
      description += " are triggered";
      formula += ")";
    } else {
      description += " is triggered";
    }
    if (withOuterParens) {
      formula += ")";
      description += ")";
    }
    break;
  }
  case 3 :  {  // ( A W ( b or c …) )    (weak until)
    if (withOuterParens) {
      formula = "(";
    }
    description += "(";
    pair<string,string> frequentEvent = randomFrequentEventFormula(atomicPropositions, temporalDepth);
    formula += frequentEvent.first;
    description += frequentEvent.second;
    formula += " W ";
    description += " holds until ";
    if (numberDisOrConjuncted > 1) {
      formula += "(";
    }
    for (int i = 0; i < numberDisOrConjuncted; ++i) {
      if (i != 0) {
	formula += " | ";
        description += " or ";
      }
      string atomicProposition = randomAtomicProposition(atomicPropositions);
      formula += atomicProposition;
      description += "\"" + atomicProposition + "\"";
    }
    if (numberDisOrConjuncted > 1) {
      description += " are triggered";
      formula += ")";
    } else {
      description += " is triggered";
    }
    if (withOuterParens) {
      formula += ")";
    }
    description += ", or the second term never holds)";
    break;
  }
  }
  return pair<string,string>(formula, description);
}


pair<string, string> ParProLtlMiner::randomFrequentEventFormula(set<string>& atomicPropositions, int temporalDepth, bool withOuterParens) {
  ROSE_ASSERT(temporalDepth >= 1 &&  temporalDepth <= 2);  // currently using fixed parameters, other case should not occur
  string formula = "";
  string description = "";
  int production;
  int numberDisOrConjuncted;
  if (temporalDepth > 1) {
    production = 0;
    numberDisOrConjuncted = 1;
  } else {
    production = randomIntInRange(pair<int,int>(0,1));
    numberDisOrConjuncted = randomIntInRange(pair<int,int>(1,2));
  }
  switch(production) {
  case 0 :  {  //  !(a or b …) 
    formula = "!";
    description += "a transition other than ";
    if (numberDisOrConjuncted > 1) {
      formula += "(";
    }
    for (int i = 0; i < numberDisOrConjuncted; ++i) {
      if (i != 0) {
	formula += " | ";
        description += " or ";
      }
      string atomicProposition = randomAtomicProposition(atomicPropositions);
      atomicPropositions.erase(atomicProposition);
      formula += atomicProposition;
      description += "\"" + atomicProposition + "\"";
    }
    if (numberDisOrConjuncted > 1) {
      formula += ")";
    }
    description += " is triggered";
    break;
  }
  case 1 :  {  // ( a => S )
    if (withOuterParens) {
      formula = "(";
      description += "(";
    }
    string atomicProposition = randomAtomicProposition(atomicPropositions);
    atomicPropositions.erase(atomicProposition);
    formula += atomicProposition;
    description += "\"" + atomicProposition + "\"";
    formula += " => ";
    description += " being triggered implies that ";
    pair<string,string> temporalFormula = randomTemporalFormula(atomicPropositions, temporalDepth + 1);
    formula += temporalFormula.first;
    description += temporalFormula.second;
    if (withOuterParens) {
      formula += ")";
      description += ")";
    }
    break;
  }
  }
  return pair<string,string>(formula, description);
}

pair<string, string> ParProLtlMiner::randomRareEventFormula(set<string>& atomicPropositions, int temporalDepth, bool withOuterParens) {
  ROSE_ASSERT(temporalDepth >= 1 &&  temporalDepth <= 2);  // currently using fixed parameters, other case should not occur
  string formula = "";
  string description = "";
  int production;
  if (temporalDepth > 1) {
    production = 0;
  } else {
    production = randomIntInRange(pair<int,int>(0,2));
  }
  switch(production) {
  case 0 :  {  //  a 
    string atomicProposition = randomAtomicProposition(atomicPropositions);
    formula += atomicProposition;
    description += "\"" + atomicProposition + "\" is triggered";
    break;
  }
  case 1 :  {  // a & S
    string atomicProposition = randomAtomicProposition(atomicPropositions);
    atomicPropositions.erase(atomicProposition);
    formula += atomicProposition;
    description += "\"" + atomicProposition + "\"";
    formula += " & ";
    description += " is triggered and ";
    pair<string,string> temporalFormula = randomTemporalFormula(atomicPropositions, temporalDepth + 1);
    formula += temporalFormula.first;
    description += temporalFormula.second;
    break;
  }
  case 2 :  {  // S
    pair<string,string> temporalFormula = randomTemporalFormula(atomicPropositions, temporalDepth + 1, false);
    formula += temporalFormula.first;
    description += temporalFormula.second;
    break;
  }
  }
  return pair<string,string>(formula, description);
}

string ParProLtlMiner::randomAtomicProposition(set<string>& atomicPropositions) {
  if (atomicPropositions.size() == 0) { //TODO: implement in a (renamed) wrapper function
    atomicPropositions.insert("true");  // default if no unused labels are left
  }
  int index = randomIntInRange( pair<int,int>(0, (atomicPropositions.size() - 1)) );
  set<string>::iterator iter = atomicPropositions.begin();
  for (int i=0; i<index; ++i) {
    ++iter;
  }
  return *iter;
}


string ParProLtlMiner::randomLtlFormula(vector<string> atomicPropositions, int maxProductions) {
  int numberOfRules = 10; // Note: this has to reflect the number of different case statements in the switch-case block below.
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

PropertyValueTable* ParProLtlMiner::mineProperties(ParallelSystem& system, int minNumComponents) {
  ROSE_ASSERT(system.hasStgOverApprox() && system.hasStgUnderApprox());
  PropertyValueTable* result = new PropertyValueTable();
  set<string> annotations;
  vector<string> annotationVec;
  ROSE_ASSERT(_numComponentsForLtlAnnotations <= system.size());
  if (_numComponentsForLtlAnnotations == system.size()) {
    annotations = system.getAnnotations();
    annotationVec.reserve(annotations.size());
    copy(annotations.begin(), annotations.end(), back_inserter(annotationVec));
  }
  for (unsigned int i = 0; i < _numberOfMiningsPerSubsystem; ++i) {
    if (_numComponentsForLtlAnnotations < system.size()) {
      pair<int,int> range(0, (system.size() - 1));
      list<int> componentIdsForAnnotations = nDifferentRandomIntsInSet(_numComponentsForLtlAnnotations, system.getComponentIds());
      annotations = system.getAnnotations(componentIdsForAnnotations);
      annotationVec.clear();
      annotationVec.reserve(annotations.size());
      copy(annotations.begin(), annotations.end(), back_inserter(annotationVec));
    }
    string ltlProperty = randomLtlFormula(annotations).first;
    if (_spotConnection.checkPropertyParPro(ltlProperty, *system.stgOverApprox(), system.getAnnotations()) == PROPERTY_VALUE_YES) {
      bool passedFilter; 
      if (_storeComputedSystems) {
	const ParallelSystem* systemPtr = _subsystems.processNewOrExisting(system);
	passedFilter = passesFilter(ltlProperty, PROPERTY_VALUE_YES, systemPtr, minNumComponents);
      } else {
	passedFilter = passesFilter(ltlProperty, PROPERTY_VALUE_YES, system, minNumComponents);
      }
      if (passedFilter) {
	result->addProperty(ltlProperty, PROPERTY_VALUE_YES);
	result->setAnnotation(result->getPropertyNumber(ltlProperty), system.toString());
      }
    } else if (_spotConnection.checkPropertyParPro(ltlProperty, *system.stgUnderApprox(), system.getAnnotations()) == PROPERTY_VALUE_NO) {
      bool passedFilter;
      if (_storeComputedSystems) {
	const ParallelSystem* systemPtr = _subsystems.processNewOrExisting(system);
	passedFilter = passesFilter(ltlProperty, PROPERTY_VALUE_NO, systemPtr, minNumComponents);
      } else {
	passedFilter = passesFilter(ltlProperty, PROPERTY_VALUE_NO, system, minNumComponents);
      }
      if (passedFilter) {
	result->addProperty(ltlProperty, PROPERTY_VALUE_NO);
	result->setAnnotation(result->getPropertyNumber(ltlProperty), system.toString());
      }
    }
  }
  return result;
}

PropertyValueTable* ParProLtlMiner::mineProperties(ParallelSystem& system, int minNumComponents, int minNumVerifiable, int minNumFalsifiable) {
  ROSE_ASSERT(system.hasStgOverApprox() && system.hasStgUnderApprox());
  PropertyValueTable* result = new PropertyValueTable();
  int verifiableCount = 0;
  int falsifiableCount = 0;
  set<string> annotations;
  vector<string> annotationVec;
  ROSE_ASSERT(_numComponentsForLtlAnnotations <= system.size());
  if (_numComponentsForLtlAnnotations == system.size()) {
    annotations = system.getAnnotations();
    annotationVec.reserve(annotations.size());
    copy(annotations.begin(), annotations.end(), back_inserter(annotationVec));
  }
  for (unsigned int i = 0; i < _numberOfMiningsPerSubsystem; ++i) {
    if (_numComponentsForLtlAnnotations < system.size()) {
      pair<int,int> range(0, (system.size() - 1));
      list<int> componentIdsForAnnotations = nDifferentRandomIntsInSet(_numComponentsForLtlAnnotations, system.getComponentIds());
      annotations = system.getAnnotations(componentIdsForAnnotations);
    }
    pair<string, string> ltlProperty = randomLtlFormula(annotations);
    string ltlFormula = ltlProperty.first;
    if (verifiableCount < minNumVerifiable
	&& _spotConnection.checkPropertyParPro(ltlFormula, *system.stgOverApprox(), system.getAnnotations()) == PROPERTY_VALUE_YES) {
      bool passedFilter; 
      if (_storeComputedSystems) {
	const ParallelSystem* systemPtr = _subsystems.processNewOrExisting(system);
	passedFilter = passesFilter(ltlFormula, PROPERTY_VALUE_YES, systemPtr, minNumComponents);
      } else {
	passedFilter = passesFilter(ltlFormula, PROPERTY_VALUE_YES, system, minNumComponents);
      }
      if (passedFilter) {
	result->addProperty(ltlFormula, PROPERTY_VALUE_YES);
	result->setAnnotation(result->getPropertyNumber(ltlFormula), system.toString());
	verifiableCount++;
      }
    } else if (falsifiableCount < minNumFalsifiable
	       && _spotConnection.checkPropertyParPro(ltlFormula, *system.stgUnderApprox(), system.getAnnotations()) == PROPERTY_VALUE_NO) {
      bool passedFilter;
      if (_storeComputedSystems) {
	const ParallelSystem* systemPtr = _subsystems.processNewOrExisting(system);
	passedFilter = passesFilter(ltlFormula, PROPERTY_VALUE_NO, systemPtr, minNumComponents);
      } else {
	passedFilter = passesFilter(ltlFormula, PROPERTY_VALUE_NO, system, minNumComponents);
      }
      if (passedFilter) {
	result->addProperty(ltlFormula, PROPERTY_VALUE_NO);
	result->setAnnotation(result->getPropertyNumber(ltlFormula), system.toString());
	falsifiableCount++;
      }
    }
  }
  return result;
}

PropertyValueTable* ParProLtlMiner::minePropertiesLtsMin(ParallelSystem& system, int minNumComponents, int minNumVerifiable, int minNumFalsifiable) {
  PropertyValueTable* result = new PropertyValueTable();
  int verifiableCount = 0;
  int falsifiableCount = 0;
  set<string> annotations;
  vector<string> annotationVec;
  ROSE_ASSERT(_numComponentsForLtlAnnotations <= system.size());
  if (_numComponentsForLtlAnnotations == system.size()) {
    annotations = system.getAnnotations();
    annotationVec.reserve(annotations.size());
    copy(annotations.begin(), annotations.end(), back_inserter(annotationVec));
  }
  for (unsigned int i = 0; i < _numberOfMiningsPerSubsystem; ++i) {
    if (_numComponentsForLtlAnnotations < system.size()) {
      pair<int,int> range(0, (system.size() - 1));
      list<int> componentIdsForAnnotations = nDifferentRandomIntsInSet(_numComponentsForLtlAnnotations, system.getComponentIds());
      annotations = system.getAnnotations(componentIdsForAnnotations);
    }
    string ltlFormula;
    pair<string, string> ltlProperty = randomLtlFormula(annotations);
    ltlFormula = ltlProperty.first;
    string ltlFormulaLtsmin = _ltsminConnection.ltlFormula2LtsminSyntax(ltlFormula);
    if (verifiableCount < minNumVerifiable &&
	_ltsminConnection.checkPropertyParPro(ltlFormulaLtsmin, system.components()) == PROPERTY_VALUE_YES) {
      //      if (passesFilterLtsMin(ltlFormulaLtsmin, PROPERTY_VALUE_YES, system, minNumComponents)) {
	result->addProperty(ltlFormula, PROPERTY_VALUE_YES);
	result->setAnnotation(result->getPropertyNumber(ltlFormula), system.toString());
	verifiableCount++;
	//      }
    } else if (falsifiableCount < minNumFalsifiable &&
	       _ltsminConnection.checkPropertyParPro(ltlFormulaLtsmin, system.components()) == PROPERTY_VALUE_NO) {
      //      if (passesFilterLtsMin(ltlFormulaLtsmin, PROPERTY_VALUE_NO, system, minNumComponents)) {
	result->addProperty(ltlFormula, PROPERTY_VALUE_NO);
	result->setAnnotation(result->getPropertyNumber(ltlFormula), system.toString());
	falsifiableCount++;
	//      }
    }
  }
  return result;
}

bool ParProLtlMiner::passesFilterLtsMin(string ltlProperty, PropertyValue correctValue, ParallelSystem& system, int minNumComponents) {
  ROSE_ASSERT(correctValue != PROPERTY_VALUE_UNKNOWN);
  ROSE_ASSERT(system.size() >= (unsigned) minNumComponents);
  if (system.size() == 1) {
    return true;
  }
  list<ParallelSystem> worklist;
  list<ParallelSystem> subsystems = initiateSubsystemsOf(system);
  for (list<ParallelSystem>::iterator i=subsystems.begin(); i!=subsystems.end(); ++i) {
    worklist.push_back(*i);
  }
  while (!worklist.empty()) {
    ParallelSystem subsystem = worklist.front();
    worklist.pop_front();
    PropertyValue resultSubsystemApprox = _ltsminConnection.checkPropertyParPro(ltlProperty, subsystem.components());
    if (resultSubsystemApprox == correctValue) {
      if ((int) subsystem.size() < minNumComponents) {
	// this property can be correctly assessed with less components than required. Discard the property
	return false;
      }
      list<ParallelSystem> subsubsystems = initiateSubsystemsOf(subsystem);
      for (list<ParallelSystem>::iterator i=subsubsystems.begin(); i!=subsubsystems.end(); ++i) {
	worklist.push_back(*i);
      }
    }
  }
  return true;
}

bool ParProLtlMiner::passesFilter(string ltlProperty, PropertyValue correctValue, const ParallelSystem* system, int minNumComponents) {
  ROSE_ASSERT(correctValue != PROPERTY_VALUE_UNKNOWN);
  ComponentApproximation approxMode;
  if (correctValue == PROPERTY_VALUE_YES) {
    approxMode = COMPONENTS_OVER_APPROX;
  } else {
    approxMode = COMPONENTS_UNDER_APPROX;
  }
  list<const ParallelSystem*> worklist;
  exploreSubsystemsAndAddToWorklist(system, approxMode, worklist);
  while (!worklist.empty()) {
    const ParallelSystem* subsystem = worklist.front();
    worklist.pop_front();
    ParProTransitionGraph* stgApprox;
    if (approxMode == COMPONENTS_OVER_APPROX) {
      stgApprox = (const_cast<ParallelSystem*>(subsystem))->stgOverApprox();
    } else {
      stgApprox = (const_cast<ParallelSystem*>(subsystem))->stgUnderApprox();
    }
    PropertyValue resultSubsystemApprox = _spotConnection.checkPropertyParPro(ltlProperty, *stgApprox, subsystem->getAnnotations());
    if (resultSubsystemApprox == correctValue) {
      if ((int) subsystem->size() < minNumComponents) {
	// this property can be correctly assessed with less components than required. Discard the property
	return false;
      }
      exploreSubsystemsAndAddToWorklist(subsystem, approxMode, worklist);
    }
  }
  return true;
}

bool ParProLtlMiner::passesFilter(string ltlProperty, PropertyValue correctValue, ParallelSystem& system, int minNumComponents) {
  ROSE_ASSERT(correctValue != PROPERTY_VALUE_UNKNOWN);
  ComponentApproximation approxMode;
  if (correctValue == PROPERTY_VALUE_YES) {
    approxMode = COMPONENTS_OVER_APPROX;
  } else {
    approxMode = COMPONENTS_UNDER_APPROX;
  }
  list<ParallelSystem> worklist;
  exploreSubsystemsAndAddToWorklist(system, approxMode, worklist);
  while (!worklist.empty()) {
    ParallelSystem subsystem = worklist.front();
    worklist.pop_front();
    ParProTransitionGraph* stgApprox;
    if (approxMode == COMPONENTS_OVER_APPROX) {
      stgApprox = subsystem.stgOverApprox();
    } else {
      stgApprox = subsystem.stgUnderApprox();
    }
    PropertyValue resultSubsystemApprox = _spotConnection.checkPropertyParPro(ltlProperty, *stgApprox, subsystem.getAnnotations());
    if (resultSubsystemApprox == correctValue) {
      if ((int) subsystem.size() < minNumComponents) {
	// this property can be correctly assessed with less components than required. Discard the property
	for (list<ParallelSystem>::iterator i=worklist.begin(); i!=worklist.end(); ++i) {
	  (*i).deleteStgs();
	}
	return false;
      }
      exploreSubsystemsAndAddToWorklist(subsystem, approxMode, worklist);
    }
    subsystem.deleteStgs();
  }
  return true;
}

void ParProLtlMiner::exploreSubsystemsAndAddToWorklist(const ParallelSystem* system, 
						       ComponentApproximation approxMode, list<const ParallelSystem*>& worklist) {
  if (_subsystemsOf.find(system) == _subsystemsOf.end()) {
    initiateSubsystemsOf(system); // compute the successors if they do not exist in the DAG yet
  }
  ParallelSystemDag::iterator iter = _subsystemsOf.find(system);
  ROSE_ASSERT(iter != _subsystemsOf.end());
  list<const ParallelSystem*> subsystemsPtrs = (*iter).second;
  // compute the required approximated STGs if they do not exist yet
  for (list<const ParallelSystem*>::iterator i=subsystemsPtrs.begin(); i!=subsystemsPtrs.end(); ++i) {
    if (approxMode == COMPONENTS_OVER_APPROX) {
      if (!((*i)->hasStgOverApprox())) {
	_parProExplorer->computeStgApprox(*const_cast<ParallelSystem*>(*i), approxMode);
      }
    } else {
      if (!((*i)->hasStgUnderApprox())) {
	_parProExplorer->computeStgApprox(*const_cast<ParallelSystem*>(*i), approxMode);
      }
    }
  }
  for (list<const ParallelSystem*>::iterator i=subsystemsPtrs.begin(); i!=subsystemsPtrs.end(); ++i) {
    worklist.push_back(*i);
  }
}

void ParProLtlMiner::exploreSubsystemsAndAddToWorklist(ParallelSystem& system, 
						       ComponentApproximation approxMode, list<ParallelSystem>& worklist) {
  list<ParallelSystem> subsystems = initiateSubsystemsOf(system); 
  // compute the required approximated STGs
  for (list<ParallelSystem>::iterator i=subsystems.begin(); i!=subsystems.end(); ++i) {
    if (approxMode == COMPONENTS_OVER_APPROX) {
      ROSE_ASSERT(!((*i).hasStgOverApprox()));
    } else {
      ROSE_ASSERT(!((*i).hasStgUnderApprox()));
    }
    _parProExplorer->computeStgApprox((*i), approxMode);
  }
  for (list<ParallelSystem>::iterator i=subsystems.begin(); i!=subsystems.end(); ++i) {
    worklist.push_back(*i);
  }
}

void ParProLtlMiner::initiateSubsystemsOf(const ParallelSystem* system) {
  ROSE_ASSERT(_subsystemsOf.find(system) == _subsystemsOf.end());
  list<const ParallelSystem*> subsystems;
  map<int, Flow*> components = system->components();
  list<map<int, Flow*> > subsystemComponentsList;
  for (map<int, Flow*>::iterator i=components.begin(); i!=components.end(); ++i) {
    map<int, Flow*> subsystemComponents = components;
    subsystemComponents.erase((*i).first);
    subsystemComponentsList.push_back(subsystemComponents);
  }
  for (list<map<int, Flow*> >::iterator i=subsystemComponentsList.begin(); i!=subsystemComponentsList.end(); ++i) {
    ParallelSystem stub;
    stub.setComponents(*i);
    const ParallelSystem* subsystem = _subsystems.processNewOrExisting(stub);
    subsystems.push_back(subsystem);
  }
  _subsystemsOf[system] = subsystems;
}

list<ParallelSystem> ParProLtlMiner::initiateSubsystemsOf(ParallelSystem& system) {
  list<ParallelSystem> result;
  map<int, Flow*> components = system.components();
  list<map<int, Flow*> > subsystemComponentsList;
  for (map<int, Flow*>::iterator i=components.begin(); i!=components.end(); ++i) {
    map<int, Flow*> subsystemComponents = components;
    subsystemComponents.erase((*i).first);
    subsystemComponentsList.push_back(subsystemComponents);
  }
  for (list<map<int, Flow*> >::iterator i=subsystemComponentsList.begin(); i!=subsystemComponentsList.end(); ++i) {
    ParallelSystem stub;
    stub.setComponents(*i);
    result.push_back(stub);
  }
  return result;
}


