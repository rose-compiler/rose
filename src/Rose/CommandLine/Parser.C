#include <sage3basic.h>                                 // needed by utility_functions.h
#include <Rose/CommandLine/Parser.h>

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/CommandLine/SelfTest.h>
#include <Rose/CommandLine/License.h>
#include <Rose/CommandLine/Version.h>
#include <Rose/Diagnostics.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/SmtCommandLine.h>
#endif
#include <Sawyer/CommandLine.h>

#include <processSupport.h>                             // ROSE
#include <rose_paths.h>
#include <rose_config.h>
#include <utility_functions.h>                          // ROSE

#include <iostream>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace CommandLine {

// Adjust the behavior for failed assertions based on the command-line
class FailedAssertionBehaviorAdjuster: public Sawyer::CommandLine::SwitchAction {
protected:
    FailedAssertionBehaviorAdjuster() {}
public:
    typedef Sawyer::SharedPointer<FailedAssertionBehaviorAdjuster> Ptr;
    enum Behavior { ABORT_ON_FAILURE, EXIT_ON_FAILURE, THROW_ON_FAILURE };
    static Ptr instance() {
        return Ptr(new FailedAssertionBehaviorAdjuster);
    }
protected:
    void operator()(const Sawyer::CommandLine::ParserResult &cmdline) {
        ASSERT_require(cmdline.have("assert"));
        Sawyer::Assert::AssertFailureHandler handler = NULL;
        switch (cmdline.parsed("assert", 0).as<Behavior>()) {
            case ABORT_ON_FAILURE: handler = abortOnFailedAssertion; break;
            case EXIT_ON_FAILURE:  handler = exitOnFailedAssertion; break;
            case THROW_ON_FAILURE: handler = throwOnFailedAssertion; break;
        }
        failedAssertionBehavior(handler);
    }
};

ROSE_DLL_API Sawyer::CommandLine::Parser
createEmptyParser(const std::string &purpose, const std::string &description) {
    Sawyer::CommandLine::Parser parser;
    parser.purpose(purpose);
    if (!description.empty())
        parser.doc("Description", description);
    parser.chapter(1, "ROSE Command-line Tools");
#if defined(ROSE_PACKAGE_VERSION)
    std::string v = ROSE_PACKAGE_VERSION;
#elif defined(PACKAGE_VERSION)
    std::string v = PACKAGE_VERSION;
#else
    std::string v = std::string(ROSE_SCM_VERSION_ID).substr(0, 8);
#endif
    parser.version(v, ROSE_CONFIGURE_DATE);
    parser.groupNameSeparator(":");                     // ROSE's style is "--rose:help" rather than "--rose-help"
    parser.errorStream(Diagnostics::mlog[FATAL]);       // probably overridden by individual tools

    parser.environmentVariable("ROSE_ARGS");
    parser.doc("Environment variables",
               "The ROSE_ARGS environment variable contains a string which is prepended to the command-line arguments "
               "in order to supply default command-line switches on a per-user basis for all ROSE tools that use the "
               "Sawyer-based parser. The string is split at white space boundaries, but quotes can be used to protect "
               "white space from splitting. When setting the environment variable from a shell, you may need to protect "
               "the quotes from the shell itself with additional quoting. A common use of the environment variable is to "
               "specify whether output should be colorized, and whether to use dark or light foreground colors.");

    return parser;
}

ROSE_DLL_API Sawyer::CommandLine::Parser
createEmptyParserStage(const std::string &purpose, const std::string &description) {
    return createEmptyParser(purpose, description).skippingNonSwitches(true).skippingUnknownSwitches(true);
}

// Global place to store result of parsing genericSwitches.
GenericSwitchArgs genericSwitchArgs;

// Helper for --architectures switch
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
class ArchitectureLoader: public Sawyer::CommandLine::SwitchAction {
public:
    using Ptr = Sawyer::SharedPointer<ArchitectureLoader>;

