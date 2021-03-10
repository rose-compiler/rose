#ifndef VARIABLE_ID_MAPPING_EXTENDED_H
#define VARIABLE_ID_MAPPING_EXTENDED_H

#include "VariableIdMapping.h"
#include "TypeSizeMapping.h"
#include <unordered_map>
#include <list>

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
    void computeVariableSymbolMapping2(SgProject* project, int maxWarningsCount = 3); // override;

    // deprecated
    void computeTypeSizes();

    // direct lookup
    CodeThorn::TypeSize getTypeSize(SgType* type);
    CodeThorn::TypeSize getTypeSize(VariableId varId);
    void setTypeSize(SgType* type, CodeThorn::TypeSize newTypeSize);
    void setTypeSize(VariableId varId,  CodeThorn::TypeSize newTypeSize);

    std::string typeSizeMappingToString();
    size_t getNumVarIds();
    virtual void toStream(std::ostream& os) override;
    CodeThorn::TypeSize getTypeSize(enum CodeThorn::BuiltInType);
    CodeThorn::TypeSize numClassMembers(SgType*); // from classMembers map
  private:
    CodeThorn::TypeSize registerClassMembers(SgClassType* classType, CodeThorn::TypeSize offset);
    CodeThorn::TypeSize registerClassMembers(SgClassType* classType, std::list<SgVariableDeclaration*>& memberList, CodeThorn::TypeSize offset);
    SgType* strippedType(SgType* type);
    CodeThorn::TypeSizeMapping typeSizeMapping;
    std::unordered_map<SgType*,CodeThorn::TypeSize> _typeSize;
    void recordWarning(std::string);
    std::list<std::string> _warnings;

    // maintaining class members for each type (required for operations such as copy struct)
    std::vector<VariableId> getRegisteredClassMemberVars(SgType*);
    bool isRegisteredClassMemberVar(SgType*,VariableId);
    void registerClassMemberVar(SgType*,VariableId);
    std::map<SgType*,std::vector<VariableId> > classMembers;

  };
}

#endif

