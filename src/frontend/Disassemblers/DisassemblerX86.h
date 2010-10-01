/* Disassembly specific to the x86 architecture. */

#ifndef ROSE_DISASSEMBLER_X86_H
#define ROSE_DISASSEMBLER_X86_H

/** Disassembler for the x86 architecture.  Most of the useful disassembly methods can be found in the superclass. There's
 *  really not much reason to use this class directly or to call any of these methods directly. */
class DisassemblerX86: public Disassembler {


    /*========================================================================================================================
     * Public methods
     *========================================================================================================================*/
public:
    DisassemblerX86(size_t wordsize)
        : insnSize(x86_insnsize_none), ip(0), insnbufat(0), segOverride(x86_segreg_none), 
          branchPrediction(x86_branch_prediction_none), branchPredictionEnabled(false), rexPresent(false), rexW(false), 
          rexR(false), rexX(false), rexB(false), sizeMustBe64Bit(false), operandSizeOverride(false), addressSizeOverride(false),
          lock(false), repeatPrefix(x86_repeat_none), modregrmByteSet(false), modregrmByte(0), modeField(0), rmField(0), 
          modrm(NULL), reg(NULL), isUnconditionalJump(false) {
        init(wordsize);
    }

    DisassemblerX86(const DisassemblerX86 &other)
        : Disassembler(other), insnSize(other.insnSize), ip(other.ip), insnbufat(other.insnbufat),
          segOverride(other.segOverride), branchPrediction(other.branchPrediction),
          branchPredictionEnabled(other.branchPredictionEnabled), rexPresent(other.rexPresent), rexW(other.rexW), 
          rexR(other.rexR), rexX(other.rexX), rexB(other.rexB), sizeMustBe64Bit(other.sizeMustBe64Bit),
          operandSizeOverride(other.operandSizeOverride), addressSizeOverride(other.addressSizeOverride),
          lock(other.lock), repeatPrefix(other.repeatPrefix), modregrmByteSet(other.modregrmByteSet),
          modregrmByte(other.modregrmByte), modeField(other.modeField), rmField(other.rmField), modrm(other.modrm),
          reg(other.reg), isUnconditionalJump(other.isUnconditionalJump) {
    }
    
    virtual ~DisassemblerX86() {}

    virtual DisassemblerX86 *clone() const { return new DisassemblerX86(*this); }

    /** See Disassembler::can_disassemble */
    virtual bool can_disassemble(SgAsmGenericHeader*) const;

    /** See Disassembler::disassembleOne */
    virtual SgAsmInstruction *disassembleOne(const MemoryMap *map, rose_addr_t start_va, AddressSet *successors=NULL);

    /** Make an unknown instruction from an exception. */
    virtual SgAsmInstruction *make_unknown_instruction(const Exception&);


    /*========================================================================================================================
     * Data types
     *========================================================================================================================*/
private:

    /** Same as Disassembler::Exception except with a different constructor for ease of use in DisassemblerX86.  This
     *  constructor should be used when an exception occurs during disassembly of an instruction; it is not suitable for
     *  errors that occur before or after (use superclass constructors for that case). */
    class ExceptionX86: public Exception {
    public:
        ExceptionX86(const std::string &mesg, const DisassemblerX86 *d)
            : Exception(mesg, d->ip, d->insnbuf, 8*d->insnbufat)
            {}
        ExceptionX86(const std::string &mesg, const DisassemblerX86 *d, size_t bit)
            : Exception(mesg, d->ip, d->insnbuf, bit)
            {}
    };

    /** ModR/M settings that create register expressions (or rmReturnNull for no register) */
    enum RegisterMode {
        rmLegacyByte, rmRexByte, rmWord, rmDWord, rmQWord, rmSegment, rmST, rmMM, rmXMM, rmControl, rmDebug, rmReturnNull
    };

    /* MMX registers? See mmPrefix method */
    enum MMPrefix {mmNone, mmF3, mm66, mmF2};




    /*========================================================================================================================
     * Methods for reading and writing bytes of the instruction.  These keep track of how much has been read or written.
     *========================================================================================================================*/
private:

    /** Returns the next byte of the instruction by looking at the insnbuf, insnbufsz, and insnbufat data members that were
     *  set by startInstruction(). Throws an exception for short reads or if we've read more than 15 bytes. The longest
     *  possible x86 instruction is 15 bytes. */
    uint8_t getByte();

    /** Returns the next two-byte, little endian word of the instruction by looking at the insnbuf, insnbufsz, and insnbufat
     *  data members that were set by startInstruction(). Throws an exception for short reads or if we've read more than 15
     *  bytes. The longest possible x86 instruction is 15 bytes. */
    uint16_t getWord();

