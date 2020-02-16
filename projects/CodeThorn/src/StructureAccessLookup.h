#ifndef STRUCTURE_ACCESS_LOOKUP_H
#define STRUCTURE_ACCESS_LOOKUP_H

#include <list>
#include <map>
#include "VariableIdMappingExtended.h"
#include "SgTypeSizeMapping.h"

class SgNode;
class SgFunctionDefinition;
class SgVariableDeclaration;

class StructureAccessLookup {
 public:

  void initializeOffsets(CodeThorn::VariableIdMappingExtended* variableIdMapping, SgProject* root);
  std::list<SgVariableDeclaration*> getDataMembers(SgClassDefinition* classDef);
  int getOffset(CodeThorn::VariableId varId);
  // returns true if the variable is a member of a struct/class/union.
  bool isStructMember(CodeThorn::VariableId varId);
  size_t numOfStoredMembers();
 private:
  bool isUnionDeclaration(SgNode* node);
  std::map<CodeThorn::VariableId,int> varIdTypeSizeMap;
};


#endif
