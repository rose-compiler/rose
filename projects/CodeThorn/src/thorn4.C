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

#include "sys/stat.h"

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
    std::string reportDir=".";
    std::string mode="abstract";
    bool checkCLanguage=true;
    bool status=true;
    bool reduceStg=false;
    std::string inputValues="{}";
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

    thorn4Parser.insert(scl::Switch("report-dir")
                        .argument("reportDir", scl::anyParser(params.reportFilePath))
                        .doc("filename prefix for all variants of graph files. Provide an absolute paths if thorn4 is invoked in multiple different directories."));
    thorn4Parser.insert(scl::Switch("mode")
                        .argument("mode", scl::anyParser(params.multiSelectors.analysisMode))
                        .doc("analysis mode: concrete, abstract."));
    thorn4Parser.insert(scl::Switch("input-values")
                        .argument("inputValues", scl::anyParser(params.inputValues))
                        .doc("set of values used for scanf in 'concrete' mode. e.g. \"{1,2,3}\""));
    thorn4Parser.insert(scl::Switch("status")
                        .intrinsicValue(true,params.status)
                        .doc("print status messages during analysis."));
    thorn4Parser.insert(scl::Switch("reduce-stg")
                        .intrinsicValue(true,params.reduceStg)
                        .doc("reduce STS graph to input/output states."));
    p.purpose("Generates State Transition System Graph files")
      .doc("synopsis",
           "@prop{programName} [@v{switches}] @v{specimen_name}")
      .doc("description",
           "This program generates state transition system graph files.");
    scl::ParserResult cmdLine = p.with(thorn4Parser).parse(clArgs).apply();

    const std::vector<std::string> remainingArgs = cmdLine.unparsedArgs();
    for (const std::string& arg: remainingArgs) {
      //mlog[DEBUG] <<"remaining arg: \"" <<Rose::StringUtility::cEscape(arg) <<"\"\n";
      if (CppStdUtilities::isPrefix("--thorn4:",arg)) {
        cerr <<"thorn4: unrecognized command line option: \"" <<Rose::StringUtility::cEscape(arg) <<"\"\n";
        exit(1);
      }
    }

    return cmdLine.unparsedArgs();
  }

  CodeThornOptions getParameters() {
    return params;
  }
private:
  CodeThornOptions params;
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

void processMultiSelectors(CodeThornOptions& ctOpt) {
  if(ctOpt.multiSelectors.analysisMode=="abstract") {
    ctOpt.solver=16; // default solver for this tool
    ctOpt.sharedPStates=false; // required for solver 16
    ctOpt.abstractionMode=1;
    ctOpt.arrayAbstractionIndex=0;
  } else if(ctOpt.multiSelectors.analysisMode=="concrete") {
    ctOpt.solver=5; // default solver for concrete mode
    ctOpt.sharedPStates=false;
    ctOpt.abstractionMode=0;
    ctOpt.arrayAbstractionIndex=-1; // no abstraction of arrays
    if(ctOpt.inputValues=="{}") {
      cerr<<"Concrete mode selected, but no input values provided. Use option --input-values=\"{ ... }\" to provide a set of input values."<<endl;
      exit(1);
    }
  } else if(ctOpt.multiSelectors.analysisMode=="none") {
    // no multi select requested
  } else {
    cerr<<"Wrong mode '"<<ctOpt.multiSelectors.analysisMode<<"' provided on command line."<<endl;
    exit(1);
  }
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

    LTLOptions ltlOpt; // not used in this tool, but required for some default parameters
    ParProOptions parProOpt; // not used in this tool, but required for some default parameters

    std::vector<std::string> cmdLineArgs{argv+0, argv+argc};
    Thorn4Parser thorn4Parser;
    CStringVector unparsedArgsCStyle(thorn4Parser.parseArgs(std::move(cmdLineArgs)));
    CodeThornOptions ctOpt=thorn4Parser.getParameters();
    int thornArgc = unparsedArgsCStyle.size();
    char** thornArgv = unparsedArgsCStyle.firstCArg();

    ctOpt.checkCLanguage=true;
    ctOpt.callStringLength=-1; // unbounded length
    ctOpt.normalizeLevel=2;
    ctOpt.intraProcedural=false; // inter-procedural
    ctOpt.contextSensitive=true;
    ctOpt.pointerSetsEnabled=false;
    ctOpt.fastPointerHashing=false; // ensure stable dot layout
    ctOpt.explorationMode="topologic-sort";
    ctOpt.precisionLevel=2;
    ctOpt.logLevel="none";
    ctOpt.visualization.vis=true; // generates ast, icfg, tg1, tg2
    ctOpt.vimReportFileName=""; // do not generated vim report

    CodeThorn::colorsEnabled=ctOpt.colors;
    processMultiSelectors(ctOpt);

    mfacilities.control(ctOpt.logLevel);

    CodeThorn::TimingCollector      tc;

    cout << "Parsing and creating AST started."<<endl;
    SgProject* project = CodeThorn::CodeThornLib::runRoseFrontEnd(thornArgc,thornArgv,ctOpt,tc);
    ROSE_ASSERT(project);
    cout << "Parsing and creating AST finished."<<endl;

    if(ctOpt.checkCLanguage) {
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

    if(ctOpt.reportFilePath!=".") {
      // create dir for ctOpt.reportFilePath
      int status = mkdir(ctOpt.reportFilePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if(status!=0) {
        cerr<<"Error: could not create directory "<<ctOpt.reportFilePath<<endl;
      } else {
        if(ctOpt.status)
          cout<<"STATUS: created directory "<<ctOpt.reportFilePath<<endl;
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

