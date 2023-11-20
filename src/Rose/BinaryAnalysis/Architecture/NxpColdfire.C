#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/NxpColdfire.h>

#include <Rose/BinaryAnalysis/CallingConvention.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

NxpColdfire::NxpColdfire()
    : Base("nxp-coldfire", 4, ByteOrder::ORDER_MSB) {}

NxpColdfire::~NxpColdfire() {}

NxpColdfire::Ptr
NxpColdfire::instance() {
    return Ptr(new NxpColdfire);
}

RegisterDictionary::Ptr
NxpColdfire::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceColdfireEmac();
    return registerDictionary_.get();
}

} // namespace
} // namespace
} // namespace

#endif
