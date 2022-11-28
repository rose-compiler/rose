#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/ConcreteExecutor.h>

#include <Rose/BinaryAnalysis/Concolic/Database.h>
#include <Rose/BinaryAnalysis/Concolic/TestCase.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

ConcreteExecutor::ConcreteExecutor(const Database::Ptr &db)
    : Concolic::ConcreteExecutor(db) {}

ConcreteExecutor::~ConcreteExecutor() {}

ConcreteExecutor::Ptr
ConcreteExecutor::instance(const Database::Ptr &db) {
    return Ptr(new ConcreteExecutor(db));
}

ConcreteExecutorResult*
ConcreteExecutor::execute(const TestCase::Ptr &tc) {}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
