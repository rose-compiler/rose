#ifndef ROSE_CommandLine_H
#define ROSE_CommandLine_H

#include <Rose/CommandLine/DurationParser.h>
#include <Rose/CommandLine/IntervalParser.h>
#include <Rose/CommandLine/License.h>
#include <Rose/CommandLine/Parser.h>
#include <Rose/CommandLine/SelfTest.h>
#include <Rose/CommandLine/SuffixMultiplierParser.h>
#include <Rose/CommandLine/Version.h>

namespace Rose {

/** Command line parsing.
 *
 *  This namespace encapsulates those things necessary for parsing the command-line for a ROSE-based tool. Command-lines
 *  consist of switches and their arguments, and positional arguments. The command-line parsers in this class are mainly for
 *  parsing the switches and their arguments, while the remaining non-switch, positional arguments can be retreived for parsing
 *  in a per-tool specific manner.
 *
 *  Command-line switches consist of an introductory sequence of characters (usually a hyphen or two) followed by the switch
 *  name. If the switch takes an argument (or arguments), that argument can be specified either as a separate entry in the
 *  program argument list (usually "argv") or separated from the switch name with an equal sign. On Unix systems, switches can
 *  also be single letters, can nestle together, and can take arguments either in the same @p argv entry or in the following
 *  entry.
 *
 *  Related switches are collected into switch groups which also have names. The name of the group is usually precedes the
 *  switch name and is separated from it by a colon.  Group names are optional on the command-line if the switch name is
 *  unambiguous.
 *
 *  All of these behaviors can be adjusted using the underlying @ref Sawyer::CommandLine API.
 *
 *  Parsers come in two flavors. The first flavor, "normal parsers", are suitable for programs that are able to define all
 *  their legal switches. They parse command-line switches and until they get to the first positional argument, or they can be
 *  configured to collect and skip over the positional arguments if desired.
 *
 *  The second flavor of parser, a "parser stage", is for programs that are unable or unwilling to define all their
 *  switches. In this mode, if a program argument has a prefix (e.g., "-") that looks like a switch but hasn't been defined as
 *  one, then it's considered to be an unrecognized switch and that single program argument is consumed and stored in an
 *  unrecognized switches list. Note that this is not a rigorous way to parse command-lines since the parser has no knowledge
 *  of how many arguments a switch takes.  For instance, if "-log" is a valid, defined switch that takes one argument, and
 *  "-yaxis" is undefined but takes as an argument a sign ("+" or "-") followed by the word "linear" or "log", then the command
 *  "a.out -yaxis -log foo bar" will be parsed as having an undefined switch "-yaxis" followed by the switch "-log" whose
 *  argument is "foo", followed by one positional argument "bar". The correct parsing, had yaxis been defined, would have been
 *  switch "-yaxis" whose argument is "-log" followed by two positional arguments "foo" and "bar".
 *
 *  This API is part of the ROSE library and can call any functions in the ROSE library. See also, CommandlineProcessing, which
 *  has only lower-level functions that cannot call other parts of the ROSE library. */
namespace CommandLine {

} // namespace
} // namespace

#endif
