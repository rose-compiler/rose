#include "PropertyValueTable.h"

// basic file operations
#include <iostream>
#include <fstream>
using namespace std;
#include <cassert>

using CodeThorn::color;

PropertyValueTable::PropertyValueTable() {
}

PropertyValueTable::PropertyValueTable(size_t size) {
  init(size);
}

void PropertyValueTable::init(size_t size) {
  if(_propertyValueTable.size()==size) {
    // reinit
    for(size_t i=0;i<size;i++) {
      setPropertyValue(i,PROPERTY_VALUE_UNKNOWN);
      _counterexamples[i] = "";
    }
  } else {
    // init
    for(size_t i=0;i<size;i++) {
      _propertyValueTable.push_back(PROPERTY_VALUE_UNKNOWN); 
      _counterexamples.insert(pair<int,string>(i, ""));
    }
  }
  assert(_propertyValueTable.size()==size);
  assert(_counterexamples.size()==size);
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
      throw "Error: property value table: reset of YES.";
    break;
  case PROPERTY_VALUE_NO:
    if(value!=PROPERTY_VALUE_NO)
      throw "Error: property value table: reset of NO.";
    break;
  default:assert(0);
  }
}

void PropertyValueTable::strictUpdatePropertyValue(size_t num, PropertyValue value) {
  assert(num>=0 && num<_propertyValueTable.size());
  if(getPropertyValue(num)==PROPERTY_VALUE_UNKNOWN) {
    setPropertyValue(num,value);
  } else {
    throw "Error: property value table: reset of value.";
  }
}

void PropertyValueTable::strictUpdateCounterexample(size_t num, string ce) {
  assert(num>=0 && num<_propertyValueTable.size());
  if(_counterexamples[num]=="") {
    _counterexamples[num] = ce;
  } else {
    throw "Error: property value table: counterexample already exists.";
  }
}

void PropertyValueTable::setPropertyValue(size_t num, PropertyValue value) {
  assert(num>=0 && num<_propertyValueTable.size());
  _propertyValueTable[num]=value;
}

void PropertyValueTable::setCounterexample(size_t num, string ce) {
  assert(num>=0 && num<_propertyValueTable.size());
  _counterexamples[num] = ce;
}

PropertyValue PropertyValueTable::getPropertyValue(size_t num) {
  assert(num>=0 && num<_propertyValueTable.size());
  return _propertyValueTable[num];
}

std::string PropertyValueTable::getCounterexample(size_t num) {
  assert(num>=0 && num<_propertyValueTable.size());
  return _counterexamples[num];
}

std::list<int>* PropertyValueTable::getPropertyNumbers(PropertyValue value) {
  std::list<int>* result = new list<int>();
  for (size_t i=0 ; i<_propertyValueTable.size() ; ++i) {
    switch(value) {
    case PROPERTY_VALUE_UNKNOWN: if (getPropertyValue(i) == PROPERTY_VALUE_UNKNOWN) { result->push_back(i); }; break;
    case PROPERTY_VALUE_YES: if (getPropertyValue(i) == PROPERTY_VALUE_YES) { result->push_back(i); }; break;
    case PROPERTY_VALUE_NO: if (getPropertyValue(i) == PROPERTY_VALUE_NO) { result->push_back(i); }; break;
    default:cerr<<"Error: unknown property value type."<<endl;assert(0);
    }
  }
  return result;
}

string PropertyValueTable::reachToString(PropertyValue num) {
  switch(num) {
  case PROPERTY_VALUE_UNKNOWN: return "unknown"; //,0";
  case PROPERTY_VALUE_YES: return "yes"; //,9";
  case PROPERTY_VALUE_NO: return "no"; //,9";
  default: {
    cerr<<"Error: unkown reachability information.";
    assert(0);
  }
  }
  assert(0);
}
// we were able to compute the entire state space. All unknown become non-reachable (=no)
void PropertyValueTable::finished() {
  for(size_t i=0;i<_propertyValueTable.size();++i) {
    if(_propertyValueTable[i]==PROPERTY_VALUE_UNKNOWN) {
      _propertyValueTable[i]=PROPERTY_VALUE_NO;
    }
  }
}
void PropertyValueTable::finishedReachability(bool isPrecise, bool isComplete) {
  if(isPrecise&&isComplete) {
    convertValue(PROPERTY_VALUE_UNKNOWN, PROPERTY_VALUE_NO);
    return;
  }
  if(isPrecise&&!isComplete) {
    // yes remains yes and unkown remains unknown (no cannot exist)
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
  for(size_t i=0;i<_propertyValueTable.size();++i) {
    if(_propertyValueTable[i]==from) {
      _propertyValueTable[i]=to;
    }
  }
}

int PropertyValueTable::entriesWithValue(PropertyValue v) {
  int result = 0;
  for(size_t i=0;i<_propertyValueTable.size();++i) {
    if(_propertyValueTable[i]==v) {
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
  for(size_t i=0;i<_propertyValueTable.size();++i) {
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
  //int maxCode=_propertyValueTable.size()-1;
  int maxCode=_propertyValueTable.size()-1;
  for(int i=0;i<=maxCode;++i) {
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
  int maxCode=_propertyValueTable.size()-1;
  int numReach=0, numNonReach=0;
  int numUnknown=0;
  for(int i=0;i<=maxCode;++i) {
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
      <<color("default-text-color")<<" Total: "<<maxCode+1
      <<endl;
}
