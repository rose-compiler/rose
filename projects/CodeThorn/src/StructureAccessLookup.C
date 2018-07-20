#include "sage3basic.h"
#include "StructureAccessLookup.h"
#include "RoseAst.h"

using namespace std;
using namespace SPRAY;

#include "SgTypeSizeMapping.h"

std::list<SgVariableDeclaration*> StructureAccessLookup::getDataMembers(SgClassDefinition* classDef) {
  std::list<SgVariableDeclaration*> varDeclList;
  typedef std::vector< std::pair< SgNode*, std::string > > DataMemberPointers;
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

void StructureAccessLookup::initialize(VariableIdMapping* variableIdMapping, SgProject* root) {
  ROSE_ASSERT(variableIdMapping);
  ROSE_ASSERT(root);
  RoseAst ast(root);
  for (auto node : ast) {
    if(SgClassDefinition* classDef=isSgClassDefinition(node)) {
      //cout<<"DEBUG: Class Definition: "<<classDef->unparseToString()<<endl;
      std::list<SgVariableDeclaration*> dataMembers=getDataMembers(classDef);
      int offset=0;
      for(auto dataMember : dataMembers) {
        if(isSgVariableDeclaration(dataMember)) {
          //cout<<"DEBUG: varDecl: "<<varDecl->unparseToString()<<" : ";
          VariableId varId=variableIdMapping->variableId(dataMember);
          if(varId.isValid()) {
            SgType* varType=variableIdMapping->getType(varId);
            if(varType) {
              // TODO: recursive for struct/class/union members
              //if(isStruct(type) ...) initialize(variableIdMapping, dataMember);
              
              int typeSize=typeSizeMapping.determineTypeSize(varType);
              
              // different varids can be mapped to the same offset
              
              // every varid is inserted exactly once.
              ROSE_ASSERT(varIdTypeSizeMap.find(varId)==varIdTypeSizeMap.end());
              //cout<<"Offset: "<<offset<<endl;
              
              varIdTypeSizeMap[varId]=offset;
              offset+=typeSize;
            } else {
              // could not determine var type
              // ...
            }
          } else {
            // non valid var id
            // throw ...
          }
        }
      }
    }
  }
}

int StructureAccessLookup::getOffset(SPRAY::VariableId varId) {
  ROSE_ASSERT(varId.isValid());
  auto varIdOffsetPairIter=varIdTypeSizeMap.find(varId);
  if(varIdOffsetPairIter!=varIdTypeSizeMap.end()) {
    return (*varIdOffsetPairIter).second;
  } else {
    return 0;
  }
}
