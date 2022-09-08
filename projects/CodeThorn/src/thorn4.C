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

class Thorn4Parser
{
public:
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
                        .argument("reportDir", scl::anyParser(_ctOpt.reportFilePath))
                        .doc("filename prefix for all variants of graph files. Provide an absolute paths if thorn4 is invoked in multiple different directories."));
    thorn4Parser.insert(scl::Switch("mode")
                        .argument("mode", scl::anyParser(_ctOpt.multiSelectors.analysisMode))
                        .doc("analysis mode: concrete, abstract."));
    thorn4Parser.insert(scl::Switch("input-values")
                        .argument("inputValues", scl::anyParser(_ctOpt.inputValues))
                        .doc("set of values used for scanf in 'concrete' mode. e.g. \"{1,2,3}\""));
    thorn4Parser.insert(scl::Switch("status")
                        .intrinsicValue(true,_ctOpt.status)
                        .doc("print status messages during analysis."));
    thorn4Parser.insert(scl::Switch("reduce-stg")
                        .intrinsicValue(true,_ctOpt.reduceStg)
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
    processMultiSelectors();
    return cmdLine.unparsedArgs();
  }

  CodeThornOptions getParameters() {
    return _ctOpt;
  }

  void processMultiSelectors() {
  if(_ctOpt.multiSelectors.analysisMode=="abstract") {
    _ctOpt.solver=16; // default solver for this tool
    _ctOpt.sharedPStates=false; // required for solver 16
    _ctOpt.abstractionMode=1;
    _ctOpt.arrayAbstractionIndex=0;
  } else if(_ctOpt.multiSelectors.analysisMode=="concrete") {
    _ctOpt.solver=5; // default solver for concrete mode
    _ctOpt.sharedPStates=false;
    _ctOpt.abstractionMode=0;
    _ctOpt.arrayAbstractionIndex=-1; // no abstraction of arrays
    if(_ctOpt.inputValues=="{}") {
      cerr<<"Concrete mode selected, but no input values provided. Use option --input-values=\"{ ... }\" to provide a set of input values."<<endl;
      exit(1);
    }
  } else if(_ctOpt.multiSelectors.analysisMode=="none") {
    // no multi select requested
  } else {
    cerr<<"Wrong mode '"<<_ctOpt.multiSelectors.analysisMode<<"' provided on command line."<<endl;
    exit(1);
  }
}
private:
  CodeThornOptions _ctOpt;
};

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

    Thorn4Parser thorn4Parser;
    std::vector<std::string> cmdLineArgs{argv+0, argv+argc};
    std::vector<std::string> modifiedCmdLineArgs=thorn4Parser.parseArgs(cmdLineArgs);
    CodeThornOptions ctOpt=thorn4Parser.getParameters();

    // fixed settings for this tool (none of those setting can be set on the command line)
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

    mfacilities.control(ctOpt.logLevel);

    CodeThorn::TimingCollector      tc;

    cout << "Parsing and creating AST started."<<endl;
    SgProject* project = CodeThorn::CodeThornLib::runRoseFrontEnd(modifiedCmdLineArgs,ctOpt,tc);
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

