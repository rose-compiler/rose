#ifndef CODETHORN_COMMAND_LINE_OPTIONS_H
#define CODETHORN_COMMAND_LINE_OPTIONS_H

#include "CommandLineOptions.h"

CodeThorn::CommandLineOptions& parseCommandLine(int argc, char* argv[], Sawyer::Message::Facility logger);

#ifdef USE_SAWYER_COMMANDLINE
namespace po = Sawyer::CommandLine::Boost;
#else
namespace po = boost::program_options;
#endif

#endif
