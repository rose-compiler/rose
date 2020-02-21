#include "sage3basic.h"
#include "VariableIdMappingExtended.h"

namespace CodeThorn {
  unsigned int VariableIdMappingExtended::getTypeSize(CodeThorn::BuiltInType biType) {
    return typeSizeMapping->getTypeSize(biType);
  }
  unsigned int VariableIdMappingExtended::getTypeSize(SgType* type) {
    return typeSizeMapping->determineTypeSize(type);
  }
  unsigned int VariableIdMappingExtended::getTypeSize(VariableId varId) {
    return getTypeSize(getType(varId));
  }
  unsigned int VariableIdMappingExtended::getOffset(VariableId varId) {
    return 0;
  }

  std::string VariableIdMappingExtended::typeSizeMappingToString() {
    return typeSizeMapping->toString();
  }

  void VariableIdMappingExtended::computeTypeSizes() {
    // compute size for all variables
    VariableIdSet varIdSet=getVariableIdSet();
    for(auto vid : varIdSet) {
      SgType* varType=getType(vid);
      if(varType) {
        typeSizeMapping->determineTypeSize(varType);
      }
    }
  }
}
