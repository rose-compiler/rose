/* Disassembly specific to the ARM architecture. */

#ifndef ROSE_DISASSEMBLER_ARM_H
#define ROSE_DISASSEMBLER_ARM_H

/* These namespace declarations copied from the old assemblers.h and should not be used anymore except by the new
 * DisassemblerArm class.  They will eventually be incorporated into that class as private methods.  The only one that's
 * probably used much is ArmDisassembler::disassemble(), which has been replaced by DisassemblerArm::disassembleOne().
 * [RPM 2008-06-10] */
namespace ArmDisassembler {
    struct Parameters {
        Parameters(uint32_t ip = 0, bool decodeUnconditionalInstructions = true)
            : ip(ip), decodeUnconditionalInstructions(decodeUnconditionalInstructions)
            {}
        uint32_t ip;
        bool decodeUnconditionalInstructions;
    };

    // Exceptions
    struct OverflowOfInstructionVector {};
    struct BadInstruction {};

    SgAsmArmInstruction *disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize,
                                     size_t positionInVector, std::set<uint64_t>* knownSuccessorsReturn = 0);
    SgAsmArmRegisterReferenceExpression* makeRegister(uint8_t reg);
};


/** Disassembler for the ARM architecture.  This class is usually instantiated indirectly through Disassembler::create().
 *  Most of the useful disassembly methods can be found in the superclass. */
class DisassemblerArm: public Disassembler {
public:
    DisassemblerArm() {} /* Such an object can only be used as a factory for Disassembler::register_subclass() */
    DisassemblerArm(SgAsmGenericHeader *fhdr) {init(fhdr);}
    virtual ~DisassemblerArm() {}
    virtual Disassembler *can_disassemble(SgAsmGenericHeader*) const;
    virtual SgAsmInstruction *disassembleOne(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va,
                                             AddressSet *successors=NULL);
    virtual SgAsmInstruction *make_unknown_instruction(const Exception&);
private:
    void init(SgAsmGenericHeader*);             /* initialize instances */
    ArmDisassembler::Parameters params;
};

#endif
