#ifndef READ_WRITE_DATA_H
#define READ_WRITE_DATA_H

#include <vector>
#include <map>
#include "VariableIdMapping.h"
#include "ArrayElementAccessData.h"

struct ReadWriteData {
  ArrayElementAccessDataSet writeArrayAccessSet;
  CodeThorn::VariableIdSet writeVarIdSet;
  ArrayElementAccessDataSet readArrayAccessSet;
  CodeThorn::VariableIdSet readVarIdSet;
};

typedef std::vector<int> IndexVector;
typedef std::map<IndexVector,ReadWriteData> IndexToReadWriteDataMap;

#endif
