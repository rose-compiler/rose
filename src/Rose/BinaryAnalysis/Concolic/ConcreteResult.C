#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/ConcreteResult.h>

#include <rose_isnan.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ConcreteResult::ConcreteResult()
    : rank_(0.0), isInteresting_(true) {}

ConcreteResult::ConcreteResult(double rank)
    : rank_(rank), isInteresting_(true) {
    ASSERT_forbid(rose_isnan(rank));
}

ConcreteResult::~ConcreteResult() {}

double
ConcreteResult::rank() const {
    return rank_;
}

void
ConcreteResult::rank(double r) {
    rank_ = r;
}

bool
ConcreteResult::isInteresting() const {
    return isInteresting_;
}

void
ConcreteResult::isInteresting(bool b) {
    isInteresting_ = b;
}

} // namespace
} // namespace
} // namespace

#endif
