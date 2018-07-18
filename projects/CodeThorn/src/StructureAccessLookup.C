#include "sage3basic.h"
#include "StructureAccessLookup.h"
#include "RoseAst.h"

using namespace std;
using namespace SPRAY;

#include "SgTypeSizeMapping.h"

std::list<SgVariableDeclaration*> StructureAccessLookup::getDataMembersX(SgClassDefinition* classDef) {
  std::list<SgVariableDeclaration*> varDeclList;
  DataMemberPointers dataMemPtrs=classDef->returnDataMemberPointers();
  // returnDataMemberPointers includes all declarations (methods need to be filtered)
  for(DataMemberPointers::iterator i=dataMemPtrs.begin();i!=dataMemPtrs.end();++i) {
    SgNode* node=(*i).first;
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
      varDeclList.push_back(varDecl);
    }
  }
  return varDeclList;
}

void StructureAccessLookup::initialize(SgNode* root) {
  RoseAst ast(root);
  for (auto node : ast) {
    if(SgClassDefinition* funDef=isSgClassDefinition(node)) {
      std::list<SgVariableDeclaration*> dataMembers=getDataMembersX(funDef);
      int offset=0;
      for(auto dataMember : dataMembers) {
        SgType* type=dataMember->get_type();
        int typeSize=typeSizeMapping=determineTypeSize(type);
        VariableId varId; // TODO
        // TODO: recursive for struct/class/union members
        // different varids can be mapped to the same offset
        varIdTypeSizeMap.insert(varId,offset);
        offset+=typeSize;
      }
    }
  }
}
