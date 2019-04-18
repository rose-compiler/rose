#include <sage3basic.h>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

Database::Ptr
ExecutionManager::database() const {
    return database_;
}

std::vector<Database::TestCaseId>
ExecutionManager::pendingConcreteResults(size_t n) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Database::TestCaseId
ExecutionManager::pendingConcreteResult() {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
ExecutionManager::insertConcreteResults(const TestCase::Ptr &testCase, const ConcreteExecutor::Result &details) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

std::vector<Database::TestCaseId>
ExecutionManager::pendingConcolicResults(size_t n) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

Database::TestCaseId
ExecutionManager::pendingConcolicResult() {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

void
ExecutionManager::insertConcolicResults(const TestCase::Ptr &original, const std::vector<TestCase::Ptr> &newCases) {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

bool
ExecutionManager::isFinished() const {
    ASSERT_not_implemented("[Robb Matzke 2019-04-15]");
}

} // namespace
} // namespace
} // namespace
