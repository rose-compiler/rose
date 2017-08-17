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

class CommandLineOptions 
#ifdef USE_SAWYER_COMMANDLINE
  : public Sawyer::CommandLine::Boost::variables_map
#else
  : public boost::program_options::variables_map
#endif
  {
public:
  bool isSet(std::string option);
  void setOption(std::string option, bool value);
};

extern CommandLineOptions args; // defined in CommandLineOptions.C

extern int option_debug_mode;
#endif
