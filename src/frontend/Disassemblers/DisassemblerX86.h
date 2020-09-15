/* Disassembly specific to the x86 architecture. */

#ifndef ROSE_DISASSEMBLER_X86_H
#define ROSE_DISASSEMBLER_X86_H

#include "Disassembler.h"
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "InstructionEnumsX86.h"
#include "Cxx_GrammarSerialization.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Disassembler for the x86 architecture.  Most of the useful disassembly methods can be found in the superclass. There's
 *  really not much reason to use this class directly or to call any of these methods directly. */
class DisassemblerX86: public Disassembler {
    /* Per-disassembler settings; see init() */
    X86InstructionSize insnSize;                    /**< Default size of instructions, based on architecture; see init() */
    size_t wordSize;                                /**< Base word size. */

    /* Per-instruction settings; see startInstruction() */
    struct State {
        uint64_t ip;                                /**< Virtual address for start of instruction */
        SgUnsignedCharList insnbuf;                 /**< Buffer containing bytes of instruction */
        size_t insnbufat;                           /**< Index of next byte to be read from or write to insnbuf */

        /* Temporary flags set by the instruction; initialized by startInstruction() */
        X86SegmentRegister segOverride;             /**< Set by 0x26,0x2e,0x36,0x3e,0x64,0x65 prefixes */
        X86BranchPrediction branchPrediction;       /*FIXME: this seems to set only to x86_branch_prediction_true [RPM 2009-06-16] */
        bool branchPredictionEnabled;
        bool rexPresent, rexW, rexR, rexX, rexB;    /**< Set by 0x40-0x4f prefixes; extended registers present; see setRex() */
        bool sizeMustBe64Bit;                       /**< Set if effective operand size must be 64 bits. */
        bool operandSizeOverride;                   /**< Set by the 0x66 prefix; used by effectiveOperandSize() and mmPrefix() */
        bool addressSizeOverride;                   /**< Set by the 0x67 prefix; used by effectiveAddressSize() */
        bool lock;                                  /**< Set by the 0xf0 prefix */
        X86RepeatPrefix repeatPrefix;               /**< Set by 0xf2 (repne) and 0xf3 (repe) prefixes */
        bool modregrmByteSet;                       /**< True if modregrmByte is initialized */
        uint8_t modregrmByte;                       /**< Set by instructions that use ModR/M when the ModR/M byte is read */
        uint8_t modeField;                          /**< Value (0-3) of high-order two bits of modregrmByte; see getModRegRM() */
        uint8_t regField;                           /**< Value (0-7) of bits 3-5 inclusive of modregrmByte; see getModRegRM() */
        uint8_t rmField;                            /**< Value (0-7) of bits 0-3 inclusive of modregrmByte; see getModRegRM() */
        SgAsmExpression *modrm;                     /**< Register or memory ref expr built from modregrmByte; see getModRegRM() */
        SgAsmExpression *reg;                       /**< Register reference expression built from modregrmByte; see getModRegRM() */
        bool isUnconditionalJump;                   /**< True for jmp, farjmp, ret, retf, iret, and hlt */

        State()
            : ip(0), insnbufat(0), segOverride(x86_segreg_none), branchPrediction(x86_branch_prediction_none),
              branchPredictionEnabled(false), rexPresent(false), rexW(false), rexR(false), rexX(false), rexB(false),
              sizeMustBe64Bit(false), operandSizeOverride(false), addressSizeOverride(false), lock(false),
              repeatPrefix(x86_repeat_none), modregrmByteSet(false), modregrmByte(0), modeField(0), rmField(0),
              modrm(NULL), reg(NULL), isUnconditionalJump(false) {}
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize_common(S &s, const unsigned /*version*/) {
        // Most of the data members don't need to be saved because we'll only save/restore disassemblers that are between
        // instructions (we never save one while it's processing an instruction). Therefore, most of the data members can be
        // constructed in their initial state by a combination of default constructor and init().
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Disassembler);
        s & BOOST_SERIALIZATION_NVP(wordSize);
    }

    template<class S>
    void save(S &s, const unsigned version) const {
        serialize_common(s, version);
    }

