#include "PropertyValueTable.h"
#include "CodeThornException.h"
#include "Miscellaneous2.h"

#include "rose_config.h"

// basic file operations
#include <iostream>
#include <fstream>
using namespace std;
#include <cassert>

using CodeThorn::color;

PropertyValueTable::PropertyValueTable() : _maximumId(0) {
}

PropertyValueTable::PropertyValueTable(size_t size) : _maximumId(size - 1) {
  init(size);
}

void PropertyValueTable::init(size_t size) {
  ROSE_ASSERT(_propertyValueTable.size() == 0);
  for(size_t i=0;i<size;i++) {
    _propertyValueTable[i] = PROPERTY_VALUE_UNKNOWN; 
    _formulas[i] = ""; 
    _counterexamples[i] = "";
  }
}

void PropertyValueTable::addProperty(string formula) {
  _maximumId++;
  _formulas[_maximumId] = formula;
  _idByFormula[formula] = _maximumId;
  _propertyValueTable[_maximumId] = PROPERTY_VALUE_UNKNOWN;
  _counterexamples[_maximumId] = "";
}

void PropertyValueTable::addProperty(string formula, PropertyValue value) {
  addProperty(formula);
  _propertyValueTable[_idByFormula[formula]] = value;
}

void PropertyValueTable::addProperty(string formula, PropertyValue value, string counterexample) {
  addProperty(formula, value);
  _counterexamples[_idByFormula[formula]] = counterexample;
}

void PropertyValueTable::addProperty(string formula, size_t id) {
  ROSE_ASSERT(_propertyValueTable.find(id) == _propertyValueTable.end());
  _maximumId = _maximumId < id ? id : _maximumId;
  _formulas[id] = formula;
  _idByFormula[formula] = id;
  _propertyValueTable[id] = PROPERTY_VALUE_UNKNOWN;
  _counterexamples[id] = "";
}

void PropertyValueTable::addProperty(string formula, size_t id, PropertyValue value) {
  addProperty(formula, id);
  _propertyValueTable[id] = value;
}

void PropertyValueTable::addProperty(string formula, size_t id, PropertyValue value, string counterexample) {
  addProperty(formula, id, value);
  _counterexamples[id] = counterexample;
}

void PropertyValueTable::append(PropertyValueTable& toBeAppended) {
  list<int>* idsOfSecondTable = toBeAppended.getPropertyNumbers();
  for (list<int>::iterator i=idsOfSecondTable->begin(); i!=idsOfSecondTable->end(); i++) {
    addProperty(toBeAppended.getFormula(*i), toBeAppended.getPropertyValue(*i), toBeAppended.getCounterexample(*i));
    if (toBeAppended.getAnnotation(*i) != "") {
      setAnnotation(getPropertyNumber(toBeAppended.getFormula(*i)), toBeAppended.getAnnotation(*i));
    }
  }
  delete idsOfSecondTable;
  idsOfSecondTable = NULL;
}

// TODO: check that every property with known result in "other" also exists in "*this" 
void PropertyValueTable::addResults(PropertyValueTable& other) {
  list<int>* idsOfSecondTable = other.getPropertyNumbers();
  for (list<int>::iterator i=idsOfSecondTable->begin(); i!=idsOfSecondTable->end(); i++) {
    if (other.getPropertyValue(*i) != PROPERTY_VALUE_UNKNOWN) {
      ROSE_ASSERT(getFormula(*i) == other.getFormula(*i));
      strictUpdatePropertyValue(*i, other.getPropertyValue(*i));
      setCounterexample(*i, other.getCounterexample(*i));
      if (other.getAnnotation(*i) != "") {
	setAnnotation(*i, other.getAnnotation(*i));
      }
    }
  }
  delete idsOfSecondTable;
  idsOfSecondTable = NULL;
}

void PropertyValueTable::reachable(size_t num) {
  setPropertyValue(num,PROPERTY_VALUE_YES);
}
void PropertyValueTable::nonReachable(size_t num) {
  setPropertyValue(num,PROPERTY_VALUE_NO);
}

void PropertyValueTable::updatePropertyValue(size_t num, PropertyValue value) {
  assert(num>=0 && num<_propertyValueTable.size());
  switch(getPropertyValue(num)) {
  case PROPERTY_VALUE_UNKNOWN:
    setPropertyValue(num,value);
  case PROPERTY_VALUE_YES:
    if(value!=PROPERTY_VALUE_YES)
      throw CodeThorn::Exception("Error: property value table: reset of YES.");
    break;
  case PROPERTY_VALUE_NO:
    if(value!=PROPERTY_VALUE_NO)
      throw CodeThorn::Exception("Error: property value table: reset of NO.");
    break;
  default:assert(0);
  }
}

