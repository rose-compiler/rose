#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumii.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelPentiumii::IntelPentiumii()
    : Base("intel-pentiumii", 4, ByteOrder::ORDER_LSB) {}

IntelPentiumii::~IntelPentiumii() {}

IntelPentiumii::Ptr
IntelPentiumii::instance() {
    return Ptr(new IntelPentiumii);
}

RegisterDictionary::Ptr
IntelPentiumii::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instancePentium(); // no PentiumII defined yet, so use Pentium for now
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
