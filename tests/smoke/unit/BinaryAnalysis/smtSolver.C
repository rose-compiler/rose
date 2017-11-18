static const char *purpose = "unit tests for SMT solvers";
static const char *description =
    "Runs various unit tests of the ROSE interface to SMT solvers.";

#include <rose.h>
#include <BinarySmtlibSolver.h>
#include <BinaryYicesSolver.h>
#include <BinaryZ3Solver.h>
#include <Diagnostics.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

enum SolverType { SOLVER_ALL, SOLVER_BEST,
                  SOLVER_GENERIC_SMTLIB2,
                  SOLVER_Z3_TEXT, SOLVER_Z3_API,
                  SOLVER_YICES_TEXT, SOLVER_YICES_API };

SolverType
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser p = CommandlineProcessing::createEmptyParser(purpose, description);
    p.errorStream(mlog[FATAL]);
    p.doc("Synopsis", "@prop{programName} [@v{switches}]");

    SwitchGroup switches = CommandlineProcessing::genericSwitches();
    switches.name("");

    SolverType solverType = SOLVER_ALL;
    switches.insert(Switch("solver", 'S')
                    .argument("type", enumParser(solverType)
                              ->with("smt-lib2", SOLVER_GENERIC_SMTLIB2)
                              ->with("z3-exe", SOLVER_Z3_TEXT)
                              ->with("z3-lib", SOLVER_Z3_API)
                              ->with("yices-exe", SOLVER_YICES_TEXT)
                              ->with("yices-lib", SOLVER_YICES_API)
                              ->with("all", SOLVER_ALL)
                              ->with("best", SOLVER_BEST))
                    .doc("Type of solver interaction. The choices are:"
                         "@named{smt-lib2}{Generic SMT-LIB2 text.}"
                         "@named{z3-exe}{The Z3 executable using its text interface.}"
                         "@named{z3-api}{The Z3 library using its API.}"
                         "@named{yices-exe}{The Yices executable using its text interface.}"
                         "@named{yices-api}{The Yices library using its API.}"
                         "@named{all}{Test all available solvers, one at a time.}"
                         "@named{best}{Best available solver.}"));
    if (!p.with(switches).parse(argc, argv).apply().unreachedArgs().empty()) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    return solverType;
}

SmtSolver*
createSolver(SolverType solverType) {
    switch (solverType) {
        case SOLVER_BEST:
            return SmtSolver::bestAvailable();
            break;
        case SOLVER_GENERIC_SMTLIB2:
            return new SmtlibSolver("SMT-LIB", "/bin/cat");
            break;
        case SOLVER_Z3_TEXT:
            return new Z3Solver(SmtSolver::LM_EXECUTABLE);
            break;
        case SOLVER_Z3_API:
            return new Z3Solver(SmtSolver::LM_LIBRARY);
            break;
        case SOLVER_YICES_TEXT:
            return new YicesSolver(SmtSolver::LM_EXECUTABLE);
            break;
        case SOLVER_YICES_API:
            return new YicesSolver(SmtSolver::LM_LIBRARY);
            break;
        default:
            ASSERT_not_reachable("invalid solver type");
    }
}

void
testSolver(SolverType solverType) {
    SmtSolver *solver = createSolver(solverType);
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
    SolverType solverType = parseCommandLine(argc, argv);

    if (SOLVER_ALL == solverType) {
        testSolver(SOLVER_YICES_TEXT);
        testSolver(SOLVER_YICES_API);
        testSolver(SOLVER_Z3_TEXT);
        testSolver(SOLVER_Z3_API);
    } else {
        testSolver(solverType);
    }
}
