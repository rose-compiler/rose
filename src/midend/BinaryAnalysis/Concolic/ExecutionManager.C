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
  return database_->needConcreteTesting(n);
}

Database::TestCaseId
ExecutionManager::pendingConcreteResult() {
  std::vector<Database::TestCaseId> res = database_->needConcreteTesting(1);
  
  if (0 == res.size()) return Database::TestCaseId();
  
  return res.front();
}

void
ExecutionManager::insertConcreteResults( const TestCase::Ptr &testCase, 
                                         const ConcreteExecutor::Result& details
                                       ) 
{
  testCase->concreteRank(details.rank());
  database_->insertConcreteResults(testCase, details);
}

std::vector<Database::TestCaseId>
ExecutionManager::pendingConcolicResults(size_t n) {
  return database_->needConcolicTesting(n);
}

Database::TestCaseId
ExecutionManager::pendingConcolicResult() {
  std::vector<Database::TestCaseId> res = database_->needConcolicTesting(1);
  
  if (0 == res.size()) return Database::TestCaseId();
  
  return res.front();
}

void
ExecutionManager::insertConcolicResults(const TestCase::Ptr& original, const std::vector<TestCase::Ptr> &newCases) {
  original->concolicResult(1);
  database_->id(original, Update::YES);
  BOOST_FOREACH (const TestCase::Ptr &tc, newCases)
      database_->save(tc);
}

bool
ExecutionManager::isFinished() const {
    return database_->hasUntested();
}

} // namespace
} // namespace
} // namespace
