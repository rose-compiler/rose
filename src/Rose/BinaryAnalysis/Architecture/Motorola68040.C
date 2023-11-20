#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola68040.h>

#include <Rose/BinaryAnalysis/CallingConvention.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Motorola68040::Motorola68040()
    : Base("motorola-68040", 4, ByteOrder::ORDER_MSB) {}

Motorola68040::~Motorola68040() {}

Motorola68040::Ptr
Motorola68040::instance() {
    return Ptr(new Motorola68040);
}

RegisterDictionary::Ptr
Motorola68040::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceM68000AlternateNames();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
