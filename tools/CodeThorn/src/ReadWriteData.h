#ifndef READ_WRITE_DATA_H
#define READ_WRITE_DATA_H

#include "VariableIdMapping.h"
#include "ArrayElementAccessData.h"

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

struct ReadWriteData {
  ArrayElementAccessDataSet writeArrayAccessSet;
  VariableIdSet writeVarIdSet;
  ArrayElementAccessDataSet readArrayAccessSet;
  VariableIdSet readVarIdSet;
};

typedef vector<int> IndexVector;
typedef map<IndexVector,ReadWriteData> IndexToReadWriteDataMap;

#endif
