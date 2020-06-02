#include "sage3basic.h"
#include "VariableIdMappingExtended.h"
#include "CodeThornLib.h"

using namespace Sawyer::Message;
using namespace std;

namespace CodeThorn {

  void VariableIdMappingExtended::computeVariableSymbolMapping(SgProject* project) {
    VariableIdMapping::computeVariableSymbolMapping(project);
    computeTypeSizes();
    typeSizeMapping.computeOffsets(project,this);
  }

  unsigned int VariableIdMappingExtended::getTypeSize(CodeThorn::BuiltInType biType) {
    CodeThorn::logger[TRACE]<<"getTypeSize(BuiltInType)"<<std::endl;
    return typeSizeMapping.getTypeSize(biType);
  }
  unsigned int VariableIdMappingExtended::getTypeSize(SgType* type) {
    CodeThorn::logger[TRACE]<<"getTypeSize(SgType*)"<<std::endl;
    return typeSizeMapping.determineTypeSize(type);
  }
  unsigned int VariableIdMappingExtended::getTypeSize(VariableId varId) {
    return getTypeSize(getType(varId));
  }
  std::string VariableIdMappingExtended::typeSizeMappingToString() {
    return typeSizeMapping.toString();
  }

  void VariableIdMappingExtended::computeTypeSizes() {
    // compute size for all variables
    VariableIdSet varIdSet=getVariableIdSet();
    for(auto vid : varIdSet) {
      SgType* varType=getType(vid);
      if(varType) {
        if(SgArrayType* arrayType=isSgArrayType(varType)) {
          setElementSize(vid,typeSizeMapping.determineElementTypeSize(arrayType));
          setNumberOfElements(vid,typeSizeMapping.determineNumberOfElements(arrayType));
        } else {
          setElementSize(vid,typeSizeMapping.determineTypeSize(varType));
          setNumberOfElements(vid,1);
        }
      }
    }
  }

}
