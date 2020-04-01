#include "sage3basic.h"
#include "CodeThornCommandLineOptions.h"
#include "ParProAutomata.h"
#include "DotGraphCfgFrontend.h"
#include "ParProAnalyzer.h"
#include "PromelaCodeGenerator.h"
#include "ParProLtlMiner.h"
#include "ParProExplorer.h"
#include "ParProAutomataGenerator.h"
// required for Parse::* functions
#include "Miscellaneous2.h"

#include "Diagnostics.h"
using namespace Sawyer::Message;

using namespace CodeThorn;
using namespace std;

namespace CodeThorn {
  bool ParProAutomata::handleCommandLineArguments(CommandLineOptions& args,Sawyer::Message::Facility& logger) {
    // ParPro command line options
    if (args.isDefined("generate-automata")) {
      CodeThorn::ParProAutomata::generateAutomata();
      return true;
    }
    if (args.isDefined("automata-dot-input")) {
      CodeThorn::ParProAutomata::automataDotInput(logger);
      return true;
    }
    return false;
  }

  void ParProAutomata::automataDotInput(Sawyer::Message::Facility logger) {
    if (args.isDefined("seed")) {
      srand(args.getInt("seed"));
    } else {
      srand(time(NULL));
    }
    DotGraphCfgFrontend dotGraphCfgFrontend;
    string filename = args.getString("automata-dot-input");
    CfgsAndAnnotationMap cfgsAndMap = dotGraphCfgFrontend.parseDotCfgs(filename);
    list<Flow> cfgs = cfgsAndMap.first;
    EdgeAnnotationMap edgeAnnotationMap = cfgsAndMap.second;

    string promelaCode;
    if (args.isDefined("promela-output")) {
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
    if (args.isDefined("verification-engine")) {
      string verificationEngine = args.getString("verification-engine");
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
    if (args.isDefined("use-components")) {
      string componentSelection = args.getString("use-components");
      if (componentSelection == "all") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_ALL);
        if (args.isDefined("ltl-mode")) {
          string ltlMode= args.getString("ltl-mode");
          if (ltlMode == "mine") {
            explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
          }
        }
      } else if (componentSelection == "subsets-fixed") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_SUBSET_FIXED);
        explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_FINITE);
        if (args.isDefined("fixed-subsets")) {
          string setsstring=args.getString("fixed-subsets");
          list<set<int> > intSets=Parse::integerSetList(setsstring);
          explorer.setFixedComponentSubsets(intSets);
        } else {
          logger[ERROR] << "selected a fixed set of components but no were selected. Please use option \"--fixed-subsets=<csv-id-list>\".";
          ROSE_ASSERT(0);
        }
      } else if (componentSelection == "subsets-random") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_SUBSET_RANDOM);
        if (args.isDefined("num-random-components")) {
          explorer.setNumberRandomComponents(args.getInt("num-random-components"));
        } else {
          explorer.setNumberRandomComponents(std::min(3, (int) cfgsAsVector.size()));
        }
        if (args.isDefined("different-component-subsets")) {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_FINITE);
          explorer.setNumberDifferentComponentSubsets(args.getInt("different-component-subsets"));
        } else {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
        }
      }
    } else {
      explorer.setComponentSelection(PAR_PRO_COMPONENTS_ALL);
      if (args.isDefined("ltl-mode")) {
        string ltlMode= args.getString("ltl-mode");
        if (ltlMode == "mine") {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
        }
      }
    }

    if ( args.isDefined("check-ltl") ) {
      explorer.setLtlMode(PAR_PRO_LTL_MODE_CHECK);
      explorer.setLtlInputFilename(args.getString("check-ltl"));
    } else {
      if ( args.isDefined("ltl-mode") ) {
        string ltlMode= args.getString("ltl-mode");
        if (ltlMode == "check") {
          logger[ERROR] << "ltl mode \"check\" selected but option \"--check-ltl=<filename>\" not used. Please provide LTL property file." << endl;
          ROSE_ASSERT(0);
        } else if (ltlMode == "mine") {
          explorer.setLtlMode(PAR_PRO_LTL_MODE_MINE);
          if (args.isDefined("num-components-ltl")) {
            explorer.setNumberOfComponentsForLtlAnnotations(args.getInt("num-components-ltl"));
          } else {
            explorer.setNumberOfComponentsForLtlAnnotations(std::min(3, (int) cfgsAsVector.size()));
          }
          if (args.isDefined("minimum-components")) {
            explorer.setMinNumComponents(args.getInt("minimum-components"));
          }
          if (args.isDefined("mine-num-verifiable")) {
            explorer.setNumRequiredVerifiable(args.getInt("mine-num-verifiable"));
          } else {
            explorer.setNumRequiredVerifiable(10);
          }
          if (args.isDefined("mine-num-falsifiable")) {
            explorer.setNumRequiredFalsifiable(args.getInt("mine-num-falsifiable"));
          } else {
            explorer.setNumRequiredFalsifiable(10);
          }
          if (args.isDefined("minings-per-subsets")) {
            explorer.setNumMiningsPerSubset(args.getInt("minings-per-subsets"));
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
  
    if (args.isDefined("check-ltl")) {
      PropertyValueTable* ltlResults=nullptr;
      if (args.getBool("promela-output-only")) { // just read the properties into a PropertyValueTable
        SpotConnection spotConnection(args.getString("check-ltl"));
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
    if (args.isDefined("promela-output")) {
      PropertyValueTable* ltlResults;
      if (args.getBool("promela-output-only")) { // just read the properties into a PropertyValueTable
        SpotConnection spotConnection(args.getString("check-ltl"));
        ltlResults = spotConnection.getLtlResults();
      } else {
        ltlResults = explorer.propertyValueTable();
      }
      // uses SpotMiscellaneous::spinSyntax as callback to avoid static dependency of ltlResults on SpotMisc.
      string promelaLtlFormulae = ltlResults->getLtlsAsPromelaCode(withResults, withAnnotations,&SpotMiscellaneous::spinSyntax);
      promelaCode += "\n" + promelaLtlFormulae;
      string filename = args.getString("promela-output");
      write_file(filename, promelaCode);
      cout << "generated " << filename  <<"."<<endl;
    }
#endif
    if (args.isDefined("ltl-properties-output")) {
      string ltlFormulae = explorer.propertyValueTable()->getLtlsRersFormat(withResults, withAnnotations);
      string filename = args.getString("ltl-properties-output");
      write_file(filename, ltlFormulae);
      cout << "generated " << filename  <<"."<<endl;
    }
    if(!args.isDefined("quiet"))
      cout << "STATUS: done." << endl;
  }

  void ParProAutomata::generateAutomata() {
    if (args.isDefined("seed")) {
      srand(args.getInt("seed"));
    } else {
      srand(time(NULL));
    }
    ParProAutomataGenerator automataGenerator;
    int numberOfAutomata = 10;
    if (args.isDefined("num-automata")) {
      numberOfAutomata = args.getInt("num-automata");
    }
    pair<int, int> numberOfSyncsRange = pair<int, int>(9, 18);
    if (args.isDefined("num-syncs-range")) {
      numberOfSyncsRange = parseCsvIntPair(args.getString("num-syncs-range"));
    }
    pair<int, int> numberOfCirclesPerAutomatonRange = pair<int, int>(2, 4);
    if (args.isDefined("num-circles-range")) {
      numberOfCirclesPerAutomatonRange = parseCsvIntPair(args.getString("num-circles-range"));
    }
    pair<int, int> circleLengthRange = pair<int, int>(5, 8);
    if (args.isDefined("circle-length-range")) {
      circleLengthRange = parseCsvIntPair(args.getString("circle-length-range"));
    }
    pair<int, int> numIntersectionsOtherCirclesRange = pair<int, int>(1, 2);
    if (args.isDefined("num-intersections-range")) {
      numIntersectionsOtherCirclesRange = parseCsvIntPair(args.getString("num-intersections-range"));
    }
    vector<Flow*> automata = automataGenerator.randomlySyncedCircleAutomata(
                                                                            numberOfAutomata,
                                                                            numberOfSyncsRange,
                                                                            numberOfCirclesPerAutomatonRange,
                                                                            circleLengthRange,
                                                                            numIntersectionsOtherCirclesRange);
    Visualizer visualizer;
    string dotCfas = visualizer.cfasToDotSubgraphs(automata);
    string outputFilename = args.getString("generate-automata");
    write_file(outputFilename, dotCfas);
    cout << "generated " << outputFilename <<"."<<endl;
  }
}
