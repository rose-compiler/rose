#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Amd64.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Amd64::Amd64()
    : Base("amd64", 8, ByteOrder::ORDER_LSB) {}

Amd64::~Amd64() {}

Amd64::Ptr
Amd64::instance() {
    return Ptr(new Amd64);
}

RegisterDictionary::Ptr
Amd64::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceAmd64();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
