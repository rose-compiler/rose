#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumiii.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelPentiumiii::IntelPentiumiii()
    : Base("intel-pentiumiii", 4, ByteOrder::ORDER_LSB) {}

IntelPentiumiii::~IntelPentiumiii() {}

IntelPentiumiii::Ptr
IntelPentiumiii::instance() {
    return Ptr(new IntelPentiumiii);
}

RegisterDictionary::Ptr
IntelPentiumiii::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instancePentiumiii();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
