#ifndef VARIABLEIDMAPPING_H
#define VARIABLEIDMAPPING_H

#include <string>
#include <map>
#include <vector>
#include <boost/unordered_set.hpp>

#include "RoseAst.h"
#include "SgNodeHelper.h"

namespace CodeThorn {

  class VariableId;
  typedef std::string VariableName;

  // typesize in bytes
  typedef long int TypeSize;

  class VariableIdMapping {
    /* NOTE: cases where the symbol is in the ROSE AST:
       1) SgInitializedName in forward declaration (symbol=0)
       2) CtorInitializerList (symbol=0)
       the symbol is missing in both cases, a VariableId can be assign to the passed SgInitializedName pointer.
    */

  public:
    VariableIdMapping();
    virtual ~VariableIdMapping();
    typedef std::set<VariableId> VariableIdSet;

    /**
     * create the mapping between symbols in the AST and associated
     * variable-ids. Each variable in the project is assigned one
     * variable-id (including global variables, local variables,
     * class/struct/union data members)
     * 
     * param[in] project: The Rose AST we're going to act on
     * param[in] maxWarningsCount: A limit for the number of warnings to print.  0 = no warnings -1 = all warnings
    */    
    virtual void computeVariableSymbolMapping(SgProject* project, int maxWarningsCount = 3);
    
    /* create a new unique variable symbol (should be used together with
       deleteUniqueVariableSymbol) this is useful if additional
       (e.g. temporary) variables are introduced in an analysis this
       function does NOT insert this new symbol in any symbol table
    */
    VariableId createUniqueTemporaryVariableId(std::string name);
    bool isTemporaryVariableId(VariableId varId);
    bool isTemporaryVariableIdSymbol(SgSymbol* sym);
    bool isHeapMemoryRegionId(VariableId varId);

    // delete a unique variable symbol (should be used together with createUniqueVariableSymbol)
    void deleteUniqueTemporaryVariableId(VariableId uniqueVarSym);

    class UniqueTemporaryVariableSymbol : public SgVariableSymbol {
    public:
      UniqueTemporaryVariableSymbol(std::string name);
      // Destructor: default is sufficient
    
      // overrides inherited get_name (we do not use a declaration)
      SgName get_name() const;
    protected:
      std::string _tmpName;
    };

    VariableId variableId(SgVariableDeclaration* decl);
    VariableId variableId(SgVarRefExp* varRefExp);
    VariableId variableId(SgInitializedName* initName);
    VariableId variableId(SgSymbol* sym);
    VariableId variableIdFromCode(int);
    SgSymbol* getSymbol(VariableId varId);
    virtual SgType* getType(VariableId varId);

    // returns true if this variable is Of type bool. This also includes the C type _Bool.
    bool isOfBoolType(VariableId varId);
    // returns true if this variable is of any signed or unsigned char type (char,char16,char32)
    bool isOfCharType(VariableId varId);
    // returns true if this variable is of any signed or unsigned integer type (short,int,long,longlong)
    bool isOfIntegerType(VariableId varId);
    // returns true if this variable is of an enum type 
    bool isOfEnumType(VariableId varId);
    // returns true if this variable is of any floating-point type (float,double,longdouble,float80,float128))
    bool isOfFloatingPointType(VariableId varId);
    bool isOfPointerType(VariableId varId);
    // schroder3 (2016-07-05): Returns whether the given variable is a reference variable
    bool isOfReferenceType(VariableId varId);
    bool isOfClassType(VariableId varId);
    bool isOfArrayType(VariableId varId);

    virtual SgVariableDeclaration* getVariableDeclaration(VariableId varId);
    // schroder3 (2016-07-05): Returns whether the given variable is valid in this mapping
    bool isVariableIdValid(VariableId varId);
    std::string variableName(VariableId varId);
    std::string uniqueVariableName(VariableId varId);

    // set number of elements of the memory region determined by this variableid
    virtual void setNumberOfElements(VariableId variableId, size_t size);
    // get number of elements of the memory region determined by this variableid
    virtual TypeSize getNumberOfElements(VariableId variableId);

