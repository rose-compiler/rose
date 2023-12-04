#include <Rose/BinaryAnalysis/InstructionEnumsM68k.h>

class SgAsmM68kInstruction: public SgAsmInstruction {
public:
    /** Property: Instruction kind.
     *
     *  Returns an enum constant describing the Motorola m68k instruction. These enum constants correspond roughly 1:1 with
     *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::M68kInstructionKind kind = Rose::BinaryAnalysis::m68k_unknown_instruction;

public:
    // Overrides are documented in the base class
    virtual bool terminatesBasicBlock() override;
    virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns,
                                    rose_addr_t *target/*out*/, rose_addr_t *ret/*out*/) override;
    virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                    rose_addr_t *target, rose_addr_t *ret) override;
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) override;
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) override;
    virtual Sawyer::Optional<rose_addr_t> branchTarget() override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) override;
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(const std::vector<SgAsmInstruction*>&,
                                                           bool &complete,
                                                           const Rose::BinaryAnalysis::MemoryMap::Ptr &initial_memory =
                                                           Rose::BinaryAnalysis::MemoryMap::Ptr()) override;
    virtual bool isUnknown() const override;
    virtual unsigned get_anyKind() const override;
};
