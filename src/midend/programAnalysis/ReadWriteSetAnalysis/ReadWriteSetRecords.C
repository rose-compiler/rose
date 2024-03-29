#include <sage3basic.h>
#include "ReadWriteSetRecords.h"
#include <string>
#include <algorithm>
#include <sageInterface.h>

const char*ReadWriteSets:: globalityStrings[] = {
  "LOCALS",
  "PARAMETERS",
  "MEMBERS",
  "OTHER_CLASS_MEMBERS",
  "STATIC_MEMBERS",
  "FILE_SCOPE",
  "GLOBALS",
  "GLOBALITY_END"
};

const char* ReadWriteSets::varTypeStrings[] = {
  "PRIMITIVES",
  "ARRAYS",
  "ARRAY_INDEX_EXPRESSIONS",
  "STRUCTS",
  "ARRAYS_OF_STRUCTS",
  "FUNCTIONS",
  "MEMBER_FUNCTIONS",
  "POINTERS",
  "VARTYPE_END"
};


const char* ReadWriteSets::accessTypeStrings[] = {
  "NORMAL",
  "ARRAY_INDEX",
  "ADDRESS_OF",
  "POINTER_DEREFERENCE",
  "POINTER_ARROW",
  "FUNCTION_POINTER_DEREF",
  "ACCESSTYPE_END"
};

//! Map to convert strings to globality
std::unordered_map<std::string, ReadWriteSets::Globality> ReadWriteSets::stringToGlobalityMap; 
//! Map to convert strings to varType
std::unordered_map<std::string, ReadWriteSets::VarType> ReadWriteSets::stringToVarTypeMap; 
//! Map to convert strings to accessType
std::unordered_map<std::string, ReadWriteSets::AccessType> ReadWriteSets::stringToAccessTypeMap; 


std::string ReadWriteSets::globalityToString(ReadWriteSets::Globality globality) {
  if(globality >= GLOBALITY_UNKNOWN) {
    return "GLOBALITY_UNKNOWN";
  }
  return globalityStrings[globality];
}

std::string ReadWriteSets::varTypeToString(ReadWriteSets::VarType varType) {
  if(varType >= VARTYPE_UNKNOWN) {
    return "VARTYPE_UNKNOWN";
  }
  return varTypeStrings[varType];
}

std::string ReadWriteSets::accessTypeToString(ReadWriteSets::AccessType accessType) {
  if(accessType == ACCESSTYPE_UNKNOWN) {
    return "ACCESSTYPE_UNKNOWN";
  }
  return accessTypeStrings[accessType];
}

ReadWriteSets::Globality ReadWriteSets::globalityFromString(const std::string& globalityStr) {
  if(stringToGlobalityMap.size() == 0) {
    initStringToGlobalityMap();
  }
  auto iter = stringToGlobalityMap.find(globalityStr);
  ROSE_ASSERT(iter != stringToGlobalityMap.end());
  return iter->second;
}

ReadWriteSets::VarType ReadWriteSets::varTypeFromString(const std::string& varTypeStr) {
  if(stringToVarTypeMap.size() == 0) {
    initStringToVarTypeMap();
  }
  auto iter = stringToVarTypeMap.find(varTypeStr);
  ROSE_ASSERT(iter != stringToVarTypeMap.end());
  return iter->second;
}


ReadWriteSets::AccessType ReadWriteSets::accessTypeFromString(const std::string& accessTypeStr) {
  if(stringToAccessTypeMap.size() == 0) {
    initStringToAccessTypeMap();
  }
  auto iter = stringToAccessTypeMap.find(accessTypeStr);
  ROSE_ASSERT(iter != stringToAccessTypeMap.end());
  return iter->second;
}

