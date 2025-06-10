#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>
#include <sage3basic.h>
#include <Rose/CommandLine.h>
#include <ReadWriteSetRecords.h>
#include <TestabilityGrader.h>
#include <DOTRepresentation.h>

#include <nlohmann/json.hpp>
#include <Sawyer/Yaml.h>
#include <Sawyer/Sawyer.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>


static const char *purpose = "Make a JSON file giving the (non-recursive) read/write sets for each function.";
static const char *description =
  "LocalRWSetGenerator:\n\n"
  ""
  "The purpose of LocalRWSetGenerator is give the read/write set for every function.\n"
  "However, in this version, called functions are not decended into.  So it's only\n"
  "the read/write set of that function, not the functions it calls.  This makes\n"
  "the analysis local, but means that a post-processing step is required to get \n"
  "full read/write sets.\n"
  ;


#define TOOL_VERSION_STRING "0.1.0"
using namespace Rose;
using namespace SageInterface;
using namespace Sawyer::CommandLine;
using namespace Sawyer::Message;
using namespace nlohmann;

//---------------------------------------------------------------------------------
// Settings that can be adjusted from the command line and their default values.
// Had to use ints because Sawyer doesn't seem to know what to with enums
struct Settings {
  std::string outputFilename;
  std::string statsFilename;
  bool force_stdout = false;

  Settings(): outputFilename(""),
              statsFilename("stats.out"),
              force_stdout(false)
  {
  }
};

// Hash function 
struct hashFunction
{
  size_t operator()(const std::pair<std::string, std::string> &x) const
  {
    std::hash<std::string> string_hash;
    return string_hash(x.first);
  }
};

// equal_to function 
struct equalToFunction
{
  bool operator()(const std::pair<std::string, std::string> &x,
                  const std::pair<std::string, std::string> &y) const
  {
    if(x.first == y.first) {
      return true;
    }
    return false;
  }
};


void countFunction(const ReadWriteSets::FunctionReadWriteRecord& thisFuncRecord, std::vector<int>& functionCount) {
  if(thisFuncRecord.varType == ReadWriteSets::VarType::VARTYPE_UNKNOWN ||
     thisFuncRecord.accessType == ReadWriteSets::AccessType::ACCESSTYPE_UNKNOWN ||
     thisFuncRecord.globality == ReadWriteSets::Globality::GLOBALITY_UNKNOWN) {
    functionCount[12] += 1;
    //Count function pointers seperately because I'm not sure about testability
  } else if(thisFuncRecord.accessType == ReadWriteSets::AccessType::FUNCTION_POINTER_DEREF) {
    functionCount[11] += 1;
  } else {
    functionCount[(int)thisFuncRecord.varType] += 1;
  }
}

void writeStatFileSection(std::ofstream& statsFile, std::string header, std::vector<int>& localFunctionCount, std::vector<int>& functionCount, int localPtrCount) {
  int totalCount = 0;
  for(int ii : localFunctionCount) {
    totalCount +=  ii;
  }
  
  statsFile << header << " : Functions: " << totalCount << ", local pointer funcs : " << localPtrCount << ", " << ((double)localPtrCount/(double)totalCount) << "%" <<std::endl;
  statsFile << " PRIME   ARRAY  INDEX  CLASS  A_CLS   FUNC M_FUNC  STD_P  C_DATA  PTRS   VOID  F_PTR   UNKNOWN" << std::endl;
  
  //Functions typed without the functions they call
  for(int ii : localFunctionCount) {
    statsFile << std::setw(7) <<  ii;
  }
  statsFile << std::endl;

  //Function types, including called functions
  for(int ii : functionCount) {
    statsFile << std::setw(7) <<  ii;
  }
  statsFile << std::endl;

  //I don't think the count up turned out to be that useful
  //  int countup = 0;
  //for(int ii : functionCount) {
  //  countup += ii;
  //  statsFile << std::setw(6) << countup;
  // }
  //statsFile << std::endl;
}



