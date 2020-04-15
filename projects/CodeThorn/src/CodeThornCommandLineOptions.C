#include "sage3basic.h"
#include "CodeThornException.h"
#include "CodeThornCommandLineOptions.h"

#include <string>
#include <sstream>
#include <iostream>

// required for checking of: HAVE_SPOT, HAVE_Z3
#include "rose_config.h"

#include "CodeThornOptions.h"

// to be move into other tool
#include "LTLOptions.h"
// to be move into other tool
#include "ltlthorn-lib/ParProOptions.h"

#include "Diagnostics.h"
using namespace Sawyer::Message;
using namespace std;
using namespace CodeThorn;

void checkSpotOptions() {
    // Check if chosen options are available
#ifndef HAVE_SPOT
    // display error message and exit in case SPOT is not avaiable, but related options are selected
    if (args.isUserProvided("csv-stats-cegpra") ||
	args.isUserProvided("cegpra-ltl") ||
	args.getBool("cegpra-ltl-all") ||
	args.isUserProvided("cegpra-max-iterations") ||
	args.isUserProvided("viz-cegpra-detailed") ||
	args.isUserProvided("csv-spot-ltl") ||
	args.isUserProvided("check-ltl") ||
	args.isUserProvided("single-property") ||
	args.isUserProvided("ltl-in-alphabet") ||
	args.isUserProvided("ltl-out-alphabet") ||
	args.getBool("ltl-driven") ||
	args.getBool("with-ltl-counterexamples") ||
	args.isUserProvided("mine-num-verifiable") ||
	args.isUserProvided("mine-num-falsifiable") ||
	args.isUserProvided("ltl-mode") ||
	args.isUserProvided("ltl-properties-output") ||
	args.isUserProvided("promela-output") ||
	args.getBool("promela-output-only") ||
	args.getBool("output-with-results") ||
	args.getBool("output-with-annotations")) {
      cerr << "Error: Options selected that require the SPOT library, however SPOT was not selected during configuration." << endl;
      exit(1);
    }
#endif
}

void checkZ3Options() {
#ifndef HAVE_Z3
    if (args.isUserProvided("z3") ||
	args.isUserProvided("rers-upper-input-bound") ||
	args.isUserProvided("rers-verifier-error-number")){
      cerr << "Error: Options selected that require the Z3 library, however Z3 was not selected during configuration." << endl;
      exit(1);
    }
#endif	
}

