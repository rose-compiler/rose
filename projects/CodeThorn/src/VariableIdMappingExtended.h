#ifndef VARIABLE_ID_MAPPING_EXTENDED_H
#define VARIABLE_ID_MAPPING_EXTENDED_H

#include "VariableIdMapping.h"
#include "TypeSizeMapping.h"
#include <unordered_map>
#include <unordered_set>
#include <list>

namespace CodeThorn {

  class VariableIdMappingExtended : public VariableIdMapping {
  public:

    enum IndexRemappingEnum { IDX_ORIGINAL, IDX_REMAPPED };

    class OffsetAbstractionMappingEntry {
    public:
      OffsetAbstractionMappingEntry();
      OffsetAbstractionMappingEntry(CodeThorn::TypeSize,IndexRemappingEnum);
      CodeThorn::TypeSize getRemappedOffset();
      IndexRemappingEnum getIndexRemappingType();
    private:
      TypeSize remappedOffset=-1;
      IndexRemappingEnum mappingType=IDX_ORIGINAL;
    };
  
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
    void computeVariableSymbolMapping2(SgProject* project, int maxWarningsCount);
    void computeMemOffsetRemap();
    
    SgVariableDeclaration* getVariableDeclaration(VariableId varId) override;
    
    // direct lookup
    CodeThorn::TypeSize getTypeSize(SgType* type);
    CodeThorn::TypeSize getTypeSize(VariableId varId);
    void setTypeSize(SgType* type, CodeThorn::TypeSize newTypeSize);
    void setTypeSize(VariableId varId,  CodeThorn::TypeSize newTypeSize);
    CodeThorn::TypeSize getBuiltInTypeSize(enum CodeThorn::BuiltInType);

    // true if consistency check passes
    bool astSymbolCheck(SgProject* project);
    std::string typeSizeMappingToString();
    size_t getNumVarIds();
    virtual void toStream(std::ostream& os) override;
    void typeSizeOverviewtoStream(std::ostream& os);
    CodeThorn::TypeSize numClassMembers(SgType*); // from classMembers map

    CodeThorn::TypeSize determineTypeSize(SgType*);
    CodeThorn::TypeSize determineElementTypeSize(SgArrayType* sgType);
    CodeThorn::TypeSize determineNumberOfArrayElements(SgArrayType* sgType);
    
    bool symbolExists(SgSymbol* sym);
    static SgVariableDeclaration* getVariableDeclarationFromSym(SgSymbol* sym);

    CodeThorn::VariableIdSet getSetOfVarIds(VariableScope vs);
    CodeThorn::VariableIdSet getSetOfGlobalVarIds();
    CodeThorn::VariableIdSet getSetOfLocalVarIds();
    CodeThorn::VariableIdSet getSetOfFunParamVarIds();

    std::list<SgVariableDeclaration*> getListOfGlobalVarDecls();
    std::list<SgVariableDeclaration*> getVariableDeclarationsOfVariableIdSet(VariableIdSet&);

    // temporary variable used for transferring return value of functions
    void registerReturnVariable();
    VariableId getReturnVariableId();
    bool isReturnVariableId(VariableId);
    
    void addVariableDeclaration(SgVariableDeclaration* decl);
    CodeThorn::TypeSize registerClassMembers(SgClassType* classType, CodeThorn::TypeSize offset, bool replaceClassDataMembers=false);
    CodeThorn::TypeSize registerClassMembers(SgClassType* classType, std::list<SgVariableDeclaration*>& memberList, CodeThorn::TypeSize offset, bool replaceClassDataMembers=false);
    void classMemberOffsetsToStream(std::ostream& os, SgType* type, std::int32_t level);

    // support for offset remapping abstraction
  public:
    void setArrayAbstractionIndex(int32_t remapIndex);
    int32_t getArrayAbstractionIndex();
    OffsetAbstractionMappingEntry getOffsetAbstractionMappingEntry(VariableId varId, CodeThorn::TypeSize regionOffset);

  private:
    void memOffsetRemap(VariableId memRegId, VariableId varId, int32_t remapIndex, CodeThorn::TypeSize regionOffset, CodeThorn::TypeSize remappedOffset, IndexRemappingEnum mappingType);
    void registerMapping(VariableId memRegId, CodeThorn::TypeSize regionOffset,CodeThorn::TypeSize remappedOffset, IndexRemappingEnum idx);
    int32_t _arrayAbstractionIndex=-1;
    VariableId _returnVarId;
    
