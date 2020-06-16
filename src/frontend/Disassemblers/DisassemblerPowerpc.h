/* Disassembly specific to the PowerPC architecture. */

#ifndef ROSE_DISASSEMBLER_POWERPC_H
#define ROSE_DISASSEMBLER_POWERPC_H

#include "Disassembler.h"
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "integerOps.h"
#include "SageBuilderAsm.h"

namespace Rose {
namespace BinaryAnalysis {

/** Disassembler for the PowerPC architecture. */
class DisassemblerPowerpc: public Disassembler {
    // Per-instruction state
    struct State {
        uint64_t ip;                                        // Instruction pointer
        uint32_t insn;                                      // 4-byte instruction word
        State(): ip(0), insn(0) {}
    };

    PowerpcWordSize wordSize_;
    ByteOrder::Endianness sex_;

public:
    /** Constructor for 32- or 64-bit disassembler. */
    explicit DisassemblerPowerpc(PowerpcWordSize wordSize, ByteOrder::Endianness sex)
        : wordSize_(wordSize), sex_(sex) {
        init();
    }
    
    // Overrides documented in a super class
    virtual ~DisassemblerPowerpc() {}
    virtual DisassemblerPowerpc *clone() const { return new DisassemblerPowerpc(*this); }
    virtual bool canDisassemble(SgAsmGenericHeader*) const;
    virtual Unparser::BasePtr unparser() const;
    virtual SgAsmInstruction *disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start_va, AddressSet *successors=NULL);
    virtual void assembleOne(SgAsmInstruction*, SgUnsignedCharList&) {abort();}
    virtual SgAsmInstruction *makeUnknownInstruction(const Exception&);

private:
    // Same as Disassembler::Exception except with a different constructor for ease of use in DisassemblerPowerpc. This
    // constructor should be used when an exception occurs during disassembly of an instruction; it is not suitable for errors
    // that occur before or after (use superclass constructors for that case).
    class ExceptionPowerpc: public Exception {
    public:
        ExceptionPowerpc(const std::string &mesg, const State &state, size_t bit=0)
            : Exception(mesg, state.ip) {
            // Convert four-byte instruction to big-endian buffer. Note that PowerPC is big-endian, but PowerPC can support
            // both big- and little-endian processor modes (with much weirdness; e.g. PDP endian like propoerties). */
            bytes.push_back((state.insn>>24) & 0xff);
            bytes.push_back((state.insn>>16) & 0xff);
            bytes.push_back((state.insn>>8) & 0xff);
            bytes.push_back(state.insn & 0xff);
            ASSERT_require(bit<=32);
            this->bit = 8*(4-(bit/8)) + bit%8;          // convert from native uint32_t bit position to big-endian
        }
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
    SgAsmPowerpcInstruction* makeInstructionWithoutOperands(uint64_t address, const std::string& mnemonic, PowerpcInstructionKind,
                                                            uint32_t insnBytes);

    // Helper function to use field definitions (counted with bits from left and inclusive on both sides) from manual.
    template <size_t First, size_t Last> uint64_t fld(State&) const;

    // Decoded fields from section 1.7.16 of the v2.01 UISA.
    bool AA(State &state) const {
        return fld<30, 30>(state);
    }

    SgAsmRegisterReferenceExpression* BA(State &state) const {
        return makeRegister(state, powerpc_regclass_cr, fld<11, 15>(state), powerpc_condreggranularity_bit);
    }

    SgAsmRegisterReferenceExpression* BB(State &state) const {
        return makeRegister(state, powerpc_regclass_cr, fld<16, 20>(state), powerpc_condreggranularity_bit);
    }

    uint64_t BD(State &state) const {
        return IntegerOps::signExtend<16, 64>((uint64_t)state.insn & 0xfffc);
    }

    SgAsmRegisterReferenceExpression* BF_cr(State &state) const {
        return makeRegister(state, powerpc_regclass_cr, fld<6, 8>(state), powerpc_condreggranularity_field);
    }

