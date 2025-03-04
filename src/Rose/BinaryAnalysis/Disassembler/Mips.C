#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Mips.h>

#include "integerOps.h"
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/Diagnostics.h>
#include <ROSE_UNUSED.h>

#include <SgAsmBinaryAdd.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmFloatType.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmMipsInstruction.h>
#include <SgAsmVectorType.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

using namespace IntegerOps;
using namespace Diagnostics;

template<size_t lobit, size_t hibit>
static unsigned mask_for()
{
    return genMask<unsigned>(lobit, hibit);
}

// These simple little functions operate on certain bits of an instruction and are used commonly throughout the
// disassembler. The functions whose name begins with "m" are for creating bit masks; those beginning with "s" are for setting
// values; and those beginning with "g" are for getting values.  Think of the instruction word (32-bits wide) as being a union
// of structs where each member is a certain number of bits--then "m" functions return a union where all bits of one member are
// set and the rest are clear; "s" assigns a value to a member; and "g" obtains a member's value.
// 
// These are the names and positions of the members:
//     (msb)_______________________________________________________(lsb)
//     |3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1| | | | | | | | | | |
//     |1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
//     +-----------+---------+---------+---------+---------+-----------+
//     | OP  (6)   | R0 (5)  | R1  (5) | R2  (5) | R3  (5) | FN    (6) |
//     +-----------+---------+---------+---------+---------+-----------+
//     |                               |           IM                  |
//     +-------------------------------+-------------------------------+
static unsigned mOP() { return mask_for<26, 31>(); }                                    // mask for field
static unsigned mR0() { return mask_for<21, 25>(); }
static unsigned mR1() { return mask_for<16, 20>(); }
static unsigned mR2() { return mask_for<11, 15>(); }
static unsigned mR3() { return mask_for< 6, 10>(); }
static unsigned mFN() { return mask_for< 0,  5>(); }
static unsigned sOP(unsigned value) { return shift_to<26, 31>(value); }                 // set field
static unsigned sR0(unsigned value) { return shift_to<21, 25>(value); }
static unsigned sR1(unsigned value) { return shift_to<16, 20>(value); }
static unsigned sR2(unsigned value) { return shift_to<11, 15>(value); }
static unsigned sR3(unsigned value) { return shift_to< 6, 10>(value); }
static unsigned sFN(unsigned value) { return shift_to< 0,  5>(value); }
static unsigned gR0(unsigned insn_bits) { return extract<21, 25>(insn_bits); }          // get field
static unsigned gR1(unsigned insn_bits) { return extract<16, 20>(insn_bits); }
static unsigned gR2(unsigned insn_bits) { return extract<11, 15>(insn_bits); }
static unsigned gR3(unsigned insn_bits) { return extract< 6, 10>(insn_bits); }
static unsigned gIM(unsigned insn_bits) { return extract< 0, 15>(insn_bits); }

// Data type generators. Names are a type kind followed by size in bits. Type kinds are:
//    B  -- bit vector; unknown type
//    I  -- signed two's complement integer
//    U  -- unsigned integer
//    F  -- IEEE floating point (at least for F32 and F64)
//
// MIPS terminology for word sizes
//    8  -- byte
//    16 -- half word           (ROSE "Word")
//    32 -- word                (ROSE "DoubleWord")
//    64 -- double word         (ROSE "QuadWord")
//
static SgAsmType *type_B8()  { return SageBuilderAsm::buildTypeU8(); }
static SgAsmType *type_B16() { return SageBuilderAsm::buildTypeU16(); }
static SgAsmType *type_B32() { return SageBuilderAsm::buildTypeU32(); }
static SgAsmType *type_B64() { return SageBuilderAsm::buildTypeU64(); }
static SgAsmType *type_I8()  { return SageBuilderAsm::buildTypeI8(); }
static SgAsmType *type_I16() { return SageBuilderAsm::buildTypeI16(); }
static SgAsmType *type_I32() { return SageBuilderAsm::buildTypeI32(); }
static SgAsmType *type_U8()  { return SageBuilderAsm::buildTypeU8(); }
static SgAsmType *type_U16() { return SageBuilderAsm::buildTypeU16(); }
static SgAsmType *type_U32() { return SageBuilderAsm::buildTypeU32(); }

/*****************************************************************************************************************************/

Mips::Mips(const Architecture::Base::ConstPtr &arch)
    : Base(arch) {
    ASSERT_not_null(arch);
    init();
}

Mips::Ptr
Mips::instance(const Architecture::Base::ConstPtr &arch) {
    return Ptr(new Mips(arch));
}

Base::Ptr
Mips::clone() const {
    return Ptr(new Mips(*this));
}

// see base class
SgAsmInstruction *
Mips::disassembleOne(const MemoryMap::Ptr &map, const Address insn_va, AddressSet *successors) {
    // Read the machine code instruction. It is always four-byte, naturally-aligned, in big- or little-endian order.
    if (insn_va % instructionAlignment_ != 0)
        throw Exception("non-aligned instruction", insn_va);
    uint32_t insn_disk; // instruction in file byte order
    if (4 != map->at(insn_va).limit(4).require(MemoryMap::EXECUTABLE).read((uint8_t*)&insn_disk).size())
        throw Exception("short read", insn_va);
    unsigned insn_bits = ByteOrder::diskToHost(byteOrder(), insn_disk);
    const std::vector<uint8_t> memBytes((unsigned char*)&insn_disk, (unsigned char*)&insn_disk + 4);

    // Try to decode the machine instruction. If this fails, then return an "unknown" instruction.
    SgAsmMipsInstruction *insn = nullptr;
    try {
        insn = disassemble_insn(insn_va, insn_bits, memBytes);
    } catch (const Exception &) {
    }
    if (!insn)
        return makeUnknownInstruction(insn_va, insn_bits);

    // Naive CFG successors
    if (successors) {
        bool complete;
        *successors |= architecture()->getSuccessors(insn, complete/*out*/);
    }

    // Some MIPS instructions have a delay slot.
    if (nDelaySlots(insn->get_kind()) > 0) {
        // We currently support only single delay slots for MIPS. Later, if we decide to support older MIPS architectures, we can
        // represent multiple delay slots by chaining them together. I.e., given instruction I0 with two delay slots I1 and I2, we
        // can represent them by setting I0.delaySlot = I1 and I1.delaySlot = I2.
        ASSERT_require(nDelaySlots(insn->get_kind()) == 1);
        const Address delayAddr = insn->get_address() + insn->get_size();
        try {
            if (SgAsmInstruction *delayInsn = disassembleOne(map, delayAddr, nullptr))
                insn->set_delaySlot(delayInsn);
        } catch (...) {
        }
    }

    return insn;
}

size_t
Mips::nDelaySlots(MipsInstructionKind kind) {
    // From the "MIPS Architecture for Programmers Volume I-A: Introduction to the MIPS32 Architecture" document MD00082 revision
    // 6.01, 2014-08-20:
    //
    //   > The original MIPS architecture supports, indeed requires, delayed branches: the instruction after the branch may be
    //   > executed before the branch is taken (since typically the instruction after the branch has already been fetched). Code //
    //   > optimization can place instructions into this branch delay slot for improved performance. Typical delayed branches
    //   > execute // the delay-slot instruction, whether or not the branch was taken.
    //
    //   > Branch-likely instructions execute the delay-slot instruction if and only if the branch is taken. If the branch is not
    //   > taken, the delay-slot instruction is not executed. While Branch Likely was deprecated prior to Release 6, Release 6
    //   > removes Branch Likelies.
    //
    //   > Release 6 introduces conditional compact branches and compact jumps that do not have a delay slot; they have instead a
    //   > forbidden slot. Release 6 unconditional compact branches have neither a delay slot nor a forbidden slot
    switch (kind) {
        // Original branch instructions
        case mips_b:
        case mips_bal:
        case mips_bc1f:
        case mips_bc1t:
        case mips_bc2f:
        case mips_bc2t:
        case mips_beq:
        case mips_bgez:
        case mips_bgezal:
        case mips_bgtz:
        case mips_blez:
        case mips_bltz:
        case mips_bltzal:
        case mips_bne:
            return 1;

        // Branch-likely instructions
        case mips_bc1fl:
        case mips_bc1tl:
        case mips_bc2fl:
        case mips_bc2tl:
        case mips_beql:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtzl:
        case mips_blezl:
        case mips_bltzall:
        case mips_bltzl:
        case mips_bnel:
            return 1;

        // Jump instructions
        case mips_j:
        case mips_jal:
        case mips_jalr:
        case mips_jalr_hb:
        case mips_jalx:
        case mips_jr:
        case mips_jr_hb:
            return 1;

        // Other instructions don't have a delay slot
        default:
            return 0;
    }
}

// see base class
SgAsmInstruction *
Mips::makeUnknownInstruction(const Exception &e)
{
    SgAsmMipsInstruction *insn = makeInstruction(e.ip, mips_unknown_instruction);
    insn->set_rawBytes(e.bytes);
    return insn;
}

SgAsmMipsInstruction*
Mips::makeUnknownInstruction(Address insn_va, unsigned ib) const {
    SgAsmMipsInstruction *insn = makeInstruction(insn_va, mips_unknown_instruction);
    insn->set_rawBytes(SgUnsignedCharList((unsigned char*)&ib, (unsigned char*)&ib + 4));
    return insn;
}

SgAsmMipsInstruction *
Mips::makeInstruction(Address insn_va, MipsInstructionKind kind, SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3,
                      SgAsmExpression *op4) const
{
    SgAsmMipsInstruction *insn = new SgAsmMipsInstruction(insn_va, *architecture()->registrationId(), kind);

    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    /* If any operand is null, then the following operands must also be null because analysis assumes that the operand vector
     * indices correspond to argument positions and don't expect null-padding in the vector. */
    ASSERT_require2((!op1 && !op2 && !op3 && !op4) ||
                    ( op1 && !op2 && !op3 && !op4) ||
                    ( op1 &&  op2 && !op3 && !op4) ||
                    ( op1 &&  op2 &&  op3 && !op4) ||
                    ( op1 &&  op2 &&  op3 &&  op4),
                    "if an operand is null then the following operands must be null");

    if (op1)
        SageBuilderAsm::appendOperand(insn, op1);
    if (op2)
        SageBuilderAsm::appendOperand(insn, op2);
    if (op3)
        SageBuilderAsm::appendOperand(insn, op3);
    if (op4)
        SageBuilderAsm::appendOperand(insn, op4);

    return insn;
}

SgAsmType*
Mips::makeType(MipsDataFormat fmt) const {
    switch (fmt) {
        case mips_fmt_w: return SageBuilderAsm::buildTypeU32();
        case mips_fmt_l: return SageBuilderAsm::buildTypeU64();
        case mips_fmt_s: return SageBuilderAsm::buildIeee754Binary32();
        case mips_fmt_d: return SageBuilderAsm::buildIeee754Binary64();
        case mips_fmt_ps: return SageBuilderAsm::buildTypeVector(2, SageBuilderAsm::buildIeee754Binary32());
    }
    ASSERT_not_reachable("invalid type");
}

SgAsmRegisterReferenceExpression *
Mips::makeRegister(Address insn_va, unsigned regnum) const
{
    std::string regname = "r" + StringUtility::numberToString(regnum);
    const RegisterDescriptor reg = architecture()->registerDictionary()->find(regname);
    if (!reg)
        throw Exception("no such register: "+regname, insn_va);
    auto retval = new SgAsmDirectRegisterExpression(reg);
    retval->set_type(SageBuilderAsm::buildTypeU(reg.nBits()));
    return retval;
}

SgAsmRegisterReferenceExpression *
Mips::makeFpRegister(const Address insnVa, const unsigned regNum, const MipsDataFormat fmt) const {
    // See MIPS RegisterDictionary. Pairs of 32-bit `fN' registers where N is even are named `fNd`
    const std::string regNameSuffix = (mips_fmt_d == fmt || mips_fmt_ps == fmt || mips_fmt_l == fmt) ? "d" : "";
    const std::string regName = "f" + boost::lexical_cast<std::string>(regNum) + regNameSuffix;

    // Find the correct register
    const RegisterDescriptor reg = architecture()->registerDictionary()->find(regName);
    if (!reg)
        throw Exception("no such register: " + regName, insnVa);

    // Build the register expression and give it the appropriate type
    auto retval = new SgAsmDirectRegisterExpression(reg);
    retval->set_type(makeType(fmt));
    ASSERT_require(retval->get_type()->get_nBits() == reg.nBits());
    return retval;
}

