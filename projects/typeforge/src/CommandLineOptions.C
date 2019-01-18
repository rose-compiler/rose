#include "sage3basic.h"
#include "CommandLineOptions.h"
#include "TypeforgeException.h"

#include <sstream>
#include <iostream>

using namespace std;
/////////////////////////////////////////////////
// Command line processing global options
/////////////////////////////////////////////////

CommandLineOptions args;

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
    throw Typeforge::Exception("Boolean command line option \"" + option + "\" accessed that is not defined.");
  }
  CommandLineOptions::iterator iter = find(option);
  try { 
    return iter->second.as<bool>();
  } catch(...) {
    throw Typeforge::Exception("Command line option \"" + option + "\" accessed as Boolean value, but has different type.");
  }
}

int CommandLineOptions::getInt(string option) {
  if (!isDefined(option)) {
    throw Typeforge::Exception("Integer command line option \"" + option + "\" accessed that is not defined.");
  }
  CommandLineOptions::iterator iter = find(option);
  try { 
    return iter->second.as<int>();
  } catch(...) {
    throw Typeforge::Exception("Command line option \"" + option + "\" accessed as integer value, but has different type.");
  }
}

string CommandLineOptions::getString(string option) {
  if (!isDefined(option)) {
    throw Typeforge::Exception("String command line option \"" + option + "\" accessed that is not defined.");
  }
  CommandLineOptions::iterator iter = find(option);
  try { 
    return iter->second.as<string>();
  } catch(...) {
    throw Typeforge::Exception("Command line option \"" + option + "\" accessed as string value, but has different type.");
  }
}
