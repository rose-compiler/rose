#ifndef VARIABLE_ID_MAPPING_EXTENDED_H
#define VARIABLE_ID_MAPPING_EXTENDED_H

#include "VariableIdMapping.h"
#include "TypeSizeMapping.h"

namespace CodeThorn {
  class VariableIdMappingExtended : public VariableIdMapping {
  public:
    
    /**
     * create the mapping between symbols in the AST and associated
     * variable-ids. Each variable in the project is assigned one
     * variable-id (including global variables, local variables,
     * class/struct/union data members)
     * 
     * param[in] project: The Rose AST we're going to act on
     * param[in] maxWarningsCount: A limit for the number of warnings to print.  0 = no warnings -1 = all warnings
    */    
    void computeVariableSymbolMapping(SgProject* project, int maxWarningsCount = 3) override;
    void computeTypeSizes();
    // direct lookup
    unsigned int getTypeSize(enum CodeThorn::BuiltInType);
    unsigned int getTypeSize(SgType* type);
    unsigned int getTypeSize(VariableId varId);
    std::string typeSizeMappingToString();
    size_t getNumVarIds();
  private:
    CodeThorn::TypeSizeMapping typeSizeMapping;
  };
}

#endif

