#include "sage3basic.h"
#include "CodeThornException.h"
#include "CodeThornCommandLineOptions.h"

#include <string>
#include <sstream>
#include <iostream>

#include "Diagnostics.h"
using namespace Sawyer::Message;
using namespace std;
using namespace CodeThorn;

CodeThorn::CommandLineOptions& parseCommandLine(int argc, char* argv[], Sawyer::Message::Facility logger) {
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
    ("csv-spot-ltl", po::value< string >(), "Output SPOT's LTL verification results into a CSV file <arg>.")
    ("csv-stats-size-and-ltl",po::value< string >(),"Output statistics regarding the final model size and results for LTL properties into a CSV file <arg>.")
    ("check-ltl", po::value< string >(), "Take a text file of LTL I/O formulae <arg> and check whether or not the analyzed program satisfies these formulae. Formulae should start with '('. Use \"csv-spot-ltl\" option to specify an output csv file for the results.")
    ("single-property", po::value< int >(), "Number (ID) of the property that is supposed to be analyzed. All other LTL properties will be ignored. ( Use \"check-ltl\" option to specify a input property file).")
    ("counterexamples-with-output", po::value< bool >()->default_value(false)->implicit_value(true), "Reported counterexamples for LTL or reachability properties also include output values.")
    ("inf-paths-only", po::value< bool >()->default_value(false)->implicit_value(true), "Recursively prune the transition graph so that only infinite paths remain when checking LTL properties.")
    ("io-reduction", po::value< int >(), "(work in progress) Reduce the transition system to only input/output/worklist states after every <arg> computed EStates.")
    ("keep-error-states",  po::value< bool >()->default_value(false)->implicit_value(true), "Do not reduce error states for the LTL analysis.")      
    ("ltl-in-alphabet",po::value< string >(),"Specify an input alphabet used by the LTL formulae. (e.g. \"{1,2,3}\")")
    ("ltl-out-alphabet",po::value< string >(),"Specify an output alphabet used by the LTL formulae. (e.g. \"{19,20,21,22,23,24,25,26}\")")
    ("ltl-driven", po::value< bool >()->default_value(false)->implicit_value(true), "Select mode to verify LTLs driven by SPOT's access to the state transitions.")
    ("reset-analyzer", po::value< bool >()->default_value(true)->implicit_value(true), "Reset the analyzer and therefore the state transition graph before checking the next property. Only affects ltl-driven mode.")
    ("no-input-input",  po::value< bool >()->default_value(false)->implicit_value(true), "remove transitions where one input states follows another without any output in between. Removal occurs before the LTL check. [yes|=no]")
    ("std-io-only", po::value< bool >()->default_value(false)->implicit_value(true), "Bypass and remove all states that are not standard I/O.")
    ("with-counterexamples", po::value< bool >()->default_value(false)->implicit_value(true), "Add counterexample I/O traces to the analysis results. Applies to reachable assertions and falsified LTL properties (uses RERS-specific alphabet).")
    ("with-assert-counterexamples", po::value< bool >()->default_value(false)->implicit_value(true), "Report counterexamples leading to failing assertion states.")
    ("with-ltl-counterexamples", po::value< bool >()->default_value(false)->implicit_value(true), "Report counterexamples that violate LTL properties.")
    ;

  hiddenOptions.add_options()
    ("max-transitions-forced-top1",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output).")
    ("max-transitions-forced-top2",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output,df).")
    ("max-transitions-forced-top3",po::value< int >(),"Performs approximation after <arg> transitions (only exact for input,output,df,ptr-vars).")
    ("max-transitions-forced-top4",po::value< int >(),"Performs approximation after <arg> transitions (exact for all but inc-vars).")
    ("max-transitions-forced-top5",po::value< int >(),"Performs approximation after <arg> transitions (exact for input,output,df and vars with 0 to 2 assigned values)).")
    ("solver",po::value< int >()->default_value(5),"Set solver <arg> to use (one of 1,2,3,...).")
    ;

  passOnToRose.add_options()
    (",I", po::value< vector<string> >(),"Include directories.")
    (",D", po::value< vector<string> >(),"Define constants for preprocessor.")
    (",std", po::value< string >(),"Compilation standard.")
    ("edg:no_warnings", po::bool_switch(),"EDG frontend flag.")
    ;

  cegpraOptions.add_options()
    ("csv-stats-cegpra",po::value< string >(),"Output statistics regarding the counterexample-guided prefix refinement analysis (CEGPRA) into a CSV file <arg>.")
    ("cegpra-ltl",po::value< int >(),"Select the ID of an LTL property that should be checked using cegpra (between 0 and 99).")
    ("cegpra-ltl-all", po::value< bool >()->default_value(false)->implicit_value(true),"Check all specified LTL properties using CEGPRA.")
    ("cegpra-max-iterations",po::value< int >(),"Select a maximum number of counterexamples anaylzed by CEGPRA.")
    ("viz-cegpra-detailed",po::value< string >(),"Generate visualization (.dot) output files with prefix <arg> for different stages within each loop of CEGPRA.")
    ;

  visualizationOptions.add_options()
    ("rw-clusters", po::value< bool >()->default_value(false)->implicit_value(true), "Draw boxes around data elements from the same array (read/write-set graphs).")      
    ("rw-data", po::value< bool >()->default_value(false)->implicit_value(true), "Display names of data elements (read/write-set graphs).") 
    ("rw-highlight-races", po::value< bool >()->default_value(false)->implicit_value(true), "Highlight data races as large red dots (read/write-set graphs).") 
    ("dot-io-stg", po::value< string >(), "Output STG with explicit I/O node information in dot file <arg>.")
    ("dot-io-stg-forced-top", po::value< string >(), "Output STG with explicit I/O node information in dot file <arg>. Groups abstract states together.")
    ("tg1-estate-address", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 1: Visualize address.")
    ("tg1-estate-id", po::value< bool >()->default_value(true)->implicit_value(true), "Transition graph 1: Visualize estate-id.")
    ("tg1-estate-properties", po::value< bool >()->default_value(true)->implicit_value(true), "Transition graph 1: Visualize all estate-properties.")
    ("tg1-estate-predicate", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 1: Show estate as predicate.")
    ("tg1-estate-memory-subgraphs", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 1: Show estate as memory graphs.")
    ("tg2-estate-address", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 2: Visualize address.")
    ("tg2-estate-id", po::value< bool >()->default_value(true)->implicit_value(true), "Transition graph 2: Visualize estate-id.")
    ("tg2-estate-properties", po::value< bool >()->default_value(false)->implicit_value(true),"Transition graph 2: Visualize all estate-properties.")
    ("tg2-estate-predicate", po::value< bool >()->default_value(false)->implicit_value(true), "Transition graph 2: Show estate as predicate.")
    ("visualize-read-write-sets", po::value< bool >()->default_value(false)->implicit_value(true), "Generate a read/write-set graph that illustrates the read and write accesses of the involved threads.")
    ("viz", po::value< bool >()->default_value(false)->implicit_value(true),"Generate visualizations of AST, CFG, and transition system as dot files (ast.dot, cfg.dot, transitiongraph1/2.dot.")
    ("viz-tg2", po::value< bool >()->default_value(false)->implicit_value(true),"Generate transition graph 2 (.dot).")
    ("cfg", po::value< string >(), "Generate inter-procedural cfg as dot file. Each function is visualized as one dot cluster.")
    ;

  parallelProgramOptions.add_options()
    ("seed",po::value< int >(),"Seed value for randomly selected integers (concurrency-related non-determinism might still affect results).")
    ("generate-automata",po::value< string >(),"Generate random control flow automata (file <arg>) that can be interpreted and analyzed as a parallel program.")
    ("num-automata",po::value< int >(),"Select the number of parallel automata to generate.")
    ("num-syncs-range",po::value< string >(),"Select a range for the number of random synchronizations between the generated automata (csv pair of integers).")
    ("num-circles-range",po::value< string >(),"Select a range for the number of circles that a randomly generated automaton consists of (csv pair of integers).")
    ("circle-length-range",po::value< string >(),"Select a range for the length of circles that are used to construct an automaton (csv pair of integers).")
    ("num-intersections-range",po::value< string >(),"Select a range for the number of intersections of a newly added circle with existing circles in the automaton (csv pair of integers).")
    ("automata-dot-input",po::value< string >(),"Reads in parallel automata with synchronized transitions from a given .dot file.")
    ("keep-systems", po::value< bool >()->default_value(false)->implicit_value(true),"Store computed parallel systems (over- and under-approximated STGs) during exploration  so that they do not need to be recomputed.")
    ("use-components",po::value< string >(),"Selects which parallel components are chosen for analyzing the (approximated) state space ([all] | subsets-fixed | subsets-random).")
    ("fixed-subsets",po::value< string >(),"A list of sets of parallel component IDs used for analysis (e.g. \"{1,2},{4,7}\"). Use only with \"--use-components=subsets-fixed\".")
    ("num-random-components",po::value< int >(),"Number of different random components used for the analysis. Use only with \"--use-components=subsets-random\". Default: min(3, <num-parallel-components>)")
    ("parallel-composition-only", po::value< bool >()->default_value(false)->implicit_value(true),"If set to \"yes\", then no approximation will take place. Instead, the parallel compositions of the respective sub-systems will be expanded (sequentialized). Skips any LTL analysis. ([yes|no])")
    ("num-components-ltl",po::value< int >(),"Number of different random components used to generate a random LTL property. Default: value of option --num-random-components (a.k.a. all analyzed components)")
    ("minimum-components",po::value< int >(),"Number of different parallel components that need to be explored together in order to be able to analyze the mined properties. (Default: 3).")
    ("different-component-subsets",po::value< int >(),"Number of random component subsets. The solver will be run for each of the random subsets. Use only with \"--use-components=subsets-random\" (Default: no termination).")
    ("ltl-mode",po::value< string >(),"\"check\" checks the properties passed to option \"--check-ltl=<filename>\". \"mine\" searches for automatically generated properties that adhere to certain criteria. \"none\" means no LTL analysis (default).")
    ("mine-num-verifiable",po::value< int >(),"Number of verifiable properties satisfying given requirements that should be collected (Default: 10).")
    ("mine-num-falsifiable",po::value< int >(),"Number of falsifiable properties satisfying given requirements that should be collected (Default: 10).")
    ("minings-per-subsets",po::value< int >(),"Number of randomly generated properties that are evaluated based on one subset of parallel components (Default: 50).")
    ("ltl-properties-output",po::value< string >(),"Writes the analyzed LTL properties to file <arg>.")
    ("promela-output",po::value< string >(),"Writes a promela program reflecting the synchronized automata of option \"--automata-dot-input\" to file <arg>. Includes LTL properties if analyzed.")
    ("promela-output-only", po::value< bool >()->default_value(false)->implicit_value(true),"Only generate Promela code, skip analysis of the input .dot graphs.")
    ("output-with-results", po::value< bool >()->default_value(false)->implicit_value(true),"Include results for the LTL properties in generated promela code and LTL property files .")
    ("output-with-annotations", po::value< bool >()->default_value(false)->implicit_value(true),"Include annotations for the LTL properties in generated promela code and LTL property files .")
    ("verification-engine",po::value< string >(),"Choose which backend verification engine is used (ltsmin|[spot]).")
    ;

  experimentalOptions.add_options()
    ("omp-ast", po::value< bool >()->default_value(false)->implicit_value(true),"Flag for using the OpenMP AST - useful when visualizing the ICFG.")
    ("normalize-all", po::value< bool >()->default_value(false)->implicit_value(true),"Normalize all expressions before analysis.")
    ("normalize-fcalls", po::value< bool >()->default_value(false)->implicit_value(true),"Normalize only expressions with function calls.")
    ("inline", po::value< bool >()->default_value(false)->implicit_value(false),"inline functions before analysis .")
    ("inlinedepth",po::value< int >()->default_value(10),"Default value is 10. A higher value inlines more levels of function calls.")
    ("eliminate-compound-assignments", po::value< bool >()->default_value(true)->implicit_value(true),"Replace all compound-assignments by assignments.")
    ("annotate-terms", po::value< bool >()->default_value(false)->implicit_value(true),"Annotate term representation of expressions in unparsed program.")
    ("eliminate-stg-back-edges", po::value< bool >()->default_value(false)->implicit_value(true), "Eliminate STG back-edges (STG becomes a tree).")
    ("generate-assertions", po::value< bool >()->default_value(false)->implicit_value(true),"Generate assertions (pre-conditions) in program and output program (using ROSE unparser).")
    ("precision-exact-constraints", po::value< bool >()->default_value(false)->implicit_value(true),"Use precise constraint extraction.")
    ("stg-trace-file", po::value< string >(), "Generate STG computation trace and write to file <arg>.")
    ("explicit-arrays", po::value< bool >()->default_value(true)->implicit_value(true),"Represent all arrays explicitly in every state.")
    ("z3", "RERS specific reachability analysis using z3.")	
    ("rers-upper-input-bound", po::value< int >(), "RERS specific parameter for z3.")
    ("rers-verifier-error-number",po::value< int >(), "RERS specific parameter for z3.")
    ("ssa",  po::value< bool >()->default_value(false)->implicit_value(true), "Generate SSA form (only works for programs without function calls, loops, jumps, pointers and returns).")
    ("null-pointer-analysis","Perform null pointer analysis and print results.")
    ("out-of-bounds-analysis","Perform out-of-bounds analysis and print results.")
    ("uninitialized-analysis","Perform uninitialized analysis and print results.")
    ("null-pointer-analysis-file",po::value< string >(),"Perform null pointer analysis and write results to file [arg].")
    ("out-of-bounds-analysis-file",po::value< string >(),"Perform out-of-bounds analysis and write results to file [arg].")
    ("uninitialized-analysis-file",po::value< string >(),"Perform uninitialized analysis and write results to file [arg].")
    ("program-stats-only",po::value< bool >()->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs and exit.")
    ("program-stats",po::value< bool >()->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs.")
    ("in-state-string-literals",po::value< bool >()->default_value(false)->implicit_value(true),"create string literals in initial state.")
    ("std-functions",po::value< bool >()->default_value(true)->implicit_value(true),"model std function semantics (malloc, memcpy, etc). Must be turned off explicitly.")
    ("ignore-unknown-functions",po::value< bool >()->default_value(true)->implicit_value(true), "Unknown functions are assumed to be side-effect free.")
    ("ignore-undefined-dereference",po::value< bool >()->default_value(false)->implicit_value(true), "Ignore pointer dereference of uninitalized value (assume data exists).")
    ("ignore-function-pointers",po::value< bool >()->default_value(false)->implicit_value(true), "Ignore function pointers (functions are not called).")
    ("function-resolution-mode",po::value< int >()->default_value(4),"1:Translation unit only, 2:slow lookup, 3: -, 4: complete resolution (including function pointers)")
    ("context-sensitive",po::value< bool >()->default_value(false)->implicit_value(true),"Perform context sensitive analysis. Uses call strings with arbitrary length, recursion is not supported yet.")
    ("abstraction-mode",po::value< int >()->default_value(0),"Select abstraction mode (0: equality merge (explicit model checking), 1: approximating merge (abstract model checking).")
    ("interpreter-mode",po::value< int >()->default_value(0),"Select interpretation mode. 0: default, 1: execute stdout functions.")
    ("interpreter-mode-file",po::value< string >()->default_value(""),"Select interpretation mode output file (otherwise stdout is used).")
    ("print-warnings",po::value< bool >()->default_value(false)->implicit_value(true),"Print warnings on stdout during analysis (this can slow down the analysis significantly)")
    ("print-violations",po::value< bool >()->default_value(false)->implicit_value(true),"Print detected violations on stdout during analysis (this can slow down the analysis significantly)")
    ("options-set",po::value< int >()->default_value(0)->implicit_value(0),"Use a predefined set of default options (0..3).")
//  ("callstring-length",po::value< int >()->default_value(10),"Set the length of the callstring for context-sensitive analysis. Default value is 10.")
    ;

  rersOptions.add_options()
    ("csv-assert", po::value< string >(), "Output assert reachability results into a CSV file <arg>.")
    ("eliminate-arrays", po::value< bool >()->default_value(false)->implicit_value(true), "Transform all arrays into single variables.")
    ("iseq-file", po::value< string >(), "Compute input sequence and generate file <arg>.")
    ("iseq-length", po::value< int >(), "Set length <arg> of input sequence to be computed.")
    ("iseq-random-num", po::value< int >(), "Select random search and number <arg> of paths.")
    ("rers-binary", po::value< bool >()->default_value(false)->implicit_value(true),"Call RERS binary functions in analysis.")
    ("rers-numeric", po::value< bool >()->default_value(false)->implicit_value(true), "Print RERS I/O values as raw numeric numbers.")
    ("rersmode", po::value< bool >()->default_value(false)->implicit_value(true), "Sets several options such that RERS specifics are utilized and observed.")
    ("stderr-like-failed-assert", po::value< bool >()->default_value(false)->implicit_value(true), "Treat output on stderr similar to a failed assert.")
    ;

  svcompOptions.add_options()
    ("svcomp-mode", po::value< bool >()->default_value(false)->implicit_value(true), "Sets default options for all following SVCOMP-specific options.")
    //("external-function-semantics",  "assumes specific semantics for the external functions: __VERIFIER_error, __VERIFIER_nondet_int, exit, memcpy.")
    ("error-function", po::value< string >(), "Detect a verifier error function with name <arg> (terminates verification).")
    ("witness-file", po::value< string >(), "Write an SV-COMP witness (counterexample) to file <arg>.")
    ;

  equivalenceCheckingOptions.add_options()
    ("dump-sorted",po::value< string >(), " (experimental) Generates sorted array updates in file <file>.")
    ("dump-non-sorted",po::value< string >(), " (experimental) Generates non-sorted array updates in file <file>.")
    ("rewrite-ssa", po::value< bool >()->default_value(false)->implicit_value(true), "Rewrite SSA form: Replace use of SSA variable by rhs of its assignment (only applied outside loops or unrolled loops).")
    ("print-rewrite-trace", po::value< bool >()->default_value(false)->implicit_value(true), "Print trace of rewrite rules.")
    ("print-update-infos", po::value< bool >()->default_value(false)->implicit_value(true), "Print information about array updates on stdout.")
    ("rule-const-subst", po::value< bool >()->default_value(true)->implicit_value(true), "Use const-expr substitution rule.")
    ("rule-commutative-sort", po::value< bool >()->default_value(false)->implicit_value(true), "Apply rewrite rule for commutative sort of expression trees.")
    ("max-extracted-updates",po::value< int >()->default_value(5000)->implicit_value(-1),"Set maximum number of extracted updates. This ends the analysis.")
    ("specialize-fun-name", po::value< string >(), "Function of name <arg> to be specialized.")
    ("specialize-fun-param", po::value< vector<int> >(), "Function parameter number to be specialized (starting at 0).")
    ("specialize-fun-const", po::value< vector<int> >(), "Constant <arg>, the param is to be specialized to.")
    ("specialize-fun-varinit", po::value< vector<string> >(), "Variable name of which the initialization is to be specialized (overrides any initializer expression).")
    ("specialize-fun-varinit-const", po::value< vector<int> >(), "Constant <arg>, the variable initialization is to be specialized to.")
    ;

  patternSearchOptions.add_options()
    ("pattern-search-max-depth", po::value< int >()->default_value(10), "Maximum input depth that is searched for cyclic I/O patterns.")
    ("pattern-search-repetitions", po::value< int >()->default_value(100), "Number of unrolled iterations of cyclic I/O patterns.")
    ("pattern-search-max-suffix", po::value< int >()->default_value(5), "Maximum input depth of the suffix that is searched for failing assertions after following an I/O-pattern.")
    ("pattern-search-exploration", po::value< string >(), "Exploration mode for the pattern search. Note: all suffixes will always be checked using depth-first search. ([depth-first]|breadth-first)")
    ;

  dataRaceOptions.add_options()
    ("data-race", po::value< bool >()->default_value(false)->implicit_value(true), "Perform data race detection.")
    ("data-race-check-shuffle", po::value< bool >()->default_value(false)->implicit_value(true), "(work in progress) Perform data race detection using the new \"shuffle\" algorithm.")
    ("data-race-csv",po::value<string >(),"Write data race detection results in specified csv file <arg>. Implicitly enables data race detection.")
    ("data-race-fail", po::value< bool >()->default_value(false)->implicit_value(true), "Perform data race detection and fail on error (codethorn exit status 1). For use in regression verification. Implicitly enables data race detection.")
    ;

  visibleOptions.add_options()            
    ("config,c", po::value< string >(), "Use the configuration specified in file <arg>.")
    ("colors", po::value< bool >()->default_value(true)->implicit_value(true),"Use colors in output.")
    ("csv-stats",po::value< string >(),"Output statistics into a CSV file <arg>.")
    ("display-diff",po::value< int >(),"Print statistics every <arg> computed estates.")
    ("exploration-mode",po::value< string >(), "Set mode in which state space is explored. ([breadth-first]|depth-first|loop-aware|loop-aware-sync)")
    ("quiet", "Produce no output on screen.")
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
    ("start-function", po::value< string >(), "Name of function to start the analysis from.")
    ("external-function-calls-file",po::value< string >(), "write a list of all function calls to external functions (functions for which no implementation exists) to a CSV file.")
    ("status", po::value< bool >()->default_value(false)->implicit_value(true), "Show status messages.")
    ("reduce-cfg", po::value< bool >()->default_value(true)->implicit_value(true), "Reduce CFG nodes that are irrelevant for the analysis.")
    ("internal-checks", "Run internal consistency checks (without input program).")
    ("cl-args",po::value< string >(),"Specify command line options for the analyzed program (as one quoted string).")
    ("input-values",po::value< string >(),"Specify a set of input values. (e.g. \"{1,2,3}\")")
    ("input-values-as-constraints", po::value< bool >()->default_value(false)->implicit_value(true),"Represent input var values as constraints (otherwise as constants in PState).")
    ("input-sequence",po::value< string >(),"Specify a sequence of input values. (e.g. \"[1,2,3]\")")
    ("log-level",po::value< string >()->default_value("none,>=error"),"Set the log level (\"x,>=y\" with x,y in: (none|info|warn|trace|error|fatal|debug)).")
    ("max-transitions",po::value< int >(),"Passes (possibly) incomplete STG to verifier after <arg> transitions have been computed.")
    ("max-iterations",po::value< int >(),"Passes (possibly) incomplete STG to verifier after <arg> loop iterations have been explored. Currently requires --exploration-mode=loop-aware[-sync].")
    ("max-memory",po::value< long int >(),"Stop computing the STG after a total physical memory consumption of approximately <arg> Bytes has been reached.")
    ("max-time",po::value< long int >(),"Stop computing the STG after an analysis time of approximately <arg> seconds has been reached.")
    ("max-transitions-forced-top",po::value< int >(),"Performs approximation after <arg> transitions.")
    ("max-iterations-forced-top",po::value< int >(),"Performs approximation after <arg> loop iterations. Currently requires --exploration-mode=loop-aware[-sync].")
    ("max-memory-forced-top",po::value< long int >(),"Performs approximation after <arg> bytes of physical memory have been used.")
    ("max-time-forced-top",po::value< long int >(),"Performs approximation after an analysis time of approximately <arg> seconds has been reached.")
    ("resource-limit-diff",po::value< int >(),"Check if the resource limit is reached every <arg> computed estates.")
    ("rewrite","Rewrite AST applying all rewrite system rules.")
    ("run-rose-tests", "Run ROSE AST tests.")
    ("analyzed-functions-csv",po::value<std::string>(),"Write list of analyzed functions to CSV file [arg].")
    ("analyzed-files-csv",po::value<std::string>(),"Write list of analyzed files (with analyzed functions) to CSV file [arg].")
    ("external-functions-csv",po::value<std::string>(),"Write list of external functions to CSV file [arg].")
    ("threads",po::value< int >(),"(experimental) Run analyzer in parallel using <arg> threads.")
    ("unparse",po::value< bool >()->default_value(false)->implicit_value(true),"unpare code (only relevant for inlining, normalization, and lowering)")
    ("version,v", "Display the version of CodeThorn.")
    ;

  infoOptions.add_options()
    ("print-variable-id-mapping",po::value< bool >()->default_value(false)->implicit_value(true),"Print variable-id-mapping on stdout.")
    ("print-function-id-mapping",po::value< bool >()->default_value(false)->implicit_value(true),"Print function-id-mapping on stdout.")
    ("ast-stats-print",po::value< bool >()->default_value(false)->implicit_value(true),"Print ast node statistics on stdout.")
    ("ast-stats-csv",po::value< string >(),"Write ast node statistics to CSV file [arg].")
    ("type-size-mapping-print",po::value< bool >()->default_value(false)->implicit_value(true),"Print type-size mapping on stdout.")
    ("type-size-mapping-csv",po::value<std::string>(),"Write type-size mapping to CSV file [arg].")
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

  po::options_description configOptions("Configuration file options");
  configOptions.add(visibleOptions)
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

  po::store(po::command_line_parser(argc, argv).options(all).run(), args);
  po::notify(args);

  if (args.isDefined("config")) {
    ifstream configStream(args.getString("config").c_str());
    po::store(po::parse_config_file(configStream, configOptions), args);
    po::notify(args);
  } 

  if (args.count("help")) {
    cout << visibleOptions << "\n";
    exit(0);
  } else if(args.count("help-cegpra")) {
    cout << cegpraOptions << "\n";
    exit(0);
  } else if(args.count("help-eq")) {
    cout << equivalenceCheckingOptions << "\n";
    exit(0);
  } else if(args.count("help-exp")) {
    cout << experimentalOptions << "\n";
    exit(0);
  } else if(args.count("help-ltl")) {
    cout << ltlOptions << "\n";
    exit(0);
  } else if(args.count("help-par")) {
    cout << parallelProgramOptions << "\n";
    exit(0);
  } else if(args.count("help-pat")) {
    cout << patternSearchOptions << "\n";
    exit(0);
  } else if(args.count("help-rers")) {
    cout << rersOptions << "\n";
    exit(0);
  } else if(args.count("help-svcomp")) {
    cout << svcompOptions << "\n";
    exit(0);
  } else if(args.count("help-vis")) {
    cout << visualizationOptions << "\n";
    exit(0);
  } else if(args.count("help-data-race")) {
    cout << dataRaceOptions << "\n";
    exit(0);
  } else if(args.count("help-info")) {
    cout << infoOptions << "\n";
    exit(0);
  } else if (args.count("version")) {
    cout << "CodeThorn version 1.11.6\n";
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

  return args;
}

