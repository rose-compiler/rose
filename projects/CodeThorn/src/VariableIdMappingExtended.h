#ifndef VARIABLE_ID_MAPPING_EXTENDED_H
#define VARIABLE_ID_MAPPING_EXTENDED_H

#include "VariableIdMapping.h"

namespace CodeThorn {
  class VariableIdMappingExtended : public VariableIdMapping {
  public:
    void computeTypeSizes();
  };
}

#endif

