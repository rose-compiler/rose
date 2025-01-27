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

using namespace std;
using namespace CodeThorn;

const std::string thorn4version = "0.9.0";

// required for some options in codethorn library (until removed)
CodeThorn::CommandLineOptions CodeThorn::args;

#include "Thorn4CommandLineParser.h"

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
    std::vector<std::string> modifiedCmdLineArgs=thorn4Parser.parseArgs(argc,argv);
    CodeThornOptions ctOpt=thorn4Parser.getOptions();

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
    msgError() << "Error: " << e.what() << endl;
  } catch(char const* str) {
    msgError() << "Error: " << str << endl;
  } catch(const std::string& str) {
    msgError() << "Error: " << str << endl;
  } catch(...) {
    msgError() << "Error: Unknown exception raised." << endl;
  }

  mfacilities.shutdown();
  return errorCode;
}

