/* Disassembly specific to the PowerPC architecture. */
#ifndef ROSE_BinaryAnalysis_Disassembler_Powerpc_H
#define ROSE_BinaryAnalysis_Disassembler_Powerpc_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Base.h>

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionEnumsPowerpc.h>

#include <SgAsmIntegerValueExpression.h>

#include "integerOps.h"
#include "SageBuilderAsm.h"

#include <Sawyer/BitFlags.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

/** Disassembler for the PowerPC architecture. */
class Powerpc: public Base {
public:
    /** Reference counting pointer. */
    using Ptr = PowerpcPtr;

private:
    // Per-instruction state
    struct State {
        uint64_t ip;                                        // Instruction pointer
        uint32_t insn;                                      // 4-byte instruction word
        State(): ip(0), insn(0) {}
    };

    PowerpcWordSize wordSize_;
    ByteOrder::Endianness sex_;
    Sawyer::BitFlags<PowerpcCapability> capabilities_;
    bool strictReserved_ = false;                       // if true, then reserved instruction bits must be zero

protected:
    explicit Powerpc(const Architecture::BaseConstPtr&);

public:
    /** Allocating constructor for 32- or 64-bit disassembler. */
    static Ptr instance(const Architecture::BaseConstPtr&);

    /** Property: Whether to enforce reserved instruction fields.
     *
     *  If this property is true, then any instruction that has a reserved field that is not zero will be treated as an invalid
     *  instruction.
     *
     * @{ */
    bool strictReserved() const;
    void strictReserved(bool);
    /** @} */

    // Overrides documented in a super class
    virtual ~Powerpc() {}
    virtual Base::Ptr clone() const;
    virtual SgAsmInstruction *disassembleOne(const MemoryMap::Ptr &map, Address start_va, AddressSet *successors=NULL);
    virtual void assembleOne(SgAsmInstruction*, SgUnsignedCharList&) {abort();}
    virtual SgAsmInstruction *makeUnknownInstruction(const Exception&);

    /** Capabilities describe what instructions can be decoded.
     *
     *  The decoder capability set describes what subsets of the PowerPC complete instruction set can be decoded. Machine
     *  instructions that fall outside the selected subset(s) are decoded as "unknown" instructions.
     *
     * @{ */
    const Sawyer::BitFlags<PowerpcCapability>& capabilities() const;
    Sawyer::BitFlags<PowerpcCapability>& capabilities();
    /** @} */

private:
    // Same as Exception except with a different constructor for ease of use in Powerpc. This constructor should be used when
    // an exception occurs during disassembly of an instruction; it is not suitable for errors that occur before or after (use
    // superclass constructors for that case).
    class ExceptionPowerpc: public Exception {
    public:
        ExceptionPowerpc(const std::string &mesg, const State &state, size_t bit=0);
    };

    SgAsmRegisterReferenceExpression*
    makeRegister(State&, PowerpcRegisterClass reg_class, int reg_number,
                 PowerpcConditionRegisterAccessGranularity reg_grainularity = powerpc_condreggranularity_whole,
                 SgAsmType *type = NULL) const;

    /** Test whether instruction is valid for only PowerPC-64.
     *
     *  Returns true if the specified instruction kind is valid only for PowerPC-64 architectures and cannot be decoded by
     *  PowerPC-32 architectures. */
    bool is64bitInsn(PowerpcInstructionKind);

    /** Creates an instruction AST. */
    SgAsmPowerpcInstruction* makeInstructionWithoutOperands(uint64_t address, PowerpcInstructionKind, uint32_t insnBytes,
                                                            PowerpcCapability);

    // Helper function to use field definitions (counted with bits from left and inclusive on both sides) from manual.
    template <size_t First, size_t Last> uint64_t fld(State&) const;

    // Helper function to test whether reserved fields have appropriate values
    template<size_t First, size_t Last> bool reservedOk(State&) const;