void ReadWriteSets::initStringToGlobalityMap() {
  stringToGlobalityMap.insert(std::make_pair<std::string, ReadWriteSets::Globality>("LOCALS",LOCALS));
  stringToGlobalityMap.insert(std::make_pair<std::string, ReadWriteSets::Globality>("PARAMETERS",PARAMETERS));
  stringToGlobalityMap.insert(std::make_pair<std::string, ReadWriteSets::Globality>("MEMBERS",MEMBERS));
  stringToGlobalityMap.insert(std::make_pair<std::string, ReadWriteSets::Globality>("OTHER_CLASS_MEMBERS",OTHER_CLASS_MEMBERS));
  stringToGlobalityMap.insert(std::make_pair<std::string, ReadWriteSets::Globality>("STATIC_MEMBERS",STATIC_MEMBERS));
  stringToGlobalityMap.insert(std::make_pair<std::string, ReadWriteSets::Globality>("FILE_SCOPE", FILE_SCOPE));
  stringToGlobalityMap.insert(std::make_pair<std::string, ReadWriteSets::Globality>("GLOBALS",GLOBALS));
  stringToGlobalityMap.insert(std::make_pair<std::string, ReadWriteSets::Globality>("GLOBALITY_UNKNOWN", GLOBALITY_UNKNOWN));
}

void ReadWriteSets::initStringToVarTypeMap() {
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("PRIMITIVES",PRIMITIVES));
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("ARRAYS",ARRAYS));
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("ARRAY_INDEX_EXPRESSIONS",ARRAY_INDEX_EXPRESSIONS));
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("STRUCTS",STRUCTS));
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("ARRAYS_OF_STRUCTS",ARRAYS_OF_STRUCTS));
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("FUNCTIONS",FUNCTIONS));
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("MEMBER_FUNCTIONS",MEMBER_FUNCTIONS));
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("POINTERS",POINTERS));
  stringToVarTypeMap.insert(std::make_pair<std::string, ReadWriteSets::VarType>("VARTYPE_UNKNOWN",VARTYPE_UNKNOWN));
}

void ReadWriteSets::initStringToAccessTypeMap() {
  stringToAccessTypeMap.insert(std::make_pair<std::string, ReadWriteSets::AccessType>("NORMAL", NORMAL));
  stringToAccessTypeMap.insert(std::make_pair<std::string, ReadWriteSets::AccessType>("ARRAY_INDEX", ARRAY_INDEX));
  stringToAccessTypeMap.insert(std::make_pair<std::string, ReadWriteSets::AccessType>("ADDRESS_OF",ADDRESS_OF));
  stringToAccessTypeMap.insert(std::make_pair<std::string, ReadWriteSets::AccessType>("POINTER_DEREFERENCE",POINTER_DEREFERENCE));
  stringToAccessTypeMap.insert(std::make_pair<std::string, ReadWriteSets::AccessType>("POINTER_ARROW",POINTER_ARROW));
  stringToAccessTypeMap.insert(std::make_pair<std::string, ReadWriteSets::AccessType>("FUNCTION_POINTER_DEREF",FUNCTION_POINTER_DEREF));
  stringToAccessTypeMap.insert(std::make_pair<std::string, ReadWriteSets::AccessType>("ACCESSTYPE_UNKNOWN",ACCESSTYPE_UNKNOWN));
}

/**
 * \brief Iterate down to bottom field and insert record.
 * Used to insert field AccessSetRecords when recursing down an a.b.c type access.
 **/
void ReadWriteSets::leafFieldInsert(std::set<AccessSetRecord>& targetSet, std::set<AccessSetRecord>& records) {
  if(targetSet.size() == 0) {
    for(auto &record : records) {
      targetSet.insert(record);
    }
    return;
  } 
  for(auto const &target : targetSet) {
    leafFieldInsert(target.fields, records);
  }
}

/**
 * \brief Insert index into indexes of record in targetSet 
 * When you have: record[expr] we insert "expr" as a "index child" record into targetSet 
 * Jim Leek 2023/03/02: Removed because Matt doesn't want it anymore  
 **/
/*void ReadWriteSets::leafIndexInsert(std::set<AccessSetRecord>& targetSet, std::set<AccessSetRecord>& records) {
  if(targetSet.size() == 0) {
    for(auto &record : records) {
      targetSet.insert(record);
    }
    return;
  } 

  for(auto const &target : targetSet) {
    leafIndexInsert(target.indexes, records);
    }
  return;
  }*/