CodeThorn::CommandLineOptions& parseCommandLine(int argc, char* argv[], Sawyer::Message::Facility logger, std::string version) {
  CodeThornOptions ctOpt;
  LTLOptions ltlOpt; // to be moved into separate tool
  ParProOptions parProOpt; // to be moved into separate tool

  // Command line option handling.
  po::options_description visibleOptions("Supported options");
  po::options_description hiddenOptions("Hidden options");
  po::options_description passOnToRose("Options passed on to ROSE frontend");
  po::options_description cegpraOptions("CEGPRA options");
  po::options_description ltlOptions("LTL options");
  po::options_description svcompOptions("SV-Comp options");
  po::options_description rersOptions("RERS options");
  po::options_description patternSearchOptions("RERS options");
  po::options_description equivalenceCheckingOptions("Equivalence checking options");
  po::options_description parallelProgramOptions("Analysis options for parallel programs");
  po::options_description dataRaceOptions("Data race detection options");
  po::options_description experimentalOptions("Experimental options");
  po::options_description visualizationOptions("Visualization options");
  po::options_description infoOptions("Program information options");

  ltlOptions.add_options()
    ("csv-spot-ltl", po::value< string >(&ltlOpt.spotVerificationResultsCSVFileName), "Output SPOT's LTL verification results into a CSV file <arg>.")
    ("csv-stats-size-and-ltl",po::value< string >(&ltlOpt.ltlStatisticsCSVFileName),"Output statistics regarding the final model size and results for LTL properties into a CSV file <arg>.")
    ("check-ltl", po::value< string >(&ltlOpt.ltlFormulaeFile), "Take a text file of LTL I/O formulae <arg> and check whether or not the analyzed program satisfies these formulae. Formulae should start with '('. Use \"csv-spot-ltl\" option to specify an output csv file for the results.")
    ("single-property", po::value< int >(&ltlOpt.propertyNrToCheck), "Number (ID) of the property that is supposed to be analyzed. All other LTL properties will be ignored. ( Use \"check-ltl\" option to specify a input property file).")
    ("counterexamples-with-output", po::value< bool >(&ltlOpt.counterExamplesWithOutput)->default_value(false)->implicit_value(true), "Reported counterexamples for LTL or reachability properties also include output values.")
    ("inf-paths-only", po::value< bool >(&ltlOpt.inifinitePathsOnly)->default_value(false)->implicit_value(true), "Recursively prune the transition graph so that only infinite paths remain when checking LTL properties.")
    ("io-reduction", po::value< int >(&ltlOpt.ioReduction), "(work in progress) IO reduction threshold. Reduce the transition system to only input/output/worklist states after every <arg> computed EStates.")
    ("keep-error-states",  po::value< bool >(&ltlOpt.keepErrorStates)->default_value(false)->implicit_value(true), "Do not reduce error states for the LTL analysis.")      
    ("ltl-in-alphabet",po::value< string >(&ltlOpt.ltlInAlphabet),"Specify an input alphabet used by the LTL formulae. (e.g. \"{1,2,3}\")")
    ("ltl-out-alphabet",po::value< string >(&ltlOpt.ltlOutAlphabet),"Specify an output alphabet used by the LTL formulae. (e.g. \"{19,20,21,22,23,24,25,26}\")")
    ("ltl-driven", po::value< bool >(&ltlOpt.ltlDriven)->default_value(false)->implicit_value(true), "Select mode to verify LTLs driven by SPOT's access to the state transitions.")
    ("reset-analyzer", po::value< bool >(&ltlOpt.resetAnalyzer)->default_value(true)->implicit_value(true), "Reset the analyzer and therefore the state transition graph before checking the next property. Only affects ltl-driven mode.")
    ("no-input-input",  po::value< bool >(&ltlOpt.noInputInputTransitions)->default_value(false)->implicit_value(true), "(deprecated) remove transitions where one input states follows another without any output in between. Removal occurs before the LTL check. [yes|=no]")
    ("std-io-only", po::value< bool >(&ltlOpt.stdIOOnly)->default_value(false)->implicit_value(true), "Bypass and remove all states that are not standard I/O.")
    ("with-counterexamples", po::value< bool >(&ltlOpt.withCounterExamples)->default_value(false)->implicit_value(true), "Add counterexample I/O traces to the analysis results. Applies to reachable assertions and falsified LTL properties (uses RERS-specific alphabet).")
    ("with-assert-counterexamples", po::value< bool >(&ltlOpt.withAssertCounterExamples)->default_value(false)->implicit_value(true), "Report counterexamples leading to failing assertion states.")
    ("with-ltl-counterexamples", po::value< bool >(&ltlOpt.withLTLCounterExamples)->default_value(false)->implicit_value(true), "Report counterexamples that violate LTL properties.")
    ;

  hiddenOptions.add_options()
    ("max-transitions-forced-top1",po::value< int >(&ctOpt.maxTransitionsForcedTop1),"Performs approximation after <arg> transitions (only exact for input,output).")
    ("max-transitions-forced-top2",po::value< int >(&ctOpt.maxTransitionsForcedTop2),"Performs approximation after <arg> transitions (only exact for input,output,df).")
    ("max-transitions-forced-top3",po::value< int >(&ctOpt.maxTransitionsForcedTop3),"Performs approximation after <arg> transitions (only exact for input,output,df,ptr-vars).")
    ("max-transitions-forced-top4",po::value< int >(&ctOpt.maxTransitionsForcedTop4),"Performs approximation after <arg> transitions (exact for all but inc-vars).")
    ("max-transitions-forced-top5",po::value< int >(&ctOpt.maxTransitionsForcedTop5),"Performs approximation after <arg> transitions (exact for input,output,df and vars with 0 to 2 assigned values)).")
    ("solver",po::value< int >(&ctOpt.solver)->default_value(5),"Set solver <arg> to use (one of 1,2,3,...).")
    ;

  passOnToRose.add_options()
    (",I", po::value< vector<string> >(&ctOpt.includeDirs),"Include directories.")
    (",D", po::value< vector<string> >(&ctOpt.preProcessorDefines),"Define constants for preprocessor.")
    (",std", po::value< string >(&ctOpt.languageStandard),"Compilation standard.")
    ("edg:no_warnings", po::bool_switch(&ctOpt.edgNoWarningsFlag),"EDG frontend flag.")
    ;

  cegpraOptions.add_options()
    ("csv-stats-cegpra",po::value< string >(&ltlOpt.cegpra.csvStats),"Output statistics regarding the counterexample-guided prefix refinement analysis (CEGPRA) into a CSV file <arg>.")
    ("cegpra-ltl",po::value< int >(&ltlOpt.cegpra.ltlPropertyNr),"Select the ID of an LTL property that should be checked using cegpra (between 0 and 99).")
    ("cegpra-ltl-all", po::value< bool >(&ltlOpt.cegpra.checkAllProperties)->default_value(false)->implicit_value(true),"Check all specified LTL properties using CEGPRA.")
    ("cegpra-max-iterations",po::value< int >(&ltlOpt.cegpra.maxIterations),"Select a maximum number of counterexamples anaylzed by CEGPRA.")
    ("viz-cegpra-detailed",po::value< string >(&ltlOpt.cegpra.visualizationDotFile),"Generate visualization (.dot) output files with prefix <arg> for different stages within each loop of CEGPRA.")
    ;

  visualizationOptions.add_options()
    ("rw-clusters", po::value< bool >(&ctOpt.visualization.rwClusters)->default_value(false)->implicit_value(true), "Draw boxes around data elements from the same array (read/write-set graphs).")      
    ("rw-data", po::value< bool >(&ctOpt.visualization.rwData)->default_value(false)->implicit_value(true), "Display names of data elements (read/write-set graphs).") 
    ("rw-highlight-races", po::value< bool >(&ctOpt.visualization.rwHighlightRaces)->default_value(false)->implicit_value(true), "Highlight data races as large red dots (read/write-set graphs).") 
    ("dot-io-stg", po::value< string >(&ctOpt.visualization. dotIOStg), "Output STG with explicit I/O node information in dot file <arg>.")
    ("dot-io-stg-forced-top", po::value< string >(&ctOpt.visualization.dotIOStgForcedTop), "Output STG with explicit I/O node information in dot file <arg>. Groups abstract states together.")
    ("tg1-estate-address", po::value< bool >(&ctOpt.visualization. tg1EStateAddress)->default_value(false)->implicit_value(true), "Transition graph 1: Visualize address.")
    ("tg1-estate-id", po::value< bool >(&ctOpt.visualization.tg1EStateId)->default_value(true)->implicit_value(true), "Transition graph 1: Visualize estate-id.")
    ("tg1-estate-properties", po::value< bool >(&ctOpt.visualization.tg1EStateProperties)->default_value(true)->implicit_value(true), "Transition graph 1: Visualize all estate-properties.")
    ("tg1-estate-predicate", po::value< bool >(&ctOpt.visualization.tg1EStatePredicate)->default_value(false)->implicit_value(true), "Transition graph 1: Show estate as predicate.")
    ("tg1-estate-memory-subgraphs", po::value< bool >(&ctOpt.visualization.tg1EStateMemorySubgraphs)->default_value(false)->implicit_value(true), "Transition graph 1: Show estate as memory graphs.")
    ("tg2-estate-address", po::value< bool >(&ctOpt.visualization.tg2EStateAddress)->default_value(false)->implicit_value(true), "Transition graph 2: Visualize address.")
    ("tg2-estate-id", po::value< bool >(&ctOpt.visualization.tg2EStateId)->default_value(true)->implicit_value(true), "Transition graph 2: Visualize estate-id.")
    ("tg2-estate-properties", po::value< bool >(&ctOpt.visualization.tg2EStateProperties)->default_value(false)->implicit_value(true),"Transition graph 2: Visualize all estate-properties.")
    ("tg2-estate-predicate", po::value< bool >(&ctOpt.visualization.tg2EStatePredicate)->default_value(false)->implicit_value(true), "Transition graph 2: Show estate as predicate.")
    ("visualize-read-write-sets", po::value< bool >(&ctOpt.visualization.visualizeRWSets)->default_value(false)->implicit_value(true), "Generate a read/write-set graph that illustrates the read and write accesses of the involved threads.")
    ("viz", po::value< bool >(&ctOpt.visualization.viz)->default_value(false)->implicit_value(true),"Generate visualizations of AST, CFG, and transition system as dot files (ast.dot, cfg.dot, transitiongraph1/2.dot.")
    ("viz-tg2", po::value< bool >(&ctOpt.visualization.vizTg2)->default_value(false)->implicit_value(true),"Generate transition graph 2 (.dot).")
    ("cfg", po::value< string >(&ctOpt.visualization.icfgFileName), "Generate inter-procedural cfg as dot file. Each function is visualized as one dot cluster.")
    ;

  parallelProgramOptions.add_options()
    ("seed",po::value< int >(&parProOpt.seed),"Seed value for randomly selected integers (concurrency-related non-determinism might still affect results).")
    ("generate-automata",po::value< string >(&parProOpt.generateAutomata),"Generate random control flow automata (file <arg>) that can be interpreted and analyzed as a parallel program.")
    ("num-automata",po::value< int >(&parProOpt.numAutomata),"Select the number of parallel automata to generate.")
    ("num-syncs-range",po::value< string >(&parProOpt.numSyncsRange),"Select a range for the number of random synchronizations between the generated automata (csv pair of integers).")
    ("num-circles-range",po::value< string >(&parProOpt.numCirclesRange),"Select a range for the number of circles that a randomly generated automaton consists of (csv pair of integers).")
    ("circle-length-range",po::value< string >(&parProOpt.circlesLengthRange),"Select a range for the length of circles that are used to construct an automaton (csv pair of integers).")
    ("num-intersections-range",po::value< string >(&parProOpt.numIntersectionsRange),"Select a range for the number of intersections of a newly added circle with existing circles in the automaton (csv pair of integers).")
    ("automata-dot-input",po::value< string >(&parProOpt.automataDotInput),"Reads in parallel automata with synchronized transitions from a given .dot file.")
    ("keep-systems", po::value< bool >(&parProOpt.keepSystems)->default_value(false)->implicit_value(true),"Store computed parallel systems (over- and under-approximated STGs) during exploration  so that they do not need to be recomputed.")
    ("use-components",po::value< string >(&parProOpt. useComponents),"Selects which parallel components are chosen for analyzing the (approximated) state space ([all] | subsets-fixed | subsets-random).")
    ("fixed-subsets",po::value< string >(&parProOpt.fixedSubsets),"A list of sets of parallel component IDs used for analysis (e.g. \"{1,2},{4,7}\"). Use only with \"--use-components=subsets-fixed\".")
    ("num-random-components",po::value< int >(&parProOpt.numRandomComponents),"Number of different random components used for the analysis. Use only with \"--use-components=subsets-random\". Default: min(3, <num-parallel-components>)")
    ("parallel-composition-only", po::value< bool >(&parProOpt.parallelCompositionOnly)->default_value(false)->implicit_value(true),"If set to \"yes\", then no approximation will take place. Instead, the parallel compositions of the respective sub-systems will be expanded (sequentialized). Skips any LTL analysis. ([yes|no])")
    ("num-components-ltl",po::value< int >(&parProOpt.numComponentsLtl),"Number of different random components used to generate a random LTL property. Default: value of option --num-random-components (a.k.a. all analyzed components)")
    ("minimum-components",po::value< int >(&parProOpt.minimumComponents),"Number of different parallel components that need to be explored together in order to be able to analyze the mined properties. (Default: 3).")
    ("different-component-subsets",po::value< int >(&parProOpt.differentComponentSubsets),"Number of random component subsets. The solver will be run for each of the random subsets. Use only with \"--use-components=subsets-random\" (Default: no termination).")
    ("ltl-mode",po::value< string >(&parProOpt.ltlMode),"\"check\" checks the properties passed to option \"--check-ltl=<filename>\". \"mine\" searches for automatically generated properties that adhere to certain criteria. \"none\" means no LTL analysis (default).")
    ("mine-num-verifiable",po::value< int >(&parProOpt.mineNumVerifiable),"Number of verifiable properties satisfying given requirements that should be collected (Default: 10).")
    ("mine-num-falsifiable",po::value< int >(&parProOpt.mineNumFalsifiable),"Number of falsifiable properties satisfying given requirements that should be collected (Default: 10).")
    ("minings-per-subsets",po::value< int >(&parProOpt.miningsPerSubset),"Number of randomly generated properties that are evaluated based on one subset of parallel components (Default: 50).")
    ("ltl-properties-output",po::value< string >(&parProOpt.ltlPropertiesOutput),"Writes the analyzed LTL properties to file <arg>.")
    ("promela-output",po::value< string >(&parProOpt.promelaOutput),"Writes a promela program reflecting the synchronized automata of option \"--automata-dot-input\" to file <arg>. Includes LTL properties if analyzed.")
    ("promela-output-only", po::value< bool >(&parProOpt.promelaOutputOnly)->default_value(false)->implicit_value(true),"Only generate Promela code, skip analysis of the input .dot graphs.")
    ("output-with-results", po::value< bool >(&parProOpt.outputWithResults)->default_value(false)->implicit_value(true),"Include results for the LTL properties in generated promela code and LTL property files .")
    ("output-with-annotations", po::value< bool >(&parProOpt.outputWithAnnotations)->default_value(false)->implicit_value(true),"Include annotations for the LTL properties in generated promela code and LTL property files .")
    ("verification-engine",po::value< string >(&parProOpt.verificationEngine),"Choose which backend verification engine is used (ltsmin|[spot]).")
    ;

  experimentalOptions.add_options()
    ("omp-ast", po::value< bool >(&ctOpt.ompAst)->default_value(false)->implicit_value(true),"Flag for using the OpenMP AST - useful when visualizing the ICFG.")
    ("normalize-all", po::value< bool >(&ctOpt.normalizeAll)->default_value(false)->implicit_value(true),"Normalize all expressions before analysis.")
    ("normalize-fcalls", po::value< bool >(&ctOpt.normalizeFCalls)->default_value(false)->implicit_value(true),"Normalize only expressions with function calls.")
    ("inline", po::value< bool >(&ctOpt.inlineFunctions)->default_value(false)->implicit_value(false),"inline functions before analysis .")
    ("inlinedepth",po::value< int >(&ctOpt.inlineFunctionsDepth)->default_value(10),"Default value is 10. A higher value inlines more levels of function calls.")
    ("eliminate-compound-assignments", po::value< bool >(&ctOpt.eliminateCompoundStatements)->default_value(true)->implicit_value(true),"Replace all compound-assignments by assignments.")
    ("annotate-terms", po::value< bool >(&ctOpt.annotateTerms)->default_value(false)->implicit_value(true),"Annotate term representation of expressions in unparsed program.")
    ("eliminate-stg-back-edges", po::value< bool >(&ctOpt.eliminateSTGBackEdges)->default_value(false)->implicit_value(true), "Eliminate STG back-edges (STG becomes a tree).")
    ("generate-assertions", po::value< bool >(&ctOpt.generateAssertions)->default_value(false)->implicit_value(true),"Generate assertions (pre-conditions) in program and output program (using ROSE unparser).")
    ("precision-exact-constraints", po::value< bool >(&ctOpt.precisionExactConstraints)->default_value(false)->implicit_value(true),"Use precise constraint extraction.")
    ("stg-trace-file", po::value< string >(&ctOpt.stgTraceFileName), "Generate STG computation trace and write to file <arg>.")
    ("explicit-arrays", po::value< bool >(&ctOpt.explicitArrays)->default_value(true)->implicit_value(true),"Represent all arrays explicitly in every state.")
    ("z3", po::value< bool >(&ctOpt.z3BasedReachabilityAnalysis)->default_value(false)->implicit_value(true), "RERS specific reachability analysis using z3.")	
    ("rers-upper-input-bound", po::value< int >(&ctOpt.z3UpperInputBound), "RERS specific parameter for z3.")
    ("rers-verifier-error-number",po::value< int >(&ctOpt.z3VerifierErrorNumber), "RERS specific parameter for z3.")
    ("ssa",  po::value< bool >(&ctOpt.ssa)->default_value(false)->implicit_value(true), "Generate SSA form (only works for programs without function calls, loops, jumps, pointers and returns).")
    ("null-pointer-analysis",po::value< bool >(&ctOpt.nullPointerAnalysis)->default_value(false)->implicit_value(true),"Perform null pointer analysis and print results.")
    ("out-of-bounds-analysis",po::value< bool >(&ctOpt.outOfBoundsAnalysis)->default_value(false)->implicit_value(true),"Perform out-of-bounds analysis and print results.")
    ("uninitialized-analysis",po::value< bool >(&ctOpt.uninitializedMemoryAnalysis)->default_value(false)->implicit_value(true),"Perform uninitialized analysis and print results.")
    ("null-pointer-analysis-file",po::value< string >(&ctOpt.nullPointerAnalysisFileName),"Perform null pointer analysis and write results to file [arg].")
    ("out-of-bounds-analysis-file",po::value< string >(&ctOpt.outOfBoundsAnalysisFileName),"Perform out-of-bounds analysis and write results to file [arg].")
    ("uninitialized-analysis-file",po::value< string >(&ctOpt.uninitializedMemoryAnalysisFileName),"Perform uninitialized analysis and write results to file [arg].")
    ("program-stats-only",po::value< bool >(&ctOpt.programStatsOnly)->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs and exit.")
    ("program-stats",po::value< bool >(&ctOpt.programStats)->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs.")
    ("in-state-string-literals",po::value< bool >(&ctOpt.inStateStringLiterals)->default_value(false)->implicit_value(true),"create string literals in initial state.")
    ("std-functions",po::value< bool >(&ctOpt.stdFunctions)->default_value(true)->implicit_value(true),"model std function semantics (malloc, memcpy, etc). Must be turned off explicitly.")
    ("ignore-unknown-functions",po::value< bool >(&ctOpt.ignoreUnknownFunctions)->default_value(true)->implicit_value(true), "Unknown functions are assumed to be side-effect free.")
    ("ignore-undefined-dereference",po::value< bool >(&ctOpt.ignoreUndefinedDereference)->default_value(false)->implicit_value(true), "Ignore pointer dereference of uninitalized value (assume data exists).")
    ("ignore-function-pointers",po::value< bool >(&ctOpt.ignoreUnknownFunctions)->default_value(false)->implicit_value(true), "Ignore function pointers (functions are not called).")
    ("function-resolution-mode",po::value< int >(&ctOpt.functionResolutionMode)->default_value(4),"1:Translation unit only, 2:slow lookup, 3: -, 4: complete resolution (including function pointers)")
    ("context-sensitive",po::value< bool >(&ctOpt.contextSensitive)->default_value(true)->implicit_value(true),"Perform context sensitive analysis. Uses call strings with arbitrary length, recursion is not supported yet.")
    ("abstraction-mode",po::value< int >(&ctOpt.abstractionMode)->default_value(0),"Select abstraction mode (0: equality merge (explicit model checking), 1: approximating merge (abstract model checking).")
    ("interpreter-mode",po::value< int >(&ctOpt.interpreterMode)->default_value(0),"Select interpretation mode. 0: default, 1: execute stdout functions.")
    ("interpreter-mode-file",po::value< string >(&ctOpt.interpreterModeOuputFileName)->default_value(""),"Select interpretation mode output file (otherwise stdout is used).")
    ("print-warnings",po::value< bool >(&ctOpt.printWarnings)->default_value(false)->implicit_value(true),"Print warnings on stdout during analysis (this can slow down the analysis significantly)")
    ("print-violations",po::value< bool >(&ctOpt.printViolations)->default_value(false)->implicit_value(true),"Print detected violations on stdout during analysis (this can slow down the analysis significantly)")
    ("options-set",po::value< int >(&ctOpt.optionsSet)->default_value(0)->implicit_value(0),"Use a predefined set of default options (0:default|1..3:abstract)|11:concrete)).")
    ("callstring-length",po::value< int >(&ctOpt.callStringLength)->default_value(10),"Set the length of the callstring for context-sensitive analysis. Default value is 10.")
    ;

  rersOptions.add_options()
    ("csv-assert", po::value< string >(&ctOpt.rers.assertResultsOutputFileName), "Output assert reachability results into a CSV file <arg>.")
    ("eliminate-arrays", po::value< bool >(&ctOpt.rers.eliminateArrays)->default_value(false)->implicit_value(true), "Transform all arrays into single variables.")
    ("iseq-file", po::value< string >(&ctOpt.rers.iSeqFile), "Compute input sequence and generate file <arg>.")
    ("iseq-length", po::value< int >(&ctOpt.rers.iSeqLength), "Set length <arg> of input sequence to be computed.")
    ("iseq-random-num", po::value< int >(&ctOpt.rers.iSeqRandomNum), "Select random search and number <arg> of paths.")
    ("rers-binary", po::value< bool >(&ctOpt.rers.rersBinary)->default_value(false)->implicit_value(true),"Call RERS binary functions in analysis.")
    ("rers-numeric", po::value< bool >(&ctOpt.rers.rersNumeric)->default_value(false)->implicit_value(true), "Print RERS I/O values as raw numeric numbers.")
    ("rersmode", po::value< bool >(&ctOpt.rers.rersMode)->default_value(false)->implicit_value(true), "Sets several options such that RERS specifics are utilized and observed.")
    ("stderr-like-failed-assert", po::value< bool >(&ctOpt.rers.stdErrLikeFailedAssert)->default_value(false)->implicit_value(true), "Treat output on stderr similar to a failed assert.")
    ;

  svcompOptions.add_options()
    ("svcomp-mode", po::value< bool >(&ctOpt.svcomp.svcompMode)->default_value(false)->implicit_value(true), "Sets default options for all following SVCOMP-specific options.")
    //("external-function-semantics",  "assumes specific semantics for the external functions: __VERIFIER_error, __VERIFIER_nondet_int, exit, memcpy.")
    ("error-function", po::value< string >(&ctOpt.svcomp.detectedErrorFunctionName), "Detect a verifier error function with name <arg> (terminates verification).")
    ("witness-file", po::value< string >(&ctOpt.svcomp.witnessFileName), "Write an SV-COMP witness (counterexample) to file <arg>.")
    ;

  equivalenceCheckingOptions.add_options()
    ("dump-sorted",po::value< string >(&ctOpt.equiCheck.dumpSortedFileName), " (experimental) Generates sorted array updates in file <file>.")
    ("dump-non-sorted",po::value< string >(&ctOpt.equiCheck.dumpNonSortedFileName), " (experimental) Generates non-sorted array updates in file <file>.")
    ("rewrite-ssa", po::value< bool >(&ctOpt.equiCheck.rewriteSSA)->default_value(false)->implicit_value(true), "Rewrite SSA form: Replace use of SSA variable by rhs of its assignment (only applied outside loops or unrolled loops).")
    ("print-rewrite-trace", po::value< bool >(&ctOpt.equiCheck.printRewriteTrace)->default_value(false)->implicit_value(true), "Print trace of rewrite rules.")
    ("print-update-infos", po::value< bool >(&ctOpt.equiCheck.printUpdateInfos)->default_value(false)->implicit_value(true), "Print information about array updates on stdout.")
    ("rule-const-subst", po::value< bool >(&ctOpt.equiCheck.ruleConstSubst)->default_value(true)->implicit_value(true), "Use const-expr substitution rule.")
    ("rule-commutative-sort", po::value< bool >(&ctOpt.equiCheck.ruleCommutativeSort)->default_value(false)->implicit_value(true), "Apply rewrite rule for commutative sort of expression trees.")
    ("max-extracted-updates",po::value< int >(&ctOpt.equiCheck.maxExtractedUpdates)->default_value(5000)->implicit_value(-1),"Set maximum number of extracted updates. This ends the analysis.")
    ("specialize-fun-name", po::value< string >(&ctOpt.equiCheck.specializeFunName), "Function of name <arg> to be specialized.")
    ("specialize-fun-param", po::value< vector<int> >(&ctOpt.equiCheck.specializeFunParamList), "Function parameter number to be specialized (starting at 0).")
    ("specialize-fun-const", po::value< vector<int> >(&ctOpt.equiCheck.specializeFunConstList), "Constant <arg>, the param is to be specialized to.")
    ("specialize-fun-varinit", po::value< vector<string> >(&ctOpt.equiCheck.specializeFunVarInitList), "Variable name of which the initialization is to be specialized (overrides any initializer expression).")
    ("specialize-fun-varinit-const", po::value< vector<int> >(&ctOpt.equiCheck.specializeFunVarInitConstList), "Constant <arg>, the variable initialization is to be specialized to.")
    ;

  patternSearchOptions.add_options()
    ("pattern-search-max-depth", po::value< int >(&ctOpt.patSearch.maxDepth)->default_value(10), "Maximum input depth that is searched for cyclic I/O patterns.")
    ("pattern-search-repetitions", po::value< int >(&ctOpt.patSearch.repetitions)->default_value(100), "Number of unrolled iterations of cyclic I/O patterns.")
    ("pattern-search-max-suffix", po::value< int >(&ctOpt.patSearch.maxSuffix)->default_value(5), "Maximum input depth of the suffix that is searched for failing assertions after following an I/O-pattern.")
    ("pattern-search-exploration", po::value< string >(&ctOpt.patSearch.explorationMode), "Exploration mode for the pattern search. Note: all suffixes will always be checked using depth-first search. ([depth-first]|breadth-first)")
    ;

  dataRaceOptions.add_options()
    ("data-race", po::value< bool >(&ctOpt.dr.detection)->default_value(false)->implicit_value(true), "Perform data race detection.")
    ("data-race-check-shuffle", po::value< bool >(&ctOpt.dr. checkShuffleAlgorithm)->default_value(false)->implicit_value(true), "(work in progress) Perform data race detection using the new \"shuffle\" algorithm.")
    ("data-race-csv",po::value<string >(&ctOpt.dr.csvResultsFile),"Write data race detection results in specified csv file <arg>. Implicitly enables data race detection.")
    ("data-race-fail", po::value< bool >(&ctOpt.dr.failOnError)->default_value(false)->implicit_value(true), "Perform data race detection and fail on error (codethorn exit status 1). For use in regression verification. Implicitly enables data race detection.")
    ;

  visibleOptions.add_options()            
    ("config,c", po::value< string >(&ctOpt.configFileName), "Use the configuration specified in file <arg>.")
    ("colors", po::value< bool >(&ctOpt.colors)->default_value(true)->implicit_value(true),"Use colors in output.")
    ("csv-stats",po::value< string >(&ctOpt.csvStatsFileName),"Output statistics into a CSV file <arg>.")
    ("display-diff",po::value< int >(&ctOpt. displayDiff),"Print statistics every <arg> computed estates.")
    ("exploration-mode",po::value< string >(&ctOpt.explorationMode), "Set mode in which state space is explored. ([breadth-first]|depth-first|loop-aware|loop-aware-sync)")
    ("quiet", po::value< bool >(&ctOpt.quiet)->default_value(false)->implicit_value(true), "Produce no output on screen.")
    ("help,h", "Produce this help message.")
    ("help-cegpra", "Show options for CEGRPA.")
    ("help-eq", "Show options for program equivalence checking.")
    ("help-exp", "Show options for experimental features.")
    ("help-pat", "Show options for pattern search mode.")
    ("help-svcomp", "Show options for SV-Comp specific features.")
    ("help-rers", "Show options for RERS specific features")
    ("help-ltl", "Show options for LTL verification.")
    ("help-par", "Show options for analyzing parallel programs.")
    ("help-vis", "Show options for visualization output files.")
    ("help-data-race", "Show options for data race detection.")
    ("help-info", "Show options for program info.")
    ("start-function", po::value< string >(&ctOpt.startFunctionName), "Name of function to start the analysis from.")
    ("external-function-calls-file",po::value< string >(&ctOpt.externalFunctionCallsFileName), "write a list of all function calls to external functions (functions for which no implementation exists) to a CSV file.")
    ("status", po::value< bool >(&ctOpt.status)->default_value(false)->implicit_value(true), "Show status messages.")
    ("reduce-cfg", po::value< bool >(&ctOpt.reduceCfg)->default_value(true)->implicit_value(true), "Reduce CFG nodes that are irrelevant for the analysis.")
    ("internal-checks", po::value< bool >(&ctOpt.internalChecks)->default_value(false)->implicit_value(true), "Run internal consistency checks (without input program).")
    ("cl-args",po::value< string >(&ctOpt.analyzedProgramCLArgs),"Specify command line options for the analyzed program (as one quoted string).")
    ("input-values",po::value< string >(&ctOpt.inputValues),"Specify a set of input values. (e.g. \"{1,2,3}\")")
    ("input-values-as-constraints", po::value< bool >(&ctOpt.inputValuesAsConstraints)->default_value(false)->implicit_value(true),"Represent input var values as constraints (otherwise as constants in PState).")
    ("input-sequence",po::value< string >(&ctOpt.inputSequence),"Specify a sequence of input values. (e.g. \"[1,2,3]\")")
    ("log-level",po::value< string >(&ctOpt.logLevel)->default_value("none,>=error"),"Set the log level (\"x,>=y\" with x,y in: (none|info|warn|trace|error|fatal|debug)).")
    ("max-transitions",po::value< int >(&ctOpt.maxTransitions),"Passes (possibly) incomplete STG to verifier after <arg> transitions have been computed.")
    ("max-iterations",po::value< int >(&ctOpt.maxIterations),"Passes (possibly) incomplete STG to verifier after <arg> loop iterations have been explored. Currently requires --exploration-mode=loop-aware[-sync].")
    ("max-memory",po::value< long int >(&ctOpt.maxMemory),"Stop computing the STG after a total physical memory consumption of approximately <arg> Bytes has been reached.")
    ("max-time",po::value< long int >(&ctOpt.maxTime),"Stop computing the STG after an analysis time of approximately <arg> seconds has been reached.")
    ("max-transitions-forced-top",po::value< int >(&ctOpt.maxTransitionsForcedTop),"Performs approximation after <arg> transitions.")
    ("max-iterations-forced-top",po::value< int >(&ctOpt.maxIterationsForcedTop),"Performs approximation after <arg> loop iterations. Currently requires --exploration-mode=loop-aware[-sync].")
    ("max-memory-forced-top",po::value< long int >(&ctOpt.maxMemoryForcedTop),"Performs approximation after <arg> bytes of physical memory have been used.")
    ("max-time-forced-top",po::value< long int >(&ctOpt.maxTimeForcedTop),"Performs approximation after an analysis time of approximately <arg> seconds has been reached.")
    ("resource-limit-diff",po::value< int >(&ctOpt. resourceLimitDiff),"Check if the resource limit is reached every <arg> computed estates.")
    ("rewrite",po::value< bool >(&ctOpt.rewrite)->default_value(false)->implicit_value(true),"Rewrite AST applying all rewrite system rules.")
    ("run-rose-tests",po::value< bool >(&ctOpt.runRoseAstChecks)->default_value(false)->implicit_value(true), "Run ROSE AST tests.")
    ("analyzed-functions-csv",po::value<std::string>(&ctOpt.analyzedFunctionsCSVFileName),"Write list of analyzed functions to CSV file [arg].")
    ("analyzed-files-csv",po::value<std::string>(&ctOpt.analyzedFilesCSVFileName),"Write list of analyzed files (with analyzed functions) to CSV file [arg].")
    ("external-functions-csv",po::value<std::string>(&ctOpt.externalFunctionsCSVFileName),"Write list of external functions to CSV file [arg].")
    ("threads",po::value< int >(&ctOpt.threads),"(experimental) Run analyzer in parallel using <arg> threads.")
    ("unparse",po::value< bool >(&ctOpt.unparse)->default_value(false)->implicit_value(true),"unpare code (only relevant for inlining, normalization, and lowering)")
    ("version,v",po::value< bool >(&ctOpt.displayVersion)->default_value(false)->implicit_value(true), "Display the version of CodeThorn.")
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
    .add(hiddenOptions)
    .add(passOnToRose)
    .add(cegpraOptions)
    .add(equivalenceCheckingOptions)
    .add(parallelProgramOptions)
    .add(experimentalOptions)
    .add(ltlOptions)
    .add(patternSearchOptions)
    .add(rersOptions)
    .add(svcompOptions)
    .add(dataRaceOptions)
    .add(visualizationOptions)
    .add(infoOptions)
    ;

  po::options_description configFileOptions("Configuration file options");
  configFileOptions.add(visibleOptions)
    .add(hiddenOptions)
    .add(cegpraOptions)
    .add(equivalenceCheckingOptions)
    .add(parallelProgramOptions)
    .add(experimentalOptions)
    .add(ltlOptions)
    .add(patternSearchOptions)
    .add(rersOptions)
    .add(svcompOptions)
    .add(dataRaceOptions)
    .add(visualizationOptions)
    .add(infoOptions)
    ;

#if 0
  po::store(po::command_line_parser(argc, argv).options(all).run(), args);
  po::notify(args);

  if (args.isUserProvided("config")) {
    ifstream configStream(args.getString("config").c_str());
    po::store(po::parse_config_file(configStream, configFileOptions), args);
    po::notify(args);
  } 
#else
  args.parse(argc,argv,all,configFileOptions);
#endif
  if (args.isUserProvided("help")) {
    cout << visibleOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-cegpra")) {
    cout << cegpraOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-eq")) {
    cout << equivalenceCheckingOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-exp")) {
    cout << experimentalOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-ltl")) {
    cout << ltlOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-par")) {
    cout << parallelProgramOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-pat")) {
    cout << patternSearchOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-rers")) {
    cout << rersOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-svcomp")) {
    cout << svcompOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-vis")) {
    cout << visualizationOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-data-race")) {
    cout << dataRaceOptions << "\n";
    exit(0);
  } else if(args.isUserProvided("help-info")) {
    cout << infoOptions << "\n";
    exit(0);
  } else if(ctOpt.displayVersion) {
    cout << "CodeThorn version "<<version<<endl;
    cout << "Written by Markus Schordan, Marc Jasper, Simon Schroder, Maximilan Fecke, Joshua Asplund, Adrian Prantl\n";
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
      string stdPrefix = "-std=";
      if(currentArg.substr(0, iPrefix.size()) == iPrefix) {
        continue;
      }
      if(currentArg.substr(0, dPrefix.size()) == dPrefix) {
        continue;
      }
      if(currentArg.substr(0, stdPrefix.size()) == stdPrefix) {
        continue;
      }
    }
    // No match with elements in the white list above. 
    // Must be a CodeThorn option, therefore remove it from argv.
    argv[i] = strdup("");
  }

  checkSpotOptions();
  checkZ3Options();

  return args;
}

