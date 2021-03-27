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
    void computeVariableSymbolMapping1(SgProject* project, int maxWarningsCount);
    void computeVariableSymbolMapping2(SgProject* project, int maxWarningsCount);
    void computeVariableSymbolMapping3(SgProject* project, int maxWarningsCount);
    void registerAllVariableSymbols(SgProject* project, int maxWarningsCount);

    // deprecated
    void computeTypeSizes();

    // direct lookup
    CodeThorn::TypeSize getTypeSize(SgType* type);
    CodeThorn::TypeSize getTypeSize(VariableId varId);
    void setTypeSize(SgType* type, CodeThorn::TypeSize newTypeSize);
    void setTypeSize(VariableId varId,  CodeThorn::TypeSize newTypeSize);
    CodeThorn::TypeSize getBuiltInTypeSize(enum CodeThorn::BuiltInType);

    std::string typeSizeMappingToString();
    size_t getNumVarIds();
    virtual void toStream(std::ostream& os) override;
    void typeSizeOverviewtoStream(std::ostream& os);
    CodeThorn::TypeSize numClassMembers(SgType*); // from classMembers map

  private:
    CodeThorn::TypeSize registerClassMembers(SgClassType* classType, CodeThorn::TypeSize offset);
    CodeThorn::TypeSize registerClassMembers(SgClassType* classType, std::list<SgVariableDeclaration*>& memberList, CodeThorn::TypeSize offset);
    void classMemberOffsetsToStream(std::ostream& os, SgType* type, std::int32_t level);
    SgType* strippedType(SgType* type);

    CodeThorn::TypeSizeMapping typeSizeMapping;
    std::unordered_map<SgType*,CodeThorn::TypeSize> _typeSize;

    // maintain class members for each type (required for operations such as copy struct)
    std::vector<VariableId> getRegisteredClassMemberVars(SgType*);
    bool isRegisteredClassMemberVar(SgType*,VariableId);
    void registerClassMemberVar(SgType*,VariableId);
    std::map<SgType*,std::vector<VariableId> > classMembers;

    void recordWarning(std::string);
    std::list<std::string> _warnings;

    // list of all global variable declarations
    // list of local variable declarations
    // declarations with initializer and complete or incomplete type: isDeclWithInitializer(getInitializer),isDeclWithoutInitializer,isDeclWithCompleteType
    // list of all types (from all declarations)
    // list of all declarations varid:(global|local|member,+class_member_list(varid),array-dim-vector<expr|num>+elementtype,varid->type,class|union|array|built-in
    // noinit|complete-type-init|incomplete-type-init)
    // determineTypeSize(type)
    // determineInitializerSize(initializer-expr)

    std::list<SgVariableDeclaration*> _globalVarDecls;
    std::list<SgFunctionDefinition*> _functionDefinitions;
    std::list<SgFunctionDeclaration*> _functionDeclarations;
  };
}

#endif