/**
 * Each AccessSetRecord may contain a bunch of AccessSetRecords, so walk down the list, inserting and creating as necessary.
 **/
void ReadWriteSets::recursiveInsert(std::set<AccessSetRecord>& set, const AccessSetRecord& record) {
  std::set<AccessSetRecord>::iterator setIt = set.find(record);
  //If the record of this name DOES NOT exist, we don't have to merge, just insert and return, which will include everything
  if(setIt == set.end()) {
    set.insert(record);
    return;
  }
  //If the record DOES exist already we need to merge.  First we verify that they have equal metadata (safety check they should)
  ROSE_ASSERT(record.variableName == setIt->variableName &&
              record.globality == setIt->globality &&
              record.nodeId == setIt->nodeId);


  //Insert each field of record into the found record field set
  AccessSetRecord setCopy = *setIt;

  //When we're merging up the tree, sometimes a pointer rereference happens in one place on a given type, but not another
  //We want to be sure to capture the worst case.
  setCopy.accessType = std::max(record.accessType, setIt->accessType);
  for(const AccessSetRecord &recordField : record.fields) {
    recursiveInsert(setCopy.fields, recordField);
  }
/*  for(const AccessSetRecord &recordIndex : record.indexes) {
    recursiveInsert(setCopy.indexes, recordIndex);
    }*/
  set.erase(setIt);
  set.insert(setCopy);

}

/**
 * Uses recursive insert to merge sourceSet into targetSet
 **/
void ReadWriteSets::recursiveMerge(std::set<AccessSetRecord>& targetSet, const std::set<AccessSetRecord>& sourceSet) {
  auto sourceIt = sourceSet.begin();
  for(;sourceIt != sourceSet.end(); ++sourceIt) {
    recursiveInsert(targetSet, *sourceIt);
  }
  return;
}

/**
 * Uses recursive insert to merge sourceSet into targetSet
 **/
void ReadWriteSets::recursiveMerge(std::set<AccessSetRecord>& targetSet, const std::vector<AccessSetRecord>& sourceSet) {
  auto sourceIt = sourceSet.begin();
  for(;sourceIt != sourceSet.end(); ++sourceIt) {
    recursiveInsert(targetSet, *sourceIt);
  }
  return;
}

/**
 * \brief Tried to make a unique name from an AccessSetRecord.  Really for function pointers. Assumes each item only has one child
 **/
std::set<std::string> ReadWriteSets::recursivelyMakeName(std::set<AccessSetRecord>& inputSet) 
{
  std::set<std::string> outputSet;

  for(auto inputIt = inputSet.begin(); inputIt != inputSet.end(); ++inputIt) {
    std::set<std::string> subSet = recursivelyMakeName(inputIt->fields);
    for(auto nameIt = subSet.begin(); nameIt != subSet.end(); ++nameIt) {
      if(*nameIt == "") {
        continue;
      }
      outputSet.insert(inputIt->variableName + "." + *nameIt);
    }
    if(outputSet.empty()) {
      //If we're a leaf, this will be empty because no children, so
      //we're the set to return
      outputSet.insert(inputIt->variableName);
    }
    
  }

  return outputSet;
}



/**
 * Recursively search an accessSet to find the worst globality and accessType
 * WARNING, before calling set maxGlobality and maxAccessType <= 0
 *
 * \param[in] targetSet : Determine the maxGlobality and maxAccessType for this AccessSetRecord and all its fields
 * \param[out] maxGlobality 
 * \param[out] maxAccessType 
 **/
void ReadWriteSets::maxTestability(const std::set<AccessSetRecord>& targetSet, Globality& maxGlobality, 
                                   VarType& maxVarType, AccessType& maxAccessType) {
  for(auto& record : targetSet) {
    if(record.globality > maxGlobality) {
      maxGlobality = record.globality;
    }
    if(record.varType > maxVarType) {
      maxVarType = record.varType;
    }
    if(record.accessType > maxAccessType) {
      maxAccessType = record.accessType;
    }
    maxTestability(record.fields, maxGlobality, maxVarType, maxAccessType);
  }
  return;
}