    template<class S>
    void load(S &s, const unsigned version) {
        serialize_common(s, version);
        init(wordSize);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
    // Default constructor for serialization
    DisassemblerX86()
        : insnSize(x86_insnsize_none), wordSize(0) {}

public:
    explicit DisassemblerX86(size_t wordsize)
        : insnSize(x86_insnsize_none), wordSize(0) {
        init(wordsize);
    }

    virtual ~DisassemblerX86() {}

    virtual DisassemblerX86 *clone() const ROSE_OVERRIDE { return new DisassemblerX86(*this); }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual bool canDisassemble(SgAsmGenericHeader*) const ROSE_OVERRIDE;

    virtual Unparser::BasePtr unparser() const ROSE_OVERRIDE;

    virtual SgAsmInstruction *disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start_va,
                                             AddressSet *successors=NULL) ROSE_OVERRIDE;

    virtual SgAsmInstruction *makeUnknownInstruction(const Exception&) ROSE_OVERRIDE;


    /*========================================================================================================================
     * Data types
     *========================================================================================================================*/
private:

    /** Same as Disassembler::Exception except with a different constructor for ease of use in DisassemblerX86.  This
     *  constructor should be used when an exception occurs during disassembly of an instruction; it is not suitable for
     *  errors that occur before or after (use superclass constructors for that case). */
    class ExceptionX86: public Exception {
    public:
        ExceptionX86(const std::string &mesg, const State &state)
            : Exception(mesg, state.ip) {
            ASSERT_require(state.insnbufat <= state.insnbuf.size());
            if (state.insnbufat > 0)
                bytes = SgUnsignedCharList(&state.insnbuf[0], &state.insnbuf[0] + state.insnbufat);
            bit = 8 * state.insnbufat;
        }

        ExceptionX86(const std::string &mesg, const State &state, size_t bit)
            : Exception(mesg, state.ip) {
            ASSERT_require(state.insnbufat <= state.insnbuf.size());
            if (state.insnbufat > 0)
                bytes = SgUnsignedCharList(&state.insnbuf[0], &state.insnbuf[0] + state.insnbufat);
            this->bit = bit;
        }
    };

    /** ModR/M settings that create register expressions (or rmReturnNull for no register) */
    enum RegisterMode {
        rmLegacyByte, rmRexByte, rmWord, rmDWord, rmQWord, rmSegment, rmST, rmMM, rmXMM, rmControl, rmDebug, rmReturnNull
    };

    /* MMX registers? See mmPrefix method */
    enum MMPrefix {
        mmNone, mmF3, mm66, mmF2
    };


    /*========================================================================================================================
     * Methods for reading and writing bytes of the instruction.  These keep track of how much has been read or written.
     *========================================================================================================================*/
private:

    /** Returns the next byte of the instruction by looking at the insnbuf, insnbufsz, and insnbufat data members that were
     *  set by startInstruction(). Throws an exception for short reads or if we've read more than 15 bytes. The longest
     *  possible x86 instruction is 15 bytes. */
    uint8_t getByte(State &state) const;

    /** Returns the next two-byte, little endian word of the instruction by looking at the insnbuf, insnbufsz, and insnbufat
     *  data members that were set by startInstruction(). Throws an exception for short reads or if we've read more than 15
     *  bytes. The longest possible x86 instruction is 15 bytes. */
    uint16_t getWord(State &state) const;

    /** Returns the next four-byte, little endian double word of the instruction by looking at the insnbuf, insnbufsz, and
     *  insnbufat data members that were set by startInstruction(). Throws an exception for short reads or if we've read more
     *  than 15 bytes. The longest possible x86 instruction is 15 bytes. */
    uint32_t getDWord(State &state) const;

    /** Returns the next eight-byte, little endian quad word of the instruction by looking at the insnbuf, insnbufsz, and
     *  insnbufat data members that were set by startInstruction(). Throws an exception for short reads or if we've read more
     *  than 15 bytes. The longest possible x86 instruction is 15 bytes. */
    uint64_t getQWord(State &state) const;

    /*========================================================================================================================
     * Miscellaneous helper methods
     *========================================================================================================================*/
private:
    /** Constructs a register reference expression for the current data segment based on whether a segment override prefix has
     *  been encountered. */
    SgAsmExpression *currentDataSegment(State &state) const;

    /** Returns the size of instruction addresses. The effective address size is normally based on the default instruction
     *  size. However, if the disassembler encounters the 0x67 instruction prefix ("Address-size Override Prefix") as
     *  indicated by the addressSizeOverride data member being set, then other sizes are used. See pattent 6571330. */
    X86InstructionSize effectiveAddressSize(State &state) const;

    /** Returns the register mode for the instruction's effective operand size. */
    RegisterMode effectiveOperandMode(State &state) const {
        return sizeToMode(effectiveOperandSize(state));
    }

