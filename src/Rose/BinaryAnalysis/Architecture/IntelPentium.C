#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/IntelPentium.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

IntelPentium::IntelPentium()
    : Base("intel-pentium", 4, ByteOrder::ORDER_LSB) {}

IntelPentium::~IntelPentium() {}

IntelPentium::Ptr
IntelPentium::instance() {
    return Ptr(new IntelPentium);
}

RegisterDictionary::Ptr
IntelPentium::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instancePentium();
    return registerDictionary_.get();
}

bool
IntelPentium::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

Disassembler::Base::Ptr
IntelPentium::newInstructionDecoder() const {
    // FIXME[Robb Matzke 2023-11-21]: No Disassembler::X86 API to create an Intel Pentium decoder.
    return {};
}

} // namespace
} // namespace
} // namespace

#endif
