/* Disassembly specific to the PowerPC architecture. */

#ifndef ROSE_DISASSEMBLER_POWERPC_H
#define ROSE_DISASSEMBLER_POWERPC_H

/* These namespace declarations copied from the old assemblers.h and should not be used anymore except by the new
 * DisassemblerPowerpc class.  They will eventually be incorporated into that class as private methods.  The only one that's
 * probably used much is PowerpcDisassembler::disassemble(), which has been replaced by DisassemblerPowerpc::disassembleOne().
 * [RPM 2008-06-10] */
namespace PowerpcDisassembler {
    struct Parameters {
        Parameters(uint64_t ip = 0): ip(ip) {}
        uint64_t ip;
        bool decodeUnconditionalInstructions;
    };

    // Exceptions
    struct OverflowOfInstructionVector {};
    struct BadInstruction {};

    SgAsmPowerpcInstruction *disassemble(const Parameters& p, const uint8_t* const insn, const uint64_t insnSize,
                                         size_t positionInVector, std::set<uint64_t>* knownSuccessorsReturn = 0);
    SgAsmPowerpcRegisterReferenceExpression *makeRegister(PowerpcRegisterClass reg_class, int reg_number,
                                                          PowerpcConditionRegisterAccessGranularity reg_grainularity);
};


/** Disassembler for the PowerPC architecture.  This class is usually instantiated indirectly through Disassembler::create().
 *  Most of the useful disassembly methods can be found in the superclass. */
class DisassemblerPowerpc: public Disassembler {
public:
    DisassemblerPowerpc() {} /* Such an object can only be used as a factory for Disassembler::register_subclass() */
    DisassemblerPowerpc(SgAsmGenericHeader *fhdr) {init(fhdr);}
    virtual ~DisassemblerPowerpc() {}
    virtual Disassembler *can_disassemble(SgAsmGenericHeader*) const;
    virtual SgAsmInstruction *disassembleOne(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va,
                                             AddressSet *successors=NULL);
    virtual SgAsmInstruction *make_unknown_instruction(const Exception&);
private:
    void init(SgAsmGenericHeader*);             /* initialize instances */
    PowerpcDisassembler::Parameters params;
};

#endif
