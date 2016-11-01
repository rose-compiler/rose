#ifndef COMMAND_LINE_OPTIONS_H
#define COMMAND_LINE_OPTIONS_H

#include <string>
#include <map>
#include <cstdio>
#include <cstring>

//preparation for using the Sawyer command line parser
//#define USE_SAWYER_COMMANDLINE
#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

//namespace po = boost::program_options;

class BoolOptions {
public:
  BoolOptions();
  BoolOptions(int argc, char* argv[]);
  void init(int argc0, char* argv0[]);
  void registerOption(std::string name, bool defaultval);
  void setOption(std::string name, bool val);

  /* checks whether the argument with name 'name' (with no parameters)
     is specified on the command line and sets the value to true if it
     is detected. If name with the prefix 'no-' is specified it sets
     the value for 'name' to false. Note, this function does *not* set a
     default value. This must be set with registerOption.
  */
  void processZeroArgumentsOption(std::string name);

  void processOptions();
  bool operator[](std::string option);
  std::string toString();
private:
  int argc;
  char** argv;
  std::map<std::string,bool> mapping;
};

extern BoolOptions boolOptions; // defined in CommandLineOptions.C

#ifdef USE_SAWYER_COMMANDLINE
extern Sawyer::CommandLine::Boost::variables_map args;
#else
extern boost::program_options::variables_map args; // defined in CommandLineOptions.C
#endif

extern int option_debug_mode;
#endif
