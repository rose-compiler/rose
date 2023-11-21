#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Intel80286.h>

#include <Rose/BinaryAnalysis/Disassembler/X86.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Intel80286::Intel80286()
    : Base("intel-80286", 2, ByteOrder::ORDER_LSB) {}

Intel80286::~Intel80286() {}

Intel80286::Ptr
Intel80286::instance() {
    return Ptr(new Intel80286);
}

RegisterDictionary::Ptr
Intel80286::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceI286();
    return registerDictionary_.get();
}

bool
Intel80286::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return isSgAsmDOSFileHeader(header) &&
        (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_IA32_Family &&
        header->get_executableFormat()->get_wordSize() == bytesPerWord();
}

Disassembler::Base::Ptr
Intel80286::newInstructionDecoder() const {
    return Disassembler::X86::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
