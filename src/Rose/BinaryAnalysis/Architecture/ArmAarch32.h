#ifndef ROSE_BinaryAnalysis_Architecture_ArmAarch32_H
#define ROSE_BinaryAnalysis_Architecture_ArmAarch32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for ARM AArch32.
 *
 *  The original (and subsequent) ARM implementation was hardwired without microcode, like the much simpler 8-bit 6502 processor
 *  used in prior Acorn microcomputers.
 *
 *  The 32-bit ARM architecture includes the following RISC features:
 *
 *  @li Load–store architecture.
 *
 *  @li No support for unaligned memory accesses in the original version of the architecture. ARMv6 and later, except some
 *  microcontroller versions, support unaligned accesses for half-word and single-word load/store instructions with some
 *  limitations, such as no guaranteed atomicity.
 *
 *  @li Uniform 16 × 32-bit register file (including the program counter, stack pointer and the link register).
 *
 *  @li Fixed instruction width of 32 bits to ease decoding and pipelining, at the cost of decreased code density. Later, the
 *  Thumb instruction set added 16-bit instructions and increased code density.
 *
 *  @li Mostly single clock-cycle execution.
 *
 *  To compensate for the simpler design, compared with processors like the Intel 80286 and Motorola 68020, some additional design
 *  features were used:
 *
 *  @li Conditional execution of most instructions reduces branch overhead and compensates for the lack of a branch predictor in
 *  early chips.
 *
 *  @li Arithmetic instructions alter condition codes only when desired.
 *
 *  @li 32-bit barrel shifter can be used without performance penalty with most arithmetic instructions and address calculations.
 *
 *  @li Has powerful indexed addressing modes.
 *
 *  @li A link register supports fast leaf function calls.
 *
 *  @li A simple, but fast, 2-priority-level interrupt subsystem has switched register banks. */
class ArmAarch32: public Base {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    using Ptr = ArmAarch32Ptr;

    /** Instruction set.
     *
     *  AArch32 has two instruction sets: T32 and A32. */
    enum class InstructionSet {
        T32,
        A32
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    InstructionSet instructionSet_ = InstructionSet::A32;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    explicit ArmAarch32(InstructionSet);                // use `instance` instead
public:
    ~ArmAarch32();

public:
    /** Allocating constructor. */
    static Ptr instance(InstructionSet);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Instruction set.
     *
     *  ARM AArch32 has two instruction sets: T32 and A32. */
    InstructionSet instructionSet() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