bool recursivelyFindLocalPtrs(const std::set<ReadWriteSets::AccessSetRecord>& RWSet) {
  for(const ReadWriteSets::AccessSetRecord& thisAccessRecord : RWSet) {
    if(thisAccessRecord.varType >= ReadWriteSets::VarType::POINTERS && thisAccessRecord.globality == ReadWriteSets::Globality::LOCALS) {
      return true;
    }
    if(recursivelyFindLocalPtrs(thisAccessRecord.fields)) {
      return true;
    }
  }
  return false;
}

/** We know that local pointers are a problem for Qing's analysis. 
 *  This function searchs a functionReadWriteRecord for a local pointer, and returns true if it finds one.
 *  Parameters: 
 *  \param[in] thisFuncRecord : The function record to work on
 **/
bool countLocalPtrs(const ReadWriteSets::FunctionReadWriteRecord& thisFuncRecord) {
  if(recursivelyFindLocalPtrs(thisFuncRecord.readSet)) {
    return true;
  }
  if(recursivelyFindLocalPtrs(thisFuncRecord.writeSet)) {
    return true;
  }
  return false;
}


void recursivelyFindType(const std::set<ReadWriteSets::AccessSetRecord>& RWSet, const ReadWriteSets::VarType worstType, std::unordered_set<std::string>& types) {
  for(const ReadWriteSets::AccessSetRecord& thisAccessRecord : RWSet) {
    if(thisAccessRecord.varType == worstType && thisAccessRecord.globality > ReadWriteSets::Globality::PARAMETERS) {
      types.insert(thisAccessRecord.type);
    }
    recursivelyFindType(thisAccessRecord.fields, worstType, types);
  }
}

/** This function tries to sort out which types are causing trouble.  
 *   So, a given function has a worst VAR_TYPE.  Find all the types in the recursive RWSet that
 *   have that particular VAR_TYPE.  So, if a given function VAR_TYPE is void*, find all the
 *   types that cause VOID*, and put them in the otherTypeCount vector.
 *
 *  Parameters: 
 *  \param[in] thisFuncRecord : The function record to work on
 *  \param[inout] otherTypeCount: Each vector entry is a VAR_TYPE containing a map from a type name to a number of times that type has been seen.
 This acumulates across the entire program, so we can see the problem type for each VAR_TYPE, and how many times it
 appears in the program.  (But not really where it appears.)
**/
void countTypes(const ReadWriteSets::FunctionReadWriteRecord& thisFuncRecord, std::vector<std::unordered_map<std::string, int> >& otherTypeCount) {
  ReadWriteSets::VarType worstType = thisFuncRecord.varType;
  //Find all variables of this varType in RWSet, record one access for each one
  std::unordered_set<std::string> types;
  recursivelyFindType(thisFuncRecord.readSet, worstType, types);
  for(const std::string& thisType : types) {
    otherTypeCount[worstType][thisType]++;
  }
}

void writeTypesFile(std::ofstream& typesFile, std::vector<std::unordered_map<std::string, int> >& typeCount) {
  for(int ii = 0; ii < typeCount.size(); ++ii) {
    typesFile << " -------------------" << ReadWriteSets::varTypeToString((ReadWriteSets::VarType)ii) << "------------------------------- " << std::endl;
    for (const auto& pair : typeCount[ii]) {
      typesFile << pair.first << " : " << pair.second << std::endl;
    }
    typesFile << std::endl;
  }
}

/** Writes out the ?????List to a file.  e.g. each getter function by name with its varType,
 *  Pipe | is used as a column splitter
 **/
void writeListFile(std::string filename, const std::unordered_set< std::pair<std::string, std::string>,  hashFunction, equalToFunction> funcList) {

  std::ofstream listFile;
  listFile.open(filename, std::ofstream::out);
  for (const auto& pair : funcList) {
    listFile << pair.first << " | " << pair.second << std::endl;
  }
  
  listFile.close();
}

