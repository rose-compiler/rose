#ifndef PROPERTYVALUETABLE_H
#define PROPERTYVALUETABLE_H

#include <string>
#include <map>

#include "sage3basic.h"
#include "Miscellaneous.h"
#include "rose_config.h"
#ifdef HAVE_SPOT
#include "ltlthorn-lib/SpotMiscellaneous.h"
#endif

using namespace std;

enum PropertyValue { PROPERTY_VALUE_UNKNOWN,PROPERTY_VALUE_YES,PROPERTY_VALUE_NO };

class PropertyValueTable {
 public:
  PropertyValueTable();
  PropertyValueTable(size_t size);
  void addProperty(string formula);
  void addProperty(string formula, PropertyValue value);
  void addProperty(string formula, PropertyValue value, string counterexample);
  void addProperty(string formula, size_t id);
  void addProperty(string formula, size_t id, PropertyValue value);
  void addProperty(string formula, size_t id, PropertyValue value, string counterexample);
  void append(PropertyValueTable& toBeAppended);
  void addResults(PropertyValueTable& other);
  void reachable(size_t num);
  void nonReachable(size_t num);
  void setPropertyValue(size_t num, PropertyValue value);
  void setFormula(size_t num, string formula);
  void setCounterexample(size_t num, string ce);
  void setAnnotation(size_t num, string annotation);
  void updatePropertyValue(size_t num, PropertyValue value);
  void strictUpdatePropertyValue(size_t num, PropertyValue value);
  void strictUpdateCounterexample(size_t num, string ce);
  //allocates and returns a new list of property numbers which are currently still unknown. The returned list has
  //to be deleted by the calling function.
  std::list<int>* getPropertyNumbers();
  std::list<int>* getPropertyNumbers(PropertyValue value);
  int getPropertyNumber(string formula);
  PropertyValue getPropertyValue(size_t num);
  std::string getFormula(size_t num);
  std::string getCounterexample(size_t num);
  std::string getAnnotation(size_t num);
  void finished();
  void finishedReachability(bool isPrecise, bool isComplete);
  void convertValue(PropertyValue from, PropertyValue to);
  void writeFile(const char* filename, bool onlyyesno=false, int offset = 0);
  void writeFile(const char* filename, bool onlyyesno, int offset, bool withCounterexamples);
  void write2013File(const char* filename, bool onlyyesno=false);
  void write2012File(const char* filename, bool onlyyesno=false);
  void printResults();
  void printLtlResults();
  void printResults(string yesAnswer, string noAnswer, string propertyName = "property", bool withCounterExample = false);
  void printResultsStatistics();
  string getLtlsRersFormat(bool withResults=false, bool withAnnotations=false);
#ifdef HAVE_SPOT
  // 3rd parameter is a callback to not have a dependency on SpotMisc.
  string getLtlsAsPromelaCode(bool withResults, bool withAnnotations, std::string (*spinSyntaxCallBackFP)(std::string));
#endif
  void init();
  void init(size_t size);
  int entriesWithValue(PropertyValue v);
  void shuffle();
  size_t size() { return _propertyValueTable.size(); }
 private:
  string reachToString(PropertyValue num);
  map<string, size_t> _idByFormula;
  map<size_t, PropertyValue> _propertyValueTable;
  map<size_t, string> _formulas;
  map<size_t, string> _counterexamples;
  map<size_t, string> _annotations;
  size_t _maximumId;
};

#endif