    /** Returns the size of the operands. The operand size is normally based on the default instruction size; however, if the
     *  disassembler encounters the 0x66 instruction prefix ("Precision-size Override Prefix") as indicated by the
     *  operandSizeOverride data member being set, then other sizes are used. See pattent 6571330. */
    X86InstructionSize effectiveOperandSize(State &state) const;

    /** Returns the data type for the instruction's effective operand size. */
    SgAsmType *effectiveOperandType(State &state) const {
        return sizeToType(effectiveOperandSize(state));
    }

    /** Returns true if we're disassembling 64-bit code. */
    bool longMode() const {
        return insnSize == x86_insnsize_64;
    }

    /* FIXME: documentation? */
    MMPrefix mmPrefix(State &state) const;

    /** Throws an exception if the instruction being disassembled is not valid for 64-bit mode. */
    void not64(State &state) const {
        if (longMode())
            throw ExceptionX86("not valid for 64-bit code", state);
    }

    /** Sets the rexPresent flag along with rexW, rexR, rexX, and/or rexB based on the instruction prefix, which should be a
     *  value between 0x40 and 0x4f, inclusive. */
    void setRex(State &state, uint8_t prefix) const;

    /** Returns the register mode for the specified instruction size. */
    static RegisterMode sizeToMode(X86InstructionSize);

    /** Returns a data type associated with an instruction size. For instance, a 32-bit instruction returns the type for a
     *  double word. */
    static SgAsmType *sizeToType(X86InstructionSize s);



    /*========================================================================================================================
     * Methods that construct something. (Their names all start with "make".)
     *========================================================================================================================*/
private:

    /** Constructs an expression for the specified address size. The @p bit_offset and @p bit_size are the offset and size
     *  where @p val was found in the instruction raw bytes. */
    SgAsmExpression *makeAddrSizeValue(State &state, int64_t val, size_t bit_offset, size_t bit_size) const;

    /** Creates an instruction with optional operands. Many of the instruction attributes come from the current state of this
     *  disassembler object (see the instruction-related data members below). In order that the new instruction contains the
     *  correct number of raw instruction bytes (p_raw_bytes) it should be called after all the instruction bytes have been
     *  read, otherwise remember to call set_raw_bytes() explicitly. */
    SgAsmX86Instruction *makeInstruction(State &state, X86InstructionKind kind, const std::string &mnemonic,
                                         SgAsmExpression *op1=NULL, SgAsmExpression *op2=NULL,
                                         SgAsmExpression *op3=NULL, SgAsmExpression *op4=NULL) const;

    /** Constructs a register reference expression for the instruction pointer register. */
    SgAsmRegisterReferenceExpression *makeIP() const;

    /* FIXME: documentation? */
    SgAsmRegisterReferenceExpression *makeOperandRegisterByte(State &state, bool rexExtension, uint8_t registerNumber) const;

    /* FIXME: documentation? */
    SgAsmRegisterReferenceExpression *makeOperandRegisterFull(State &state, bool rexExtension, uint8_t registerNumber) const;

    /** Constructs a register reference expression. The @p registerType is only used for vector registers that can have more
     *  than one type. */
    SgAsmRegisterReferenceExpression *makeRegister(State &state, uint8_t fullRegisterNumber, RegisterMode,
                                                   SgAsmType *registerType=NULL) const;

    /* FIXME: documentation? */
    SgAsmRegisterReferenceExpression *makeRegisterEffective(State &state, uint8_t fullRegisterNumber) const {
        return makeRegister(state, fullRegisterNumber, effectiveOperandMode(state));
    }

    /* FIXME: documentation? */
    SgAsmRegisterReferenceExpression *makeRegisterEffective(State &state, bool rexExtension, uint8_t registerNumber) const {
        return makeRegister(state, registerNumber + (rexExtension ? 8 : 0), effectiveOperandMode(state));
    }

    /** Constructs a register reference expression for a segment register. */
    SgAsmExpression *makeSegmentRegister(State &state, X86SegmentRegister so, bool insn64) const;



    /*========================================================================================================================
     * Methods for operating on the ModR/M byte.
     *========================================================================================================================*/
private:

