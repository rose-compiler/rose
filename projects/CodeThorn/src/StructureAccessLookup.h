#ifndef STRUCTURE_ACCESS_LOOKUP_H
#define STRUCTURE_ACCESS_LOOKUP_H

#include <list>
#include <map>
#include "VariableIdMapping.h"
#include "SgTypeSizeMapping.h"

class SgNode;
class SgFunctionDefinition;
class SgVariableDeclaration;

class StructureAccessLookup {
 public:
  void initializeOffsets(SPRAY::VariableIdMapping* variableIdMapping, SgProject* root);
  std::list<SgVariableDeclaration*> getDataMembers(SgClassDefinition* classDef);
  int getOffset(SPRAY::VariableId varId);
 private:
  std::map<SPRAY::VariableId,int> varIdTypeSizeMap;
};


#endif
