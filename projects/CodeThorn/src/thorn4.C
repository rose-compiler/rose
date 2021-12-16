// An example ROSE plugin
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include <boost/lexical_cast.hpp>

// Mandatory include headers
#include "rose.h"
#include "Rose/CommandLine.h"
#include "Sawyer/CommandLine.h"
#include "sageGeneric.h"

// Programmatic codethorn headers
#include "CodeThornLib.h"
#include "CodeThornOptions.h"
#include "TimeMeasurement.h"

// tool  specific
#include "AstTerm.h"
#include "CppStdUtilities.h"
#include "LanguageRestrictor.h"
#include "LanguageRestrictorCollection.h"

namespace si  = SageInterface;
namespace scl = Sawyer::CommandLine;

const std::string thorn4version = "0.9.0";

// required for some options in codethorn library (until removed)
CodeThorn::CommandLineOptions CodeThorn::args;

namespace {
  
} // anonymous namespace

class Thorn4Parser
{
public:
  struct Parameters
  {
    std::string graphFileNamePrefix="astterm";
    std::string mode="abstract";
    bool checkCLanguage=true;
  };
  
  /// sets the Thorn4Parser settings using the command line arguments
  /// \returns a list of unparsed arguments
  std::vector<std::string>
  parseArgs(std::vector<std::string> clArgs)
  {
    scl::Parser p = Rose::CommandLine::createEmptyParserStage("", "");

    // things like --help, --version, --log, --threads, etc.
    p.with(Rose::CommandLine::genericSwitches());
    //~ p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis

    // Create a group of switches specific to this tool
    scl::SwitchGroup thorn4Parser("thorn4 - specific switches");

    thorn4Parser.name("thorn4");  // the optional switch prefix

    thorn4Parser.insert(scl::Switch("file-prefix")
                        .argument("filename", scl::anyParser(params.graphFileNamePrefix))
                        .doc("filename prefix for all variants of graph files. Provide an absolute paths if thorn4 is invoked in multiple different directories."));
    thorn4Parser.insert(scl::Switch("mode")
                        .argument("mode", scl::anyParser(params.graphFileNamePrefix))
                        .doc("analysis mode: concrete, abstract."));
    p.purpose("Generates State Transition System Graph files")
      .doc("synopsis",
           "@prop{programName} [@v{switches}] @v{specimen_name}")
      .doc("description",
           "This program generates AST files in term format. The same term format can be used in the AstMatcher as input for matching AST subtrees.");
    scl::ParserResult cmdline = p.with(thorn4Parser).parse(clArgs).apply();
    
    return cmdline.unparsedArgs();
  }

  Parameters getParameters() {
    return params;
  }
private:
  Parameters params;
};

namespace
{
  struct CStringVector : private std::vector<char*>
  {
      using base = std::vector<char*>;

      CStringVector()
      : base()
      {}

      explicit
      CStringVector(const std::vector<std::string>& clArgs)
      : base()
      {
        base::reserve(clArgs.size());

        for (const std::string& s : clArgs)
        {
          const size_t sz         = s.size();
          char*        cstr       = new char[sz+1];
          const char*  char_begin = s.c_str();

          std::copy(char_begin, char_begin+sz, cstr);
          cstr[sz] = '\0';
          base::push_back(cstr);
        }
      }

      ~CStringVector()
      {
        for (char* elem : *this)
          delete elem;
      }

      using base::size;

      char** firstCArg() { return &front(); }
  };
}

int main( int argc, char * argv[] )
{
  using Sawyer::Message::mfacilities;

  int errorCode = 1;

  try
  {
    ROSE_INITIALIZE;
    CodeThorn::CodeThornLib::configureRose();

    std::vector<std::string> cmdLineArgs{argv+0, argv+argc};
    Thorn4Parser thorn4Parser;
    CStringVector unparsedArgsCStyle(thorn4Parser.parseArgs(std::move(cmdLineArgs)));
    Thorn4Parser::Parameters params=thorn4Parser.getParameters();
    int thornArgc = unparsedArgsCStyle.size();
    char** thornArgv = unparsedArgsCStyle.firstCArg();

    //~ for (int i = 0; i < thornArgc; ++i)
      //~ std::cerr << thornArgv[i] << std::endl;

    CodeThornOptions         ctOpt;

    mfacilities.control(ctOpt.logLevel);
    //logTrace() << "Log level is " << ctOpt.logLevel << endl;
    CodeThorn::TimingCollector      tc;

    cout << "Parsing and creating AST started."<<endl;
    //~ tc.startTimer();
    SgProject* project = CodeThorn::CodeThornLib::runRoseFrontEnd(thornArgc,thornArgv,ctOpt,tc);
    ROSE_ASSERT(project);
    cout << "Parsing and creating AST finished."<<endl;

    string fileNamePrefix=params.graphFileNamePrefix;
    //cout<<"DEBUG: "<<fileNamePrefix<<":"<<params.astTerm<<params.typedAstTerm<<params.mlAstTerm<<endl;
    
    if(params.checkCLanguage) {
      LanguageRestrictorC cLangRestrictor;
      bool programOK=cLangRestrictor.checkProgram(project);
      cout<<"C Program check: ";
      if(programOK)
        cout<<"PASS";
      else
        cout<<"FAIL";
      cout<<endl;
      if(!programOK) {
        exit(1);
      }
    }
    cout<<"No graph files generated (not activated yet)."<<endl;
    errorCode = 0;
  } catch(const std::exception& e) {
    logError() << "Error: " << e.what() << endl;
  } catch(char const* str) {
    logError() << "Error: " << str << endl;
  } catch(const std::string& str) {
    logError() << "Error: " << str << endl;
  } catch(...) {
    logError() << "Error: Unknown exception raised." << endl;
  }

  mfacilities.shutdown();
  return errorCode;
}