    SgAsmRegisterReferenceExpression* BF_fpscr(State &state) const {
        return makeRegister(state, powerpc_regclass_fpscr, fld<6, 8>(state), powerpc_condreggranularity_field);
    }

    SgAsmRegisterReferenceExpression* BFA_cr(State &state) const {
        return makeRegister(state, powerpc_regclass_cr, fld<11, 13>(state), powerpc_condreggranularity_field);
    }

    SgAsmRegisterReferenceExpression* BFA_fpscr(State &state) const {
        return makeRegister(state, powerpc_regclass_fpscr, fld<11, 13>(state), powerpc_condreggranularity_field);
    }

    SgAsmValueExpression* BH(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<19, 20>(state));
    }

    SgAsmRegisterReferenceExpression* BI(State &state) const {
        return BA(state);
    }

    SgAsmValueExpression* BO(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<6, 10>(state));
    }

    SgAsmRegisterReferenceExpression* BT(State &state) const {
        return makeRegister(state, powerpc_regclass_cr, fld<6, 10>(state), powerpc_condreggranularity_bit);
    }

    SgAsmValueExpression* D(State &state) const {
        switch (wordSize_) {
            case powerpc_32:
                return SageBuilderAsm::buildValueU32(IntegerOps::signExtend<16, 32>((uint64_t)fld<16, 31>(state)));
            case powerpc_64:
                return SageBuilderAsm::buildValueU64(IntegerOps::signExtend<16, 64>((uint64_t)fld<16, 31>(state)));
        }
        ASSERT_not_reachable("invalid word size");
    }

    SgAsmValueExpression* DS(State &state) const {
        switch (wordSize_) {
            case powerpc_32:
                return SageBuilderAsm::buildValueU32(IntegerOps::signExtend<16, 32>((uint64_t)fld<16, 31>(state) & 0xfffc));
            case powerpc_64:
                return SageBuilderAsm::buildValueU64(IntegerOps::signExtend<16, 64>((uint64_t)fld<16, 31>(state) & 0xfffc));
        }
        ASSERT_not_reachable("invalid word size");
    }

