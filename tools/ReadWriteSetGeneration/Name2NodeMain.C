#include <sage3basic.h>
#include <sageInterface.h>
#include <Rose/CommandLine.h>
#include <nlohmann/json.hpp>
#include <Rose/Yaml.h>
#include <Name2Node.h>
#include <ReadWriteSetRecords.h>
#include "Name2Node.h"
#include "ReadWriteSetRecords.h"
#include <sstream>
#include <unordered_set>
// 03/20/2023): Adding boost filesystem usage:
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>


static const char *purpose = "Demonstrate the ability to take names from a RWSet JSON file and lookup the nodes.";
static const char *description =
    "Name2NodeMain:\n\n"
    ""
    "The purpose of Name2NodeMain is to demonstrate that names given for functions\n"
    "and variables in the RWSet JSON file can be turned into Sage Nodes with the \n"
    "Name2Node class.\n"
    "So it takes at least on JSON file to read, and tried to locate the nodes \n"
    "contained in it.\n"
    ;


#define TOOL_VERSION_STRING "0.1.0"

using namespace Rose;
using namespace SageInterface;
using namespace Sawyer::Message::Common;
using namespace nlohmann;

    

//---------------------------------------------------------------------------------
// Settings that can be adjusted from the command line and their default values.
// Had to use ints because Sawyer doesn't seem to know what to with enums
struct Settings {
    std::string yamlConfigFilename      = "";
    std::string rwSetsFilename      = "";
    //Optional, if no output given, output goes to terminal
    std::string outputFilename      = "";
    std::string applicationRootDir  = "";
    bool force_stdout = false;
  
  
    Settings(): yamlConfigFilename(""), 
                rwSetsFilename(""),
                outputFilename(""),
                applicationRootDir(""),
                force_stdout(false)
        {
        }
};

/**
 * readConfigFile
 *
 * \brief Fills in settings from a configuration file if provided
 *
 * Reads a yaml configuration file, if one was provided, and fills in
 * the settings.  
 *
 **/
void readConfigFile(Settings& settings) 
{
    if(settings.yamlConfigFilename != "" ) {
        Yaml::Node config;
        try
            {
                Sawyer::Yaml::Parse(config, settings.yamlConfigFilename);
            }
        catch (const Yaml::Exception& e)
            {
                Sawyer::Message::mlog[ERROR] << "Exception " << e.Type() << ": " << e.what() << " configuration file not found: " << settings.yamlConfigFilename << std::endl;
                exit(12);
            }

        settings.outputFilename = config["output-filename"].As<std::string>(settings.outputFilename);
        settings.rwSetsFilename = config["rwSets-filename"].As<std::string>(settings.rwSetsFilename);
        settings.applicationRootDir = config["app-root"].As<std::string>(settings.applicationRootDir);
        settings.force_stdout = config["force-stdout"].As<bool>(settings.force_stdout);
    }
}


