#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc32.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Powerpc32::Powerpc32(ByteOrder::Endianness byteOrder)
    : Base(ByteOrder::ORDER_MSB == byteOrder ? "ppc32-be" : (ByteOrder::ORDER_LSB == byteOrder ? "ppc32-el" : "ppc32"),
           4, byteOrder) {}

Powerpc32::~Powerpc32() {}

Powerpc32::Ptr
Powerpc32::instance(ByteOrder::Endianness byteOrder) {
    return Ptr(new Powerpc32(byteOrder));
}

RegisterDictionary::Ptr
Powerpc32::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instancePowerpc32();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
