#include <Rose/BinaryAnalysis/InstructionEnumsM68k.h>

class SgAsmM68kInstruction: public SgAsmInstruction {
public:
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the Motorola m68k instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::M68kInstructionKind kind = Rose::BinaryAnalysis::m68k_unknown_instruction;

    /** Property: Data format. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::M68kDataFormat dataFormat = Rose::BinaryAnalysis::m68k_fmt_unknown;

public:
    // Overrides are documented in the base class
    virtual unsigned get_anyKind() const override;
};
