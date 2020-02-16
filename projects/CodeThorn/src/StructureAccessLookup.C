#include "sage3basic.h"
#include "StructureAccessLookup.h"
#include "RoseAst.h"
#include "SgTypeSizeMapping.h"
#include "AbstractValue.h"
#include "VariableIdMapping.h"

using namespace std;
using namespace CodeThorn;
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

bool StructureAccessLookup::isUnionDeclaration(SgNode* node) {
  if(SgClassDefinition* classDef=isSgClassDefinition(node)) {
    node=classDef->get_declaration();
  }
  if(SgClassDeclaration* classDecl=isSgClassDeclaration(node)) {
    SgClassDeclaration::class_types classType=classDecl->get_class_type();
    return classType==SgClassDeclaration::e_union;
  }
  return false;
}

void StructureAccessLookup::initializeOffsets(VariableIdMappingExtended* variableIdMapping, SgProject* root) {
  ROSE_ASSERT(variableIdMapping);
  ROSE_ASSERT(root);
  RoseAst ast(root);
  int numUnknownVarType=0;
  int numNonValidVarId=0;
  int numZeroTypeSize=0;
  for (RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgNode* node=*i;
    ROSE_ASSERT(node);
    if(SgClassDefinition* classDef=isSgClassDefinition(node)) {
      //cout<<"DEBUG: class def: "<<classDef->unparseToString()<<endl;
      //cout<<"DEBUG: Class Definition: "<<classDef->unparseToString()<<endl;
      std::list<SgVariableDeclaration*> dataMembers=getDataMembers(classDef);
      int offset=0;
      for(auto dataMember : dataMembers) {
        //cout<<"DEBUG: at data member: "<<dataMember->unparseToString()<<endl;
        if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(dataMember)) {
          SgInitializedName* initName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
          if(VariableIdMapping::isAnonymousBitfield(initName)) {
            // ROSE AST BUG WORKAROUND (ROSE-1867): anonymous bitfields are assigned the same SgSymbol
            continue;
          }
          //cout<<"DEBUG: struct data member decl: "<<dataMember->unparseToString()<<" : ";
          VariableId varId=variableIdMapping->variableId(dataMember);
          if(varId.isValid()) {
            SgType* varType=variableIdMapping->getType(varId);
            if(varType) {

              // TODO: recursive type size initialization for nested struct/class/union members
              // currently nested types are ignored!
              //if(isStruct(type) ...) initialize(variableIdMapping, dataMember);

              int typeSize=variableIdMapping->getTypeSize(varType);
              if(typeSize==0) {
                numZeroTypeSize++;
                //cout<<"DEBUG: Type of size 0: "<<varType->unparseToString()<<endl;
              }
              
              // different varids can be mapped to the same offset
              
              // every varid is inserted exactly once.
              if(varIdTypeSizeMap.find(varId)!=varIdTypeSizeMap.end()) {
                // if the same varId is found, ensure that the offset
                // is the same again (e.g. the same headerfile is
                // included in 2 different files, both provided on the
                // command line
                if(varIdTypeSizeMap[varId]!=offset) {
                  continue; // ROSE AST WORKAROUND (for BUG ROSE-1879): ignore double entries in structs which are the result of a bug
                  // do nothing for now
                  //cerr<<"WARNING: Data structure offset mismatch at "<<SgNodeHelper::sourceFilenameLineColumnToString(dataMember)<<":"<<dataMember->unparseToString()<<":"<<varIdTypeSizeMap[varId]<<" vs "<<offset<<endl;

                  //variableIdMapping->toStream(cerr);
                  //cerr<<"Internal error: StructureAccessLookup::initializeOffsets: varid already exists."<<endl;
                  //cerr<<"existing var id: "<<varId.toUniqueString(variableIdMapping)<<endl;
                  //cerr<<"Symbol: "<<variableIdMapping->getSymbol(varId)<<endl;
                  //cerr<<"Type: "<<variableIdMapping->getType(varId)->unparseToString()<<endl;
                  //cerr<<"Declaration: "<<node->unparseToString()<<endl;
                  //exit(1);
                }                  
              } else {
                //cout<<" DEBUG Offset: "<<offset<<endl;
                varIdTypeSizeMap.emplace(varId,offset);
              }
              // for unions the offset is not increased (it is the same for all members)
              if(!isUnionDeclaration(node)) {
                offset+=typeSize;
              }
            } else {
              // could not determine var type
              // ...
              //cout<<"DEBUG: unknown var type."<<endl;
              numUnknownVarType++;
            }
          } else {
            // non valid var id
            // throw ...
            cerr<<"Internal Error: StructureAccessLookup: invalid varid."<<endl;
            numNonValidVarId++;
            exit(1);
          }
        }
      }
      // skip subtree of class definition (would revisit nodes).
      i.skipChildrenOnForward();
    }
  }
#if 0
  cerr<<"DEBUG: Number of unknown var types: "<<numUnknownVarType<<endl;
  cerr<<"DEBUG: Number of non-valid varids: "<<numNonValidVarId<<endl;
  cerr<<"DEBUG: Number of types with 0 size: "<<numZeroTypeSize<<endl;
  cerr<<"DEBUG: typesize map size: "<<varIdTypeSizeMap.size()<<endl;
#endif
}

int StructureAccessLookup::getOffset(CodeThorn::VariableId varId) {
  ROSE_ASSERT(varId.isValid());
  auto varIdOffsetPairIter=varIdTypeSizeMap.find(varId);
  if(varIdOffsetPairIter!=varIdTypeSizeMap.end()) {
    return (*varIdOffsetPairIter).second;
  } else {
    return 0;
  }
}

bool StructureAccessLookup::isStructMember(CodeThorn::VariableId varId) {
  return varIdTypeSizeMap.find(varId)!=varIdTypeSizeMap.end();
}

size_t StructureAccessLookup::numOfStoredMembers() {
  return varIdTypeSizeMap.size();
}
