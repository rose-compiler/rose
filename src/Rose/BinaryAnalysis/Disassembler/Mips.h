/* Disassembly specific to the MIPS architecture */
#ifndef ROSE_BinaryAnalysis_Disassembler_Mips_H
#define ROSE_BinaryAnalysis_Disassembler_Mips_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Base.h>

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <Rose/BinaryAnalysis/InstructionEnumsMips.h>

#include <SageBuilderAsm.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

/** MIPS32 instruction decoder.
 *
 *  MIPS (Microprocessor without Interlocked Pipelined Stages) is a family of reduced instruction set computer (RISC) instruction
 *  set architectures (ISA) developed by MIPS Computer Systems, now MIPS Technologies, based in the United States.
 *
 *  There are multiple versions of MIPS: including MIPS I, II, III, IV, and V; as well as five releases of MIPS32/64 (for 32- and
 *  64-bit implementations, respectively). The early MIPS architectures were 32-bit; 64-bit versions were developed later. As of
 *  April 2017, the current version of MIPS is MIPS32/64 Release 6. MIPS32/64 primarily differs from MIPS I–V by defining the
 *  privileged kernel mode System Control Coprocessor in addition to the user mode architecture.
 *
 *  The MIPS architecture has several optional extensions. MIPS-3D which is a simple set of floating-point SIMD instructions
 *  dedicated to common 3D tasks, MDMX (MaDMaX) which is a more extensive integer SIMD instruction set using the 64-bit
 *  floating-point registers, MIPS16e which adds compression to the instruction stream to make programs take up less room, and MIPS
 *  MT, which adds multithreading capability.
 *
 *  The first release of MIPS32, based on MIPS II, added conditional moves, prefetch instructions, and other features from the R4000
 *  and R5000 families of 64-bit processors. The MUL and MADD (multiply-add) instructions, previously available in some
 *  implementations, were added to the MIPS32 specifications, as were cache control instructions. For the purpose of cache control,
 *  both SYNC and SYNCI instructions were prepared. */
class Mips: public Base {
public:
    /** Reference counting pointer. */
    using Ptr = MipsPtr;

protected:
    explicit Mips(const Architecture::BaseConstPtr&);

public:
    /** Allocating constructor for MIPS decoder.
     *
     *  MIPS executables can be big- or little-endian. */
    static Ptr instance(const Architecture::BaseConstPtr&);

    virtual Base::Ptr clone() const override;
    virtual SgAsmInstruction *disassembleOne(const MemoryMap::Ptr&, rose_addr_t start_va,
                                             AddressSet *successors=NULL) override;
    virtual size_t nDelaySlots(MipsInstructionKind);
    virtual SgAsmInstruction *makeUnknownInstruction(const Exception&) override;
    SgAsmMipsInstruction *makeUnknownInstruction(rose_addr_t insn_va, unsigned opcode) const;

    /** Interface for disassembling a single instruction.  Each instruction (or in some cases groups of closely related
     *  instructions) will define a subclass whose operator() unparses a single instruction word and returns an
     *  SgAsmMipsInstruction. These functors are allocated and inserted into a list. When an instruction word is to be
     *  disassembled, the list is scanned to find the first entry that matches, and then its operator() is invoked.  An entry
     *  matches if the instruction bits to be disassembled match the @p match data member after both are masked according to
     *  the @p mask data member.  The @p mask and @p match (as a pair) are unique across all the subclass instances. */
    class Decoder {
    public:
        enum Architecture { Release1, Release2, Release3, Micro };
        Decoder(Architecture arch, unsigned match, unsigned mask): arch(arch), match(match), mask(mask) {}
        virtual ~Decoder() {}
        Architecture arch;      // architecture where this instruction was introduced
        unsigned match;         // value of compared bits
        unsigned mask;          // bits of 'match' that will be compared
        typedef Mips D;
        virtual SgAsmMipsInstruction *operator()(rose_addr_t insn_va, const D *d, unsigned insn_bits) = 0;
    };

    /** Find an instruction-specific disassembler.  Using the specified instruction bits, search for and return an
     *  instruction-specific disassembler.  Returns null if no appropriate disassembler can be found.  Instruction-specific
     *  disassemblers know how to disassemble specific instruction types (or groups of closely related instructions). */
    Decoder *find_idis(rose_addr_t insn_va, unsigned insn_bits) const;

    /** Insert an instruction-specific disassembler. If @p replace is false (the default) then the table must not already
     *  contain an entry that has the same @p mask and @p match values. The pointers are managed by the caller and must not be
     *  deleted while they are in the table. */
    void insert_idis(Decoder*, bool replace=false);

    /** Disassemble a single instruction. Given the bits of a MIPS32 instruction, attempt to disassemble the instruction.  If
     *  the bits can be disassembled, then a new SgAsmMipsInstruction is returned, otherwise it returns the null pointer. It
     *  may also throw an exception if a valid instruction-specific disassembler can be found but the instruction is
     *  malformed. */
    SgAsmMipsInstruction *disassemble_insn(Address insn_va, unsigned insn_bits, const std::vector<uint8_t> &bytes) const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // The following functions are used by the various instruction-specific Mips32 subclasses.

