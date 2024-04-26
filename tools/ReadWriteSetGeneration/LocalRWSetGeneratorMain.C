#include <sage3basic.h>
#include <sageInterface.h>
#include <Rose/CommandLine.h>
#include <nlohmann/json.hpp>
#include <Rose/Yaml.h>
#include "LocalRWSetGenerator.h"
#include <sstream>
// 03/20/2023): Adding boost filesystem usage:
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>


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
using namespace Sawyer::Message::Common;
using namespace nlohmann;


//---------------------------------------------------------------------------------
// Settings that can be adjusted from the command line and their default values.
// Had to use ints because Sawyer doesn't seem to know what to with enums
struct Settings {
    std::string yamlConfigFilename      = "";
    bool include_empties= false; // Include empty functions in output
    
    std::string rwSetsFilename      = "";
    std::string applicationRootDir  = "";
    bool force_stdout = false;


    Settings(): yamlConfigFilename(""), 
                include_empties(false),
                rwSetsFilename(""),
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
            std::cout << "Exception " << e.Type() << ": " << e.what() << " configuration file not found: " << settings.yamlConfigFilename << std::endl;
            exit(12);
        }

        settings.include_empties = config["include-empties"].As<bool>(settings.include_empties);  //How global reads are allowed to be
        settings.rwSetsFilename = config["rwSets-filename"].As<std::string>(settings.rwSetsFilename);  //How global reads are allowed to be
        settings.applicationRootDir = config["app-root"].As<std::string>(settings.applicationRootDir);  //How global reads are allowed to be
        settings.force_stdout = config["force-stdout"].As<bool>(settings.force_stdout);
    }
}

/** For documentation purposes each function contains the full command
 *  of how it was created (except the initial command, which might be confusing)
 **/
std::string combineCommandLine(size_t argc, char *argv[]) {
  std::stringstream ss;
  for(size_t ii = 1; ii <argc; ++ii) {
    ss << argv[ii];
    if(ii < argc-1) {
      ss << " ";
    } 
  }
  return ss.str();
}


int
main(size_t argc, char *argv[]) {
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
    SwitchGroup localRWSetGeneratorSwitches("LocalRWSetGenerator-specific switches");
    localRWSetGeneratorSwitches.name("LocalRWSetGenerator");                                  // the optional switch prefix, i.e., "--tool:whatever"

    localRWSetGeneratorSwitches.insert(Switch("config-filename")
                                  .argument("filename", anyParser(settings.yamlConfigFilename))
                                  .doc("Filename of yaml configuration file, used as alternate to command line arguments"));
    localRWSetGeneratorSwitches.insert(Switch("rwSets-filename")
                                  .argument("filename", anyParser(settings.rwSetsFilename))
                                  .doc("Filename to output the list of read/write sets to.  If not present, writes to stdout."));
    localRWSetGeneratorSwitches.insert(Switch("include-empties", 'e')
                                  .shortName('e')
                                  .intrinsicValue(true, settings.include_empties)
                                  .doc("Empty functions are normally filtered out, pass this to include them."));
    localRWSetGeneratorSwitches.insert(Switch("app-root")
                                  .argument("filename", anyParser(settings.applicationRootDir))
                                  .doc("The root of the application we're processing.  Used to filter external functions.  Duplicate of the rose option -rose:applicationRootDirectory"));
    localRWSetGeneratorSwitches.insert(Switch("force-stdout")
                                  .intrinsicValue(true, settings.force_stdout)
                                  .doc("Forces the output to be written to stdout regardless of whether --rwSets-filename is used."));
    // Parse the command-line and get the non-switch, positional arguments at the end
    std::vector<std::string> files = p.with(localRWSetGeneratorSwitches).parse(argc, argv).apply().unreachedArgs();
    
    // See if the yaml configuration exists and is useful.
    readConfigFile(settings);

    std::string combinedCommandLine = combineCommandLine(argc, argv);


    //----------------------------------------------------------
    //Command line processing done, now do ROSE work
    SgProject *root = frontend(argc, argv);
    
    if(settings.applicationRootDir != "") {
        root->set_applicationRootDirectory(settings.applicationRootDir);
    }

    LocalRWSetGenerator rwSetGen(combinedCommandLine);
    rwSetGen.collectReadWriteSets(root);

    if(settings.rwSetsFilename == "" || settings.force_stdout) {
      std::cout << rwSetGen;
    }
    if (settings.rwSetsFilename != "");
    {
      rwSetGen.outputCache(settings.rwSetsFilename);
    }
  

   return 0;
}

