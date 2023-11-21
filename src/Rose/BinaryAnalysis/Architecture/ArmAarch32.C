#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/ArmAarch32.h>

#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

ArmAarch32::ArmAarch32(InstructionSet instructionSet)
    : Base(InstructionSet::T32 == instructionSet ? "arm-t32" : "arm-a32", 4, ByteOrder::ORDER_LSB),
      instructionSet_(instructionSet) {}

ArmAarch32::~ArmAarch32() {}

ArmAarch32::Ptr
ArmAarch32::instance(InstructionSet instructionSet) {
    return Ptr(new ArmAarch32(instructionSet));
}

ArmAarch32::InstructionSet
ArmAarch32::instructionSet() const {
    return instructionSet_;
}

RegisterDictionary::Ptr
ArmAarch32::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceAarch32();
    return registerDictionary_.get();
}

bool
ArmAarch32::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_ARM_Family) {
        if (header->get_executableFormat()->get_wordSize() == 4 && InstructionSet::A32 == instructionSet())
            return true;
        if (header->get_executableFormat()->get_wordSize() == 2 && InstructionSet::T32 == instructionSet())
            return true;
    }
    return false;
}

Disassembler::Base::Ptr
ArmAarch32::newInstructionDecoder() const {
    switch (instructionSet()) {
        case InstructionSet::A32:
            return Disassembler::Aarch32::instanceA32(shared_from_this());
        case InstructionSet::T32:
            return Disassembler::Aarch32::instanceT32(shared_from_this());
    }
    ASSERT_not_reachable("invalid instruction set");
}

} // namespace
} // namespace
} // namespace

#endif
