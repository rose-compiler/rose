#ifndef ROSE_DISASSEMBLER_ARM_H
#define ROSE_DISASSEMBLER_ARM_H
#include <Disassembler.h>
#ifdef ROSE_ENABLE_ASM_A64

#include <capstone/capstone.h>

namespace Rose {
namespace BinaryAnalysis {

/** ARM instruction decoder.
 *
 * Most of the useful disassembly methods can be found in the @ref Disassembler superclass.  Some of the constants
 * have the same ill-defined meanings as they do in the Capstone library. */
class DisassemblerArm: public Disassembler {
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

    /** Capstone "Architecture type" limited to those related to ARM. */
    enum Architecture {
        ARCH_ARM = CS_ARCH_ARM,                         /**< Capstone: "ARM architecture (including Thumb, Thumb)". */
        ARCH_ARM64 = CS_ARCH_ARM64                      /**< Capstone: "ARM-64, also called AArch64." */
    };

    /** Capstone "Mode type", limited to those related to ARM. Warning: these are non-orthogonal concepts. */
    enum Mode {
        MODE_ARM32 = CS_MODE_ARM,                       /**< Capstone: "32-bit ARM". */ // probably zero, not really a bit flag
        MODE_THUMB = CS_MODE_THUMB,                     /**< Capstone: "ARM's Thumb mode, including Thumb-2". */
        MODE_MCLASS = CS_MODE_MCLASS,                   /**< Capstone: "ARM's Cortex-M series". */
        MODE_V8 = CS_MODE_V8                            /**< Capstone: "ARMv8 A32 encodngs for ARM". */
    };

    /** Collection of Modes. */
    using Modes = BitFlags<Mode>;

private:
    Architecture arch_;                                 // a subset of Capstone's cs_arch constants
    Modes modes_;                                       // a subset of Capstone's cs_mode constants (warning: nonorthoganal concepts)
    csh capstone_;                                      // the capstone handle
    bool capstoneOpened_;                               // whether capstone_ is initialized

public:
    /** Constructor for specific architecture. */
    DisassemblerArm(Architecture arch, Modes modes = Modes())
        : arch_(arch), modes_(modes), capstoneOpened_(false) {
        init();
    }

    ~DisassemblerArm();

    // overrides
    bool canDisassemble(SgAsmGenericHeader*) const override;
    Disassembler* clone() const override;
    Unparser::BasePtr unparser() const override;
    SgAsmInstruction* disassembleOne(const MemoryMap::Ptr&, rose_addr_t startVa, AddressSet *successors=NULL) override;
    SgAsmInstruction* makeUnknownInstruction(const Exception&) override;

private:
    void init();

    // Make a ROSE instruction operand from a Capstone operand
    SgAsmExpression* makeOperand(const cs_insn&, const cs_arm64_op&);

    // Make a ROSE register descriptor from a Capstone register enum constant.
    RegisterDescriptor makeRegister(arm64_reg);

    // Extend an expression. Given an expression of some integer type, and given a destination type and an ARM extender function, generate a new expression (if necessary)
    // that represents the ARM extender function. This may consist of an truncation and/or a signed or unsigned extend operation.
    SgAsmExpression* extendOperand(SgAsmExpression*, arm64_extender, SgAsmType*, arm64_shifter, unsigned shiftAmount) const;

    // Return a type for register.
    SgAsmType* registerType(arm64_reg, arm64_vas);

    // Capstone doesn't return information about how much memory is read for a memory read operand. Therefore, we need to
    // partially decode instructions ourselves to get this information.
    SgAsmType* typeForMemoryRead(const cs_insn&);

    // Change a memory reference expresson's address by wrapping it in a SgAsmPreIncrementExpression or SgAsmPostIncrementExpression if necessary.
    void wrapPrePostIncrement(SgAsmOperandList*, const cs_arm64&);
};

} // namespace
} // namespace

#endif
#endif