    /** Returns the next four-byte, little endian double word of the instruction by looking at the insnbuf, insnbufsz, and
     *  insnbufat data members that were set by startInstruction(). Throws an exception for short reads or if we've read more
     *  than 15 bytes. The longest possible x86 instruction is 15 bytes. */
    uint32_t getDWord();

    /** Returns the next eight-byte, little endian quad word of the instruction by looking at the insnbuf, insnbufsz, and
     *  insnbufat data members that were set by startInstruction(). Throws an exception for short reads or if we've read more
     *  than 15 bytes. The longest possible x86 instruction is 15 bytes. */
    uint64_t getQWord();

    /*========================================================================================================================
     * Miscellaneous helper methods
     *========================================================================================================================*/
private:
    /** Constructs a register reference expression for the current data segment based on whether a segment override prefix has
     *  been encountered. */
    SgAsmExpression *currentDataSegment() const;

    /** Returns the size of instruction addresses. The effective address size is normally based on the default instruction
     *  size. However, if the disassembler encounters the 0x67 instruction prefix ("Address-size Override Prefix") as
     *  indicated by the addressSizeOverride data member being set, then other sizes are used. See pattent 6571330. */
    X86InstructionSize effectiveAddressSize() const;

    /** Returns the register mode for the instruction's effective operand size. */
    RegisterMode effectiveOperandMode() const {
        return sizeToMode(effectiveOperandSize());
    }

    /** Returns the size of the operands. The operand size is normally based on the default instruction size; however, if the
     *  disassembler encounters the 0x66 instruction prefix ("Precision-size Override Prefix") as indicated by the
     *  operandSizeOverride data member being set, then other sizes are used. See pattent 6571330. */
    X86InstructionSize effectiveOperandSize() const;

    /** Returns the data type for the instruction's effective operand size. */
    SgAsmType *effectiveOperandType() const {
        return sizeToType(effectiveOperandSize());
    }

    /** Returns true if we're disassembling 64-bit code. */
    bool longMode() const {
        return insnSize == x86_insnsize_64;
    }

    /* FIXME: documentation? */
    MMPrefix mmPrefix() const;

    /** Throws an exception if the instruction being disassembled is not valid for 64-bit mode. */
    void not64() const {
        if (longMode())
            throw ExceptionX86("not valid for 64-bit code", this);
    }

    /** Sets the rexPresent flag along with rexW, rexR, rexX, and/or rexB based on the instruction prefix, which should be a
     *  value between 0x40 and 0x4f, inclusive. */
    void setRex(uint8_t prefix);

    /** Returns the register mode for the specified instruction size. */
    static RegisterMode sizeToMode(X86InstructionSize);

    /** Returns a register position for an instruction size.  For instance, x86_regpos_dword is returned for 32-bit
     *  instructions. */
    static X86PositionInRegister sizeToPos(X86InstructionSize s);

    /** Returns a data type associated with an instruction size. For instance, a 32-bit instruction returns the type for a
     *  double word. */
    static SgAsmType *sizeToType(X86InstructionSize s);



    /*========================================================================================================================
     * Methods that construct something. (Their names all start with "make".)
     *========================================================================================================================*/
private:

    /** Constructs an expression for the specified address size. The @p bit_offset and @p bit_size are the offset and size
     *  where @p val was found in the instruction raw bytes. */
    SgAsmExpression *makeAddrSizeValue(int64_t val, size_t bit_offset, size_t bit_size);

    /** Creates an instruction with optional operands. Many of the instruction attributes come from the current state of this
     *  disassembler object (see the instruction-related data members below). In order that the new instruction contains the
     *  correct number of raw instruction bytes (p_raw_bytes) it should be called after all the instruction bytes have been
     *  read, otherwise remember to call set_raw_bytes() explicitly. */
    SgAsmx86Instruction *makeInstruction(X86InstructionKind kind, const std::string &mnemonic,
                                         SgAsmExpression *op1=NULL, SgAsmExpression *op2=NULL,
                                         SgAsmExpression *op3=NULL, SgAsmExpression *op4=NULL);

    /** Constructs a register reference expression for the instruction pointer register. */
    SgAsmx86RegisterReferenceExpression *makeIP();

    /* FIXME: documentation? */
    SgAsmx86RegisterReferenceExpression *makeOperandRegisterByte(bool rexExtension, uint8_t registerNumber);

