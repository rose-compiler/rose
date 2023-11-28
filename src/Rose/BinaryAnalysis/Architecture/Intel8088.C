#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Intel8088.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Intel8088::Intel8088()
    : Base("intel-8088", 2, ByteOrder::ORDER_LSB) {}

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

Disassembler::Base::Ptr
Intel8088::newInstructionDecoder() const {
    // FIXME[Robb Matzke 2023-11-21]: No Disassembler::X86 API to create an Intel 8088 decoder.
    return {};
}

Unparser::Base::Ptr
Intel8088::newUnparser() const {
    return Unparser::X86::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
Intel8088::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherX86::instance(shared_from_this(), ops);
}

} // namespace
} // namespace
} // namespace

#endif
