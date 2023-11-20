#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI386.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelI386::IntelI386()
    : Base("intel-i386", 4, ByteOrder::ORDER_LSB) {}

IntelI386::~IntelI386() {}

IntelI386::Ptr
IntelI386::instance() {
    return Ptr(new IntelI386);
}

RegisterDictionary::Ptr
IntelI386::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceI386Math();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
