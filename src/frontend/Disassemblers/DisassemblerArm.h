/* Disassembly specific to the ARM architecture. */

#ifndef ROSE_DISASSEMBLER_ARM_H
#define ROSE_DISASSEMBLER_ARM_H

/** Disassembler for the ARM architecture. Most of the useful disassembly methods can be found in the superclass. */
class DisassemblerArm: public Disassembler {
public:
    DisassemblerArm()
        : decodeUnconditionalInstructions(true), ip(0), insn(0), cond(arm_cond_unknown) {
        init();
    }

    DisassemblerArm(const DisassemblerArm& other)
        : Disassembler(other), decodeUnconditionalInstructions(other.decodeUnconditionalInstructions), 
          ip(other.ip), insn(other.insn), cond(other.cond) {
    }

    virtual ~DisassemblerArm() {}

    virtual Disassembler *clone() const {
        return new DisassemblerArm(*this);
    }

    /** See Disassembler::can_disassemble */
    virtual bool can_disassemble(SgAsmGenericHeader*) const;

    /** See Disassembler::disassembleOne */
    virtual SgAsmInstruction *disassembleOne(const MemoryMap *map, rose_addr_t start_va, AddressSet *successors=NULL);

    /** See Disassembler::assembleOne */
    virtual void assembleOne(SgAsmInstruction*, SgUnsignedCharList&) {abort();}

    /** See Disassembler::can_disassemble */
    virtual SgAsmInstruction *make_unknown_instruction(const Exception&);

private:
    /** Same as Disassembler::Exception except with a different constructor for ease of use in DisassemblerArm.  This
     *  constructor should be used when an exception occurs during disassembly of an instruction; it is not suitable for
     *  errors that occur before or after (use superclass constructors for that case). */
    class ExceptionArm: public Exception {
    public:
        ExceptionArm(const std::string &mesg, const DisassemblerArm *d, size_t bit=0)
            : Exception(mesg, d->ip) {
            /* Convert four-byte instruction to little-endian buffer. FIXME: assumes little-endian ARM system */
            bytes.push_back(d->insn & 0xff);
            bytes.push_back((d->insn>>8) & 0xff);
            bytes.push_back((d->insn>>16) & 0xff);
            bytes.push_back((d->insn>>24) & 0xff);
            this->bit = bit;
        }
    };

    static SgAsmArmInstruction *makeInstructionWithoutOperands(uint32_t address, const std::string& mnemonic, int condPos,
                                                               ArmInstructionKind kind, ArmInstructionCondition cond,
                                                               uint32_t insn);
    SgAsmArmRegisterReferenceExpression *makeRegister(uint8_t reg) const;
    SgAsmArmRegisterReferenceExpression *makePsrFields(bool useSPSR, uint8_t fields) const;
    SgAsmArmRegisterReferenceExpression *makePsr(bool useSPSR) const;

    SgAsmExpression *makeRotatedImmediate() const;
    SgAsmExpression *makeShifterField() const; /**< Decode last 12 bits and bit 25 (I) */
    SgAsmArmInstruction *makeDataProcInstruction(uint8_t opcode, bool s, SgAsmExpression* rn, SgAsmExpression* rd,
                                                 SgAsmExpression* rhsOperand);
    SgAsmDoubleWordValueExpression *makeSplit8bitOffset() const;
    SgAsmDoubleWordValueExpression *makeBranchTarget() const;
    SgAsmExpression *decodeMemoryAddress(SgAsmExpression* rn) const;
    SgAsmArmInstruction *decodeMediaInstruction() const;
    SgAsmArmInstruction *decodeMultiplyInstruction() const;
    SgAsmArmInstruction *decodeExtraLoadStores() const;
    SgAsmArmInstruction *decodeMiscInstruction() const;
    SgAsmArmInstruction *disassemble();
    
    /** Initialize instances of this class. Called by constructor. */
    void init();

    /** Resets disassembler state to beginning of an instruction. */
    void startInstruction(rose_addr_t start_va, uint32_t c) {
        ip = start_va;
        insn = c;
        cond = arm_cond_unknown;
    }

    /* Per-instruction data members (mostly set by startInstruction()) */
    bool decodeUnconditionalInstructions;       /**< set by init() */
    uint32_t ip;                                /**< instruction pointer */
    uint32_t insn;                              /**< 4-byte instruction word */
    ArmInstructionCondition cond;
};

#endif
