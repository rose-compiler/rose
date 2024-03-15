#ifndef ROSE_BinaryAnalysis_Disassembler_Aarch32_H
#define ROSE_BinaryAnalysis_Disassembler_Aarch32_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <Rose/BinaryAnalysis/Disassembler/Base.h>

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>

#include <capstone/capstone.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

/** Instruction decoder for ARM A32 and T32 instruction sets.
 *
 * Most of the useful disassembly methods can be found in the @ref Disassembler superclass.  Some of the constants
 * have the same ill-defined meanings as they do in the Capstone library. */
class Aarch32: public Base {
public:
    /** Shared ownership pointer. */
    using Ptr = Aarch32Ptr;

    /** Capstone "Mode type", limited to those related to AArch32. Warning: these are non-orthogonal concepts. */
    enum class Mode {
        ARM32 = CS_MODE_ARM,                            /**< Capstone: "32-bit ARM". */ // probably zero, not really a bit flag
        THUMB = CS_MODE_THUMB,                          /**< Capstone: "ARM's Thumb mode, including Thumb-2". */
        MCLASS = CS_MODE_MCLASS,                        /**< Capstone: "ARM's Cortex-M series". */
        V8 = CS_MODE_V8                                 /**< Capstone: "ARMv8 A32 encodngs for ARM". */
    };

    /** Collection of Modes. */
    using Modes = BitFlags<Mode>;

private:
    Modes modes_;                                       // a subset of Capstone's cs_mode constants (warning: nonorthoganal concepts)
    csh capstone_;                                      // the capstone handle
    bool capstoneOpened_ = false;                       // whether capstone_ is initialized

protected:
    // Constructor for specific architecture. */
    explicit Aarch32(const Architecture::BaseConstPtr&, Modes = Modes());

public:
    /** Allocating constructor for an A32 instruction set decoder. */
    static Ptr instanceA32(const Architecture::BaseConstPtr&);

    /** Allocating constructor for a T32 instruction set decoder. */
    static Ptr instanceT32(const Architecture::BaseConstPtr&);

    /** Allocating constructor for A32 instruction set decoder. */
    static Ptr instance(const Architecture::BaseConstPtr&);

    ~Aarch32();

    // overrides
    Base::Ptr clone() const override;
    SgAsmInstruction* disassembleOne(const MemoryMap::Ptr&, rose_addr_t startVa, AddressSet *successors=nullptr) override;
    SgAsmInstruction* makeUnknownInstruction(const Exception&) override;

private:
    // Open the capstone library connection
    void openCapstone();

    // Convert the instruction bytes to a 32- or 16- bit integer.
    uint32_t bytesToWord(size_t nBytes, const uint8_t *bytes);

    // Replace instruction-pointer expressions with constants if possible, leaving a comment in its place.
    void commentIpRelative(SgAsmInstruction*);

    // Fix the mnemonic to be something more human readable.
    std::string fixMnemonic(const std::string&, arm_cc);

    // Make a ROSE instruction operand from a Capstone operand
    SgAsmExpression* makeOperand(const cs_insn&, const cs_arm_op&);

    // Add shift or rotate operations to an expression based on the Capstone operand.
    SgAsmExpression* shiftOrRotate(SgAsmExpression*, const cs_arm_op&);

    // Change a memory reference expression's address by wrapping it in a SgAsmPreIncrementExpression or SgAsmPostIncrement
    // expression if necessary.
    void wrapPrePostIncrement(SgAsmAarch32Instruction*, const cs_insn&, const uint32_t insnWord);

    // Convert a Capstone register number to a ROSE register descriptor. ROSE's register descriptor are a lot more than just an
    // ID number.
    RegisterDescriptor makeRegister(arm_reg);

    // Convert a Capstone system register number to a ROSE expression.
    SgAsmExpression* makeSystemRegister(arm_sysreg);

    // Create a register descriptor for a coprocessor register.
    RegisterDescriptor makeCoprocRegister(int registerNumber);

    // Restrict a register to just part of a register
    RegisterDescriptor subRegister(RegisterDescriptor, int idx);

    // Return a type for register.
    SgAsmType* registerType(RegisterDescriptor);

    // Capstone doesn't return information about how much memory is read for a memory read operand. Therefore, we need to
    // partially decode instructions ourselves to get this information.
    SgAsmType* typeForMemoryRead(const cs_insn&);

    // Returns the opcode as a 32-bit value.
    uint32_t opcode(const cs_insn&);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
