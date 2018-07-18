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
  void initialize(SgNode* root);
  std::list<SgVariableDeclaration*> getDataMembersX(SgFunctionDefinition* funDef);
 private:
  SPRAY::SgTypeSizeMapping typeSizeMapping;
  std::map<SPRAY::VariableId,int> varIdTypeSizeMap;
};


#endif