    /** Decodes the ModR/M byte of an instruction. The ModR/M byte is used to carry operand information when the first byte
     *  (after prefixes) cannot do so.  It consists of three parts:
     *
     *     * Bits 6-7: the "Mod" (i.e., mode) bits. They are saved in the DisassemblerX86::modeField data member.  A mode of
     *       3 indicates that the "M" bits designate a register; otherwise the M bits are used for memory coding.
     *
     *     * Bits 3-5: the "R" (i.e., register) bits, saved in the DisassemblerX86::regField data member.
     *
     *     * Bits 0-2: the "M" (i.e., memory) bits, saved in the DisassemblerX86::rmField data member. These are used to
     *       specify or help specify a memory location except when the mode bits have the value 3.
     *
     * The @p regMode is the register kind for the "R" bits and is used when constructing the DisassemblerX86::reg data member.
     * The @p rmMode is the register kind for the "RM" field when the mode refers to a register. */
    void getModRegRM(State &state, RegisterMode regMode, RegisterMode rmMode, SgAsmType *t, SgAsmType *tForReg = NULL) const;

    /** Decodes the ModR/M byte to a memory reference expression. See makeModrmNormal(). */
    SgAsmMemoryReferenceExpression *decodeModrmMemory(State &state) const;

    /** If ModR/M is a memory reference, fill in its type; otherwise, make a register with the appropriate mode and put it
     *  into the modrm data member. */
    void fillInModRM(State &state, RegisterMode rmMode, SgAsmType *t) const;

    /** Builds the register or memory reference expression for the ModR/M byte. See getModRegRM(). */
    SgAsmExpression *makeModrmNormal(State &state, RegisterMode, SgAsmType *mrType) const;

    /** Builds the register reference expression for the ModR/M byte. See getModRegRM(). The @p mrType is only used for vector
     *  registers. */
    SgAsmRegisterReferenceExpression *makeModrmRegister(State &state, RegisterMode, SgAsmType* mrType=NULL) const;

    /** Throw an exceptions if the instruction requires the "Mod" part of the ModR/M byte to have the value 3. */
    void requireMemory(State &state) const {
        if (!state.modregrmByteSet)
            throw ExceptionX86("requires Mod/RM byte", state);
        if (state.modeField == 3)
            throw ExceptionX86("requires memory", state);
    }



    /*========================================================================================================================
     * Methods that construct an SgAsmExpression for an immediate operand.
     *========================================================================================================================*/
private:

    SgAsmExpression *getImmByte(State &state) const;
    SgAsmExpression *getImmWord(State &state) const;
    SgAsmExpression* getImmDWord(State &state) const;
    SgAsmExpression* getImmQWord(State &state) const;
    SgAsmExpression *getImmForAddr(State &state) const;
    SgAsmExpression *getImmIv(State &state) const;
    SgAsmExpression *getImmJz(State &state) const;
    SgAsmExpression *getImmByteAsIv(State &state) const;
    SgAsmExpression *getImmIzAsIv(State &state) const;
    SgAsmExpression *getImmJb(State &state) const;




    /*========================================================================================================================
     * Main disassembly functions, each generally containing a huge "switch" statement based on one of the opcode bytes.
     *========================================================================================================================*/
private:

    /** Disassembles an instruction. This is the workhorse: it reads and decodes bytes of the instruction in a huge switch
     *  statement. */
    SgAsmX86Instruction *disassemble(State &state) const;

    /** Disassemble an instruction following the 0x0f prefix. */
    SgAsmX86Instruction *decodeOpcode0F(State &state) const;

    /** Disassemble SSE3 instructions. */
    SgAsmX86Instruction *decodeOpcode0F38(State &state) const;

    /** Disassembles an instruction with primary opcode 0xd8 */
    SgAsmX86Instruction *decodeX87InstructionD8(State &state) const;

    /** Disassembles an instruction with primary opcode 0xd9 */
    SgAsmX86Instruction *decodeX87InstructionD9(State &state) const;

    /** Disassembles an instruction with primary opcode 0xda */
    SgAsmX86Instruction *decodeX87InstructionDA(State &state) const;

    /** Disassembles an instruction with primary opcode 0xdb */
    SgAsmX86Instruction *decodeX87InstructionDB(State &state) const;

    /** Disassembles an instruction with primary opcode 0xdc */
    SgAsmX86Instruction *decodeX87InstructionDC(State &state) const;

    /** Disassembles an instruction with primary opcode 0xdd */
    SgAsmX86Instruction *decodeX87InstructionDD(State &state) const;

    /** Disassembles an instruction with primary opcode 0xde */
    SgAsmX86Instruction *decodeX87InstructionDE(State &state) const;

    /** Disassembles an instruction with primary opcode 0xdf */
    SgAsmX86Instruction *decodeX87InstructionDF(State &state) const;

