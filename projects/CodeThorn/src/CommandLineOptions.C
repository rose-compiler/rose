#include "CommandLineOptions.h"
#include "CodeThornException.h"

#include <sstream>
#include <iostream>

using namespace std;
/////////////////////////////////////////////////
// Command line processing global options
/////////////////////////////////////////////////

CommandLineOptions args;

/////////////////////////////////////////////////

bool CommandLineOptions::isSet(string option) {
  if (count(option) < 1) {
    throw CodeThorn::Exception("Boolean command line option \"" + option + "\" accessed that does not exist.");
  }
  CommandLineOptions::iterator iter = find(option);
  try { 
    return iter->second.as<bool>();
  } catch(...) {
    throw CodeThorn::Exception("Command line option \"" + option + "\" accessed as Boolean value, but has different type.");
  }
}

void CommandLineOptions::setOption(string option, bool value) {
  if (count(option) < 1) {
    throw CodeThorn::Exception("Trying to set Boolean command line option \"" + option + "\" that does not exist.");
  }
  const_cast<boost::program_options::variable_value&>(operator[](option)) = 
    boost::program_options::variable_value(boost::any(value), false);
}