/**
 * Recursively search an a FunctionReadWriteRecord's readSet and writeSet to find the worst globality and accessType
 * WARNING, before calling set maxGlobality and maxAccessType <= 0
 *
 * \param[in] targetSet : Determine the maxGlobality and maxAccessType for this AccessSetRecord and all its fields
 * \param[out] maxGlobality 
 * \param[out] maxAccessType 
 **/
void ReadWriteSets::maxTestability(const FunctionReadWriteRecord& targetRecord, Globality& maxGlobality, 
                                   VarType& maxVarType, AccessType& maxAccessType) {
  
  maxTestability(targetRecord.readSet, maxGlobality, maxVarType, maxAccessType);
  maxTestability(targetRecord.writeSet, maxGlobality, maxVarType, maxAccessType);
  return;
}

/**
 * Modify all accessTypes.
 * When determining accessType in recursivelyMakeAccessRecord, sometimes we don't know the correct accessType
 * when we make the AccessSetRecord, so we have to modify it later.  But you can't modify set items in place,
 * so I had to make this function to handle this case.  It just makes a new set to replace the old one.
 **/
std::set<ReadWriteSets::AccessSetRecord> ReadWriteSets::updateAccessTypes(
  const std::set<ReadWriteSets::AccessSetRecord>& inSet, AccessType newAccessType) {

  std::set<ReadWriteSets::AccessSetRecord> outSet;
  for(ReadWriteSets::AccessSetRecord record : inSet) {
    record.setAccessType(newAccessType);
    outSet.insert(record);
  }
  return outSet;
}

/**
 * Modify all accessTypes if not this
 * There's one special case this->, which isn't really a POINTERS.  But there's no way to check without cracking
 * open the set, so just do that here.
 **/
std::set<ReadWriteSets::AccessSetRecord> ReadWriteSets::updateAccessTypesIfNotThis(
  const std::set<ReadWriteSets::AccessSetRecord>& inSet, 
  AccessType newAccessType) { 

  std::set<ReadWriteSets::AccessSetRecord> outSet;
  for(ReadWriteSets::AccessSetRecord record : inSet) {
    if(record.variableName != "this") {
      record.setAccessType(newAccessType);
    }
    outSet.insert(record);
  }
  return outSet;
}

/**
 * Modify all varTypes.
 * When determining varType in recursivelyMakeVarRecord, sometimes we don't know the correct varType
 * when we make the VarSetRecord, so we have to modify it later.  But you can't modify set items in place,
 * so I had to make this function to handle this case.  It just makes a new set to replace the old one.
 **/
std::set<ReadWriteSets::AccessSetRecord> ReadWriteSets::updateAccessVarTypes(
  const std::set<ReadWriteSets::AccessSetRecord>& inSet, VarType newVarType, AccessType newAccessType) {

  std::set<ReadWriteSets::AccessSetRecord> outSet;
  for(ReadWriteSets::AccessSetRecord record : inSet) {
    record.setVarType(newVarType);
    record.setAccessType(newAccessType);
    outSet.insert(record);
  }
  return outSet;
}



//! \brief nlohmann's default way to write to json, works with >> operator 
void ReadWriteSets::to_json(nlohmann::json& funcJson, const ReadWriteSets::FunctionReadWriteRecord& record) {
  funcJson = nlohmann::json{ 
    {"filename", record.filename},
    {"NodeId", record.nodeId },
    {"globality", globalityToString(record.globality)},
    {"varType", varTypeToString(record.varType)},
    {"accessType", accessTypeToString(record.accessType)},
    {"commandLine", record.commandLine}};
    
  //Output called functions (Name is only useful id?)
  std::vector<std::string> calledFunctionsVec;
  auto calledFuncIt = record.calledFunctions.begin();
  for(;calledFuncIt != record.calledFunctions.end(); ++calledFuncIt) {
    calledFunctionsVec.push_back(*calledFuncIt);
  }
  std::sort(calledFunctionsVec.begin(), calledFunctionsVec.end());
  funcJson["calledFunctions"] = calledFunctionsVec;
  
  
  std::vector<nlohmann::json> readSetJson;
  auto readSetIt = record.readSet.begin();
  for(;readSetIt != record.readSet.end(); ++readSetIt) {
    readSetJson.push_back(*readSetIt);
  }
  std::sort(readSetJson.begin(), readSetJson.end());

  std::vector<nlohmann::json> writeSetJson;
  auto writeSetIt = record.writeSet.begin();
  for(;writeSetIt != record.writeSet.end(); ++writeSetIt) {
    writeSetJson.push_back(*writeSetIt);
  }
  std::sort(writeSetJson.begin(), writeSetJson.end());

  funcJson["readSet"] = readSetJson;
  funcJson["writeSet"] = writeSetJson;

}

