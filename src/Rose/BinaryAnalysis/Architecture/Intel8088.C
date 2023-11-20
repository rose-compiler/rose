#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8088.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Intel8088::Intel8088()
    : Base("intel-8088", 2, ByteOrder::ORDER_LSB) {}

Intel8088::~Intel8088() {}

Intel8088::Ptr
Intel8088::instance() {
    return Ptr(new Intel8088);
}

RegisterDictionary::Ptr
Intel8088::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceI8088();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