    virtual void setNumDimensionElements(VariableId variableId, TypeSize dimNr, TypeSize numElems);
    virtual TypeSize getNumDimensionElements(VariableId variableId, TypeSize dimNr);

    
    // set the size of an element of the memory region determined by this variableid
    virtual void setElementSize(VariableId variableId, TypeSize size);
    // get the size of an element of the memory region determined by this variableid
    virtual TypeSize getElementSize(VariableId variableId);

    // set total size in bytes of variableId's memory region (for arrays not necessary, computed from other 2 values)
    virtual void setTotalSize(VariableId variableId, TypeSize size);
    virtual TypeSize getTotalSize(VariableId variableId);
    
    // set offset of member variable (type is implicit as varids are unique across all types)
    virtual void setOffset(VariableId variableId, TypeSize offset);
    // get offset of member variable (type is implicit as varids are unique across all types)
    virtual TypeSize getOffset(VariableId variableId);
    virtual bool isMemberVariable(VariableId variableId);
    virtual void setIsMemberVariable(VariableId variableId, bool flag);
    
    SgSymbol* createAndRegisterNewSymbol(std::string name);
    CodeThorn::VariableId createAndRegisterNewVariableId(std::string name);
    CodeThorn::VariableId createAndRegisterNewMemoryRegion(std::string name, int regionSize);
    void registerNewSymbol(SgSymbol* sym);
    void registerNewArraySymbol(SgSymbol* sym, TypeSize arraySize);
    virtual void toStream(std::ostream& os);
    void generateDot(std::string filename,SgNode* astRoot);

    VariableIdSet getVariableIdSet();
    VariableIdSet determineVariableIdsOfVariableDeclarations(std::set<SgVariableDeclaration*> varDecls);
    VariableIdSet determineVariableIdsOfSgInitializedNames(SgInitializedNamePtrList& namePtrList);
    VariableIdSet variableIdsOfAstSubTree(SgNode* node);

    /* if this mode is activated variable ids are created for each element of arrays with fixed size
       e.g. a[3] gets assigned 3 variable-ids (where the first one denotes a[0])
       this mode must be set before the mapping is computed with computeVariableSymbolMapping
    */
    //void setModeVariableIdForEachArrayElement(bool active);
    //bool getModeVariableIdForEachArrayElement();

    bool hasAssignInitializer(VariableId arrayVar);
    bool isAggregateWithInitializerList(VariableId arrayVar);
    SgExpressionPtrList& getInitializerListOfArrayVariable(VariableId arrayVar);
    virtual size_t getArrayDimensions(SgArrayType* t, std::vector<size_t> *dimensions = NULL);
    virtual size_t getArrayElementCount(SgArrayType* t);
    virtual size_t getArrayDimensionsFromInitializer(SgAggregateInitializer* init, std::vector<size_t> *dimensions = NULL);
    virtual VariableId idForArrayRef(SgPntrArrRefExp* ref);
  
    // memory locations of string literals
    void registerStringLiterals(SgNode* root);
    int numberOfRegisteredStringLiterals();
    VariableId getStringLiteralVariableId(SgStringVal* sval);
    bool isStringLiteralAddress(VariableId stringVarId);
    std::map<SgStringVal*,VariableId>* getStringLiteralsToVariableIdMapping();

    // returns true if the variable is a formal parameter in a function definition
    virtual bool isFunctionParameter(VariableId varId);

    // determines for struct/class/union's data member if its
    // SgInitializeName defines an anonymous bitfield (e.g. struct S {
    // int :0; }). Anonymous bitfields in the same struct are mapped
    // to the same SgSymbol. This function is used to handle this
    // special case.
    static bool isAnonymousBitfield(SgInitializedName* initName);
    std::string mangledName(VariableId varId);

    static bool isUnknownSizeValue(TypeSize size);
    static TypeSize unknownSizeValue();
    
