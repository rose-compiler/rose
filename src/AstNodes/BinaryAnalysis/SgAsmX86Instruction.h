#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionEnumsX86.h>

/** Represents one Intel x86 machine instruction. */
class SgAsmX86Instruction: public SgAsmInstruction {
public:
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the x86 instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::X86InstructionKind kind= Rose::BinaryAnalysis::x86_unknown_instruction;

    /** Property: An enum constant describing the base size of an x86 instruction. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::X86InstructionSize baseSize = Rose::BinaryAnalysis::x86_insnsize_none;

    /** Property: An enum describing the x86 instruction operand size. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::X86InstructionSize operandSize = Rose::BinaryAnalysis::x86_insnsize_none;

    /** Property: An enum describing the x86 address size. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::X86InstructionSize addressSize = Rose::BinaryAnalysis::x86_insnsize_none;

    /** Property: Whether the x86 lock prefix was present.
     *
     *  Returns true if this instruction had the x86 lock prefix byte; false otherwise. */
    [[using Rosebud: rosetta]]
    bool lockPrefix = false;

    /** Property: An enum constant describing whether the instruction is repeated. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::X86RepeatPrefix repeatPrefix = Rose::BinaryAnalysis::x86_repeat_none;

    /** Property: An enum constant describing branch prediction. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::X86BranchPrediction branchPrediction = Rose::BinaryAnalysis::x86_branch_prediction_none;

    /** Property: The segment override register.
     *
     *  Returns an enum describing the segment register override. Note that this is not a register descriptor, but an enum
     *  constant. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::X86SegmentRegister segmentOverride = Rose::BinaryAnalysis::x86_segreg_none;

public:
    /** Converts a size to an instruction size enum.
     *
     *  Given a size in bits, which must be 16, 32, or 64, return the corresponding enum constant. */
    static Rose::BinaryAnalysis::X86InstructionSize instructionSizeForWidth(size_t);

    /** Converts a size enum constant to a size.
     *
     *  Given a size enum constant, return the number of bits that enum represents. */
    static size_t widthForInstructionSize(Rose::BinaryAnalysis::X86InstructionSize);

    /** Return the register dictionary for an x86 architecture.
     *
     *  Given an instruction size enum constant return the register dictionary that describes the x86 architecture with
     *  the specified word size.  See also, @ref registersForWidth. */
    static Rose::BinaryAnalysis::RegisterDictionaryPtr registersForInstructionSize(Rose::BinaryAnalysis::X86InstructionSize);

    /** Return the register dictionary for an x86 architecture.
     *
     *  Given an instruction size of 16, 32, or 64 return the register dictionary that describes the x86 architecture with
     *  the specified word size.  See also, @ref registersForInstructionSize. */
    static Rose::BinaryAnalysis::RegisterDictionaryPtr registersForWidth(size_t);

    // Overrides are documented in the base class
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) override;
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) override;
    virtual Sawyer::Optional<rose_addr_t> branchTarget() override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(const std::vector<SgAsmInstruction*>&,
                                                           bool &complete,
                                                           const Rose::BinaryAnalysis::MemoryMap::Ptr &initial_memory =
                                                           Rose::BinaryAnalysis::MemoryMap::Ptr()) override;
    virtual bool isUnknown() const override;
    virtual unsigned get_anyKind() const override;
};