//! \brief nlohmann's default way to create AccessSetRecord from json, works with >> operator 
void ReadWriteSets::from_json(const nlohmann::json& funcJson, ReadWriteSets::FunctionReadWriteRecord& record) 
{
  funcJson.at("filename").get_to(record.filename);
  funcJson.at("NodeId").get_to(record.nodeId);
  
  std::string tmpGlobality;
  funcJson.at("globality").get_to(tmpGlobality);
  record.globality = globalityFromString(tmpGlobality);

  std::string tmpVarType;  
  funcJson.at("varType").get_to(tmpVarType);
  record.varType = varTypeFromString(tmpVarType);

  std::string tmpAccessType;  
  funcJson.at("accessType").get_to(tmpAccessType);
  record.accessType = accessTypeFromString(tmpAccessType);
  
  funcJson.at("commandLine").get_to(record.commandLine);

  // I turn the sets into vectors for easy output.  But for some reason
  // the insertion order matters.  So to make testing easier, I reverse
  // the order here so when run JsonTest the output file comes out the same
  // as the input file.  Weird. -Jim
  nlohmann::json calledFuncsArray(funcJson.at("calledFunctions"));
  for (auto calledFuncIt = calledFuncsArray.rbegin(); calledFuncIt !=  calledFuncsArray.rend(); ++calledFuncIt) {//auto& calledFunc : calledFuncsArray) {
    std::string funcStr(*calledFuncIt);
    record.calledFunctions.insert(funcStr);
  }
     
  nlohmann::json readSetArray(funcJson.at("readSet"));
  for (auto& accessRecordJson : readSetArray) {
    ReadWriteSets::AccessSetRecord accessRecord(accessRecordJson);
    record.readSet.insert(accessRecord);
  }
  nlohmann::json writeSetArray(funcJson.at("writeSet"));
  for (auto& accessRecordJson : writeSetArray) {
    ReadWriteSets::AccessSetRecord accessRecord(accessRecordJson);
    record.writeSet.insert(accessRecord);
  }
    
}  


//! \brief nlohmann's default way to write to json, works with >> operator 
void ReadWriteSets::to_json(nlohmann::json& recordJson, const ReadWriteSets::AccessSetRecord& record) {
  recordJson = nlohmann::json{
    {"name", record.getVariableName()},
    {"globality", globalityToString(record.getGlobality())},
    {"varType", varTypeToString(record.getVarType())},
    {"accessType", accessTypeToString(record.getAccessType())},
    {"NodeId", record.nodeId },
    {"type", record.type },
    {"filename", record.filename },
    {"note", record.noteStr} };
  
  std::vector<ReadWriteSets::AccessSetRecord> fieldRecords(record.fields.begin(), record.fields.end());
  recordJson["fields"] = fieldRecords;

/*  std::vector<ReadWriteSets::AccessSetRecord> indexRecords(record.indexes.begin(), record.indexes.end());
  recordJson["indexes"] = indexRecords;
*/
}

