#include <Rose/BinaryAnalysis/InstructionEnumsMips.h>

/** Represents one MIPS machine instruction. */
class SgAsmMipsInstruction: public SgAsmInstruction {
public:
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the MIPS instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::MipsInstructionKind kind = Rose::BinaryAnalysis::mips_unknown_instruction;

public:
    // Overrides are documented in the base class
    virtual unsigned get_anyKind() const override;
};