    /** Create a new instruction. */
    SgAsmMipsInstruction *makeInstruction(rose_addr_t insn_va, MipsInstructionKind,
                                          SgAsmExpression *arg1=NULL, SgAsmExpression *arg2=NULL,
                                          SgAsmExpression *arg3=NULL, SgAsmExpression *arg4=NULL) const;

    /** Create a ROSE type form a MIPS data format. */
    SgAsmType* makeType(MipsDataFormat) const;

    /** Create a new general purpose register reference expression. */
    SgAsmRegisterReferenceExpression *makeRegister(rose_addr_t insn_va, unsigned regnum) const;

    /** Create a new floating point register reference expression. */
    SgAsmRegisterReferenceExpression *makeFpRegister(rose_addr_t insn_va, unsigned regnum, MipsDataFormat) const;

    /** Create a new register reference for Coprocessor 0. */
    SgAsmRegisterReferenceExpression *makeCp0Register(rose_addr_t insn_va, unsigned regnum, unsigned sel) const;

    /** Create a new register reference for Coprocessor 2. */
    SgAsmRegisterReferenceExpression *makeCp2Register(unsigned regnum) const;

    /** Create a new floating point condition flag register reference expression.  The return value is a reference to one of
     *  the bits from the FCSR register.  If @p cc is zero then bit 23 is referenced, otherwise bit 24+cc is referenced. The @p
     *  cc value must be zero through seven, inclusive. */
    SgAsmRegisterReferenceExpression *makeFpccRegister(rose_addr_t insn_va, unsigned cc) const;

    /** Create a new register reference for a COP2 condition code.  See COP2ConditionCode() in the MIPS reference manual. */
    SgAsmRegisterReferenceExpression *makeCp2ccRegister(unsigned cc) const;

    /** Create a new register reference for a hardware register. See the RDHWR instruction documentation. */
    SgAsmRegisterReferenceExpression *makeHwRegister(unsigned regnum) const;

    /** Create a new register reference for a shadow GPR. */
    SgAsmRegisterReferenceExpression *makeShadowRegister(rose_addr_t insn_va, unsigned regnum) const;

    /** Create a new 8-bit value expression from an 8-bit value.  The @p bit_offset and @p nbits indicate where the value
     * originally came from in the instruction. */
    SgAsmIntegerValueExpression *makeImmediate8(unsigned value, size_t bit_offset, size_t nbits) const;

    /** Create a new 16-bit value expression from a 16-bit value. The @p bit_offset and @p nbits indicate where the value
     *  originally came from in the instruction. */
    SgAsmIntegerValueExpression *makeImmediate16(unsigned value, size_t bit_offset, size_t nbits) const;

    /** Create a new 32-bit value expression from a 32-bit value. The @p bit_offset and @p nbits indicate where the value
     *  originally came from in the instruction. */
    SgAsmIntegerValueExpression *makeImmediate32(unsigned value, size_t bit_offset, size_t nbits) const;

    /** Create a 32-bit PC-relative branch target address from a 16-bit offset. The @p bit_offset and @p nbits indicate where
     *  the value originally came from in the instruction (usually 0 and 16, respectively). The return address is the
     *  address of the delay slot plus four times the signed @p offset16. */
    SgAsmIntegerValueExpression *makeBranchTargetRelative(rose_addr_t insn_va, unsigned offset16, size_t bit_offset,
                                                          size_t nbits) const;

    /** Create a 32-bit branch address from an instruction index value.  The returned value is the @p insn_index (@p nbits
     * wide) multiplied by four and then combined with the address of the delay slot.  They are combined such that the
     * low-order @p nbits+2 bits are from the product and the upper bits are from the delay slot address. */
    SgAsmIntegerValueExpression *makeBranchTargetAbsolute(rose_addr_t insn_va, unsigned insn_index, size_t bit_offset,
                                                          size_t nbits) const;

    /** Build an expression for an offset from a register.  The return value is GPR[regnum]+signExtend(offset) expressed as an
     *  SgAsmBinaryAdd expression whose first operand is the register reference expression and second operand is the
     *  sign-extended offset. */
    SgAsmBinaryAdd *makeRegisterOffset(rose_addr_t insn_va, unsigned gprnum, unsigned offset16) const;

    /** Build a register index expression.  The returned value is makeRegister(base_gprnum)+makeRegister(index_gprnum). */
    SgAsmBinaryAdd *makeRegisterIndexed(rose_addr_t insn_va, unsigned base_gprnum, unsigned index_gprnum) const;

    /** Build a memory reference expression. */
    SgAsmMemoryReferenceExpression *makeMemoryReference(SgAsmExpression *addr, SgAsmType *type) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
    void init();

protected:
    /** Table of instruction-specific disassemblers.  This is the table of instruction-specific disassemblers consulted by
     *  find_idis(). */
    std::vector<Decoder*> idis_table;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
