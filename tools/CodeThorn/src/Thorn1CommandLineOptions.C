#include "sage3basic.h"
// required for checking of: HAVE_SPOT, HAVE_Z3
#include "rose_config.h"

#include "CodeThornException.h"
#include "CodeThornCommandLineOptions.h"
#include "CppStdUtilities.h"

#include <string>
#include <sstream>
#include <iostream>

#include "Rose/Diagnostics.h"
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
                                                CodeThornOptions& ctOpt, LTLOptions& /*ltlOpt*/, ParProOptions& /*parProOpt*/) {

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
    ("rose:ast:write", po::value<bool>(&ctOpt.roseAstWrite)->implicit_value(true),"write AST binary file.")
    ("rose:ast:merge", po::value<bool>(&ctOpt.roseAstMerge)->implicit_value(true),"merge ASTs of read files (is implict for rose:ast:read).")
    ;

  experimentalOptions.add_options()
    //("normalize-level", po::value< int >(&ctOpt.normalizeLevel),"Normalize all expressions (2), only fcalls (1), turn off (0).")
    //("ignore-function-pointers",po::value< bool >(&ctOpt.ignoreFunctionPointers)->default_value(false)->implicit_value(true), "Unknown functions are assumed to be side-effect free.")
    ("function-resolution-mode",po::value< int >(&ctOpt.functionResolutionMode)->default_value(4),"1:Translation unit only, 2:slow lookup, 3: -, 4: complete resolution (including function pointers)")
    //("test-selector",po::value< int >(&ctOpt.testSelector)->default_value(0)->implicit_value(0),"Option for selecting dev tests.")
    ("ast-symbol-check",po::value< bool >(&ctOpt.astSymbolCheckFlag),"Allows to turn off the AST consistency symbol check (by default the check is enabled).")
    ("ast-symbol-pointer-check",po::value< bool >(&ctOpt.info.astSymbolPointerCheckReport)->implicit_value(true),"Run ast symbol pointer check. Writes report to ast-symbol-pointer-check-report.txt.")
    ("vim-report-file",po::value< string >(&ctOpt.vimReportFileName),"Generates a report of ast symbol check and some additional ast checks.")
    ;

  visibleOptions.add_options()
    ("config", po::value< string >(&ctOpt.configFileName), "Use the configuration specified in file <arg>.")
    ("colors", po::value< bool >(&ctOpt.colors)->default_value(true)->implicit_value(true),"Use colors in output.")
    ("quiet", po::value< bool >(&ctOpt.quiet)->default_value(false)->implicit_value(true), "Produce no output on screen.")
    ("help,h", "Produce this help message.")
    ("help-all", "Show all help options.")
    ("help-exp", "Show options for experimental features.")
    ("start-function", po::value< string >(&ctOpt.startFunctionName), "Name of function to start the analysis from (only for compatibility, has no effect)")
    //    ("program-stats-only",po::value< bool >(&ctOpt.programStatsOnly)->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs and exit.")
    ("external-functions-csv",po::value<std::string>(&ctOpt.externalFunctionsCSVFileName),"Write list of external functions (functions for which no implementation is provided) to CSV file [arg].")
    //    ("external-function-calls-csv",po::value< string >(&ctOpt.externalFunctionCallsCSVFileName), "write a list of all function calls to external functions encountered during the program analysis to a CSV file.")
    //("program-stats",po::value< bool >(&ctOpt.programStats)->default_value(false)->implicit_value(true),"print some basic program statistics about used language constructs.")
    ("status", po::value< bool >(&ctOpt.status)->default_value(false)->implicit_value(true), "Show status messages.")
    ("run-rose-tests",po::value< bool >(&ctOpt.runRoseAstChecks)->default_value(false)->implicit_value(true), "Run ROSE AST tests.")
    ("internal-checks", po::value< bool >(&ctOpt.internalChecks)->default_value(false)->implicit_value(true), "Run internal consistency checks (without input program).")
    ("report-file-path",po::value< string >(&ctOpt.reportFilePath),"Sets file path for all reports.")
    ("file-path-prefix-to-remove", po::value< string >(&ctOpt.filePathPrefixToRemove))
    ("log-level",po::value< string >(&ctOpt.logLevel)->default_value("none,>=error"),"Set the log level (\"x,>=y\" with x,y in: (none|info|warn|trace|error|fatal|debug)).")
    ("version,v",po::value< bool >(&ctOpt.displayVersion)->default_value(false)->implicit_value(true), "Display the program version.")
    ;

  po::options_description all("All supported options");
  all.add(visibleOptions)
    .add(passOnToRoseOptions)
    .add(experimentalOptions)
    ;

  po::options_description configFileOptions("Configuration file options");
  configFileOptions.add(visibleOptions)
    //    .add(passOnToRoseOptions) [cannot be used in config file]
    .add(experimentalOptions)
    ;

  args.parseAllowUnregistered(argc,argv,all);
  //args.parse(argc,argv,all);

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
    cout << "thorn1 version "<<version<<endl;
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
    } else if (currentArg == "--rose:ast:write"||currentArg == "--rose:ast:merge") {
      argv[i] = strdup("");
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
    //cout<<"Removing option "<<argv[i]<<endl;
    //argv[i] = strdup("");
  }

  return args;
}
