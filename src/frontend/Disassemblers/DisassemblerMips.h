/* Disassembly specific to the MIPS architecture */
#ifndef ROSE_DISASSEMBLER_MIPS_H
#define ROSE_DISASSEMBLER_MIPS_H

#include "Disassembler.h"
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "InstructionEnumsMips.h"
#include "SageBuilderAsm.h"

namespace Rose {
namespace BinaryAnalysis {

class DisassemblerMips: public Disassembler {
public:
    /** Create a MIPS disassembler.
     *
     *  MIPS executables can be big- or little-endian. */
    explicit DisassemblerMips(ByteOrder::Endianness sex = ByteOrder::ORDER_MSB) { init(sex); }

    virtual DisassemblerMips *clone() const ROSE_OVERRIDE { return new DisassemblerMips(*this); }
    virtual bool canDisassemble(SgAsmGenericHeader*) const ROSE_OVERRIDE;
    virtual SgAsmInstruction *disassembleOne(const MemoryMap::Ptr&, rose_addr_t start_va,
                                             AddressSet *successors=NULL) ROSE_OVERRIDE;
    virtual SgAsmInstruction *makeUnknownInstruction(const Disassembler::Exception&) ROSE_OVERRIDE;
    virtual Unparser::BasePtr unparser() const ROSE_OVERRIDE;

    /** Interface for disassembling a single instruction.  Each instruction (or in some cases groups of closely related
     *  instructions) will define a subclass whose operator() unparses a single instruction word and returns an
     *  SgAsmMipsInstruction. These functors are allocated and inserted into a list. When an instruction word is to be
     *  disassembled, the list is scanned to find the first entry that matches, and then its operator() is invoked.  An entry
     *  matches if the instruction bits to be disassembled match the @p match data member after both are masked according to
     *  the @p mask data member.  The @p mask and @p match (as a pair) are unique across all the subclass instances. */
    class Mips32 {
    public:
        enum Architecture { Release1, Release2, Release3, Micro };
        Mips32(Architecture arch, unsigned match, unsigned mask): arch(arch), match(match), mask(mask) {}
        virtual ~Mips32() {}
        Architecture arch;      // architecture where this instruction was introduced
        unsigned match;         // value of compared bits
        unsigned mask;          // bits of 'match' that will be compared
        typedef DisassemblerMips D;
        virtual SgAsmMipsInstruction *operator()(D *d, unsigned insn_bits) = 0;
    };

    /** Find an instruction-specific disassembler.  Using the specified instruction bits, search for and return an
     *  instruction-specific disassembler.  Returns null if no appropriate disassembler can be found.  Instruction-specific
     *  disassemblers know how to disassemble specific instruction types (or groups of closely related instructions). */
    Mips32 *find_idis(unsigned insn_bits);

    /** Insert an instruction-specific disassembler. If @p replace is false (the default) then the table must not already
     *  contain an entry that has the same @p mask and @p match values. The pointers are managed by the caller and must not be
     *  deleted while they are in the table. */
    void insert_idis(Mips32*, bool replace=false);

    /** Disassemble a single instruction. Given the bits of a MIPS32 instruction, attempt to disassemble the instruction.  If
     *  the bits can be disassembled, then a new SgAsmMipsInstruction is returned, otherwise it returns the null pointer. It
     *  may also throw an exception if a valid instruction-specific disassembler can be found but the instruction is
     *  malformed. */
    SgAsmMipsInstruction *disassemble_insn(unsigned insn_bits);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The following functions are used by the various instruction-specific Mips32 subclasses.

    /** Obtain the virtual address for the instruction being disassembled. */
    rose_addr_t get_ip() const { return insn_va; }

    /** Create a new instruction. */
    SgAsmMipsInstruction *makeInstruction(MipsInstructionKind, const std::string &mnemonic,
                                          SgAsmExpression *arg1=NULL, SgAsmExpression *arg2=NULL,
                                          SgAsmExpression *arg3=NULL, SgAsmExpression *arg4=NULL);

    /** Create a new general purpose register reference expression. */
    SgAsmRegisterReferenceExpression *makeRegister(unsigned regnum);

