#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>

#include <Rose/CommandLine.h>
#include <Rose/StringUtility/Diagnostics.h>
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
Settings::commandLineDebugSwitches() {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Model checker debugging");
    sg.switchOrder(INSERTION_ORDER);
    sg.doc("These switches control what information is displayed to the model checker debug diagnostic stream. Most "
           "only have an effect if the " + mlog.name() + "'s debug facility is enabled (see @s{log}=list).");

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

    return sg;
}

Sawyer::CommandLine::SwitchGroup
Settings::commandLineModelSwitches() {
    using namespace Sawyer::CommandLine;
    SwitchGroup sg("Model checker settings");

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

    Rose::CommandLine::insertBooleanSwitch(sg, "explore-duplicate-states", exploreDuplicateStates,
                                           "If a machine state recurs, then all paths leading out of the second occurrence must "
                                           "necessarily be the same as the paths leading out of the first occurrenace even if "
                                           "the prefixes for those paths (up to the state in question) differ. However, due to "
                                           "various exploration limits, the sets of explored paths might differ. For instance, "
                                           "if the state occurred late in a path, then the @s{k} limit results less subsequent "
                                           "exploration than what would be explored if the state occured early in a path.\n\n"

                                           "A trivial example of duplicate machine states is the C source code to implement "
                                           "a busy wait, such as \"while (1);\".\n\n"

                                           "Turning off exploration of duplicate states may result in faster exploration, but "
                                           "the trade off is that a hash must be computed and stored.");

    sg.insert(Switch("max-path-length", 'k')
              .argument("nsteps", positiveIntegerParser(kSteps))
              .doc("Maximum path length in steps before abandoning any further exploration. A step generally corresponds to "
                   "a single instruction for those execution units that have instructions. The default is " +
                   (UNLIMITED == kSteps ? "unlimited" : StringUtility::plural(kSteps, "steps")) + "."));

    sg.insert(Switch("max-path-nodes")
              .argument("nnodes", positiveIntegerParser(kNodes))
              .doc("Maximum path length in nodes before abandoning any further exploration. A node is usually a basic block "
                   "but can occassionally be an instruction or a function call summary. The default is " +
                   (UNLIMITED == kNodes ? "unlimited" : StringUtility::plural(kNodes, "nodes")) + "."));

    sg.insert(Switch("max-path-time", 't')
              .argument("duration", Rose::CommandLine::durationParser(maxTime))
              .doc("Maximum time to spend exploring any given path. If a path takes longer than the specified time, "
                   "then no paths with that prefix are explored. " + Rose::CommandLine::DurationParser::docString() +
                   " The default is " + Rose::CommandLine::durationParser()->toString(maxTime) + "."));

    sg.insert(Switch("replace-symbolic")
              .argument("nnodes", nonNegativeIntegerParser(maxSymbolicSize))
              .doc("Maximum size of symbolic expressions before they're replaced by a new variable. A value of zero means do "
                   "not ever replace symbolic expressions with variables. Setting this to non-zero may make the analysis faster "
                   "at the cost of less precision. The default is " + boost::lexical_cast<std::string>(maxSymbolicSize) + "."));

    sg.insert(Switch("solver-timeout")
              .argument("duration", Rose::CommandLine::durationParser(solverTimeout))
              .doc("Specifies the maximum amount of elapsed time for each call to the SMT solver. If this time limit expires, the "
                   "solver returns an answer of \"unknown\". " + Rose::CommandLine::DurationParser::docString() +
                   " The default is " + Rose::CommandLine::durationParser()->toString(solverTimeout) + "."));

    return sg;
}

} // namespace
} // namespace
} // namespace

#endif
