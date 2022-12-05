#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/ConcreteExecutorResult.h>

#include <rose_isnan.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ConcreteExecutorResult::ConcreteExecutorResult()
    : rank_(0.0), isInteresting_(true) {}

ConcreteExecutorResult::ConcreteExecutorResult(double rank)
    : rank_(rank), isInteresting_(true) {
    ASSERT_forbid(rose_isnan(rank));
}

ConcreteExecutorResult::~ConcreteExecutorResult() {}

double
ConcreteExecutorResult::rank() const {
    return rank_;
}

void
ConcreteExecutorResult::rank(double r) {
    rank_ = r;
}

bool
ConcreteExecutorResult::isInteresting() const {
    return isInteresting_;
}

void
ConcreteExecutorResult::isInteresting(bool b) {
    isInteresting_ = b;
}

} // namespace
} // namespace
} // namespace

#endif
