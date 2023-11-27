#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentiumii.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

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

    if (!registerDictionary_.isCached()) {
        auto regs = RegisterDictionary::instance(name());
        regs->insert(Architecture::findByName("intel-pentium").orThrow()->registerDictionary());
        registerDictionary_ = regs;
    }

    return registerDictionary_.get();
}

bool
IntelPentiumii::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

Disassembler::Base::Ptr
IntelPentiumii::newInstructionDecoder() const {
    // FIXME[Robb Matzke 2023-11-21]: No Disassembler::X86 API to create an Intel Pentium II decoder.
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
