#ifndef CODETHORN_COMMAND_LINE_OPTIONS_H
#define CODETHORN_COMMAND_LINE_OPTIONS_H

#include "CommandLineOptions.h"

CodeThorn::CommandLineOptions& parseCommandLine(int argc, char* argv[], Sawyer::Message::Facility logger, std::string version);

#endif