    // Decoded fields from section 1.7.16 of the v2.01 UISA.
    bool AA(State &state) const;
    SgAsmRegisterReferenceExpression* BA(State &state) const;
    SgAsmRegisterReferenceExpression* BB(State &state) const;
    uint64_t BD(State &state) const;
    SgAsmRegisterReferenceExpression* BF_cr(State &state) const;
    SgAsmRegisterReferenceExpression* BF_fpscr(State &state) const;
    SgAsmRegisterReferenceExpression* BFA_cr(State &state) const;
    SgAsmRegisterReferenceExpression* BFA_fpscr(State &state) const;
    SgAsmValueExpression* BH(State &state) const;
    SgAsmRegisterReferenceExpression* BI(State &state) const;
    SgAsmValueExpression* BO(State &state) const;
    SgAsmRegisterReferenceExpression* BT(State &state) const;
    SgAsmValueExpression* D(State &state) const;
    SgAsmValueExpression* DS(State &state) const;
    SgAsmValueExpression* FLM(State &state) const;
    SgAsmRegisterReferenceExpression* FRA(State &state) const;
    SgAsmRegisterReferenceExpression* FRB(State &state) const;
    SgAsmRegisterReferenceExpression* FRC(State &state) const;
    SgAsmRegisterReferenceExpression* FRS(State &state) const;
    SgAsmRegisterReferenceExpression* FRT(State &state) const;
    SgAsmValueExpression* FXM(State &state) const;
    SgAsmValueExpression* L_10(State &state) const;
    SgAsmValueExpression* L_15(State &state) const;
    uint8_t L_sync(State &state) const;
    SgAsmValueExpression* LEV(State &state) const;
    uint64_t LI(State &state) const;
    bool LK(State &state) const;
    SgAsmValueExpression* MB_32bit(State &state) const;
    SgAsmValueExpression* ME_32bit(State &state) const;
    SgAsmValueExpression* MB_64bit(State &state) const;
    SgAsmValueExpression* ME_64bit(State &state) const;
    SgAsmValueExpression* NB(State &state) const;
    bool OE(State &state) const;
    SgAsmRegisterReferenceExpression* RA(State &state) const;
    SgAsmExpression* RA_or_zero(State &state) const;
    SgAsmRegisterReferenceExpression* RB(State &state) const;
    bool Rc(State &state) const;
    SgAsmRegisterReferenceExpression* RS(State &state) const;
    SgAsmRegisterReferenceExpression* RT(State &state) const;
    SgAsmValueExpression* SH_32bit(State &state) const;
    SgAsmValueExpression* SH_64bit(State &state) const;
    SgAsmValueExpression* SI(State &state) const;
    SgAsmRegisterReferenceExpression* SPR(State &state) const;
    SgAsmRegisterReferenceExpression* SR(State &state) const;
    SgAsmRegisterReferenceExpression* TBR(State &state) const;
    SgAsmValueExpression* TH(State &state) const;
    SgAsmValueExpression* TO(State &state) const;
    SgAsmValueExpression* U(State &state) const;
    SgAsmValueExpression* UI(State &state) const;
    SgAsmMemoryReferenceExpression* memref(State &state, SgAsmType* t) const;
    SgAsmMemoryReferenceExpression* memrefds(State &state, SgAsmType *t) const;
    SgAsmMemoryReferenceExpression* memrefra(State &state, SgAsmType *t) const;
    SgAsmMemoryReferenceExpression* memrefx(State &state, SgAsmType* t) const;
    SgAsmMemoryReferenceExpression* memrefu(State &state, SgAsmType* t) const;
    SgAsmMemoryReferenceExpression* memrefux(State &state, SgAsmType* t) const;

    // There are 15 different forms of PowerPC instructions, but all are 32-bit (fixed length instruction set).
    SgAsmPowerpcInstruction* decode_I_formInstruction(State&);
    SgAsmPowerpcInstruction* decode_B_formInstruction(State&);
    SgAsmPowerpcInstruction* decode_SC_formInstruction(State&);
    SgAsmPowerpcInstruction* decode_DS_formInstruction(State&);
    SgAsmPowerpcInstruction* decode_X_formInstruction_00(State&);
    SgAsmPowerpcInstruction* decode_X_formInstruction_1F(State&);
    SgAsmPowerpcInstruction* decode_X_formInstruction_3F(State&);
    SgAsmPowerpcInstruction* decode_XL_formInstruction(State&);
    SgAsmPowerpcInstruction* decode_XS_formInstruction(State&);
    SgAsmPowerpcInstruction* decode_A_formInstruction_00(State&);
    SgAsmPowerpcInstruction* decode_A_formInstruction_04(State&);
    SgAsmPowerpcInstruction* decode_A_formInstruction_3B(State&);
    SgAsmPowerpcInstruction* decode_A_formInstruction_3F(State&);
    SgAsmPowerpcInstruction* decode_MD_formInstruction(State&);
    SgAsmPowerpcInstruction* decode_MDS_formInstruction(State&);

    SgAsmIntegerValueExpression* makeBranchTarget( uint64_t targetAddr ) const;

    SgAsmPowerpcInstruction* disassemble(State&);

    // Initialize instances of this class. Called by constructor.
    void init();
    
    // Resets disassembler state to beginning of an instruction.
    void startInstruction(State &state, Address start_va, uint32_t c) const;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
