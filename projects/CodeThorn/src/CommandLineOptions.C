#include "sage3basic.h"
#include "CommandLineOptions.h"
#include "CodeThornException.h"

#include <sstream>
#include <iostream>

using namespace std;
/////////////////////////////////////////////////
// Command line processing global options
/////////////////////////////////////////////////

namespace CodeThorn {

  /////////////////////////////////////////////////

  bool CommandLineOptions::isDefined(string option) {
    return (find(option) != end());
  }

  bool CommandLineOptions::isDefaulted(string option) {
    ROSE_ASSERT(isDefined(option));
    return (*find(option)).second.defaulted();
  }

  bool CommandLineOptions::isUserProvided(string option) {
    return (isDefined(option) && !isDefaulted(option));
  }

  bool CommandLineOptions::getBool(string option) {
    if (!isDefined(option)) {
      throw CodeThorn::Exception("Boolean command line option \"" + option + "\" accessed that is not defined.");
    }
    CommandLineOptions::iterator iter = find(option);
    try { 
      return iter->second.as<bool>();
    } catch(...) {
      throw CodeThorn::Exception("Command line option \"" + option + "\" accessed as Boolean value, but has different type.");
    }
  }

  int CommandLineOptions::getInt(string option) {
    if (!isDefined(option)) {
      throw CodeThorn::Exception("Integer command line option \"" + option + "\" accessed that is not defined.");
    }
    CommandLineOptions::iterator iter = find(option);
    try { 
      return iter->second.as<int>();
    } catch(...) {
      throw CodeThorn::Exception("Command line option \"" + option + "\" accessed as integer value, but has different type.");
    }
  }

  std::vector<int> CommandLineOptions::getIntVector(string option) {
    if (!isDefined(option)) {
      throw CodeThorn::Exception("Integer command line option \"" + option + "\" accessed that is not defined.");
    }
    CommandLineOptions::iterator iter = find(option);
    try { 
      return iter->second.as< std::vector<int> >();
    } catch(...) {
      throw CodeThorn::Exception("Command line option \"" + option + "\" accessed as integer value, but has different type.");
    }
  }

  long int CommandLineOptions::getLongInt(string option) {
    if (!isDefined(option)) {
      throw CodeThorn::Exception("Integer command line option \"" + option + "\" accessed that is not defined.");
    }
    CommandLineOptions::iterator iter = find(option);
    try { 
      return iter->second.as<long int>();
    } catch(...) {
      throw CodeThorn::Exception("Command line option \"" + option + "\" accessed as integer value, but has different type.");
    }
  }

  string CommandLineOptions::getString(string option) {
    if (!isDefined(option)) {
      throw CodeThorn::Exception("String command line option \"" + option + "\" accessed that is not defined.");
    }
    CommandLineOptions::iterator iter = find(option);
    try { 
      return iter->second.as<string>();
    } catch(...) {
      throw CodeThorn::Exception("Command line option \"" + option + "\" accessed as string value, but has different type.");
    }
  }
  vector<string> CommandLineOptions::getStringVector(string option) {
    if (!isDefined(option)) {
      throw CodeThorn::Exception("String command line option \"" + option + "\" accessed that is not defined.");
    }
    CommandLineOptions::iterator iter = find(option);
    try { 
      return iter->second.as< vector<string> >();
    } catch(...) {
      throw CodeThorn::Exception("Command line option \"" + option + "\" accessed as string value, but has different type.");
    }
  }
  void CommandLineOptions::parse(int argc, char * argv[], po::options_description all) {
    po::store(po::command_line_parser(argc, argv).options(all).run(), args);
    //                                                        .allow_unregistered()
    po::notify(args);
  }
  void CommandLineOptions::parse(int argc, char * argv[], po::options_description all, po::options_description configFileOptions) {
    po::store(po::command_line_parser(argc, argv).options(all).run(), args);
    //                                                        .allow_unregistered()
    po::notify(args);
    
    if (args.isUserProvided("config")) {
      ifstream configStream(args.getString("config").c_str());
      // passing *this allows access to private data members of inherited class
        po::store(po::parse_config_file(configStream, configFileOptions), *this);
        po::notify(args);
    } 
  }
}
// TODO: move to CodeThornCommandLineOptions, once all args
// references are removed from codethorn library.
CodeThorn::CommandLineOptions CodeThorn::args;

