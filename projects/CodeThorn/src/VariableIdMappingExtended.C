#include "sage3basic.h"
#include "VariableIdMappingExtended.h"
#include "CodeThornLib.h"

using namespace Sawyer::Message;

namespace CodeThorn {
  unsigned int VariableIdMappingExtended::getTypeSize(CodeThorn::BuiltInType biType) {
    CodeThorn::logger[TRACE]<<"getTypeSize(BuiltInType)"<<std::endl;
    ROSE_ASSERT(typeSizeMapping);
    return typeSizeMapping->getTypeSize(biType);
  }
  unsigned int VariableIdMappingExtended::getTypeSize(SgType* type) {
    CodeThorn::logger[TRACE]<<"getTypeSize(SgType*)"<<std::endl;
    ROSE_ASSERT(typeSizeMapping);
    return typeSizeMapping->determineTypeSize(type);
  }
  unsigned int VariableIdMappingExtended::getTypeSize(VariableId varId) {
    return getTypeSize(getType(varId));
  }
  unsigned int VariableIdMappingExtended::getOffset(VariableId varId) {
    return 0;
  }

  std::string VariableIdMappingExtended::typeSizeMappingToString() {
    ROSE_ASSERT(typeSizeMapping);
    return typeSizeMapping->toString();
  }

  void VariableIdMappingExtended::computeTypeSizes() {
    // compute size for all variables
    VariableIdSet varIdSet=getVariableIdSet();
    for(auto vid : varIdSet) {
      SgType* varType=getType(vid);
      if(varType) {
        ROSE_ASSERT(typeSizeMapping);
        typeSizeMapping->determineTypeSize(varType);
      }
    }
  }
}
