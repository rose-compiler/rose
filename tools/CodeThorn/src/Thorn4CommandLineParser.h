#ifndef THORN4_COMMAND_LINE_PARSER_H
#define THORN4_COMMAND_LINE_PARSER_H

#include "sage3basic.h"
#include <iostream>

#include "Rose/CommandLine.h"
#include "Sawyer/CommandLine.h"
#include "sageGeneric.h"

// Programmatic codethorn headers
#include "CodeThornLib.h"
#include "CodeThornOptions.h"
#include "TimeMeasurement.h"

#include "CppStdUtilities.h"

namespace si  = SageInterface;
namespace scl = Sawyer::CommandLine;

//const std::string thorn4version = "0.9.0";

class Thorn4Parser {
 public:
  std::vector<std::string> parseArgs(int argc, char** argv);
  CodeThornOptions getOptions();

 protected:
  /// sets the Thorn4Parser settings using the command line arguments
  /// \returns a list of unparsed arguments
  std::vector<std::string> parseArgs(std::vector<std::string> clArgs);
  /// sets multiple options based on one single option
  void processMultiSelectors();
 private:
  CodeThornOptions _ctOpt;
};

#endif
