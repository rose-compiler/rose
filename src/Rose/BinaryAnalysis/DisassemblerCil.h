/* Disassembly specific to Motorola architectures */
#ifndef ROSE_BinaryAnalysis_DisassemblerCil_H
#define ROSE_BinaryAnalysis_DisassemblerCil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler.h>

#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>
#include "BitPattern.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Disassembler for CIL instruction set architectures. */
class DisassemblerCil: public Disassembler {
public:
    // State mutated during the call to disassembleOne. Used internally.
    struct State: boost::noncopyable { // noncopyable is so we don't accidentally pass it by value
        MemoryMap::Ptr map;                         /**< Map from which to read instruction words. */
        rose_addr_t insn_va;                        /**< Address of instruction. */
        uint16_t    iwords[11];                     /**< Instruction words. */
        size_t      niwords;                        /**< Number of instruction words read. */
        size_t      niwords_used;                   /**< High water number of instruction words used by instructionWord(). */

        State()
            : insn_va(0), niwords(0), niwords_used(0) {}
    };

public:
    /** Interface for disassembling a single instruction.  Each instruction (or in some cases groups of closely related
     *  instructions) will define a subclass whose operator() unparses a single instruction and returns a
     *  SgAsmCilInstruction. These functors are allocated and inserted into a list. When an instruction is to be
     *  disassembled, the list is scanned to find the first entry that matches, and then its operator() is invoked.  An entry
     *  matches if the instruction bits to be disassembled match any of the BitPattern objects.
     *
     *  An instruction decoder is enabled if the disassembler's family (see DisassemblerCil constructors) bit-wise ANDed with
     *  the decoder family (see DisassemblerCil::Cil constructor) is non-zero. */
    class Cil {
    public:
        Cil(const std::string &name, unsigned family, const BitPattern<uint16_t> &pattern)
            : name(name), family(family), pattern(pattern) {}
        virtual ~Cil() {}
        std::string name;                               // for debugging; same as class name but without the "Cil_" prefix
        unsigned family;                                // bitmask of CilFamily bits
        BitPattern<uint16_t> pattern;                   // bits that match
        typedef DisassemblerCil D;
        virtual SgAsmCilInstruction *operator()(State&, const D *d, unsigned w0) = 0;
    };

private:
    CilFamily  family;                         /**< Specific family being disassembled. */

