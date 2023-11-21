#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch64.h>

#include <Rose/BinaryAnalysis/Disassembler/Aarch64.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

ArmAarch64::ArmAarch64()
    : Base("arm-a64", 8, ByteOrder::ORDER_LSB) {}

ArmAarch64::~ArmAarch64() {}

ArmAarch64::Ptr
ArmAarch64::instance() {
    return Ptr(new ArmAarch64);
}

RegisterDictionary::Ptr
ArmAarch64::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceAarch64();
    return registerDictionary_.get();
}

bool
ArmAarch64::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_ARM_Family &&
        header->get_executableFormat()->get_wordSize() == 8;
}

Disassembler::Base::Ptr
ArmAarch64::newInstructionDecoder() const {
    return Disassembler::Aarch64::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