/** Returns a count of the number of local functions fit the particular type
    (e.g. getters, setters, others)
    Based on appearance in both the localPtrFuncs and the type list
**/
int countLocalPtrFunctionType(std::unordered_set< std::string > localPtrFuncs,
                          const std::unordered_set< std::pair<std::string, std::string>,  hashFunction, equalToFunction> funcList) {
  int count = 0;
  for(auto& localPtrFunc : localPtrFuncs) {
    std::string fakeVal = "fake";
    std::pair<std::string, std::string> lookupPair = make_pair(localPtrFunc, fakeVal);
    if(funcList.find(lookupPair) != funcList.end()){
      count++;
    }
  }
  return count;
}

/**
 * The Group G code has a lot of very simple "getter" and "setter" functions.  That probably
 * aren't really worth testing.  So, I decided to sort these out so we can determine how
 * difficult it is to test the "interesting" functions.  
 **/
void sortGettersAndSetters(const std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& cache,
                           std::vector<int>& getterCount,
                           std::vector<int>& setterCount,
                           std::vector<int>& otherCount,
                           std::vector<std::unordered_map<std::string, int> >& gettersTypeCount,
                           std::vector<std::unordered_map<std::string, int> >& settersTypeCount,
                           std::vector<std::unordered_map<std::string, int> >& othersTypeCount,
                           std::unordered_set< std::pair<std::string, std::string>, hashFunction, equalToFunction >& gettersList,
                           std::unordered_set< std::pair<std::string, std::string>, hashFunction, equalToFunction >& settersList,
                           std::unordered_set< std::pair<std::string, std::string>, hashFunction, equalToFunction >& othersList) {

  for(const ReadWriteSets::FunctionReadWriteRecord& thisFuncRecord : cache) {
      
      //Check for getters and setters
      //thisFuncRecord.internalFunctionName.find("::get") != std::string::npos &&
      if(thisFuncRecord.readSet.size() == 1 &&
         thisFuncRecord.writeSet.size() == 0) {
        countFunction(thisFuncRecord, getterCount);
        countTypes(thisFuncRecord, gettersTypeCount);
        gettersList.insert(std::make_pair(thisFuncRecord.internalFunctionName, ReadWriteSets::varTypeToString(thisFuncRecord.varType)));
      } else if(thisFuncRecord.readSet.size() <= 1 &&
                thisFuncRecord.writeSet.size() == 1) {
        //thisFuncRecord.internalFunctionName.find("::set") != std::string::npos
        countFunction(thisFuncRecord, setterCount);
        countTypes(thisFuncRecord, settersTypeCount);
        settersList.insert(std::make_pair(thisFuncRecord.internalFunctionName, ReadWriteSets::varTypeToString(thisFuncRecord.varType)));

      } else {
        countFunction(thisFuncRecord, otherCount);
        countTypes(thisFuncRecord, othersTypeCount);
        othersList.insert(std::make_pair(thisFuncRecord.internalFunctionName, ReadWriteSets::varTypeToString(thisFuncRecord.varType)));
      }
    }
  }


