#include <Rose/BinaryAnalysis/InstructionEnumsPowerpc.h>

/** Represents one PowerPC machine instruction. */
class SgAsmPowerpcInstruction: public SgAsmInstruction {
public:
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the PowerPC instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::PowerpcInstructionKind kind = Rose::BinaryAnalysis::powerpc_unknown_instruction;

public:
    /** Description of conditional branch BO constant.
     *
     *  Given a BO constant from a PowerPC conditional branch instruction, convert it to a description of the condition
     *  that causes the branch to be taken. "BO" is the term used in the PowerPC documentation to denote the first argument
     *  of the conditional instruction; it is a 5-bit integer constant. */
    std::string conditionalBranchDescription() const;

    /** Return the register dictionary for a PowerPC architecture.
     *
     *  Given an instruction size of 32 or 64 return the register dictionary that describes the PowerPC architecture with
     *  the specified word size. */
    static Rose::BinaryAnalysis::RegisterDictionaryPtr registersForWidth(size_t);

    // Overrides are documented in the base class
    virtual std::string description() const override;
    virtual bool terminatesBasicBlock() override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) override;
    virtual bool isUnknown() const override;
    virtual unsigned get_anyKind() const override;
    virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&,
                                    rose_addr_t *target, rose_addr_t *retva) override;
    virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                    rose_addr_t *target, rose_addr_t *retva) override;
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) override;
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) override;
};
