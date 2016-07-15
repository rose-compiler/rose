// Author: Marc Jasper, 2016.

#include "ParProLtlMiner.h"
#include "ParProExplorer.h"
#include "PropertyValueTable.h"
#include "SpotConnection.h"

using namespace SPRAY;
using namespace boost;
using namespace std;

ParallelSystem::ParallelSystem() :
_stg(NULL),
_stgOverApprox(NULL), 
_stgUnderApprox(NULL) {
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

PropertyValueTable* ParProLtlMiner::mineProperties(ParallelSystem& system, int minNumComponents) {
  ROSE_ASSERT(system.hasStgOverApprox() && system.hasStgUnderApprox());
  PropertyValueTable* result = new PropertyValueTable();
  set<string> annotations = system.getAnnotations();
  vector<string> annotationVec;
  annotationVec.reserve(annotations.size());
  copy(begin(annotations), end(annotations), back_inserter(annotationVec));
  for (unsigned int i = 0; i < _numberOfMiningsPerSubsystem; ++i) {
    string ltlProperty = randomLtlFormula(annotationVec, 2);
    if (_spotConnection.checkPropertyParPro(ltlProperty, *system.stgOverApprox(), system.getAnnotations()) == PROPERTY_VALUE_YES) {
      const ParallelSystem* systemPtr = _subsystems.processNewOrExisting(system);
      bool passedFilter = passesFilter(ltlProperty, PROPERTY_VALUE_YES, systemPtr, minNumComponents);
      if (passedFilter) {
	result->addProperty(ltlProperty, PROPERTY_VALUE_YES);
	result->setAnnotation(result->getPropertyNumber(ltlProperty), system.toString());
      }
    } else if (_spotConnection.checkPropertyParPro(ltlProperty, *system.stgUnderApprox(), system.getAnnotations()) == PROPERTY_VALUE_NO) {
      const ParallelSystem* systemPtr = _subsystems.processNewOrExisting(system);
      bool passedFilter = passesFilter(ltlProperty, PROPERTY_VALUE_NO, systemPtr, minNumComponents);
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
  set<string> annotations = system.getAnnotations();
  vector<string> annotationVec;
  annotationVec.reserve(annotations.size());
  copy(begin(annotations), end(annotations), back_inserter(annotationVec));
  for (unsigned int i = 0; i < _numberOfMiningsPerSubsystem; ++i) {
    string ltlProperty = randomLtlFormula(annotationVec, 2);
    if (verifiableCount < minNumVerifiable
	&& _spotConnection.checkPropertyParPro(ltlProperty, *system.stgOverApprox(), system.getAnnotations()) == PROPERTY_VALUE_YES) {
      const ParallelSystem* systemPtr = _subsystems.processNewOrExisting(system);
      bool passedFilter = passesFilter(ltlProperty, PROPERTY_VALUE_YES, systemPtr, minNumComponents);
      if (passedFilter) {
	result->addProperty(ltlProperty, PROPERTY_VALUE_YES);
	result->setAnnotation(result->getPropertyNumber(ltlProperty), system.toString());
	verifiableCount++;
      }
    } else if (falsifiableCount < minNumFalsifiable
	       && _spotConnection.checkPropertyParPro(ltlProperty, *system.stgUnderApprox(), system.getAnnotations()) == PROPERTY_VALUE_NO) {
      const ParallelSystem* systemPtr = _subsystems.processNewOrExisting(system);
      bool passedFilter = passesFilter(ltlProperty, PROPERTY_VALUE_NO, systemPtr, minNumComponents);
      if (passedFilter) {
	result->addProperty(ltlProperty, PROPERTY_VALUE_NO);
	result->setAnnotation(result->getPropertyNumber(ltlProperty), system.toString());
	falsifiableCount++;
      }
    }
  }
  return result;
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

