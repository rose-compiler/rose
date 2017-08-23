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
