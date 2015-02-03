#include "CommandLineOptions.h"
#include "rose.h"
#include "assert.h"
#include <boost/program_options.hpp>

using namespace std;
using namespace Backstroke;

boost::program_options::variables_map args;

Backstroke::CommandLineOptions::CommandLineOptions(): _optionVersion(false),
                                                      _optionRoseHelp(false),
                                                      _optionRoseAstCheck(false),
                                                      _optionStatusMessages(false),
                                                      _optionShowTransformationTrace(false),
                                                      _isFinished(false) {
}

bool
Backstroke::CommandLineOptions::isFinished() {
  return _isFinished;
}

bool
Backstroke::CommandLineOptions::isError() {
  // there is no error detection yet, as unrecognized arguments are passed to ROSE (e.g. -I)
  return false;
}

bool
Backstroke::CommandLineOptions::optionRoseAstCheck() {
  return _optionRoseAstCheck;
}

bool
Backstroke::CommandLineOptions::optionStatusMessages() {
  return _optionStatusMessages;
}

bool
Backstroke::CommandLineOptions::optionShowTransformationTrace() {
  return _optionShowTransformationTrace;
}

void
Backstroke::CommandLineOptions::process(int argc, char* argv[]) {
  if(argc==1) {
    cout << "Error: wrong command line options."<<endl;
    exit(1);
  }
  // Command line option handling.
  namespace po = boost::program_options;
  boost::program_options::options_description desc
    ("supported options");
  
  desc.add_options()
    ("rose-help", "show ROSE help for compiler frontend options.")
    ("rose-ast-check", "perform consistency test of ROSE AST.")
    ("trace", "shows a trace of all transformations performed to obtain reverse code.")
    ("status-messages,s", "print status messages while processing input file.")
    ("help,h", "produce this help message.")
    ("version,v", "display the backstroke2 version number.")
    ;
  
  po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), args);
  po::notify(args);
  
  if (args.count("help")) {
    cout << "backstroke2 <filename> [OPTIONS]"<<endl;
    cout << desc << "\n";
    _isFinished=true;
  }
  if (args.count("rose-help")) {
      argv[1] = strdup("--help");
      _optionRoseHelp=true;
      _isFinished=true;
  }
  if (args.count("rose-ast-check")) {
    _optionRoseAstCheck=true;
  }
  if (args.count("status-messages")) {
    _optionStatusMessages=true;
  }
  if (args.count("version")) {
    _optionVersion=true;
    _isFinished=true;
    cout << "version 2.0.0\n";
    cout << "Written by Markus Schordan 2014,2015.\n";
  }
  if (args.count("trace")) {
    _optionShowTransformationTrace=true;
  }
    
  // clean up string-options in argv
  for (int i=1; i<argc; ++i) {
    if (string(argv[i]) == "--prefix" 
        ) {
      // do not confuse ROSE frontend
      argv[i] = strdup("");
      assert(i+1<argc);
      argv[i+1] = strdup("");
    }
  }

}