//! \brief nlohmann's default way to create AccessSetRecord from json, works with >> operator 
void ReadWriteSets::from_json(const nlohmann::json& recordJson, ReadWriteSets::AccessSetRecord& record) {
    
  recordJson.at("name").get_to(record.variableName);

  std::string tmpGlobality;
  recordJson.at("globality").get_to(tmpGlobality);
  record.globality = globalityFromString(tmpGlobality);

  std::string tmpAccessType;  
  recordJson.at("accessType").get_to(tmpAccessType);
  record.accessType = accessTypeFromString(tmpAccessType);

  std::string tmpVarType;  
  recordJson.at("varType").get_to(tmpVarType);
  record.varType = varTypeFromString(tmpVarType);

  recordJson.at("NodeId").get_to(record.nodeId);
    
  recordJson.at("type").get_to(record.type);
  recordJson.at("filename").get_to(record.filename);
  recordJson.at("note").get_to(record.noteStr);

  nlohmann::json fieldArray(recordJson.at("fields"));
  for (auto& field : fieldArray) {
    ReadWriteSets::AccessSetRecord fieldRecord(field);
    record.fields.insert(fieldRecord);
  }

/*  nlohmann::json indexArray(recordJson.at("indexes"));
  for (auto& index : indexArray) {
    ReadWriteSets::AccessSetRecord indexRecord(index);
    record.indexes.insert(indexRecord);
    }*/

}


/**
 * Walks through the cache inserting it all into a nlohmann json object.
 * It's possible that if I used a std::string to represent the function
 * instead of an SgFunctionDeclaration nlohmann could convert the whole
 * cache directly to json, but using an SgFunctionDeclaration is 
 * convienent when actually making the cache.
 * This just reads directly from the global cache, so no arguments needed
 *
 **/
nlohmann::json ReadWriteSets::convertCacheToJson(const std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& rwSetCache) 
{
  nlohmann::json jsonDocument;
  int count = 0;
  auto cacheIt = rwSetCache.begin();   //For each function
  for(;cacheIt != rwSetCache.end(); ++cacheIt) {
    ROSE_ASSERT(cacheIt->internalFunctionName != "");
    jsonDocument[cacheIt->internalFunctionName] = *cacheIt;
  }

  return jsonDocument;
}

/**
 * \brief Recreates the rwSetCache from a jsonDocument
 *
 * Used by TestabilityGrader.  WARNING, NodeIds may not be valid, check that FunctionReadWriteRecord.commandLine matches
 **/
void ReadWriteSets::readCacheFromJson(const nlohmann::json& jsonDocument, std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& rwSetCache) 
{
  for (auto& jsonElement : jsonDocument.items()) {
    ReadWriteSets::FunctionReadWriteRecord funcRecord(jsonElement.value());
    funcRecord.internalFunctionName = jsonElement.key();
    rwSetCache.insert(funcRecord);
  }
}



/**
 * This function attempts to insert a node into a read or writeset
 * or a message explaining why it can't. This can be quite a fraught 
 * process. 
 *
 * \param[in] coarseName: The node given as a read or write.  Not always 
 *                     sensible.
 *
 **/
