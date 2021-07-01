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

    SgVariableDeclaration* getVariableDeclaration(VariableId varId) override;
    
    // direct lookup
    CodeThorn::TypeSize getTypeSize(SgType* type);
    CodeThorn::TypeSize getTypeSize(VariableId varId);
    void setTypeSize(SgType* type, CodeThorn::TypeSize newTypeSize);
    void setTypeSize(VariableId varId,  CodeThorn::TypeSize newTypeSize);
    CodeThorn::TypeSize getBuiltInTypeSize(enum CodeThorn::BuiltInType);

    // true if consistency check passed
    bool consistencyCheck(SgProject* project);
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

    void addVariableDeclaration(SgVariableDeclaration* decl);
    CodeThorn::TypeSize registerClassMembers(SgClassType* classType, CodeThorn::TypeSize offset, bool repairMode=false);
    CodeThorn::TypeSize registerClassMembers(SgClassType* classType, std::list<SgVariableDeclaration*>& memberList, CodeThorn::TypeSize offset, bool repairMode=false);
    void classMemberOffsetsToStream(std::ostream& os, SgType* type, std::int32_t level);
    SgType* strippedType(SgType* type);
    // does not strip pointer types, to avoid infinite recursion in rekursive data types
    SgType* strippedType2(SgType* type);
    SgExprListExp* getAggregateInitExprListExp(SgVariableDeclaration* varDecl);
    static SgClassDeclaration* getClassDeclarationOfClassType(SgClassType* type);
    static bool isUnion(SgClassType* type);
    static std::pair<bool,std::list<SgVariableDeclaration*> > memberVariableDeclarationsList(SgClassType* classType);
    static bool isDataMemberAccess(SgVarRefExp* varRefExp);
    static bool isGlobalOrLocalVariableAccess(SgVarRefExp* varRefExp);
    void setAstSymbolCheckFlag(bool flag);
    bool getAstConsistencySymbolCheckFlag();
    
  private:
    bool _astConsistencySymbolCheckFlag=true;
    
    class MemPoolTraversal : public ROSE_VisitTraversal {
    public:
      void visit(SgNode* node) {
	if(SgClassType* ctype=isSgClassType(node)) {
	  classTypes.insert(ctype);
	} else if(SgArrayType* ctype=isSgArrayType(node)) {
	  arrayTypes.insert(ctype);
	} else if(SgType* type=isSgType(node)) {
	  builtInTypes.insert(ctype);
	} else if(SgClassDefinition* cdef=isSgClassDefinition(node)) {
	  classDefinitions.insert(cdef);
	}
      }
      std::unordered_set<SgClassType*> classTypes; // class, struct, union
      std::unordered_set<SgArrayType*> arrayTypes; // any dimension
      std::unordered_set<SgType*> builtInTypes; // all other types
      std::unordered_set<SgClassDefinition*> classDefinitions; // all other pointers

      void dumpClassTypes() {
	int i=0;
	for(auto t:classTypes) {
	  auto mList=VariableIdMappingExtended::memberVariableDeclarationsList(t);
	  std::cout<<i++<<": class Type (";
	  if(mList.first)
	    std::cout<<mList.second.size();
	  else
	    std::cout<<"unknown";
	  std::cout<<") :"<<t->unparseToString()<<std::endl;
	}
      }
      void dumpArrayTypes() {
	int i=0;
	for(auto t:arrayTypes) {
	  std::cout<<"Array Type "<<i++<<":"<<t->unparseToString()<<std::endl;
	}
      }
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
    std::int32_t repairVarRefExpAccessList(std::list<SgVarRefExp*>& l, std::string accessName); // workaround
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
    std::map<SgType*,std::vector<VariableId> > classMembers;

  private:
    bool isMemberVariableDeclaration(SgVariableDeclaration*);
    // determines all size information obtainable from SgType and sets values in varidinfo
    // 3rd param can be a nullptr, in which case no decl is determined and no aggregate initializer is checked for size (this is the case for formal function parameters)
    void setVarIdInfoFromType(VariableId varId);
    std::string varIdInfoToString(VariableId varId);
  
    void recordWarning(std::string);
    std::list<std::string> _warnings;

    bool isRegisteredType(SgType* type);
    void registerType(SgType* type);
    
    MemPoolTraversal _memPoolTraversal;

    std::list<SgFunctionDefinition*> _functionDefinitions;
    std::list<SgFunctionDeclaration*> _functionDeclarations;
    std::unordered_set<SgType*> _registeredTypes;

  };
}

#endif

