#ifndef ARRAY_ELEMENT_ACCESS_DATA
#define ARRAY_ELEMENT_ACCESS_DATA

#include "sage3basic.h"
#include "VariableIdMapping.h"

#include <set>

using namespace std;

using namespace SPRAY;

struct ArrayElementAccessData {
  VariableId varId;
  std::vector<int> subscripts;
  VariableId getVariable();
  int getSubscript(int numOfDimension);
  int getDimensions();
  ArrayElementAccessData();
  ArrayElementAccessData(SgPntrArrRefExp* ref, VariableIdMapping* variableIdMapping);
  std::string toString(VariableIdMapping* variableIdMapping);
  //! checks validity of data. The default value is not valid (does not correspond to any array) but can be used when creating STL containers.
  bool isValid();
};

bool operator==(const ArrayElementAccessData& a,const ArrayElementAccessData& other) {
    for(size_t i=0;i<a.subscripts.size();++i)
      if(a.subscripts[i]!=other.subscripts[i])
        return false;
    return a.varId==other.varId;
}
bool operator!=(const ArrayElementAccessData& a, const ArrayElementAccessData& other) {
  return !(a==other);
}
bool operator<(const ArrayElementAccessData& a, const ArrayElementAccessData& other) {
    if(a.varId!=other.varId)
      return a.varId<other.varId;
    if(a.subscripts.size()!=other.subscripts.size())
      return a.subscripts.size()<other.subscripts.size();
    vector<int>::const_iterator i=a.subscripts.begin();
    vector<int>::const_iterator j=other.subscripts.begin();
    while(i!=a.subscripts.end() && j!=other.subscripts.end()) {
      if(*i!=*j) {
        return *i<*j;
      } else {
        ++i;++j;
      }
    }
    ROSE_ASSERT(i==a.subscripts.end() && j==other.subscripts.end());
    return false; // both are equal
  }

typedef std::set<ArrayElementAccessData> ArrayElementAccessDataSet;

#endif

