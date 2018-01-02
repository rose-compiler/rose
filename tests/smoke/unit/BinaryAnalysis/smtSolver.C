static const char *purpose = "unit tests for SMT solvers";
static const char *description =
    "Runs various unit tests of the ROSE interface to SMT solvers.  If no solver is specified, then all solvers are tested.";

#include <rose.h>
#include <BinarySmtlibSolver.h>
#include <BinaryYicesSolver.h>
#include <BinaryZ3Solver.h>
#include <Diagnostics.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

void
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser p = CommandlineProcessing::createEmptyParser(purpose, description);
    p.errorStream(mlog[FATAL]);
    p.doc("Synopsis", "@prop{programName} [@v{switches}]");

    SwitchGroup switches = CommandlineProcessing::genericSwitches();
    switches.name("");

    if (!p.with(switches).parse(argc, argv).apply().unreachedArgs().empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
}

void
testSolver(SmtSolver *solver) {
    mlog[INFO] <<"testing " <<solver->name() <<"\n";
    if (solver->linkage() == SmtSolver::LM_NONE) {
        mlog[WARN] <<"test skipped due to lack of SMT solver\n";
    } else {
        solver->selfTest();
    }
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    parseCommandLine(argc, argv);

    if (CommandlineProcessing::genericSwitchArgs.smtSolver == "" ||
        CommandlineProcessing::genericSwitchArgs.smtSolver == "none") {
        BOOST_FOREACH (const SmtSolver::Availability::value_type &node, SmtSolver::availability())
            testSolver(SmtSolver::instance(node.first));
    } else {
        testSolver(SmtSolver::instance(CommandlineProcessing::genericSwitchArgs.smtSolver));
    }
}
