#ifndef VARIABLE_ID_MAPPING_EXTENDED_H
#define VARIABLE_ID_MAPPING_EXTENDED_H

#include "VariableIdMapping.h"
#include "SgTypeSizeMapping.h"
#include <memory>

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
  private:
    std::unique_ptr<SgTypeSizeMapping> typeSizeMapping=std::make_unique<SgTypeSizeMapping>();
  };
}

#endif

