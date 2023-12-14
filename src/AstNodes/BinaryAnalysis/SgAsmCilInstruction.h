#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>

class SgAsmCilInstruction: public SgAsmInstruction {
public:
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the CIL instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::CilInstructionKind kind = Rose::BinaryAnalysis::Cil_unknown_instruction;

public:
    // Overrides are documented in the base classa
    virtual unsigned get_anyKind() const override;
};
