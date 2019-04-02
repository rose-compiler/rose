#include <sage3basic.h>
#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

// class method
Specimen::Ptr
Specimen::instance(const boost::filesystem::path &executableName) {
    Ptr self = Ptr(new Specimen);
    if (!executableName.empty())
        self->open(executableName);
    return self;
}

bool
Specimen::isEmpty() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return specimen_.empty();
}

std::string
Specimen::name() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return name_;
}

void
Specimen::name(const std::string &s) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    name_ = s;
}

} // namespace
} // namespace
} // namespace
