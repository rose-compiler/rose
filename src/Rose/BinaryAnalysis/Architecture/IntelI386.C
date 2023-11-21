#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelI386.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelI386::IntelI386()
    : Base("intel-i386", 4, ByteOrder::ORDER_LSB) {}

IntelI386::~IntelI386() {}

IntelI386::Ptr
IntelI386::instance() {
    return Ptr(new IntelI386);
}

RegisterDictionary::Ptr
IntelI386::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceI386Math();
    return registerDictionary_.get();
}

bool
IntelI386::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

Disassembler::Base::Ptr
IntelI386::newInstructionDecoder() const {
    // FIXME[Robb Matzke 2023-11-21]: No Disassembler::X86 API to create an Intel Pentium decoder.
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