    /* FIXME: documentation? */
    SgAsmx86RegisterReferenceExpression *makeOperandRegisterFull(bool rexExtension, uint8_t registerNumber);

    /** Constructs a register reference expression. The @p registerType is only used for vector registers that can have more
     *  than one type. */
    SgAsmx86RegisterReferenceExpression *makeRegister(uint8_t fullRegisterNumber, RegisterMode, SgAsmType *registerType=NULL) const;

    /* FIXME: documentation? */
    SgAsmx86RegisterReferenceExpression *makeRegisterEffective(uint8_t fullRegisterNumber) {
        return makeRegister(fullRegisterNumber, effectiveOperandMode());
    }

    /* FIXME: documentation? */
    SgAsmx86RegisterReferenceExpression *makeRegisterEffective(bool rexExtension, uint8_t registerNumber) {
        return makeRegister(registerNumber + (rexExtension ? 8 : 0), effectiveOperandMode());
    }

    /** Constructs a register reference expression for a segment register. */
    SgAsmExpression *makeSegmentRegister(X86SegmentRegister so, bool insn64) const;



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
    void getModRegRM(RegisterMode regMode, RegisterMode rmMode, SgAsmType *t, SgAsmType *tForReg = NULL);

    /** Decodes the ModR/M byte to a memory reference expression. See makeModrmNormal(). */
    SgAsmMemoryReferenceExpression *decodeModrmMemory();

    /** If ModR/M is a memory reference, fill in its type; otherwise, make a register with the appropriate mode and put it
     *  into the modrm data member. */
    void fillInModRM(RegisterMode rmMode, SgAsmType *t);

    /** Builds the register or memory reference expression for the ModR/M byte. See getModRegRM(). */
    SgAsmExpression *makeModrmNormal(RegisterMode, SgAsmType *mrType);

    /** Builds the register reference expression for the ModR/M byte. See getModRegRM(). The @p mrType is only used for vector
     *  registers. */
    SgAsmx86RegisterReferenceExpression *makeModrmRegister(RegisterMode, SgAsmType* mrType=NULL);

    /** Throw an exceptions if the instruction requires the "Mod" part of the ModR/M byte to have the value 3. */
    void requireMemory() const {
        if (!modregrmByteSet)
            throw ExceptionX86("requires Mod/RM byte", this);
        if (modeField == 3)
            throw ExceptionX86("requires memory", this);
    }



    /*========================================================================================================================
     * Methods that construct an SgAsmExpression for an immediate operand.
     *========================================================================================================================*/
private:

    SgAsmExpression *getImmByte();
    SgAsmExpression *getImmWord();
    SgAsmExpression* getImmDWord();
    SgAsmExpression* getImmQWord();
    SgAsmExpression *getImmForAddr();
    SgAsmExpression *getImmIv();
    SgAsmExpression *getImmJz();
    SgAsmExpression *getImmByteAsIv();
    SgAsmExpression *getImmIzAsIv();
    SgAsmExpression *getImmJb();




    /*========================================================================================================================
     * Main disassembly functions, each generally containing a huge "switch" statement based on one of the opcode bytes.
     *========================================================================================================================*/
private:

    /** Disassembles an instruction. This is the workhorse: it reads and decodes bytes of the instruction in a huge switch
     *  statement. */
    SgAsmx86Instruction *disassemble();

    /** Disassemble an instruction following the 0x0f prefix. */
    SgAsmx86Instruction *decodeOpcode0F();

    /** Disassemble SSE3 instructions. */
    SgAsmx86Instruction *decodeOpcode0F38();

    /** Disassembles an instruction with primary opcode 0xd8 */
    SgAsmx86Instruction *decodeX87InstructionD8();

    /** Disassembles an instruction with primary opcode 0xd9 */
    SgAsmx86Instruction *decodeX87InstructionD9();

    /** Disassembles an instruction with primary opcode 0xda */
    SgAsmx86Instruction *decodeX87InstructionDA();

    /** Disassembles an instruction with primary opcode 0xdb */
    SgAsmx86Instruction *decodeX87InstructionDB();

    /** Disassembles an instruction with primary opcode 0xdc */
    SgAsmx86Instruction *decodeX87InstructionDC();

    /** Disassembles an instruction with primary opcode 0xdd */
    SgAsmx86Instruction *decodeX87InstructionDD();

    /** Disassembles an instruction with primary opcode 0xde */
    SgAsmx86Instruction *decodeX87InstructionDE();

    /** Disassembles an instruction with primary opcode 0xdf */
    SgAsmx86Instruction *decodeX87InstructionDF();

