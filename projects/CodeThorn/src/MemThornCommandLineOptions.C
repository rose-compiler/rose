#include "sage3basic.h"
#include "CodeThornException.h"
#include "MemThornCommandLineOptions.h"
#include "CppStdUtilities.h"

#include <string>
#include <sstream>
#include <iostream>

// required for checking of: HAVE_SPOT, HAVE_Z3
#include "rose_config.h"

#include "Diagnostics.h"
using namespace Sawyer::Message;
using namespace std;
using namespace CodeThorn;

void checkSpotOptions(LTLOptions& ltlOpt, ParProOptions& parProOpt) {
    // Check if chosen options are available
#ifndef HAVE_SPOT
    // display error message and exit in case SPOT is not avaiable, but related options are selected
  if (ltlOpt.activeOptionsRequireSPOTLibrary()
      || parProOpt.activeOptionsRequireSPOTLibrary()) {
    cerr << "Error: Options selected that require the SPOT library, however SPOT was not selected during configuration." << endl;
    exit(1);
  }
#endif
}

void checkZ3Options(CodeThornOptions& ctOpt) {
#ifndef HAVE_Z3
  if(ctOpt.activeOptionsRequireZ3Library()) {
    cerr << "Error: Options selected that require the Z3 library, however Z3 was not selected during configuration." << endl;
    exit(1);
  }
#endif	
}

