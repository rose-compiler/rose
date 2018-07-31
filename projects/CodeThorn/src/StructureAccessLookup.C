#include "sage3basic.h"
#include "StructureAccessLookup.h"
#include "RoseAst.h"
#include "SgTypeSizeMapping.h"
#include "AbstractValue.h"

using namespace std;
using namespace SPRAY;
using namespace CodeThorn;

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

void StructureAccessLookup::initializeOffsets(VariableIdMapping* variableIdMapping, SgProject* root) {
  ROSE_ASSERT(variableIdMapping);
  ROSE_ASSERT(root);
  RoseAst ast(root);
  int numUnknownVarType=0;
  int numNonValidVarId=0;
  int numZeroTypeSize=0;
#if 1
  for (RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgNode* node=*i;
    ROSE_ASSERT(node);
    if(SgClassDefinition* classDef=isSgClassDefinition(node)) {
      //cout<<"DEBUG: Class Definition: "<<classDef->unparseToString()<<endl;
#if 1                
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
              
              SgTypeSizeMapping* typeSizeMapping=AbstractValue::getTypeSizeMapping();
              ROSE_ASSERT(typeSizeMapping);
              int typeSize=typeSizeMapping->determineTypeSize(varType);
              if(typeSize==0) {
                numZeroTypeSize++;
                cout<<"DEBUG: Type of size 0: "<<varType->unparseToString()<<endl;
              }
              
              // different varids can be mapped to the same offset
              
              // every varid is inserted exactly once.
              ROSE_ASSERT(varIdTypeSizeMap.find(varId)==varIdTypeSizeMap.end());
              //cout<<"Offset: "<<offset<<endl;
              
              varIdTypeSizeMap.emplace(varId,offset);
              offset+=typeSize;
            } else {
              // could not determine var type
              // ...
              numUnknownVarType++;
            }
          } else {
            // non valid var id
            // throw ...
            numNonValidVarId++;
          }
        }
      }
#endif
    }
  }
#endif
  cerr<<"DEBUG: Number of unknown var types: "<<numUnknownVarType<<endl;
  cerr<<"DEBUG: Number of non-valid varids: "<<numNonValidVarId<<endl;
  cerr<<"DEBUG: Number of types with 0 size: "<<numZeroTypeSize<<endl;
  cerr<<"DEBUG: typesize map size: "<<varIdTypeSizeMap.size()<<endl;
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

bool StructureAccessLookup::isStructMember(SPRAY::VariableId varId) {
  return varIdTypeSizeMap.find(varId)!=varIdTypeSizeMap.end();
}

size_t StructureAccessLookup::numOfStoredMembers() {
  return varIdTypeSizeMap.size();
}
