
#ifndef ReadWriteSetRecords_H
#define ReadWriteSetRecords_H

#include <Rose/AST/NodeId.h>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <nlohmann/json.hpp>
#include "VxUtilFuncs.h"
#include <nlohmann/json.hpp>

namespace ast = Rose::AST;

namespace ReadWriteSets {


  /** 
   *  \brief The level of globality allowed, each higher number includes those below
   *  so MEMBERS means both LOCALS and MEMBERS are allowed, but not
   *  GLOBALS.
   *  STATIC_MEMBERS is c++ only, it allows access within a class to
   *  it's own static members, but not access to another class's static members.
   **/ 
  enum Globality {
    LOCALS = 0,
    PARAMETERS = 1,
    MEMBERS = 2,
    OTHER_CLASS_MEMBERS = 3,
    STATIC_MEMBERS = 4,
    FILE_SCOPE = 5,
    GLOBALS = 6,
    GLOBALITY_UNKNOWN = 7  //Just bigger than the others
  };


  /** 
   *  \brief The type (well, general class of type) of the variable being read or written to 
   *
   **/ 
  enum VarType {
    PRIMITIVES = 0,
    ARRAYS = 1,
    ARRAY_INDEX_EXPRESSIONS = 2,
    STRUCTS = 3,
    ARRAYS_OF_STRUCTS = 4,
    FUNCTIONS = 5,
    MEMBER_FUNCTIONS = 6,
    POINTERS = 7,
    VARTYPE_UNKNOWN = 8
  };
    
  /** 
   *  \brief The type of access of the variable.  Made to include
   *  Address_Of and dereference operations 
   **/ 
  enum AccessType {
    NORMAL = 0,
    ARRAY_INDEX = 1,
    ADDRESS_OF = 2,
    POINTER_DEREFERENCE = 3,
    POINTER_ARROW = 4,
    FUNCTION_POINTER_DEREF = 5, 
    ACCESSTYPE_UNKNOWN = 6
  };
  //! String names for Globality
  extern const char* globalityStrings[8];

  //! String names for varType
  extern const char* varTypeStrings[9];

  //! String names for AccessType
  extern const char* accessTypeStrings[7];

  //! Map to convert strings to globality
  extern std::unordered_map<std::string, Globality> stringToGlobalityMap; 
  //! Map to convert strings to accessType
  extern std::unordered_map<std::string, VarType> stringToVarTypeMap; 
  //! Map to convert strings to accessType
  extern std::unordered_map<std::string, AccessType> stringToAccessTypeMap; 

  void initStringToGlobalityMap();
  void initStringToVarTypeMap();
  void initStringToAccessTypeMap();
  
  //! Convert Globality to a string name
  std::string globalityToString(Globality globality);
  //! Convert VarType to a string name
  std::string varTypeToString(VarType varType);
  //! Convert AccessType to a string name
  std::string accessTypeToString(AccessType accessType);

  Globality globalityFromString(const std::string& globalityStr);
  VarType varTypeFromString(const std::string& varTypeStr);    
  AccessType accessTypeFromString(const std::string& accessTypeStr);
    
  std::string generateAccessNameStrings(SgFunctionDeclaration* funcDecl, SgThisExp* thisExp);
  std::string generateAccessNameStrings(SgInitializedName* coarseName, ReadWriteSets::Globality globality);
  /**
   * determineGlobality
   *
   * \brief determines the globality of current
   *
   * param[in] funcDef : The function the reference came from, mostly for error messages
   * param[in] current : The reference to investigate usually an SgInitializedName, but 
   *                     may also be a function reference or thisExp
   * param[in] accessOrigin : The line where the access is actually
   *                          performed. Only used for noteStr  
   * param[out] noteStr  : Sometimes gives a useful message about errors or why globality was determined
   **/
  ReadWriteSets::Globality determineGlobality(SgFunctionDefinition* funcDef, SgNode* current, SgNode* accessOrigin, std::string& noteStr ); 
  

  /**
   * class AccessSetRecord
   * \brief Represents an entry in a Read or Write set.  
   * 
   * Everything we know about an entry in a Read/Write set.
   * Variable name, its globality, its access type, node id, etc.
   * It also my include fields to represent dot and arrow expressions like:
   * a.b.c.  Where A is a class that contains class B, which contains field C.
   * Then C is a child of B and B is a child of A.  The insturmentation tool needs
   * this information to know exactly what to save.
   * For the Testability grader, generally A defined globality, and C determines accessType
   **/
  class AccessSetRecord {
      
