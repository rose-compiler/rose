
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>

#include "rose.h"
#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "CommandLineOptions.h"
#include "AstProcessing.h"
#include "AstMatching.h"
#include "Sawyer/Graph.h"
#include "TFTypeTransformer.h"
#include "SpecFrontEnd.h"
#include "Analysis.h"
#include "ToolConfig.hpp"

//preparation for using the Sawyer command line parser
//#define USE_SAWYER_COMMANDLINE
#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

#include "CastStats.h"
#include "CastTransformer.h"
#include "CastGraphVis.h"
#include "CppStdUtilities.h"
#include <utility>
#include <functional>
#include <regex>
#include <algorithm>
#include <list>
#include "TFTransformation.h"

#include "OperandNetwork.h"

using namespace std;
using namespace Typeforge;

int main (int argc, char* argv[]) {
  ROSE_INITIALIZE;
  Rose::global_options.set_frontend_notes(false);
  Rose::global_options.set_frontend_warnings(false);
  Rose::global_options.set_backend_warnings(false);

  auto rose_args = ::Typeforge::parse_args(argc, argv);
  rose_args.insert(rose_args.begin(), "rose");
  if (!args.count("compile")) {
    rose_args.push_back("-rose:skipfinalCompileStep");
  }
 
  // Build ROSE IR and use it to initialize Typeforge

  ::Typeforge::typechain.initialize(frontend(rose_args));
  if (args.isUserProvided("set-analysis")) {
    ::Typeforge::typechain.toDot("set_analysis.dot", nullptr);
    ::Typeforge::typechain.toDot("set_analysis_double.dot", SageBuilder::buildDoubleType());
  }

  if (args.isUserProvided("opnet")) {
    ::Typeforge::opnet.initialize();
    ::Typeforge::opnet.toDot("operand-network.dot");
  }

  // [ALTERNATIVE] "--cast-stats": display statistics about casts operations in the program TODO This should be a simple query to the model.

  if(args.isUserProvided("cast-stats")) {
    CastStats castStats;
    castStats.computeStats(::Typeforge::project);
    cout << castStats.toString();
    return 0;
  }

  // Transformation Objects => TODO should be only one

  TFTransformation tfTransformation;

  // Traces?

  if (args.isUserProvided("trace")) {
    ::Typeforge::transformer.setTraceFlag(true);
  }
  tfTransformation.trace = ::Typeforge::transformer.getTraceFlag();

  // Three cases:
  //   "--explicit": makes implicit cast explicit
  //   "--annotate": adds comments in the generated file
  //   commands from file(s) TODO all commands should execute from the command list: 1st & 2nd cases should simply add commands to the list

  if (args.isUserProvided("explicit")) {
    ::Typeforge::makeAllCastsExplicit();
    std::cout << "Converted all implicit casts to explicit casts." << std::endl;

  } else if (args.isUserProvided("annotate")) {
    ::Typeforge::annotateImplicitCastsAsComments();
    std::cout << "Annotated program with comments." << std::endl;

  } else {

    // Read commands

    CommandList commandList;
    if (args.isUserProvided("plugin")) {
      SpecFrontEnd::parse(args["plugin"].as< std::vector< std::string > >(), commandList);
    }
    if (args.isUserProvided("spec-file")) {
      SpecFrontEnd::parse(args["spec-file"].as< std::vector< std::string > >(), commandList);
    }

    // Run

    commandList.runCommands(tfTransformation);

    // Read in the JSON file used to store "defered" actions ("list_*" queries)
    //   FIXME race-condition: parallel make with "CC=typeforge --typeforge-out xxx.json"

    if (args.isUserProvided("typeforge-out")) {
      ToolConfig::filename = args["typeforge-out"].as<string>();
    }

    // Analyze Phase

//  ::Typeforge::transformer.analyze(commandList);
    tfTransformation.transformationAnalyze();

    // Execution Phase

    ::Typeforge::transformer.execute();
    tfTransformation.transformationExecution();

    // Overwrite the JSON file used to store "defered" actions (cumulate previous one)
    //   FIXME race-condition: parallel make (see above)

    if (args.isUserProvided("typeforge-out")) {
      ::Typeforge::ToolConfig::appendAnalysis(SageBuilder::buildDoubleType());
      ::Typeforge::ToolConfig::writeGlobal();
    }

    // Output Phase

    if (args.isUserProvided("csv-stats-file")) {
      string csvFileName=args.getString("csv-stats-file");
      ::Typeforge::TFTypeTransformer::generateCsvTransformationStats(csvFileName, 0 /* FIXME typeforgeSpecFrontEnd.getNumTypeReplace() => SpecFrontEnd::numTypeReplace was never incremented*/, ::Typeforge::transformer, tfTransformation);
    }

    if (args.isUserProvided("stats")) {
      ::Typeforge::TFTypeTransformer::printTransformationStats(0 /* FIXME see above */, ::Typeforge::transformer, tfTransformation);
    }
  }

  return backend(::Typeforge::project);
}
