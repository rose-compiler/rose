#ifndef ARRAY_ELEMENT_ACCESS_DATA
#define ARRAY_ELEMENT_ACCESS_DATA

#include "sage3basic.h"
#include "VariableIdMapping.h"

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
  bool operator==(ArrayElementAccessData& other) {
    for(size_t i=0;i<subscripts.size();++i)
      if(subscripts[i]!=other.subscripts[i])
        return false;
    return varId==other.varId;
  }
};

#endif