    static Ptr instance() {
        return Ptr(new ArchitectureLoader);
    }

protected:
    void operator()(const Sawyer::CommandLine::ParserResult &cmdline) {
        ASSERT_always_require(cmdline.have("architectures"));
        BinaryAnalysis::Architecture::registerDefinitions(genericSwitchArgs.architectureLibraries.begin(),
                                                          genericSwitchArgs.architectureLibraries.end());
    }
};
#endif

// Returns command-line description for switches that should be always available.
// Don't add anything to this that might not be applicable to some tool -- this is for all tools, both source and binary.
// See header file for more documentation including examples.
ROSE_DLL_API Sawyer::CommandLine::SwitchGroup
genericSwitches() {
    using namespace Sawyer::CommandLine;
    SwitchGroup gen("General switches");

    gen.insert(Switch("help", 'h')
               .doc("Show this documentation.")
               .action(showHelpAndExit(0)));

    gen.insert(Switch("color")
               .argument("how", Color::colorizationParser(genericSwitchArgs.colorization), "on,dark")
               .whichValue(SAVE_AUGMENTED)
               .valueAugmenter(Color::ColorizationMerge::instance())
               .doc("Whether to use color in the output, and the theme to use. " +
                    Color::ColorizationParser::docString() +
                    " The @s{color} switch with no argument is the same as @s{color}=on,dark, and @s{no-color} is "
                    "shorthand for @s{color}=off."));
    gen.insert(Switch("no-color")
               .key("color")
               .intrinsicValue("off", Color::colorizationParser(genericSwitchArgs.colorization))
               .hidden(true));

    gen.insert(Switch("log")
               .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
               .argument("config")
               .whichValue(SAVE_ALL)
               .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));

    gen.insert(Switch("quiet", 'q')
               .action(configureDiagnosticsQuiet(Sawyer::Message::mfacilities))
               .doc("Turn off all diagnostic output except the error and fatal levels. This is identical to saying "
                    "\"--log='none,>=error'\"."));

    gen.insert(Switch("version-long")
               .action(showVersionAndExit(version_message(), 0))
               .doc("Shows version information for ROSE and various dependencies and then exits. The shorter @s{version} "
                    "switch shows only the dotted quad of the ROSE library itself."));

    gen.insert(Switch("version", 'V')
               .action(showVersionAndExit(versionString, 0))
               .doc("Shows the version and then exits.  See also @s{version-long}, which prints much more "
                    "information about the ROSE library and supporting software."));

    // Control how a failing assertion acts. It could abort, exit with non-zero, or throw Rose::Diagnostics::FailedAssertion.
    gen.insert(Switch("assert")
               .action(FailedAssertionBehaviorAdjuster::instance())
               .argument("how", enumParser<FailedAssertionBehaviorAdjuster::Behavior>()
                         ->with("exit", FailedAssertionBehaviorAdjuster::EXIT_ON_FAILURE)
                         ->with("abort", FailedAssertionBehaviorAdjuster::ABORT_ON_FAILURE)
                         ->with("throw", FailedAssertionBehaviorAdjuster::THROW_ON_FAILURE))
               .doc("Determines how a failed assertion behaves.  The choices are \"abort\", \"exit\" with a non-zero value, "
                    "or \"throw\" a Rose::Diagnostics::FailedAssertion exception. The default behavior depends on how ROSE "
                    "was configured."));

    gen.insert(Switch("threads", 'j')
               .argument("n", nonNegativeIntegerParser(genericSwitchArgs.threads))
               .doc("Number of threads to use for algorithms that support multi-threading.  The default is " +
                    StringUtility::numberToString(genericSwitchArgs.threads) + ". A value of zero means use the "
                    "same number of threads as there is hardware concurrency (or one thread if the hardware "
                    "concurrency can't be determined)."));

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    // Global SMT solver name. This is used by any analysis that needs a solver and for which the user hasn't told that
    // specific analysis which solver to use. Specific analyses may override this global solver with other command-line
    // switches. The value "list" means generate a list of available solvers. So far this is only implemented for the SMT
    // solvers used by binary analysis, but the intention is that it would be available for all parts of ROSE.
    gen.insert(Switch("smt-solver")
               .argument("name", anyParser(genericSwitchArgs.smtSolver))
               .action(BinaryAnalysis::SmtSolverValidator::instance())
               .doc(BinaryAnalysis::smtSolverDocumentationString(genericSwitchArgs.smtSolver)));
