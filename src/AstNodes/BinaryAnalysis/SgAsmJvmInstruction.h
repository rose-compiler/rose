#include <Rose/BinaryAnalysis/InstructionEnumsJvm.h>

/** Represents one JVS machine instruction. */
class SgAsmJvmInstruction: public SgAsmInstruction {
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the JVM instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::JvmInstructionKind kind = Rose::BinaryAnalysis::JvmInstructionKind::unknown;

public:
    // Overrides are documented in the base class
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) override;
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) override;
    virtual bool isUnknown() const override;
    virtual Sawyer::Optional<rose_addr_t> branchTarget() override;
    virtual unsigned get_anyKind() const override;
};
