#ifndef VARIABLE_ID_MAPPING_EXTENDED_H
#define VARIABLE_ID_MAPPING_EXTENDED_H

#include "VariableIdMapping.h"
#include "TypeSizeMapping.h"

namespace CodeThorn {
  class VariableIdMappingExtended : public VariableIdMapping {
  public:
    void computeVariableSymbolMapping(SgProject* project) override;
    void computeTypeSizes();
    // direct lookup
    unsigned int getTypeSize(enum CodeThorn::BuiltInType);
    unsigned int getTypeSize(SgType* type);
    unsigned int getTypeSize(VariableId varId);
    std::string typeSizeMappingToString();
  private:
    CodeThorn::TypeSizeMapping typeSizeMapping;
  };
}

#endif