    /** Disassembles ADD, OR, ADC, SBB, AND, SUB, XOR, CMP. */
    SgAsmx86Instruction *decodeGroup1(SgAsmExpression *imm);

    /** Disassembles POP */
    SgAsmx86Instruction *decodeGroup1a();

    /** Disassembles ROL, ROR, RCL, RCR, SHL, SHR, SHL, SAR */
    SgAsmx86Instruction *decodeGroup2(SgAsmExpression *count);

    /** Disassembles TEST, NOT, NEG, MUL, IMUL, DIV, IDIV */
    SgAsmx86Instruction *decodeGroup3(SgAsmExpression *immMaybe);

    /** Disassembles INC, DEC */
    SgAsmx86Instruction *decodeGroup4();

    /** Disassembles INC, DEC, CALL, FARCALL, JMP, FARJMP, PUSH */
    SgAsmx86Instruction *decodeGroup5();

    /** Disassembles SLDT, STR, LLDT, LTR, VERR, VERW */
    SgAsmx86Instruction *decodeGroup6();

    /** Disassembles VMCALL, VMLAUNCH, VMRESUME, VMXOFF, SGDT, MONITOR, MWAIT, SIDT, SGDT, XGETBV, XSETBV, LGDT, VMRUN,
     *  VMMCALL, VMLOAD, VMSAVE, STGI, CLGI, SKINIT, INVLPGA, LIDT, SMSW, LMSW, SWAPGS, RDTSCP, INVLPG */
    SgAsmx86Instruction *decodeGroup7();

    /** Disassembles BT, BTS, BTR, BTC */
    SgAsmx86Instruction *decodeGroup8(SgAsmExpression *imm);

    /** Disassembles MOV */
    SgAsmx86Instruction *decodeGroup11(SgAsmExpression *imm);

    /** Disassembles FXSAVE, FXRSTOR, LDMXCSR, STMXCSR, XSAVE, LFENCE, XRSTOR, MFENCE, SFENCE, CLFLUSH */
    SgAsmx86Instruction *decodeGroup15();

    /** Disassembles PREFETCHNTA, PREFETCH0, PREFETCH1, PREFETCH2, PREFETCH */
    SgAsmx86Instruction *decodeGroup16();

    /** Disassembles PREFETCH, PREFETCHW */
    SgAsmx86Instruction *decodeGroupP();



    /*========================================================================================================================
     * Data members and their initialization.
     *========================================================================================================================*/
private:

    /** Initialize instances of this class. Called by constructor. */
    void init(size_t wordsize);

    /** Resets disassembler state to beginning of an instruction for assembly. */
    void startInstruction(SgAsmx86Instruction *insn) {
        startInstruction(insn->get_address(), NULL, 0);
        insnSize = insn->get_baseSize();
        lock = insn->get_lockPrefix();
        branchPrediction = insn->get_branchPrediction();
        branchPredictionEnabled = branchPrediction != x86_branch_prediction_none;
        segOverride = insn->get_segmentOverride();
    }
    
    /** Resets disassembler state to beginning of an instruction for disassembly. */
    void startInstruction(rose_addr_t start_va, const uint8_t *buf, size_t bufsz) {
        ip = start_va;
        insnbuf = SgUnsignedCharList(buf, buf+bufsz);
        insnbufat = 0;

        /* Prefix flags */
        segOverride = x86_segreg_none;
        branchPrediction = x86_branch_prediction_none;
        branchPredictionEnabled = false;
        rexPresent = rexW = rexR = rexX = rexB = false;
        sizeMustBe64Bit = false;
        operandSizeOverride = false;
        addressSizeOverride = false;
        lock = false;
        repeatPrefix = x86_repeat_none;
        modregrmByteSet = false;
        modregrmByte = modeField = regField = rmField = 0; /*arbitrary since modregrmByteSet is false*/
        modrm = reg = NULL;
        isUnconditionalJump = false;
    }

    /* Per-disassembler settings; see init() */
    X86InstructionSize insnSize;                /**< Default size of instructions, based on architecture; see init() */

    /* Per-instruction settings; see startInstruction() */
    uint64_t ip;                                /**< Virtual address for start of instruction */
    SgUnsignedCharList insnbuf;                 /**< Buffer containing bytes of instruction */
    size_t insnbufat;                           /**< Index of next byte to be read from or write to insnbuf */

    /* Temporary flags set by the instruction; initialized by startInstruction() */
    X86SegmentRegister segOverride;             /**< Set to other than x86_segreg_none by 0x26,0x2e,0x36,0x3e,0x64,0x65 prefixes */
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
};

#endif
