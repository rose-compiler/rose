#ifndef ROSE_BinaryAnalysis_Architecture_ArmAarch64_H
#define ROSE_BinaryAnalysis_Architecture_ArmAarch64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for ARM AArch64.
 *
 *  AArch64 or ARM64 is the 64-bit extension of the ARM architecture family.  It was first introduced with the Armv8-A
 *  architecture. Arm releases a new extension every year. */
class ArmAarch64: public Base {
public:
    using Ptr = ArmAarch64Ptr;

protected:
    ArmAarch64();                                       // use `instance` instead
public:
    ~ArmAarch64();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;

    Sawyer::Container::Interval<size_t> bytesPerInstruction() const override;
    Alignment instructionAlignment() const override;
    std::string instructionMnemonic(const SgAsmInstruction*) const override;
    std::string instructionDescription(const SgAsmInstruction*) const override;
    bool isUnknown(const SgAsmInstruction*) const override;
    bool terminatesBasicBlock(SgAsmInstruction*) const override;
    bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) const override;
    bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) const override;
    Sawyer::Optional<rose_addr_t> branchTarget(SgAsmInstruction*) const override;
    AddressSet getSuccessors(SgAsmInstruction*, bool &complete) const override;

    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;

    virtual InstructionSemantics::BaseSemantics::DispatcherPtr
    newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&) const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
