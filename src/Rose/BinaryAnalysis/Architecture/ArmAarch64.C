#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch64.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

ArmAarch64::ArmAarch64()
    : Base("arm-aarch64", 8, ByteOrder::ORDER_LSB) {}

ArmAarch64::~ArmAarch64() {}

ArmAarch64::Ptr
ArmAarch64::instance() {
    return Ptr(new ArmAarch64);
}

RegisterDictionary::Ptr
ArmAarch64::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceAarch64();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
