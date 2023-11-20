#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch32.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

ArmAarch32::ArmAarch32()
    : Base("arm-aarch32", 4, ByteOrder::ORDER_LSB) {}

ArmAarch32::~ArmAarch32() {}

ArmAarch32::Ptr
ArmAarch32::instance() {
    return Ptr(new ArmAarch32);
}

RegisterDictionary::Ptr
ArmAarch32::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceAarch32();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
