#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Mips32.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Mips32::Mips32(ByteOrder::Endianness byteOrder)
    : Base(ByteOrder::ORDER_MSB == byteOrder ? "mips32-be" : (ByteOrder::ORDER_LSB == byteOrder ? "mips32-el" : "mips32"),
           4, byteOrder) {}

Mips32::~Mips32() {}

Mips32::Ptr
Mips32::instance(ByteOrder::Endianness byteOrder) {
    return Ptr(new Mips32(byteOrder));
}

RegisterDictionary::Ptr
Mips32::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceMips32();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
