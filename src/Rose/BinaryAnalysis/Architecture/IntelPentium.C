#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelPentium::IntelPentium()
    : Base("intel-pentium", 4, ByteOrder::ORDER_LSB) {}

IntelPentium::~IntelPentium() {}

IntelPentium::Ptr
IntelPentium::instance() {
    return Ptr(new IntelPentium);
}

RegisterDictionary::Ptr
IntelPentium::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instancePentium();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
