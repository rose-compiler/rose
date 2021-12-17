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
#include "Miscellaneous.h" // colors on/off variable

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
    std::string graphFileNamePrefix="";
    std::string mode="abstract";
    bool checkCLanguage=true;
    bool status=true;
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
    thorn4Parser.insert(scl::Switch("status")
                        //.argument("selection", scl::booleanParser(params.astTerm))
                        //.intrinsicValue("true",scl::booleanParser(params.astTerm))
                        .intrinsicValue(true,params.status)
                        .doc("print status messages during analysis."));
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
    CodeThorn::initDiagnostics();
    //CodeThorn::initDiagnosticsLTL();
    CodeThorn::CodeThornLib::configureRose();
    //configureRersSpecialization();

    CodeThornOptions ctOpt;
    LTLOptions ltlOpt; // not used in this tool, but required for some default parameters
    ParProOptions parProOpt; // not used in this tool, but required for some default parameters

    ctOpt.solver=16; // default solver for this tool
    ctOpt.abstractionMode=1;
    //ctOpt.interpreterMode=0;
    ctOpt.reduceStg=false;
    ctOpt.callStringLength=-1; // unbounded length
    ctOpt.normalizeLevel=2;
    ctOpt.arrayAbstractionIndex=0;
    ctOpt.initialStateGlobalVarsAbstractionLevel=1;
    ctOpt.intraProcedural=false;
    ctOpt.pointerSetsEnabled=false;
    ctOpt.sharedPStates=false;
    ctOpt.fastPointerHashing=true;
    ctOpt.explorationMode="topologic-sort";
    ctOpt.precisionLevel=2;
    ctOpt.contextSensitive=true;
    ctOpt.logLevel="none";
    ctOpt.visualization.vis=true;

    CodeThorn::colorsEnabled=ctOpt.colors;
    std::vector<std::string> cmdLineArgs{argv+0, argv+argc};
    Thorn4Parser thorn4Parser;
    CStringVector unparsedArgsCStyle(thorn4Parser.parseArgs(std::move(cmdLineArgs)));
    Thorn4Parser::Parameters params=thorn4Parser.getParameters();
    int thornArgc = unparsedArgsCStyle.size();
    char** thornArgv = unparsedArgsCStyle.firstCArg();

    ctOpt.status=params.status;

    //~ for (int i = 0; i < thornArgc; ++i)
      //~ std::cerr << thornArgv[i] << std::endl;

    mfacilities.control(ctOpt.logLevel);
    //logTrace() << "Log level is " << ctOpt.logLevel << endl;
    CodeThorn::TimingCollector      tc;

    cout << "Parsing and creating AST started."<<endl;
    //~ tc.startTimer();
    SgProject* project = CodeThorn::CodeThornLib::runRoseFrontEnd(thornArgc,thornArgv,ctOpt,tc);
    ROSE_ASSERT(project);
    cout << "Parsing and creating AST finished."<<endl;
    mfacilities.control(ctOpt.logLevel);

    string fileNamePrefix=params.graphFileNamePrefix;
    
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


    IOAnalyzer* analyzer=CodeThornLib::createAnalyzer(ctOpt,ltlOpt); // sets ctOpt,ltlOpt in analyzer
    CodeThornLib::optionallyRunInternalChecks(ctOpt,argc,argv);
    analyzer->configureOptions(ctOpt,ltlOpt,parProOpt);
    Solver* solver = CodeThornLib::createSolver(ctOpt);
    ROSE_ASSERT(solver);
    analyzer->setSolver(solver);
    analyzer->setOptionContextSensitiveAnalysis(ctOpt.contextSensitive);
    //optionallySetRersMapping(ctOpt,ltlOpt,analyzer);
    AbstractValue::pointerSetsEnabled=ctOpt.pointerSetsEnabled;
    analyzer->runAnalysisPhase1(project,tc);
    if(ctOpt.runSolver) {
      analyzer->runAnalysisPhase2(tc);
    } else {
      cout<<"STATUS: skipping solver run."<<endl;
    }
    tc.startTimer();
    CodeThornLib::optionallyGenerateCallGraphDotFile(ctOpt,analyzer);
    tc.stopTimer(TimingCollector::callGraphDotFile);

    if(analyzer->getSolver()->createsTransitionSystem()) {
      if(ctOpt.reduceStg) {
        analyzer->reduceStgToInOutStates();
      }
    }

    tc.startTimer();
    CodeThornLib::optionallyRunVisualizer(ctOpt,analyzer,project);
    tc.stopTimer(TimingCollector::visualization);
    CodeThornLib::optionallyPrintRunTimeAndMemoryUsageReport(ctOpt,tc);
    CodeThornLib::generateRunTimeAndMemoryUsageReport(ctOpt,tc);
    if(ctOpt.status) cout<<color("normal")<<"done."<<endl;

    delete analyzer;

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

