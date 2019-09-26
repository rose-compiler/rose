#include <sage3basic.h>
#include <CommandLine.h>
#include <Diagnostics.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <BinarySmtCommandLine.h>
#endif

#include <boost/algorithm/string/predicate.hpp>

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

// Run self tests from the command-line, then exit.
class SelfTests: public Sawyer::CommandLine::SwitchAction {
protected:
    SelfTests() {}

public:
    typedef Sawyer::SharedPointer<SelfTests> Ptr;

    static Ptr instance() {
        return Ptr(new SelfTests);
    }

protected:
    void operator()(const Sawyer::CommandLine::ParserResult &cmdline) {
        ASSERT_require(cmdline.have("self-test"));
        runSelfTestsAndExit();
    }
};

std::vector<SelfTest::Ptr> selfTests;

const char *licenseText =
#include "license.h"
;

// Show license text and exit.
class ShowLicenseAndExit: public Sawyer::CommandLine::SwitchAction {
protected:
    ShowLicenseAndExit() {}

public:
    typedef Sawyer::SharedPointer<ShowLicenseAndExit> Ptr;

    static Ptr instance() {
        return Ptr(new ShowLicenseAndExit);
    }

protected:
    void operator()(const Sawyer::CommandLine::ParserResult&) {
        std::cout <<licenseText;
        exit(0);
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
    return parser;
}

ROSE_DLL_API Sawyer::CommandLine::Parser
createEmptyParserStage(const std::string &purpose, const std::string &description) {
    return createEmptyParser(purpose, description).skippingNonSwitches(true).skippingUnknownSwitches(true);
}

// Global place to store result of parsing genericSwitches.
GenericSwitchArgs genericSwitchArgs;

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
#if defined(ROSE_PACKAGE_VERSION)
               .action(showVersionAndExit(ROSE_PACKAGE_VERSION, 0))
#elif defined(PACKAGE_VERSION)
               .action(showVersionAndExit(PACKAGE_VERSION, 0))
#else
               .action(showVersionAndExit("unknown", 0))
#endif
               .doc("Shows the dotted quad ROSE version and then exits.  See also @s{version-long}, which prints much more "
                    "information."));

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

    gen.insert(Switch("threads")
               .argument("n", nonNegativeIntegerParser(genericSwitchArgs.threads))
               .doc("Number of threads to use for algorithms that support multi-threading.  The default is " +
                    StringUtility::numberToString(genericSwitchArgs.threads) + ". A value of zero means use the "
                    "same number of threads as there is hardware concurrency (or one thread if the hardware "
                    "concurrency can't be determined)."));

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
    // Global SMT solver name. This is used by any analysis that needs a solver and for which the user hasn't told that
    // specific analysis which solver to use. Specific analyses may override this global solver with other command-line
    // switches. The value "list" means generate a list of available solvers. So far this is only impelemented for the SMT
    // solvers used by binary analysis, but the intention is that it would be available for all parts of ROSE.
    gen.insert(Switch("smt-solver")
               .argument("name", anyParser(genericSwitchArgs.smtSolver))
               .action(BinaryAnalysis::SmtSolverValidator::instance())
               .doc(BinaryAnalysis::smtSolverDocumentationString(genericSwitchArgs.smtSolver)));
#endif

    gen.insert(Switch("self-test")
               .action(SelfTests::instance())
               .doc("Instead of doing any real work, run any self tests registered with this tool then exit with success "
                    "or failure status depending on whether all such tests pass."));

    gen.insert(Switch("license")
               .action(ShowLicenseAndExit::instance())
               .doc("Show the ROSE software license and exiit."));

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

ROSE_DLL_API void
runSelfTestsAndExit() {
    using namespace Rose::Diagnostics;

    // Run each test sequentially
    size_t npass=0, nfail=0;
    BOOST_FOREACH (const SelfTest::Ptr &test, selfTests) {
        if (test) {
            mlog[DEBUG] <<"running self test \"" <<StringUtility::cEscape(test->name()) <<"\"...\n";
            if ((*test)()) {
                mlog[INFO] <<"passed: self test \"" <<StringUtility::cEscape(test->name()) <<"\"\n";
                ++npass;
            } else {
                mlog[ERROR] <<"failed: self test \"" <<StringUtility::cEscape(test->name()) <<"\"\n";
                ++nfail;
            }
        }
    }

    // Report results and exit
    if (npass + nfail == 0) {
        mlog[INFO] <<"no self tests available for this tool\n";
        exit(0);
    } else if (nfail > 0) {
        mlog[FATAL] <<StringUtility::plural(nfail, "self tests") <<" failed; "
                    <<StringUtility::plural(npass, "self tests") <<" passed\n";
        exit(1);
    } else {
        mlog[INFO] <<"all self tests pass\n";
        exit(0);
    }
}

} // namespace
} // namespace