    /** Create a new floating point register reference expression. */
    SgAsmRegisterReferenceExpression *makeFpRegister(unsigned regnum);

    /** Create a new register reference for Coprocessor 0. */
    SgAsmRegisterReferenceExpression *makeCp0Register(unsigned regnum, unsigned sel);

    /** Create a new register reference for Coprocessor 2. */
    SgAsmRegisterReferenceExpression *makeCp2Register(unsigned regnum);

    /** Create a new floating point condition flag register reference expression.  The return value is a reference to one of
     *  the bits from the FCSR register.  If @p cc is zero then bit 23 is referenced, otherwise bit 24+cc is referenced. The @p
     *  cc value must be zero through seven, inclusive. */
    SgAsmRegisterReferenceExpression *makeFpccRegister(unsigned cc);

    /** Create a new register reference for a COP2 condition code.  See COP2ConditionCode() in the MIPS reference manual. */
    SgAsmRegisterReferenceExpression *makeCp2ccRegister(unsigned cc);

    /** Create a new register reference for a hardware register. See the RDHWR instruction documentation. */
    SgAsmRegisterReferenceExpression *makeHwRegister(unsigned regnum);

    /** Create a new register reference for a shadow GPR. */
    SgAsmRegisterReferenceExpression *makeShadowRegister(unsigned regnum);

    /** Create a new 8-bit value expression from an 8-bit value.  The @p bit_offset and @p nbits indicate where the value
     * originally came from in the instruction. */
    SgAsmIntegerValueExpression *makeImmediate8(unsigned value, size_t bit_offset, size_t nbits);

    /** Create a new 16-bit value expression from a 16-bit value. The @p bit_offset and @p nbits indicate where the value
     *  originally came from in the instruction. */
    SgAsmIntegerValueExpression *makeImmediate16(unsigned value, size_t bit_offset, size_t nbits);

    /** Create a new 32-bit value expression from a 32-bit value. The @p bit_offset and @p nbits indicate where the value
     *  originally came from in the instruction. */
    SgAsmIntegerValueExpression *makeImmediate32(unsigned value, size_t bit_offset, size_t nbits);

    /** Create a 32-bit PC-relative branch target address from a 16-bit offset. The @p bit_offset and @p nbits indicate where
     *  the value originally came from in the instruction (usually 0 and 16, respectively). The return address is the
     *  address of the delay slot plus four times the signed @p offset16. */
    SgAsmIntegerValueExpression *makeBranchTargetRelative(unsigned offset16, size_t bit_offset, size_t nbits);

    /** Create a 32-bit branch address from an instruction index value.  The returned value is the @p insn_index (@p nbits
     * wide) multiplied by four and then combined with the address of the delay slot.  They are combined such that the
     * low-order @p nbits+2 bits are from the product and the upper bits are from the delay slot address. */
    SgAsmIntegerValueExpression *makeBranchTargetAbsolute(unsigned insn_index, size_t bit_offset, size_t nbits);

    /** Build an expression for an offset from a register.  The return value is GPR[regnum]+signExtend(offset) expressed as an
     *  SgAsmBinaryAdd expression whose first operand is the register reference expression and second operand is the
     *  sign-extended offset. */
    SgAsmBinaryAdd *makeRegisterOffset(unsigned gprnum, unsigned offset16);

    /** Build a register index expression.  The returned value is makeRegister(base_gprnum)+makeRegister(index_gprnum). */
    SgAsmBinaryAdd *makeRegisterIndexed(unsigned base_gprnum, unsigned index_gprnum);

    /** Build a memory reference expression. */
    SgAsmMemoryReferenceExpression *makeMemoryReference(SgAsmExpression *addr, SgAsmType *type);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
    void init(ByteOrder::Endianness);

protected:
    /** Table of instruction-specific disassemblers.  This is the table of instruction-specific disassemblers consulted by
     *  find_idis(). */
    std::vector<Mips32*> idis_table;

    /** Address of instruction currently being disassembled. This is set each time disassembleOne() is called. */
    rose_addr_t insn_va;
};

} // namespace
} // namespace

#endif
#endif