/*std::string ReadWriteSets::generateAccessNameStrings(SgInitializedName* coarseName, ReadWriteSets::Globality globality) 
{
  switch(globality) {
  case PARAMETERS:
  case LOCALS:
    {
      
      SgNode* funcDefNode = VxUtilFuncs::getAncestorOfType(coarseName, V_SgFunctionDefinition);
      SgFunctionDeclaration* funcDecl = nullptr;
      if(funcDefNode != nullptr) {
      SgFunctionDefinition* funcDef = isSgFunctionDefinition(funcDefNode);
      funcDecl = funcDef->get_declaration();
      } else {
        SgNode* paramListNode = VxUtilFuncs::getAncestorOfType(coarseName, V_SgFunctionParameterList);
        if(paramListNode) {
          funcDecl = isSgFunctionDeclaration(paramListNode->get_parent());
        }
      }
      if(funcDecl == nullptr) {
        //Can't identify the globality, so don't know what else to do
        //but return and error
        return "!LOCALS type but couldn't find function! " + coarseName->get_qualified_name().getString();
      }
      SgStatement* coarseNameScope = coarseName->get_scope();
      SgType* coarseType = coarseName->get_type();
      if(isSgClassType(coarseType)) {
        return "@" + VxUtilFuncs::getExpandedFunctionName(funcDecl) + "@" + 
          coarseName->unparseToString();
      } else {
        return "@" + VxUtilFuncs::getExpandedFunctionName(funcDecl) + "@" + 
          coarseName->get_qualified_name().getString();        
      }
    }
    
  case GLOBALS:
    return "::"+coarseName->unparseToString();
    
  case FILE_SCOPE:
    return VxUtilFuncs::getNodeRelativePath(coarseName) + " " + coarseName->unparseToString();
  case MEMBERS:
  case OTHER_CLASS_MEMBERS:
  case STATIC_MEMBERS:
    {
      
      SgStatement* coarseNameScope = coarseName->get_scope();
      SgClassDefinition* clsDef = isSgClassDefinition(coarseNameScope);
      if(clsDef) {
        std::string clsName = clsDef->get_qualified_name();
      return "$" + clsName + "$" + coarseName->unparseToString();
      } else {
        return coarseName->get_qualified_name().getString();
      }
    }
    

  case GLOBALITY_UNKNOWN:
  default:
    return "!access unknown scope! " + coarseName->get_qualified_name().getString();
    
  }
  

}
*/
/**
 * meetsGlobalityConstraints
 *
 * tests a reference to see if it meets globality contraints
 *
 * param[in] funcDef : The function the reference came from, mostly for error messages
 * param[in] current : The reference to investigate usually an SgInitializedName, but 
 *                     may also be a function reference or thisExp
 * param[in] accessOrigin : The line where the access is actually
 *                          performed. Only used for noteStr  
 * param[out] noteStr  : If a violation is found, an expination his put here
 **/
