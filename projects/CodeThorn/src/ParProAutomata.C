#include "sage3basic.h"
#include "CodeThornCommandLineOptions.h"
#include "ParProAutomata.h"
#include "DotGraphCfgFrontend.h"
#include "ParProAnalyzer.h"
#include "PromelaCodeGenerator.h"
#include "ParProLtlMiner.h"
#include "ParProExplorer.h"
#include "ParallelAutomataGenerator.h"
// required for Parse::* functions
#include "Miscellaneous2.h"

#include "Diagnostics.h"
using namespace Sawyer::Message;

using namespace CodeThorn;
using namespace std;

namespace CodeThorn {
  bool ParProAutomata::handleCommandLineArguments(CommandLineOptions& args,Sawyer::Message::Facility& logger) {
    // ParPro command line options
    if (args.count("generate-automata")) {
      CodeThorn::ParProAutomata::generateAutomata();
      return true;
    }
    if (args.count("automata-dot-input")) {
      CodeThorn::ParProAutomata::automataDotInput(logger);
      return true;
    }
    return false;
  }

  void ParProAutomata::automataDotInput(Sawyer::Message::Facility logger) {
    if (args.count("seed")) {
      srand(args["seed"].as<int>());
    } else {
      srand(time(NULL));
    }
    DotGraphCfgFrontend dotGraphCfgFrontend;
    string filename = args["automata-dot-input"].as<string>();
    CfgsAndAnnotationMap cfgsAndMap = dotGraphCfgFrontend.parseDotCfgs(filename);
    list<Flow> cfgs = cfgsAndMap.first;
    EdgeAnnotationMap edgeAnnotationMap = cfgsAndMap.second;

    string promelaCode;
    if (args.count("promela-output")) {
      cout << "STATUS: generating PROMELA code (parallel processes based on CFG automata)..." << endl;
      PromelaCodeGenerator codeGenerator;
      promelaCode = codeGenerator.generateCode(cfgsAndMap);
      cout << "STATUS: done (LTLs not added yet)." << endl;
    }

    if (args.getBool("viz")) {
      int counter = 0;
      for(list<Flow>::iterator i=cfgs.begin(); i!=cfgs.end(); i++) {
        Flow cfg = *i;
        cfg.setDotOptionDisplayLabel(false);
        cfg.setDotOptionDisplayStmt(false);
        cfg.setDotOptionEdgeAnnotationsOnly(true);
        string outputFilename = "parallelComponentCfg_" + boost::lexical_cast<string>(counter) + ".dot";
        write_file(outputFilename, cfg.toDot(NULL));
        cout << "generated " << outputFilename <<"."<<endl;
        counter++;
      }
    }

    vector<Flow*> cfgsAsVector(cfgs.size());
    int index = 0;
    for (list<Flow>::iterator i=cfgs.begin(); i!=cfgs.end(); ++i) {
      cfgsAsVector[index] = &(*i);
      ++index;
    }

    ParProExplorer explorer(cfgsAsVector, edgeAnnotationMap);
    if (args.count("verification-engine")) {
      string verificationEngine = args["verification-engine"].as<string>();
      if (verificationEngine == "ltsmin") {
        explorer.setUseLtsMin(true);
      }
    } 
    if (args.getBool("keep-systems")) {
      explorer.setStoreComputedSystems(true);
    } else {
      explorer.setStoreComputedSystems(false);
    }
    if (args.getBool("parallel-composition-only")) {
      explorer.setParallelCompositionOnly(true);
    } else {
      explorer.setStoreComputedSystems(false);
    }
    if (args.count("use-components")) {
      string componentSelection = args["use-components"].as<string>();
      if (componentSelection == "all") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_ALL);
        if (args.count("ltl-mode")) {
          string ltlMode= args["ltl-mode"].as<string>();
          if (ltlMode == "mine") {
            explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
          }
        }
      } else if (componentSelection == "subsets-fixed") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_SUBSET_FIXED);
        explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_FINITE);
        if (args.count("fixed-subsets")) {
          string setsstring=args["fixed-subsets"].as<string>();
          list<set<int> > intSets=Parse::integerSetList(setsstring);
          explorer.setFixedComponentSubsets(intSets);
        } else {
          logger[ERROR] << "selected a fixed set of components but no were selected. Please use option \"--fixed-subsets=<csv-id-list>\".";
          ROSE_ASSERT(0);
        }
      } else if (componentSelection == "subsets-random") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_SUBSET_RANDOM);
        if (args.count("num-random-components")) {
          explorer.setNumberRandomComponents(args["num-random-components"].as<int>());
        } else {
          explorer.setNumberRandomComponents(std::min(3, (int) cfgsAsVector.size()));
        }
        if (args.count("different-component-subsets")) {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_FINITE);
          explorer.setNumberDifferentComponentSubsets(args["different-component-subsets"].as<int>());
        } else {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
        }
      }
    } else {
      explorer.setComponentSelection(PAR_PRO_COMPONENTS_ALL);
      if (args.count("ltl-mode")) {
        string ltlMode= args["ltl-mode"].as<string>();
        if (ltlMode == "mine") {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
        }
      }
    }

    if ( args.count("check-ltl") ) {
      explorer.setLtlMode(PAR_PRO_LTL_MODE_CHECK);
      explorer.setLtlInputFilename(args["check-ltl"].as<string>());
    } else {
      if ( args.count("ltl-mode") ) {
        string ltlMode= args["ltl-mode"].as<string>();
        if (ltlMode == "check") {
          logger[ERROR] << "ltl mode \"check\" selected but option \"--check-ltl=<filename>\" not used. Please provide LTL property file." << endl;
          ROSE_ASSERT(0);
        } else if (ltlMode == "mine") {
          explorer.setLtlMode(PAR_PRO_LTL_MODE_MINE);
          if (args.count("num-components-ltl")) {
            explorer.setNumberOfComponentsForLtlAnnotations(args["num-components-ltl"].as<int>());
          } else {
            explorer.setNumberOfComponentsForLtlAnnotations(std::min(3, (int) cfgsAsVector.size()));
          }
          if (args.count("minimum-components")) {
            explorer.setMinNumComponents(args["minimum-components"].as<int>());
          }
          if (args.count("mine-num-verifiable")) {
            explorer.setNumRequiredVerifiable(args["mine-num-verifiable"].as<int>());
          } else {
            explorer.setNumRequiredVerifiable(10);
          }
          if (args.count("mine-num-falsifiable")) {
            explorer.setNumRequiredFalsifiable(args["mine-num-falsifiable"].as<int>());
          } else {
            explorer.setNumRequiredFalsifiable(10);
          }
          if (args.count("minings-per-subsets")) {
            explorer.setNumMiningsPerSubset(args["minings-per-subsets"].as<int>());
          } else {
            explorer.setNumMiningsPerSubset(50);
          }
        } else if (ltlMode == "none") {
          explorer.setLtlMode(PAR_PRO_LTL_MODE_NONE);
        }
      } else {
        explorer.setLtlMode(PAR_PRO_LTL_MODE_NONE);
      }
    }

    if (args.getBool("viz")) {
      explorer.setVisualize(true);
    }

    if (!args.getBool("promela-output-only")) {
      explorer.explore();
    }
  
    if (args.count("check-ltl")) {
      PropertyValueTable* ltlResults=nullptr;
      if (args.getBool("promela-output-only")) { // just read the properties into a PropertyValueTable
        SpotConnection spotConnection(args["check-ltl"].as<string>());
        ltlResults = spotConnection.getLtlResults();
      } else {
        ltlResults = explorer.propertyValueTable();
      }
      bool withCounterexamples = false;
      ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexamples);
      cout << "=============================================================="<<endl;
      ltlResults->printResultsStatistics();
      cout << "=============================================================="<<endl;
    }

    bool withResults = args.getBool("output-with-results");
    bool withAnnotations = args.getBool("output-with-annotations");