    // link analysis is by default disabled (=false)
    void setLinkAnalysisFlag(bool);
    enum AggregateType { AT_UNKNOWN, AT_SINGLE, AT_ARRAY, AT_STRUCT };
    enum VariableScope { VS_UNKNOWN, VS_LOCAL, VS_GLOBAL, VS_MEMBER };
    bool isVolatile(VariableId varId);
    void setVolatileFlag(VariableId varId, bool flag);

  protected:
    struct VariableIdInfo {
    public:
      VariableIdInfo();
      SgSymbol* sym;
      TypeSize numberOfElements; // can be zero for arrays, it is 1 for a single variable, for structs/classes/unions it is the number of member variables, if unknown -1.
      TypeSize elementSize; // in bytes, if unknown -1
      std::map<TypeSize,TypeSize> numDimensionElements; // in elements, if unknown -1 or non existent (getDimensionElements(0) returns -1)
      TypeSize totalSize; // in bytes, if unknown -1
      TypeSize offset;      // in bytes, only for member variables, if unknown -1
      AggregateType aggregateType;
      VariableScope variableScope;
      bool isVolatileFlag;
      bool relinked; // true if link analysis relinked this entry
      std::string toString();
      std::string aggregateTypeToString();
      std::string variableScopeToString();
    };
    std::map<SgStringVal*,VariableId> sgStringValueToVariableIdMapping;
    std::map<VariableId, SgStringVal*> variableIdToSgStringValueMapping;

    void generateStmtSymbolDotEdge(std::ofstream&, SgNode* node,VariableId id);
    std::string generateDotSgSymbol(SgSymbol* sym);
    typedef std::pair<std::string,SgSymbol*> MapPair;
    typedef std::pair<VariableId,VariableName> PairOfVarIdAndVarName;
    typedef std::set<PairOfVarIdAndVarName> TemporaryVariableIdMapping;
    TemporaryVariableIdMapping temporaryVariableIdMapping;
    VariableId addNewSymbol(SgSymbol* sym);

    // used for link analysis of global variables based on mangled names
    typedef std::map<SgName,std::set<SgSymbol*> > VarNameToSymMappingType;
    VarNameToSymMappingType mappingGlobalVarNameToSymSet;

    // used for mapping in both directions
    std::map<SgSymbol*,VariableId> mappingSymToVarId;
    std::map<VariableId,VariableIdInfo> mappingVarIdToInfo;

    SgSymbol* selectLinkSymbol(std::set<SgSymbol*>& symSet);
    void performLinkAnalysisRemapping();
    bool linkAnalysis;
  public:
    VariableIdInfo getVariableIdInfo(VariableId vid);
    VariableIdInfo* getVariableIdInfoPtr(VariableId vid);
    void setVariableIdInfo(VariableId vid, VariableIdInfo vif);

  }; // end of class VariableIdMapping

  typedef VariableIdMapping::VariableIdSet VariableIdSet;

  class VariableId {
    friend class VariableIdMapping;
    friend bool operator<(VariableId id1, VariableId id2);
    friend bool operator==(VariableId id1, VariableId id2);
    friend class ConstraintSetHashFun;
    //friend size_t hash_value(const VariableId&);
  public:
    VariableId();
    std::string toString() const;
    std::string toString(VariableIdMapping& vid) const;
    //std::string toUniqueString() const;
    std::string toUniqueString(VariableIdMapping& vid) const;

    /* if VariableIdMapping is a valid pointer a variable name is returned
       otherwise toString() is called and a generic name (V..) is returned.
    */
    std::string toString(VariableIdMapping* vid) const;
    std::string toUniqueString(VariableIdMapping* vid) const;

    int getIdCode() const { return _id; }
    void setIdCode(int id) {_id=id;}
    bool isValid() const { return _id!=-1; }
    static const char * const idKindIndicator;
  public:

  private: 
    int _id;
  };

  size_t hash_value(const VariableId& vid);

  bool operator<(VariableId id1, VariableId id2);
  bool operator==(VariableId id1, VariableId id2);
  bool operator!=(VariableId id1, VariableId id2);
  VariableIdSet& operator+=(VariableIdSet& s1, VariableIdSet& s2);

}

// backward compatibility
namespace SPRAY = CodeThorn;

#endif
