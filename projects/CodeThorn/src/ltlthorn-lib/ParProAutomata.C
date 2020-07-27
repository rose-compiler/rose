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
  bool ParProAutomata::handleCommandLineArguments(ParProOptions& parProOpt, CodeThornOptions& ctOpt, LTLOptions& ltlOpt, Sawyer::Message::Facility& logger) {
    // ParPro command line options
    if(parProOpt.generateAutomata.size()>0) {
      CodeThorn::ParProAutomata::generateAutomata(parProOpt);
      return true;
    }
    if(parProOpt.automataDotInput.size()>0) {
      CodeThorn::ParProAutomata::automataDotInput(parProOpt, ctOpt, ltlOpt, logger);
      return true;
    }
    return false;
  }

  void ParProAutomata::automataDotInput(ParProOptions& parProOpt, CodeThornOptions& ctOpt, LTLOptions& ltlOpt, Sawyer::Message::Facility logger) {
    if (parProOpt.seed!=-1) {
      srand(parProOpt.seed);
    } else {
      srand(time(NULL));
    }
    DotGraphCfgFrontend dotGraphCfgFrontend;
    string filename = parProOpt.automataDotInput;
    CfgsAndAnnotationMap cfgsAndMap = dotGraphCfgFrontend.parseDotCfgs(filename);
    list<Flow> cfgs = cfgsAndMap.first;
    EdgeAnnotationMap edgeAnnotationMap = cfgsAndMap.second;

    string promelaCode;
    if (parProOpt.promelaOutput.size()>0) {
      cout << "STATUS: generating PROMELA code (parallel processes based on CFG automata)..." << endl;
      PromelaCodeGenerator codeGenerator;
      promelaCode = codeGenerator.generateCode(cfgsAndMap);
      cout << "STATUS: done (LTLs not added yet)." << endl;
    }

    if (ctOpt.visualization.viz) {
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
    if (parProOpt.verificationEngine.size()>0) {
      string verificationEngine = parProOpt.verificationEngine;
      if (verificationEngine == "ltsmin") {
        explorer.setUseLtsMin(true);
      }
    } 
    if (parProOpt.keepSystems) {
      explorer.setStoreComputedSystems(true);
    } else {
      explorer.setStoreComputedSystems(false);
    }
    if (parProOpt.parallelCompositionOnly) {
      explorer.setParallelCompositionOnly(true);
    } else {
      explorer.setStoreComputedSystems(false);
    }
    if (parProOpt.useComponents.size()>0) {
      string componentSelection = parProOpt. useComponents;
      if (componentSelection == "all") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_ALL);
        if (parProOpt.ltlMode.size()>0) {
          string ltlMode=parProOpt.ltlMode;
          if (ltlMode == "mine") {
            explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
          }
        }
      } else if (componentSelection == "subsets-fixed") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_SUBSET_FIXED);
        explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_FINITE);
        if (parProOpt.fixedSubsets.size()>0) {
          string setsstring=parProOpt.fixedSubsets;
          list<set<int> > intSets=Parse::integerSetList(setsstring);
          explorer.setFixedComponentSubsets(intSets);
        } else {
          logger[ERROR] << "selected a fixed set of components but no were selected. Please use option \"--fixed-subsets=<csv-id-list>\".";
          ROSE_ASSERT(0);
        }
      } else if (componentSelection == "subsets-random") {
        explorer.setComponentSelection(PAR_PRO_COMPONENTS_SUBSET_RANDOM);
        if (parProOpt.numRandomComponents!=-1) {
          explorer.setNumberRandomComponents(parProOpt.numRandomComponents);
        } else {
          explorer.setNumberRandomComponents(std::min(3, (int) cfgsAsVector.size()));
        }
        if (parProOpt.differentComponentSubsets!=-1) {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_FINITE);
          explorer.setNumberDifferentComponentSubsets(parProOpt.differentComponentSubsets);
        } else {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
        }
      }
    } else {
      explorer.setComponentSelection(PAR_PRO_COMPONENTS_ALL);
      if (parProOpt.ltlMode.size()>0) {
        string ltlMode= parProOpt.ltlMode;
        if (ltlMode == "mine") {
          explorer.setRandomSubsetMode(PAR_PRO_NUM_SUBSETS_INFINITE);
        }
      }
    }

    if ( ltlOpt.ltlFormulaeFile.size()>0 ) {
      explorer.setLtlMode(PAR_PRO_LTL_MODE_CHECK);
      explorer.setLtlInputFilename(ltlOpt.ltlFormulaeFile);
    } else {
      if (parProOpt.ltlMode.size()>0) {
        string ltlMode= parProOpt.ltlMode;
        if (ltlMode == "check") {
          logger[ERROR] << "ltl mode \"check\" selected but option \"--check-ltl=<filename>\" not used. Please provide LTL property file." << endl;
          ROSE_ASSERT(0);
        } else if (ltlMode == "mine") {
          explorer.setLtlMode(PAR_PRO_LTL_MODE_MINE);
          if (parProOpt.numComponentsLtl!=-1) {
            explorer.setNumberOfComponentsForLtlAnnotations(parProOpt.numComponentsLtl);
          } else {
            explorer.setNumberOfComponentsForLtlAnnotations(std::min(3, (int) cfgsAsVector.size()));
          }
          if (parProOpt.minimumComponents!=-1) {
            explorer.setMinNumComponents(parProOpt.minimumComponents);
          }
          if (parProOpt.mineNumVerifiable!=-1) {
            explorer.setNumRequiredVerifiable(parProOpt.mineNumVerifiable);
          } else {
            explorer.setNumRequiredVerifiable(10);
          }
          if (parProOpt.mineNumFalsifiable!=-1) {
            explorer.setNumRequiredFalsifiable(parProOpt.mineNumFalsifiable);
          } else {
            explorer.setNumRequiredFalsifiable(10);
          }
          if (parProOpt.miningsPerSubset!=-1) {
            explorer.setNumMiningsPerSubset(parProOpt.miningsPerSubset);
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

    if (ctOpt.visualization.viz) {
      explorer.setVisualize(true);
    }

    if (!parProOpt.promelaOutputOnly) {
      explorer.explore();
    }
  
    if (ltlOpt.ltlFormulaeFile.size()>0) {
      PropertyValueTable* ltlResults=nullptr;
      if (parProOpt.promelaOutputOnly) { // just read the properties into a PropertyValueTable
        SpotConnection spotConnection(ltlOpt.ltlFormulaeFile);
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

    bool withResults = parProOpt.outputWithResults;
    bool withAnnotations = parProOpt.outputWithAnnotations;
#ifdef HAVE_SPOT
    if (parProOpt.promelaOutput.size()>0) {
      PropertyValueTable* ltlResults;
      if (parProOpt.promelaOutputOnly) { // just read the properties into a PropertyValueTable; MS 2020: this may need to be flipped?
        SpotConnection spotConnection(ltlOpt.ltlFormulaeFile);
        ltlResults = spotConnection.getLtlResults();
      } else {
        ltlResults = explorer.propertyValueTable();
      }
      // uses SpotMiscellaneous::spinSyntax as callback to avoid static dependency of ltlResults on SpotMisc.
      string promelaLtlFormulae = ltlResults->getLtlsAsPromelaCode(withResults, withAnnotations,&SpotMiscellaneous::spinSyntax);
      promelaCode += "\n" + promelaLtlFormulae;
      string filename = parProOpt.promelaOutput;
      write_file(filename, promelaCode);
      cout << "generated " << filename  <<"."<<endl;
    }
#endif
    if (parProOpt.ltlPropertiesOutput.size()>0) {
      string ltlFormulae = explorer.propertyValueTable()->getLtlsRersFormat(withResults, withAnnotations);
      string filename = parProOpt.ltlPropertiesOutput;
      write_file(filename, ltlFormulae);
      cout << "generated " << filename  <<"."<<endl;
    }
    if(!ctOpt.quiet)
      cout << "STATUS: done." << endl;
  }

  void ParProAutomata::generateAutomata(ParProOptions& parProOpt) {
    if (parProOpt.seed!=-1) {
      srand(parProOpt.seed);
    } else {
      srand(time(NULL));
    }
    ParProAutomataGenerator automataGenerator;
    int numberOfAutomata = 10;
    if (parProOpt.numAutomata!=-1) {
      numberOfAutomata = parProOpt.numAutomata;
    }
    pair<int, int> numberOfSyncsRange = pair<int, int>(9, 18);
    if (parProOpt.numSyncsRange.size()>0) {
      numberOfSyncsRange = parseCsvIntPair(parProOpt.numSyncsRange);
    }
    pair<int, int> numberOfCirclesPerAutomatonRange = pair<int, int>(2, 4);
    if (parProOpt.numCirclesRange.size()>0) {
      numberOfCirclesPerAutomatonRange = parseCsvIntPair(parProOpt.numCirclesRange);
    }
    pair<int, int> circleLengthRange = pair<int, int>(5, 8);
    if (parProOpt.circlesLengthRange.size()>0) {
      circleLengthRange = parseCsvIntPair(parProOpt.circlesLengthRange);
    }
    pair<int, int> numIntersectionsOtherCirclesRange = pair<int, int>(1, 2);
    if (parProOpt.numIntersectionsRange.size()>0) {
      numIntersectionsOtherCirclesRange = parseCsvIntPair(parProOpt.numIntersectionsRange);
    }
    vector<Flow*> automata = automataGenerator.randomlySyncedCircleAutomata(
                                                                            numberOfAutomata,
                                                                            numberOfSyncsRange,
                                                                            numberOfCirclesPerAutomatonRange,
                                                                            circleLengthRange,
                                                                            numIntersectionsOtherCirclesRange);
    Visualizer visualizer;
    string dotCfas = visualizer.cfasToDotSubgraphs(automata);
    string outputFilename = parProOpt.generateAutomata;
    write_file(outputFilename, dotCfas);
    cout << "generated " << outputFilename <<"."<<endl;
  }
}
