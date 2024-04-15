#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/PathQueue.h>

#include <Rose/BinaryAnalysis/ModelChecker/Path.h>
#include <Rose/BinaryAnalysis/ModelChecker/PathPrioritizer.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

PathQueue::PathQueue(const PathPrioritizer::Ptr &prioritizer)
    : prioritizer_(prioritizer) {
}

PathQueue::~PathQueue() {}

PathPrioritizer::Ptr
PathQueue::prioritizer() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_not_null(prioritizer_);
    return prioritizer_;
}

void
PathQueue::prioritizer(const PathPrioritizer::Ptr &p) {
    ASSERT_not_null(prioritizer_);
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    prioritizer_ = p;
    std::make_heap(paths_.begin(), paths_.end(), [this](const Path::Ptr &a, const Path::Ptr &b) {
            return (*prioritizer_)(a, b);
        });
}

size_t
PathQueue::size() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return paths_.size();
}

bool
PathQueue::isEmpty() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return paths_.empty();
}

void
PathQueue::reset() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    paths_.clear();
}

void
PathQueue::insert(const Path::Ptr &path) {
    ASSERT_not_null(path);
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_require2(std::find(paths_.begin(), paths_.end(), path) == paths_.end(), "attempted to insert duplicate");
    paths_.push_back(path);
    std::push_heap(paths_.begin(), paths_.end(), [this](const Path::Ptr &a, const Path::Ptr &b) {
            return (*prioritizer_)(a, b);
        });
}

Path::Ptr
PathQueue::takeNext() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    Path::Ptr retval;
    if (!paths_.empty()) {
        std::pop_heap(paths_.begin(), paths_.end(), [this](const Path::Ptr &a, const Path::Ptr &b) {
                return (*prioritizer_)(a, b);
            });
        retval = paths_.back();
        paths_.pop_back();
    }
    return retval;
}

void
PathQueue::traverse(Visitor &visitor) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    for (const Path::Ptr &path: paths_) {
        if (!visitor(path))
            return;
    }
}

} // namespace
} // namespace
} // namespace

#endif
