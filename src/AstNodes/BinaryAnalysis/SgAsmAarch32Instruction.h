#ifdef ROSE_ENABLE_ASM_AARCH32
#include <Rose/BinaryAnalysis/InstructionEnumsAarch32.h>

/** Represents one A32 or T32 machine instruction. */
class SgAsmAarch32Instruction: public SgAsmInstruction {
public:
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the AArch32 A32 or T32 instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::Aarch32InstructionKind kind = Rose::BinaryAnalysis::Aarch32InstructionKind::ARM_INS_INVALID;

    /** Property: ARM AArch32 instruction condition.
     *
     *  This property indicates when the instruction is executed. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::Aarch32InstructionCondition condition =
        Rose::BinaryAnalysis::Aarch32InstructionCondition::ARM_CC_INVALID;

    /** Property: Whether this instruction updates N, Z, C, and/or V status flags. */
    [[using Rosebud: rosetta]]
    bool updatesFlags = false;

    /** Property: Whether this instruction writes back to an operand at the end. */
    [[using Rosebud: rosetta]]
    bool writesBack = false;

    /** Property: Whether this instruction writes to the instruction pointer register.
     *
     *  On ARM architectures, the instruction pointer register is named "PC", for "program counter". ROSE always calls this
     *  register the instruction pointer register. */
    [[using Rosebud: rosetta]]
    bool writesToIp = false;

public:
    // Overrides are documented in the base class
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) override;
    virtual bool isUnknown() const override;
    virtual unsigned get_anyKind() const override;
    virtual Sawyer::Optional<rose_addr_t> branchTarget() override;
};
#endif // ROSE_ENABLE_ASM_AARCH32