void PropertyValueTable::strictUpdatePropertyValue(size_t num, PropertyValue value) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  if(getPropertyValue(num)==PROPERTY_VALUE_UNKNOWN) {
    setPropertyValue(num,value);
  } else {
    throw CodeThorn::Exception("Error: property value table: reset of value.");
  }
}

void PropertyValueTable::strictUpdateCounterexample(size_t num, string ce) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  if(_counterexamples[num]=="") {
    _counterexamples[num] = ce;
  } else {
    throw CodeThorn::Exception("Error: property value table: counterexample already exists.");
  }
}

void PropertyValueTable::setPropertyValue(size_t num, PropertyValue value) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  _propertyValueTable[num]=value;
}

void PropertyValueTable::setFormula(size_t num, string formula) {
  ROSE_ASSERT(_formulas.find(num) != _formulas.end());
  _formulas[num] = formula;
  _idByFormula[formula] = num;
}

void PropertyValueTable::setCounterexample(size_t num, string ce) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  _counterexamples[num] = ce;
}

void PropertyValueTable::setAnnotation(size_t num, string annotation) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  _annotations[num] = annotation;
}

PropertyValue PropertyValueTable::getPropertyValue(size_t num) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  return _propertyValueTable[num];
}

std::string PropertyValueTable::getFormula(size_t num) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  return _formulas[num];
}

std::string PropertyValueTable::getCounterexample(size_t num) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  return _counterexamples[num];
}

std::string PropertyValueTable::getAnnotation(size_t num) {
  ROSE_ASSERT(_propertyValueTable.find(num) != _propertyValueTable.end());
  if (_annotations.find(num) != _annotations.end()) {
    return _annotations[num];
  } else {
    return "";
  }
}

std::list<int>* PropertyValueTable::getPropertyNumbers() {
  std::list<int>* result = new list<int>();
  for (map<size_t, PropertyValue>::iterator i=_propertyValueTable.begin(); i!=_propertyValueTable.end(); i++) {
    result->push_back(i->first);
  }
  return result;
}

std::list<int>* PropertyValueTable::getPropertyNumbers(PropertyValue value) {
  std::list<int>* result = new list<int>();
  for (map<size_t, PropertyValue>::iterator i=_propertyValueTable.begin(); i!=_propertyValueTable.end(); i++) {
    switch(value) {
    case PROPERTY_VALUE_UNKNOWN: if (getPropertyValue(i->first) == PROPERTY_VALUE_UNKNOWN) { result->push_back(i->first); }; break;
    case PROPERTY_VALUE_YES: if (getPropertyValue(i->first) == PROPERTY_VALUE_YES) { result->push_back(i->first); }; break;
    case PROPERTY_VALUE_NO: if (getPropertyValue(i->first) == PROPERTY_VALUE_NO) { result->push_back(i->first); }; break;
    default:cerr<<"Error: unknown property value type."<<endl;assert(0);
    }
  }
  return result;
}

int PropertyValueTable::getPropertyNumber(string formula) {
  map<string, size_t>::iterator iter = _idByFormula.find(formula);
  ROSE_ASSERT(iter != _idByFormula.end());
  return iter->second;
}

string PropertyValueTable::reachToString(PropertyValue num) {
  switch(num) {
  case PROPERTY_VALUE_UNKNOWN: return "unknown"; //,0";
  case PROPERTY_VALUE_YES: return "yes"; //,9";
  case PROPERTY_VALUE_NO: return "no"; //,9";
  default: {
    cerr<<"Error: unknown reachability information.";
    assert(0);
  }
  }
  assert(0);
}
// we were able to compute the entire state space. All unknown become non-reachable (=no)
void PropertyValueTable::finished() {
  convertValue(PROPERTY_VALUE_UNKNOWN, PROPERTY_VALUE_NO);
}

void PropertyValueTable::finishedReachability(bool isPrecise, bool isComplete) {
  cout<<"STATUS: reachability finished: isPrecise: "<<isPrecise<<" isComplete: "<<isComplete<<endl;
  if(isPrecise&&isComplete) {
    convertValue(PROPERTY_VALUE_UNKNOWN, PROPERTY_VALUE_NO);
    return;
  }
  if(isPrecise&&!isComplete) {
    // yes remains yes and unknown remains unknown (no cannot exist)
    return;
  }
  if(!isPrecise&&isComplete) {
    convertValue(PROPERTY_VALUE_UNKNOWN, PROPERTY_VALUE_NO);
    convertValue(PROPERTY_VALUE_YES, PROPERTY_VALUE_UNKNOWN);
    return;
  }
  if(!isPrecise&&!isComplete) {
    convertValue(PROPERTY_VALUE_YES,PROPERTY_VALUE_UNKNOWN);
    convertValue(PROPERTY_VALUE_NO,PROPERTY_VALUE_UNKNOWN);
    return;
  }
  assert(0);
}

