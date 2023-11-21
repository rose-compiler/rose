#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Motorola68040.h>

#include <Rose/BinaryAnalysis/Disassembler/M68k.h>

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

bool
Motorola68040::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_M68K_Family;
}

Disassembler::Base::Ptr
Motorola68040::newInstructionDecoder() const {
    return Disassembler::M68k::instance(shared_from_this(), m68k_68040);
}

} // namespace
} // namespace
} // namespace

#endif
