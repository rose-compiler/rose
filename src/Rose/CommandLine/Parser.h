#ifndef ROSE_CommandLine_Parser_H
#define ROSE_CommandLine_Parser_H

#include <Rose/Color.h>

namespace Rose {
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
 *  See also, @ref createEmptyParser, @ref genericSwitches. */
ROSE_DLL_API Sawyer::CommandLine::Parser createEmptyParserStage(const std::string &purpose, const std::string &description);

/** Generic command-line components.
 *
 *  Returns a description of the switches that should be available for all ROSE tools. For consistency's sake, most tools will
 *  want to have at least this set of switches which is intended to be common across all tools. These switches fall into some
 *  categories:
 *
 *  @li Actions: switches that cause some special action to be performed such as showing the documentation or version number,
 *      or running self-tests. If such a switch is specified, its action is performed instead of the tools normal action.
 *
 *  @li Adjustments: switches that cause a tool-wide adjustment in behavior, such as how internal program logic errors are
 *      handled or what diagnostic facilities are enabled.
 *
 *  @li Defaults: switches that provide default values for multiple software components, such as the maximum number of threads
 *      that a parallel analysis can use, or the name of the default SMT solver connection. These defaults are generic in the
 *      sense that they don't prescribe requirements for all components--they only provide defaults for those components that
 *      don't otherwise have a command-line setting.
 *
 *  To make a command-line parser that recognizes these switches, add the switches to the parser using its @c with method.  For
 *  example, here's how to construct a parser that recognizes only these switches:
 *
 * @code
 *  static Sawyer::CommandLine::ParserResult
 *  parseCommandLine(int argc, char *argv[]) {
 *      Sawyer::CommandLine::Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
 *      return parser
 *          .with(Rose::CommandLine::genericSwitches()) // these generic switches
 *          .with(mySwitches)                               // my own switches, etc.
 *          .parse(argc, argv)                              // parse without side effects
 *          .apply();                                       // apply parser results
 *  }
 * @endcode
 *
 *  In general, we want all tools to have all these switches. At a minimum, a tool developer should be aware that the switches
 *  in this group are in some sense reserved across all tools and should not be circumvented for other purposes. However, if a
 *  tool doesn't use a switch, the developer can remove that switch from the parser and its documentation in order to prevent
 *  user confusion. Here's an example of removing the "--threads" switch from a parser for a tool that doesn't support multiple
 *  threads:
 *
 * @code
 *  Sawyer::CommandLine::Parser p = createParser(purpose, description).with(genericSwitches());
 *  p.removeMatchingSwitch("--threads=1"); // must parse
 * @endcode
 *
 *  If you encounter strange errors near this call, make sure you're using -pthread consistently in your compile and link
 *  commands. Its presence or absence should be the same as however the ROSE library itself was compiled and linked. Mixing up
 *  the -pthread switch creates ABI incompatibilities that manifest themselves in various ways that usually look like a problem
 *  with a function that's called from a program that uses librose: often a segmentation fault, but can also be hangs,
 *  incorrect results, etc.  Note that -pthread is both a compile and a link switch.
 *
 *  See any recent tool for more examples.
 *
 *  See also, @ref createEmptyParser, @ref createEmptyParserStage. */
ROSE_DLL_API Sawyer::CommandLine::SwitchGroup genericSwitches();

/** Type for storing generic switch arguments.
 *
 *  For instance, the "--threads=N" switch takes an integer that should be stored somewhere.
 *
 *  See also, @ref genericSwitchArgs. */
struct GenericSwitchArgs {
    unsigned int threads;                               /**< Number of threads analyses should use. Zero means use the number
                                                         *   of threads that the hardware provides. */
    std::string smtSolver;                              /**< Name of SMT solver interface. "list" means show a list and exit.
                                                         *   The empty string means no solver is used. Additional switches
                                                         *   might be present to override this global solver for specific
                                                         *   situations. */
    bool errorIfDisabled;                               /**< Controls behavior of a tool when disabled. If true (the default)
                                                         *   and a tool's primary feature set is disabled (such as when ROSE is
                                                         *   compiled with too old a compiler or without the necessary
                                                         *   supporting software packages), then the tool should emit an error
                                                         *   message and exit with a failure status. When this data member is
                                                         *   false, then the tool will silently exit with success, which is
                                                         *   useful during "make check" or similar testing. */
    Color::Colorization colorization;                   /**< Controls colorized output. */
    std::vector<std::string> architectureLibraries;     /**< List of directories containing architecture definition libraries. */

    GenericSwitchArgs()
        : threads(0), smtSolver("none"), errorIfDisabled(true) {}
};

/** Global location for parsed generic command-line switches.
 *
 *  This global variable holds the results of command-line parsing using @ref genericSwitches.  Normally these settings are
 *  passed per command-line parsing request, but the interface in ROSE doesn't have that ability yet, so we use a global
 *  variable.
 *
 *  See also, @ref genericSwitches. */
ROSE_DLL_API extern GenericSwitchArgs genericSwitchArgs;

/** Convenience for for adding Boolean switches.
 *
 *  Adds "--foo" (if @p switchName is "foo") and "--no-foo" to the specified switch group. The storage location's lifetime must
 *  extend to the point where the command-line is parsed.  This function adds additional documentation describing how to
 *  disable the switch using "--no-foo" and what the default is (current value of storage location).
 *
 *  An alternative is to use a switch that takes a Boolean argument (e.g., "--foo=yes" or "--foo=no"), but this is more
 *  difficult for users to remember and type than just "--foo" and "--no-foo".
 *
 *  See also, @ref createEmptyParser, @ref createEmptyParserStage, @ref insertEnableSwitch.
 *
 * @{ */
ROSE_DLL_API void insertBooleanSwitch(Sawyer::CommandLine::SwitchGroup&, const std::string &switchName,
                                      bool &storageLocation, const std::string &documentation);
ROSE_DLL_API void insertBooleanSwitch(Sawyer::CommandLine::SwitchGroup&, const std::string &switchName,
                                      Sawyer::Optional<bool> &storageLocation, const std::string &documentation);
/** @} */

/** Convenience for adding enable/disable switches.
 *
 *  Inserts a switch for enabling or disabling something. This adds two switches named "enable-{name}" and "disable-{name}" both
 *  keyed to the "enable-{name}" string. If @p what is the empty string, then the switches are named only "enable" and "disable"
 *  and the key is "enable".
 *
 *  The @p storageLocation contains an initial value and is updated when the parser results are applied. It must outlive the
 *  parser.
 *
 *  Only one of the switches appears in the documentation: if the initial storage location is true, then the "disable" switch is
 *  documented, otherwise the "enable" switch is documented. In any case, the documented switch mentions the undocumented switch.
 *  Part of the documentation is generated automatically, and part is supplied by the @p documentation argument. The switch
 *  documentation will start with a sentence like "Enables {thing}." or "Disables {thing}" depending on the initial value in the
 *  @p storageLocation (see above). This is followed by the user-supplied @p documentation. Finally a sentence is added to mention
 *  the opposite switch. The @p thing should be a singular noun or singular noun phrase. */
ROSE_DLL_API void insertEnableSwitch(Sawyer::CommandLine::SwitchGroup&, const std::string &name, const std::string &thing,
                                     bool &storageLocation, const std::string &documentation);

} // namespace
} // namespace

#endif
