#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingManager.h>

#include <Rose/BinaryAnalysis/Concolic/ConcolicExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/TracingResult.h>
#include <Rose/BinaryAnalysis/Concolic/Specimen.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

TracingManager::TracingManager(const Database::Ptr &db)
    : Super(db) {}

TracingManager::~TracingManager() {}

TracingManager::Ptr
TracingManager::create(const std::string&/*databaseUrl*/, const boost::filesystem::path &executableName) {
    // Create the initial test case
    std::string name = executableName.filename().string();
    auto specimen = Specimen::instance(executableName);
    auto testCase0 = TestCase::instance(specimen);
    testCase0->name(name + "#0");

    // Create the database
    auto db = Database::create("sqlite:" + name + ".db", name);
    db->id(testCase0);                                  // save the first test case, side effect of obtaining an ID

    return Ptr(new TracingManager(db));
}

TracingManager::Ptr
TracingManager::instance(const std::string &databaseUrl, const std::string &testSuiteName) {
    ASSERT_not_implemented("[Robb Matzke 2022-11-21]");
}

void
TracingManager::run() {
    auto concreteExecutor = TracingExecutor::instance(database());
    auto concolicExecutor = ConcolicExecutor::instance();

    while (!isFinished()) {
        // Run as many test cases concretely as possible.
        while (TestCaseId testCaseId = pendingConcreteResult()) {
            TestCase::Ptr testCase = database()->object(testCaseId);
            ConcreteResult::Ptr concreteResult = concreteExecutor->execute(testCase);
            ASSERT_not_null(concreteResult);
            insertConcreteResults(testCase, concreteResult);
        }

        // Now that all the test cases have run concretely, run a few of the "best" onces concolically. The "best" is defined
        // either by the ranks returned from the concrete executor, or by this class overriding pendingConcolicResult (which we
        // haven't done).
        for (TestCaseId testCaseId: pendingConcolicResults(10 /*arbitrary*/)) {
            TestCase::Ptr testCase = database()->object(testCaseId);
            std::vector<TestCase::Ptr> newTestCases = concolicExecutor->execute(database(), testCase);
            insertConcolicResults(testCase, newTestCases);
        }
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