#ifdef HAVE_SPOT
    if (args.count("promela-output")) {
      PropertyValueTable* ltlResults;
      if (args.getBool("promela-output-only")) { // just read the properties into a PropertyValueTable
        SpotConnection spotConnection(args["check-ltl"].as<string>());
        ltlResults = spotConnection.getLtlResults();
      } else {
        ltlResults = explorer.propertyValueTable();
      }
      // uses SpotMiscellaneous::spinSyntax as callback to avoid static dependency of ltlResults on SpotMisc.
      string promelaLtlFormulae = ltlResults->getLtlsAsPromelaCode(withResults, withAnnotations,&SpotMiscellaneous::spinSyntax);
      promelaCode += "\n" + promelaLtlFormulae;
      string filename = args["promela-output"].as<string>();
      write_file(filename, promelaCode);
      cout << "generated " << filename  <<"."<<endl;
    }
#endif
    if (args.count("ltl-properties-output")) {
      string ltlFormulae = explorer.propertyValueTable()->getLtlsRersFormat(withResults, withAnnotations);
      string filename = args["ltl-properties-output"].as<string>();
      write_file(filename, ltlFormulae);
      cout << "generated " << filename  <<"."<<endl;
    }
    if(!args.count("quiet"))
      cout << "STATUS: done." << endl;
  }

  void ParProAutomata::generateAutomata() {
    if (args.count("seed")) {
      srand(args["seed"].as<int>());
    } else {
      srand(time(NULL));
    }
    ParallelAutomataGenerator automataGenerator;
    int numberOfAutomata = 10;
    if (args.count("num-automata")) {
      numberOfAutomata = args["num-automata"].as<int>();
    }
    pair<int, int> numberOfSyncsRange = pair<int, int>(9, 18);
    if (args.count("num-syncs-range")) {
      numberOfSyncsRange = parseCsvIntPair(args["num-syncs-range"].as<string>());
    }
    pair<int, int> numberOfCirclesPerAutomatonRange = pair<int, int>(2, 4);
    if (args.count("num-circles-range")) {
      numberOfCirclesPerAutomatonRange = parseCsvIntPair(args["num-circles-range"].as<string>());
    }
    pair<int, int> circleLengthRange = pair<int, int>(5, 8);
    if (args.count("circle-length-range")) {
      circleLengthRange = parseCsvIntPair(args["circle-length-range"].as<string>());
    }
    pair<int, int> numIntersectionsOtherCirclesRange = pair<int, int>(1, 2);
    if (args.count("num-intersections-range")) {
      numIntersectionsOtherCirclesRange = parseCsvIntPair(args["num-intersections-range"].as<string>());
    }
    vector<Flow*> automata = automataGenerator.randomlySyncedCircleAutomata(
                                                                            numberOfAutomata,
                                                                            numberOfSyncsRange,
                                                                            numberOfCirclesPerAutomatonRange,
                                                                            circleLengthRange,
                                                                            numIntersectionsOtherCirclesRange);
    Visualizer visualizer;
    string dotCfas = visualizer.cfasToDotSubgraphs(automata);
    string outputFilename = args["generate-automata"].as<string>();
    write_file(outputFilename, dotCfas);
    cout << "generated " << outputFilename <<"."<<endl;
  }
}
