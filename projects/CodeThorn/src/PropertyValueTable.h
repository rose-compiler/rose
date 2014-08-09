#ifndef PROPERTYVALUETABLE_H
#define PROPERTYVALUETABLE_H

#include <vector>
#include <string>

#include "Miscellaneous.h"

using namespace std;

enum PropertyValue { PROPERTY_VALUE_UNKNOWN,PROPERTY_VALUE_YES,PROPERTY_VALUE_NO };

class PropertyValueTable {
 public:
  PropertyValueTable();
  PropertyValueTable(size_t size);
  void reachable(size_t num);
  void nonReachable(size_t num);
  void setPropertyValue(size_t num, PropertyValue value);
  void updatePropertyValue(size_t num, PropertyValue value);
  void strictUpdatePropertyValue(size_t num, PropertyValue value);
  //allocates and returns a new list of property numbers which are currently still unknown. The returned list has
  //to be deleted by the calling function.
  std::list<int>* getPropertyNumbers(PropertyValue value);
  PropertyValue getPropertyValue(size_t num);
  void finished();
  void finishedReachability(bool isPrecise, bool isComplete);
  void convertValue(PropertyValue from, PropertyValue to);
  void writeFile(const char* filename, bool onlyyesno=false, int offset=0);
  void write2013File(const char* filename, bool onlyyesno=false);
  void write2012File(const char* filename, bool onlyyesno=false);
  void printResults();
  void printLtlResults();
  void printResultsStatistics();
  void init();
  void init(size_t size);
  size_t size() { return _propertyValueTable.size(); }
 private:
  string reachToString(PropertyValue num);
  vector<PropertyValue> _propertyValueTable;
};

#endif