SgAsmRegisterReferenceExpression *
Mips::makeCp0Register(Address insn_va, unsigned regnum, unsigned sel) const
{
    ASSERT_require(regnum<32);
    ASSERT_require(sel<8);
    std::string s;
    switch (regnum) {
        case 0:
            switch (sel) {
                case 0: s = "Index";            break;
                case 1: s = "MVPControl";       break;
                case 2: s = "MVPConf0";         break;
                case 3: s = "MVPConf1";         break;
            }
            break;
        case 1:
            switch (sel) {
                case 0: s = "Random";           break;
                case 1: s = "VPEControl";       break;
                case 2: s = "VPEConf0";         break;
                case 3: s = "VPEConf1";         break;
                case 4: s = "YQMask";           break;
                case 5: s = "VPESchedule";      break;
                case 6: s = "VPEScheFBack";     break;
                case 7: s = "VPEOpt";           break;
            }
            break;
        case 2:
            switch (sel) {
                case 0: s = "EntryLo0";         break;
                case 1: s = "TCStatus";         break;
                case 2: s = "TCBind";           break;
                case 3: s = "TCRestart";        break;
                case 4: s = "TCHalt";           break;
                case 5: s = "TCContext";        break;
                case 6: s = "TCSchedule";       break;
                case 7: s = "TCScheFBack";      break;
            }
            break;
        case 3:
            switch (sel) {
                case 0: s = "EntryLo1";         break;
                case 7: s = "TCOpt";            break;
            }
            break;
        case 4:
            switch (sel) {
                case 0: s = "Context";          break;
                case 1: s = "ContextConfig";    break;
                case 2: s = "UserLocal";        break;
            }
            break;
        case 5:
            switch (sel) {
                case 0: s = "PageMask";         break;
                case 1: s = "PageGrain";        break;
            }
            break;
        case 6:
            switch (sel) {
                case 0: s = "Wired";            break;
                case 1: s = "SRSConf0";         break;
                case 2: s = "SRSConf1";         break;
                case 3: s = "SRSConf2";         break;
                case 4: s = "SRSConf3";         break;
                case 5: s = "SRSConf4";         break;
            }
            break;
        case 7:
            switch (sel) {
                case 0: s = "HWREna";           break;
            }
            break;
        case 8:
            switch (sel) {
                case 0: s = "BadVAddr";         break;
            }
            break;
        case 9:
            switch (sel) {
                case 0: s = "Count";            break;
            }
            break;
        case 10:
            switch (sel) {
                case 0: s = "EntryHi";          break;
            }
            break;
        case 11:
            switch (sel) {
                case 0: s = "Compare";          break;
            }
            break;
        case 12:
            switch (sel) {
                case 0: s = "Status";           break;
                case 1: s = "IntCtl";           break;
                case 2: s = "SRSCtl";           break;
                case 3: s = "SRSMap";           break;
                case 4: s = "View_IPL";         break;
                case 5: s = "SRSMap2";          break;
            }
            break;
        case 13:
            switch (sel) {
                case 0: s = "Cause";            break;
                case 4: s = "View_RIPL";        break;
            }
            break;
        case 14:
            switch (sel) {
                case 0: s = "EPC";              break;
            }
            break;
        case 15:
            switch (sel) {
                case 0: s = "PRId";             break;
                case 1: s = "EBase";            break;
                case 2: s = "CDMMBase";         break;
                case 3: s = "CMGCRBase";        break;
            }
            break;
        case 16:
            switch (sel) {
                case 0: s = "Config";           break;
                case 1: s = "Config1";          break;
                case 2: s = "Config2";          break;
                case 3: s = "Config3";          break;
                case 4: s = "Config4";          break;  // Typo in the MIPS32 Architecture Volume III (it has sel==3)
            }
            break;
        case 17:
            switch (sel) {
                case 0: s = "LLAddr";           break;
            }
            break;
        case 18:
            s = "WatchLo";
            break;
        case 19:
            s = "WatchHi";
            break;
        case 20:
            break;
        case 21:
            break;
        case 22:
            break;
        case 23:
            switch (sel) {
                case 0: s = "Debug";            break;
                case 1: s = "TraceControl";     break;
                case 2: s = "TraceControl2";    break;
                case 3: s = "UserTraceData1";   break;
                case 4: s = "TraceIBPC";        break;
                case 5: s = "TraceDBPC";        break;
                case 6: s = "Debug2";           break;
            }
            break;
        case 24:
            switch (sel) {
                case 0: s = "DEPC";             break;
                case 1:                         break;
                case 2: s = "TraceControl3";    break;
                case 3: s = "UserTraceData2";   break;
            }
            break;
        case 25:
            s = "PerfCnt";
            break;
        case 26:
            switch (sel) {
                case 0: s = "ErrCtl";           break;
            }
            break;
        case 27:
            switch (sel) {
                case 0:
                case 1:
                case 2:
                case 3: s = "CacheErr";         break;
            }
            break;
        case 28:
            switch (sel) {
                case 0:
                case 2:
                case 4:
                case 6: s = "TagLo";            break;
                case 1:
                case 3:
                case 5:
                case 7: s = "DataLo";           break;
            }
            break;
        case 29:
            switch (sel) {
                case 0:
                case 2:
                case 4:
                case 6: s = "TagHi";            break;
                case 1:
                case 3:
                case 5:
                case 7: s = "DataHi";           break;
            }
            break;
        case 30:
            switch (sel) {
                case 0: s = "ErrorEPC";         break;
            }
            break;
        case 31:
            switch (sel) {
                case 0: s = "DESAVE";           break;
                case 1:                         break;
                case 2: s = "KScratch1";        break;
                case 3: s = "KScratch2";        break;
                case 4: s = "KScratch3";        break;
                case 5: s = "KScratch4";        break;
                case 6: s = "KScratch5";        break;
                case 7: s = "KScratch6";        break;
            }
            break;
    }
    if (s.empty())
        throw Exception("invalid CP0 register "+StringUtility::numberToString(regnum)+
                        " (sel="+StringUtility::numberToString(sel)+")", insn_va);
    const RegisterDescriptor regdesc = architecture()->registerDictionary()->find(s);
    if (!regdesc)
        throw Exception("no such register: " + s, insn_va);
    auto retval = new SgAsmDirectRegisterExpression(regdesc);
    retval->set_type(SageBuilderAsm::buildTypeU(regdesc.nBits()));
    return retval;
}

SgAsmRegisterReferenceExpression *
Mips::makeFpccRegister(const Address insn_va, const unsigned cc) const {
    ASSERT_require(cc <= 7);

    // Find the condition code bits of the FCSR
    const RegisterDescriptor regdesc = architecture()->registerDictionary()->find("fccr");
    if (!regdesc) {
        throw Exception("no such register: fccr", insn_va);
    }

    // cc0 (offset 23) ... cc7 (offset 30)
    RegisterDescriptor r(regdesc.majorNumber(), regdesc.minorNumber(), 23 + cc , 1);

    auto retval = new SgAsmDirectRegisterExpression(r);
    retval->set_type(SageBuilderAsm::buildTypeU(r.nBits()));
    return retval;
}

SgAsmRegisterReferenceExpression *
Mips::makeCp2Register(const unsigned regnum) const {
    // Coprocessor 2 is implementation defined. We're assuming 32 individual 32-bit registers numbered 0-31.
    ASSERT_require(regnum<32);
    const auto r = RegisterDescriptor(mips_regclass_cp2gpr, regnum, 0, 32);
    auto retval = new SgAsmDirectRegisterExpression(r);
    retval->set_type(SageBuilderAsm::buildTypeU(r.nBits()));
    return retval;
}
    
SgAsmRegisterReferenceExpression *
Mips::makeCp2ccRegister(const unsigned regnum) const {
    // Coprocessor 2 is implementation defined. We're assuming 32 individual 32-bit registers numbered 0-31.
    ASSERT_require(regnum<32);
    const auto r = RegisterDescriptor(mips_regclass_cp2spr, regnum, 0, 32);
    auto retval = new SgAsmDirectRegisterExpression(r);
    retval->set_type(SageBuilderAsm::buildTypeU(r.nBits()));
    return retval;
}

SgAsmRegisterReferenceExpression*
Mips::makeHwRegister(const unsigned regnum) const {
    ASSERT_require(regnum < 32);
    const auto r = RegisterDescriptor(mips_regclass_hw, regnum, 0, 32);
    auto retval = new SgAsmDirectRegisterExpression(r);
    retval->set_type(SageBuilderAsm::buildTypeU(r.nBits()));
    return retval;
}

SgAsmRegisterReferenceExpression *
Mips::makeShadowRegister(const Address insn_va, const unsigned cc) const {
    // Get the general purpose register
    SgAsmRegisterReferenceExpression *regref = makeRegister(insn_va, cc);
    ASSERT_not_null(regref);

    // Turn it into a shadow register
    RegisterDescriptor desc = regref->get_descriptor();
    desc.majorNumber(mips_regclass_sgpr);
    regref->set_descriptor(desc);
    return regref;
}

SgAsmIntegerValueExpression *
Mips::makeImmediate8(const unsigned value, const size_t bit_offset, const size_t nbits) const {
    ASSERT_require(0 == (value & ~0xff));
    SgAsmIntegerValueExpression *retval = SageBuilderAsm::buildValueU8(value);
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(nbits);
    return retval;
}

SgAsmIntegerValueExpression *
Mips::makeImmediate16(const unsigned value, const size_t bit_offset, const size_t nbits) const {
    ASSERT_require(0 == (value & ~0xffff));
    SgAsmIntegerValueExpression *retval = SageBuilderAsm::buildValueU16(value);
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(nbits);
    return retval;
}

SgAsmIntegerValueExpression *
Mips::makeImmediate32(const unsigned value, const size_t bit_offset, const size_t nbits) const {
    ASSERT_require(0 == (value & ~0xffffffffull));
    SgAsmIntegerValueExpression *retval = SageBuilderAsm::buildValueU32(value);
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(nbits);
    return retval;
}

SgAsmIntegerValueExpression *
Mips::makeBranchTargetRelative(const Address insn_va, unsigned pc_offset, const size_t bit_offset, size_t nbits) const {
    ASSERT_require(0 == (pc_offset & ~0xffff));
    pc_offset = shiftLeft<32>(pc_offset, 2);        // insns have 4-byte alignment
    pc_offset = signExtend<18, 32>(pc_offset);      // offsets are signed
    const unsigned target = (insn_va + 4 + pc_offset) & GenMask<unsigned, 32>::value; // measured from next instruction
    SgAsmIntegerValueExpression *retval = SageBuilderAsm::buildValueU32(target);
    retval->set_bitOffset(bit_offset);
    retval->set_bitSize(nbits);
    return retval;
}

SgAsmIntegerValueExpression *
Mips::makeBranchTargetAbsolute(Address insn_va, unsigned insn_index, size_t bit_offset, size_t nbits) const
{
    ROSE_UNUSED(bit_offset);

    ASSERT_require(nbits>0);
    ASSERT_require(bit_offset+nbits+2<=32);
    ASSERT_require(0==(insn_index & ~genMask<uint64_t>(nbits)));
    unsigned lo_nbits = nbits+2;        // number of bits coming from instr_index after multiplying by four
    unsigned lo_mask = genMask<uint64_t>(lo_nbits);
    unsigned lo_target = shiftLeft<32>(insn_index, 2) & lo_mask;
    unsigned hi_target = (insn_va + 4) & ~lo_mask;
    unsigned target = hi_target | lo_target;
    return SageBuilderAsm::buildValueU32(target);
}

SgAsmBinaryAdd *
Mips::makeRegisterOffset(Address insn_va, unsigned gprnum, unsigned offset16) const
{
    SgAsmRegisterReferenceExpression *regref = makeRegister(insn_va, gprnum);
    ASSERT_require(0==(offset16 & ~0xffff));
    unsigned offset32 = signExtend<16, 32>(offset16);
    SgAsmIntegerValueExpression *offset = SageBuilderAsm::buildValueU32(offset32);
    SgAsmBinaryAdd *retval = SageBuilderAsm::buildAddExpression(regref, offset);
    return retval;
}

SgAsmBinaryAdd *
Mips::makeRegisterIndexed(Address insn_va, unsigned base_gprnum, unsigned index_gprnum) const
{
    SgAsmRegisterReferenceExpression *base_reg = makeRegister(insn_va, base_gprnum);
    SgAsmRegisterReferenceExpression *index_reg = makeRegister(insn_va, index_gprnum);
    SgAsmBinaryAdd *retval = SageBuilderAsm::buildAddExpression(base_reg, index_reg);
    return retval;
}

SgAsmMemoryReferenceExpression *
Mips::makeMemoryReference(SgAsmExpression *addr, SgAsmType *type) const
{
    ASSERT_not_null(addr);
    ASSERT_not_null(type);
    return SageBuilderAsm::buildMemoryReferenceExpression(addr, NULL, type);
}

Mips::Decoder *
Mips::find_idis(Address insn_va, unsigned insn_bits) const
{
    Decoder *retval = NULL;
    for (size_t i=0; i<idis_table.size(); ++i) {
        if ((insn_bits & idis_table[i]->mask) == idis_table[i]->match) {
            if (retval) {
                std::string mesg = ("Rose::BinaryAnalysis::Disassembler::Mips::find_idis: multiple matches for instruction word " +
                                    StringUtility::addrToString(insn_bits) +
                                    " (first found mask=" + StringUtility::addrToString(retval->mask) +
                                    " match=" + StringUtility::addrToString(retval->match) +
                                    "; addnl found mask=" + StringUtility::addrToString(idis_table[i]->mask) +
                                    " match=" + StringUtility::addrToString(idis_table[i]->match) + ")");
                throw Exception(mesg, insn_va);
            } else {
                retval = idis_table[i];
            }
        }
    }
    return retval;
}

void
Mips::insert_idis(Decoder *idis, bool replace)
{
    ASSERT_not_null(idis);
    ASSERT_require(idis->mask!=0);
    ASSERT_require((idis->match & ~idis->mask)==0);
    bool inserted = false;
    for (size_t i=0; i<idis_table.size(); ++i) {
        if (replace && !inserted &&
            idis->mask==idis_table[i]->mask && idis->match==(idis_table[i]->match & idis_table[i]->mask)) {
            idis_table[i] = idis;
            inserted = true;
        } else if ((bitmask_subset(idis->mask, idis_table[i]->mask) &&
                    idis->match == (idis_table[i]->match & idis->mask)) ||
                   (bitmask_subset(idis_table[i]->mask, idis->mask) &&
                    idis_table[i]->match == (idis->match & idis_table[i]->mask))) {
            // Inserting this instruction-specific disassembler would cause an ambiguity in the table.
            mlog[DEBUG] <<"Rose::BinaryAnalysis::Disassembler::Mips::insert_idis: insertion key"
                        <<" mask=" <<StringUtility::addrToString(idis->mask)
                        <<" match=" <<StringUtility::addrToString(idis->match)
                        <<" conflicts with existing key"
                        <<" mask=" <<StringUtility::addrToString(idis_table[i]->mask)
                        <<" match="<<StringUtility::addrToString(idis_table[i]->match) <<"\n";
            throw Exception("insert_idis() would cause an ambiguity in the MIPS instruction disassembly table");
        }
    }
    if (!inserted)
        idis_table.push_back(idis);
}

SgAsmMipsInstruction *
Mips::disassemble_insn(const Address insn_va, const unsigned insn_bits, const std::vector<uint8_t> &bytes) const {
    if (Decoder *idis = find_idis(insn_va, insn_bits)) {
        if (SgAsmMipsInstruction *insn = (*idis)(insn_va, this, insn_bits)) {
            insn->set_rawBytes(bytes);
            return insn;
        }
    }
    return nullptr;
}

/*******************************************************************************************************************************
 *                                      Instruction-specific disassemblers
 *******************************************************************************************************************************/

