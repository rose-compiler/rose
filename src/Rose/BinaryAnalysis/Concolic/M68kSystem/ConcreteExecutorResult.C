#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/M68kSystem/ConcreteExecutorResult.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {
namespace M68kSystem {

ConcreteExecutorResult::ConcreteExecutorResult() {}

ConcreteExecutorResult::ConcreteExecutorResult(double rank)
    : Super(rank) {}

ConcreteExecutorResult::~ConcreteExecutorResult() {}

ConcreteExecutorResult::Ptr
ConcreteExecutorResult::instance(double rank) {
    return Ptr(new ConcreteExecutorResult(rank));
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
