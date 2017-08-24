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
  bool isDefined(std::string option);
  bool isDefaulted(std::string option);
  bool isUserProvided(std::string option);
  bool getBool(std::string option);
  int getInt(std::string option);
  std::string getString(std::string option);
  template<typename T> void setOption(std::string option, T value);
};

extern CommandLineOptions args; // defined in CommandLineOptions.C

extern int option_debug_mode;


// template function implementation
#include "CodeThornException.h"
using namespace std;

template<typename T>
void CommandLineOptions::setOption(string option, T value) {
  if (count(option) < 1) {
    throw CodeThorn::Exception("Trying to set command line option \"" + option + "\" that does not exist.");
  }
  const_cast<boost::program_options::variable_value&>(operator[](option)) = 
    boost::program_options::variable_value(boost::any(value), false);
}
#endif

