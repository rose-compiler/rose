#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/PathPredicate.h>

#include <Rose/BinaryAnalysis/ModelChecker/Path.h>
#include <Rose/BinaryAnalysis/ModelChecker/PathNode.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <rose_isnan.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PathPredicate::PathPredicate() {}

PathPredicate::~PathPredicate() {}

std::pair<bool, const char*>
PathPredicate::test(const Settings::Ptr &settings, const Path::Ptr &path) {
    return (*this)(settings, path);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AlwaysTrue
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AlwaysTrue::Ptr
AlwaysTrue::instance() {
    return Ptr(new AlwaysTrue);
}

std::pair<bool, const char*>
AlwaysTrue::operator()(const Settings::Ptr&, const Path::Ptr&) {
    return {true, "true"};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WorkPredicate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WorkPredicate::Ptr
WorkPredicate::instance() {
    return Ptr(new WorkPredicate);
}

size_t
WorkPredicate::kLimitReached() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return kLimitReached_;
}

size_t
WorkPredicate::timeLimitReached() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return timeLimitReached_;
}

std::pair<bool, const char*>
WorkPredicate::operator()(const Settings::Ptr &settings, const Path::Ptr &path) {
    ASSERT_not_null(settings);
    ASSERT_not_null(path);
    ASSERT_forbid(path->isEmpty());
    const double timeUsed = path->processingTime();

    if (path->nSteps() >= settings->kSteps) {
        {
            SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
            ++kLimitReached_;
        }
        return {false, "K bound (steps)"};
    } else if (path->nNodes() > settings->kNodes) {
        {
            SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
            ++kLimitReached_;
        }
        return {false, "K bound (nodes)"};
    } else if (settings->maxTime && !rose_isnan(timeUsed) && timeUsed >= settings->maxTime.get()) {
        {
            SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
            ++timeLimitReached_;
        }
        return {false, "time limit"};
    } else {
        return {true, "okay"};
    }
}

void
WorkPredicate::reset() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    kLimitReached_ = 0;
    timeLimitReached_ = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HasFinalTags
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HasFinalTags::Ptr
HasFinalTags::instance() {
    return Ptr(new HasFinalTags);
}

std::pair<bool, const char*>
HasFinalTags::operator()(const Settings::Ptr&, const Path::Ptr &path) {
    ASSERT_not_null(path);
    ASSERT_forbid(path->isEmpty());
    if (path->lastNode()->nTags() > 0) {
        return {true, "has tag"};
    } else {
        return {false, "lacks tag"};
    }
}

} // namespace
} // namespace
} // namespace

#endif
