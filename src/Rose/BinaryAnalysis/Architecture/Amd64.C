#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Amd64.h>

#include <Rose/BinaryAnalysis/Disassembler/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Amd64::Amd64()
    : Base("amd64", 8, ByteOrder::ORDER_LSB) {}

Amd64::~Amd64() {}

Amd64::Ptr
Amd64::instance() {
    return Ptr(new Amd64);
}

RegisterDictionary::Ptr
Amd64::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceAmd64();
    return registerDictionary_.get();
}

bool
Amd64::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_X8664_Family;
}

Disassembler::Base::Ptr
Amd64::newInstructionDecoder() const {
    return Disassembler::X86::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
