#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionManager.h>

#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutor.h>
#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ExecutionManager::ExecutionManager(const DatabasePtr &db)
    : database_(db) {
    ASSERT_not_null(db);
}

ExecutionManager::~ExecutionManager() {}

Database::Ptr
ExecutionManager::database() const {
    return database_;
}

std::vector<TestCaseId>
ExecutionManager::pendingConcreteResults(size_t n) {
  return database_->needConcreteTesting(n);
}

TestCaseId
ExecutionManager::pendingConcreteResult() {
  std::vector<TestCaseId> res = database_->needConcreteTesting(1);
  
  if (0 == res.size()) return TestCaseId();
  
  return res.front();
}

void
ExecutionManager::insertConcreteResults( const TestCase::Ptr &testCase, 
                                         const ConcreteExecutorResult& details
                                       ) 
{
  testCase->concreteRank(details.rank());
  database_->saveConcreteResult(testCase, &details);
}

std::vector<TestCaseId>
ExecutionManager::pendingConcolicResults(size_t n) {
  return database_->needConcolicTesting(n);
}

TestCaseId
ExecutionManager::pendingConcolicResult() {
  std::vector<TestCaseId> res = database_->needConcolicTesting(1);
  
  if (0 == res.size()) return TestCaseId();
  
  return res.front();
}

void
ExecutionManager::insertConcolicResults(const TestCase::Ptr& original, const std::vector<TestCase::Ptr> &newCases) {
  original->concolicResult(1);
  database_->id(original, Update::YES);
  for (const TestCase::Ptr &tc: newCases)
      database_->save(tc);
}

bool
ExecutionManager::isFinished() const {
    return database_->hasUntested();
}

} // namespace
} // namespace
} // namespace

#endif
