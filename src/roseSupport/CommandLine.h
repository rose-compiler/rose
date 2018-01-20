#ifndef ROSE_CommandLine_H
#define ROSE_CommandLine_H

#include <Sawyer/CommandLine.h>

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

/** Empty command-line parser.
 *
 *  Returns a command-line parser that has no switch declarations, but is set up consistently for ROSE tools. The @p purpose
 *  should be an uncapitalized, short, single-line string that appears near the top of the man page. The @p description can be
 *  much longer, multiple paragraphs, free-format, with Sawyer markup. It will appear under the heading "Description" in the
 *  man page.
 *
 *  See also, @ref createEmptyParserStage. */
ROSE_DLL_API Sawyer::CommandLine::Parser createEmptyParser(const std::string &purpose, const std::string &description);

/** Empty command-line parser suitable for use with other parsers.
 *
 *  Returns a command-line parser that has no switch declarations, but is set up consistently for ROSE tools. The parser is
 *  configured to skip over any program arguments it doesn't recognize, with the assumption that those arguments will be passed
 *  to another parser.  This also means that this parser cannot report errors for misspelled or misused switches because it
 *  cannot tell whether the switch is misspelled or simply intended for the next parser.
 *
 *  The @p purpose should be an uncapitalized, short, single-line string that appears near the top of the man page. The @p
 *  description can be much longer, multiple paragraphs, free-format, with Sawyer markup. It will appear under the heading
 *  "Description" in the man page.
 *
 *  See also, @ref createEmptyParser. */
ROSE_DLL_API Sawyer::CommandLine::Parser createEmptyParserStage(const std::string &purpose, const std::string &description);

/** Generic command-line components.
 *
 *  Returns a description of the switches that should be available for all ROSE tools. To make a command-line parser that
 *  recognizes these switches, add the switches to the parser using its @c with method.  For example, here's how to construct a
 *  parser that recognizes only these switches:
 *
 * @code
 *  static Sawyer::CommandLine::ParserResult
 *  parseCommandLine(int argc, char *argv[]) {
 *      Sawyer::CommandLine::Parser parser = CommandlineProcessing::createEmptyParser(purpose, description);
 *      return parser
 *          .with(CommandlineProcessing::genericSwitches()) // these generic switches
 *          .with(mySwitches)                               // my own switches, etc.
 *          .parse(argc, argv)                              // parse without side effects
 *          .apply();                                       // apply parser results
 *  }
 * @endcode
 *
 *  If you encounter strange errors near this call, make sure you're using -pthread consistently in your compile and link
 *  commands. Its presence or absence should be the same as however the ROSE library itself was compiled and linked. Mixing up
 *  the -pthread switch creates ABI incompatibilities that manifest themselves in various ways that usually look like a problem
 *  with a function that's called from a program that uses librose: often a segmentation fault, but can also be hangs,
 *  incorrect results, etc.  Note that -pthread is both a compile and a link switch.
 *
 *  See any recent tool for more examples. */
ROSE_DLL_API Sawyer::CommandLine::SwitchGroup genericSwitches();

/** Type for storing generic switch arguments.
 *
 *  For instance, the "--threads=N" switch takes an integer that should be stored somewhere. */
struct GenericSwitchArgs {
    unsigned int threads;                               /**< Number of threads analyses should use. Zero means use the number
                                                         *   of threads that the hardware provides. */
    std::string smtSolver;                              /**< Name of SMT solver interface. "list" means show a list and exit.
                                                         *   The empty string means no solver is used. Additional switches
                                                         *   might be present to override this global solver for specific
                                                         *   situations. */

    GenericSwitchArgs()
        : threads(0), smtSolver("none") {}
};

/** Global location for parsed generic command-line switches.
 *
 *  This global variable holds the results of command-line parsing using @ref genericSwitches.  Normally these settings are
 *  passed per command-line parsing request, but the interface in ROSE doesn't have that ability yet, so we use a global
 *  variable. */
ROSE_DLL_API extern GenericSwitchArgs genericSwitchArgs;

/** Convenience for for adding Boolean switches.
 *
 *  Adds "--foo" (if @p switchName is "foo") and "--no-foo" to the specified switch group. The storage location's lifetime must
 *  extend to the point where the command-line is parsed.  This function adds additional documentation describing how to
 *  disable the switch using "--no-foo" and what the default is (current value of storage location).
 *
 *  An alternative is to use a switch that takes a Boolean argument (e.g., "--foo=yes" or "--foo=no"), but this is more
 *  difficult for users to remember and type than just "--foo" and "--no-foo". */
ROSE_DLL_API void insertBooleanSwitch(Sawyer::CommandLine::SwitchGroup&, const std::string &switchName,
                                      bool &storageLocation, const std::string &documentation);

} // namespace
} // namespace

#endif
