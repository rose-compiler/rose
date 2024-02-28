#include <iostream>
#include <fstream>
#include <ReadWriteSetRecords.h>
#include <TestabilityGrader.h>

#include <nlohmann/json.hpp>
#include <Yaml.hpp>
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
using namespace nlohmann;


Sawyer::Message::Common::Facility mlog;
using namespace Sawyer::CommandLine;
using namespace Sawyer::Message;

//---------------------------------------------------------------------------------
// Settings that can be adjusted from the command line and their default values.
// Had to use ints because Sawyer doesn't seem to know what to with enums
struct Settings {
  std::string outputFilename      = "TestGraded.json";

  Settings(): outputFilename("TestGraded.json")
  {
  }
};


int main(int argc, char** argv) {

  //----------------------------------------------------------
  // Parse Command line args
  //Diagnostics::initAndRegister(&mlog, "TestabilityGrader");

  Settings settings;
  Parser p;
  p.purpose(purpose)
    .doc("description", description)
    .doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis
  p.errorStream(Sawyer::Message::mlog[Sawyer::Message::FATAL]);                         // print messages and exit rather than throwing exceptions

  // Create a group of switches specific to this tool
  SwitchGroup testabilityGraderSwitches("-specific switches");
  testabilityGraderSwitches.name("testabilityGrader");                                  // the optional switch prefix, i.e., "--tool:whatever"

  testabilityGraderSwitches.insert(Switch("output-filename", 'o')
                                   .argument("filename", anyParser(settings.outputFilename))
                                   .doc("Filename to output the merged json file to. \"TestGraded.json\""));

  // Parse the command-line and get the non-switch, positional arguments at the end
  std::vector<std::string> files = p.with(testabilityGraderSwitches).parse(argc, argv).apply().unreachedArgs();

  TestabilityGrader testabilityGrader;
  
  if(files.size() < 1) {
    std::cout << "Please provide at least one input json file" << std::endl;
  }

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

    //Merge into main cache
    testabilityGrader.mergeFileIntoCache(tmpCache);
  }
  
  for(const auto& funcRecord : testabilityGrader.startCache) {
    ReadWriteSets::Globality maxGlobality = ReadWriteSets::LOCALS;
    ReadWriteSets::VarType maxVarType = ReadWriteSets::PRIMITIVES;
    ReadWriteSets::AccessType maxAccessType = ReadWriteSets::NORMAL;
    std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash> workingSet;
    testabilityGrader.recursivelyEvaluateFunctionRecords(funcRecord, maxGlobality, maxVarType, maxAccessType, workingSet);
  }

  
     
  nlohmann::json outDocument = convertCacheToJson(testabilityGrader.completedCache); 
  std::ofstream outFile;
  outFile.open(settings.outputFilename, std::ofstream::out);
  outFile << std::setw(4) << outDocument << std::endl;


  return 0;
}
