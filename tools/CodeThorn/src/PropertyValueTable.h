#ifndef PROPERTYVALUETABLE_H
#define PROPERTYVALUETABLE_H

#include <string>
#include <map>

#include "sage3basic.h"
#include "Miscellaneous.h"

enum PropertyValue { PROPERTY_VALUE_UNKNOWN,PROPERTY_VALUE_YES,PROPERTY_VALUE_NO };

class PropertyValueTable {
 public:
  PropertyValueTable();
  PropertyValueTable(size_t size);
  void addProperty(std::string formula);
  void addProperty(std::string formula, PropertyValue value);
  void addProperty(std::string formula, PropertyValue value, std::string counterexample);
  void addProperty(std::string formula, size_t id);
  void addProperty(std::string formula, size_t id, PropertyValue value);
  void addProperty(std::string formula, size_t id, PropertyValue value, std::string counterexample);
  void append(PropertyValueTable& toBeAppended);
  void addResults(PropertyValueTable& other);
  void reachable(size_t num);
  void nonReachable(size_t num);
  void setPropertyValue(size_t num, PropertyValue value);
  void setFormula(size_t num, std::string formula);
  void setCounterexample(size_t num, std::string ce);
  void setAnnotation(size_t num, std::string annotation);
  void updatePropertyValue(size_t num, PropertyValue value);
  void strictUpdatePropertyValue(size_t num, PropertyValue value);
  void strictUpdateCounterexample(size_t num, std::string ce);
  //allocates and returns a new list of property numbers which are currently still unknown. The returned list has
  //to be deleted by the calling function.
  std::list<int>* getPropertyNumbers();
  std::list<int>* getPropertyNumbers(PropertyValue value);
  int getPropertyNumber(std::string formula);
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
  void printResults(std::string yesAnswer, std::string noAnswer, std::string propertyName = "property", bool withCounterExample = false);
  void printResultsStatistics();
  std::string getLtlsRersFormat(bool withResults=false, bool withAnnotations=false);
  std::string getLtlsAsPromelaCode(bool withResults, bool withAnnotations, std::string (*spinSyntaxCallBackFP)(std::string));
  void init();
  void init(size_t size);
  int entriesWithValue(PropertyValue v);
  void shuffle();
  size_t size() { return _propertyValueTable.size(); }
 private:
  std::string reachToString(PropertyValue num);
  std::map<std::string, size_t> _idByFormula;
  std::map<size_t, PropertyValue> _propertyValueTable;
  std::map<size_t, std::string> _formulas;
  std::map<size_t, std::string> _counterexamples;
  std::map<size_t, std::string> _annotations;
  size_t _maximumId;
};

#endif