// ABS.S -- floating point absolute value
static struct Mips32_abs_s: Mips::Decoder {
    Mips32_abs_s(): Mips::Decoder(Release1,
                           sOP(021)|sR0(020)|sR1(000)|sFN(005),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_abs_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_abs_s;

// ABS.D -- floating point absolute value
static struct Mips32_abs_d: Mips::Decoder {
    Mips32_abs_d(): Mips::Decoder(Release1,
                           sOP(021)|sR0(021)|sR1(000)|sFN(005),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_abs_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_abs_d;

// ABS.PS -- floating point absolute value
static struct Mips32_abs_ps: Mips::Decoder {
    Mips32_abs_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sR1(000)|sFN(005),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_abs_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps));
    }
} mips32_abs_ps;

// ADD -- add word
static struct Mips32_add: Mips::Decoder {
    Mips32_add(): Mips::Decoder(Release1,
                         sOP(000)|sR3(000)|sFN(040),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_add,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_add;

// ADD.S -- floating point add
static struct Mips32_add_s: Mips::Decoder {
    Mips32_add_s(): Mips::Decoder(Release1,
                           sOP(021)|sR0(020)|sFN(000),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_add_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_add_s;

// ADD.D -- floating point add
static struct Mips32_add_d: Mips::Decoder {
    Mips32_add_d(): Mips::Decoder(Release1,
                           sOP(021)|sR0(021)|sFN(000),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_add_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_add_d;

// ADD.PS -- floating point add
static struct Mips32_add_ps: Mips::Decoder {
    Mips32_add_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sFN(000),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_add_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_add_ps;

// ADDI -- add immediate word
static struct Mips32_addi: Mips::Decoder {
    Mips32_addi(): Mips::Decoder(Release1, sOP(010), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_addi,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_addi;

// ADDIU -- add immediate unsigned word
static struct Mips32_addiu: Mips::Decoder {
    Mips32_addiu(): Mips::Decoder(Release1, sOP(011), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_addiu,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16)); 
    }
} mips32_addiu;

// ADDU -- add unsigned word
static struct Mips32_addu: Mips::Decoder {
    Mips32_addu(): Mips::Decoder(Release1,
                          sOP(000)|sR3(000)|sFN(041),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_addu,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_addu;

// ALNV.PS -- floating point align variable
static struct Mips32_alnv_ps: Mips::Decoder {
    Mips32_alnv_ps(): Mips::Decoder(Release2,
                             sOP(023)|sFN(036),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_alnv_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_alnv_ps;

// AND -- bitwise logical AND
static struct Mips32_and: Mips::Decoder {
    Mips32_and(): Mips::Decoder(Release1,
                         sOP(000)|sR3(000)|sFN(044),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_and,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_and;

// ANDI -- and immediate
static struct Mips32_andi: Mips::Decoder {
    Mips32_andi(): Mips::Decoder(Release1, sOP(014), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_andi,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_andi;

// BC1F -- branch on FP false
// The "BC1F offset" is the same as "BC1F cc, offset" where cc==0
static struct Mips32_bc1f: Mips::Decoder {
    Mips32_bc1f(): Mips::Decoder(Release1,
                          sOP(021)|sR0(010)|shift_to<16, 17>(0),
                          mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(insn_va, mips_bc1f,
                                  d->makeFpccRegister(insn_va, cc),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bc1f;

// BC1FL -- branch on FP false likely (deprecated instruction)
static struct Mips32_bc1fl: Mips::Decoder {
    Mips32_bc1fl(): Mips::Decoder(Release1,
                           sOP(021)|sR0(010)|shift_to<16, 17>(2),
                           mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(insn_va, mips_bc1fl,
                                  d->makeFpccRegister(insn_va, cc),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bc1fl;

// BC1T -- branch on FP true
static struct Mips32_bc1t: Mips::Decoder {
    Mips32_bc1t(): Mips::Decoder(Release1,
                          sOP(021)|sR0(010)|shift_to<16, 17>(1),
                          mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(insn_va, mips_bc1t,
                                  d->makeFpccRegister(insn_va, cc),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bc1t;

// BC1TL -- branch on FP true likely (deprecated instruction)
static struct Mips32_bc1tl: Mips::Decoder {
    Mips32_bc1tl(): Mips::Decoder(Release1,
                           sOP(21)|sR0(010)|shift_to<16, 17>(3),
                           mOP()  |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(insn_va, mips_bc1tl,
                                  d->makeFpccRegister(insn_va, cc),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bc1tl;

// BC2F -- branch on COP2 false
static struct Mips32_bc2f: Mips::Decoder {
    Mips32_bc2f(): Mips::Decoder(Release1,
                          sOP(022)|sR0(010)|shift_to<16, 17>(0),
                          mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(insn_va, mips_bc2f,
                                  d->makeCp2ccRegister(cc),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bc2f;

// BC2FL -- branch on COP2 false likely (deprecated instruction)
static struct Mips32_bc2fl: Mips::Decoder {
    Mips32_bc2fl(): Mips::Decoder(Release1,
                           sOP(022)|sR0(010)|shift_to<16, 17>(2),
                           mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(insn_va, mips_bc2fl,
                                  d->makeCp2ccRegister(cc),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bc2fl;

// BC2T -- branch on COP2 true
static struct Mips32_bc2t: Mips::Decoder {
    Mips32_bc2t(): Mips::Decoder(Release1,
                          sOP(022)|sR0(010)|shift_to<16, 17>(1),
                          mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(insn_va, mips_bc2t,
                                  d->makeCp2ccRegister(cc),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bc2t;

// BC2TL -- branch on COP2 true likely (deprecated instruction)
static struct Mips32_bc2tl: Mips::Decoder {
    Mips32_bc2tl(): Mips::Decoder(Release1,
                           sOP(022)|sR0(010)|shift_to<16, 17>(3),
                           mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(insn_va, mips_bc2tl,
                                  d->makeCp2ccRegister(cc),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bc2tl;

// BEQ -- branch on equal
static struct Mips32_beq: Mips::Decoder {
    Mips32_beq(): Mips::Decoder(Release1, sOP(004), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        if (gR0(ib) == 0 && gR1(ib) == 0) {
            return d->makeInstruction(insn_va, mips_b, d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
        } else {
            return d->makeInstruction(insn_va, mips_beq,
                                      d->makeRegister(insn_va, gR0(ib)),
                                      d->makeRegister(insn_va, gR1(ib)),
                                      d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
        }
    }
} mips32_beq;

// BEQL -- branch on equal likely
static struct Mips32_beql: Mips::Decoder {
    Mips32_beql(): Mips::Decoder(Release1, sOP(024), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_beql,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_beql;

// BGEZ -- branch on greater than or equal to zero
static struct Mips32_bgez: Mips::Decoder {
    Mips32_bgez(): Mips::Decoder(Release1,
                          sOP(001)|sR1(001),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bgez,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bgez;

// BGEZAL -- branch on greater than or equal to zero and link
static struct Mips32_bgezal: Mips::Decoder {
    Mips32_bgezal(): Mips::Decoder(Release1,
                            sOP(001)|sR1(021),
                            mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        if (gR0(ib) == 0) {
            return d->makeInstruction(insn_va, mips_bal, d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
        } else {
            return d->makeInstruction(insn_va, mips_bgezal,
                                      d->makeRegister(insn_va, gR0(ib)),
                                      d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
        }
    }
} mips32_bgezal;

// BGEZALL -- branch on greater than or equal to zero and link likely (deprecated instruction)
static struct Mips32_bgezall: Mips::Decoder {
    Mips32_bgezall(): Mips::Decoder(Release1,
                             sOP(001)|sR1(023),
                             mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bgezall,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bgezall;

// BGEZL -- branch on greater than or equal to zero likely (deprecated instruction)
static struct Mips32_bgezl: Mips::Decoder {
    Mips32_bgezl(): Mips::Decoder(Release1,
                           sOP(001)|sR1(003),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bgezl,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bgezl;

// BGTZ -- branch on greater than zero
static struct Mips32_bgtz: Mips::Decoder {
    Mips32_bgtz(): Mips::Decoder(Release1,
                          sOP(007)|sR1(000),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bgtz,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bgtz;

// BGTZL -- branch on greater than zero likely
static struct Mips32_bgtzl: Mips::Decoder {
    Mips32_bgtzl(): Mips::Decoder(Release1,
                           sOP(023)|sR1(000),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bgtzl,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bgtzl;

// BLEZ -- branch on less than or equal to zero
static struct Mips32_blez: Mips::Decoder {
    Mips32_blez(): Mips::Decoder(Release1,
                          sOP(006)|sR1(000),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_blez,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_blez;

// BLEZL -- branch on less than or equal to zero likely (deprecated instruction)
static struct Mips32_blezl: Mips::Decoder {
    Mips32_blezl(): Mips::Decoder(Release1,
                           sOP(026)|sR1(000),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_blezl,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_blezl;

// BLTZ -- branch on less than zero
static struct Mips32_bltz: Mips::Decoder {
    Mips32_bltz(): Mips::Decoder(Release1,
                          sOP(001)|sR1(000),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bltz,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bltz;

// BLTZAL -- branch on less than zero and link
static struct Mips32_bltzal: Mips::Decoder {
    Mips32_bltzal(): Mips::Decoder(Release1,
                            sOP(001)|sR1(020),
                            mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bltzal,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bltzal;

// BLTZALL -- branch on less than zero and link likely (deprecated instruction)
static struct Mips32_bltzall: Mips::Decoder {
    Mips32_bltzall(): Mips::Decoder(Release1,
                             sOP(001)|sR1(022),
                             mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bltzall,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bltzall;

// BLTZL -- branch on less than zero likely
static struct Mips32_bltzl: Mips::Decoder {
    Mips32_bltzl(): Mips::Decoder(Release1,
                           sOP(001)|sR1(002),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bltzl,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bltzl;

// BNE -- branch on not equal
static struct Mips32_bne: Mips::Decoder {
    Mips32_bne(): Mips::Decoder(Release1, sOP(005), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bne,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bne;

// BNEL -- branch on not equal likely (deprecated instruction)
static struct Mips32_bnel: Mips::Decoder {
    Mips32_bnel(): Mips::Decoder(Release1, sOP(025), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_bnel,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeBranchTargetRelative(insn_va, gIM(ib), 0, 16));
    }
} mips32_bnel;
#ifdef _MSC_VER
#define UNUSED_VAR
#else
#define UNUSED_VAR __attribute__((unused))
#endif
// BREAK -- breakpoint
static struct Mips32_break: Mips::Decoder {
    Mips32_break(): Mips::Decoder(Release1,
                           sOP(000)|sFN(015),
                           mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 25>(ib); // only available by reading the instruction
        return d->makeInstruction(insn_va, mips_break);
    }
} mips32_break;

// C.cond.S --floating point compare single precision
// where "cond" is one of F, UN, EQ, UEQ, OLT, ULT, OLE, ULE, SF, NGLE, SEQ, NGL, LT, NGE, LE, or NGT.
// Note that the "C.cond.S fs, ft" form is the same as "C.cond.S 0, fs, ft" (i.e., cc==0)
static struct Mips32_c_cond_s: Mips::Decoder {
    Mips32_c_cond_s(): Mips::Decoder(Release1,
                              sOP(021)|sR0(020)|shift_to<4, 7>(003),
                              mOP()   |mR0()   |mask_for<4, 7>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<8, 10>(ib);
        unsigned condition = extract<0, 3>(ib);
        MipsInstructionKind kind;
        switch (condition) {
            case  0: kind = mips_c_f_s;    break;
            case  1: kind = mips_c_un_s;   break;
            case  2: kind = mips_c_eq_s;   break;
            case  3: kind = mips_c_ueq_s;  break;
            case  4: kind = mips_c_olt_s;  break;
            case  5: kind = mips_c_ult_s;  break;
            case  6: kind = mips_c_ole_s;  break;
            case  7: kind = mips_c_ule_s;  break;
            case  8: kind = mips_c_sf_s;   break;
            case  9: kind = mips_c_ngle_s; break;
            case 10: kind = mips_c_seq_s;  break;
            case 11: kind = mips_c_ngl_s;  break;
            case 12: kind = mips_c_lt_s;   break;
            case 13: kind = mips_c_nge_s;  break;
            case 14: kind = mips_c_le_s;   break;
            case 15: kind = mips_c_ngt_s;  break;
            default: ASSERT_not_reachable("invalid condition " + StringUtility::numberToString(condition));
        }
        return d->makeInstruction(insn_va, kind,
                                  d->makeFpccRegister(insn_va, cc),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_c_cond_s;

// C.cond.D --floating point compare double precision
// where "cond" is one of F, UN, EQ, UEQ, OLT, ULT, OLE, ULE, SF, NGLE, SEQ, NGL, LT, NGE, LE, or NGT.
// Note that the "C.cond.D fs, ft" form is the same as "C.cond.D 0, fs, ft" (i.e., cc==0)
static struct Mips32_c_cond_d: Mips::Decoder {
    Mips32_c_cond_d(): Mips::Decoder(Release1,
                              sOP(021)|sR0(021)|shift_to<4, 7>(003),
                              mOP()   |mR0()   |mask_for<4, 7>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<8, 10>(ib);
        unsigned condition = extract<0, 3>(ib);
        MipsInstructionKind kind;
        switch (condition) {
            case  0: kind = mips_c_f_d;    break;
            case  1: kind = mips_c_un_d;   break;
            case  2: kind = mips_c_eq_d;   break;
            case  3: kind = mips_c_ueq_d;  break;
            case  4: kind = mips_c_olt_d;  break;
            case  5: kind = mips_c_ult_d;  break;
            case  6: kind = mips_c_ole_d;  break;
            case  7: kind = mips_c_ule_d;  break;
            case  8: kind = mips_c_sf_d;   break;
            case  9: kind = mips_c_ngle_d; break;
            case 10: kind = mips_c_seq_d;  break;
            case 11: kind = mips_c_ngl_d;  break;
            case 12: kind = mips_c_lt_d;   break;
            case 13: kind = mips_c_nge_d;  break;
            case 14: kind = mips_c_le_d;   break;
            case 15: kind = mips_c_ngt_d;  break;
            default: ASSERT_not_reachable("invalid condition " + StringUtility::numberToString(condition));
        }
        return d->makeInstruction(insn_va, kind,
                                  d->makeFpccRegister(insn_va, cc),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_c_cond_d;

// C.cond.PS --floating point compare pair of single precision
// where "cond" is one of F, UN, EQ, UEQ, OLT, ULT, OLE, ULE, SF, NGLE, SEQ, NGL, LT, NGE, LE, or NGT.
// Note that the "C.cond.PS fs, ft" form is the same as "C.cond.PS 0, fs, ft" (i.e., cc==0)
static struct Mips32_c_cond_ps: Mips::Decoder {
    Mips32_c_cond_ps(): Mips::Decoder(Release2,
                               sOP(021)|sR0(026)|shift_to<4, 7>(003),
                               mOP()   |mR0()   |mask_for<4, 7>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned cc = extract<8, 10>(ib);
        unsigned condition = extract<0, 3>(ib);
        MipsInstructionKind kind;
        switch (condition) {
            case  0: kind = mips_c_f_ps;    break;
            case  1: kind = mips_c_un_ps;   break;
            case  2: kind = mips_c_eq_ps;   break;
            case  3: kind = mips_c_ueq_ps;  break;
            case  4: kind = mips_c_olt_ps;  break;
            case  5: kind = mips_c_ult_ps;  break;
            case  6: kind = mips_c_ole_ps;  break;
            case  7: kind = mips_c_ule_ps;  break;
            case  8: kind = mips_c_sf_ps;   break;
            case  9: kind = mips_c_ngle_ps; break;
            case 10: kind = mips_c_seq_ps;  break;
            case 11: kind = mips_c_ngl_ps;  break;
            case 12: kind = mips_c_lt_ps;   break;
            case 13: kind = mips_c_nge_ps;  break;
            case 14: kind = mips_c_le_ps;   break;
            case 15: kind = mips_c_ngt_ps;  break;
            default: ASSERT_not_reachable("invalid condition " + StringUtility::numberToString(condition));
        }
        return d->makeInstruction(insn_va, kind,
                                  d->makeFpccRegister(insn_va, cc),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_c_cond_ps;

// CACHE -- perform cache operation
static struct Mips32_cache: Mips::Decoder {
    Mips32_cache(): Mips::Decoder(Release1, sOP(057), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cache,
                                  d->makeImmediate8(gR1(ib), 16, 5),
                                  d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib)));
    }
} mips32_cache;

// CACHEE -- perform cache operation EVA
static struct Mips32_cachee: Mips::Decoder {
    Mips32_cachee(): Mips::Decoder(Release1,
                            sOP(037)|sFN(033)|shift_to<6, 6>(0),
                            mOP()   |mFN()   |mask_for<6, 6>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cachee,
                                  d->makeImmediate8(gR1(ib), 16, 5),
                                  d->makeRegisterOffset(insn_va, gR0(ib), extract<7, 15>(ib)));
    }
} mips32_cachee;

// CEIL.L.S -- fixed point ceiling convert to long fixed point
static struct Mips32_ceil_l_s: Mips::Decoder {
    Mips32_ceil_l_s(): Mips::Decoder(Release2,
                              sOP(021)|sR0(020)|sR1(0)||sFN(012),
                              mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ceil_l_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_ceil_l_s;

// CEIL.L.D -- fixed point ceiling convert to long fixed point
static struct Mips32_ceil_l_d: Mips::Decoder {
    Mips32_ceil_l_d(): Mips::Decoder(Release2,
                              sOP(021)|sR0(021)|sR1(0)|sFN(012),
                              mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ceil_l_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_ceil_l_d;

// CEIL.W.S -- floating point ceiling convert to word fixed point
static struct Mips32_ceil_w_s: Mips::Decoder {
    Mips32_ceil_w_s(): Mips::Decoder(Release1,
                              sOP(021)|sR0(020)|sR1(0)|sFN(016),
                              mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ceil_w_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_ceil_w_s;

// CEIL.W.D -- floating point ceiling convert to word fixed point
static struct Mips32_ceil_w_d: Mips::Decoder {
    Mips32_ceil_w_d(): Mips::Decoder(Release1,
                              sOP(021)|sR0(021)|sR1(0)|sFN(016),
                              mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ceil_w_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_ceil_w_d;

// CFC1 -- move control word from floating point
static struct Mips32_cfc1: Mips::Decoder {
    Mips32_cfc1(): Mips::Decoder(Release1,
                          sOP(021)|sR0(002)|shift_to<0, 10>(0),
                          mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cfc1,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate8(gR2(ib), 11, 5));
    }
} mips32_cfc1;

// CFC2 -- move control word from coprocessor 2
static struct Mips32_cfc2: Mips::Decoder {
    Mips32_cfc2(): Mips::Decoder(Release1,
                          sOP(022)|sR0(002),
                          mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cfc2,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_cfc2;

// CLO -- count leading ones in word
static struct Mips32_clo: Mips::Decoder {
    Mips32_clo(): Mips::Decoder(Release1,
                         sOP(034)|sR3(0)|sFN(041),
                         mOP()   |mR3() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        bool valid UNUSED_VAR = gR1(ib) == gR2(ib); // r1 must be same register number as r2
        return d->makeInstruction(insn_va, mips_clo,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_clo;

// CLZ -- count leading zeros in word
static struct Mips32_clz: Mips::Decoder {
    Mips32_clz(): Mips::Decoder(Release1,
                         sOP(034)|sR3(0)|sFN(040),
                         mOP()   |mR3() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        bool valid UNUSED_VAR = gR1(ib) == gR2(ib); // r1 must be same register number as r2
        return d->makeInstruction(insn_va, mips_clz,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_clz;

// COP2 -- coprocessor operation to coprocessor 2
static struct Mips32_cop2: Mips::Decoder {
    Mips32_cop2(): Mips::Decoder(Release1,
                          sOP(022)|shift_to<25, 25>(1),
                          mOP()   |mask_for<25, 25>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cop2,
                                  d->makeImmediate32(extract<0, 24>(ib), 0, 24));
    }
} mips32_cop2;

// CTC1 -- move control word to floating point
static struct Mips32_ctc1: Mips::Decoder {
    Mips32_ctc1(): Mips::Decoder(Release1,
                          sOP(021)|sR0(006)|shift_to<0, 10>(0),
                          mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ctc1,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate8(gR2(ib), 11, 5));
    }
} mips32_ctc1;

// CTC2 -- move control word to coprocessor 2
static struct Mips32_ctc2: Mips::Decoder {
    Mips32_ctc2(): Mips::Decoder(Release1,
                          sOP(022)|sR0(006),
                          mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ctc2,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_ctc2;

// CVT.D.S -- floating point convert to double floating point
static struct Mips32_cvt_d_s: Mips::Decoder {
    Mips32_cvt_d_s(): Mips::Decoder(Release1,
                             sOP(021)|sR0(020)|sR1(0)|sFN(041),
                             mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_d_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_cvt_d_s;

// CVT.D.W -- floating point convert to double floating point
static struct Mips32_cvt_d_w: Mips::Decoder {
    Mips32_cvt_d_w(): Mips::Decoder(Release1,
                             sOP(021)|sR0(024)|sR1(0)|sFN(041),
                             mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_d_w,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_w));
    }
} mips32_cvt_d_w;

// CVT.D.L -- floating point convert to double floating point
static struct Mips32_cvt_d_l: Mips::Decoder {
    Mips32_cvt_d_l(): Mips::Decoder(Release2,
                             sOP(021)|sR0(025)|sR1(0)|sFN(041),
                             mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_d_l,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_l));
    }
} mips32_cvt_d_l;

// CVT.L.S -- floating point convert to long fixed point
static struct Mips32_cvt_l_s: Mips::Decoder {
    Mips32_cvt_l_s(): Mips::Decoder(Release2,
                             sOP(021)|sR0(020)|sR1(0)|sFN(045),
                             mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_l_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_cvt_l_s;

// CVT.L.D -- floating point convert to long fixed point
static struct Mips32_cvt_l_d: Mips::Decoder {
    Mips32_cvt_l_d(): Mips::Decoder(Release2,
                             sOP(21)|sR0(021)|sR1(0)|sFN(045),
                             mOP()  |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_l_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_cvt_l_d;

// CVT.PS.S -- floating point convert to paired singled
static struct Mips32_cvt_ps_s: Mips::Decoder {
    Mips32_cvt_ps_s(): Mips::Decoder(Release2,
                              sOP(021)|sR0(020)|sFN(046),
                              mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_ps_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_cvt_ps_s;

// CVT.S.D -- floating point convert to single floating point
static struct Mips32_cvt_s_d: Mips::Decoder {
    Mips32_cvt_s_d(): Mips::Decoder(Release1,
                             sOP(021)|sR0(021)|sR1(000)|sFN(040),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_s_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_cvt_s_d;

// CVT.S.W -- floating point convert to single floating point
static struct Mips32_cvt_s_w: Mips::Decoder {
    Mips32_cvt_s_w(): Mips::Decoder(Release1,
                             sOP(021)|sR0(024)|sR1(000)|sFN(040),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_s_w,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_w));
    }
} mips32_cvt_s_w;

// CVT.S.L -- floating point convert to single floating point
static struct Mips32_cvt_s_l: Mips::Decoder {
    Mips32_cvt_s_l(): Mips::Decoder(Release2,
                             sOP(021)|sR0(025)|sR1(000)|sFN(040),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_s_l,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_l));
    }
} mips32_cvt_s_l;

// CVT.S.PL -- floating point convert pair lower to single floating point
static struct Mips32_cvt_s_pl: Mips::Decoder {
    Mips32_cvt_s_pl(): Mips::Decoder(Release2,
                              sOP(021)|sR0(026)|sR1(000)|sFN(050),
                              mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_s_pl,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps));
    }
} mips32_cvt_s_pl;

// CVT.S.PU -- floating point convert pair upper to single floating point
static struct Mips32_cvt_s_pu: Mips::Decoder {
    Mips32_cvt_s_pu(): Mips::Decoder(Release2,
                              sOP(021)|sR0(026)|sR1(000)|sFN(040),
                              mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_s_pu,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps));
    }
} mips32_cvt_s_pu;

// CVT.W.S -- floating point convert to word fixed point
static struct Mips32_cvt_w_s: Mips::Decoder {
    Mips32_cvt_w_s(): Mips::Decoder(Release1,
                             sOP(021)|sR0(020)|sR1(000)|sFN(044),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_w_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_cvt_w_s;

// CVT.W.D -- floating point convert to word fixed point
static struct Mips32_cvt_w_d: Mips::Decoder {
    Mips32_cvt_w_d(): Mips::Decoder(Release1,
                             sOP(021)|sR0(021)|sR1(000)|sFN(044),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_cvt_w_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_cvt_w_d;

// DI -- disable interrupts
// The "DI" form is the same as "DI r0"
static struct Mips32_di: Mips::Decoder {
    Mips32_di(): Mips::Decoder(Release2,
                        sOP(020)|sR0(023)|sR2(024)|sR3(0)|shift_to<5, 5>(0)|shift_to<3, 4>(0)|shift_to<0, 2>(0),
                        mOP()   |mR0()   |mR2()   |mR3() |mask_for<5, 5>() |mask_for<3, 4>() |mask_for<0, 2>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_di,
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_di;

// DIV -- divide word
static struct Mips32_div: Mips::Decoder {
    Mips32_div(): Mips::Decoder(Release1,
                         sOP(000)|shift_to<6, 15>(0)|sFN(032),
                         mOP()   |mask_for<6, 15>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_div,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_div;

// DIV.S -- floating point divide
static struct Mips32_div_s: Mips::Decoder {
    Mips32_div_s(): Mips::Decoder(Release1,
                           sOP(021)|sR0(020)|sFN(003),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_div_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_div_s;

// DIV.D -- floating point divide
static struct Mips32_div_d: Mips::Decoder {
    Mips32_div_d(): Mips::Decoder(Release1,
                           sOP(021)|sR0(021)|sFN(003),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_div_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_div_d;

// DIVU -- divide unsigned word
static struct Mips32_divu: Mips::Decoder {
    Mips32_divu(): Mips::Decoder(Release1,
                          sOP(000)|shift_to<6, 15>(0)|sFN(033),
                          mOP()   |mask_for<6, 15>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_divu,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_divu;

// EI -- enable interrupts
// The "EI" form is the same as "EI r0"
static struct Mips32_ei: Mips::Decoder {
    Mips32_ei(): Mips::Decoder(Release2,
                        sOP(020)|sR0(023)|sR2(014)|sR3(0)|shift_to<5, 5>(1)|shift_to<3, 4>(0)|shift_to<0, 2>(0),
                        mOP()   |mR0()   |mR2()   |mR3() |mask_for<5, 5>() |mask_for<3, 4>() |mask_for<0, 2>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ei,
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_ei;

// ERET -- exception return
static struct Mips32_eret: Mips::Decoder {
    Mips32_eret(): Mips::Decoder(Release1,
                          sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(030),
                          mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned) {
        return d->makeInstruction(insn_va, mips_eret);
    }
} mips32_eret;

// EXT -- extract bit field
static struct Mips32_ext: Mips::Decoder {
    Mips32_ext(): Mips::Decoder(Release2,
                         sOP(037)|sFN(000),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ext,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate8(gR3(ib), 6, 5),
                                  d->makeImmediate8(gR2(ib)+1, 11, 5));
    }
} mips32_ext;

// FLOOR.L.S -- floating point floor convert to long fixed point
static struct Mips32_floor_l_s: Mips::Decoder {
    Mips32_floor_l_s(): Mips::Decoder(Release2,
                               sOP(021)|sR0(020)|sR1(000)|sFN(013),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_floor_l_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_floor_l_s;

// FLOOR.L.D -- floating point floor convert to long fixed point
static struct Mips32_floor_l_d: Mips::Decoder {
    Mips32_floor_l_d(): Mips::Decoder(Release2,
                               sOP(021)|sR0(021)|sR1(000)|sFN(013),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_floor_l_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_floor_l_d;

// FLOOR.W.S -- floating point floor convert to word fixed point
static struct Mips32_floor_w_s: Mips::Decoder {
    Mips32_floor_w_s(): Mips::Decoder(Release1,
                               sOP(021)|sR0(020)|sR1(000)|sFN(017),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_floor_w_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_floor_w_s;

// FLOOR.W.D -- floating point floor convert to word fixed point
static struct Mips32_floor_w_d: Mips::Decoder {
    Mips32_floor_w_d(): Mips::Decoder(Release1,
                               sOP(021)|sR0(021)|sR1(000)|sFN(017),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_floor_w_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_floor_w_d;

// INS -- insert bit field
static struct Mips32_ins: Mips::Decoder {
    Mips32_ins(): Mips::Decoder(Release2,
                         sOP(037)|sFN(004),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned pos = gR3(ib);
        unsigned size = gR2(ib) - pos + 1;
        if (pos >= 32 || size > 32 || pos + size > 32)
            return d->makeUnknownInstruction(insn_va, ib);
        return d->makeInstruction(insn_va, mips_ins,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate8(pos, 6, 5),
                                  d->makeImmediate8(size, 11, 5));
    }
} mips32_ins;

// J -- jump
static struct Mips32_j: Mips::Decoder {
    Mips32_j(): Mips::Decoder(Release1, sOP(002), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_j,
                                  d->makeBranchTargetAbsolute(insn_va, extract<0, 25>(ib), 0, 26));
    }
} mips32_j;

// JAL -- jump and link
static struct Mips32_jal: Mips::Decoder {
    Mips32_jal(): Mips::Decoder(Release1, sOP(003), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_jal,
                                  d->makeBranchTargetAbsolute(insn_va, extract<0, 25>(ib), 0, 26));
    }
} mips32_jal;

// JALR -- jump and link register
// The "JALR rs" form is the same as "JALR rd, rs" when rd==31
static struct Mips32_jalr: Mips::Decoder {
    Mips32_jalr(): Mips::Decoder(Release1,
                          sOP(000)|sR1(000)|sR3(000)|sFN(011),  // R3 "hint" field must be zero for Release1
                          mOP()   |mR1()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_jalr,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_jalr;

// JALR.HB -- jump and link register with hazard barrier
// The "JALR.HB rs" form is the same as "JALR.HB rd, rs" when rd==31
static struct Mips32_jalr_hb: Mips::Decoder {
    Mips32_jalr_hb(): Mips::Decoder(Release2, // "hint" field (bits 6-9) must be zero for Release1
                             sOP(000)|sR1(000)|shift_to<10, 10>(1)|shift_to<6, 9>(0)|sFN(011),
                             mOP()   |mR1()   |mask_for<10, 10>() |mask_for<6, 9>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_jalr_hb,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_jalr_hb;

// JALX -- jump and link exchange
static struct Mips32_jalx: Mips::Decoder {
    Mips32_jalx(): Mips::Decoder(Release1, sOP(035), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_jalx,
                                  d->makeBranchTargetAbsolute(insn_va, extract<0, 25>(ib), 0, 26));
    }
} mips32_jalx;

// JR -- jump register
static struct Mips32_jr: Mips::Decoder {
    Mips32_jr(): Mips::Decoder(Release1, // "hint" field (bits 6-10) must be zero for Release1
                        sOP(000)|shift_to<11, 20>(0)|shift_to<6, 10>(0)|sFN(010),
                        mOP()   |mask_for<11, 20>() |mask_for<6, 10>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_jr,
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_jr;

// JR.HB -- jump register with hazard barrier
static struct Mips32_jr_hb: Mips::Decoder {
    Mips32_jr_hb(): Mips::Decoder(Release2, // "hint field (bits 6-9) must be zero for Release1
                           sOP(000)|shift_to<11, 20>(0)|shift_to<10, 10>(1)|shift_to<6, 9>(0)|sFN(010),
                           mOP()   |mask_for<11, 20>() |mask_for<10, 10>() |mask_for<6, 9>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_jr_hb,
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_jr_hb;

// LB -- load byte
static struct Mips32_lb: Mips::Decoder {
    Mips32_lb(): Mips::Decoder(Release1, sOP(040), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lb,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I8()));
    }
} mips32_lb;

// LBE -- load byte EVA
static struct Mips32_lbe: Mips::Decoder {
    Mips32_lbe(): Mips::Decoder(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(054),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_lbe,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I8()));
    }
} mips32_lbe;

// LBU -- load byte unsigned
static struct Mips32_lbu: Mips::Decoder {
    Mips32_lbu(): Mips::Decoder(Release1, sOP(044), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lbu,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_U8()));
    }
} mips32_lbu;

// LBUE -- load byte unsigned EVA
static struct Mips32_lbue: Mips::Decoder {
    Mips32_lbue(): Mips::Decoder(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(050),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_lbue,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_U8()));
    }
} mips32_lbue;

// LDC1 -- load doubleword to floating point
static struct Mips32_ldc1: Mips::Decoder {
    Mips32_ldc1(): Mips::Decoder(Release1, sOP(065), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_ldc1,
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d),
                                  d->makeMemoryReference(addr, type_B64()));
    }
} mips32_ldc1;

// LDC2 -- load doubleword to coprocessor 2
static struct Mips32_ldc2: Mips::Decoder {
    Mips32_ldc2(): Mips::Decoder(Release1, sOP(066), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_ldc2,
                                  d->makeCp2Register(gR1(ib)),
                                  d->makeMemoryReference(addr, type_B64()));
    }
} mips32_ldc2;

// LDXC1 -- load doubleword indexed to floating point
static struct Mips32_ldxc1: Mips::Decoder {
    Mips32_ldxc1(): Mips::Decoder(Release2,
                           sOP(023)|sR2(000)|sFN(001),
                           mOP()   |mR2()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(insn_va, gR0(ib), gR1(ib));
        return d->makeInstruction(insn_va, mips_ldxc1,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeMemoryReference(addr, type_B64()));
    }
} mips32_ldxc1;

// LH -- load halfword
static struct Mips32_lh: Mips::Decoder {
    Mips32_lh(): Mips::Decoder(Release1, sOP(041), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lh,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I16()));
    }
} mips32_lh;

// LHE -- load halfword EVA
static struct Mips32_lhe: Mips::Decoder {
    Mips32_lhe(): Mips::Decoder(Release1,
                         sOP(037)|shift_to<6, 6>(1)|sFN(055),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_lhe,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I16()));
    }
} mips32_lhe;

// LHU -- load halfword unsigned
static struct Mips32_lhu: Mips::Decoder {
    Mips32_lhu(): Mips::Decoder(Release1, sOP(045), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lhu,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_U16()));
    }
} mips32_lhu;

// LHUE -- load halfword unsigned EVA
static struct Mips32_lhue: Mips::Decoder {
    Mips32_lhue(): Mips::Decoder(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(051),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_lhue,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_U16()));
    }
} mips32_lhue;

// LL -- load linked word
static struct Mips32_ll: Mips::Decoder {
    Mips32_ll(): Mips::Decoder(Release1, sOP(060), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_ll,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_ll;

// LLE -- load linked word EVA
static struct Mips32_lle: Mips::Decoder {
    Mips32_lle(): Mips::Decoder(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(056),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_lle,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lle;

// LUI -- load upper immediate
static struct Mips32_lui: Mips::Decoder {
    Mips32_lui(): Mips::Decoder(Release1,
                         sOP(017)|sR0(000),
                         mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_lui,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_lui;

// LUXC1 -- load doubleword indexed unaligned to floating point
static struct Mips32_luxc1: Mips::Decoder {
    Mips32_luxc1(): Mips::Decoder(Release2,
                           sOP(023)|sR2(000)|sFN(005),
                           mOP()   |mR2()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_luxc1,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeRegisterIndexed(insn_va, gR0(ib), gR1(ib)));
    }
} mips32_luxc1;

// LW -- load word
static struct Mips32_lw: Mips::Decoder {
    Mips32_lw(): Mips::Decoder(Release1, sOP(043), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lw,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lw;

// LWC1 -- load word to floating point
static struct Mips32_lwc1: Mips::Decoder {
    Mips32_lwc1(): Mips::Decoder(Release1, sOP(061), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lwc1,
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_lwc1;

// LWC2 -- load word to coprocessor 2
static struct Mips32_lwc2: Mips::Decoder {
    Mips32_lwc2(): Mips::Decoder(Release1, sOP(062), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lwc2,
                                  d->makeCp2Register(gR1(ib)),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_lwc2;

// LWE -- load word EVA
static struct Mips32_lwe: Mips::Decoder {
    Mips32_lwe(): Mips::Decoder(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(057),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_lwe,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwe;

// LWL -- load word left
static struct Mips32_lwl: Mips::Decoder {
    Mips32_lwl(): Mips::Decoder(Release1, sOP(042), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lwl,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwl;

// LWLE -- load word left EVA
static struct Mips32_lwle: Mips::Decoder {
    Mips32_lwle(): Mips::Decoder(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(031),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_lwle,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwle;

// LWR -- load word right
static struct Mips32_lwr: Mips::Decoder {
    Mips32_lwr(): Mips::Decoder(Release1, sOP(046), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lwr,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwr;

// LWRE -- load word right EVA
static struct Mips32_lwre: Mips::Decoder {
    Mips32_lwre(): Mips::Decoder(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(032),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_lwre,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwre;

// LWU -- load word unsigned
static struct Mips32_lwu: Mips::Decoder {
    Mips32_lwu(): Mips::Decoder(Release1, sOP(047), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_lwu,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_U32()));
    }
} mips32_lwu;

// LWXC1 -- load word indexed to floating point
static struct Mips32_lwxc1: Mips::Decoder {
    Mips32_lwxc1(): Mips::Decoder(Release2,
                           sOP(023)|sR2(000)|sFN(000),
                           mOP()   |mR2()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(insn_va, gR0(ib), gR1(ib));
        return d->makeInstruction(insn_va, mips_lwxc1,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_lwxc1;

// MADD -- multiply and add word to hi, lo
static struct Mips32_madd: Mips::Decoder {
    Mips32_madd(): Mips::Decoder(Release1,
                          sOP(034)|sR2(000)|sR3(000)|sFN(000),
                          mOP()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_madd,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_madd;

// MADD.S -- floating point multiply add
static struct Mips32_madd_s: Mips::Decoder {
    Mips32_madd_s(): Mips::Decoder(Release2,
                            sOP(023)|sFN(040),
                            mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_madd_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_madd_s;

// MADD.D -- floating point multiply add
static struct Mips32_madd_d: Mips::Decoder {
    Mips32_madd_d(): Mips::Decoder(Release2,
                            sOP(23)|sFN(041),
                            mOP()  |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_madd_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_madd_d;

// MADD.PS -- floating point multiply add
static struct Mips32_madd_ps: Mips::Decoder {
    Mips32_madd_ps(): Mips::Decoder(Release2,
                             sOP(23)|sFN(046),
                             mOP()  |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_madd_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_madd_ps;

// MADDU -- multiply and add unsigned word to hi, lo
static struct Mips32_maddu: Mips::Decoder {
    Mips32_maddu(): Mips::Decoder(Release1,
                           sOP(034)|sR2(000)|sR3(000)|sFN(001),
                           mOP()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_maddu,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_maddu;

// MFC0 -- move from coprocessor 0
static struct Mips32_mfc0: Mips::Decoder {
    Mips32_mfc0(): Mips::Decoder(Release1,
                          sOP(020)|sR0(000)|shift_to<3, 10>(0),
                          mOP()   |mR0()   |mask_for<3, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned sel = extract<0, 2>(ib);
        return d->makeInstruction(insn_va, mips_mfc0,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeCp0Register(insn_va, gR2(ib), sel),
                                  d->makeImmediate8(extract<0, 2>(ib), 0, 3));
    }
} mips32_mfc0;

// MFC1 -- move word from floating point
static struct Mips32_mfc1: Mips::Decoder {
    Mips32_mfc1(): Mips::Decoder(Release1,
                          sOP(021)|sR0(000)|shift_to<0, 10>(0),
                          mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mfc1,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_mfc1;

// MFC2 -- move word from coprocessor 2
static struct Mips32_mfc2: Mips::Decoder {
    Mips32_mfc2(): Mips::Decoder(Release1,
                          sOP(022)|sR0(000),
                          mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mfc2,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_mfc2;

// MFHC1 -- move word from high half of floating point register
static struct Mips32_mfhc1: Mips::Decoder {
    Mips32_mfhc1(): Mips::Decoder(Release2,
                           sOP(021)|sR0(003)|shift_to<0, 10>(0),
                           mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mfhc1,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_mfhc1;

// MFHC2 -- move word from high half of coprocessor 2 register
static struct Mips32_mfhc2: Mips::Decoder {
    Mips32_mfhc2(): Mips::Decoder(Release2,
                           sOP(022)|sR0(003),
                           mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mfhc2,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_mfhc2;

// MFHI -- move from hi register
static struct Mips32_mfhi: Mips::Decoder {
    Mips32_mfhi(): Mips::Decoder(Release1,
                          sOP(000)|sR0(000)|sR1(000)|sR3(000)|sFN(020),
                          mOP()   |mR0()   |mR1()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mfhi,
                                  d->makeRegister(insn_va, gR2(ib)));
    }
} mips32_mfhi;

// MFLO -- move from lo register
static struct Mips32_mflo: Mips::Decoder {
    Mips32_mflo(): Mips::Decoder(Release1,
                          sOP(000)|sR0(000)|sR1(000)|sR3(000)|sFN(022),
                          mOP()   |mR0()   |mR1()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mflo,
                                  d->makeRegister(insn_va, gR2(ib)));
    }
} mips32_mflo;

// MOV.S -- floating point move
static struct Mips32_mov_s: Mips::Decoder {
    Mips32_mov_s(): Mips::Decoder(Release1,
                           sOP(021)|sR0(020)|sR1(000)|sFN(006),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mov_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_mov_s;

// MOV.D -- floating point move
static struct Mips32_mov_d: Mips::Decoder {
    Mips32_mov_d(): Mips::Decoder(Release1,
                           sOP(021)|sR0(021)|sR1(000)|sFN(006),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mov_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_mov_d;

// MOV.PS -- floating point move
static struct Mips32_mov_ps: Mips::Decoder {
    Mips32_mov_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sR1(000)|sFN(006),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mov_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps));
    }
} mips32_mov_ps;

// MOVF -- move conditional on floating point false
static struct Mips32_movf: Mips::Decoder {
    Mips32_movf(): Mips::Decoder(Release1,
                          sOP(000)|shift_to<16, 17>(0)|sR3(000)|sFN(001),
                          mOP()   |mask_for<16, 17>() |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movf,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movf;

// MOVF.S -- floating point move conditional on floating point false
static struct Mips32_movf_s: Mips::Decoder {
    Mips32_movf_s(): Mips::Decoder(Release1,
                            sOP(021)|sR0(020)|shift_to<16, 17>(0)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movf_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movf_s;

// MOVF.D -- floating point move conditional on floating point false
static struct Mips32_movf_d: Mips::Decoder {
    Mips32_movf_d(): Mips::Decoder(Release1,
                            sOP(021)|sR0(021)|shift_to<16, 17>(0)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movf_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movf_d;

// MOVF.PS -- floating point move conditional on floating point false
static struct Mips32_movf_ps: Mips::Decoder {
    Mips32_movf_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|shift_to<16, 17>(0)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movf_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movf_ps;

// MOVN -- move conditional on not zero
static struct Mips32_movn: Mips::Decoder {
    Mips32_movn(): Mips::Decoder(Release1,
                          sOP(000)|sR3(000)|sFN(013),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movn,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_movn;

// MOVN.S -- move floating point conditional on not zero
static struct Mips32_movn_s: Mips::Decoder {
    Mips32_movn_s(): Mips::Decoder(Release1,
                            sOP(021)|sR0(020)|sFN(023),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movn_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movn_s;

// MOVN.D -- move floating point conditional on not zero
static struct Mips32_movn_d: Mips::Decoder {
    Mips32_movn_d(): Mips::Decoder(Release1,
                            sOP(021)|sR0(021)|sFN(023),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movn_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movn_d;

// MOVN.PS -- move floating point conditional on not zero
static struct Mips32_movn_ps: Mips::Decoder {
    Mips32_movn_ps(): Mips::Decoder(Release2,
                             sOP(021)|sR0(026)|sFN(023),
                             mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movn_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movn_ps;

// MOVT -- move conditional on floating point true
static struct Mips32_movt: Mips::Decoder {
    Mips32_movt(): Mips::Decoder(Release1,
                          sOP(000)|shift_to<16, 17>(1)|sR3(000)|sFN(001),
                          mOP()   |mask_for<16, 17>() |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movt,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movt;

// MOVT.S -- floating point move conditional on floating point true
static struct Mips32_movt_s: Mips::Decoder {
    Mips32_movt_s(): Mips::Decoder(Release1,
                            sOP(021)|sR0(020)|shift_to<16, 17>(1)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movt_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movt_s;

// MOVT.D -- floating point move conditional on floating point true
static struct Mips32_movt_d: Mips::Decoder {
    Mips32_movt_d(): Mips::Decoder(Release1,
                            sOP(021)|sR0(021)|shift_to<16, 17>(1)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movt_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movt_d;

// MOVT.PS -- floating point move conditional on floating point true
static struct Mips32_movt_ps: Mips::Decoder {
    Mips32_movt_ps(): Mips::Decoder(Release2,
                             sOP(021)|sR0(026)|shift_to<16, 17>(1)|sFN(021),
                             mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movt_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpccRegister(insn_va, extract<18, 20>(ib)));
    }
} mips32_movt_ps;

// MOVZ -- move conditional on zero
static struct Mips32_movz: Mips::Decoder {
    Mips32_movz(): Mips::Decoder(Release1,
                          sOP(000)|sR3(000)|sFN(012),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movz,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_movz;

// MOVZ.S -- floating point move conditional on zero
static struct Mips32_movz_s: Mips::Decoder {
    Mips32_movz_s(): Mips::Decoder(Release1,
                            sOP(021)|sR0(020)|sFN(022),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movz_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_movz_s;

// MOVZ.D -- floating point move conditional on zero
static struct Mips32_movz_d: Mips::Decoder {
    Mips32_movz_d(): Mips::Decoder(Release1,
                            sOP(021)|sR0(021)|sFN(022),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movz_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_movz_d;

// MOVZ.PS -- floating point move conditional on zero
static struct Mips32_movz_ps: Mips::Decoder {
    Mips32_movz_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sFN(022),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_movz_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_movz_ps;

// MSUB -- multiply and subtract word to hi, lo
static struct Mips32_msub: Mips::Decoder {
    Mips32_msub(): Mips::Decoder(Release1,
                          sOP(034)|sR2(000)|sR3(000)|sFN(004),
                          mOP()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_msub,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_msub;

// MSUB.S -- floating point multiply subtract
static struct Mips32_msub_s: Mips::Decoder {
    Mips32_msub_s(): Mips::Decoder(Release2,
                            sOP(023)|sFN(050),
                            mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_msub_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_msub_s;

// MSUB.D -- floating point multiply subtract
static struct Mips32_msub_d: Mips::Decoder {
    Mips32_msub_d(): Mips::Decoder(Release2,
                            sOP(023)|sFN(051),
                            mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_msub_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_msub_d;

// MSUB.PS -- floating point multiply subtract
static struct Mips32_msub_ps: Mips::Decoder {
    Mips32_msub_ps(): Mips::Decoder(Release2,
                             sOP(023)|sFN(056),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_msub_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_msub_ps;

// MSUBU -- multiply and subtract unsigned word to hi, lo
static struct Mips32_msubu: Mips::Decoder {
    Mips32_msubu(): Mips::Decoder(Release1,
                           sOP(034)|sR2(000)|sR3(000)|sFN(005),
                           mOP()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_msubu,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_msubu;

// MTC0 -- move to coprocessor 0
static struct Mips32_mtc0: Mips::Decoder {
    Mips32_mtc0(): Mips::Decoder(Release1,
                          sOP(020)|sR0(004)|shift_to<3, 10>(0),
                          mOP()   |mR0()   |mask_for<3, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned sel = extract<0, 2>(ib);
        return d->makeInstruction(insn_va, mips_mtc0,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeCp0Register(insn_va, gR2(ib), sel),
                                  d->makeImmediate8(extract<0, 2>(ib), 0, 3));
    }
} mips32_mtc0;

// MTC1 -- move word to floating point
static struct Mips32_mtc1: Mips::Decoder {
    Mips32_mtc1(): Mips::Decoder(Release1,
                          sOP(021)|sR0(004)|shift_to<0, 10>(0),
                          mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mtc1,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_mtc1;

// MTC2 -- move word to coprocessor 2
static struct Mips32_mtc2: Mips::Decoder {
    Mips32_mtc2(): Mips::Decoder(Release1,
                          sOP(022)|sR0(004),
                          mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mtc2,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_mtc2;

// MTHC1 -- move word to high half of floating point register
static struct Mips32_mthc1: Mips::Decoder {
    Mips32_mthc1(): Mips::Decoder(Release2,
                           sOP(021)|sR0(007)|shift_to<0, 10>(0),
                           mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mthc1,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_mthc1;

// MTHC2 -- move word to high half of coprocessor 2 register
static struct Mips32_mthc2: Mips::Decoder {
    Mips32_mthc2(): Mips::Decoder(Release2,
                           sOP(022)|sR0(007),
                           mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mthc2,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_mthc2;

// MTHI -- move to hi register
static struct Mips32_mthi: Mips::Decoder {
    Mips32_mthi(): Mips::Decoder(Release1,
                          sOP(000)|shift_to<6, 20>(0)|sFN(021),
                          mOP()   |mask_for<6, 20>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mthi,
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_mthi;

// MTLO -- move to lo register
static struct Mips32_mtlo: Mips::Decoder {
    Mips32_mtlo(): Mips::Decoder(Release1,
                          sOP(000)|shift_to<6, 20>(0)|sFN(023),
                          mOP()   |mask_for<6, 20>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mtlo,
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_mtlo;

// MUL -- multiply word to GPR
static struct Mips32_mul: Mips::Decoder {
    Mips32_mul(): Mips::Decoder(Release1,
                         sOP(034)|sR3(000)|sFN(002),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mul,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_mul;

// MUL.S -- floating point multiply
static struct Mips32_mul_s: Mips::Decoder {
    Mips32_mul_s(): Mips::Decoder(Release1,
                           sOP(021)|sR0(020)|sFN(002),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mul_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_mul_s;

// MUL.D -- floating point multiply
static struct Mips32_mul_d: Mips::Decoder {
    Mips32_mul_d(): Mips::Decoder(Release1,
                           sOP(021)|sR0(021)|sFN(002),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mul_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_mul_d;

// MUL.PS -- floating point multiply
static struct Mips32_mul_ps: Mips::Decoder {
    Mips32_mul_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sFN(002),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mul_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_mul_ps;

// MULT -- multiply word
static struct Mips32_mult: Mips::Decoder {
    Mips32_mult(): Mips::Decoder(Release1,
                          sOP(000)|shift_to<6, 15>(0)|sFN(030),
                          mOP()   |mask_for<6, 15>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_mult,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_mult;

// MULTU -- multiply unsigned word
static struct Mips32_multu: Mips::Decoder {
    Mips32_multu(): Mips::Decoder(Release1,
                           sOP(000)|shift_to<6, 15>(0)|sFN(031),
                           mOP()   |mask_for<6, 15>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_multu,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_multu;

// NEG.S -- floating point negate
static struct Mips32_neg_s: Mips::Decoder {
    Mips32_neg_s(): Mips::Decoder(Release1,
                           sOP(021)|sR0(020)|sR1(000)|sFN(007),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_neg_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_neg_s;

// NEG.D -- floating point negate
static struct Mips32_neg_d: Mips::Decoder {
    Mips32_neg_d(): Mips::Decoder(Release1,
                           sOP(021)|sR0(021)|sR1(000)|sFN(007),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_neg_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_neg_d;

// NEG.PS -- floating point negate
static struct Mips32_neg_ps: Mips::Decoder {
    Mips32_neg_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sR1(000)|sFN(007),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_neg_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps));
    }
} mips32_neg_ps;

// NMADD.S -- floating point negative multiply add
static struct Mips32_nmadd_s: Mips::Decoder {
    Mips32_nmadd_s(): Mips::Decoder(Release2,
                             sOP(023)|sFN(060),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_nmadd_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_nmadd_s;

// NMADD.D -- floating point negative multiply add
static struct Mips32_nmadd_d: Mips::Decoder {
    Mips32_nmadd_d(): Mips::Decoder(Release2,
                             sOP(023)|sFN(061),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_nmadd_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_nmadd_d;

// NMADD.PS -- floating point negative multiply add
static struct Mips32_nmadd_ps: Mips::Decoder {
    Mips32_nmadd_ps(): Mips::Decoder(Release2,
                              sOP(023)|sFN(066),
                              mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_nmadd_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_nmadd_ps;

// NMSUB.S -- floating point negative multiply subtract
static struct Mips32_nmsub_s: Mips::Decoder {
    Mips32_nmsub_s(): Mips::Decoder(Release2,
                             sOP(023)|sFN(070),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_nmsub_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_nmsub_s;

// NMSUB.D -- floating point negative multiply subtract
static struct Mips32_nmsub_d: Mips::Decoder {
    Mips32_nmsub_d(): Mips::Decoder(Release2,
                             sOP(023)|sFN(071),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_nmsub_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_nmsub_d;

// NMSUB.PS -- floating point negative multiply subtract
static struct Mips32_nmsub_ps: Mips::Decoder {
    Mips32_nmsub_ps(): Mips::Decoder(Release2,
                              sOP(023)|sFN(076),
                              mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_nmsub_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR0(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_nmsub_ps;

// NOR -- not or
static struct Mips32_nor: Mips::Decoder {
    Mips32_nor(): Mips::Decoder(Release1,
                         sOP(000)|sR3(000)|sFN(047),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_nor,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_nor;

// OR -- bitwise or
static struct Mips32_or: Mips::Decoder {
    Mips32_or(): Mips::Decoder(Release1,
                        sOP(000)|sR3(000)|sFN(045),
                        mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_or,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_or;

// ORI -- bitwise OR immediate
static struct Mips32_ori: Mips::Decoder {
    Mips32_ori(): Mips::Decoder(Release1, sOP(015), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_ori,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_ori;

// PAUSE -- wait for the LLBit to clear
static struct Mips32_pause: Mips::Decoder {
    Mips32_pause(): Mips::Decoder(Release2,
                           sOP(000)|sR0(000)|sR1(000)|sR2(000)|sR3(005)|sFN(000),
                           mOP()   |mR0()   |mR1()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned) {
        return d->makeInstruction(insn_va, mips_pause);
    }
} mips32_pause;

// PLL.PS -- pair lower lower
static struct Mips32_pll_ps: Mips::Decoder {
    Mips32_pll_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sFN(054),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_pll_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_pll_ps;

// PLU.PS -- pair lower upper
static struct Mips32_plu_ps: Mips::Decoder {
    Mips32_plu_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sFN(055),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_plu_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_plu_ps;

// PREF -- prefetch
static struct Mips32_pref: Mips::Decoder {
    Mips32_pref(): Mips::Decoder(Release1, sOP(063), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_pref,
                                  d->makeImmediate8(gR1(ib), 16, 5),
                                  d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib)));
    }
} mips32_pref;

// PREFE -- prefetch EVA
static struct Mips32_prefe: Mips::Decoder {
    Mips32_prefe(): Mips::Decoder(Release1,
                           sOP(037)|shift_to<6, 6>(0)|sFN(043),
                           mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        return d->makeInstruction(insn_va, mips_prefe,
                                  d->makeImmediate8(gR1(ib), 16, 5),
                                  d->makeRegisterOffset(insn_va, gR0(ib), offset16));
    }
} mips32_prefe;

// PREFX -- prefetch indexed
static struct Mips32_prefx: Mips::Decoder {
    Mips32_prefx(): Mips::Decoder(Release2,
                           sOP(023)|sR3(000)|sFN(017),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_prefx,
                                  d->makeImmediate8(gR2(ib), 11, 5),
                                  d->makeRegisterIndexed(insn_va, gR0(ib), gR1(ib)));
    }
} mips32_prefx;

// PUL.PS -- pair upper lower
static struct Mips32_pul_ps: Mips::Decoder {
    Mips32_pul_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sFN(056),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_pul_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_pul_ps;

// PUU.PS -- pair upper upper
static struct Mips32_puu_ps: Mips::Decoder {
    Mips32_puu_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sFN(057),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_puu_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_puu_ps;

// RDHWR -- read hardware register
static struct Mips32_rdhwr: Mips::Decoder {
    Mips32_rdhwr(): Mips::Decoder(Release2,
                           sOP(037)|sR0(000)|sR3(000)|sFN(073),
                           mOP()   |mR0()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_rdhwr,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeHwRegister(gR2(ib)));
    }
} mips32_rdhwr;

// RDPGPR -- read GPR from previous shadow set
static struct Mips32_rdpgpr: Mips::Decoder {
    Mips32_rdpgpr(): Mips::Decoder(Release2,
                            sOP(020)|sR0(012)|shift_to<0, 10>(0),
                            mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_rdpgpr,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeShadowRegister(insn_va, gR1(ib)));
    }
} mips32_rdpgpr;

// RECIP.S -- reciprocal approximation
static struct Mips32_recip_s: Mips::Decoder {
    Mips32_recip_s(): Mips::Decoder(Release2,
                             sOP(021)|sR0(020)|sR1(000)|sFN(025),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_recip_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_recip_s;

// RECIP.D -- reciprocal approximation
static struct Mips32_recip_d: Mips::Decoder {
    Mips32_recip_d(): Mips::Decoder(Release2,
                             sOP(021)|sR0(021)|sR1(000)|sFN(025),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_recip_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_recip_d;

// ROTR -- rotate word right
static struct Mips32_rotr: Mips::Decoder {
    Mips32_rotr(): Mips::Decoder(Release2,
                          sOP(000)|sR0(001)|sFN(002),
                          mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_rotr,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_rotr;

// ROTRV -- rotate word right variable
static struct Mips32_rotrv: Mips::Decoder {
    Mips32_rotrv(): Mips::Decoder(Release2,
                           sOP(000)|sR3(001)|sFN(006),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_rotrv,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_rotrv;

// ROUND.L.S -- floating point round to long fixed point
static struct Mips32_round_l_s: Mips::Decoder {
    Mips32_round_l_s(): Mips::Decoder(Release2,
                               sOP(021)|sR0(020)|sR1(000)|sFN(010),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_round_l_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_round_l_s;

// ROUND.L.D -- floating point round to long fixed point
static struct Mips32_round_l_d: Mips::Decoder {
    Mips32_round_l_d(): Mips::Decoder(Release2,
                               sOP(021)|sR0(021)|sR1(000)|sFN(010),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_round_l_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_round_l_d;

// ROUND.W.S -- floating point round to word fixed point
static struct Mips32_round_w_s: Mips::Decoder {
    Mips32_round_w_s(): Mips::Decoder(Release1,
                               sOP(021)|sR0(020)|sR1(000)|sFN(014),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_round_w_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_round_w_s;

// ROUND.W.D -- floating point round to word fixed point
static struct Mips32_round_w_d: Mips::Decoder {
    Mips32_round_w_d(): Mips::Decoder(Release1,
                               sOP(021)|sR0(021)|sR1(000)|sFN(014),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_round_w_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_round_w_d;

// RSQRT.S -- reciprocal square root approximation
static struct Mips32_rsqrt_s: Mips::Decoder {
    Mips32_rsqrt_s(): Mips::Decoder(Release2,
                             sOP(021)|sR0(020)|sR1(000)|sFN(026),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_rsqrt_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_rsqrt_s;

// RSQRT.D -- reciprocal square root approximation
static struct Mips32_rsqrt_d: Mips::Decoder {
    Mips32_rsqrt_d(): Mips::Decoder(Release2,
                             sOP(021)|sR0(021)|sR1(000)|sFN(026),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_rsqrt_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_rsqrt_d;

// SB -- store byte
static struct Mips32_sb: Mips::Decoder {
    Mips32_sb(): Mips::Decoder(Release1, sOP(050), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_sb,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B8()));
    }
} mips32_sb;

// SBE -- store byte EVA
static struct Mips32_sbe: Mips::Decoder {
    Mips32_sbe(): Mips::Decoder(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(034),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_sbe,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B8()));
    }
} mips32_sbe;

// SC -- store conditional word
static struct Mips32_sc: Mips::Decoder {
    Mips32_sc(): Mips::Decoder(Release1, sOP(070), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_sc,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_sc;

// SCE -- store conditional word EVA
static struct Mips32_sce: Mips::Decoder {
    Mips32_sce(): Mips::Decoder(Release1,
                         sOP(047)|shift_to<6, 6>(0)|sFN(036),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_sce,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_I32()));
    }
} mips32_sce;

// SDC1 -- store doubleword from floating point
static struct Mips32_sdc1: Mips::Decoder {
    Mips32_sdc1(): Mips::Decoder(Release1, sOP(075), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_sdc1,
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d),
                                  d->makeMemoryReference(addr, type_B64()));
    }
} mips32_sdc1;

// SDC2 -- store doubleword from coprocessor 2
static struct Mips32_sdc2: Mips::Decoder {
    Mips32_sdc2(): Mips::Decoder(Release1, sOP(076), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_sdc2,
                                  d->makeCp2Register(gR1(ib)),
                                  d->makeMemoryReference(addr, type_B64()));
    }
} mips32_sdc2;

// SDXC1 -- store doubleword indexed from floating point
static struct Mips32_sdxc1: Mips::Decoder {
    Mips32_sdxc1(): Mips::Decoder(Release2,
                           sOP(023)|sR3(000)|sFN(011),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(insn_va, gR0(ib), gR1(ib));
        return d->makeInstruction(insn_va, mips_sdxc1,
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeMemoryReference(addr, type_B64()));
    }
} mips32_sdxc1;

// SEB -- sign extend byte
static struct Mips32_seb: Mips::Decoder {
    Mips32_seb(): Mips::Decoder(Release2,
                         sOP(037)|sR0(000)|sR3(020)|sFN(040),
                         mOP()   |mR0()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_seb,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_seb;

// SEH -- sign extend halfword
static struct Mips32_seh: Mips::Decoder {
    Mips32_seh(): Mips::Decoder(Release2,
                         sOP(037)|sR0(000)|sR3(030)|sFN(040),
                         mOP()   |mR0()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_seh,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_seh;

// SH -- store halfword
static struct Mips32_sh: Mips::Decoder {
    Mips32_sh(): Mips::Decoder(Release1, sOP(051), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_sh,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B16()));
    }
} mips32_sh;

// SHE -- store halfword EVA
static struct Mips32_she: Mips::Decoder {
    Mips32_she(): Mips::Decoder(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(035),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_she,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B16()));
    }
} mips32_she;

// SLL -- shift word left logical
// NOP -- no operation (when "SSL r0, r0, 0"); an assembly macro
// SSNOP -- superscalar no operation (when "SLL r0, r0, 1")
// EHB -- execution hazard barrier (when "SLL r0, r0, 3")
static struct Mips32_sll: Mips::Decoder {
    Mips32_sll(): Mips::Decoder(Release1,
                         sOP(000)|sR0(000)|sFN(000),
                         mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        if (gR1(ib)==0 && gR2(ib)==0 && gR3(ib)==0)
            return d->makeInstruction(insn_va, mips_nop);
        if (gR1(ib)==0 && gR2(ib)==0 && gR3(ib)==1)
            return d->makeInstruction(insn_va, mips_ssnop);
        if (gR1(ib)==0 && gR2(ib)==0 && gR3(ib)==3)
            return d->makeInstruction(insn_va, mips_ehb);
        return d->makeInstruction(insn_va, mips_sll,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_sll;

// SLLV -- shift word left logical variable
static struct Mips32_sllv: Mips::Decoder {
    Mips32_sllv(): Mips::Decoder(Release1,
                          sOP(000)|sR3(000)|sFN(004),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sllv,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_sllv;

// SLT -- set on less than
static struct Mips32_slt: Mips::Decoder {
    Mips32_slt(): Mips::Decoder(Release1,
                         sOP(000)|sR3(000)|sFN(052),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_slt,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_slt;

// SLTI -- set on less than immediate
static struct Mips32_slti: Mips::Decoder {
    Mips32_slti(): Mips::Decoder(Release1, sOP(012), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_slti,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_slti;

// SLTIU -- set on less than immediate unsigned
static struct Mips32_sltiu: Mips::Decoder {
    Mips32_sltiu(): Mips::Decoder(Release1, sOP(013), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sltiu,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_sltiu;

// SLTU -- set on less than unsigned
static struct Mips32_sltu: Mips::Decoder {
    Mips32_sltu(): Mips::Decoder(Release1,
                          sOP(000)|sR3(000)|sFN(053),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sltu,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_sltu;

// SQRT.S -- floating point square root
static struct Mips32_sqrt_s: Mips::Decoder {
    Mips32_sqrt_s(): Mips::Decoder(Release1,
                            sOP(021)|sR0(020)|sR1(000)|sFN(004),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sqrt_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_sqrt_s;

// SQRT.D -- floating point square root
static struct Mips32_sqrt_d: Mips::Decoder {
    Mips32_sqrt_d(): Mips::Decoder(Release1,
                            sOP(021)|sR0(021)|sR1(000)|sFN(004),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sqrt_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_sqrt_d;

// SRA -- shift word right arithmetic
static struct Mips32_sra: Mips::Decoder {
    Mips32_sra(): Mips::Decoder(Release1,
                         sOP(000)|sR0(000)|sFN(003),
                         mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sra,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_sra;

// SRAV -- shift word right arithmetic variable
static struct Mips32_srav: Mips::Decoder {
    Mips32_srav(): Mips::Decoder(Release1,
                          sOP(000)|sR3(000)|sFN(007),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_srav,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_srav;

// SRL -- shift word right logical
static struct Mips32_srl: Mips::Decoder {
    Mips32_srl(): Mips::Decoder(Release1,
                         sOP(000)|sR0(000)|sFN(002),
                         mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_srl,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_srl;

// SRLV -- shift word right logical variable
static struct Mips32_srlv: Mips::Decoder {
    Mips32_srlv(): Mips::Decoder(Release1,
                          sOP(000)|sR3(000)|sFN(006),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_srlv,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)));
    }
} mips32_srlv;

// SUB -- subtract word
static struct Mips32_sub: Mips::Decoder {
    Mips32_sub(): Mips::Decoder(Release1,
                         sOP(000)|sR3(000)|sFN(042),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sub,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_sub;

// SUB.S -- subtract floating point
static struct Mips32_sub_s: Mips::Decoder {
    Mips32_sub_s(): Mips::Decoder(Release1,
                           sOP(021)|sR0(020)|sFN(001),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sub_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s));
    }
} mips32_sub_s;

// SUB.D -- subtract floating point
static struct Mips32_sub_d: Mips::Decoder {
    Mips32_sub_d(): Mips::Decoder(Release1,
                           sOP(021)|sR0(021)|sFN(001),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sub_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_d));
    }
} mips32_sub_d;

// SUB.PS -- subtract floating point
static struct Mips32_sub_ps: Mips::Decoder {
    Mips32_sub_ps(): Mips::Decoder(Release2,
                            sOP(021)|sR0(026)|sFN(001),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sub_ps,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_ps),
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_ps));
    }
} mips32_sub_ps;

// SUBU -- subtract unsigned word
static struct Mips32_subu: Mips::Decoder {
    Mips32_subu(): Mips::Decoder(Release1,
                          sOP(000)|sR3(000)|sFN(043),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_subu,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_subu;

// SUXC1 -- store doubleword indexed unaligned from floating point
static struct Mips32_suxc1: Mips::Decoder {
    Mips32_suxc1(): Mips::Decoder(Release2,
                           sOP(023)|sR3(000)|sFN(015),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(insn_va, gR0(ib), gR1(ib));
        return d->makeInstruction(insn_va, mips_suxc1,
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d),
                                  d->makeMemoryReference(addr, type_B64()));
    }
} mips32_suxc1;

// SW -- store word
static struct Mips32_sw: Mips::Decoder {
    Mips32_sw(): Mips::Decoder(Release1, sOP(053), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_sw,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_sw;

// SWC1 -- store word from floating point
static struct Mips32_swc1: Mips::Decoder {
    Mips32_swc1(): Mips::Decoder(Release1, sOP(071), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_swc1,
                                  d->makeFpRegister(insn_va, gR1(ib), mips_fmt_s),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swc1;

// SWC2 -- store word from coprocessor 2
static struct Mips32_swc2: Mips::Decoder {
    Mips32_swc2(): Mips::Decoder(Release1, sOP(072), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_swc2,
                                  d->makeCp2Register(gR1(ib)),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swc2;

// SWE -- store word EVA
static struct Mips32_swe: Mips::Decoder {
    Mips32_swe(): Mips::Decoder(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(037),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_swe,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swe;

// SWL -- store word left
static struct Mips32_swl: Mips::Decoder {
    Mips32_swl(): Mips::Decoder(Release1, sOP(052), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_swl,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swl;

// SWLE -- store word left EVA
static struct Mips32_swle: Mips::Decoder {
    Mips32_swle(): Mips::Decoder(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(041),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_swle,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swle;

// SWR -- store word right
static struct Mips32_swr: Mips::Decoder {
    Mips32_swr(): Mips::Decoder(Release1, sOP(056), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib));
        return d->makeInstruction(insn_va, mips_swr,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swr;

// SWRE -- store word right EVA
static struct Mips32_swre: Mips::Decoder {
    Mips32_swre(): Mips::Decoder(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(042),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(insn_va, gR0(ib), offset16);
        return d->makeInstruction(insn_va, mips_swre,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swre;

// SWXC1 -- store word indexed from floating point
static struct Mips32_swxc1: Mips::Decoder {
    Mips32_swxc1(): Mips::Decoder(Release2,
                           sOP(023)|sR3(000)|sFN(010),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(insn_va, gR0(ib), gR1(ib));
        return d->makeInstruction(insn_va, mips_swxc1,
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s),
                                  d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swxc1;

// SYNC -- synchronize
// The "SYNC" form is the same as "SYNC stype" when stype==0
static struct Mips32_sync: Mips::Decoder {
    Mips32_sync(): Mips::Decoder(Release1,
                          sOP(000)|shift_to<11, 25>(0)|sFN(017),
                          mOP()   |mask_for<11, 25>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_sync,
                                  d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_sync;

// SYNCI -- synchronize caches to make instruction writes effective
static struct Mips32_synci: Mips::Decoder {
    Mips32_synci(): Mips::Decoder(Release2,
                           sOP(001)|sR1(037),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_synci,
                                  d->makeRegisterOffset(insn_va, gR0(ib), gIM(ib)));
    }
} mips32_synci;

// SYSCALL -- system call
static struct Mips32_syscall: Mips::Decoder {
    Mips32_syscall(): Mips::Decoder(Release1,
                             sOP(000)|sFN(014),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 25>(ib);
        return d->makeInstruction(insn_va, mips_syscall);
    }
} mips32_syscall;

// TEQ -- trap if equal
static struct Mips32_teq: Mips::Decoder {
    Mips32_teq(): Mips::Decoder(Release1,
                         sOP(000)|sFN(064),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(insn_va, mips_teq,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_teq;

// TEQI -- trap if equal immediate
static struct Mips32_teqi: Mips::Decoder {
    Mips32_teqi(): Mips::Decoder(Release1,
                          sOP(001)|sR1(014),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_teqi,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_teqi;

// TGE -- trap if greater or equal
static struct Mips32_tge: Mips::Decoder {
    Mips32_tge(): Mips::Decoder(Release1,
                         sOP(000)|sFN(060),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(insn_va, mips_tge,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_tge;

// TGEI -- trap greater or equal immediate
static struct Mips32_tgei: Mips::Decoder {
    Mips32_tgei(): Mips::Decoder(Release1,
                          sOP(001)|sR1(010),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_tgei,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tgei;

// TGEIU -- trap greater or equal immediate unsigned
static struct Mips32_tgeiu: Mips::Decoder {
    Mips32_tgeiu(): Mips::Decoder(Release1,
                           sOP(001)|sR1(011),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_tgeiu,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tgeiu;

// TGEU -- trap greater or equal unsigned
static struct Mips32_tgeu: Mips::Decoder {
    Mips32_tgeu(): Mips::Decoder(Release1,
                          sOP(000)|sFN(061),
                          mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(insn_va, mips_tgeu,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_tgeu;

// TLBINV -- TLB invalidate
static struct Mips32_tlbinv: Mips::Decoder {
    Mips32_tlbinv(): Mips::Decoder(Release1,
                            sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(003),
                            mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned) {
        return d->makeInstruction(insn_va, mips_tlbinv);
    }
} mips32_tlbinv;

// TLBINVF -- TLB invalidate flush
static struct Mips32_tlbinvf: Mips::Decoder {
    Mips32_tlbinvf(): Mips::Decoder(Release1,
                             sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(004),
                             mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned) {
        return d->makeInstruction(insn_va, mips_tlbinvf);
    }
} mips32_tlbinvf;

// TLBP -- probe TLB for matching entry
static struct Mips32_tlbp: Mips::Decoder {
    Mips32_tlbp(): Mips::Decoder(Release1,
                          sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(010),
                          mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned) {
        return d->makeInstruction(insn_va, mips_tlbp);
    }
} mips32_tlbp;

// TLBR -- read indexed TLB entry
static struct Mips32_tlbr: Mips::Decoder {
    Mips32_tlbr(): Mips::Decoder(Release1,
                          sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(001),
                          mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned) {
        return d->makeInstruction(insn_va, mips_tlbr);
    }
} mips32_tlbr;

// TLBWI -- write indexed TLB entry
static struct Mips32_tlbwi: Mips::Decoder {
    Mips32_tlbwi(): Mips::Decoder(Release1,
                           sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(002),
                           mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned) {
        return d->makeInstruction(insn_va, mips_tlbwi);
    }
} mips32_tlbwi;

// TLBWR -- write random TLB entry
static struct Mips32_tlbwr: Mips::Decoder {
    Mips32_tlbwr(): Mips::Decoder(Release1,
                           sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(006),
                           mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned) {
        return d->makeInstruction(insn_va, mips_tlbwr);
    }
} mips32_tlbwr;

// TLT -- trap if less than
static struct Mips32_tlt: Mips::Decoder {
    Mips32_tlt(): Mips::Decoder(Release1,
                         sOP(000)|sFN(062),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(insn_va, mips_tlt,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_tlt;

// TLTI -- trap less than immediate
static struct Mips32_tlti: Mips::Decoder {
    Mips32_tlti(): Mips::Decoder(Release1,
                          sOP(001)|sR1(012),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_tlti,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tlti;

// TLTIU -- trap less than immediate unsigned
static struct Mips32_tltiu: Mips::Decoder {
    Mips32_tltiu(): Mips::Decoder(Release1,
                           sOP(001)|sR1(013),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_tltiu,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tltiu;

// TLTU -- trap if less than unsigned
static struct Mips32_tltu: Mips::Decoder {
    Mips32_tltu(): Mips::Decoder(Release1,
                          sOP(000)|sFN(063),
                          mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(insn_va, mips_tltu,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_tltu;

// TNE -- trap if not equal
static struct Mips32_tne: Mips::Decoder {
    Mips32_tne(): Mips::Decoder(Release1,
                         sOP(000)|sFN(066),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(insn_va, mips_tne,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_tne;

// TNEI -- trap if not equal immediate
static struct Mips32_tnei: Mips::Decoder {
    Mips32_tnei(): Mips::Decoder(Release1,
                          sOP(001)|sR1(016),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_tnei,
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tnei;

// TRUNC.L.S -- floating point truncate to long fixed point
static struct Mips32_trunc_l_s: Mips::Decoder {
    Mips32_trunc_l_s(): Mips::Decoder(Release2,
                               sOP(021)|sR0(020)|sR1(000)|sFN(011),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_trunc_l_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_trunc_l_s;

// TRUNC.L.D -- floating point truncate to long fixed point
static struct Mips32_trunc_l_d: Mips::Decoder {
    Mips32_trunc_l_d(): Mips::Decoder(Release2,
                               sOP(021)|sR0(021)|sR1(000)|sFN(011),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_trunc_l_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_l),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_trunc_l_d;

// TRUNC.W.S -- floating point truncate to word fixed point
static struct Mips32_trunc_w_s: Mips::Decoder {
    Mips32_trunc_w_s(): Mips::Decoder(Release1,
                               sOP(021)|sR0(020)|sR1(000)|sFN(015),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_trunc_w_s,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_s));
    }
} mips32_trunc_w_s;

// TRUNC.W.D -- floating point truncate to word fixed point
static struct Mips32_trunc_w_d: Mips::Decoder {
    Mips32_trunc_w_d(): Mips::Decoder(Release1,
                               sOP(021)|sR0(021)|sR1(000)|sFN(015),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_trunc_w_d,
                                  d->makeFpRegister(insn_va, gR3(ib), mips_fmt_w),
                                  d->makeFpRegister(insn_va, gR2(ib), mips_fmt_d));
    }
} mips32_trunc_w_d;

// WAIT -- enter standby mode
static struct Mips32_wait: Mips::Decoder {
    Mips32_wait(): Mips::Decoder(Release1,
                          sOP(020)|shift_to<25, 25>(1)|sFN(040),
                          mOP()   |mask_for<25, 25>() |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        unsigned impl UNUSED_VAR = extract<6, 24>(ib);
        return d->makeInstruction(insn_va, mips_wait);
    }
} mips32_wait;

// WRPGPR -- write to  GPR in previous shadow set
static struct Mips32_wrpgpr: Mips::Decoder {
    Mips32_wrpgpr(): Mips::Decoder(Release2,
                            sOP(020)|sR0(016)|shift_to<0, 10>(0),
                            mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_wrpgpr,
                                  d->makeShadowRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_wrpgpr;

// WSBH -- word swap bytes within halfwords
static struct Mips32_wsbh: Mips::Decoder {
    Mips32_wsbh(): Mips::Decoder(Release2,
                          sOP(037)|sR0(000)|sR3(002)|sFN(040),
                          mOP()   |mR0()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_wsbh,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_wsbh;

// XOR -- exclusive OR
static struct Mips32_xor: Mips::Decoder {
    Mips32_xor(): Mips::Decoder(Release1,
                         sOP(000)|sR3(000)|sFN(046),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_xor,
                                  d->makeRegister(insn_va, gR2(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeRegister(insn_va, gR1(ib)));
    }
} mips32_xor;

// XORI -- exclusive OR immediate
static struct Mips32_xori: Mips::Decoder {
    Mips32_xori(): Mips::Decoder(Release1, sOP(016), mOP()) {}
    SgAsmMipsInstruction *operator()(Address insn_va, const D *d, unsigned ib) {
        return d->makeInstruction(insn_va, mips_xori,
                                  d->makeRegister(insn_va, gR1(ib)),
                                  d->makeRegister(insn_va, gR0(ib)),
                                  d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_xori;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
Mips::init()
{
    REG_IP = architecture()->registerDictionary()->instructionPointerRegister();
    REG_SP = architecture()->registerDictionary()->stackPointerRegister();
    ASSERT_require(REG_IP);
    ASSERT_require(REG_SP);

    instructionAlignment_ = 4;

    insert_idis(&mips32_abs_s);
    insert_idis(&mips32_abs_d);
    insert_idis(&mips32_abs_ps);
    insert_idis(&mips32_add);
    insert_idis(&mips32_add_s);
    insert_idis(&mips32_add_d);
    insert_idis(&mips32_add_ps);
    insert_idis(&mips32_addi);
    insert_idis(&mips32_addiu);
    insert_idis(&mips32_addu);
    insert_idis(&mips32_alnv_ps);
    insert_idis(&mips32_and);
    insert_idis(&mips32_andi);
    insert_idis(&mips32_bc1f);
    insert_idis(&mips32_bc1fl);
    insert_idis(&mips32_bc1t);
    insert_idis(&mips32_bc1tl);
    insert_idis(&mips32_bc2f);
    insert_idis(&mips32_bc2fl);
    insert_idis(&mips32_bc2t);
    insert_idis(&mips32_bc2tl);
    insert_idis(&mips32_beq);
    insert_idis(&mips32_beql);
    insert_idis(&mips32_bgez);
    insert_idis(&mips32_bgezal);
    insert_idis(&mips32_bgezall);
    insert_idis(&mips32_bgezl);
    insert_idis(&mips32_bgtz);
    insert_idis(&mips32_bgtzl);
    insert_idis(&mips32_blez);
    insert_idis(&mips32_blezl);
    insert_idis(&mips32_bltz);
    insert_idis(&mips32_bltzal);
    insert_idis(&mips32_bltzall);
    insert_idis(&mips32_bltzl);
    insert_idis(&mips32_bne);
    insert_idis(&mips32_bnel);
    insert_idis(&mips32_c_cond_s);
    insert_idis(&mips32_c_cond_d);
    insert_idis(&mips32_c_cond_ps);
    insert_idis(&mips32_cache);
    insert_idis(&mips32_cachee);
    insert_idis(&mips32_ceil_l_s);
    insert_idis(&mips32_ceil_l_d);
    insert_idis(&mips32_ceil_w_s);
    insert_idis(&mips32_ceil_w_d);
    insert_idis(&mips32_cfc1);
    insert_idis(&mips32_cfc2);
    insert_idis(&mips32_clo);
    insert_idis(&mips32_clz);
    insert_idis(&mips32_cop2);
    insert_idis(&mips32_ctc1);
    insert_idis(&mips32_ctc2);
    insert_idis(&mips32_cvt_d_s);
    insert_idis(&mips32_cvt_d_w);
    insert_idis(&mips32_cvt_d_l);
    insert_idis(&mips32_cvt_l_s);
    insert_idis(&mips32_cvt_l_d);
    insert_idis(&mips32_cvt_ps_s);
    insert_idis(&mips32_cvt_s_w);
    insert_idis(&mips32_cvt_s_l);
    insert_idis(&mips32_cvt_s_pl);
    insert_idis(&mips32_cvt_s_pu);
    insert_idis(&mips32_cvt_w_s);
    insert_idis(&mips32_cvt_w_d);
    insert_idis(&mips32_di);
    insert_idis(&mips32_div);
    insert_idis(&mips32_div_s);
    insert_idis(&mips32_div_d);
    insert_idis(&mips32_divu);
    insert_idis(&mips32_ei);
    insert_idis(&mips32_eret);
    insert_idis(&mips32_ext);
    insert_idis(&mips32_floor_l_s);
    insert_idis(&mips32_floor_l_d);
    insert_idis(&mips32_floor_w_s);
    insert_idis(&mips32_floor_w_d);
    insert_idis(&mips32_ins);
    insert_idis(&mips32_j);
    insert_idis(&mips32_jal);
    insert_idis(&mips32_jalr);
    insert_idis(&mips32_jalr_hb);
    insert_idis(&mips32_jalx);
    insert_idis(&mips32_jr);
    insert_idis(&mips32_jr_hb);
    insert_idis(&mips32_lb);
    insert_idis(&mips32_lbe);
    insert_idis(&mips32_lbu);
    insert_idis(&mips32_lbue);
    insert_idis(&mips32_ldc1);
    insert_idis(&mips32_ldc2);
    insert_idis(&mips32_ldxc1);
    insert_idis(&mips32_lh);
    insert_idis(&mips32_lhe);
    insert_idis(&mips32_lhu);
    insert_idis(&mips32_lhue);
    insert_idis(&mips32_ll);
    insert_idis(&mips32_lle);
    insert_idis(&mips32_lui);
    insert_idis(&mips32_luxc1);
    insert_idis(&mips32_lw);
    insert_idis(&mips32_lwc1);
    insert_idis(&mips32_lwc2);
    insert_idis(&mips32_lwe);
    insert_idis(&mips32_lwl);
    insert_idis(&mips32_lwle);
    insert_idis(&mips32_lwr);
    insert_idis(&mips32_lwre);
    insert_idis(&mips32_lwu);
    insert_idis(&mips32_lwxc1);
    insert_idis(&mips32_madd);
    insert_idis(&mips32_madd_s);
    insert_idis(&mips32_madd_d);
    insert_idis(&mips32_madd_ps);
    insert_idis(&mips32_maddu);
    insert_idis(&mips32_mfc0);
    insert_idis(&mips32_mfc1);
    insert_idis(&mips32_mfc2);
    insert_idis(&mips32_mfhc1);
    insert_idis(&mips32_mfhc2);
    insert_idis(&mips32_mfhi);
    insert_idis(&mips32_mflo);
    insert_idis(&mips32_mov_s);
    insert_idis(&mips32_mov_d);
    insert_idis(&mips32_mov_ps);
    insert_idis(&mips32_movf);
    insert_idis(&mips32_movf_s);
    insert_idis(&mips32_movf_d);
    insert_idis(&mips32_movf_ps);
    insert_idis(&mips32_movn);
    insert_idis(&mips32_movn_s);
    insert_idis(&mips32_movn_d);
    insert_idis(&mips32_movn_ps);
    insert_idis(&mips32_movt);
    insert_idis(&mips32_movt_s);
    insert_idis(&mips32_movt_d);
    insert_idis(&mips32_movt_ps);
    insert_idis(&mips32_movz);
    insert_idis(&mips32_movz_s);
    insert_idis(&mips32_movz_d);
    insert_idis(&mips32_movz_ps);
    insert_idis(&mips32_msub);
    insert_idis(&mips32_msub_s);
    insert_idis(&mips32_msub_d);
    insert_idis(&mips32_msub_ps);
    insert_idis(&mips32_msubu);
    insert_idis(&mips32_mtc0);
    insert_idis(&mips32_mtc1);
    insert_idis(&mips32_mtc2);
    insert_idis(&mips32_mthi);
    insert_idis(&mips32_mtlo);
    insert_idis(&mips32_mul);
    insert_idis(&mips32_mul_s);
    insert_idis(&mips32_mul_d);
    insert_idis(&mips32_mul_ps);
    insert_idis(&mips32_mult);
    insert_idis(&mips32_multu);
    insert_idis(&mips32_neg_s);
    insert_idis(&mips32_neg_d);
    insert_idis(&mips32_neg_ps);
    insert_idis(&mips32_nmadd_s);
    insert_idis(&mips32_nmadd_d);
    insert_idis(&mips32_nmadd_ps);
    insert_idis(&mips32_nmsub_s);
    insert_idis(&mips32_nmsub_d);
    insert_idis(&mips32_nmsub_ps);
    insert_idis(&mips32_nor);
    insert_idis(&mips32_or);
    insert_idis(&mips32_ori);
    insert_idis(&mips32_pause);
    insert_idis(&mips32_pll_ps);
    insert_idis(&mips32_plu_ps);
    insert_idis(&mips32_pref);
    insert_idis(&mips32_prefe);
    insert_idis(&mips32_prefx);
    insert_idis(&mips32_pul_ps);
    insert_idis(&mips32_puu_ps);
    insert_idis(&mips32_rdhwr);
    insert_idis(&mips32_rdpgpr);
    insert_idis(&mips32_recip_s);
    insert_idis(&mips32_recip_d);
    insert_idis(&mips32_rotr);
    insert_idis(&mips32_rotrv);
    insert_idis(&mips32_round_l_s);
    insert_idis(&mips32_round_l_d);
    insert_idis(&mips32_round_w_s);
    insert_idis(&mips32_round_w_d);
    insert_idis(&mips32_rsqrt_s);
    insert_idis(&mips32_rsqrt_d);
    insert_idis(&mips32_sb);
    insert_idis(&mips32_sbe);
    insert_idis(&mips32_sc);
    insert_idis(&mips32_sce);
    insert_idis(&mips32_sdc1);
    insert_idis(&mips32_sdc2);
    insert_idis(&mips32_sdxc1);
    insert_idis(&mips32_seb);
    insert_idis(&mips32_seh);
    insert_idis(&mips32_sh);
    insert_idis(&mips32_she);
    insert_idis(&mips32_sll);
    insert_idis(&mips32_sllv);
    insert_idis(&mips32_slt);
    insert_idis(&mips32_slti);
    insert_idis(&mips32_sltiu);
    insert_idis(&mips32_sltu);
    insert_idis(&mips32_sqrt_s);
    insert_idis(&mips32_sqrt_d);
    insert_idis(&mips32_sra);
    insert_idis(&mips32_srav);
    insert_idis(&mips32_srl);
    insert_idis(&mips32_srlv);
    insert_idis(&mips32_sub);
    insert_idis(&mips32_sub_s);
    insert_idis(&mips32_sub_d);
    insert_idis(&mips32_sub_ps);
    insert_idis(&mips32_subu);
    insert_idis(&mips32_suxc1);
    insert_idis(&mips32_sw);
    insert_idis(&mips32_swc1);
    insert_idis(&mips32_swc2);
    insert_idis(&mips32_swe);
    insert_idis(&mips32_swl);
    insert_idis(&mips32_swle);
    insert_idis(&mips32_swr);
    insert_idis(&mips32_swre);
    insert_idis(&mips32_swxc1);
    insert_idis(&mips32_sync);
    insert_idis(&mips32_synci);
    insert_idis(&mips32_syscall);
    insert_idis(&mips32_teq);
    insert_idis(&mips32_teqi);
    insert_idis(&mips32_tge);
    insert_idis(&mips32_tgei);
    insert_idis(&mips32_tgeiu);
    insert_idis(&mips32_tgeu);
    insert_idis(&mips32_tlbinv);
    insert_idis(&mips32_tlbinvf);
    insert_idis(&mips32_tlbp);
    insert_idis(&mips32_tlbr);
    insert_idis(&mips32_tlbwi);
    insert_idis(&mips32_tlbwr);
    insert_idis(&mips32_tlt);
    insert_idis(&mips32_tlti);
    insert_idis(&mips32_tltiu);
    insert_idis(&mips32_tltu);
    insert_idis(&mips32_tne);
    insert_idis(&mips32_tnei);
    insert_idis(&mips32_trunc_l_s);
    insert_idis(&mips32_trunc_l_d);
    insert_idis(&mips32_trunc_w_s);
    insert_idis(&mips32_trunc_w_d);
    insert_idis(&mips32_wait);
    insert_idis(&mips32_wrpgpr);
    insert_idis(&mips32_wsbh);
    insert_idis(&mips32_xor);
    insert_idis(&mips32_xori);
}

// Notes:
// COP1 encoding of RS field
//   020 = S    single precision
//   021 = D    double precision
//   024 = W    word fixed point
//   025 = L    long fixed point
//   026 = PS   pair of single precision

} // namespace
} // namespace
} // namespace

#endif