  public:

      /**
       * \brief AccessSetRecord default constructor
       *
       * I'd rather this wasn't public, because it shouldn't really be used.  But 
       * nlohmann::json requires it, and always fully fills it in.  
       * YOU shouldn't use it though.
       **/
    AccessSetRecord() : 
      variableName(""), 
      globality(Globality::GLOBALITY_UNKNOWN), 
      varType(VarType::VARTYPE_UNKNOWN),
      accessType(AccessType::ACCESSTYPE_UNKNOWN),
      //nodeId default constructor used.
      noteStr("") 
      //fields is empty
    { };
    
      
      /**
       * \brief AccessSetRecord preferred constructor
       *
       * The constructor you should use, when you know everything about the access 
       **/
    AccessSetRecord(SgNode* node, const std::string inVariableName, 
                    Globality in_globality, VarType in_varType, AccessType in_accessType,
                    const std::string in_type, const std::string in_filename,
                    const std::string inNote) : 
      variableName(inVariableName), 
      globality(in_globality), 
      varType(in_varType),
      accessType(in_accessType),
      nodeId(Rose::AST::NodeId(node).toString()),
      type(in_type),
      filename(in_filename),
      noteStr(inNote) {};
      
      /**
       * \brief "this" special case AccessSet constructor
       *
       * "this" is a special case in many ways in C++
       * 1. It isn't really a variable, but acts like one.  And we have to record
       *    it because it tells us where it's child is (it's a member)
       * 2. It acts like a pointer, but isn't really one.
       *
       * So, this tells use both gobality (MEMBERS) and accessType (structs)
       * but it isn't really a variable.  So we can't save an SgInitializedName, 
       * so instead we save the information for the SgClassDeclaration
       *
       **/
      AccessSetRecord(SgFunctionDeclaration* funcDecl, SgThisExp* thisExp) : variableName(VxUtilFuncs::generateAccessNameStrings(funcDecl, thisExp)),
					    globality(Globality::MEMBERS),
					    varType(STRUCTS),
					    accessType(POINTER_ARROW),
					    nodeId(Rose::AST::NodeId(thisExp).toString()),
                                            type(""), 
                                            filename(""),
					    noteStr("") {};

    //! \brief copy constructor
    AccessSetRecord(const AccessSetRecord &rhs) : 
      variableName(rhs.variableName), 
      globality(rhs.globality), 
      varType(rhs.varType),
      accessType(rhs.accessType),
      nodeId(rhs.nodeId),
      noteStr(rhs.noteStr),
      fields(rhs.fields),
      //indexes(rhs.indexes),
      type(rhs.type),
      filename(rhs.filename) {}

    //! \brief assignment operator
    AccessSetRecord& operator=(const AccessSetRecord& rhs) {
      variableName = rhs.variableName;
      globality = rhs.globality;
      varType = rhs.varType;
      accessType = rhs.accessType;
      nodeId = rhs.nodeId;
      noteStr = rhs.noteStr;
      fields = rhs.fields;
      //indexes = rhs.indexes;
      type = rhs.type;
      filename = rhs.filename;
      return *this;
    }

      /**
       * Comparison includes accessType because one variable may be
       * accessed in multiple ways.
       **/  
    bool operator==(const AccessSetRecord& rhs) const {
      if(nodeId == rhs.nodeId && accessType == rhs.accessType) {
        return true;
      }
      return false;
    }

      /**
       * Comparison includes accessType because one variable may be
       * accessed in multiple ways.
       **/  
    bool operator<(const AccessSetRecord& rhs) const {
        if(nodeId < rhs.nodeId ||
           nodeId == rhs.nodeId && accessType < rhs.accessType) {
            return true;
        }
        return false;
    }
    //! \brief nlohmann's default way to write to json, works with >> operator 
    friend void to_json(nlohmann::json& funcJson, const ReadWriteSets::AccessSetRecord& record);
    //! \brief nlohmann's default way to create AccessSetRecord from json, works with >> operator 
    friend void from_json(const nlohmann::json& funcJson, ReadWriteSets::AccessSetRecord& record);
      

    std::string getVariableName() const { return variableName; };
    Globality getGlobality() const { return globality; };
    VarType getVarType() const { return varType; };
    AccessType getAccessType() const { return accessType; };
    std::string getType() const { return type; };
    std::string getFilename() const { return filename; };
      
      
          
    std::string getNoteStr() const { return noteStr; };

