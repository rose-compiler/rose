#ifndef CODETHORN_COMMAND_LINE_OPTIONS_H
#define CODETHORN_COMMAND_LINE_OPTIONS_H

#include "CommandLineOptions.h"

#include "CodeThornOptions.h"

// to be move into other tool
#include "LTLOptions.h"
// to be move into other tool
#include "ParProOptions.h"

CodeThorn::CommandLineOptions& parseCommandLine(int argc, char* argv[], Sawyer::Message::Facility logger, std::string version,
                                                CodeThornOptions& ctOpt, LTLOptions& ltlOpt, ParProOptions& parProOpt);
void checkSpotOptions(LTLOptions& ltlOpt, ParProOptions& parProOpt);
void checkZ3Options(CodeThornOptions& ctOpt);

#endif
