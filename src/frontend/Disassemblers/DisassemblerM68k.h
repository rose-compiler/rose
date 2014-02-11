/* Disassembly specific to Motorola architectures */
#ifndef ROSE_DisassemblerM68k_H
#define ROSE_DisassemblerM68k_H

#include "Disassembler.h"
#include "InstructionEnumsM68k.h"
#include "BitPattern.h"

class DisassemblerM68k: public Disassembler {
public:
    DisassemblerM68k() { init(); }
    virtual DisassemblerM68k *clone() const /*override*/ { return new DisassemblerM68k(*this); }
    virtual bool can_disassemble(SgAsmGenericHeader*) const /*override*/;
    virtual SgAsmInstruction *disassembleOne(const MemoryMap*, rose_addr_t start_va, AddressSet *successors=NULL) /*override*/;
    virtual SgAsmInstruction *make_unknown_instruction(const Disassembler::Exception&) /*override*/;

    /** Interface for disassembling a single instruction.  Each instruction (or in some cases groups of closely related
     *  instructions) will define a subclass whose operator() unparses a single instruction and returns a
     *  SgAsmM68kInstruction. These functors are allocated and inserted into a list. When an instruction is to be
     *  disassembled, the list is scanned to find the first entry that matches, and then its operator() is invoked.  An entry
     *  matches if the instruction bits to be disassembled match any of the BitPattern objects. */
    class M68k {
    public:
        M68k(const BitPattern<uint16_t> &pattern): pattern(pattern) {}
        virtual ~M68k() {}
        BitPattern<uint16_t> pattern;
        typedef DisassemblerM68k D;
        virtual SgAsmM68kInstruction *operator()(D *d, unsigned w0) = 0;
    };

    /** Find an instruction-specific disassembler.  Using the specified instruction bits, search for and return an
     *  instruction-specific disassembler.  Returns null if no appropriate disassembler can be found.  Instruction-specific
     *  disassemblers know how to disassemble specific instruction types (or groups of closely related instructions). */
    M68k *find_idis(uint16_t *insn_bytes, size_t nbytes) const;

    /** Insert an instruction-specific disassembler. The table must not already contain an entry that has the same @p mask and
     *  @p match values. The pointers are managed by the caller and must not be deleted while they are in the table. */
    void insert_idis(M68k*);

    /** Called by disassembleOne() to initialize the disassembler state for the next instruction. */
    void start_instruction(const MemoryMap *map, rose_addr_t start_va) {
        this->map = map;
        insn_va = start_va;
        niwords = 0;
        memset(iwords, 0, sizeof iwords);
    }

    /** Return the Nth instruction word. */
    uint16_t instruction_word(size_t n);

    /** Create a data register reference expression. */
    SgAsmM68kRegisterReferenceExpression *makeDataRegister(unsigned regnum, size_t nbits, size_t bit_offset=0);

    /** Create an address register reference expression. */
    SgAsmM68kRegisterReferenceExpression *makeAddressRegister(unsigned regnum, size_t nbits, size_t bit_offset=0);

    /** Create either a data or address register reference expression. When @p regnum is zero through seven a data register is
     *  created; when @p regnum is eight through 15 an address register is created. */
    SgAsmM68kRegisterReferenceExpression *makeDataAddressRegister(unsigned regnum, size_t nbits, size_t bit_offset=0);

    /** Create a reference to the condition code register. */
    SgAsmM68kRegisterReferenceExpression *makeConditionCodeRegister();

    /** Create a reference to the program counter register. */
    SgAsmM68kRegisterReferenceExpression *makeProgramCounter();

    /** Create a MAC register reference expression. */
    SgAsmM68kRegisterReferenceExpression *makeMacRegister(M68kMacRegister);

    /** Create an EMAC register reference expression. */
    SgAsmM68kRegisterReferenceExpression *makeEmacRegister(M68kEmacRegister);

    /** Generic ways to make a register. */
    SgAsmM68kRegisterReferenceExpression *makeRegister(const RegisterDescriptor&);

    /** Create an immediate value.
     * @{ */
    SgAsmIntegerValueExpression *makeImmediate1(unsigned value);
    SgAsmIntegerValueExpression *makeImmediate8(unsigned value);
    SgAsmIntegerValueExpression *makeImmediate16(unsigned value);
    SgAsmIntegerValueExpression *makeImmediate32(unsigned value, unsigned hi_bits=0);
    /** @} */

    /** Create an expression for m68k "<ea>x" or "<ea>y". The @p modreg is a six-bit value whose high-order three bits are the
     * addressing mode and whose low-order three bits are (usually) a register number. The return value has a type of the
     * specified size measured in bits. The @p ext_offset indicates how many instruction extension words have already been
     * consumed. */
    SgAsmExpression *makeEffectiveAddress(unsigned modreg, size_t nbits, size_t ext_offset);

    /** Determines the effective address mode for a 6-bit modreg value. The @p nbits is used only to distinguish between
     *  absolute short addressing mode (m68k_eam_absw) and absolute long addressing mode (m68k_eam_absl). */
    M68kEffectiveAddressMode effectiveAddressMode(unsigned modreg, size_t nbits);

    /** Number of instruction extension words needed for the specified effective address mode.  The @p nbits is used only to
     *  distinguish between absolute short addressing mode (m68k_eam_absw) and absolute long addressing mode
     *  (m68k_eam_absl). */
    size_t nExtensionWords(M68kEffectiveAddressMode, size_t nbits);

    /** Build an instruction. */
    SgAsmM68kInstruction *makeInstruction(M68kInstructionKind, const std::string &mnemonic,
                                          SgAsmExpression *arg0=NULL, SgAsmExpression *arg1=NULL, SgAsmExpression *arg2=NULL,
                                          SgAsmExpression *arg3=NULL, SgAsmExpression *arg4=NULL, SgAsmExpression *arg5=NULL);

    /** Return the address of the instruction we are disassembling. */
    rose_addr_t get_insn_va() const { return insn_va; }

    /** Convert a size in bits to a mnemonic suffix letter. */
    char sizeToLetter(size_t nbits);

private:
    void init();
    const MemoryMap *map;                       /**< Map from which to read instruction words. */
    rose_addr_t insn_va;                        /**< Address of instruction. */
    uint16_t    iwords[3];                      /**< Instruction words. */
    size_t      niwords;                        /**< Number of instruction words read. */
    size_t      niwords_used;                   /**< High water number of instruction words used by instruction_word(). */
    typedef std::list<M68k*> IdisTable;
    IdisTable idis_table;                       /**< Instruction specific disassemblers. */
};

#endif