  public:
    SgType* strippedType(SgType* type);
    // does not strip pointer types, to avoid infinite recursion in rekursive data types
    SgType* strippedType2(SgType* type);
    SgExprListExp* getAggregateInitExprListExp(SgVariableDeclaration* varDecl);
    static SgClassDeclaration* getClassDeclarationOfClassType(SgClassType* type);
    bool isUnion(SgClassType* type);
    static std::pair<bool,std::list<SgVariableDeclaration*> > memberVariableDeclarationsList(SgClassType* classType);
    bool isDataMemberAccess(SgVarRefExp* varRefExp);
    bool isGlobalOrLocalVariableAccess(SgVarRefExp* varRefExp);
    void setAstSymbolCheckFlag(bool flag);
    bool getAstSymbolCheckFlag();
    
  private:
    bool _astSymbolCheckFlag=false;
    
    class MemPoolTraversal : public ROSE_VisitTraversal {
    public:
      void visit(SgNode* node);
      void dumpClassTypes();
      void dumpArrayTypes();
      std::unordered_set<SgClassType*> classTypes; // class, struct, union
      std::unordered_set<SgArrayType*> arrayTypes; // any dimension
      std::unordered_set<SgType*> builtInTypes; // all other types
      std::unordered_set<SgClassDefinition*> classDefinitions; // all other pointers
    };
    
    void createTypeLists();
    void initTypeSizes();
    void computeTypeSizes();
    CodeThorn::TypeSize computeTypeSize(SgType* type);
    CodeThorn::TypeSize getTypeSizeNew(SgType* type);
    void dumpTypeLists();
    void dumpTypeSizes();
    void registerClassMembersNew();
    std::list<SgVarRefExp*> structAccessesInsideFunctions(SgProject* project);
    std::list<SgVarRefExp*> variableAccessesInsideFunctions(SgProject* project);
    std::int32_t checkVarRefExpAccessList(std::list<SgVarRefExp*>& l, std::string accessName);
      
    CodeThorn::TypeSizeMapping typeSizeMapping;
    std::unordered_map<SgType*,CodeThorn::TypeSize> _typeSize;

  public:
    // maintain class members for each type (required for operations such as copy struct)
    std::vector<VariableId> getRegisteredClassMemberVars(SgType*);
    bool isRegisteredClassMemberVar(SgType*,VariableId);
    void registerClassMemberVar(SgType*,VariableId);
    void removeDataMembersOfClass(SgClassType* type);
    std::vector<VariableId> getClassMembers(SgType*);
    std::vector<VariableId> getClassMembers(VariableId);
    std::map<SgType*,std::vector<VariableId> > classMembers;
    
    void setErrorReportFileName(std::string name);
    void setStatusFlag(bool flag);
    std::string unusedVariablesCsvReport();
    
  private:
    // will set the isUsed flag in VIM for all variables (invoked by computeSymbolMapping)
    void determineVarUsage(SgProject* project);

    typedef std::list<std::pair<SgStatement*,SgVarRefExp*>> BrokenExprStmtList;
    
    SgStatement* correspondingStmtOfExpression(SgExpression* exp);
    BrokenExprStmtList computeCorrespondingStmtsOfBrokenExpressions(std::list<SgVarRefExp*>& accesses);
    
    bool isMemberVariableDeclaration(SgVariableDeclaration*);
    // determines all size information obtainable from SgType and sets values in varidinfo
    // 3rd param can be a nullptr, in which case no decl is determined and no aggregate initializer is checked for size (this is the case for formal function parameters)
    void setVarIdInfoFromType(VariableId varId);
    std::string varIdInfoToString(VariableId varId);
  
    bool isRegisteredType(SgType* type);
    void registerType(SgType* type);
    
    MemPoolTraversal _memPoolTraversal;

    std::list<SgFunctionDefinition*> _functionDefinitions;
    std::list<SgFunctionDeclaration*> _functionDeclarations;
    std::unordered_set<SgType*> _registeredTypes;

    typedef std::unordered_map<TypeSize,OffsetAbstractionMappingEntry> TypeSizeOffsetAbstractionMapType;
    typedef std::unordered_map<CodeThorn::VariableId, TypeSizeOffsetAbstractionMapType, VariableIdHash> VariableIdTypeSizeMappingType;
    VariableIdTypeSizeMappingType _offsetAbstractionMapping;

    void generateErrorReport(bool astSymbolCheckResult);
    std::string errorReportFileName;
    void appendErrorReportLine(std::string s);
    std::list<std::string> errorReport;
    bool _status=false;

  };
}

#endif