    void setGlobality(Globality in_globalit) { globality = in_globalit; };
    void setVarType(VarType in_varType) { varType = in_varType; };
    void setAccessType(AccessType in_accessType) { accessType = in_accessType; };
    void setType(std::string& in_str) { type = in_str; };
    void setFilename(std::string& in_str) { filename = in_str; };
    void setNoteStr(std::string& in_str) { noteStr = in_str; };

    //! brief Variable name, purely for human use
    std::string variableName;

    /**
     *  \brief Globality for this access
     *  WARNING: May not be interesting in itself.  A parent may
     *  define the expression's true globality in dot expressions.
     **/
    Globality globality;

    /**
     *  \brief VarType for this access
     *
     *  Gives a general kind of variable being accessed, like
     *  PRIMITIVE or STRUCT.   
     *  WARNING: May not be interesting in itself.  A child 
     *  really defines the access type for STRUCTS
     **/
    VarType varType;

    /**
     *  \brief AccessType for this access
     *
     *  Gives the way in which the variable is accessed.  So "NORMAL"
     *  a normal variable access, but you also have ADDRESS_OF (&), 
     *  POINTER_DEREFERENCE (*), and ARROW (->)
     **/
    AccessType accessType;

    /**
     *  \brief NodeId for the SgInitializedName of this access 
     *
     *  WARNING: Invalid if ROSE was not run in exactly the same
     *  way as this nodeId was generated for.  Additional files or 
     *  different file orderings WILL make this invalid. See  FunctionReadWriteRecord
     *  for the relevent command line
     **/
    std::string nodeId;

    /**
     *  \brief String representing fully qualified, no-typedef type of
     *  the varaible
     *
     *  WARNING: Header file include may be required to disambiguate
     *  the type
     **/
      std::string type;

    /**
     *  \brief Filename where the variable is defined, so unit test
     *  can tell if a definition is necessary
     *
     **/
      std::string filename;
      
    //! \brief A note string about Globality.  Maybe delete later.
    std::string noteStr;
   
    /**
     *  The fields reference set represents class member references.
     *  For example a.b.c we need to know all the information about a, b, AND c.
     *  So we need 3 AccessSetRecords, and their relationship needs to be kept.
     *  Most references dont require this.
     *  WARNING
     *  The entries in fields CANNOT affect set or unordered set ordering, so I made
     *  it mutable to simplify the code.  Make sure you don't break this invariant! 
     *  (By changing the relational operators)
     **/
    mutable std::set<AccessSetRecord> fields;

    /**
     *  The indexes reference set represents array index expressions.
     *  For example a[x-1] Matt wants to record x-1 as the index expression so 
     *  he knows what to save from an array
     *  Most references dont require this.
     *  WARNING
     *  The entries in fields CANNOT affect set or unordered set ordering, so I made
     *  it mutable to simplify the code.  Make sure you don't break this invariant! 
     *  (By changing the relational operators)
     **/
//    mutable std::set<AccessSetRecord> indexes;

      
  };

  /**
   * \brief Hash operator for AccessSetRecord for use with unordered_set or _map.
   **/
  struct AccessSetRecord_hash {
    std::size_t operator()(const AccessSetRecord& record) const {
        std::string hashString = record.nodeId + accessTypeToString(record.accessType) ;
        return std::hash<std::string>()(hashString);
    }
  };

    
  class FunctionReadWriteRecord {
  public:
    
    /**
     * \brief FunctionReadWriteRecord default constructor
     *
     * I'd rather this wasn't public, because it shouldn't really be used.  But 
     * nlohmann::json requires it, and always fully fills it in.  
     * YOU shouldn't use it though.
     **/
    FunctionReadWriteRecord() :
      sgFunctionDeclaration(nullptr),
      filename(""),
      commandLine(""),
      nodeId(""),
      isFileScope(false),
      globality(Globality::GLOBALITY_UNKNOWN),
      varType(VarType::VARTYPE_UNKNOWN),
      accessType(AccessType::ACCESSTYPE_UNKNOWN),
      internalFunctionName("") {};
    

