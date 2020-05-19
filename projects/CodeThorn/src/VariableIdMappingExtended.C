#include "sage3basic.h"
#include "VariableIdMappingExtended.h"
#include "CodeThornLib.h"

using namespace Sawyer::Message;

namespace CodeThorn {
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
  unsigned int VariableIdMappingExtended::getOffset(VariableId varId) {
    return 0;
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
        typeSizeMapping.determineTypeSize(varType);
      }
    }
  }

  bool VariableIdMappingExtended::isStructMember(VariableId varId) {
    return typeSizeMapping.isStructMember(varId);
  }
  
}
