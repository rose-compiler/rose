#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8086.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Intel8086::Intel8086()
    : Base("intel-8086", 2, ByteOrder::ORDER_LSB) {}

Intel8086::~Intel8086() {}

Intel8086::Ptr
Intel8086::instance() {
    return Ptr(new Intel8086);
}

RegisterDictionary::Ptr
Intel8086::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceI8086();
    return registerDictionary_.get();
}

bool
Intel8086::matchesHeader(SgAsmGenericHeader*) const {
    return false;
}

Disassembler::Base::Ptr
Intel8086::newInstructionDecoder() const {
    // FIXME[Robb Matzke 2023-11-21]: No Disassembler::X86 API to create an Intel 8086 decoder.
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