    /**
     * \brief FunctionReadWriteRecord preferred constructor
     *
     * \param[in] inSgFunctionDeclaration: Almost all necessary information can be pulled from an SgFunctionDeclaration
     * \param[in] commandLine: Defined the commandLine the function was generated from (except for argv[0] which 
     *                         is the tool name.)  Useful for humans trying to run in the same way to get the same nodeIds
     *                         Currently not used for automatic verification, but maybe in the future.
     *          
     **/
    FunctionReadWriteRecord(SgFunctionDeclaration* inSgFunctionDeclaration, const std::string& commandLine) :
      sgFunctionDeclaration(inSgFunctionDeclaration),
      filename(VxUtilFuncs::getNodeRelativePath(inSgFunctionDeclaration)),
      commandLine(commandLine),
      nodeId(Rose::AST::NodeId(inSgFunctionDeclaration).toString()),
      isFileScope(VxUtilFuncs::isFileScope(inSgFunctionDeclaration)), //FIXME TODO
      globality(Globality::GLOBALITY_UNKNOWN),
      varType(VarType::VARTYPE_UNKNOWN),
      accessType(AccessType::ACCESSTYPE_UNKNOWN),
      internalFunctionName(VxUtilFuncs::compileInternalFunctionName(inSgFunctionDeclaration, filename)) {};
                   

    bool operator==(const FunctionReadWriteRecord& rhs) const {
      if(internalFunctionName == rhs.internalFunctionName) {
        return true;
      }
      return false;
    }
    bool operator<(const FunctionReadWriteRecord& rhs) const {
      if(internalFunctionName < rhs.internalFunctionName) {
        return true;
      }
      return false;
    }

    //! \brief nlohmann's default way to write to json, works with >> operator 
    friend void to_json(nlohmann::json& funcJson, const ReadWriteSets::FunctionReadWriteRecord& record);

    //! \brief nlohmann's default way to create AccessSetRecord from json, works with >> operator 
    friend void from_json(const nlohmann::json& funcJson, ReadWriteSets::FunctionReadWriteRecord& record);


    /**
     *  \brief sgFunctionDeclaration that defines this function. (For convienence )
     *
     *  WARNING: Will be null if read from a json file.  We can't 
     *  guarantee that the nodeId will be valid, so users of this
     *  class may need to fill this in if they want it.
     *
     * TODO: turn into an access function and make this private
     **/
    SgFunctionDeclaration* sgFunctionDeclaration;

    //\brief Name of file this function was defined in
    std::string filename;
    /**
     * \brief Command line used to generate this function, may be used to make nodeIds valid
     * WARNING, not verified automatically.
     **/
    std::string commandLine;
      
    /**
     * \brief NodeId for this SgFunctionDeclaration
     * WARNING, may not be valid if ROSE was run with different commandLine
     **/
    std::string nodeId;
    
    //! \brief local readSet of this function (does not include called Functions)
    std::set<AccessSetRecord> readSet;
    //! \brief local writeSet of this function (does not include called Functions)
    std::set<AccessSetRecord> writeSet;

    //! \brief True iff the function is static or in an anonymous namespace (ie, only has file scope)
    bool isFileScope;

    //! \brief Globality of this function.  May be GLOBALITY_UNKNOWN. TestabilityGrader fills this in
    Globality globality;

    //! \brief VarType of this function.  May be VARTYPE_UNKNOWN. TestabilityGrader fills this in
    VarType varType;

    //! \brief AccessType of this function.  May be ACCESSTYPE_UNKNOWN. TestabilityGrader fills this in
    AccessType accessType;

    //! \brief List of called functions by "internalFunctionName", which should be enough to find them in the AST if NodeId is not valid. 
    std::unordered_set<std::string> calledFunctions;

    //! \brief Internal Function Name, Key used in hash. Should be enough to find them in the AST if NodeId is not valid. 
    std::string internalFunctionName;
  };

  /**
   * \brief Hash operator for FunctionReadWriteRecord for use with unordered_set or _map.
   **/
  struct FunctionReadWriteRecord_hash {
    std::size_t operator()(const FunctionReadWriteRecord& record) const {
      return std::hash<std::string>()(record.internalFunctionName);
    }
  };

  /**
   * \brief Iterate down to bottom field and insert record.
   * Used to insert field AccessSetRecords when recursing down an a.b.c type access.
   * (Because it seems to be the only way to get the ordering right)
   **/
  void leafFieldInsert(std::set<AccessSetRecord>& set, std::set<AccessSetRecord>& records);

  /**
   * \brief Insert index into indexes of record in set
   **/
//  void leafIndexInsert(std::set<AccessSetRecord>& set, std::set<AccessSetRecord>& records);


  /**
   * \brief Each AccessSetRecord may contain a bunch of AccessSetRecords, so walk down the list, inserting and creating as necessary.
   **/
  void recursiveInsert(std::set<AccessSetRecord>& set, const AccessSetRecord& record);

