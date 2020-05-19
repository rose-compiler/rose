#ifndef VARIABLE_ID_MAPPING_EXTENDED_H
#define VARIABLE_ID_MAPPING_EXTENDED_H

#include "VariableIdMapping.h"
#include "TypeSizeMapping.h"

namespace CodeThorn {
  class VariableIdMappingExtended : public VariableIdMapping {
  public:
    void computeTypeSizes();
    // direct lookup
    unsigned int getTypeSize(CodeThorn::BuiltInType);
    unsigned int getTypeSize(SgType* type);
    unsigned int getTypeSize(VariableId varId);
    unsigned int getOffset(VariableId varId);
    std::string typeSizeMappingToString();
    bool isStructMember(VariableId varId);
  private:
    TypeSizeMapping typeSizeMapping;
  };
}

#endif

