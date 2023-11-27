#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI486.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelI486::IntelI486()
    : Base("intel-i486", 4, ByteOrder::ORDER_LSB) {}

IntelI486::~IntelI486() {}

IntelI486::Ptr
IntelI486::instance() {
    return Ptr(new IntelI486);
}

RegisterDictionary::Ptr
IntelI486::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-i386").orThrow()->registerDictionary());
        regs->insert("ac", x86_regclass_flags, x86_flags_status, 18, 1); /* alignment check system flag */
        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
IntelI486::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

Disassembler::Base::Ptr
IntelI486::newInstructionDecoder() const {
    // FIXME[Robb Matzke 2023-11-21]: No Disassembler::X86 API to create an Intel i486 decoder.
    return {};
}

Unparser::Base::Ptr
IntelI486::newUnparser() const {
    return Unparser::X86::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
