#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI486.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelI486::IntelI486()
    : Base("intel-i486", 4, ByteOrder::ORDER_LSB) {}

IntelI486::~IntelI486() {}

IntelI486::Ptr
IntelI486::instance() {
    return Ptr(new IntelI486);
}

RegisterDictionary::Ptr
IntelI486::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceI486();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