ReadWriteSets::Globality ReadWriteSets::determineGlobality(SgFunctionDefinition* funcDef, SgNode* current, SgNode* accessOrigin, std::string& noteStr ) 
{

  // 1. The normal case is that current is an SgInitializedName, and
  // that's where the globality checking actually happens, so do
  // that first.
  SgInitializedName* coarseName = isSgInitializedName(current);
  if(coarseName) {
    SgStatement* coarseNameScope = coarseName->get_scope();
    

    //Is global scope?  I think variables with namespace scope are
    //ALWAYS global? (And if we're here it's a variable)
    if(isSgGlobal(coarseNameScope) || isSgNamespaceDefinitionStatement(coarseNameScope)) {
      //File Scope is always within global scope
      if(VxUtilFuncs::isFileScope(coarseName->get_declaration())) {
        return FILE_SCOPE;
      }

      noteStr = VxUtilFuncs::makeNoteString("global reference found", accessOrigin, accessOrigin->unparseToString());
      return GLOBALS;   

    } else if(isSgClassDefinition(coarseNameScope)) {
      SgClassDefinition* coarseNameClass = isSgClassDefinition(coarseNameScope);
      if(coarseName->get_declaration()->get_declarationModifier().get_storageModifier().isStatic()) {
          return STATIC_MEMBERS;
      }
      SgMemberFunctionDeclaration* memberFuncDecl = isSgMemberFunctionDeclaration(funcDef->get_declaration());
      if(memberFuncDecl == NULL) {
        noteStr = VxUtilFuncs::makeNoteString("reference found to class member in non-member function", 
                                 accessOrigin,   
                                 funcDef->get_declaration()->get_qualified_name().getString());
        return OTHER_CLASS_MEMBERS;  //Accessed from nonmember function
      }
      SgClassDefinition* functionClass = 
        isSgClassDefinition(memberFuncDecl->get_class_scope());
      //This is a bit dodgy, A: is testing pointere equality
      //sufficent?  It should be with merge, but...
      // B. This doesn't test inheritence, but that's a decision to be made by someone else.
      if(functionClass && functionClass == coarseNameClass)
        {
          return MEMBERS;
        } else {
        noteStr = VxUtilFuncs::makeNoteString("reference found to different class", 
                                 accessOrigin, coarseNameClass->get_declaration()->get_type()->unparseToString());
        return OTHER_CLASS_MEMBERS; //Accessed from a different class, so it's a global access
      }
            
    } else {  //Otherwise, check that it's a local variable or an argument
      //A little annoying, any parameter arguments will be associated with the declaration attached to
      //the definition, not the first non-defining declaration.  So get the parameter list that way
      bool isDefAncestor = SageInterface::isAncestor(funcDef, coarseName);    //Local variable
      if(isDefAncestor) {
          return LOCALS;
      }
      SgFunctionDeclaration* funcDecl =funcDef->get_declaration();
      bool isDeclAncestor = SageInterface::isAncestor(funcDecl, coarseName);  //Writing function arguments is OK.
      if(isDeclAncestor) {
          return PARAMETERS;
      }

      noteStr = VxUtilFuncs::makeNoteString("write to non-local variable, give leek2 this example", accessOrigin, accessOrigin->unparseToString());
      return GLOBALITY_UNKNOWN;
      
    }
        
  }
    
    
  //2. It's something other than an SgInitailizedName, figure out what it is a deal with it.
  //The below error messages might not make sense in this new usage, where we're expecting to see a log of 
  //undefined functions.  We'll see.  
  if (isSgFunctionRefExp(current)) {
    SgFunctionRefExp* funcRef = isSgFunctionRefExp(current);
    SgFunctionDeclaration* funcDecl = funcRef->getAssociatedFunctionDeclaration();
    if(funcDecl) {
      Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "Got function declaration: " << funcDecl->get_qualified_name().getString() << std::endl;
      return GLOBALITY_UNKNOWN;  
    } else {
      Sawyer::Message::mlog[Sawyer::Message::Common::WARN] << VxUtilFuncs::makeNoteString("Got SgFunctionRef but couldn't resolve it", accessOrigin, accessOrigin->unparseToString()) << std::endl;
      return GLOBALITY_UNKNOWN;
    }
  }
  if (isSgMemberFunctionRefExp(current)) {
    SgMemberFunctionRefExp* funcRef = isSgMemberFunctionRefExp(current);
    SgMemberFunctionDeclaration* funcDecl = funcRef->getAssociatedMemberFunctionDeclaration();
    if(funcDecl) {
      Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "Got Member function declaration: " << funcDecl->get_qualified_name().getString() << std::endl;
      return GLOBALITY_UNKNOWN;    
    } else {
      Sawyer::Message::mlog[Sawyer::Message::Common::WARN] << VxUtilFuncs::makeNoteString("Got SgMemberFunctionRef but couldn't resolve it", accessOrigin, accessOrigin->unparseToString()) << std::endl;
      return GLOBALITY_UNKNOWN;
    }
  }
  if (isSgTemplateFunctionRefExp(current) || isSgTemplateMemberFunctionRefExp(current)) {
    //I used to have a case to handle this, but it turns out that
    //TemplateFunctionRefExp only happen when an uninstantiated
    //template calls a templated function.  Which should never been
    //seen in the RWSets.  So this is left here as a safetly check.
    Sawyer::Message::mlog[Sawyer::Message::Common::ERROR] << "We got an SgTemplateMemberFunctionRefExp, which shouldn't be possible.  Contact ROSE team."  << std::endl;
    ROSE_ABORT();
  }
  if (isSgConstructorInitializer(current)) {
    //This just tells us that an object is being constructed.  It
    //doesn't really tell us anything about the globality of that
    //object. So call it LOCALS, and let it get upgraded later if necessary
    return LOCALS;  
  }
  if (isSgThisExp(current)) {
    noteStr = VxUtilFuncs::makeNoteString("\'this\' reference found", accessOrigin, accessOrigin->unparseToString());
    return MEMBERS;
  }
    
  Sawyer::Message::mlog[Sawyer::Message::Common::ERROR] <<
    "meetsGlobalityConstraints: " <<
    current->get_file_info()->get_filename() << ":" <<
    current->get_file_info()->get_line() << "-" << current->get_file_info()->get_col()<<std::endl;
  Sawyer::Message::mlog[Sawyer::Message::Common::ERROR] <<
    "In meetsGlobalityConstraints: unhandled reference type:"  <<
    current->class_name()<<std::endl;
  ROSE_ABORT();
}
