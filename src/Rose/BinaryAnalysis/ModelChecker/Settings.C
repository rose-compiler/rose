#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>

#include <Rose/CommandLine.h>
#include <rose_isnan.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

Settings::Settings() {}
Settings::~Settings() {}

Settings::Ptr
Settings::instance() {
    return Ptr(new Settings);
}

Sawyer::CommandLine::SwitchGroup
Settings::commandLineSwitches() {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Model checker settings");

    Rose::CommandLine::insertBooleanSwitch(sg, "debug-initial-states", showInitialStates,
                                           "Show initial semantic states for each execution unit when "
                                           "Rose::BinaryAnalysis::ModelChecker(debug) diagnostics are enabled.");

    Rose::CommandLine::insertBooleanSwitch(sg, "debug-final-states", showFinalStates,
                                           "Show final semantic states for each execution unit when "
                                           "Rose::BinaryAnalysis::ModelChecker(debug) diagnostics are enabled.");

    Rose::CommandLine::insertBooleanSwitch(sg, "debug-intermediate-states", showIntermediateStates,
                                           "Show intermediate semantic states within execution units when "
                                           "Rose::BinaryAnalysis::ModelChecker(debug) diagnostics are enabled.");

    Rose::CommandLine::insertBooleanSwitch(sg, "debug-assertions", showAssertions,
                                           "Show SMT assertions at the beginning of each execution unit when "
                                           "Rose::BinaryAnalysis::ModelChecker(debug) diagnostics are enabled.");

    Rose::CommandLine::insertBooleanSwitch(sg, "ignore-semantic-failures", ignoreSemanticFailures,
                                           "Ignore all semantic failures when executing an instruction, and use whatever "
                                           "machine state resulted from the part of the execution that worked (if any). Normally, "
                                           "a semantic failure would cause the affected execution path to not be extended any "
                                           "farther.");

    Rose::CommandLine::insertBooleanSwitch(sg, "reject-unknown-insns", rejectUnknownInsns,
                                           "Reject paths that contain unknown instructions even if semantic failures are "
                                           "otherwise allowed.  An unknown instruction happens when the system is able to read "
                                           "the machine code for the instruction, but is unable to decode it to a valid "
                                           "instruction. This often happens when the system is trying to execute data areas of "
                                           "the memory, or if it's assuming an incorrect instruction set architecture.");

    sg.insert(Switch("max-path-length", 'k')
              .argument("nsteps", positiveIntegerParser(k))
              .doc("Maximum path length in steps before abandoning any further exploration. A step generally corresponds to "
                   "a single instruction for those execution units that have instructions. The default is " +
                   StringUtility::plural(k, "steps") + "."));

    sg.insert(Switch("max-path-time", 't')
              .argument("seconds", realNumberParser(maxTime))
              .doc("Maximum time in seconds to spend exploring any given path. If a path takes longer than the specified time, "
                   "then no paths with that prefix are explored. The default is " +
                   std::string(rose_isnan(maxTime) ? "to not limit the time." :
                               (1.0 == maxTime ? "1 second." :
                                (boost::format("%g seconds.") % maxTime).str()))));

    sg.insert(Switch("replace-symbolic")
              .argument("nnodes", nonNegativeIntegerParser(maxSymbolicSize))
              .doc("Maximum size of symbolic expressions before they're replaced by a new variable. A value of zero means do "
                   "not ever replace symbolic expressions with variables. Setting this to non-zero may make the analysis faster "
                   "at the cost of less precision. The default is " + boost::lexical_cast<std::string>(maxSymbolicSize) + "."));

    sg.insert(Switch("solver-timeout")
              .argument("seconds", realNumberParser(solverTimeout))
              .doc("Specifies the maximum amount of elapsed time in seconds for each call to the SMT solver. If this time "
                   "limit expires, the solver returns an answer of \"unknown\". The default is " +
                   (rose_isnan(solverTimeout) ? std::string("unlimited") : (boost::format("%1.3f seconds") % solverTimeout).str()) +
                   "."));

    return sg;
}

} // namespace
} // namespace
} // namespace

#endif