    // The instruction disassembly table is an array indexed by the high-order nybble of the first 16-bit word of the
    // instruction's pattern, the so-called "operator" bits. Since most instruction disassembler have invariant operator
    // bits, we can divide the table into 16 entries for these invariant bits, and another entry (index 16) for the cases
    // with a variable operator byte.  Each of these 17 buckets is an unordered list of instruction disassemblers whose
    // patterns we attempt to match one at a time (the insertion function checks that there are no ambiguities).
    typedef std::list<Cil*> IdisList;
    typedef std::vector<IdisList> IdisTable;
    IdisTable idis_table;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize_common(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Disassembler);
        s & BOOST_SERIALIZATION_NVP(family);
        //s & idis_table; -- not saved
    }

    template<class S>
    void save(S &s, const unsigned version) const {
        serialize_common(s, version);
    }

    template<class S>
    void load(S &s, const unsigned version) {
        serialize_common(s, version);
        init();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

public:
// protected:
    // undocumented constructor for serialization. The init() will be called by the serialization.
    DisassemblerCil()
        : family(Cil_family) {}
        
public:
#if 0
    /** Constructor for a specific family.
     *
     *  The @p family argument selectively activates certain features of the generic Cil disassembler.  For instance, to get a
     *  disassembler specific to the FreeScale ColdFire series using "ISA_B", invoke as:
     *
     * @code
     *  Disassembler *disassembler = new DisassemblerCil(Cil_freescale_isab);
     * @endcode */
    explicit DisassemblerCil(CilFamily family)
        : family(family) {
        init();
    }
#endif

    virtual DisassemblerCil *clone() const override { return new DisassemblerCil(*this); }
    virtual bool canDisassemble(SgAsmGenericHeader*) const override;
    virtual SgAsmInstruction *disassembleOne(const MemoryMap::Ptr&, rose_addr_t start_va,
                                             AddressSet *successors=NULL) override;
    virtual SgAsmInstruction *makeUnknownInstruction(const Disassembler::Exception&) override;

 // DQ (11/3/2021): Simpler way to build unknown instructions.
    virtual SgAsmCilInstruction* makeUnknownInstruction(rose_addr_t address, uint8_t opt_code);

    virtual Unparser::BasePtr unparser() const override;

    typedef std::pair<SgAsmExpression*, SgAsmExpression*> ExpressionPair;

    /** Find an instruction-specific disassembler.  Using the specified instruction bits, search for and return an
     *  instruction-specific disassembler.  Returns null if no appropriate disassembler can be found.  Instruction-specific
     *  disassemblers know how to disassemble specific instruction types (or groups of closely related instructions). */
    Cil *find_idis(uint16_t *insn_bytes, size_t nbytes) const;

    /** Insert an instruction-specific disassembler. The table must not already contain an entry that has the same @p mask and
     *  @p match values. The pointers are managed by the caller and must not be deleted while they are in the table. */
    void insert_idis(Cil*);

    /** Called by disassembleOne() to initialize the disassembler state for the next instruction. */
    void start_instruction(State &state, const MemoryMap::Ptr &map, rose_addr_t start_va) const{
        state.map = map;
        state.insn_va = start_va;
        state.niwords = 0;
        memset(state.iwords, 0, sizeof state.iwords);
        state.niwords_used = 0;
    }

    /** Return the Nth instruction word. */
    uint16_t instructionWord(State&, size_t n) const;

    /** Returns number of instruction words referenced so far in the current instruction. */
    size_t extensionWordsUsed(State&) const;

    /** Create a ROSE data type for Cil data format. */
    SgAsmType *makeType(State&, CilDataFormat) const;

    /** Create a data register reference expression. */
    SgAsmRegisterReferenceExpression *makeDataRegister(State&, unsigned regnum, CilDataFormat, size_t bit_offset=0) const;

    /** Create an address register reference expression. */
    SgAsmRegisterReferenceExpression *makeAddressRegister(State&, unsigned regnum, CilDataFormat, size_t bit_offset=0) const;

    /** Make a memory reference expression using an address register in pre-decrement mode. The @p fmt is the format of the
     *  memory reference; all 32-bits of the address register are accessed. */
    SgAsmMemoryReferenceExpression *makeAddressRegisterPreDecrement(State&, unsigned regnum, CilDataFormat fmt) const;

    /** Make a memory reference expression using an address register in post-increment mode. The @p fmt is the format of the
     *  memory reference; all 32-bits of the address register are accessed. */
    SgAsmMemoryReferenceExpression *makeAddressRegisterPostIncrement(State&, unsigned regnum, CilDataFormat fmt) const;

    /** Create either a data or address register reference expression. When @p regnum is zero through seven a data register is
     *  created; when @p regnum is eight through 15 an address register is created. */
    SgAsmRegisterReferenceExpression *makeDataAddressRegister(State&, unsigned regnum, CilDataFormat fmt,
                                                              size_t bit_offset=0) const;

    /** Create a list of data and/or address registers.
     *
     *  The bit mask indicates the registers. Starting at the least significant bit, the register are either:
     *  D0, D1, ... D7, A0, A1, ... A7 if @p reverse is false, or A7, A6, ... A0, D7, D6, ... D0 if @p reverse is true.  The
     *  returned list has the registers in order starting at the least significant bit. */
    SgAsmRegisterNames *makeRegistersFromMask(State&, unsigned mask, CilDataFormat fmt, bool reverse=false) const;

    /** Create a list of floating-point data registers.
     *
     *  The bit mask indicates the registers. Starting at the least significant bit, the registers are either:
     *  FP0 through FP7 if @p reverse is false, or FP7 through FP0 if @p reverse is true.  The returned list has the registers
     *  in order starting at the least significant bit. */
    SgAsmRegisterNames *makeFPRegistersFromMask(State&, unsigned mask, CilDataFormat fmt, bool reverse=false) const;

    /** Create a reference to the status register. */
    SgAsmRegisterReferenceExpression *makeStatusRegister(State&) const;

    /** Create a reference to the condition code register. This is the low-order 8 bits of the status register. */
    SgAsmRegisterReferenceExpression *makeConditionCodeRegister(State&) const;

    /** Create control register for ColdFire cpu. */
    SgAsmRegisterReferenceExpression* makeColdFireControlRegister(State&, unsigned regnum) const;

    /** Create a reference to the program counter register. */
    SgAsmRegisterReferenceExpression *makeProgramCounter(State&) const;

    /** Create a MAC register reference expression. */
    SgAsmRegisterReferenceExpression *makeMacRegister(State&, CilMacRegister) const;

    /** Create a MAC accumulator register. These are ACC0 through ACC3, 32-bit integers. */
    SgAsmRegisterReferenceExpression *makeMacAccumulatorRegister(State&, unsigned accumIndex) const;

    /** Create a floating point register.  Floating point registers are different sizes on different platforms. For example,
     * the M68040 has 80-bit registers that can store 96-bit extended-precision real values (16-bits of which are zero), but
     * the follow on FreeScale ColdFire processors have only 64-bit registers that hold double-precision real values. */
    SgAsmRegisterReferenceExpression *makeFPRegister(State&, unsigned regnum) const;

    /** Generic ways to make a register. */
    SgAsmRegisterReferenceExpression *makeRegister(RegisterDescriptor) const;

    /** Create an integer expression from a specified value. */
    SgAsmIntegerValueExpression *makeImmediateValue(State&, CilDataFormat fmt, unsigned value) const;

    /** Create an integer expression from extension words. */
    SgAsmIntegerValueExpression *makeImmediateExtension(State&, CilDataFormat fmt, size_t ext_word_idx) const;

    /** Create an expression for Cil "<ea>x" or "<ea>y". The @p modreg is a six-bit value whose high-order three bits are the
     * addressing mode and whose low-order three bits are (usually) a register number. The return value has a type of the
     * specified data format. The @p ext_offset indicates how many instruction extension words have already been
     * consumed.
     *
     * @{ */
    SgAsmExpression *makeEffectiveAddress(State&, unsigned modreg, CilDataFormat fmt, size_t ext_offset) const;
    SgAsmExpression *makeEffectiveAddress(State&, unsigned mode, unsigned reg, CilDataFormat fmt, size_t ext_offset) const;
    /** @} */

    /** Converts a memory-reference expression to an address.  This is used for things like the JSR instruction that takes an
     *  effective address that's a memory reference, and converts it to just an address. It also rewrites PC-relative addresses
     *  since the PC is constant. */
    SgAsmExpression *makeAddress(State&, SgAsmExpression *expr) const;

    /** Create an offset width pair from an extension word.  The extension word contains an offset and width expression each of
     *  which is either a 5-bit unsigned integer or a data register number. This is used by various bit field instructions. */
    ExpressionPair makeOffsetWidthPair(State&, unsigned extension_word) const;

    /** Build an instruction. */
 // DQ (10/20/2021): We don't need the State &state function parameter for .Cil
 // SgAsmCilInstruction *makeInstruction(State&, CilInstructionKind, const std::string &mnemonic,
 //                                       SgAsmExpression *arg0=NULL, SgAsmExpression *arg1=NULL, SgAsmExpression *arg2=NULL,
 //                                       SgAsmExpression *arg3=NULL, SgAsmExpression *arg4=NULL, SgAsmExpression *arg5=NULL,
 //                                       SgAsmExpression *arg6=NULL) const;
    SgAsmCilInstruction *makeInstruction(rose_addr_t start_va, CilInstructionKind, const std::string &mnemonic,
                                          SgAsmExpression *arg0=NULL, SgAsmExpression *arg1=NULL, SgAsmExpression *arg2=NULL,
                                          SgAsmExpression *arg3=NULL, SgAsmExpression *arg4=NULL, SgAsmExpression *arg5=NULL,
                                          SgAsmExpression *arg6=NULL, SgAsmExpression *arg7=NULL) const;

    /** Returns ISA family specified in constructor. */
    CilFamily get_family() const { return family; }

private:
    void init();
};

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::DisassemblerCil);
#endif

#endif
#endif
