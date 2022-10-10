#ifndef ARRAY_ELEMENT_ACCESS_DATA
#define ARRAY_ELEMENT_ACCESS_DATA

#include <set>
#include "VariableIdMapping.h"

struct ArrayElementAccessData {
  CodeThorn::VariableId varId;
  std::vector<int> subscripts;
  CodeThorn::VariableId getVariable() const;
  int getSubscript(int numOfDimension) const;
  int getDimensions() const;
  ArrayElementAccessData();
  ArrayElementAccessData(SgPntrArrRefExp* ref, CodeThorn::VariableIdMapping* variableIdMapping);
  std::string toString(CodeThorn::VariableIdMapping* variableIdMapping) const;
  std::string toStringNoSsaSubscript(CodeThorn::VariableIdMapping* variableIdMapping) const;
  //! checks validity of data. The default value is not valid (does not correspond to any array) but can be used when creating STL containers.
  bool isValid() const;
  bool hasNegativeIndex() const;
};

bool operator==(const ArrayElementAccessData& a,const ArrayElementAccessData& other);
bool operator!=(const ArrayElementAccessData& a, const ArrayElementAccessData& other);
bool operator<(const ArrayElementAccessData& a, const ArrayElementAccessData& other);

typedef std::set<ArrayElementAccessData> ArrayElementAccessDataSet;

#endif