  /**
   * \brief Uses recursive insert to merge sourceSet into targetSet
   **/
  void recursiveMerge(std::set<AccessSetRecord>& targetSet, const std::set<AccessSetRecord>& sourceSet);

  /**
   * \brief Uses recursive insert to merge sourceSet into targetSet
   **/
  void recursiveMerge(std::set<AccessSetRecord>& targetSet, const std::vector<AccessSetRecord>& sourceSet);

  /**
   * \brief Tried to make a unique name from an AccessSetRecord.  Really for function pointers. Assumes each item only has one child
   **/
  std::set<std::string>recursivelyMakeName(std::set<AccessSetRecord>& targetSet);
  

  /**
   * Recursively search an accessSet to find the worst globality and accessType
   * WARNING, before calling set maxGlobality and maxAccessType <= 0
   *
   * \param[in] targetSet : Determine the maxGlobality and maxAccessType for this AccessSetRecord and all its fields
   * \param[out] maxGlobality 
   * \param[out] maxAccessType 
   **/
  void maxTestability(const std::set<AccessSetRecord>& targetSet, 
                      Globality& maxGlobality, VarType& maxVarType, AccessType& maxAccessType);

  /**
   * Recursively search an a FunctionReadWriteRecord's readSet and writeSet to find the worst globality and accessType
   * WARNING, before calling set maxGlobality and maxAccessType <= 0
   *
   * \param[in] targetSet : Determine the maxGlobality and maxAccessType for this AccessSetRecord and all its fields
   * \param[out] maxGlobality 
   * \param[out] maxAccessType 
   **/
  void maxTestability(const FunctionReadWriteRecord& targetRecord, 
                      Globality& maxGlobality, VarType& maxVarType, AccessType& maxAccessType);

  /**
   * \brief Modify all accessTypes.
   * When determining accessType in recursivelyMakeAccessRecord, sometimes we don't know the correct accessType
   * when we make the AccessSetRecord, so we have to modify it later.  But you can't modify set items in place,
   * so I had to make this function to handle this case.  It just makes a new set to replace the old one.
   **/
  std::set<AccessSetRecord> updateAccessTypes(const std::set<AccessSetRecord>& inSet, AccessType newAccessType);

  /**
   * \brief Modify all accessTypes, except "this".
   * When determining accessType in recursivelyMakeAccessRecord, sometimes we don't know the correct accessType
   * when we make the AccessSetRecord, so we have to modify it later.  But you can't modify set items in place,
   * so I had to make this function to handle this case.  It just makes a new set to replace the old one.
   **/
  std::set<AccessSetRecord> updateAccessTypesIfNotThis(const std::set<AccessSetRecord>& inSet, AccessType newType);

  /**
   * \brief Modify all varTypes and accessTypes.
   * Used for arrays, where we sometimes have to update both varType
   * and accessType to an array related type.
   **/
  std::set<AccessSetRecord> updateAccessVarTypes(const std::set<AccessSetRecord>& inSet, VarType newVarType, AccessType AccessType);

  //! \brief nlohmann's default way to write to json, works with >> operator 
  void to_json(nlohmann::json& funcJson, const ReadWriteSets::AccessSetRecord& record);

  //! \brief nlohmann's default way to create AccessSetRecord from json, works with >> operator 
  void from_json(const nlohmann::json& funcJson, ReadWriteSets::AccessSetRecord& record);

  //! \brief nlohmann's default way to write to json, works with >> operator 
  void to_json(nlohmann::json& funcJson, const ReadWriteSets::FunctionReadWriteRecord& record);

    //! \brief nlohmann's default way to create AccessSetRecord from json, works with >> operator 
  void from_json(const nlohmann::json& funcJson, ReadWriteSets::FunctionReadWriteRecord& record);
  
  /**
   * Walks through the cache inserting it all into a nlohmann json object.
   * It's possible that if I used a std::string to represent the function
   * instead of an SgFunctionDeclaration nlohmann could convert the whole
   * cache directly to json, but using an SgFunctionDeclaration is 
   * convienent when actually making the cache.
   * This just reads directly from the global cache, so no arguments needed
   **/
  nlohmann::json convertCacheToJson(const std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& rwSetCache); 


  /**
   * \brief Recreates the rwSetCache from a jsonDocument
   *
   * Used by TestabilityGrader.  WARNING, NodeIds may not be valid, check that FunctionReadWriteRecord.commandLine matches
   **/
  void readCacheFromJson(const nlohmann::json& jsonDocument, std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& rwSetCache);

};



#endif //ReadWriteSetRecords_H
