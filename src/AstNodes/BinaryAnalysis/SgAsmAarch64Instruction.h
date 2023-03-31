#ifdef ROSE_ENABLE_ASM_AARCH64
#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>

/** Represents one ARM A64 machine instruction. */
class SgAsmAarch64Instruction: public SgAsmInstruction {
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the AArch64 A64 instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::Aarch64InstructionKind kind = Rose::BinaryAnalysis::Aarch64InstructionKind::ARM64_INS_INVALID;

    /** Property: ARM A64 instruction condition. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::Aarch64InstructionCondition condition =
        Rose::BinaryAnalysis::Aarch64InstructionCondition::ARM64_CC_INVALID;

    // FIXME[Robb Matzke 2023-03-18]: is the no_serialize a bug?
    /** Property: Whether this instruction updates N, Z, C, and/or V status flags. */
    [[using Rosebud: rosetta, no_serialize]]
    bool updatesFlags = false;

public:
    // Overrides are documented in the base class
    virtual std::string description() const override;
    virtual bool terminatesBasicBlock() override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) override;
    virtual bool isUnknown() const override;
    virtual unsigned get_anyKind() const override;
    virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target,
                                    rose_addr_t *return_va) override;
    virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target,
                                    rose_addr_t *return_va) override;
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) override;
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) override;
    virtual Sawyer::Optional<rose_addr_t> branchTarget() override;
};

#endif // ROSE_ENABLE_ASM_AARCH64
