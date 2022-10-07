#include "Thorn4CommandLineParser.h"

std::vector<std::string> Thorn4Parser::parseArgs(int argc, char** argv) {
  std::vector<std::string> cmdLineArgs{argv+0, argv+argc};
  return parseArgs(cmdLineArgs);
}

CodeThornOptions Thorn4Parser::getOptions() {
  return _ctOpt;
}

std::vector<std::string> Thorn4Parser::parseArgs(std::vector<std::string> clArgs) {
  scl::Parser p = Rose::CommandLine::createEmptyParserStage("", "");
    
  // things like --help, --version, --log, --threads, etc.
  p.with(Rose::CommandLine::genericSwitches());
  //~ p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis
    
  // Create a group of switches specific to this tool
  scl::SwitchGroup thorn4Parser("thorn4 - specific switches");

  thorn4Parser.name("thorn4");  // the optional switch prefix

  thorn4Parser.insert(scl::Switch("report-dir")
                      .argument("reportDir", scl::anyParser(_ctOpt.reportFilePath))
                      .doc("filename prefix for all variants of graph files. Provide an absolute paths if thorn4 is invoked in multiple different directories."));
  thorn4Parser.insert(scl::Switch("mode")
                      .argument("mode", scl::anyParser(_ctOpt.multiSelectors.analysisMode))
                      .doc("analysis mode: concrete, abstract."));
  thorn4Parser.insert(scl::Switch("input-values")
                      .argument("inputValues", scl::anyParser(_ctOpt.inputValues))
                      .doc("set of values used for scanf in 'concrete' mode. e.g. \"{1,2,3}\""));
  thorn4Parser.insert(scl::Switch("status")
                      .intrinsicValue(true,_ctOpt.status)
                      .doc("print status messages during analysis."));
  thorn4Parser.insert(scl::Switch("reduce-stg")
                      .intrinsicValue(true,_ctOpt.reduceStg)
                      .doc("reduce STS graph to input/output states."));
  p.purpose("Generates State Transition System Graph files")
    .doc("synopsis",
         "@prop{programName} [@v{switches}] @v{specimen_name}")
    .doc("description",
         "This program generates state transition system graph files.");
  scl::ParserResult cmdLine = p.with(thorn4Parser).parse(clArgs).apply();

  const std::vector<std::string> remainingArgs = cmdLine.unparsedArgs();
  for (const std::string& arg: remainingArgs) {
    //mlog[DEBUG] <<"remaining arg: \"" <<Rose::StringUtility::cEscape(arg) <<"\"\n";
    if (CppStdUtilities::isPrefix("--thorn4:",arg)) {
      cerr <<"thorn4: unrecognized command line option: \"" <<Rose::StringUtility::cEscape(arg) <<"\"\n";
      exit(1);
    }
  }
  processMultiSelectors();
  return cmdLine.unparsedArgs();
}

void Thorn4Parser::processMultiSelectors() {
  if(_ctOpt.multiSelectors.analysisMode=="abstract") {
    _ctOpt.solver=16; // default solver for this tool
    _ctOpt.sharedPStates=false; // required for solver 16
    _ctOpt.abstractionMode=1;
    _ctOpt.arrayAbstractionIndex=0;
  } else if(_ctOpt.multiSelectors.analysisMode=="concrete") {
    _ctOpt.solver=5; // default solver for concrete mode
    _ctOpt.sharedPStates=false;
    _ctOpt.abstractionMode=0;
    _ctOpt.arrayAbstractionIndex=-1; // no abstraction of arrays
    if(_ctOpt.inputValues=="{}") {
      cerr<<"Concrete mode selected, but no input values provided. Use option --input-values=\"{ ... }\" to provide a set of input values."<<endl;
      exit(1);
    }
  } else if(_ctOpt.multiSelectors.analysisMode=="none") {
    // no multi select requested
  } else {
    cerr<<"Wrong mode '"<<_ctOpt.multiSelectors.analysisMode<<"' provided on command line."<<endl;
    exit(1);
  }
}