#endif

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    gen.insert(Switch("architectures")
               .argument("names", listParser(anyParser(genericSwitchArgs.architectureLibraries), ":"))
               .explosiveLists(true)
               .whichValue(SAVE_ALL)
               .action(ArchitectureLoader::instance())
               .doc("List of shared libraries that define instruction set architectures for binary analysis. The argument should "
                    "be a list of one or more names separated by colons, and if this switch appears multiple times then the full "
                    "list is the concatenation of the individual lists. Each item in the list is:"

                    "@bullet{A name of an existing directory, in which case that item is expanded to a list of file names "
                    "to be processed as described below, except that no errors are reported for files that cannot be loaded. "
                    "Directories are not searched recursively.}"

                    "@bullet{A file name containing at least one slash (\"/\") character. These names are passed to "
                    "@man{dlopen}{3} directly, which will search for them as named (absolute, or relative to the current "
                    "working directory). If the file cannot be loaded, then an error is reported unless the file name was "
                    "expanded from a directory as mentioned above.}"

                    "@bullet{A library name containing no slashes. The names are first passed to @man{dlopen}{3}, which may search "
                    "various directories to find the library. If the library cannot be found with the specified name, then three "
                    "additional variations are tried by appending \".so\", prepending \"lib\", and doing both. If none of the "
                    "four forms of the name can be loaded, then an error is reported.}"

                    "Once a library is found and loaded, its @c{registerArchitectures} function is called, if any. This "
                    "function should have C linkage, not take any arguments, and not return any value. It should register "
                    "architecture definitions with the ROSE library by calling "
                    "@c{Rose::BinaryAnalysis::Architecture::registerDifinition}. Absence of this function or failure to register "
                    "any definitions is not an error, but may cause a warning message."));
#endif

    gen.insert(Switch("self-test")
               .action(SelfTests::instance())
               .doc("Instead of doing any real work, run any self tests registered with this tool then exit with success "
                    "or failure status depending on whether all such tests pass."));

    gen.insert(Switch("license")
               .action(ShowLicenseAndExit::instance())
               .doc("Show the ROSE software license and exit."));

    // This undocumented switch is used for internal testing during "make check" and similar. If a tool is disabled due to ROSE
    // being compiled with too old a compiler or without some necessary software prerequisite, then the tool will print an
    // error message that it is disabled but will still exit with a successful status (i.e., main returns zero).
    gen.insert(Switch("no-error-if-disabled")
               .intrinsicValue(false, genericSwitchArgs.errorIfDisabled)
               .hidden(true));

    return gen;
}

ROSE_DLL_API void
insertBooleanSwitch(Sawyer::CommandLine::SwitchGroup &sg, const std::string &switchName, bool &storageLocation,
                    const std::string &documentation) {
    using namespace Sawyer::CommandLine;

    ASSERT_forbid2(boost::starts_with(switchName, "-"), "specify only the name, not the prefix");

    std::string defaults = " This can be disabled with @s{no-" + switchName + "}. The default is " +
                           (storageLocation ? "yes" : "no") + ".";

    sg.insert(Switch(switchName)
              .intrinsicValue(true, storageLocation)
              .doc(documentation + defaults));
    sg.insert(Switch("no-"+switchName)
              .key(switchName)
              .intrinsicValue(false, storageLocation)
              .hidden(true));
}

} // namespace
} // namespace
