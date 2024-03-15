#ifndef ROSE_BinaryAnalysis_Disassembler_Aarch64_H
#define ROSE_BinaryAnalysis_Disassembler_Aarch64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <Rose/BinaryAnalysis/Disassembler/Base.h>

#include <capstone/capstone.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

/** ARM instruction decoder for AArch64.
 *
 *  This is the decoder for the A64 instruction set of the AArch64 architecture. At the time of this writing, A64 is the only
 *  instruction set for this architecture.
 *
 *  Most of the useful disassembly methods can be found in the @ref Rose::BinaryAnalysis::Disassembler::Base "Base" superclass.
 *  Some of the constants have the same ill-defined meanings as they do in the Capstone library. */
class Aarch64: public Base {
public:
    // <rant>
    //
    //   ARM naming is a total mess! The term "ARM" is used for the company that licenses all this, and the architecture, and
    //   some of the instruction sets, and some of the implementations. For instance, "ARM10E" is an implementation of
    //   "ARMv5TE" architecture having the "A32" (a.k.a., "AArch32") and "T32" (a.k.a., "Thumb", which also sometimes means
    //   both "Thumb" and "Thumb-2") instruction sets, all of which was designed by "Arm Holdings". Another case in point: the
    //   "Thumb-2" instruction set was extended and named "ThumbEE" (why not just "Thumb-3"?), was erroneously called
    //   "Thumb-2EE" in some ARM documentation, and was marketed as "Jazelle RCT".  Then there's the inconsistencies with
    //   dashes and spaces, as in "ARM Cortex-A64" and "ARM Neoverse E1" which are both implementations created by Arm
    //   Holdings; and inconsistencies in capitalization as in "ARM" vs. "Arm" (as in "Arm Limited") vs. "arm" (as on the title
    //   page of Arm Limited's instruction reference guide); "Armv8" vs. "ARMv8", etc. It's no wonder that references like the
    //   Wikipedia page for ARM (if "ARM" is even the correct term) are riddled with mistakes, and disassemblers like Capstone
    //   are confusingly ambiguous, and ROSE itself has poor documentation for all things ARM related. The whole thing is a
    //   rat's nest probably creatied by some overzealous marketing department.
    //
    //   Since ROSE is using Capstone for the decoding, instead of spending more effort trying to make heads and tails of this
    //   mess, I'm going to just use the same parameters as Capstone, namely a set of non-orthogonal and ill-documented
    //   enums. Best of luck to you.
    //
    // </rant>

    /** Shared ownership pointer for an Aarch64 decoder. */
    using Ptr = Aarch64Ptr;

    /** Capstone "Mode type", limited to those related to AArch64. */
    enum Mode {
        MODE_MCLASS = CS_MODE_MCLASS,                   /**< Capstone: "ARM's Cortex-M series". */
    };

    /** Collection of Modes. */
    using Modes = BitFlags<Mode>;

private:
    Modes modes_;                                       // a subset of Capstone's cs_mode constants (warning: nonorthoganal concepts)
    csh capstone_;                                      // the capstone handle
    bool capstoneOpened_ = false;                       // whether capstone_ is initialized

protected:
    // Constructor for specific architecture.
    Aarch64(const Architecture::BaseConstPtr&, Modes);

public:
    /** Allocating constructor for Aarch64 decoder. */
    static Ptr instance(const Architecture::BaseConstPtr&, Modes modes = Modes());

    ~Aarch64();

    // overrides
    Base::Ptr clone() const override;
    SgAsmInstruction* disassembleOne(const MemoryMap::Ptr&, rose_addr_t startVa, AddressSet *successors=nullptr) override;
    SgAsmInstruction* makeUnknownInstruction(const Exception&) override;

private:
    void openCapstone();

    // Returns the opcode as a 32-bit value.
    uint32_t opcode(const cs_insn&);
    uint32_t opcode(SgAsmInstruction*);

    // Make a ROSE instruction operand from a Capstone operand
    SgAsmExpression* makeOperand(const cs_insn&, const cs_arm64_op&);

    // Make a ROSE register descriptor from a Capstone register enum constant.
    RegisterDescriptor makeRegister(arm64_reg);

    // Restrict a register to just part of a register
    RegisterDescriptor subRegister(RegisterDescriptor reg, int idx, arm64_vess elmtSize);

//    // Extract a particular element out of the expression and return a new expression.
//    SgAsmExpression* extractElement(SgAsmExpression*, arm64_vess elmtSizeSpec, int idx);

    // Extend an expression. Given an expression of some integer type, and given a destination type and an ARM extender
    // function, generate a new expression (if necessary) that represents the ARM extender function. This may consist of an
    // truncation and/or a signed or unsigned extend operation.
    SgAsmExpression* extendOperand(SgAsmExpression*, const cs_insn&, arm64_extender, SgAsmType*, arm64_shifter, unsigned shiftAmount) const;

    // Return a type for register.
    SgAsmType* registerType(RegisterDescriptor, arm64_vas);

    // Capstone doesn't return information about how much memory is read for a memory read operand. Therefore, we need to
    // partially decode instructions ourselves to get this information.
    SgAsmType* typeForMemoryRead(const cs_insn&);

    // Change a memory reference expresson's address by wrapping it in a SgAsmPreIncrementExpression or
    // SgAsmPostIncrementExpression if necessary.
    void wrapPrePostIncrement(SgAsmOperandList*, const cs_arm64&);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