void PropertyValueTable::convertValue(PropertyValue from, PropertyValue to) {
  for (map<size_t, PropertyValue>::iterator i=_propertyValueTable.begin(); i!=_propertyValueTable.end(); i++) {
    if(i->second==from) {
      _propertyValueTable[i->first]=to;
    }
  }
}

int PropertyValueTable::entriesWithValue(PropertyValue v) {
  int result = 0;
  for (map<size_t, PropertyValue>::iterator i=_propertyValueTable.begin(); i!=_propertyValueTable.end(); i++) {
    if(i->second==v) {
      result++;
    }
  }
  return result; 
}

void PropertyValueTable::writeFile(const char* filename, bool onlyyesno, int offset) {
  writeFile(filename, onlyyesno, offset, false);
}

void PropertyValueTable::writeFile(const char* filename, bool onlyyesno, int offset, bool withCounterexamples) {
  ofstream myfile;
  myfile.open(filename);
  for (map<size_t, PropertyValue>::iterator k=_propertyValueTable.begin(); k!=_propertyValueTable.end(); k++) {
    size_t i = k->first;
    if(onlyyesno && (_propertyValueTable[i]!=PROPERTY_VALUE_YES && _propertyValueTable[i]!=PROPERTY_VALUE_NO))
      continue;
    myfile<<i+offset<<","<<reachToString(_propertyValueTable[i]);
    if (withCounterexamples && 
       	    (_propertyValueTable[i]== PROPERTY_VALUE_NO || _propertyValueTable[i]== PROPERTY_VALUE_YES) ) {
      myfile<<","<<_counterexamples[i];
    }
    myfile<<endl;
  }
  myfile.close();
}

void PropertyValueTable::write2013File(const char* filename, bool onlyyesno) {
  ofstream myfile;
  myfile.open(filename);
  for(int i=0;i<60;++i) {
    if(onlyyesno && (_propertyValueTable[i]!=PROPERTY_VALUE_YES && _propertyValueTable[i]!=PROPERTY_VALUE_NO))
      continue;
    myfile<<i+100<<","<<reachToString(_propertyValueTable[i])<<endl;
  }
  myfile.close();
}
void PropertyValueTable::write2012File(const char* filename, bool onlyyesno) {
  ofstream myfile;
  myfile.open(filename);
  // 2012: difference by 1 to 2013
  for(int i=0;i<61;++i) {
    if(onlyyesno && (_propertyValueTable[i]!=PROPERTY_VALUE_YES && _propertyValueTable[i]!=PROPERTY_VALUE_NO))
      continue;
    myfile<<i<<","<<reachToString(_propertyValueTable[i])<<endl;
  }
  myfile.close();
}

void PropertyValueTable::printLtlResults() {
  printResults("YES (verified)", "NO (falsified)", "ltl_property_"); 
}

void PropertyValueTable::printResults() {
  printResults("YES (REACHABLE)", "NO (UNREACHABLE)", "error_"); 
}

void PropertyValueTable::printResults(string yesAnswer, string noAnswer, string propertyName, bool withCounterexample) {
  cout<<"Analysis Property Results:"<<endl;
  for (map<size_t, PropertyValue>::iterator k=_propertyValueTable.begin(); k!=_propertyValueTable.end(); k++) {
    size_t i = k->first;
    cout<<color("white")<<propertyName<<i<<": ";
    switch(_propertyValueTable[i]) {
    case PROPERTY_VALUE_UNKNOWN: cout <<color("magenta")<<"UNKNOWN"; break; 
    case PROPERTY_VALUE_YES:
      {
      cout <<color("green")<< yesAnswer;
      if(withCounterexample) {cout << "   " << _counterexamples[i];}
      }
      break;
    case PROPERTY_VALUE_NO:
      {
      cout  <<color("cyan")<<noAnswer;
      if(withCounterexample) {cout << "   " << _counterexamples[i];}
      }
      break;
    default:cerr<<"Error: unknown property type."<<endl;assert(0);
    }
    cout<<color("normal")<<endl;
  }
  cout<<color("default-text-color");
}

void PropertyValueTable::printResultsStatistics() {
  int numReach=0, numNonReach=0;
  int numUnknown=0;
  for (map<size_t, PropertyValue>::iterator k=_propertyValueTable.begin(); k!=_propertyValueTable.end(); k++) {
    size_t i = k->first;
    switch(_propertyValueTable[i]) {
    case PROPERTY_VALUE_UNKNOWN: numUnknown++;break;
    case PROPERTY_VALUE_YES: numReach++;break;
    case PROPERTY_VALUE_NO: numNonReach++;break;
    default:cerr<<"Error: unknown property value type."<<endl;assert(0);
    }
  }
  cout<<"Property value statistics: "
      <<color("white")<<"YES: "<<color("green")<<numReach
      <<color("white")<<" NO: "<<color("cyan")<<numNonReach
      <<color("white")<<" UNKNOWN: "<<color("magenta")<<numUnknown
      <<color("default-text-color")<<" Total: "<<_propertyValueTable.size()
      <<endl;
}