    SgAsmValueExpression* FLM(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<7, 14>(state));
    }

    SgAsmRegisterReferenceExpression* FRA(State &state) const {
        return makeRegister(state, powerpc_regclass_fpr, fld<11, 15>(state));
    }

    SgAsmRegisterReferenceExpression* FRB(State &state) const {
        return makeRegister(state, powerpc_regclass_fpr, fld<16, 20>(state));
    }

    SgAsmRegisterReferenceExpression* FRC(State &state) const {
        return makeRegister(state, powerpc_regclass_fpr, fld<21, 25>(state));
    }

    SgAsmRegisterReferenceExpression* FRS(State &state) const {
        return makeRegister(state, powerpc_regclass_fpr, fld<6, 10>(state));
    }

    SgAsmRegisterReferenceExpression* FRT(State &state) const {
        return FRS(state);
    }

    SgAsmValueExpression* FXM(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<12, 19>(state));
    }
          
    SgAsmValueExpression* L_10(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<10, 10>(state));
    }

    SgAsmValueExpression* L_15(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<15, 15>(state));
    }

    uint8_t L_sync(State &state) const {
        return fld<9, 10>(state);
    }

    SgAsmValueExpression* LEV(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<20, 26>(state));
    }

    uint64_t LI(State &state) const {
        return IntegerOps::signExtend<26, 64>(uint64_t(fld<6, 29>(state) * 4));
    }

    bool LK(State &state) const {
        return fld<31, 31>(state);
    }

    SgAsmValueExpression* MB_32bit(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<21, 25>(state));
    }

    SgAsmValueExpression* ME_32bit(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<26, 30>(state));
    }

    SgAsmValueExpression* MB_64bit(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<21, 25>(state) + 32 * fld<26, 26>(state));
    } 

    SgAsmValueExpression* ME_64bit(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<21, 25>(state) + 32 * fld<26, 26>(state));
    }   

    SgAsmValueExpression* NB(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<16, 20>(state) == 0 ? 32 : fld<16, 20>(state));
    }

    bool OE(State &state) const {
        return fld<21, 21>(state);
    }

    SgAsmRegisterReferenceExpression* RA(State &state) const {
        return makeRegister(state, powerpc_regclass_gpr, fld<11, 15>(state));
    }

    SgAsmExpression* RA_or_zero(State &state) const {
        return fld<11, 15>(state) == 0 ? (SgAsmExpression*)SageBuilderAsm::buildValueU8(0) : RA(state);
    }

    SgAsmRegisterReferenceExpression* RB(State &state) const {
        return makeRegister(state, powerpc_regclass_gpr, fld<16, 20>(state));
    }

    bool Rc(State &state) const {
        return fld<31, 31>(state);
    }

    SgAsmRegisterReferenceExpression* RS(State &state) const {
        return makeRegister(state, powerpc_regclass_gpr, fld<6, 10>(state));
    }

    SgAsmRegisterReferenceExpression* RT(State &state) const {
        return RS(state);
    }

    SgAsmValueExpression* SH_32bit(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<16, 20>(state));
    }

    SgAsmValueExpression* SH_64bit(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<16, 20>(state) + fld<30, 30>(state) * 32); // FIXME check
    } 

    SgAsmValueExpression* SI(State &state) const {
        return D(state);
    }

    SgAsmRegisterReferenceExpression* SPR(State &state) const {
        return makeRegister(state, powerpc_regclass_spr, fld<16, 20>(state) * 32 + fld<11, 15>(state));
    }

    SgAsmRegisterReferenceExpression* SR(State &state) const {
        return makeRegister(state, powerpc_regclass_sr, fld<12, 15>(state));
    }

    SgAsmRegisterReferenceExpression* TBR(State &state) const {
        return makeRegister(state, powerpc_regclass_tbr, fld<16, 20>(state) * 32 + fld<11, 15>(state));
    }

    SgAsmValueExpression* TH(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<9, 10>(state));
    }

    SgAsmValueExpression* TO(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<6, 10>(state));
    }

    SgAsmValueExpression* U(State &state) const {
        return SageBuilderAsm::buildValueU8(fld<16, 19>(state));
    }

    SgAsmValueExpression* UI(State &state) const {
        switch (wordSize_) {
            case powerpc_32:
                return SageBuilderAsm::buildValueU32(fld<16, 31>(state));
            case powerpc_64:
                return SageBuilderAsm::buildValueU64(fld<16, 31>(state));
        }
        ASSERT_not_reachable("invalid word size");
    }

    SgAsmMemoryReferenceExpression* memref(State &state, SgAsmType* t) const {
        return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA_or_zero(state), D(state)), NULL, t);
    }

    SgAsmMemoryReferenceExpression* memrefds(State &state, SgAsmType *t) const {
        return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA_or_zero(state), DS(state)), NULL, t);
    }

    SgAsmMemoryReferenceExpression* memrefra(State &state, SgAsmType *t) const {
        return SageBuilderAsm::buildMemoryReferenceExpression(RA_or_zero(state), NULL, t);
    }
    
    SgAsmMemoryReferenceExpression* memrefx(State &state, SgAsmType* t) const {
        return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA_or_zero(state), RB(state)),
                                                              NULL, t);
    }

    SgAsmMemoryReferenceExpression* memrefu(State &state, SgAsmType* t) const {
        if (fld<11, 15>(state) == 0)
            throw ExceptionPowerpc("bits 11-15 must be nonzero", state);
        return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA(state), D(state)), NULL, t);
    }

    SgAsmMemoryReferenceExpression* memrefux(State &state, SgAsmType* t) const {
        if (fld<11, 15>(state) == 0)
            throw ExceptionPowerpc("bits 11-15 must be nonzero", state);
        return SageBuilderAsm::buildMemoryReferenceExpression(SageBuilderAsm::buildAddExpression(RA(state), RB(state)), NULL, t);
    }

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
    void startInstruction(State &state, rose_addr_t start_va, uint32_t c) const {
        state.ip = start_va;
        state.insn = c;
    }
};

} // namespace
} // namespace

#endif
#endif