int main(int argc, char** argv) {

  //----------------------------------------------------------
  // Parse Command line args
  ROSE_INITIALIZE;
  VxUtilFuncs::initDiagnostics();

  Settings settings;
  using namespace Sawyer::CommandLine;
  Parser p;
  p.purpose(purpose)
    .doc("description", description)
    .doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis
  p.with(Rose::CommandLine::genericSwitches());   // things like --help, --version, --log, --threads, etc.
  p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis;

  // Create a group of switches specific to this tool
  SwitchGroup testabilityGraderSwitches("-specific switches");
  testabilityGraderSwitches.name("testabilityGrader");                                  // the optional switch prefix, i.e., "--tool:whatever"

  testabilityGraderSwitches.insert(Switch("output-filename", 'o')
                                   .argument("filename", anyParser(settings.outputFilename))
                                   .doc("Filename to output the merged json file to.  If not present, output is written to stdout"));
  testabilityGraderSwitches.insert(Switch("stats-filename", 's')
                                   .argument("filename", anyParser(settings.statsFilename))
                                   .doc("Filename to output the statistics to.  The default is \"stats.out\""));
  testabilityGraderSwitches.insert(Switch("force-stdout")
                                   .intrinsicValue(true, settings.force_stdout)
                                   .doc("Forces the output to be written to stdout regardless of whether -o is used."));
  // Parse the command-line and get the non-switch, positional arguments at the end
  std::vector<std::string> files = p.with(testabilityGraderSwitches).parse(argc, argv).apply().unreachedArgs();

  std::set<std::string> ignoreFunctions = {"::printf(const char *,...)",
                                           "::sprintf(char *,const char *,...)",
                                           "::fprintf(FILE *,const char *,...)",
                                           "::printf_log(int,const char *,...)",
                                           "::sscanf(const char *,const char *,...)"};
                                          
  
  TestabilityGrader testabilityGrader;
  
  if(files.size() < 1) {
    std::cout << "Please provide at least one input json file" << std::endl;
  }

  //We want to know which functions have local Ptr usage inside them, so we count these up when we open each local RW set file.
  std::unordered_set<std::string> localPtrFuncs;  
  //  int localPtrFunctionCount = 0;

  //Count up the VarType of a function before it's merged with the functions it calls.
  std::vector<int> localGetterCount = {0,0,0,0,0,0,0,0,0,0,0,0,0 };
  std::vector<int> localSetterCount = {0,0,0,0,0,0,0,0,0,0,0,0,0 };
  std::vector<int> localOtherCount = {0,0,0,0,0,0,0,0,0,0,0,0,0 };

  //Count of each actual type seen in each varType bin.  So, the vector part is the varType, then you a count of
  //each type as a string name.  (This set is for the local files)
  std::vector<std::unordered_map<std::string, int> > localGettersTypeCount(13);
  std::vector<std::unordered_map<std::string, int> > localSettersTypeCount(13);
  std::vector<std::unordered_map<std::string, int> > localOthersTypeCount(13);

  //File that lists each function just with it's varType, AS FOUND LOCALLY, divided by getters, setters, and others.
  //This info isn't currently availible anywhere else.
  std::unordered_set<std::pair<std::string, std::string>, hashFunction, equalToFunction > localGettersList;
  std::unordered_set<std::pair<std::string, std::string>, hashFunction, equalToFunction > localSettersList;
  std::unordered_set<std::pair<std::string, std::string>, hashFunction, equalToFunction > localOthersList;

  //Files listing a count of types seen in each varType bin, as seen from local functions.
  //May not really be useful beyond the global types, but it falls out of the analysis anyway, so what the hey.
  std::ofstream localGettersTypesFile;
  std::ofstream localSettersTypesFile;
  std::ofstream localOthersTypesFile;
  localGettersTypesFile.open("localGettersTypesFile.txt", std::ofstream::out);
  localSettersTypesFile.open("localSettersTypesFile.txt", std::ofstream::out);
  localOthersTypesFile.open("localOthersTypesFile.txt", std::ofstream::out);
  
  //Count of functions with particular varTypes.  
  std::vector<int> getterCount = {0,0,0,0,0,0,0,0,0,0,0,0,0 };
  std::vector<int> setterCount = {0,0,0,0,0,0,0,0,0,0,0,0,0 };
  std::vector<int> otherCount = {0,0,0,0,0,0,0,0,0,0,0,0,0 };

  //Count of each actual type seen in each varType bin.  So, the vector part is the varType, then you a count of
  //each type as a string name.
  std::vector<std::unordered_map<std::string, int> > gettersTypeCount(13);
  std::vector<std::unordered_map<std::string, int> > settersTypeCount(13);
  std::vector<std::unordered_map<std::string, int> > othersTypeCount(13);

  //File that lists each function just with it's varType, AS FOUND LOCALLY, divided by getters, setters, and others.
  //This info isn't currently availible anywhere else.
  std::unordered_set<std::pair<std::string, std::string>, hashFunction, equalToFunction > gettersList;
  std::unordered_set<std::pair<std::string, std::string>, hashFunction, equalToFunction > settersList;
  std::unordered_set<std::pair<std::string, std::string>, hashFunction, equalToFunction > othersList;

  //File that lists each function just with it's varType, divided by getters, setters, and others.
  //This is just to make that basic information easier to find, since it's hard to find in the json file.
  //(Could be done with postprocessing.)
  std::ofstream gettersListFile;
  std::ofstream settersListFile;
  std::ofstream othersListFile;
  gettersListFile.open("gettersListFile.txt", std::ofstream::out);
  settersListFile.open("settersListFile.txt", std::ofstream::out);
  othersListFile.open("othersListFile.txt", std::ofstream::out);

  //Files listing a count of types seen in each varType bin.  It can be useful for figuring out how
  //often a particular type occurs, and getting an idea of type diversity.
  std::ofstream gettersTypesFile;
  std::ofstream settersTypesFile;
  std::ofstream othersTypesFile;
  gettersTypesFile.open("gettersTypesFile.txt", std::ofstream::out);
  settersTypesFile.open("settersTypesFile.txt", std::ofstream::out);
  othersTypesFile.open("othersTypesFile.txt", std::ofstream::out);

  // Record of functions already seen so we can skip them
  std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash> localRWCache;

  for(auto& file : files) {
    std::ifstream inFile;
    inFile.open(file, std::ifstream::in);
    char c;
    inFile >> c;  //flag eof if at end of file, doesn't seem to throw exception
    if(inFile.eof()) {  //nlohmann doesn't like empty files
      inFile.close();
      continue;
    }
    inFile.seekg(0, std::ios_base::beg);
    
    nlohmann::json jsonDocument;
    inFile >> jsonDocument;
    inFile.close();
    std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash> tmpCache;
    
    ReadWriteSets::readCacheFromJson(jsonDocument, tmpCache);

    for(const ReadWriteSets::FunctionReadWriteRecord& thisFuncRecord : tmpCache) {
      //Find any functions in the file that use a local pointer.  This is a stat that's useful for Lindsay
      if(countLocalPtrs(thisFuncRecord)) {
        localPtrFuncs.insert(thisFuncRecord.internalFunctionName);
      }

      ReadWriteSets::Globality maxGlobality = ReadWriteSets::LOCALS;
      ReadWriteSets::VarType maxVarType = ReadWriteSets::PRIMITIVES;
      ReadWriteSets::AccessType maxAccessType = ReadWriteSets::NORMAL;
      ReadWriteSets::FunctionReadWriteRecord thisFuncRecord2(thisFuncRecord);
      ReadWriteSets::maxTestability(thisFuncRecord2, maxGlobality, maxVarType, maxAccessType);
      thisFuncRecord2.globality = maxGlobality;
      thisFuncRecord2.varType = maxVarType;
      thisFuncRecord2.accessType = maxAccessType;
      localRWCache.insert(thisFuncRecord2);
    }
    //Merge into main cache
    testabilityGrader.mergeFileIntoCache(tmpCache);

  }
  //Sort the functions into getter, setters, and others so we get types for each function without looking at who it calls.
  sortGettersAndSetters(localRWCache, localGetterCount,  localSetterCount, localOtherCount,
                       localGettersTypeCount, localSettersTypeCount, localOthersTypeCount,
                       localGettersList, localSettersList, localOthersList);

  writeListFile("localGettersListFile.txt", localGettersList);
  writeListFile("localSettersListFile.txt", localSettersList);
  writeListFile("localOthersListFile.txt", localOthersList);

  // Now that we have every function in the database, trace down all the called functions and
  // assign the worst accesstypes to the caller functions.
  for(const auto& funcRecord : testabilityGrader.startCache) {
    ReadWriteSets::Globality maxGlobality = ReadWriteSets::LOCALS;
    ReadWriteSets::VarType maxVarType = ReadWriteSets::PRIMITIVES;
    ReadWriteSets::AccessType maxAccessType = ReadWriteSets::NORMAL;
    std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash> workingSet;
    testabilityGrader.recursivelyEvaluateFunctionRecords(funcRecord, maxGlobality, maxVarType, maxAccessType, ignoreFunctions, localPtrFuncs, workingSet);
  }

  //Write out the main json output file
  nlohmann::json outDocument = convertCacheToJson(testabilityGrader.completedCache); 

  if (settings.outputFilename == "" || settings.force_stdout) {
    std::cout << std::setw(4) << outDocument << std::endl;
  }
  if (settings.outputFilename != "") {
    std::ofstream outFile;
    outFile.open(settings.outputFilename, std::ofstream::out);
    outFile << std::setw(4) << outDocument << std::endl;
    outFile.close();
  }

  //Now output the statistics
  std::ofstream statsFile;
  statsFile.open(settings.statsFilename, std::ofstream::out);

  statsFile << "\nTotal number of Functions: " << testabilityGrader.completedCache.size() << std::endl;
  //FunctionCount has 13 Entries: All varTypes 0-10, + Functions that use function pointers + Unknowns

  //Sort the functions into getter, setters, and others so we get types for each function including who they call
  sortGettersAndSetters(testabilityGrader.completedCache, getterCount,  setterCount, otherCount,
                       gettersTypeCount, settersTypeCount, othersTypeCount, gettersList, settersList, othersList);
  writeListFile("gettersListFile.txt", gettersList);
  writeListFile("settersListFile.txt", settersList);
  writeListFile("othersListFile.txt", othersList);
  
  //! The DOT representation of the limited call graph
  DOTRepresentation<std::string> dotrep;
  std::unordered_set<std::string> seenNodes; 

  for(const ReadWriteSets::FunctionReadWriteRecord& thisFuncRecord : testabilityGrader.completedCache) {
    seenNodes.insert(thisFuncRecord.internalFunctionName);
  }
  for(const std::string& node : seenNodes) {
    dotrep.addNode( node,node);
  }
  for(const ReadWriteSets::FunctionReadWriteRecord& thisFuncRecord : testabilityGrader.completedCache) {    
    for(const std::string& calledFunc : thisFuncRecord.calledFunctions) {
      dotrep.addEdge( thisFuncRecord.internalFunctionName, calledFunc);
    }
  }
  dotrep.writeToFileAsGraph("callGraph.dot");


  //Now write out all the files that list particular kinds of functions.
  writeTypesFile(gettersTypesFile, gettersTypeCount);
  writeTypesFile(settersTypesFile, settersTypeCount);
  writeTypesFile(othersTypesFile, othersTypeCount);
  gettersTypesFile.close();
  settersTypesFile.close();
  othersTypesFile.close();

  writeTypesFile(localGettersTypesFile, localGettersTypeCount);
  writeTypesFile(localSettersTypesFile, localSettersTypeCount);
  writeTypesFile(localOthersTypesFile, localOthersTypeCount);
  localGettersTypesFile.close();
  localSettersTypesFile.close();
  localOthersTypesFile.close();

  writeStatFileSection(statsFile, "getters", localGetterCount, getterCount, countLocalPtrFunctionType(localPtrFuncs, gettersList));
  writeStatFileSection(statsFile, "setters", localSetterCount, setterCount, countLocalPtrFunctionType(localPtrFuncs, settersList));
  writeStatFileSection(statsFile, "others", localOtherCount, otherCount, countLocalPtrFunctionType(localPtrFuncs, othersList));  
  statsFile.close();
  
  
  std::ofstream localPtrsFile;
  localPtrsFile.open("LocalPtrsFile.txt", std::ofstream::out);
  for(const std::string& funcName : localPtrFuncs) {
    localPtrsFile << funcName << std::endl;
  }
  localPtrsFile.close();
  
  return 0;
}
