/* Disassembly specific to Motorola architectures */
#ifndef ROSE_DisassemblerM68k_H
#define ROSE_DisassemblerM68k_H

#include "Disassembler.h"
#include "InstructionEnumsM68k.h"
#include "BitPattern.h"

/** Disassembler for Motorola M68k-based instruction set architectures. */
class DisassemblerM68k: public Disassembler {
public:
    /** Constructor for a specific family.
     *
     *  The @p family argument selectively activates certain features of the generic m68k disassembler.  For instance, to get a
     *  disassembler specific to the FreeScale ColdFire series using "ISA_B", invoke as:
     *
     * @code
     *  Disassembler *disassembler = new DisassemblerM68k(m68k_freescale_isab);
     * @endcode */
    explicit DisassemblerM68k(M68kFamily family)
        : family(family), map(NULL), insn_va(0), niwords(0), niwords_used(0) {
        init();
    }
    virtual DisassemblerM68k *clone() const /*override*/ { return new DisassemblerM68k(*this); }
    virtual bool can_disassemble(SgAsmGenericHeader*) const /*override*/;
    virtual SgAsmInstruction *disassembleOne(const MemoryMap*, rose_addr_t start_va, AddressSet *successors=NULL) /*override*/;
    virtual SgAsmInstruction *make_unknown_instruction(const Disassembler::Exception&) /*override*/;

    typedef std::pair<SgAsmExpression*, SgAsmExpression*> ExpressionPair;

    /** Interface for disassembling a single instruction.  Each instruction (or in some cases groups of closely related
     *  instructions) will define a subclass whose operator() unparses a single instruction and returns a
     *  SgAsmM68kInstruction. These functors are allocated and inserted into a list. When an instruction is to be
     *  disassembled, the list is scanned to find the first entry that matches, and then its operator() is invoked.  An entry
     *  matches if the instruction bits to be disassembled match any of the BitPattern objects.
     *
     *  An instruction decoder is enabled if the disassembler's family (see DisassemblerM68k constructors) bit-wise ANDed with
     *  the decoder family (see DisassemblerM68k::M68k constructor) is non-zero. */
    class M68k {
    public:
        M68k(const std::string &name, unsigned family, const BitPattern<uint16_t> &pattern)
            : name(name), family(family), pattern(pattern) {}
        virtual ~M68k() {}
        std::string name;                               // for debugging; same as class name but without the "M68k_" prefix
        unsigned family;                                // bitmask of M68kFamily bits
        BitPattern<uint16_t> pattern;                   // bits that match
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
        niwords_used = 0;
    }

    /** Return the Nth instruction word. */
    uint16_t instruction_word(size_t n);

    /** Create an integer data type of specified width. */
    SgAsmType *makeIntegerType(size_t nbits);

    /** Create a data register reference expression. */
    SgAsmM68kRegisterReferenceExpression *makeDataRegister(unsigned regnum, size_t nbits, size_t bit_offset=0);

    /** Create an address register reference expression. */
    SgAsmM68kRegisterReferenceExpression *makeAddressRegister(unsigned regnum, size_t nbits, size_t bit_offset=0);

    /** Make a memory reference expression using an address register in pre-decrement mode. The @p nbits are the size of the
     *  memory reference; all 32-bits of the address register are accessed. */
    SgAsmMemoryReferenceExpression *makeAddressRegisterPreDecrement(unsigned regnum, size_t nbits);

    /** Make a memory reference expression using an address register in post-increment mode. The @p nbits are the size of the
     *  memory reference; all 32-bits of the address register are accessed. */
    SgAsmMemoryReferenceExpression *makeAddressRegisterPostIncrement(unsigned regnum, size_t nbits);

    /** Create either a data or address register reference expression. When @p regnum is zero through seven a data register is
     *  created; when @p regnum is eight through 15 an address register is created. */
    SgAsmM68kRegisterReferenceExpression *makeDataAddressRegister(unsigned regnum, size_t nbits, size_t bit_offset=0);

    /** Create a list of data and/or address registers.
     *
     *  The bit mask indicates the registers. Starting at the least significant bit, the register are either:
     *  D0, D1, ... D7, A0, A1, ... A7 if @p reverse is false, or A7, A6, ... A0, D7, D6, ... D0 if @p reverse is true.  The
     *  returned list has the registers in order starting at the least significant bit. */
    SgAsmRegisterNames *makeRegistersFromMask(unsigned mask, size_t nbits, bool reverse=false);

    /** Create a reference to the status register. */
    SgAsmM68kRegisterReferenceExpression *makeStatusRegister();

    /** Create a reference to the condition code register. This is the low-order 8 bits of the status register. */
    SgAsmM68kRegisterReferenceExpression *makeConditionCodeRegister();

    /** Create a reference to the program counter register. */
    SgAsmM68kRegisterReferenceExpression *makeProgramCounter();

    /** Create a MAC register reference expression. */
    SgAsmM68kRegisterReferenceExpression *makeMacRegister(M68kMacRegister);

    /** Create an EMAC register reference expression. */
    SgAsmM68kRegisterReferenceExpression *makeEmacRegister(M68kEmacRegister);

    /** Generic ways to make a register. */
    SgAsmM68kRegisterReferenceExpression *makeRegister(const RegisterDescriptor&);

    /** Create an integer expression from a specified value. */
    SgAsmIntegerValueExpression *makeImmediateValue(size_t nbits, unsigned value);

    /** Create an integer expression from extension words. */
    SgAsmIntegerValueExpression *makeImmediateExtension(size_t nbits, size_t ext_word_idx);

    /** Create an expression for m68k "<ea>x" or "<ea>y". The @p modreg is a six-bit value whose high-order three bits are the
     * addressing mode and whose low-order three bits are (usually) a register number. The return value has a type of the
     * specified size measured in bits. The @p ext_offset indicates how many instruction extension words have already been
     * consumed.
     *
     * @{ */
    SgAsmExpression *makeEffectiveAddress(unsigned modreg, size_t nbits, size_t ext_offset);
    SgAsmExpression *makeEffectiveAddress(unsigned mode, unsigned reg, size_t nbits, size_t ext_offset);
    /** @} */

    /** Create an offset width pair from an extension word.  The extension word contains an offset and width expression each of
     *  which is either a 5-bit unsigned integer or a data register number. This is used by various bit field instructions. */
    ExpressionPair makeOffsetWidthPair(unsigned extension_word);

    /** Build an instruction. */
    SgAsmM68kInstruction *makeInstruction(M68kInstructionKind, const std::string &mnemonic,
                                          SgAsmExpression *arg0=NULL, SgAsmExpression *arg1=NULL, SgAsmExpression *arg2=NULL,
                                          SgAsmExpression *arg3=NULL, SgAsmExpression *arg4=NULL, SgAsmExpression *arg5=NULL);

    /** Return the address of the instruction we are disassembling. */
    rose_addr_t get_insn_va() const { return insn_va; }

private:
    void init();
    M68kFamily  family;                         /**< Specific family being disassembled. */
    const MemoryMap *map;                       /**< Map from which to read instruction words. */
    rose_addr_t insn_va;                        /**< Address of instruction. */
    uint16_t    iwords[11];                     /**< Instruction words. */
    size_t      niwords;                        /**< Number of instruction words read. */
    size_t      niwords_used;                   /**< High water number of instruction words used by instruction_word(). */
    typedef std::list<M68k*> IdisTable;
    IdisTable idis_table;                       /**< Instruction specific disassemblers. */
};

#endif
