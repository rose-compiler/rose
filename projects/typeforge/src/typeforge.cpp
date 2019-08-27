
#include "rose.h"

#include "AstTerm.h"
#include "SgNodeHelper.h"

#include "AstProcessing.h"
#include "AstMatching.h"

#include "Sawyer/Graph.h"

#include "Typeforge/CommandLineOptions.hpp"
#include "Typeforge/TFTypeTransformer.hpp"
#include "Typeforge/SpecFrontEnd.hpp"
#include "Typeforge/Analysis.hpp"
#include "Typeforge/ToolConfig.hpp"
#include "Typeforge/CastStats.hpp"
#include "Typeforge/CppStdUtilities.hpp"
#include "Typeforge/TFTransformation.hpp"

#include "Typeforge/OpNet/OperandNetwork.hpp"
#include "Typeforge/OpNet/CollapseRules.hpp"
#include "Typeforge/OpNet/OperandData.hpp" // needed to instantiate ::Typeforge::OperandNetwork::collapse

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include <functional>
#include <regex>
#include <algorithm>
#include <list>

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

  // [ALTERNATIVE] "--opnet"

  if (args.isUserProvided("opnet")) {
    ::Typeforge::opnet.initialize(frontend(rose_args));
    ::Typeforge::opnet.toDot("opnet-0.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::AssignOperator>();
    ::Typeforge::opnet.toDot("opnet-1.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::MemberAccess>();
    ::Typeforge::opnet.toDot("opnet-2.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::ArrayAccess>();
    ::Typeforge::opnet.toDot("opnet-3.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::Dereference>();
    ::Typeforge::opnet.toDot("opnet-4.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::AddressOf>();
    ::Typeforge::opnet.toDot("opnet-5.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::VarRef>();
    ::Typeforge::opnet.toDot("opnet-6.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::FRef>();
    ::Typeforge::opnet.toDot("opnet-7.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::ThisRef>();
    ::Typeforge::opnet.toDot("opnet-8.dot");

    ::Typeforge::opnet.collapse<::Typeforge::CollapseRules::Call>();
    ::Typeforge::opnet.toDot("opnet-9.dot");

    return 0;
  }

  // [ALTERNATIVE] "--cast-stats":
  //     display statistics about casts operations in the program
  //     TODO This should be a simple query to the model.

  if(args.isUserProvided("cast-stats")) {
    CastStats castStats;
    castStats.computeStats(frontend(rose_args));
    cout << castStats.toString();
    return 0;
  }

  if (args.isUserProvided("typeforge-out")) {
    ::Typeforge::ToolConfig::filename = args["typeforge-out"].as<string>();
  }

  // Build ROSE IR and use it to initialize Typeforge

  ::Typeforge::typechain.initialize(frontend(rose_args));
  if (args.isUserProvided("set-analysis")) {
    ::Typeforge::typechain.toDot("set_analysis.dot", nullptr);
    ::Typeforge::typechain.toDot("set_analysis_double.dot", SageBuilder::buildDoubleType());
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
      ::Typeforge::SpecFrontEnd::parse(args["plugin"].as< std::vector< std::string > >(), commandList);
    }
    if (args.isUserProvided("spec-file")) {
      ::Typeforge::SpecFrontEnd::parse(args["spec-file"].as< std::vector< std::string > >(), commandList);
    }

    // Run

    commandList.runCommands(tfTransformation);

    // Read in the JSON file used to store "defered" actions ("list_*" queries)
    //   FIXME race-condition: parallel make with "CC=typeforge --typeforge-out xxx.json"

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
