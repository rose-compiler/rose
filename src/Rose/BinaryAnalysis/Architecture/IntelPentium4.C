#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium4.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelPentium4::IntelPentium4()
    : Base("intel-pentium4", 4, ByteOrder::ORDER_LSB) {}

IntelPentium4::~IntelPentium4() {}

IntelPentium4::Ptr
IntelPentium4::instance() {
    return Ptr(new IntelPentium4);
}

RegisterDictionary::Ptr
IntelPentium4::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instancePentium4();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
