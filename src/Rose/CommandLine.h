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
 *  This namespace encapsulates those things necessary for parsing the command-line for a ROSE-based tool. Command-lines consist of
 *  switches and their arguments, and positional arguments. The command-line parsers in this class are mainly for parsing the
 *  switches and their arguments, while the remaining non-switch, positional arguments can be retreived for parsing in a per-tool
 *  specific manner.
 *
 *  This API is built on @ref Sawyer::CommandLine, and more documentation and examples can be found there.
 *
 *  @section rose_commandline_user_perspective User perspective
 *
 *  The user's perspective of a command-line parser is the ability to specify command-line switches and arguments when running a
 *  ROSE tool, and to be able to obtain detailed documentation about the tool and its command-line. The goals of this API are
 *  therefore
 *
 *  @li To provide a consistent command-line experience across all the ROSE tools.
 *
 *  @li To provide a command-line syntax for ROSE tools that is similar to that of other common tools in the same operating system.
 *
 *  @li To provide a consistent mechanism for obtaining tool-specific, version-specific documentation about a command-line.
 *
 *  @li To present the tool documentation in a consistent style.
 *
 *  This API concentrates primarily on the parsing of command-line switches and their arguments, and not much on the parsing of
 *  non-switch, positional arguments.
 *
 *  A command-line switch consists of an introductory sequence of characters (usually a hyphen or two) followed by the switch
 *  name. If the switch takes an argument, the switch argument can be separated from the switch name with an equal sign or appear as
 *  its own command-line argument.
 *
 *  <b>Examples of command-line switches from the user's perspective:</b>
 *
 *  @li "--help" is a Unix-like command-line switch whose name is "help". Multi-character switches are customarily introduced with a
 *  double hyphen. A tool running on Windows might use "/help" instead of "--help".
 *
 *  @li "-h" is a single-letter switch perhaps being shorthand for "--help". Single-character switches are customarily introduced
 *  with a single hyphen.
 *
 *  @li "--maximum=5" is a multi-character switch named "maximum" taking an argument whose value is the string "5". In this case,
 *  the switch and its value are contained in a single command-line argument and separated from one another with an equal sign.
 *
 *  @li "--maximum 5" means the same thing as "--maximum=5" except the switch and its argument are stored in two separate
 *  command-line arguments, such as `argv[i]` contains "--max" and `argv[i+1]` contains "5".
 *
 *  @li "-M5" might be a single-letter switch "M" whose argument is "5". Or it might be the single-letter switch "M" taking no
 *  argument followed by the single-letter switch "5" also taking no argument.
 *
 *  @li" -hM5" might be the single-letter switch "h" taking no argument, followed by the single-letter switch "M" taking the
 *  argument "5". Or it might be the single-letter switch "h" taking the argument "M5". Or it might be three switches "h", "M", and
 *  "5" none of which take arguments.
 *
 *  @note As you can see, single-character switches are not very self-documenting and can be confusing to users. It is best for
 *  tools to use mostly multi-character switches, reserving single-letter switches for extremely common and often used purposes like
 *  "-h" for "--help" or "-V" for "--version".
 *
 *  The arguments for switches have their own sub-parsers. Predefined parsers exist for integers, non-negative integers,
 *  floating-point, colors, ranges, time durations, arbitrary strings, lists, enumerated strings, and more. If a switch doesn't have
 *  an explicit argument, it might have an implicit argument such as a Boolean to indicate whether it was specified. Implicit
 *  Boolean arguments are also often used for pairs of switches like "--verbose" vs. "--quiet", or "--optimize" vs. "--no-optimize".
 *
 *  Related switches (such as those for a single analysis in a tool having multiple analyses) are collected into switch groups which
 *  also have names. For instance, if a tool has a stack-delta analysis and a tainted-flow analysis then it may wisely place all the
 *  stack-delta switches into a group named "stack" and all the tainted-flow switches into a group named "taint". Since both
 *  analyses are a kind of data-flow, they likely both have a "--maximum-iterations" switch that takes a non-negative integer
 *  argument to limit the search space. If the user invokes the tool with "--maximum-iterations=10" then an error will be emitted to
 *  indicate that the switch is ambiguous. Instead, the user should qualify the switch names as "--stack:maximum-iterations=10" and
 *  "--taint:maximum-iterations=20".
 *
 *  @section rose_commandline_programmer_perspective Programmer perspective
 *
 *  From a programming perspective, this API's goals are:
 *
 *  @li To provide a way for any ROSE component (such as a particular analysis) to specify command-line switches that adjust its
 *  main settings.
 *
 *  @li To provide a way for each ROSE component to document its purpose and behavior in sufficient detail to be used appropriately
 *  from a command-line.
 *
 *  @li To provide a way for a tool author to easily mix and match command-line switches from multiple ROSE components, or even
 *  multiple instances of a single component.
 *
 *  @li To provide a way for a tool author to easily define new tool-specific switches and their documentation.
 *
 *  The @ref Rose::CommandLine API builds upon @ref Sawyer::CommandLine, which should be consulted for its API documentation and
 *  examples.  The ROSE switch prefixes, argument separators, group delimiters, nestling behavior, etc. have been chosen in such a
 *  way as to give users a consistent experience across all ROSE tools using a grammer whose parsing is sound. Although these
 *  defaults can be changed through the @ref Sawyer::CommandLine API, doing so for ROSE tools is discouraged.
 *
 *  The @ref Sawyer::CommandLine has two main modes of operation called "pull" and "push". In "pull" mode, one parses a command-line
 *  and then queries a result object to obtain information about the switches and their arguments. This is how many non-Sawyer
 *  command-line parsers work, but is not the preferred mechanism in ROSE tools. Instead, ROSE tools should use the "push" mode, in
 *  which the command-line switches and their arguments are parsed and then programmer-supplied C++ variables are automatically
 *  updated with the results. The "push" mode of operation is what we describe below.
 *
 *  Parsers also come in two flavors. The first flavor, "normal parsers" (see @ref createEmptyParser), are suitable for programs
 *  that are able to define all their legal switches. They parse command-line switches until they get to the first positional
 *  argument, or they can be configured to collect and skip over the positional arguments if desired. These parsers can be sound
 *  when they're able to distinguish between command-line switches and non-switch (positional) arguments and therefore this is the
 *  flavor your tools should strive to use. The other flavor, a "parser stage" (see @ref createEmptyParserStage), is for programs
 *  that are unable or unwilling to define all their switches, in which case the parser will skip over (and save for later) things
 *  that it doesn't understand. A parser stage is generally unsound since it's essentially parsing two languages at once while
 *  knowing the syntax for only one of them.
 *
 *  @note Here's an example demonstrating how a parser stage can be unsound. If "-log" is a defined switch that takes one argument,
 *  and "-yaxis" is undefined but takes as an argument a sign ("+" or "-") followed by the word "linear" or "log", then the command
 *  "a.out -yaxis -log foo bar" will be parsed as having an undefined switch "-yaxis" followed by the switch "-log" whose argument
 *  is "foo", followed by one positional argument "bar". The correct parsing, had yaxis been defined, would have been the switch
 *  "-yaxis" whose argument is "-log" followed by two positional arguments "foo" and "bar".
 *
 *  To meet the goals from a programmer's perspective, the following design should be used for ROSE-based tools and ROSE library
 *  components:
 *
 *  @li Each ROSE component (e.g., an analysis) should define a `Settings` struct that holds the settings that control how that
 *  component operates. These settings should all have reasonable default values. For instance, a data-flow analysis might have a
 *  data member `size_t maxIterations = 1000;` Each of these data members will also have a command-line switch.
 *
 *  @li Each ROSE component should define a `commandLineSwitches` function that generates and returns a @ref
 *  Sawyer::CommandLine::SwitchGroup "SwitchGroup" that defines and documents all of the command-line switches for that
 *  component. The switch group should also document the component in general (besides describing each switch). If this is a static
 *  member function, then its argument should be a `Settings` reference, otherwise it should adjust the settings in the object on
 *  which it was invoked.  The switch documentation should indicate what the default values are for the various switch arguments,
 *  and it can obtain those values from the settings (passed by argument or part of `this` component). The switch group should
 *  arrange to push parsed switched arguments to that same settings object.
 *
 *  @li Since documentation is generated on demand and uses the default values from various `Settings` structs, it is permissible
 *  for a tool to adjust the defaults before generating the command-line switch parser. For instance, the default SMT solver (@ref
 *  GenericSwitchArgs::smtSolver) is "none", but a tool might change the default to "best" before generating its command-line
 *  parser. Then, if the user doesn't specify an SMT solver with the "--smt-solver" switch, its value will still be "best" after
 *  parsing and the "--help" output may indicate that the default is "best".
 *
 *  @li A complex component might need to define more than one switch group, or need adjust the command-line parser in other
 *  ways. Such components may have a function to adjust a @ref Sawyer::CommandLine::Parser "parser" reference instead of the
 *  `commandLineSwitches` function(s) mentioned above.
 *
 *  @li ROSE has a number of globally useful command-line switches such as "--help", "--version", "--log", "--self-tests", etc.
 *  These switches are stored in a switch group obtained by calling @ref genericSwitches, and their parsed arguments are pushed to
 *  the global @ref genericSwitchArgs settings. By default, many of these also have certain actions that happen automatically if the
 *  switch is parsed (such as "--help" showing the documentation and then exiting the program).
 *
 *  @li A tool author that uses a ROSE component will create a command-line parser, and insert the component's switch definitions
 *  into that parser. The author may then adjust his parser by renaming switch groups; adjusting switch definitions; deleting or
 *  renaming switches; adding tool-specific switches; etc.
 *
 *  Once a tool creates a command-line switch parser, there are a few steps to use it. In particular, parsing a command-line is a
 *  separate operation from storing/pushing the results of the parse to the various settings data members. This allows one to test
 *  whether a command-line is valid without having any side effects that might be difficult to undo.  Parsing a command-line happens
 *  with the @ref Sawyer::CommandLine::Parser::parse "parse" method, which returns a @ref Sawyer::CommandLine::ParserResult
 *  "ParserResult" object. The ParserResult object is not normally used directly in the "push" mode of operation, except that the
 *  results are pushed to C++ variables with its @ref Sawyer::CommandLine::ParserResult::apply "apply" function. Finally, the tool
 *  needs to obtain the non-switch, positional arguments that appear after the switches. These steps are usually done in one
 *  statement like this:
 *
 *  @code
 *  // Given a command-line switch parser (parser) and a command-line (argc, argv):
 *  //   + try to parse the command-line without any side effects
 *  //   + if successful, try to push the switch arguments to their defined storage locations
 *  //     and also execute any defined actions (for things like "--help")
 *  //   + if successful, return the positional arguments that follow the switches
 *  //   + if an error occurs at any step, print an error message and fail
 *  std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();
 *  @endcode */
namespace CommandLine {

} // namespace
} // namespace

#endif