CodeThorn::CommandLineOptions& parseCommandLine(int argc, char* argv[], Sawyer::Message::Facility logger, std::string version,
                                                CodeThornOptions& ctOpt, LTLOptions& ltlOpt, ParProOptions& parProOpt) {

  // Command line option handling.
  po::options_description visibleOptions("Supported options");
  po::options_description hiddenOptions("Hidden options");
  po::options_description passOnToRoseOptions("Options passed on to ROSE frontend");
  po::options_description experimentalOptions("Experimental options");
  po::options_description visualizationOptions("Visualization options");
  po::options_description infoOptions("Program information options");

  passOnToRoseOptions.add_options()
    (",I", po::value< vector<string> >(&ctOpt.includeDirs),"Include directories.")
    (",D", po::value< vector<string> >(&ctOpt.preProcessorDefines),"Define constants for preprocessor.")
    ("edg:no_warnings", po::bool_switch(&ctOpt.edgNoWarningsFlag),"EDG frontend flag.")
    ("rose:ast:read", po::value<std::string>(&ctOpt.roseAstReadFileName),"read in binary AST from comma separated list (no spaces)")
    ;

  visualizationOptions.add_options()
    ("tg1-estate-address", po::value< bool >(&ctOpt.visualization.tg1EStateAddress)->default_value(false)->implicit_value(true), "Transition graph 1: Visualize address.")
    ("tg1-estate-id", po::value< bool >(&ctOpt.visualization.tg1EStateId)->default_value(true)->implicit_value(true), "Transition graph 1: Visualize estate-id.")
    ("tg1-estate-properties", po::value< bool >(&ctOpt.visualization.tg1EStateProperties)->default_value(true)->implicit_value(true), "Transition graph 1: Visualize all estate-properties.")
    ("tg1-estate-predicate", po::value< bool >(&ctOpt.visualization.tg1EStatePredicate)->default_value(false)->implicit_value(true), "Transition graph 1: Show estate as predicate.")
    ("tg1-estate-memory-subgraphs", po::value< bool >(&ctOpt.visualization.tg1EStateMemorySubgraphs)->default_value(false)->implicit_value(true), "Transition graph 1: Show estate as memory graphs.")
    ("tg2-estate-address", po::value< bool >(&ctOpt.visualization.tg2EStateAddress)->default_value(false)->implicit_value(true), "Transition graph 2: Visualize address.")
    ("tg2-estate-id", po::value< bool >(&ctOpt.visualization.tg2EStateId)->default_value(true)->implicit_value(true), "Transition graph 2: Visualize estate-id.")
    ("tg2-estate-properties", po::value< bool >(&ctOpt.visualization.tg2EStateProperties)->default_value(false)->implicit_value(true),"Transition graph 2: Visualize all estate-properties.")
    ("tg2-estate-predicate", po::value< bool >(&ctOpt.visualization.tg2EStatePredicate)->default_value(false)->implicit_value(true), "Transition graph 2: Show estate as predicate.")
    ("viz", po::value< bool >(&ctOpt.visualization.viz)->default_value(false)->implicit_value(true),"Generate visualizations of AST, CFG, and transition system as dot files (ast.dot, cfg.dot, transitiongraph1/2.dot.")
    ("viz-tg2", po::value< bool >(&ctOpt.visualization.vizTg2)->default_value(false)->implicit_value(true),"Generate transition graph 2 (.dot).")
    ("cfg", po::value< string >(&ctOpt.visualization.icfgFileName), "Generate inter-procedural cfg as dot file. Each function is visualized as one dot cluster.")
    ;

  experimentalOptions.add_options()
    ("normalize-all", po::value< bool >(&ctOpt.normalizeAll)->default_value(true)->implicit_value(true),"Normalize all expressions before analysis.")
    ("normalize-fcalls", po::value< bool >(&ctOpt.normalizeFCalls)->default_value(false)->implicit_value(true),"Normalize only expressions with function calls.")
    ("eliminate-compound-assignments", po::value< bool >(&ctOpt.eliminateCompoundStatements)->default_value(true)->implicit_value(true),"Replace all compound-assignments by assignments.")
    ("arrays-not-in-state", po::value< bool >(&ctOpt.arraysNotInState)->default_value(false)->implicit_value(true),"Arrays are not represented in state. Only correct if all arrays are read-only (manual optimization - to be eliminated).")
    ("in-state-string-literals",po::value< bool >(&ctOpt.inStateStringLiterals)->default_value(true)->implicit_value(true),"create string literals in initial state.")
    ("std-functions",po::value< bool >(&ctOpt.stdFunctions)->default_value(true)->implicit_value(true),"model std function semantics (malloc, memcpy, etc). Must be turned off explicitly.")
    ("ignore-function-pointers",po::value< bool >(&ctOpt.ignoreFunctionPointers)->default_value(false)->implicit_value(true), "Unknown functions are assumed to be side-effect free.")
    ("ignore-undefined-dereference",po::value< bool >(&ctOpt.ignoreUndefinedDereference)->default_value(false)->implicit_value(true), "Ignore pointer dereference of uninitalized value (assume data exists).")
    ("ignore-unknown-functions",po::value< bool >(&ctOpt.ignoreUnknownFunctions)->default_value(true)->implicit_value(true), "Ignore function pointers (functions are not called).")
    ("function-resolution-mode",po::value< int >(&ctOpt.functionResolutionMode)->default_value(4),"1:Translation unit only, 2:slow lookup, 3: -, 4: complete resolution (including function pointers)")
    ("context-sensitive",po::value< bool >(&ctOpt.contextSensitive)->default_value(true)->implicit_value(true),"Perform context sensitive analysis. Uses call strings with arbitrary length, recursion is not supported yet.")
    ("abstraction-mode",po::value< int >(&ctOpt.abstractionMode)->default_value(1),"Select abstraction mode (0: equality merge (explicit model checking), 1: approximating merge (abstract model checking).")
    ("interpreter-mode",po::value< int >(&ctOpt.interpreterMode)->default_value(0),"Select interpretation mode. 0: default, 1: execute stdout functions.")
    ("interpreter-mode-file",po::value< string >(&ctOpt.interpreterModeOuputFileName)->default_value(""),"Select interpretation mode output file (otherwise stdout is used).")
    ("print-warnings",po::value< bool >(&ctOpt.printWarnings)->default_value(false)->implicit_value(true),"Print warnings on stdout during analysis (this can slow down the analysis significantly)")
    ("print-violations",po::value< bool >(&ctOpt.printViolations)->default_value(false)->implicit_value(true),"Print detected violations on stdout during analysis (this can slow down the analysis significantly)")
    ("options-set",po::value< int >(&ctOpt.optionsSet)->default_value(2)->implicit_value(0),"Use a predefined set of default options (2:default|1..3:abstract)|11:concrete)).")
    ("callstring-length",po::value< int >(&ctOpt.callStringLength)->default_value(10),"Set the length of the callstring for context-sensitive analysis. Default value is 10.")
    ("byte-mode", po::value< bool >(&ctOpt.byteMode)->default_value(false)->implicit_value(true),"switches from index-based addresses to byte-based addresses in state representation.")
    ("test-selector",po::value< int >(&ctOpt.testSelector)->default_value(0)->implicit_value(0),"Option for selecting dev tests.")
    ;

  visibleOptions.add_options()            
    ("config,c", po::value< string >(&ctOpt.configFileName), "Use the configuration specified in file <arg>.")
    ("colors", po::value< bool >(&ctOpt.colors)->default_value(true)->implicit_value(true),"Use colors in output.")
    ("quiet", po::value< bool >(&ctOpt.quiet)->default_value(false)->implicit_value(true), "Produce no output on screen.")
    ("help,h", "Produce this help message.")
    ("help-all", "Show all help options.")
    ("help-rose", "Show options that can be passed to ROSE.")
    ("help-exp", "Show options for experimental features.")
    ("help-vis", "Show options for visualization output files.")
    ("help-info", "Show options for program info.")
    ("start-function", po::value< string >(&ctOpt.startFunctionName), "Name of function to start the analysis from.")
    ("null-pointer-analysis",po::value< bool >(&ctOpt.nullPointerAnalysis)->default_value(true)->implicit_value(true),"Perform null pointer analysis and print results.")
    ("out-of-bounds-analysis",po::value< bool >(&ctOpt.outOfBoundsAnalysis)->default_value(true)->implicit_value(true),"Perform out-of-bounds analysis and print results.")
    ("uninitialized-analysis",po::value< bool >(&ctOpt.uninitializedMemoryAnalysis)->default_value(true)->implicit_value(true),"Perform uninitialized analysis and print results.")
    ("null-pointer-analysis-file",po::value< string >(&ctOpt.nullPointerAnalysisFileName),"Perform null pointer analysis and write results to file [arg].")
    ("out-of-bounds-analysis-file",po::value< string >(&ctOpt.outOfBoundsAnalysisFileName),"Perform out-of-bounds analysis and write results to file [arg].")
    ("uninitialized-analysis-file",po::value< string >(&ctOpt.uninitializedMemoryAnalysisFileName),"Perform uninitialized analysis and write results to file [arg].")
    ("program-stats-only",po::value< bool >(&ctOpt.programStatsOnly)->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs and exit.")
    ("program-stats",po::value< bool >(&ctOpt.programStats)->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs.")
    ("external-function-calls-file",po::value< string >(&ctOpt.externalFunctionCallsFileName), "write a list of all function calls to external functions (functions for which no implementation exists) to a CSV file.")
    ("status", po::value< bool >(&ctOpt.status)->default_value(false)->implicit_value(true), "Show status messages.")
    ("reduce-cfg", po::value< bool >(&ctOpt.reduceCfg)->default_value(true)->implicit_value(true), "Reduce CFG nodes that are irrelevant for the analysis.")
    ("internal-checks", po::value< bool >(&ctOpt.internalChecks)->default_value(false)->implicit_value(true), "Run internal consistency checks (without input program).")
    ("cl-args",po::value< string >(&ctOpt.analyzedProgramCLArgs),"Specify command line options for the analyzed program (as one quoted string).")
    ("log-level",po::value< string >(&ctOpt.logLevel)->default_value("none,>=error"),"Set the log level (\"x,>=y\" with x,y in: (none|info|warn|trace|error|fatal|debug)).")
    ("analyzed-functions-csv",po::value<std::string>(&ctOpt.analyzedFunctionsCSVFileName),"Write list of analyzed functions to CSV file [arg].")
    ("analyzed-files-csv",po::value<std::string>(&ctOpt.analyzedFilesCSVFileName),"Write list of analyzed files (with analyzed functions) to CSV file [arg].")
    ("external-functions-csv",po::value<std::string>(&ctOpt.externalFunctionsCSVFileName),"Write list of external functions to CSV file [arg].")
    ("unparse",po::value< bool >(&ctOpt.unparse)->default_value(false)->implicit_value(true),"unpare code (only relevant for inlining, normalization, and lowering)")
    ("version,v",po::value< bool >(&ctOpt.displayVersion)->default_value(false)->implicit_value(true), "Display the program version.")
    ;

  infoOptions.add_options()
    ("print-variable-id-mapping",po::value< bool >(&ctOpt.info.printVariableIdMapping)->default_value(false)->implicit_value(true),"Print variable-id-mapping on stdout.")
    ("print-function-id-mapping",po::value< bool >(&ctOpt.info.printFunctionIdMapping)->default_value(false)->implicit_value(true),"Print function-id-mapping on stdout.")
    ("ast-stats-print",po::value< bool >(&ctOpt.info.printAstNodeStats)->default_value(false)->implicit_value(true),"Print ast node statistics on stdout.")
    ("ast-stats-csv",po::value< string >(&ctOpt.info.astNodeStatsCSVFileName),"Write ast node statistics to CSV file [arg].")
    ("type-size-mapping-print",po::value< bool >(&ctOpt.info.printTypeSizeMapping)->default_value(false)->implicit_value(true),"Print type-size mapping on stdout.")
    ("type-size-mapping-csv",po::value<std::string>(&ctOpt.info.typeSizeMappingCSVFileName),"Write type-size mapping to CSV file [arg].")
    ;

  po::options_description all("All supported options");
  all.add(visibleOptions)
    .add(passOnToRoseOptions)
    .add(experimentalOptions)
    .add(visualizationOptions)
    .add(infoOptions)
    ;

  po::options_description configFileOptions("Configuration file options");
  configFileOptions.add(visibleOptions)
    //    .add(passOnToRoseOptions) [cannot be used in config file]
    .add(experimentalOptions)
    .add(visualizationOptions)
    .add(infoOptions)
    ;

  args.parse(argc,argv,all,configFileOptions);

  if (args.isUserProvided("help")) {
    cout << visibleOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-all")) {
    cout << all << "\n";
    exit(0);
  } else if(args.isUserProvided("help-exp")) {
    cout << experimentalOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-rose")) {
    cout << passOnToRoseOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-vis")) {
    cout << visualizationOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-info")) {
    cout << infoOptions << "\n";
    exit(0);
  } else if(ctOpt.displayVersion) {
    cout << "MemThorn version "<<version<<endl;
    cout << "Written by Markus Schordan\n";
    exit(0);
  }

  // Additional checks for options passed on to the ROSE frontend.
  // "-std" is a short option with long name. Check that it still has an argument if used.
  // deactivated  // "-I" should either be followed by a whitespace or by a slash
  for (int i=1; i < argc; ++i) {
    string currentArg(argv[i]);
    if (currentArg == "-std") {
      logger[ERROR] << "Option \"-std\" requires an argument." << endl;
      ROSE_ASSERT(0);
    }
  }

  // Remove all CodeThorn-specific elements of argv (do not confuse ROSE frontend)
  for (int i=1; i < argc; ++i) {
    string currentArg(argv[i]);
    if (currentArg == "--rose:ast:read"){
      argv[i] = strdup("");
      ROSE_ASSERT(i+1<argc);
      argv[i+1]= strdup("");
      continue;
    }
    if (currentArg[0] != '-' ){
      continue;  // not an option      
    }
    // explicitly keep options relevant to the ROSE frontend (white list) 
    else if (currentArg == "-I") {
      assert(i+1<argc);
      ++i;
      continue;
    } else if (currentArg == "--edg:no_warnings") {
      continue;
    } else {
      string iPrefix = "-I/";
      string dPrefix = "-D"; // special case, cannot contain separating space
      if(currentArg.substr(0, iPrefix.size()) == iPrefix) {
        continue;
      }
      if(currentArg.substr(0, dPrefix.size()) == dPrefix) {
        continue;
      }
    }
    // No match with elements in the white list above. 
    // Must be a CodeThorn option, therefore remove it from argv.
    argv[i] = strdup("");
  }

  return args;
}

