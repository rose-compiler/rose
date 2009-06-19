/* Disassembly specific to the x86 architecture. */

#ifndef ROSE_DISASSEMBLER_X86_H
#define ROSE_DISASSEMBLER_X86_H

/* These namespace declarations copied from the old assemblers.h and should not be used anymore except by the new
 * DisassemblerX86 class.  They will eventually be incorporated into that class as private methods.  The only one that's
 * probably used much is X86Disassembler::disassemble(), which has been replaced by DisassemblerX86::disassembleOne().
 * [RPM 2008-06-08] */
namespace X86Disassembler {
    struct Parameters {
        uint64_t ip;
        X86InstructionSize insnSize;
        Parameters(uint64_t ip = 0, X86InstructionSize insnSize = x86_insnsize_32): ip(ip), insnSize(insnSize) {}
    };

    // Exceptions
    struct OverflowOfInstructionVector {};
    struct BadInstruction {};

    SgAsmx86Instruction *disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize,
                                     size_t positionInVector, std::set<uint64_t>* knownSuccessorsReturn = 0);
    void disassembleFile(SgAsmFile* f);
    bool doesBBStartFunction(SgAsmBlock* bb, bool use64bit);

    inline SgAsmType* sizeToType(X86InstructionSize s) {
        switch (s) {
            case x86_insnsize_none: return NULL;
            case x86_insnsize_16: return SgAsmTypeWord::createType();
            case x86_insnsize_32: return SgAsmTypeDoubleWord::createType();
            case x86_insnsize_64: return SgAsmTypeQuadWord::createType();
            default: abort();;
        }
    }

    inline X86PositionInRegister sizeToPos(X86InstructionSize s) {
        switch (s) {
            case x86_insnsize_none: return x86_regpos_all;
            case x86_insnsize_16: return x86_regpos_word;
            case x86_insnsize_32: return x86_regpos_dword;
            case x86_insnsize_64: return x86_regpos_qword;
            default: abort();;
        }
    }
}


/** Disassembler for the x86 architecture.  This class is usually instantiated indirectly through Disassembler::create(). Most
 *  of the useful disassembly methods can be found in the superclass. */
class DisassemblerX86: public Disassembler {
public:
    DisassemblerX86() {} /* Such an object can only be used as a factory for Disassembler::register_subclass() */
    DisassemblerX86(SgAsmGenericHeader *fhdr) {init(fhdr);}
    virtual ~DisassemblerX86() {}
    virtual Disassembler *can_disassemble(SgAsmGenericHeader*) const;
    virtual SgAsmInstruction *disassembleOne(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va,
                                             AddressSet *successors=NULL);
private:
    void init(SgAsmGenericHeader*);             /* initialize instances */
    X86Disassembler::Parameters params;
};

#endif