    /** Disassembles ADD, OR, ADC, SBB, AND, SUB, XOR, CMP. */
    SgAsmX86Instruction *decodeGroup1(State &state, SgAsmExpression *imm) const;

    /** Disassembles POP */
    SgAsmX86Instruction *decodeGroup1a(State &state) const;

    /** Disassembles ROL, ROR, RCL, RCR, SHL, SHR, SHL, SAR */
    SgAsmX86Instruction *decodeGroup2(State &state, SgAsmExpression *count) const;

    /** Disassembles TEST, NOT, NEG, MUL, IMUL, DIV, IDIV */
    SgAsmX86Instruction *decodeGroup3(State &state, SgAsmExpression *immMaybe) const;

    /** Disassembles INC, DEC */
    SgAsmX86Instruction *decodeGroup4(State &state) const;

    /** Disassembles INC, DEC, CALL, FARCALL, JMP, FARJMP, PUSH */
    SgAsmX86Instruction *decodeGroup5(State &state) const;

    /** Disassembles SLDT, STR, LLDT, LTR, VERR, VERW */
    SgAsmX86Instruction *decodeGroup6(State &state) const;

    /** Disassembles VMCALL, VMLAUNCH, VMRESUME, VMXOFF, SGDT, MONITOR, MWAIT, SIDT, SGDT, XGETBV, XSETBV, LGDT, VMRUN,
     *  VMMCALL, VMLOAD, VMSAVE, STGI, CLGI, SKINIT, INVLPGA, LIDT, SMSW, LMSW, SWAPGS, RDTSCP, INVLPG */
    SgAsmX86Instruction *decodeGroup7(State &state) const;

    /** Disassembles BT, BTS, BTR, BTC */
    SgAsmX86Instruction *decodeGroup8(State &state, SgAsmExpression *imm) const;

    /** Disassembles MOV */
    SgAsmX86Instruction *decodeGroup11(State &state, SgAsmExpression *imm) const;

    /** Disassembles FXSAVE, FXRSTOR, LDMXCSR, STMXCSR, XSAVE, LFENCE, XRSTOR, MFENCE, SFENCE, CLFLUSH */
    SgAsmX86Instruction *decodeGroup15(State &state) const;

    /** Disassembles PREFETCHNTA, PREFETCH0, PREFETCH1, PREFETCH2, PREFETCH */
    SgAsmX86Instruction *decodeGroup16(State &state) const;

    /** Disassembles PREFETCH, PREFETCHW */
    SgAsmX86Instruction *decodeGroupP(State &state) const;



    /*========================================================================================================================
     * Supporting functions
     *========================================================================================================================*/
private:
    // Initialize instances of this class. Called by constructor.
    void init(size_t wordsize);

#if 0 // is this ever used?
    /** Resets disassembler state to beginning of an instruction for assembly. */
    void startInstruction(State &state, SgAsmX86Instruction *insn) const {
        startInstruction(insn->get_address(), NULL, 0);
        insnSize = insn->get_baseSize();
        state.lock = insn->get_lockPrefix();
        state.branchPrediction = insn->get_branchPrediction();
        state.branchPredictionEnabled = state.branchPrediction != x86_branch_prediction_none;
        state.segOverride = insn->get_segmentOverride();
    }
#endif

    // Resets disassembler state to beginning of an instruction for disassembly.
    void startInstruction(State &state, rose_addr_t start_va, const uint8_t *buf, size_t bufsz) const {
        state.ip = start_va;
        state.insnbuf = SgUnsignedCharList(buf, buf+bufsz);
        state.insnbufat = 0;

        // Prefix flags
        state.segOverride = x86_segreg_none;
        state.branchPrediction = x86_branch_prediction_none;
        state.branchPredictionEnabled = false;
        state.rexPresent = state.rexW = state.rexR = state.rexX = state.rexB = false;
        state.sizeMustBe64Bit = false;
        state.operandSizeOverride = false;
        state.addressSizeOverride = false;
        state.lock = false;
        state.repeatPrefix = x86_repeat_none;
        state.modregrmByteSet = false;
        state.modregrmByte = state.modeField = state.regField = state.rmField = 0; /*arbitrary since modregrmByteSet is false*/
        state.modrm = state.reg = NULL;
        state.isUnconditionalJump = false;
    }

    // Add comments to any IP relative addition expressions. We're not constant folding these because it's sometimes useful to
    // know that the address is relative to the instruction address, but the comment is useful for understanding the disassembly.
    void commentIpRelative(SgAsmInstruction*);
};

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::DisassemblerX86);
#endif

#endif
#endif