#ifdef HAVE_SPOT
string PropertyValueTable::getLtlsAsPromelaCode(bool withResults, bool withAnnotations) {
  stringstream propertiesSpinSyntax;
  for (map<size_t, string>::iterator i=_formulas.begin(); i!=_formulas.end(); i++) {
    propertiesSpinSyntax << "ltl p"<<i->first<<"\t { "<<SpotMiscellaneous::spinSyntax(i->second)<<" }";
    if (withResults) {
      PropertyValue val = _propertyValueTable[i->first];
      if (val == PROPERTY_VALUE_YES) {
	propertiesSpinSyntax << "\t /* true */";
      } else if (val == PROPERTY_VALUE_NO) {
	propertiesSpinSyntax << "\t /* false */";
      } else if (val == PROPERTY_VALUE_UNKNOWN) {
	propertiesSpinSyntax << "\t /* unknown */";
      } else {
	cerr << "ERROR: Unknown PropertyValue detected." << endl;
	ROSE_ASSERT(0);
      }
    }
    if (withAnnotations) {
      propertiesSpinSyntax << "\t /* annotation: "<<_annotations[i->first]<<" */"; 
    }
    propertiesSpinSyntax << endl;
  }
  return propertiesSpinSyntax.str();
}
#endif

string PropertyValueTable::getLtlsRersFormat(bool withResults, bool withAnnotations) {
  stringstream propertiesRersFormat;
  for (map<size_t, string>::iterator i=_formulas.begin(); i!=_formulas.end(); i++) {
    propertiesRersFormat << "#"<<i->first<<": ";
    if (withResults) {
      PropertyValue val = _propertyValueTable[i->first];
      if (val == PROPERTY_VALUE_YES) {
	propertiesRersFormat << "\t true";
      } else if (val == PROPERTY_VALUE_NO) {
	propertiesRersFormat << "\t false";
      } else if (val == PROPERTY_VALUE_UNKNOWN) {
	propertiesRersFormat << "\t unknown";
      } else {
	cerr << "ERROR: Unknown PropertyValue detected." << endl;
	ROSE_ASSERT(0);
      }
    }
    if (withAnnotations) {
      propertiesRersFormat << "\t ("<<_annotations[i->first]<<")";
    }
    propertiesRersFormat << endl;
    propertiesRersFormat << "( "<<i->second<<" )" << endl;
    propertiesRersFormat << endl;
  }
  return propertiesRersFormat.str();
}

void PropertyValueTable::shuffle() {
  // create temporary maps that the randomly selected properties will be copied to
  // (this is not a copy-in-place function)
  map<string, size_t> idByFormula;
  map<size_t, PropertyValue> propertyValueTable;
  map<size_t, string> formulas;
  map<size_t, string> counterexamples;
  map<size_t, string> annotations;
  // store information on what has been copied already (used to randomly select the next property)
  int numCopied = 0;
  map<size_t, bool> copied;
  for (map<size_t, string>::iterator i=_formulas.begin(); i!=_formulas.end(); ++i) {
    copied[(*i).first] = false;
  }
  for (unsigned int i = 1; i <= size(); ++i) {
    // randomly select a property that has not been copied yet
    int indexNotCopied = CodeThorn::randomIntInRange( pair<int,int>(0, ((size() -1) - numCopied)) );
    map<size_t, string>::iterator iter = _formulas.begin();
    int indexNotCopiedIter = 0;
    while (indexNotCopiedIter < indexNotCopied || copied[(*iter).first]) {
      if (!copied[(*iter).first]) {
	++indexNotCopiedIter;
      }
      ++iter;
      ROSE_ASSERT(iter != _formulas.end());
    }
    size_t index = (*iter).first;
    // cout << "DEBUG: new index: " << i << "   old index: " << index << endl;
    // copy property information
    propertyValueTable[i] = _propertyValueTable[index];
    formulas[i] = _formulas[index];
    idByFormula[formulas[i]] = i;
    counterexamples[i] = _counterexamples[index];
    annotations[i] = _annotations[index];    
    // update the information on already copied properties
    ++numCopied;
    copied[index] = true;
  }
  // replace existing maps that represent the entire PropertyValueTable
  _idByFormula = idByFormula;
  _propertyValueTable = propertyValueTable;
  _formulas = formulas;
  _counterexamples = counterexamples;
  _annotations = annotations;
}
