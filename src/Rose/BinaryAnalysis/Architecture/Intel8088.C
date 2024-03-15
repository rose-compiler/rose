#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Intel8088.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Intel8088::Intel8088()
    : X86("intel-8088", 2) {}

Intel8088::~Intel8088() {}

Intel8088::Ptr
Intel8088::instance() {
    return Ptr(new Intel8088);
}

RegisterDictionary::Ptr
Intel8088::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-8086").orThrow()->registerDictionary());
        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
Intel8088::matchesHeader(SgAsmGenericHeader*) const {
    return false;
}

} // namespace
} // namespace
} // namespace

#endif
