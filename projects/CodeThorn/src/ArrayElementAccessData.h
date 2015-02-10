#ifndef ARRAY_ELEMENT_ACCESS_DATA
#define ARRAY_ELEMENT_ACCESS_DATA

#include "VariableIdMapping.h"

#include <set>

using namespace std;

using namespace SPRAY;

struct ArrayElementAccessData {
  VariableId varId;
  std::vector<int> subscripts;
  VariableId getVariable() const;
  int getSubscript(int numOfDimension) const;
  int getDimensions() const;
  ArrayElementAccessData();
  ArrayElementAccessData(SgPntrArrRefExp* ref, VariableIdMapping* variableIdMapping);
  std::string toString(VariableIdMapping* variableIdMapping) const;
  //! checks validity of data. The default value is not valid (does not correspond to any array) but can be used when creating STL containers.
  bool isValid() const;
};

bool operator==(const ArrayElementAccessData& a,const ArrayElementAccessData& other);
bool operator!=(const ArrayElementAccessData& a, const ArrayElementAccessData& other);
bool operator<(const ArrayElementAccessData& a, const ArrayElementAccessData& other);

typedef std::set<ArrayElementAccessData> ArrayElementAccessDataSet;

#endif