int
main(int argc, char *argv[]) {
    // Initialization
    ROSE_INITIALIZE;
    Rose::CommandLine::versionString =
        TOOL_VERSION_STRING " using " +
        Rose::CommandLine::versionString;
    Rose::Diagnostics::initialize(); 

    //~  CodeThorn::initDiagnostics();
    //----------------------------------------------------------
    // Parse Command line args

    Settings settings;
    using namespace Sawyer::CommandLine;
    Parser p = CommandLine::createEmptyParserStage(purpose, description);
    p.with(CommandLine::genericSwitches());   // things like --help, --version, --log, --threads, etc.
    p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis

    // Create a group of switches specific to this tool
    SwitchGroup name2NodeSwitches("Name2Node-specific switches");
    name2NodeSwitches.name("Name2Node");                        // the optional switch prefix, i.e., "--tool:whatever"

    name2NodeSwitches.insert(Switch("config-filename")
                             .argument("filename", anyParser(settings.yamlConfigFilename))
                             .doc("Filename of yaml configuration file, used as alternate to command line arguments"));
    name2NodeSwitches.insert(Switch("rwSets-filename")
                             .argument("filename", anyParser(settings.rwSetsFilename))
                             .doc("Filename to output the list of read/write sets to. Required"));
    name2NodeSwitches.insert(Switch("output-filename")
                             .argument("filename", anyParser(settings.outputFilename))
                             .doc("Filename to output the list of read/write sets to.  Defaults to terminal"));
    name2NodeSwitches.insert(Switch("app-root")
                             .argument("filename", anyParser(settings.applicationRootDir))
                             .doc("The root of the application we're processing.  Used to filter external functions.  Duplicate of the rose option -rose:applicationRootDirectory"));
    name2NodeSwitches.insert(Switch("force-stdout")
                             .intrinsicValue(true, settings.force_stdout)
                             .doc("Forces the output to be written to stdout regardless of whether --filename is used."));
    // Parse the command-line and get the non-switch, positional arguments at the end
    std::vector<std::string> files = p.with(name2NodeSwitches).parse(argc, argv).apply().unreachedArgs();
    
    // See if the yaml configuration exists and is useful.
    readConfigFile(settings);

    if(settings.rwSetsFilename == "") {
        Sawyer::Message::mlog[ERROR] << "Please provide a non-empty RWSets JSON file" <<    std::endl;
        exit(12);
    }

    //----------------------------------------------------------
    //Command line processing done, now do ROSE work
    SgProject *root = frontend(argc, argv);

    if(settings.applicationRootDir != "") {
        root->set_applicationRootDirectory(settings.applicationRootDir);
    }

    //Calculate up the cache of names->nodes
    Name2Node name2Node(root);

    //Open up the RWSets Json file
    std::ifstream inFile;
    inFile.open(settings.rwSetsFilename, std::ifstream::in);
    //We have to do this weird little dance of nlohmann will crash on empty files.
    char c;
    inFile >> c;  //flag eof if at end of file, doesn't seem to throw exception
    if(inFile.eof()) {  //nlohmann doesn't like empty files
        inFile.close();
        Sawyer::Message::mlog[ERROR] << "Please provide a RWSets JSON file" <<    std::endl;
        exit(13);
    }
    inFile.seekg(0, std::ios_base::beg);

    //Now read in the sets
    nlohmann::json jsonDocument;
    inFile >> jsonDocument;
    inFile.close();
    std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash> rwSetRecords;

    ReadWriteSets::readCacheFromJson(jsonDocument, rwSetRecords);

    //Open output file, (or set it to std::cout)
    std::streambuf * outBuffer;
    std::ofstream tmpOutFile;
    if(settings.outputFilename == "") {
        outBuffer = std::cout.rdbuf();
    } else {
        tmpOutFile.open(settings.outputFilename);
        outBuffer = tmpOutFile.rdbuf();
    }
    std::ostream  outFile(outBuffer);

    //Now iterate through the RWSets, looking stuff up and outputting it
    for(const auto& funcRecord : rwSetRecords) {
        SgFunctionDefinition* funcDef = name2Node.getFunction(funcRecord.internalFunctionName);
        SgFunctionDeclaration* funcDecl =  VxUtilFuncs::getUniqueDeclaration(funcDef);
        if(funcDef) {
            outFile << "Function: " << funcRecord.internalFunctionName << " : " << funcDecl->get_qualified_name() << std::endl;
        } else {
            outFile << "Function: " << funcRecord.internalFunctionName << " : NOT FOUND" << std::endl;
            continue;
        }

        //If we found the function, continue with the variables
        for(const auto& readRecord : funcRecord.readSet) {
            SgNode* varSgNode = NULL;
            outFile << "ReadVar : " << readRecord.getVariableName();
            try {
                varSgNode = name2Node.getVariable(readRecord.getVariableName());
            } 
            catch (const AnonymousNameException& e) { outFile << " Error: " << e.what() << std::endl; continue;}
            catch (const AddressReferenceException& e) { outFile << " Error: " << e.what() << std::endl; continue;}

            SgInitializedName* varSgName = isSgInitializedName(varSgNode);
            SgFunctionDefinition* readFuncDef = isSgFunctionDefinition(varSgNode); 
            SgThisExp* thisRefExp = isSgThisExp(varSgNode);

            if(varSgName) {
                outFile << " : " << Rose::AST::NodeId(varSgName).toString()  << std::endl;
            } else if (thisRefExp) {
                outFile << " : " << Rose::AST::NodeId(thisRefExp).toString()  << std::endl;
            } else if (readFuncDef) {
                outFile << " : " << funcDecl->get_qualified_name() << std::endl;
            } else {
                outFile << " : NOT FOUND" << std::endl;
            }
        }
        for(const auto& writeRecord : funcRecord.writeSet) {
            SgNode* varSgNode = NULL;
            outFile << "WriteVar : " << writeRecord.getVariableName();
            try {
                varSgNode = name2Node.getVariable(writeRecord.getVariableName());
            } 
            catch (const AnonymousNameException& e) { outFile << " Error: " << e.what() << std::endl; continue;}
            catch (const AddressReferenceException& e) { outFile << " Error: " << e.what() << std::endl; continue;}
            
            SgInitializedName* varSgName = isSgInitializedName(varSgNode);
            SgFunctionDefinition* writtenFuncDef = isSgFunctionDefinition(varSgNode); 
            SgThisExp* thisRefExp = isSgThisExp(varSgNode);

            if(varSgName) {
                outFile << " : " << Rose::AST::NodeId(varSgName).toString()  << std::endl; 
            } else if (thisRefExp) {
                outFile << " : " << Rose::AST::NodeId(thisRefExp).toString()  << std::endl;
            } else if (writtenFuncDef) {
                outFile << " : " << funcDecl->get_qualified_name() << std::endl;
            } else {
                outFile << " : NOT FOUND" << std::endl;
            }
        }
    }

    //Well, this is kludgier than I expected maybe Herb Sutter has a better idea: http://www.gotw.ca/gotw/048.htm
    if(settings.outputFilename != "") {
        tmpOutFile.close();
        if (settings.force_stdout) {
            //this is the kludgiest
            std::ifstream infile;
            infile.open(settings.outputFilename);
            std::string line;
            while(std::getline(infile, line)) {
                std::cout << line << std::endl;
            }
        }
    }
    return 0;
}

