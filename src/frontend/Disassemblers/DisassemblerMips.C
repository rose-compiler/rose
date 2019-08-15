#include "sage3basic.h"

#include "DisassemblerMips.h"
#include "integerOps.h"
#include "Diagnostics.h"
#include "BinaryUnparserMips.h"

namespace Rose {
namespace BinaryAnalysis {

using namespace IntegerOps;
using namespace Diagnostics;

class DisassemblerMips;

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
//     (msb)_________________________________________________________(lsb)
//     |3|3|3|2|2|2|2|2|2|2|2|2|2|1|1|1|1|1|1|1|1|1|1| | | | | | | | | | |
//     |2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|9|8|7|6|5|4|3|2|1|0|
//     +-------------+---------+---------+---------+---------+-----------+
//     |    OP       |   R0    |   R1    |   R2    |   R3    |     FN    |
//     +-------------+---------+---------+---------+---------+-----------+
//     |                                 |           IM                  |
//     +---------------------------------+-------------------------------+
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

/*****************************************************************************************************************************/

// see base class
bool
DisassemblerMips::canDisassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_MIPS_Family;
}

Unparser::BasePtr
DisassemblerMips::unparser() const {
    return Unparser::Mips::instance();
}

// see base class
SgAsmInstruction *
DisassemblerMips::disassembleOne(const MemoryMap::Ptr &map, rose_addr_t start_va, AddressSet *successors)
{
    // Instructions are always four-byte, naturally-aligned, in big- or little-endian order.
    insn_va = start_va;
    if (start_va & 0x03)
        throw Exception("non-aligned instruction", start_va);
    uint32_t insn_disk; // instruction in file byte order
    if (4!=map->at(start_va).limit(4).require(MemoryMap::EXECUTABLE).read((uint8_t*)&insn_disk).size())
        throw Exception("short read", start_va);
    unsigned insn_bits = ByteOrder::disk_to_host(byteOrder(), insn_disk);
    SgAsmMipsInstruction *insn = disassemble_insn(insn_bits);
    if (!insn)
        throw Exception("cannot disassemble MIPS instruction: " + StringUtility::addrToString(insn_bits));
    insn->set_raw_bytes(SgUnsignedCharList((unsigned char*)&insn_disk, (unsigned char*)&insn_disk+4));

    if (successors) {
        bool complete;
        AddressSet suc2 = insn->getSuccessors(&complete);
        successors->insert(suc2.begin(), suc2.end());
    }
    return insn;
}

// see base class
SgAsmInstruction *
DisassemblerMips::makeUnknownInstruction(const Disassembler::Exception &e)
{
    SgAsmMipsInstruction *insn = makeInstruction(mips_unknown_instruction, "unknown");
    insn->set_raw_bytes(e.bytes);
    return insn;
}

SgAsmMipsInstruction *
DisassemblerMips::makeInstruction(MipsInstructionKind kind, const std::string &mnemonic,
                                  SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3, SgAsmExpression *op4)
{
    SgAsmMipsInstruction *insn = new SgAsmMipsInstruction(insn_va, mnemonic, kind);

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

SgAsmRegisterReferenceExpression *
DisassemblerMips::makeRegister(unsigned regnum)
{
    std::string regname = "r" + StringUtility::numberToString(regnum);
    const RegisterDescriptor *regdesc = registerDictionary()->lookup(regname);
    if (!regdesc)
        throw Exception("no such register: "+regname);
    return new SgAsmDirectRegisterExpression(*regdesc);
}

SgAsmRegisterReferenceExpression *
DisassemblerMips::makeFpRegister(unsigned regnum)
{
    std::string regname = "f" + StringUtility::numberToString(regnum);
    const RegisterDescriptor *regdesc = registerDictionary()->lookup(regname);
    if (!regdesc)
        throw Exception("no such register: "+regname);
    return new SgAsmDirectRegisterExpression(*regdesc);
}

SgAsmRegisterReferenceExpression *
DisassemblerMips::makeCp0Register(unsigned regnum, unsigned sel)
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
                        " (sel="+StringUtility::numberToString(sel)+")");
    const RegisterDescriptor *regdesc = registerDictionary()->lookup(s);
    if (!regdesc)
        throw Exception("no such register: " + s);
    return new SgAsmDirectRegisterExpression(*regdesc);
}

SgAsmRegisterReferenceExpression *
DisassemblerMips::makeFpccRegister(unsigned cc)
{
    ASSERT_require(cc<=7);
    const RegisterDescriptor *regdesc = registerDictionary()->lookup("fscr");
    if (!regdesc)
        throw Exception("no such register: fcsr");
    RegisterDescriptor r(regdesc->majorNumber(), regdesc->minorNumber(), cc?24+cc:23, 1);
    return new SgAsmDirectRegisterExpression(r);
}

SgAsmRegisterReferenceExpression *
DisassemblerMips::makeCp2Register(unsigned regnum)
{
    // Coprocessor 2 is implementation defined. We're assuming 32 individual 32-bit registers numbered 0-31.
    ASSERT_require(regnum<32);
    return new SgAsmDirectRegisterExpression(RegisterDescriptor(mips_regclass_cp2gpr, regnum, 0, 32));
}
    
SgAsmRegisterReferenceExpression *
DisassemblerMips::makeCp2ccRegister(unsigned regnum)
{
    // Coprocessor 2 is implementation defined. We're assuming 32 individual 32-bit registers numbered 0-31.
    ASSERT_require(regnum<32);
    return new SgAsmDirectRegisterExpression(RegisterDescriptor(mips_regclass_cp2spr, regnum, 0, 32));
}

SgAsmRegisterReferenceExpression *
DisassemblerMips::makeHwRegister(unsigned cc)
{
    ASSERT_not_implemented("[Robb Matzke 2014-01-27]");
    return NULL;
}

SgAsmRegisterReferenceExpression *
DisassemblerMips::makeShadowRegister(unsigned cc)
{
    // Get the general purpose register
    SgAsmRegisterReferenceExpression *regref = makeRegister(cc);
    ASSERT_not_null(regref);

    // Turn it into a shadow register
    RegisterDescriptor desc = regref->get_descriptor();
    desc.majorNumber(mips_regclass_sgpr);
    regref->set_descriptor(desc);
    return regref;
}

SgAsmIntegerValueExpression *
DisassemblerMips::makeImmediate8(unsigned value, size_t bit_offset, size_t nbits)
{
    ASSERT_require(0==(value & ~0xff));
    SgAsmIntegerValueExpression *retval = SageBuilderAsm::buildValueU8(value);
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(nbits);
    return retval;
}

SgAsmIntegerValueExpression *
DisassemblerMips::makeImmediate16(unsigned value, size_t bit_offset, size_t nbits)
{
    ASSERT_require(0==(value & ~0xffff));
    SgAsmIntegerValueExpression *retval = SageBuilderAsm::buildValueU16(value);
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(nbits);
    return retval;
}

SgAsmIntegerValueExpression *
DisassemblerMips::makeImmediate32(unsigned value, size_t bit_offset, size_t nbits)
{
    ASSERT_require(0==(value & ~0xffffffffull));
    SgAsmIntegerValueExpression *retval = SageBuilderAsm::buildValueU32(value);
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(nbits);
    return retval;
}

SgAsmIntegerValueExpression *
DisassemblerMips::makeBranchTargetRelative(unsigned pc_offset, size_t bit_offset, size_t nbits)
{
    ASSERT_require(0==(pc_offset & ~0xffff));
    pc_offset = shiftLeft<32>(pc_offset, 2);        // insns have 4-byte alignment
    pc_offset = signExtend<18, 32>(pc_offset);      // offsets are signed
    unsigned target = (get_ip() + 4 + pc_offset) & GenMask<unsigned, 32>::value; // measured from next instruction
    SgAsmIntegerValueExpression *retval = SageBuilderAsm::buildValueU32(target);
    retval->set_bit_offset(bit_offset);
    retval->set_bit_size(nbits);
    return retval;
}

SgAsmIntegerValueExpression *
DisassemblerMips::makeBranchTargetAbsolute(unsigned insn_index, size_t bit_offset, size_t nbits)
{
    ASSERT_require(nbits>0);
    ASSERT_require(bit_offset+nbits+2<=32);
    ASSERT_require(0==(insn_index & ~genMask<uint64_t>(nbits)));
    unsigned lo_nbits = nbits+2;        // number of bits coming from instr_index after multiplying by four
    unsigned lo_mask = genMask<uint64_t>(lo_nbits);
    unsigned lo_target = shiftLeft<32>(insn_index, 2) & lo_mask;
    unsigned hi_target = (get_ip() + 4) & ~lo_mask;
    unsigned target = hi_target | lo_target;
    return SageBuilderAsm::buildValueU32(target);
}

SgAsmBinaryAdd *
DisassemblerMips::makeRegisterOffset(unsigned gprnum, unsigned offset16)
{
    SgAsmRegisterReferenceExpression *regref = makeRegister(gprnum);
    ASSERT_require(0==(offset16 & ~0xffff));
    unsigned offset32 = signExtend<16, 32>(offset16);
    SgAsmIntegerValueExpression *offset = SageBuilderAsm::buildValueU32(offset32);
    SgAsmBinaryAdd *retval = SageBuilderAsm::buildAddExpression(regref, offset);
    return retval;
}

SgAsmBinaryAdd *
DisassemblerMips::makeRegisterIndexed(unsigned base_gprnum, unsigned index_gprnum)
{
    SgAsmRegisterReferenceExpression *base_reg = makeRegister(base_gprnum);
    SgAsmRegisterReferenceExpression *index_reg = makeRegister(index_gprnum);
    SgAsmBinaryAdd *retval = SageBuilderAsm::buildAddExpression(base_reg, index_reg);
    return retval;
}

SgAsmMemoryReferenceExpression *
DisassemblerMips::makeMemoryReference(SgAsmExpression *addr, SgAsmType *type)
{
    ASSERT_not_null(addr);
    ASSERT_not_null(type);
    return SageBuilderAsm::buildMemoryReferenceExpression(addr, NULL, type);
}

DisassemblerMips::Mips32 *
DisassemblerMips::find_idis(unsigned insn_bits)
{
    Mips32 *retval = NULL;
    for (size_t i=0; i<idis_table.size(); ++i) {
        if ((insn_bits & idis_table[i]->mask) == idis_table[i]->match) {
            if (retval) {
                std::string mesg = ("DisassemblerMips::find_idis: multiple matches for instruction word " +
                                    StringUtility::addrToString(insn_bits) +
                                    " (first found mask=" + StringUtility::addrToString(retval->mask) +
                                    " match=" + StringUtility::addrToString(retval->match) +
                                    "; addnl found mask=" + StringUtility::addrToString(idis_table[i]->mask) +
                                    " match=" + StringUtility::addrToString(idis_table[i]->match) + ")");
                throw Exception(mesg);
            } else {
                retval = idis_table[i];
            }
        }
    }
    return retval;
}

void
DisassemblerMips::insert_idis(Mips32 *idis, bool replace)
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
            mlog[DEBUG] <<"DisassemblerMips::insert_idis: insertion key"
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
DisassemblerMips::disassemble_insn(unsigned insn_bits)
{
    if (Mips32 *idis = find_idis(insn_bits))
        return (*idis)(this, insn_bits);
    return NULL;
}

/*******************************************************************************************************************************
 *                                      Instruction-specific disassemblers
 *******************************************************************************************************************************/

typedef DisassemblerMips::Mips32 Mips32;

// ABS.S -- floating point absolute value
static struct Mips32_abs_s: Mips32 {
    Mips32_abs_s(): Mips32(Release1,
                           sOP(021)|sR0(020)|sR1(000)|sFN(005),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_abs_s, "abs.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_abs_s;

// ABS.D -- floating point absolute value
static struct Mips32_abs_d: Mips32 {
    Mips32_abs_d(): Mips32(Release1,
                           sOP(021)|sR0(021)|sR1(000)|sFN(005),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_abs_d, "abs.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_abs_d;

// ABS.PS -- floating point absolute value
static struct Mips32_abs_ps: Mips32 {
    Mips32_abs_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sR1(000)|sFN(005),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_abs_ps, "abs.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_abs_ps;

// ADD -- add word
static struct Mips32_add: Mips32 {
    Mips32_add(): Mips32(Release1,
                         sOP(000)|sR3(000)|sFN(040),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_add, "add",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_add;

// ADD.S -- floating point add
static struct Mips32_add_s: Mips32 {
    Mips32_add_s(): Mips32(Release1,
                           sOP(021)|sR0(020)|sFN(000),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_add_s, "add.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_add_s;

// ADD.D -- floating point add
static struct Mips32_add_d: Mips32 {
    Mips32_add_d(): Mips32(Release1,
                           sOP(021)|sR0(021)|sFN(000),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_add_d, "add.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_add_d;

// ADD.PS -- floating point add
static struct Mips32_add_ps: Mips32 {
    Mips32_add_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sFN(000),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_add_ps, "add.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_add_ps;

// ADDI -- add immediate word
static struct Mips32_addi: Mips32 {
    Mips32_addi(): Mips32(Release1, sOP(010), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_addi, "addi",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_addi;

// ADDIU -- add immediate unsigned word
static struct Mips32_addiu: Mips32 {
    Mips32_addiu(): Mips32(Release1, sOP(011), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_addiu, "addiu",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16)); 
    }
} mips32_addiu;

// ADDU -- add unsigned word
static struct Mips32_addu: Mips32 {
    Mips32_addu(): Mips32(Release1,
                          sOP(000)|sR3(000)|sFN(041),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_addu, "addu",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_addu;

// ALNV.PS -- floating point align variable
static struct Mips32_alnv_ps: Mips32 {
    Mips32_alnv_ps(): Mips32(Release2,
                             sOP(023)|sFN(036),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_alnv_ps, "alnv.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)),
                                  d->makeRegister(gR0(ib)));
    }
} mips32_alnv_ps;

// AND -- bitwise logical AND
static struct Mips32_and: Mips32 {
    Mips32_and(): Mips32(Release1,
                         sOP(000)|sR3(000)|sFN(044),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_and, "and",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_and;

// ANDI -- and immediate
static struct Mips32_andi: Mips32 {
    Mips32_andi(): Mips32(Release1, sOP(014), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_andi, "andi",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_andi;

// BC1F -- branch on FP false
// The "BC1F offset" is the same as "BC1F cc, offset" where cc==0
static struct Mips32_bc1f: Mips32 {
    Mips32_bc1f(): Mips32(Release1,
                          sOP(021)|sR0(010)|shift_to<16, 17>(0),
                          mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(mips_bc1f, "bc1f",
                                  d->makeFpccRegister(cc), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bc1f;

// BC1FL -- branch on FP false likely (deprecated instruction)
static struct Mips32_bc1fl: Mips32 {
    Mips32_bc1fl(): Mips32(Release1,
                           sOP(021)|sR0(010)|shift_to<16, 17>(2),
                           mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(mips_bc1fl, "bc1fl",
                                  d->makeFpccRegister(cc), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bc1fl;

// BC1T -- branch on FP true
static struct Mips32_bc1t: Mips32 {
    Mips32_bc1t(): Mips32(Release1,
                          sOP(021)|sR0(010)|shift_to<16, 17>(1),
                          mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(mips_bc1t, "bc1t",
                                  d->makeFpccRegister(cc), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bc1t;

// BC1TL -- branch on FP true likely (deprecated instruction)
static struct Mips32_bc1tl: Mips32 {
    Mips32_bc1tl(): Mips32(Release1,
                           sOP(21)|sR0(010)|shift_to<16, 17>(3),
                           mOP()  |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(mips_bc1tl, "bc1tl",
                                  d->makeFpccRegister(cc), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bc1tl;

// BC2F -- branch on COP2 false
static struct Mips32_bc2f: Mips32 {
    Mips32_bc2f(): Mips32(Release1,
                          sOP(022)|sR0(010)|shift_to<16, 17>(0),
                          mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(mips_bc2f, "bc2f",
                                  d->makeCp2ccRegister(cc), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bc2f;

// BC2FL -- branch on COP2 false likely (deprecated instruction)
static struct Mips32_bc2fl: Mips32 {
    Mips32_bc2fl(): Mips32(Release1,
                           sOP(022)|sR0(010)|shift_to<16, 17>(2),
                           mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(mips_bc2fl, "bc2fl",
                                  d->makeCp2ccRegister(cc), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bc2fl;

// BC2T -- branch on COP2 true
static struct Mips32_bc2t: Mips32 {
    Mips32_bc2t(): Mips32(Release1,
                          sOP(022)|sR0(010)|shift_to<16, 17>(1),
                          mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(mips_bc2t, "bc2t",
                                  d->makeCp2ccRegister(cc), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bc2t;

// BC2TL -- branch on COP2 true likely (deprecated instruction)
static struct Mips32_bc2tl: Mips32 {
    Mips32_bc2tl(): Mips32(Release1,
                           sOP(022)|sR0(010)|shift_to<16, 17>(3),
                           mOP()   |mR0()   |mask_for<16, 17>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<18, 20>(ib);
        return d->makeInstruction(mips_bc2tl, "bc2tl",
                                  d->makeCp2ccRegister(cc), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bc2tl;

// BEQ -- branch on equal
static struct Mips32_beq: Mips32 {
    Mips32_beq(): Mips32(Release1, sOP(004), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_beq, "beq",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)),
                                  d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_beq;

// BEQL -- branch on equal likely
static struct Mips32_beql: Mips32 {
    Mips32_beql(): Mips32(Release1, sOP(024), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_beql, "beql",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)),
                                  d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_beql;

// BGEZ -- branch on greater than or equal to zero
static struct Mips32_bgez: Mips32 {
    Mips32_bgez(): Mips32(Release1,
                          sOP(001)|sR1(001),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bgez, "bgez",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bgez;

// BGEZAL -- branch on greater than or equal to zero and link
static struct Mips32_bgezal: Mips32 {
    Mips32_bgezal(): Mips32(Release1,
                            sOP(001)|sR1(021),
                            mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bgezal, "bgezal",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bgezal;

// BGEZALL -- branch on greater than or equal to zero and link likely (deprecated instruction)
static struct Mips32_bgezall: Mips32 {
    Mips32_bgezall(): Mips32(Release1,
                             sOP(001)|sR1(023),
                             mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bgezall, "bgezall",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bgezall;

// BGEZL -- branch on greater than or equal to zero likely (deprecated instruction)
static struct Mips32_bgezl: Mips32 {
    Mips32_bgezl(): Mips32(Release1,
                           sOP(001)|sR1(003),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bgezl, "bgezl",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bgezl;

// BGTZ -- branch on greater than zero
static struct Mips32_bgtz: Mips32 {
    Mips32_bgtz(): Mips32(Release1,
                          sOP(007)|sR1(000),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bgtz, "bgtz",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bgtz;

// BGTZL -- branch on greater than zero likely
static struct Mips32_bgtzl: Mips32 {
    Mips32_bgtzl(): Mips32(Release1,
                           sOP(023)|sR1(000),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bgtzl, "bgtzl",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bgtzl;

// BLEZ -- branch on less than or equal to zero
static struct Mips32_blez: Mips32 {
    Mips32_blez(): Mips32(Release1,
                          sOP(006)|sR1(000),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_blez, "blez",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_blez;

// BLEZL -- branch on less than or equal to zero likely (deprecated instruction)
static struct Mips32_blezl: Mips32 {
    Mips32_blezl(): Mips32(Release1,
                           sOP(026)|sR1(000),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_blezl, "blezl",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_blezl;

// BLTZ -- branch on less than zero
static struct Mips32_bltz: Mips32 {
    Mips32_bltz(): Mips32(Release1,
                          sOP(001)|sR1(000),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bltz, "bltz",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bltz;

// BLTZAL -- branch on less than zero and link
static struct Mips32_bltzal: Mips32 {
    Mips32_bltzal(): Mips32(Release1,
                            sOP(001)|sR1(020),
                            mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bltzal, "bltzal",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bltzal;

// BLTZALL -- branch on less than zero and link likely (deprecated instruction)
static struct Mips32_bltzall: Mips32 {
    Mips32_bltzall(): Mips32(Release1,
                             sOP(001)|sR1(022),
                             mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bltzall, "bltzall",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bltzall;

// BLTZL -- branch on less than zero likely
static struct Mips32_bltzl: Mips32 {
    Mips32_bltzl(): Mips32(Release1,
                           sOP(001)|sR1(002),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bltzl, "bltzl",
                                  d->makeRegister(gR0(ib)), d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bltzl;

// BNE -- branch on not equal
static struct Mips32_bne: Mips32 {
    Mips32_bne(): Mips32(Release1, sOP(005), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bne, "bne",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)),
                                  d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bne;

// BNEL -- branch on not equal likely (deprecated instruction)
static struct Mips32_bnel: Mips32 {
    Mips32_bnel(): Mips32(Release1, sOP(025), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_bnel, "bnel",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)),
                                  d->makeBranchTargetRelative(gIM(ib), 0, 16));
    }
} mips32_bnel;
#ifdef _MSC_VER
#define UNUSED_VAR
#else
#define UNUSED_VAR __attribute__((unused))
#endif
// BREAK -- breakpoint
static struct Mips32_break: Mips32 {
    Mips32_break(): Mips32(Release1,
                           sOP(000)|sFN(015),
                           mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 25>(ib); // only available by reading the instruction
        return d->makeInstruction(mips_break, "break");
    }
} mips32_break;

// C.cond.S --floating point compare single precision
// where "cond" is one of F, UN, EQ, UEQ, OLT, ULT, OLE, ULE, SF, NGLE, SEQ, NGL, LT, NGE, LE, or NGT.
// Note that the "C.cond.S fs, ft" form is the same as "C.cond.S 0, fs, ft" (i.e., cc==0)
static struct Mips32_c_cond_s: Mips32 {
    Mips32_c_cond_s(): Mips32(Release1,
                              sOP(021)|sR0(020)|shift_to<4, 7>(003),
                              mOP()   |mR0()   |mask_for<4, 7>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<8, 10>(ib);
        unsigned condition = extract<0, 3>(ib);
        MipsInstructionKind kind;
        std::string mnemonic;
        switch (condition) {
            case  0: kind = mips_c_f_s;    mnemonic = "c.f.s";    break;
            case  1: kind = mips_c_un_s;   mnemonic = "c.un.s";   break;
            case  2: kind = mips_c_eq_s;   mnemonic = "c.eq.s";   break;
            case  3: kind = mips_c_ueq_s;  mnemonic = "c.ueq.s";  break;
            case  4: kind = mips_c_olt_s;  mnemonic = "c.olt.s";  break;
            case  5: kind = mips_c_ult_s;  mnemonic = "c.ult.s";  break;
            case  6: kind = mips_c_ole_s;  mnemonic = "c.ole.s";  break;
            case  7: kind = mips_c_ule_s;  mnemonic = "c.ule.s";  break;
            case  8: kind = mips_c_sf_s;   mnemonic = "c.sf.s";   break;
            case  9: kind = mips_c_ngle_s; mnemonic = "c.ngle.s"; break;
            case 10: kind = mips_c_seq_s;  mnemonic = "c.seq.s";  break;
            case 11: kind = mips_c_ngl_s;  mnemonic = "c.ngl.s";  break;
            case 12: kind = mips_c_lt_s;   mnemonic = "c.lt.s";   break;
            case 13: kind = mips_c_nge_s;  mnemonic = "c.nge.s";  break;
            case 14: kind = mips_c_le_s;   mnemonic = "c.le.s";   break;
            case 15: kind = mips_c_ngt_s;  mnemonic = "c.ngt.s";  break;
            default: ASSERT_not_reachable("invalid condition " + StringUtility::numberToString(condition));
        }
        return d->makeInstruction(kind, mnemonic,
                                  d->makeFpccRegister(cc), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_c_cond_s;

// C.cond.D --floating point compare double precision
// where "cond" is one of F, UN, EQ, UEQ, OLT, ULT, OLE, ULE, SF, NGLE, SEQ, NGL, LT, NGE, LE, or NGT.
// Note that the "C.cond.D fs, ft" form is the same as "C.cond.D 0, fs, ft" (i.e., cc==0)
static struct Mips32_c_cond_d: Mips32 {
    Mips32_c_cond_d(): Mips32(Release1,
                              sOP(021)|sR0(021)|shift_to<4, 7>(003),
                              mOP()   |mR0()   |mask_for<4, 7>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<8, 10>(ib);
        unsigned condition = extract<0, 3>(ib);
        MipsInstructionKind kind;
        std::string mnemonic;
        switch (condition) {
            case  0: kind = mips_c_f_d;    mnemonic = "c.f.d";    break;
            case  1: kind = mips_c_un_d;   mnemonic = "c.un.d";   break;
            case  2: kind = mips_c_eq_d;   mnemonic = "c.eq.d";   break;
            case  3: kind = mips_c_ueq_d;  mnemonic = "c.ueq.d";  break;
            case  4: kind = mips_c_olt_d;  mnemonic = "c.olt.d";  break;
            case  5: kind = mips_c_ult_d;  mnemonic = "c.ult.d";  break;
            case  6: kind = mips_c_ole_d;  mnemonic = "c.ole.d";  break;
            case  7: kind = mips_c_ule_d;  mnemonic = "c.ule.d";  break;
            case  8: kind = mips_c_sf_d;   mnemonic = "c.sf.d";   break;
            case  9: kind = mips_c_ngle_d; mnemonic = "c.ngle.d"; break;
            case 10: kind = mips_c_seq_d;  mnemonic = "c.seq.d";  break;
            case 11: kind = mips_c_ngl_d;  mnemonic = "c.ngl.d";  break;
            case 12: kind = mips_c_lt_d;   mnemonic = "c.lt.d";   break;
            case 13: kind = mips_c_nge_d;  mnemonic = "c.nge.d";  break;
            case 14: kind = mips_c_le_d;   mnemonic = "c.le.d";   break;
            case 15: kind = mips_c_ngt_d;  mnemonic = "c.ngt.d";  break;
            default: ASSERT_not_reachable("invalid condition " + StringUtility::numberToString(condition));
        }
        return d->makeInstruction(kind, mnemonic,
                                  d->makeFpccRegister(cc), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_c_cond_d;

// C.cond.PS --floating point compare pair of single precision
// where "cond" is one of F, UN, EQ, UEQ, OLT, ULT, OLE, ULE, SF, NGLE, SEQ, NGL, LT, NGE, LE, or NGT.
// Note that the "C.cond.PS fs, ft" form is the same as "C.cond.PS 0, fs, ft" (i.e., cc==0)
static struct Mips32_c_cond_ps: Mips32 {
    Mips32_c_cond_ps(): Mips32(Release2,
                               sOP(021)|sR0(026)|shift_to<4, 7>(003),
                               mOP()   |mR0()   |mask_for<4, 7>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned cc = extract<8, 10>(ib);
        unsigned condition = extract<0, 3>(ib);
        MipsInstructionKind kind;
        std::string mnemonic;
        switch (condition) {
            case  0: kind = mips_c_f_ps;    mnemonic = "c.f.ps";    break;
            case  1: kind = mips_c_un_ps;   mnemonic = "c.un.ps";   break;
            case  2: kind = mips_c_eq_ps;   mnemonic = "c.eq.ps";   break;
            case  3: kind = mips_c_ueq_ps;  mnemonic = "c.ueq.ps";  break;
            case  4: kind = mips_c_olt_ps;  mnemonic = "c.olt.ps";  break;
            case  5: kind = mips_c_ult_ps;  mnemonic = "c.ult.ps";  break;
            case  6: kind = mips_c_ole_ps;  mnemonic = "c.ole.ps";  break;
            case  7: kind = mips_c_ule_ps;  mnemonic = "c.ule.ps";  break;
            case  8: kind = mips_c_sf_ps;   mnemonic = "c.sf.ps";   break;
            case  9: kind = mips_c_ngle_ps; mnemonic = "c.ngle.ps"; break;
            case 10: kind = mips_c_seq_ps;  mnemonic = "c.seq.ps";  break;
            case 11: kind = mips_c_ngl_ps;  mnemonic = "c.ngl.ps";  break;
            case 12: kind = mips_c_lt_ps;   mnemonic = "c.lt.ps";   break;
            case 13: kind = mips_c_nge_ps;  mnemonic = "c.nge.ps";  break;
            case 14: kind = mips_c_le_ps;   mnemonic = "c.le.ps";   break;
            case 15: kind = mips_c_ngt_ps;  mnemonic = "c.ngt.ps";  break;
            default: ASSERT_not_reachable("invalid condition " + StringUtility::numberToString(condition));
        }
        return d->makeInstruction(kind, mnemonic,
                                  d->makeFpccRegister(cc), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_c_cond_ps;

// CACHE -- perform cache operation
static struct Mips32_cache: Mips32 {
    Mips32_cache(): Mips32(Release1, sOP(057), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cache, "cache",
                                  d->makeImmediate8(gR1(ib), 16, 5), d->makeRegisterOffset(gR0(ib), gIM(ib)));
    }
} mips32_cache;

// CACHEE -- perform cache operation EVA
static struct Mips32_cachee: Mips32 {
    Mips32_cachee(): Mips32(Release1,
                            sOP(037)|sFN(033)|shift_to<6, 6>(0),
                            mOP()   |mFN()   |mask_for<6, 6>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cachee, "cachee",
                                  d->makeImmediate8(gR1(ib), 16, 5), d->makeRegisterOffset(gR0(ib), extract<7, 15>(ib)));
    }
} mips32_cachee;

// CEIL.L.S -- fixed point ceiling convert to long fixed point
static struct Mips32_ceil_l_s: Mips32 {
    Mips32_ceil_l_s(): Mips32(Release2,
                              sOP(021)|sR0(020)|sR1(0)||sFN(012),
                              mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ceil_l_s, "ceil.l.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_ceil_l_s;

// CEIL.L.D -- fixed point ceiling convert to long fixed point
static struct Mips32_ceil_l_d: Mips32 {
    Mips32_ceil_l_d(): Mips32(Release2,
                              sOP(021)|sR0(021)|sR1(0)|sFN(012),
                              mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ceil_l_d, "ceil.l.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_ceil_l_d;

// CEIL.W.S -- floating point ceiling convert to word fixed point
static struct Mips32_ceil_w_s: Mips32 {
    Mips32_ceil_w_s(): Mips32(Release1,
                              sOP(021)|sR0(020)|sR1(0)|sFN(016),
                              mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ceil_w_s, "ceil.w.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_ceil_w_s;

// CEIL.W.D -- floating point ceiling convert to word fixed point
static struct Mips32_ceil_w_d: Mips32 {
    Mips32_ceil_w_d(): Mips32(Release1,
                              sOP(021)|sR0(021)|sR1(0)|sFN(016),
                              mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ceil_w_d, "ceil.w.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_ceil_w_d;

// CFC1 -- move control word from floating point
static struct Mips32_cfc1: Mips32 {
    Mips32_cfc1(): Mips32(Release1,
                          sOP(021)|sR0(002)|shift_to<0, 10>(0),
                          mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cfc1, "cfc1",
                                  d->makeRegister(gR1(ib)), d->makeImmediate8(gR2(ib), 11, 5));
    }
} mips32_cfc1;

// CFC2 -- move control word from coprocessor 2
static struct Mips32_cfc2: Mips32 {
    Mips32_cfc2(): Mips32(Release1,
                          sOP(022)|sR0(002),
                          mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cfc2, "cfc2",
                                  d->makeRegister(gR1(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_cfc2;

// CLO -- count leading ones in word
static struct Mips32_clo: Mips32 {
    Mips32_clo(): Mips32(Release1,
                         sOP(034)|sR3(0)|sFN(041),
                         mOP()   |mR3() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        bool valid UNUSED_VAR = gR1(ib) == gR2(ib); // r1 must be same register number as r2
        return d->makeInstruction(mips_clo, "clo",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)));
    }
} mips32_clo;

// CLZ -- count leading zeros in word
static struct Mips32_clz: Mips32 {
    Mips32_clz(): Mips32(Release1,
                         sOP(034)|sR3(0)|sFN(040),
                         mOP()   |mR3() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        bool valid UNUSED_VAR = gR1(ib) == gR2(ib); // r1 must be same register number as r2
        return d->makeInstruction(mips_clz, "clz",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)));
    }
} mips32_clz;

// COP2 -- coprocessor operation to coprocessor 2
static struct Mips32_cop2: Mips32 {
    Mips32_cop2(): Mips32(Release1,
                          sOP(022)|shift_to<25, 25>(1),
                          mOP()   |mask_for<25, 25>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cop2, "cop2",
                                  d->makeImmediate32(extract<0, 24>(ib), 0, 24));
    }
} mips32_cop2;

// CTC1 -- move control word to floating point
static struct Mips32_ctc1: Mips32 {
    Mips32_ctc1(): Mips32(Release1, 
                          sOP(021)|sR0(006)|shift_to<0, 10>(0),
                          mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ctc1, "ctc1",
                                  d->makeRegister(gR1(ib)), d->makeImmediate8(gR2(ib), 11, 5));
    }
} mips32_ctc1;

// CTC2 -- move control word to coprocessor 2
static struct Mips32_ctc2: Mips32 {
    Mips32_ctc2(): Mips32(Release1,
                          sOP(022)|sR0(006),
                          mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ctc2, "ctc2",
                                  d->makeRegister(gR1(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_ctc2;

// CVT.D.S -- floating point convert to double floating point
static struct Mips32_cvt_d_s: Mips32 {
    Mips32_cvt_d_s(): Mips32(Release1,
                             sOP(021)|sR0(020)|sR1(0)|sFN(041),
                             mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_d_s, "cvt.d.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_d_s;

// CVT.D.W -- floating point convert to double floating point
static struct Mips32_cvt_d_w: Mips32 {
    Mips32_cvt_d_w(): Mips32(Release1,
                             sOP(021)|sR0(024)|sR1(0)|sFN(041),
                             mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_d_w, "cvt.d.w",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_d_w;

// CVT.D.L -- floating point convert to double floating point
static struct Mips32_cvt_d_l: Mips32 {
    Mips32_cvt_d_l(): Mips32(Release2,
                             sOP(021)|sR0(025)|sR1(0)|sFN(041),
                             mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_d_l, "cvt.d.l",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_d_l;

// CVT.L.S -- floating point convert to long fixed point
static struct Mips32_cvt_l_s: Mips32 {
    Mips32_cvt_l_s(): Mips32(Release2,
                             sOP(021)|sR0(020)|sR1(0)|sFN(045),
                             mOP()   |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_l_s, "cvt.l.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_l_s;

// CVT.L.D -- floating point convert to long fixed point
static struct Mips32_cvt_l_d: Mips32 {
    Mips32_cvt_l_d(): Mips32(Release2,
                             sOP(21)|sR0(021)|sR1(0)|sFN(045),
                             mOP()  |mR0()   |mR1() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_l_d, "cvt.l.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_l_d;

// CVT.PS.S -- floating point convert to paired singled
static struct Mips32_cvt_ps_s: Mips32 {
    Mips32_cvt_ps_s(): Mips32(Release2,
                              sOP(021)|sR0(020)|sFN(046),
                              mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_ps_s, "cvt.l.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_cvt_ps_s;

// CVT.S.D -- floating point convert to single floating point
static struct Mips32_cvt_s_d: Mips32 {
    Mips32_cvt_s_d(): Mips32(Release1,
                             sOP(021)|sR0(021)|sR1(000)|sFN(040),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_s_d, "cvt.s.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_s_d;

// CVT.S.W -- floating point convert to single floating point
static struct Mips32_cvt_s_w: Mips32 {
    Mips32_cvt_s_w(): Mips32(Release1,
                             sOP(021)|sR0(024)|sR1(000)|sFN(040),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_s_w, "cvt.s.w",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_s_w;

// CVT.S.L -- floating point convert to single floating point
static struct Mips32_cvt_s_l: Mips32 {
    Mips32_cvt_s_l(): Mips32(Release2,
                             sOP(021)|sR0(025)|sR1(000)|sFN(040),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_s_l, "cvt.s.l",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_s_l;

// CVT.S.PL -- floating point convert pair lower to single floating point
static struct Mips32_cvt_s_pl: Mips32 {
    Mips32_cvt_s_pl(): Mips32(Release2,
                              sOP(021)|sR0(026)|sR1(000)|sFN(050),
                              mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_s_pl, "cvt.s.pl",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_s_pl;

// CVT.S.PU -- floating point convert pair upper to single floating point
static struct Mips32_cvt_s_pu: Mips32 {
    Mips32_cvt_s_pu(): Mips32(Release2,
                              sOP(021)|sR0(026)|sR1(000)|sFN(040),
                              mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_s_pu, "cvt.s.pu",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_s_pu;

// CVT.W.S -- floating point convert to word fixed point
static struct Mips32_cvt_w_s: Mips32 {
    Mips32_cvt_w_s(): Mips32(Release1,
                             sOP(021)|sR0(020)|sR1(000)|sFN(044),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_w_s, "cvt.w.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_w_s;

// CVT.W.D -- floating point convert to word fixed point
static struct Mips32_cvt_w_d: Mips32 {
    Mips32_cvt_w_d(): Mips32(Release1,
                             sOP(021)|sR0(021)|sR1(000)|sFN(044),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_cvt_w_d, "cvt.w.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_cvt_w_d;

// DI -- disable interrupts
// The "DI" form is the same as "DI r0"
static struct Mips32_di: Mips32 {
    Mips32_di(): Mips32(Release2,
                        sOP(020)|sR0(023)|sR2(024)|sR3(0)|shift_to<5, 5>(0)|shift_to<3, 4>(0)|shift_to<0, 2>(0),
                        mOP()   |mR0()   |mR2()   |mR3() |mask_for<5, 5>() |mask_for<3, 4>() |mask_for<0, 2>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_di, "di",
                                  d->makeRegister(gR1(ib)));
    }
} mips32_di;

// DIV -- divide word
static struct Mips32_div: Mips32 {
    Mips32_div(): Mips32(Release1,
                         sOP(000)|shift_to<6, 15>(0)|sFN(032),
                         mOP()   |mask_for<6, 15>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_div, "div",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_div;

// DIV.S -- floating point divide
static struct Mips32_div_s: Mips32 {
    Mips32_div_s(): Mips32(Release1,
                           sOP(021)|sR0(020)|sFN(003),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_div_s, "div.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_div_s;

// DIV.D -- floating point divide
static struct Mips32_div_d: Mips32 {
    Mips32_div_d(): Mips32(Release1,
                           sOP(021)|sR0(021)|sFN(003),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_div_d, "div.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_div_d;

// DIVU -- divide unsigned word
static struct Mips32_divu: Mips32 {
    Mips32_divu(): Mips32(Release1,
                          sOP(000)|shift_to<6, 15>(0)|sFN(033),
                          mOP()   |mask_for<6, 15>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_divu, "divu",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_divu;

// EHB -- execution hazard barrier
static struct Mips32_ehb: Mips32 {
    Mips32_ehb(): Mips32(Release2,
                         sOP(000)|sR0(000)|sR1(000)|sR2(000)|sR3(003)|sFN(000),
                         mOP()   |mR0()   |mR1()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ehb, "ehb");
    }
} mips32_ehb;

// EI -- enable interrupts
// The "EI" form is the same as "EI r0"
static struct Mips32_ei: Mips32 {
    Mips32_ei(): Mips32(Release2,
                        sOP(020)|sR0(023)|sR2(014)|sR3(0)|shift_to<5, 5>(1)|shift_to<3, 4>(0)|shift_to<0, 2>(0),
                        mOP()   |mR0()   |mR2()   |mR3() |mask_for<5, 5>() |mask_for<3, 4>() |mask_for<0, 2>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ei, "ei",
                                  d->makeRegister(gR1(ib)));
    }
} mips32_ei;

// ERET -- exception return
static struct Mips32_eret: Mips32 {
    Mips32_eret(): Mips32(Release1,
                          sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(030),
                          mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_eret, "eret");
    }
} mips32_eret;

// EXT -- extract bit field
static struct Mips32_ext: Mips32 {
    Mips32_ext(): Mips32(Release2,
                         sOP(037)|sFN(000),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ext, "ext",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)),
                                  d->makeImmediate8(gR3(ib), 6, 5), d->makeImmediate8(gR2(ib)+1, 11, 5));
    }
} mips32_ext;

// FLOOR.L.S -- floating point floor convert to long fixed point
static struct Mips32_floor_l_s: Mips32 {
    Mips32_floor_l_s(): Mips32(Release2,
                               sOP(021)|sR0(020)|sR1(000)|sFN(013),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_floor_l_s, "floor.l.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_floor_l_s;

// FLOOR.L.D -- floating point floor convert to long fixed point
static struct Mips32_floor_l_d: Mips32 {
    Mips32_floor_l_d(): Mips32(Release2,
                               sOP(021)|sR0(021)|sR1(000)|sFN(013),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_floor_l_d, "floor.l.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_floor_l_d;

// FLOOR.W.S -- floating point floor convert to word fixed point
static struct Mips32_floor_w_s: Mips32 {
    Mips32_floor_w_s(): Mips32(Release1,
                               sOP(021)|sR0(020)|sR1(000)|sFN(017),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_floor_w_s, "floor.w.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_floor_w_s;

// FLOOR.W.D -- floating point floor convert to word fixed point
static struct Mips32_floor_w_d: Mips32 {
    Mips32_floor_w_d(): Mips32(Release1,
                               sOP(021)|sR0(021)|sR1(000)|sFN(017),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_floor_w_d, "floor.w.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_floor_w_d;

// INS -- insert bit field
static struct Mips32_ins: Mips32 {
    Mips32_ins(): Mips32(Release2,
                         sOP(037)|sFN(004),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned pos = gR3(ib);
        unsigned size = gR2(ib) - pos + 1;
        return d->makeInstruction(mips_ins, "ins",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)),
                                  d->makeImmediate8(pos, 6, 5), d->makeImmediate8(size, 11, 5));
    }
} mips32_ins;

// J -- jump
static struct Mips32_j: Mips32 {
    Mips32_j(): Mips32(Release1, sOP(002), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_j, "j",
                                  d->makeBranchTargetAbsolute(extract<0, 25>(ib), 0, 26));
    }
} mips32_j;

// JAL -- jump and link
static struct Mips32_jal: Mips32 {
    Mips32_jal(): Mips32(Release1, sOP(003), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_jal, "jal",
                                  d->makeBranchTargetAbsolute(extract<0, 25>(ib), 0, 26));
    }
} mips32_jal;

// JALR -- jump and link register
// The "JALR rs" form is the same as "JALR rd, rs" when rd==31
static struct Mips32_jalr: Mips32 {
    Mips32_jalr(): Mips32(Release1,
                          sOP(000)|sR1(000)|sR3(000)|sFN(011),  // R3 "hint" field must be zero for Release1
                          mOP()   |mR1()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_jalr, "jalr",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)));
    }
} mips32_jalr;

// JALR.HB -- jump and link register with hazard barrier
// The "JALR.HB rs" form is the same as "JALR.HB rd, rs" when rd==31
static struct Mips32_jalr_hb: Mips32 {
    Mips32_jalr_hb(): Mips32(Release2, // "hint" field (bits 6-9) must be zero for Release1
                             sOP(000)|sR1(000)|shift_to<10, 10>(1)|shift_to<6, 9>(0)|sFN(011),
                             mOP()   |mR1()   |mask_for<10, 10>() |mask_for<6, 9>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_jalr_hb, "jalr.hb",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)));
    }
} mips32_jalr_hb;

// JALX -- jump and link exchange
static struct Mips32_jalx: Mips32 {
    Mips32_jalx(): Mips32(Release1, sOP(035), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_jalx, "jalx",
                                  d->makeBranchTargetAbsolute(extract<0, 25>(ib), 0, 26));
    }
} mips32_jalx;

// JR -- jump register
static struct Mips32_jr: Mips32 {
    Mips32_jr(): Mips32(Release1, // "hint" field (bits 6-10) must be zero for Release1
                        sOP(000)|shift_to<11, 20>(0)|shift_to<6, 10>(0)|sFN(010),
                        mOP()   |mask_for<11, 20>() |mask_for<6, 10>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_jr, "jr",
                                  d->makeRegister(gR0(ib)));
    }
} mips32_jr;

// JR.HB -- jump register with hazard barrier
static struct Mips32_jr_hb: Mips32 {
    Mips32_jr_hb(): Mips32(Release2, // "hint field (bits 6-9) must be zero for Release1
                           sOP(000)|shift_to<11, 20>(0)|shift_to<10, 10>(1)|shift_to<6, 9>(0)|sFN(010),
                           mOP()   |mask_for<11, 20>() |mask_for<10, 10>() |mask_for<6, 9>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_jr_hb, "jr.hb",
                                  d->makeRegister(gR0(ib)));
    }
} mips32_jr_hb;

// LB -- load byte
static struct Mips32_lb: Mips32 {
    Mips32_lb(): Mips32(Release1, sOP(040), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lb, "lb",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I8()));
    }
} mips32_lb;

// LBE -- load byte EVA
static struct Mips32_lbe: Mips32 {
    Mips32_lbe(): Mips32(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(054),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_lbe, "lbe",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I8()));
    }
} mips32_lbe;

// LBU -- load byte unsigned
static struct Mips32_lbu: Mips32 {
    Mips32_lbu(): Mips32(Release1, sOP(044), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lbu, "lbu",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_U8()));
    }
} mips32_lbu;

// LBUE -- load byte unsigned EVA
static struct Mips32_lbue: Mips32 {
    Mips32_lbue(): Mips32(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(050),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_lbue, "lbue",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_U8()));
    }
} mips32_lbue;

// LDC1 -- load doubleword to floating point
static struct Mips32_ldc1: Mips32 {
    Mips32_ldc1(): Mips32(Release1, sOP(065), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_ldc1, "ldc1",
                                  d->makeFpRegister(gR1(ib)), d->makeMemoryReference(addr, type_B64()));
    }
} mips32_ldc1;

// LDC2 -- load doubleword to coprocessor 2
static struct Mips32_ldc2: Mips32 {
    Mips32_ldc2(): Mips32(Release1, sOP(066), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_ldc2, "ldc2",
                                  d->makeCp2Register(gR1(ib)), d->makeMemoryReference(addr, type_B64()));
    }
} mips32_ldc2;

// LDXC1 -- load doubleword indexed to floating point
static struct Mips32_ldxc1: Mips32 {
    Mips32_ldxc1(): Mips32(Release2,
                           sOP(023)|sR2(000)|sFN(001),
                           mOP()   |mR2()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(gR0(ib), gR1(ib));
        return d->makeInstruction(mips_ldxc1, "ldxc1",
                                  d->makeFpRegister(gR3(ib)), d->makeMemoryReference(addr, type_B64()));
    }
} mips32_ldxc1;

// LH -- load halfword
static struct Mips32_lh: Mips32 {
    Mips32_lh(): Mips32(Release1, sOP(041), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lh, "lh",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I16()));
    }
} mips32_lh;

// LHE -- load halfword EVA
static struct Mips32_lhe: Mips32 {
    Mips32_lhe(): Mips32(Release1,
                         sOP(037)|shift_to<6, 6>(1)|sFN(055),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_lhe, "lhe",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I16()));
    }
} mips32_lhe;

// LHU -- load halfword unsigned
static struct Mips32_lhu: Mips32 {
    Mips32_lhu(): Mips32(Release1, sOP(045), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lhu, "lhu",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_U16()));
    }
} mips32_lhu;

// LHUE -- load halfword unsigned EVA
static struct Mips32_lhue: Mips32 {
    Mips32_lhue(): Mips32(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(051),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_lhue, "lhue",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_U16()));
    }
} mips32_lhue;

// LL -- load linked word
static struct Mips32_ll: Mips32 {
    Mips32_ll(): Mips32(Release1, sOP(060), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_ll, "ll",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_ll;

// LLE -- load linked word EVA
static struct Mips32_lle: Mips32 {
    Mips32_lle(): Mips32(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(056),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_lle, "lle",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lle;

// LUI -- load upper immediate
static struct Mips32_lui: Mips32 {
    Mips32_lui(): Mips32(Release1,
                         sOP(017)|sR0(000),
                         mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_lui, "lui",
                                  d->makeRegister(gR1(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_lui;

// LUXC1 -- load doubleword indexed unaligned to floating point
static struct Mips32_luxc1: Mips32 {
    Mips32_luxc1(): Mips32(Release2,
                           sOP(023)|sR2(000)|sFN(005),
                           mOP()   |mR2()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_luxc1, "luxc1",
                                  d->makeFpRegister(gR3(ib)), d->makeRegisterIndexed(gR0(ib), gR1(ib)));
    }
} mips32_luxc1;

// LW -- load word
static struct Mips32_lw: Mips32 {
    Mips32_lw(): Mips32(Release1, sOP(043), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lw, "lw",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lw;

// LWC1 -- load word to floating point
static struct Mips32_lwc1: Mips32 {
    Mips32_lwc1(): Mips32(Release1, sOP(061), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lwc1, "lwc1",
                                  d->makeFpRegister(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_lwc1;

// LWC2 -- load word to coprocessor 2
static struct Mips32_lwc2: Mips32 {
    Mips32_lwc2(): Mips32(Release1, sOP(062), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lwc2, "lwc2",
                                  d->makeCp2Register(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_lwc2;

// LWE -- load word EVA
static struct Mips32_lwe: Mips32 {
    Mips32_lwe(): Mips32(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(057),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_lwe, "lwe",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwe;

// LWL -- load word left
static struct Mips32_lwl: Mips32 {
    Mips32_lwl(): Mips32(Release1, sOP(042), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lwl, "lwl",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwl;

// LWLE -- load word left EVA
static struct Mips32_lwle: Mips32 {
    Mips32_lwle(): Mips32(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(031),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_lwle, "lwle",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwle;

// LWR -- load word right
static struct Mips32_lwr: Mips32 {
    Mips32_lwr(): Mips32(Release1, sOP(046), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_lwr, "lwr",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwr;

// LWRE -- load word right EVA
static struct Mips32_lwre: Mips32 {
    Mips32_lwre(): Mips32(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(032),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_lwre, "lwre",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_lwre;

// LWXC1 -- load word indexed to floating point
static struct Mips32_lwxc1: Mips32 {
    Mips32_lwxc1(): Mips32(Release2,
                           sOP(023)|sR2(000)|sFN(000),
                           mOP()   |mR2()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(gR0(ib), gR1(ib));
        return d->makeInstruction(mips_lwxc1, "lwxc1",
                                  d->makeFpRegister(gR3(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_lwxc1;

// MADD -- multiply and add word to hi, lo
static struct Mips32_madd: Mips32 {
    Mips32_madd(): Mips32(Release1,
                          sOP(034)|sR2(000)|sR3(000)|sFN(000),
                          mOP()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_madd, "madd",
                                  d->makeFpRegister(gR0(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_madd;

// MADD.S -- floating point multiply add
static struct Mips32_madd_s: Mips32 {
    Mips32_madd_s(): Mips32(Release2,
                            sOP(023)|sFN(040),
                            mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_madd_s, "madd.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_madd_s;

// MADD.D -- floating point multiply add
static struct Mips32_madd_d: Mips32 {
    Mips32_madd_d(): Mips32(Release2,
                            sOP(23)|sFN(041),
                            mOP()  |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_madd_d, "madd.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_madd_d;

// MADD.PS -- floating point multiply add
static struct Mips32_madd_ps: Mips32 {
    Mips32_madd_ps(): Mips32(Release2,
                             sOP(23)|sFN(046),
                             mOP()  |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_madd_ps, "madd.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_madd_ps;

// MADDU -- multiply and add unsigned word to hi, lo
static struct Mips32_maddu: Mips32 {
    Mips32_maddu(): Mips32(Release1,
                           sOP(034)|sR2(000)|sR3(000)|sFN(001),
                           mOP()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_maddu, "maddu",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_maddu;

// MFC0 -- move from coprocessor 0
static struct Mips32_mfc0: Mips32 {
    Mips32_mfc0(): Mips32(Release1,
                          sOP(020)|sR0(000)|shift_to<3, 10>(0),
                          mOP()   |mR0()   |mask_for<3, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned sel = extract<0, 2>(ib);
        return d->makeInstruction(mips_mfc0, "mfc0",
                                  d->makeRegister(gR1(ib)), d->makeCp0Register(gR2(ib), sel),
                                  d->makeImmediate8(extract<0, 2>(ib), 0, 3));
    }
} mips32_mfc0;

// MFC1 -- move word from floating point
static struct Mips32_mfc1: Mips32 {
    Mips32_mfc1(): Mips32(Release1,
                          sOP(021)|sR0(000)|shift_to<0, 10>(0),
                          mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mfc1, "mfc1",
                                  d->makeRegister(gR1(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_mfc1;

// MFC2 -- move word from coprocessor 2
static struct Mips32_mfc2: Mips32 {
    Mips32_mfc2(): Mips32(Release1,
                          sOP(022)|sR0(000),
                          mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mfc2, "mfc2",
                                  d->makeRegister(gR1(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_mfc2;

// MFHC1 -- move word from high half of floating point register
static struct Mips32_mfhc1: Mips32 {
    Mips32_mfhc1(): Mips32(Release2,
                           sOP(021)|sR0(003)|shift_to<0, 10>(0),
                           mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mfhc1, "mfhc1",
                                  d->makeRegister(gR1(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_mfhc1;

// MFHC2 -- move word from high half of coprocessor 2 register
static struct Mips32_mfhc2: Mips32 {
    Mips32_mfhc2(): Mips32(Release2,
                           sOP(022)|sR0(003),
                           mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mfhc2, "mfhc2",
                                  d->makeRegister(gR1(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_mfhc2;

// MFHI -- move from hi register
static struct Mips32_mfhi: Mips32 {
    Mips32_mfhi(): Mips32(Release1,
                          sOP(000)|sR0(000)|sR1(000)|sR3(000)|sFN(020),
                          mOP()   |mR0()   |mR1()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mfhi, "mfhi",
                                  d->makeRegister(gR2(ib)));
    }
} mips32_mfhi;

// MFLO -- move from lo register
static struct Mips32_mflo: Mips32 {
    Mips32_mflo(): Mips32(Release1,
                          sOP(000)|sR0(000)|sR1(000)|sR3(000)|sFN(022),
                          mOP()   |mR0()   |mR1()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mflo, "mflo",
                                  d->makeRegister(gR2(ib)));
    }
} mips32_mflo;

// MOV.S -- floating point move
static struct Mips32_mov_s: Mips32 {
    Mips32_mov_s(): Mips32(Release1,
                           sOP(021)|sR0(020)|sR1(000)|sFN(006),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mov_s, "mov.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_mov_s;

// MOV.D -- floating point move
static struct Mips32_mov_d: Mips32 {
    Mips32_mov_d(): Mips32(Release1,
                           sOP(021)|sR0(021)|sR1(000)|sFN(006),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mov_d, "mov.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_mov_d;

// MOV.PS -- floating point move
static struct Mips32_mov_ps: Mips32 {
    Mips32_mov_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sR1(000)|sFN(006),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mov_ps, "mov.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_mov_ps;

// MOVF -- move conditional on floating point false
static struct Mips32_movf: Mips32 {
    Mips32_movf(): Mips32(Release1,
                          sOP(000)|shift_to<16, 17>(0)|sR3(000)|sFN(001),
                          mOP()   |mask_for<16, 17>() |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movf, "movf",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movf;

// MOVF.S -- floating point move conditional on floating point false
static struct Mips32_movf_s: Mips32 {
    Mips32_movf_s(): Mips32(Release1,
                            sOP(021)|sR0(020)|shift_to<16, 17>(0)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movf_s, "movf.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movf_s;

// MOVF.D -- floating point move conditional on floating point false
static struct Mips32_movf_d: Mips32 {
    Mips32_movf_d(): Mips32(Release1,
                            sOP(021)|sR0(021)|shift_to<16, 17>(0)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movf_d, "movf.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movf_d;

// MOVF.PS -- floating point move conditional on floating point false
static struct Mips32_movf_ps: Mips32 {
    Mips32_movf_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|shift_to<16, 17>(0)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movf_ps, "movf.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movf_ps;

// MOVN -- move conditional on not zero
static struct Mips32_movn: Mips32 {
    Mips32_movn(): Mips32(Release1,
                          sOP(000)|sR3(000)|sFN(013),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movn, "movn",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR2(ib)));
    }
} mips32_movn;

// MOVN.S -- move floating point conditional on not zero
static struct Mips32_movn_s: Mips32 {
    Mips32_movn_s(): Mips32(Release1,
                            sOP(021)|sR0(020)|sFN(023),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movn_s, "movn.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movn_s;

// MOVN.D -- move floating point conditional on not zero
static struct Mips32_movn_d: Mips32 {
    Mips32_movn_d(): Mips32(Release1,
                            sOP(021)|sR0(021)|sFN(023),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movn_d, "movn.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movn_d;

// MOVN.PS -- move floating point conditional on not zero
static struct Mips32_movn_ps: Mips32 {
    Mips32_movn_ps(): Mips32(Release2,
                             sOP(021)|sR0(026)|sFN(023),
                             mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movn_ps, "movn.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movn_ps;

// MOVT -- move conditional on floating point true
static struct Mips32_movt: Mips32 {
    Mips32_movt(): Mips32(Release1,
                          sOP(000)|shift_to<16, 17>(1)|sR3(000)|sFN(001),
                          mOP()   |mask_for<16, 17>() |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movt, "movt",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movt;

// MOVT.S -- floating point move conditional on floating point true
static struct Mips32_movt_s: Mips32 {
    Mips32_movt_s(): Mips32(Release1,
                            sOP(021)|sR0(020)|shift_to<16, 17>(1)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movt_s, "movt.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movt_s;

// MOVT.D -- floating point move conditional on floating point true
static struct Mips32_movt_d: Mips32 {
    Mips32_movt_d(): Mips32(Release1,
                            sOP(021)|sR0(021)|shift_to<16, 17>(1)|sFN(021),
                            mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movt_d, "movt.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movt_d;

// MOVT.PS -- floating point move conditional on floating point true
static struct Mips32_movt_ps: Mips32 {
    Mips32_movt_ps(): Mips32(Release2,
                             sOP(021)|sR0(026)|shift_to<16, 17>(1)|sFN(021),
                             mOP()   |mR0()   |mask_for<16, 17>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movt_ps, "movt.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)),
                                  d->makeFpccRegister(extract<18, 20>(ib)));
    }
} mips32_movt_ps;

// MOVZ -- move conditional on zero
static struct Mips32_movz: Mips32 {
    Mips32_movz(): Mips32(Release1,
                          sOP(000)|sR3(000)|sFN(012),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movz, "movz",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_movz;

// MOVZ.S -- floating point move conditional on zero
static struct Mips32_movz_s: Mips32 {
    Mips32_movz_s(): Mips32(Release1,
                            sOP(021)|sR0(020)|sFN(022),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movz_s, "movz.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_movz_s;

// MOVZ.D -- floating point move conditional on zero
static struct Mips32_movz_d: Mips32 {
    Mips32_movz_d(): Mips32(Release1,
                            sOP(021)|sR0(021)|sFN(022),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movz_d, "movz.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_movz_d;

// MOVZ.PS -- floating point move conditional on zero
static struct Mips32_movz_ps: Mips32 {
    Mips32_movz_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sFN(022),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_movz_ps, "movz.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_movz_ps;

// MSUB -- multiply and subtract word
static struct Mips32_msub: Mips32 {
    Mips32_msub(): Mips32(Release1,
                          sOP(034)|sR2(000)|sR3(000)|sFN(004),
                          mOP()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_msub, "msub",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_msub;

// MSUB.S -- floating point multiply subtract
static struct Mips32_msub_s: Mips32 {
    Mips32_msub_s(): Mips32(Release2,
                            sOP(023)|sFN(050),
                            mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_msub_s, "msub.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_msub_s;

// MSUB.D -- floating point multiply subtract
static struct Mips32_msub_d: Mips32 {
    Mips32_msub_d(): Mips32(Release2,
                            sOP(023)|sFN(051),
                            mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_msub_d, "msub.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_msub_d;

// MSUB.PS -- floating point multiply subtract
static struct Mips32_msub_ps: Mips32 {
    Mips32_msub_ps(): Mips32(Release2,
                             sOP(023)|sFN(056),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_msub_ps, "msub.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_msub_ps;

// MSUBU -- multiply and subtract word to hi, lo
static struct Mips32_msubu: Mips32 {
    Mips32_msubu(): Mips32(Release1,
                           sOP(034)|sR2(000)|sR3(000)|sFN(005),
                           mOP()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_msubu, "msubu",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_msubu;

// MTC0 -- move to coprocessor 0
static struct Mips32_mtc0: Mips32 {
    Mips32_mtc0(): Mips32(Release1,
                          sOP(020)|sR0(004)|shift_to<3, 10>(0),
                          mOP()   |mR0()   |mask_for<3, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned sel = extract<0, 2>(ib);
        return d->makeInstruction(mips_mtc0, "mtc0",
                                  d->makeRegister(gR1(ib)), d->makeCp0Register(gR2(ib), sel),
                                  d->makeImmediate8(extract<0, 2>(ib), 0, 3));
    }
} mips32_mtc0;

// MTC1 -- move word to floating point
static struct Mips32_mtc1: Mips32 {
    Mips32_mtc1(): Mips32(Release1,
                          sOP(021)|sR0(004)|shift_to<0, 10>(0),
                          mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mtc1, "mtc1",
                                  d->makeRegister(gR1(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_mtc1;

// MTC2 -- move word to coprocessor 2
static struct Mips32_mtc2: Mips32 {
    Mips32_mtc2(): Mips32(Release1,
                          sOP(022)|sR0(004),
                          mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mtc2, "mtc2",
                                  d->makeRegister(gR1(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_mtc2;

// MTHC1 -- move word to high lalf of floating point register
static struct Mips32_mthc1: Mips32 {
    Mips32_mthc1(): Mips32(Release2,
                           sOP(021)|sR0(007)|shift_to<0, 10>(0),
                           mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mthc1, "mthc1",
                                  d->makeRegister(gR1(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_mthc1;

// MTHC2 -- move word to high half of coprocessor 2 register
static struct Mips32_mthc2: Mips32 {
    Mips32_mthc2(): Mips32(Release2,
                           sOP(022)|sR0(007),
                           mOP()   |mR0()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mthc2, "mthc2",
                                  d->makeRegister(gR1(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_mthc2;

// MTHI -- move to hi register
static struct Mips32_mthi: Mips32 {
    Mips32_mthi(): Mips32(Release1,
                          sOP(000)|shift_to<6, 20>(0)|sFN(021),
                          mOP()   |mask_for<6, 20>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mthi, "mthi",
                                  d->makeRegister(gR0(ib)));
    }
} mips32_mthi;

// MTLO -- move to lo register
static struct Mips32_mtlo: Mips32 {
    Mips32_mtlo(): Mips32(Release1,
                          sOP(000)|shift_to<6, 20>(0)|sFN(023),
                          mOP()   |mask_for<6, 20>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mtlo, "mtlo",
                                  d->makeRegister(gR0(ib)));
    }
} mips32_mtlo;

// MUL -- multiply word to GPR
static struct Mips32_mul: Mips32 {
    Mips32_mul(): Mips32(Release1,
                         sOP(034)|sR3(000)|sFN(002),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mul, "mul",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_mul;

// MUL.S -- floating point multiply
static struct Mips32_mul_s: Mips32 {
    Mips32_mul_s(): Mips32(Release1,
                           sOP(021)|sR0(020)|sFN(002),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mul_s, "mul.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_mul_s;

// MUL.D -- floating point multiply
static struct Mips32_mul_d: Mips32 {
    Mips32_mul_d(): Mips32(Release1,
                           sOP(021)|sR0(021)|sFN(002),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mul_d, "mul.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_mul_d;

// MUL.PS -- floating point multiply
static struct Mips32_mul_ps: Mips32 {
    Mips32_mul_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sFN(002),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mul_ps, "mul.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_mul_ps;

// MULT -- multiply word
static struct Mips32_mult: Mips32 {
    Mips32_mult(): Mips32(Release1,
                          sOP(000)|shift_to<6, 15>(0)|sFN(030),
                          mOP()   |mask_for<6, 15>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_mult, "mult",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_mult;

// MULTU -- multiply unsigned word
static struct Mips32_multu: Mips32 {
    Mips32_multu(): Mips32(Release1,
                           sOP(000)|shift_to<6, 15>(0)|sFN(031),
                           mOP()   |mask_for<6, 15>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_multu, "multu",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_multu;

// NEG.S -- floating point negate
static struct Mips32_neg_s: Mips32 {
    Mips32_neg_s(): Mips32(Release1,
                           sOP(021)|sR0(020)|sR1(000)|sFN(007),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_neg_s, "neg.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_neg_s;

// NEG.D -- floating point negate
static struct Mips32_neg_d: Mips32 {
    Mips32_neg_d(): Mips32(Release1,
                           sOP(021)|sR0(021)|sR1(000)|sFN(007),
                           mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_neg_d, "neg.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_neg_d;

// NEG.PS -- floating point negate
static struct Mips32_neg_ps: Mips32 {
    Mips32_neg_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sR1(000)|sFN(007),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_neg_ps, "neg.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_neg_ps;

// NMADD.S -- floating point negative multiply add
static struct Mips32_nmadd_s: Mips32 {
    Mips32_nmadd_s(): Mips32(Release2,
                             sOP(023)|sFN(060),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_nmadd_s, "nmadd.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_nmadd_s;

// NMADD.D -- floating point negative multiply add
static struct Mips32_nmadd_d: Mips32 {
    Mips32_nmadd_d(): Mips32(Release2,
                             sOP(023)|sFN(061),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_nmadd_d, "nmadd.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_nmadd_d;

// NMADD.PS -- floating point negative multiply add
static struct Mips32_nmadd_ps: Mips32 {
    Mips32_nmadd_ps(): Mips32(Release2,
                              sOP(023)|sFN(066),
                              mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_nmadd_ps, "nmadd.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_nmadd_ps;

// NMSUB.S -- floating point negative multiply subtract
static struct Mips32_nmsub_s: Mips32 {
    Mips32_nmsub_s(): Mips32(Release2,
                             sOP(023)|sFN(070),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_nmsub_s, "nmsub.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_nmsub_s;

// NMSUB.D -- floating point negative multiply subtract
static struct Mips32_nmsub_d: Mips32 {
    Mips32_nmsub_d(): Mips32(Release2,
                             sOP(023)|sFN(071),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_nmsub_d, "nmsub.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_nmsub_d;

// NMSUB.PS -- floating point negative multiply subtract
static struct Mips32_nmsub_ps: Mips32 {
    Mips32_nmsub_ps(): Mips32(Release2,
                              sOP(023)|sFN(076),
                              mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_nmsub_ps, "nmsub.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR0(ib)),
                                  d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_nmsub_ps;

// NOR -- not or
static struct Mips32_nor: Mips32 {
    Mips32_nor(): Mips32(Release1,
                         sOP(000)|sR3(000)|sFN(047),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_nor, "nor",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_nor;

// OR -- bitwise or
static struct Mips32_or: Mips32 {
    Mips32_or(): Mips32(Release1,
                        sOP(000)|sR3(000)|sFN(045),
                        mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_or, "or",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_or;

// ORI -- bitwise OR immediate
static struct Mips32_ori: Mips32 {
    Mips32_ori(): Mips32(Release1, sOP(015), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_ori, "ori",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_ori;

// PAUSE -- wait for the LLBit to clear
static struct Mips32_pause: Mips32 {
    Mips32_pause(): Mips32(Release2,
                           sOP(000)|sR0(000)|sR1(000)|sR2(000)|sR3(005)|sFN(000),
                           mOP()   |mR0()   |mR1()   |mR2()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_pause, "pause");
    }
} mips32_pause;

// PLL.PS -- pair lower lower
static struct Mips32_pll_ps: Mips32 {
    Mips32_pll_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sFN(054),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_pll_ps, "pll.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_pll_ps;

// PLU.PS -- pair lower upper
static struct Mips32_plu_ps: Mips32 {
    Mips32_plu_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sFN(055),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_plu_ps, "plu.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_plu_ps;

// PREF -- prefetch
static struct Mips32_pref: Mips32 {
    Mips32_pref(): Mips32(Release1, sOP(063), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_pref, "pref",
                                  d->makeImmediate8(gR1(ib), 16, 5), d->makeRegisterOffset(gR0(ib), gIM(ib)));
    }
} mips32_pref;

// PREFE -- prefetch EVA
static struct Mips32_prefe: Mips32 {
    Mips32_prefe(): Mips32(Release1,
                           sOP(037)|shift_to<6, 6>(0)|sFN(043),
                           mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        return d->makeInstruction(mips_prefe, "prefe",
                                  d->makeImmediate8(gR1(ib), 16, 5), d->makeRegisterOffset(gR0(ib), offset16));
    }
} mips32_prefe;

// PREFX -- prefetch indexed
static struct Mips32_prefx: Mips32 {
    Mips32_prefx(): Mips32(Release2,
                           sOP(023)|sR3(000)|sFN(017),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_prefx, "prefx",
                                  d->makeImmediate8(gR2(ib), 11, 5), d->makeRegisterIndexed(gR0(ib), gR1(ib)));
    }
} mips32_prefx;

// PUL.PS -- pair upper lower
static struct Mips32_pul_ps: Mips32 {
    Mips32_pul_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sFN(056),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_pul_ps, "pul.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_pul_ps;

// PUU.PS -- pair upper upper
static struct Mips32_puu_ps: Mips32 {
    Mips32_puu_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sFN(057),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_puu_ps, "puu.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_puu_ps;

// RDHWR -- read hardware register
static struct Mips32_rdhwr: Mips32 {
    Mips32_rdhwr(): Mips32(Release2,
                           sOP(037)|sR0(000)|sR3(000)|sFN(073),
                           mOP()   |mR0()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_rdhwr, "rdhwr",
                                  d->makeRegister(gR1(ib)), d->makeHwRegister(gR2(ib)));
    }
} mips32_rdhwr;

// RDPGPR -- read GPR from previous shadow set
static struct Mips32_rdpgpr: Mips32 {
    Mips32_rdpgpr(): Mips32(Release2,
                            sOP(020)|sR0(012)|shift_to<0, 10>(0),
                            mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_rdpgpr, "rdpgpr",
                                  d->makeRegister(gR2(ib)), d->makeShadowRegister(gR1(ib)));
    }
} mips32_rdpgpr;

// RECIP.S -- reciprocal approximation
static struct Mips32_recip_s: Mips32 {
    Mips32_recip_s(): Mips32(Release2,
                             sOP(021)|sR0(020)|sR1(000)|sFN(025),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_recip_s, "recip.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_recip_s;

// RECIP.D -- reciprocal approximation
static struct Mips32_recip_d: Mips32 {
    Mips32_recip_d(): Mips32(Release2,
                             sOP(021)|sR0(021)|sR1(000)|sFN(025),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_recip_d, "recip.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_recip_d;

// ROTR -- rotate word right
static struct Mips32_rotr: Mips32 {
    Mips32_rotr(): Mips32(Release2,
                          sOP(000)|sR0(001)|sFN(002),
                          mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_rotr, "rotr",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)), d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_rotr;

// ROTRV -- rotate word right variable
static struct Mips32_rotrv: Mips32 {
    Mips32_rotrv(): Mips32(Release2,
                           sOP(000)|sR3(001)|sFN(006),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_rotrv, "rotrv",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)));
    }
} mips32_rotrv;

// ROUND.L.S -- floating point round to long fixed point
static struct Mips32_round_l_s: Mips32 {
    Mips32_round_l_s(): Mips32(Release2,
                               sOP(021)|sR0(020)|sR1(000)|sFN(010),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_round_l_s, "round.l.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_round_l_s;

// ROUND.L.D -- floating point round to long fixed point
static struct Mips32_round_l_d: Mips32 {
    Mips32_round_l_d(): Mips32(Release2,
                               sOP(021)|sR0(021)|sR1(000)|sFN(010),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_round_l_d, "round.l.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_round_l_d;

// ROUND.W.S -- floating point round to word fixed point
static struct Mips32_round_w_s: Mips32 {
    Mips32_round_w_s(): Mips32(Release1,
                               sOP(021)|sR0(020)|sR1(000)|sFN(014),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_round_w_s, "round.w.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_round_w_s;

// ROUND.W.D -- floating point round to word fixed point
static struct Mips32_round_w_d: Mips32 {
    Mips32_round_w_d(): Mips32(Release1,
                               sOP(021)|sR0(021)|sR1(000)|sFN(014),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_round_w_d, "round.w.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_round_w_d;

// RSQRT.S -- reciprocal square root approximation
static struct Mips32_rsqrt_s: Mips32 {
    Mips32_rsqrt_s(): Mips32(Release2,
                             sOP(021)|sR0(020)|sR1(000)|sFN(026),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_rsqrt_s, "rsqrt.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_rsqrt_s;

// RSQRT.D -- reciprocal square root approximation
static struct Mips32_rsqrt_d: Mips32 {
    Mips32_rsqrt_d(): Mips32(Release2,
                             sOP(021)|sR0(021)|sR1(000)|sFN(026),
                             mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_rsqrt_d, "rsqrt.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_rsqrt_d;

// SB -- store byte
static struct Mips32_sb: Mips32 {
    Mips32_sb(): Mips32(Release1, sOP(050), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_sb, "sb",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B8()));
    }
} mips32_sb;

// SBE -- store byte EVA
static struct Mips32_sbe: Mips32 {
    Mips32_sbe(): Mips32(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(034),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_sbe, "sbe",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B8()));
    }
} mips32_sbe;

// SC -- store conditional word
static struct Mips32_sc: Mips32 {
    Mips32_sc(): Mips32(Release1, sOP(070), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_sc, "sc",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_sc;

// SCE -- store conditional word EVA
static struct Mips32_sce: Mips32 {
    Mips32_sce(): Mips32(Release1,
                         sOP(047)|shift_to<6, 6>(0)|sFN(036),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_sce, "sce",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_I32()));
    }
} mips32_sce;

// SDC1 -- store doubleword from floating point
static struct Mips32_sdc1: Mips32 {
    Mips32_sdc1(): Mips32(Release1, sOP(075), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_sdc1, "sdc1",
                                  d->makeFpRegister(gR1(ib)), d->makeMemoryReference(addr, type_B64()));
    }
} mips32_sdc1;

// SDC2 -- store doubleword from coprocessor 2
static struct Mips32_sdc2: Mips32 {
    Mips32_sdc2(): Mips32(Release1, sOP(076), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_sdc2, "sdc2",
                                  d->makeCp2Register(gR1(ib)), d->makeMemoryReference(addr, type_B64()));
    }
} mips32_sdc2;

// SDXC1 -- store doubleword indexed from floating point
static struct Mips32_sdxc1: Mips32 {
    Mips32_sdxc1(): Mips32(Release2,
                           sOP(023)|sR3(000)|sFN(011),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(gR0(ib), gR1(ib));
        return d->makeInstruction(mips_sdxc1, "sdxc1",
                                  d->makeFpRegister(gR2(ib)), d->makeMemoryReference(addr, type_B64()));
    }
} mips32_sdxc1;

// SEB -- sign extend byte
static struct Mips32_seb: Mips32 {
    Mips32_seb(): Mips32(Release2,
                         sOP(037)|sR0(000)|sR3(020)|sFN(040),
                         mOP()   |mR0()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_seb, "seb",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_seb;

// SEH -- sign extend halfword
static struct Mips32_seh: Mips32 {
    Mips32_seh(): Mips32(Release2,
                         sOP(037)|sR0(000)|sR3(030)|sFN(040),
                         mOP()   |mR0()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_seh, "seh",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_seh;

// SH -- store halfword
static struct Mips32_sh: Mips32 {
    Mips32_sh(): Mips32(Release1, sOP(051), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_sh, "sh",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B16()));
    }
} mips32_sh;

// SHE -- store halfword EVA
static struct Mips32_she: Mips32 {
    Mips32_she(): Mips32(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(035),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_she, "she",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B16()));
    }
} mips32_she;

// SLL -- shift word left logical
// NOP -- no operation (when "SSL r0, r0, 0"); an assembly macro
// SSNOP -- superscalar no operation (when "SLL r0, r0, 1")
static struct Mips32_sll: Mips32 {
    Mips32_sll(): Mips32(Release1,
                         sOP(000)|sR0(000)|sFN(000),
                         mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        if (gR1(ib)==0 && gR2(ib)==0 && gR3(ib)==0)
            return d->makeInstruction(mips_nop, "nop");
        if (gR1(ib)==0 && gR2(ib)==0 && gR3(ib)==1)
            return d->makeInstruction(mips_ssnop, "ssnop");
        return d->makeInstruction(mips_sll, "sll",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)), d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_sll;

// SLLV -- shift word left logical variable
static struct Mips32_sllv: Mips32 {
    Mips32_sllv(): Mips32(Release1,
                          sOP(000)|sR3(000)|sFN(004),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sllv, "sllv",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)));
    }
} mips32_sllv;

// SLT -- set on less than
static struct Mips32_slt: Mips32 {
    Mips32_slt(): Mips32(Release1,
                         sOP(000)|sR3(000)|sFN(052),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_slt, "slt",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_slt;

// SLTI -- set on less than immediate
static struct Mips32_slti: Mips32 {
    Mips32_slti(): Mips32(Release1, sOP(012), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_slti, "slti",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_slti;

// SLTIU -- set on less than immediate unsigned
static struct Mips32_sltiu: Mips32 {
    Mips32_sltiu(): Mips32(Release1, sOP(013), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sltiu, "sltiu",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_sltiu;

// SLTU -- set on less than unsigned
static struct Mips32_sltu: Mips32 {
    Mips32_sltu(): Mips32(Release1,
                          sOP(000)|sR3(000)|sFN(053),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sltu, "sltu",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_sltu;

// SQRT.S -- floating point square root
static struct Mips32_sqrt_s: Mips32 {
    Mips32_sqrt_s(): Mips32(Release1,
                            sOP(021)|sR0(020)|sR1(000)|sFN(004),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sqrt_s, "sqrt.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_sqrt_s;

// SQRT.D -- floating point square root
static struct Mips32_sqrt_d: Mips32 {
    Mips32_sqrt_d(): Mips32(Release1,
                            sOP(021)|sR0(021)|sR1(000)|sFN(004),
                            mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sqrt_d, "sqrt.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_sqrt_d;

// SRA -- shift word right arithmetic
static struct Mips32_sra: Mips32 {
    Mips32_sra(): Mips32(Release1,
                         sOP(000)|sR0(000)|sFN(003),
                         mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sra, "sra",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)), d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_sra;

// SRAV -- shift word right arithmetic variable
static struct Mips32_srav: Mips32 {
    Mips32_srav(): Mips32(Release1,
                          sOP(000)|sR3(000)|sFN(007),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_srav, "srav",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)));
    }
} mips32_srav;

// SRL -- shift word right logical
static struct Mips32_srl: Mips32 {
    Mips32_srl(): Mips32(Release1,
                         sOP(000)|sR0(000)|sFN(002),
                         mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_srl, "srl",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)), d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_srl;

// SRLV -- shift word right logical variable
static struct Mips32_srlv: Mips32 {
    Mips32_srlv(): Mips32(Release1,
                          sOP(000)|sR3(000)|sFN(006),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_srlv, "srlv",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)));
    }
} mips32_srlv;

// SUB -- subtract word
static struct Mips32_sub: Mips32 {
    Mips32_sub(): Mips32(Release1,
                         sOP(000)|sR3(000)|sFN(042),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sub, "sub",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_sub;

// SUB.S -- subtract floating point
static struct Mips32_sub_s: Mips32 {
    Mips32_sub_s(): Mips32(Release1,
                           sOP(021)|sR0(020)|sFN(001),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sub_s, "sub.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_sub_s;

// SUB.D -- subtract floating point
static struct Mips32_sub_d: Mips32 {
    Mips32_sub_d(): Mips32(Release1,
                           sOP(021)|sR0(021)|sFN(001),
                           mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sub_d, "sub.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_sub_d;

// SUB.PS -- subtract floating point
static struct Mips32_sub_ps: Mips32 {
    Mips32_sub_ps(): Mips32(Release2,
                            sOP(021)|sR0(026)|sFN(001),
                            mOP()   |mR0()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sub_ps, "sub.ps",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)), d->makeFpRegister(gR1(ib)));
    }
} mips32_sub_ps;

// SUBU -- subtract unsigned word
static struct Mips32_subu: Mips32 {
    Mips32_subu(): Mips32(Release1,
                          sOP(000)|sR3(000)|sFN(043),
                          mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_subu, "subu",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_subu;

// SUXC1 -- store doubleword indexed unaligned from floating point
static struct Mips32_suxc1: Mips32 {
    Mips32_suxc1(): Mips32(Release2,
                           sOP(023)|sR3(000)|sFN(015),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(gR0(ib), gR1(ib));
        return d->makeInstruction(mips_suxc1, "suxc1",
                                  d->makeFpRegister(gR2(ib)), d->makeMemoryReference(addr, type_B64()));
    }
} mips32_suxc1;

// SW -- store word
static struct Mips32_sw: Mips32 {
    Mips32_sw(): Mips32(Release1, sOP(053), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_sw, "sw",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_sw;

// SWC1 -- store word from floating point
static struct Mips32_swc1: Mips32 {
    Mips32_swc1(): Mips32(Release1, sOP(071), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_swc1, "swc1",
                                  d->makeFpRegister(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swc1;

// SWC2 -- store word from coprocessor 2
static struct Mips32_swc2: Mips32 {
    Mips32_swc2(): Mips32(Release1, sOP(072), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_swc2, "swc2",
                                  d->makeCp2Register(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swc2;

// SWE -- store word EVA
static struct Mips32_swe: Mips32 {
    Mips32_swe(): Mips32(Release1,
                         sOP(037)|shift_to<6, 6>(0)|sFN(037),
                         mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_swe, "swe",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swe;

// SWL -- store word left
static struct Mips32_swl: Mips32 {
    Mips32_swl(): Mips32(Release1, sOP(052), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_swl, "swl",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swl;

// SWLE -- store word left EVA
static struct Mips32_swle: Mips32 {
    Mips32_swle(): Mips32(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(041),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_swle, "swle",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swle;

// SWR -- store word right
static struct Mips32_swr: Mips32 {
    Mips32_swr(): Mips32(Release1, sOP(056), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), gIM(ib));
        return d->makeInstruction(mips_swr, "swr",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swr;

// SWRE -- store word right EVA
static struct Mips32_swre: Mips32 {
    Mips32_swre(): Mips32(Release1,
                          sOP(037)|shift_to<6, 6>(0)|sFN(042),
                          mOP()   |mask_for<6, 6>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned offset16 = signExtend<9, 16>(extract<7, 15>(ib));
        SgAsmExpression *addr = d->makeRegisterOffset(gR0(ib), offset16);
        return d->makeInstruction(mips_swre, "swre",
                                  d->makeRegister(gR1(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swre;

// SWXC1 -- store word indexed from floating point
static struct Mips32_swxc1: Mips32 {
    Mips32_swxc1(): Mips32(Release2,
                           sOP(023)|sR3(000)|sFN(010),
                           mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        SgAsmExpression *addr = d->makeRegisterIndexed(gR0(ib), gR1(ib));
        return d->makeInstruction(mips_swxc1, "swxc1",
                                  d->makeFpRegister(gR2(ib)), d->makeMemoryReference(addr, type_B32()));
    }
} mips32_swxc1;

// SYNC -- synchronize
// The "SYNC" form is the same as "SYNC stype" when stype==0
static struct Mips32_sync: Mips32 {
    Mips32_sync(): Mips32(Release1,
                          sOP(000)|shift_to<11, 25>(0)|sFN(017),
                          mOP()   |mask_for<11, 25>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_sync, "sync",
                                  d->makeImmediate8(gR3(ib), 6, 5));
    }
} mips32_sync;

// SYNCI -- synchronize caches to make instruction writes effective
static struct Mips32_synci: Mips32 {
    Mips32_synci(): Mips32(Release2,
                           sOP(001)|sR1(037),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_synci, "synci",
                                  d->makeRegisterOffset(gR0(ib), gIM(ib)));
    }
} mips32_synci;

// SYSCALL -- system call
static struct Mips32_syscall: Mips32 {
    Mips32_syscall(): Mips32(Release1,
                             sOP(000)|sFN(014),
                             mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 25>(ib);
        return d->makeInstruction(mips_syscall, "syscall");
    }
} mips32_syscall;

// TEQ -- trap if equal
static struct Mips32_teq: Mips32 {
    Mips32_teq(): Mips32(Release1,
                         sOP(000)|sFN(064),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(mips_teq, "teq",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_teq;

// TEQI -- trap if equal immediate
static struct Mips32_teqi: Mips32 {
    Mips32_teqi(): Mips32(Release1,
                          sOP(001)|sR1(014),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_teqi, "teqi",
                                  d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_teqi;

// TGE -- trap if greater or equal
static struct Mips32_tge: Mips32 {
    Mips32_tge(): Mips32(Release1,
                         sOP(000)|sFN(060),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(mips_tge, "tge",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_tge;

// TGEI -- trap greater or equal immediate
static struct Mips32_tgei: Mips32 {
    Mips32_tgei(): Mips32(Release1,
                          sOP(001)|sR1(010),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tgei, "tgei",
                                  d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tgei;

// TGEIU -- trap greater or equal immediate unsigned
static struct Mips32_tgeiu: Mips32 {
    Mips32_tgeiu(): Mips32(Release1,
                           sOP(001)|sR1(011),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tgeiu, "tgeiu",
                                  d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tgeiu;

// TGEU -- trap greater or equal unsigned
static struct Mips32_tgeu: Mips32 {
    Mips32_tgeu(): Mips32(Release1,
                          sOP(000)|sFN(061),
                          mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(mips_tgeu, "tgeu",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_tgeu;

// TLBINV -- TLB invalidate
static struct Mips32_tlbinv: Mips32 {
    Mips32_tlbinv(): Mips32(Release1,
                            sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(003),
                            mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tlbinv, "tlbinv");
    }
} mips32_tlbinv;

// TLBINVF -- TLB invalidate flush
static struct Mips32_tlbinvf: Mips32 {
    Mips32_tlbinvf(): Mips32(Release1,
                             sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(004),
                             mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tlbinvf, "tlbinvf");
    }
} mips32_tlbinvf;

// TLBP -- probe TLB for matching entry
static struct Mips32_tlbp: Mips32 {
    Mips32_tlbp(): Mips32(Release1,
                          sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(010),
                          mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tlbp, "tlbp");
    }
} mips32_tlbp;

// TLBR -- read indexed TLB entry
static struct Mips32_tlbr: Mips32 {
    Mips32_tlbr(): Mips32(Release1,
                          sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(001),
                          mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tlbr, "tlbr");
    }
} mips32_tlbr;

// TLBWI -- write indexed TLB entry
static struct Mips32_tlbwi: Mips32 {
    Mips32_tlbwi(): Mips32(Release1,
                           sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(002),
                           mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tlbwi, "tlbwi");
    }
} mips32_tlbwi;

// TLBWR -- write random TLB entry
static struct Mips32_tlbwr: Mips32 {
    Mips32_tlbwr(): Mips32(Release1,
                           sOP(020)|shift_to<25, 25>(1)|shift_to<6, 24>(0)|sFN(006),
                           mOP()   |mask_for<25, 25>() |mask_for<6, 24>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tlbwr, "tlbwr");
    }
} mips32_tlbwr;

// TLT -- trap if less than
static struct Mips32_tlt: Mips32 {
    Mips32_tlt(): Mips32(Release1,
                         sOP(000)|sFN(062),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(mips_tlt, "tlt",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_tlt;

// TLTI -- trap less than immediate
static struct Mips32_tlti: Mips32 {
    Mips32_tlti(): Mips32(Release1,
                          sOP(001)|sR1(012),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tlti, "tlti",
                                  d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tlti;

// TLTIU -- trap less than immediate unsigned
static struct Mips32_tltiu: Mips32 {
    Mips32_tltiu(): Mips32(Release1,
                           sOP(001)|sR1(013),
                           mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tltiu, "tltiu",
                                  d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tltiu;

// TLTU -- trap if less than unsigned
static struct Mips32_tltu: Mips32 {
    Mips32_tltu(): Mips32(Release1,
                          sOP(000)|sFN(063),
                          mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(mips_tltu, "tltu",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_tltu;

// TNE -- trap if not equal
static struct Mips32_tne: Mips32 {
    Mips32_tne(): Mips32(Release1,
                         sOP(000)|sFN(066),
                         mOP()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned code UNUSED_VAR = extract<6, 15>(ib);
        return d->makeInstruction(mips_tne, "tne",
                                  d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_tne;

// TNEI -- trap if not equal immediate
static struct Mips32_tnei: Mips32 {
    Mips32_tnei(): Mips32(Release1,
                          sOP(001)|sR1(016),
                          mOP()   |mR1()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_tnei, "tnei",
                                  d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_tnei;

// TRUNC.L.S -- floating point truncate to long fixed point
static struct Mips32_trunc_l_s: Mips32 {
    Mips32_trunc_l_s(): Mips32(Release2,
                               sOP(021)|sR0(020)|sR1(000)|sFN(011),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_trunc_l_s, "tunc.l.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_trunc_l_s;

// TRUNC.L.D -- floating point truncate to long fixed point
static struct Mips32_trunc_l_d: Mips32 {
    Mips32_trunc_l_d(): Mips32(Release2,
                               sOP(021)|sR0(021)|sR1(000)|sFN(011),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_trunc_l_d, "tunc.l.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_trunc_l_d;

// TRUNC.W.S -- floating point truncate to word fixed point
static struct Mips32_trunc_w_s: Mips32 {
    Mips32_trunc_w_s(): Mips32(Release1,
                               sOP(021)|sR0(020)|sR1(000)|sFN(015),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_trunc_w_s, "trunc.w.s",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_trunc_w_s;

// TRUNC.W.D -- floating point truncate to word fixed point
static struct Mips32_trunc_w_d: Mips32 {
    Mips32_trunc_w_d(): Mips32(Release1,
                               sOP(021)|sR0(021)|sR1(000)|sFN(015),
                               mOP()   |mR0()   |mR1()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_trunc_w_d, "trunc.w.d",
                                  d->makeFpRegister(gR3(ib)), d->makeFpRegister(gR2(ib)));
    }
} mips32_trunc_w_d;

// WAIT -- enter standby mode
static struct Mips32_wait: Mips32 {
    Mips32_wait(): Mips32(Release1,
                          sOP(020)|shift_to<25, 25>(1)|sFN(040),
                          mOP()   |mask_for<25, 25>() |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        unsigned impl UNUSED_VAR = extract<6, 24>(ib);
        return d->makeInstruction(mips_wait, "wait");
    }
} mips32_wait;

// WRPGPR -- write to  GPR in previous shadow set
static struct Mips32_wrpgpr: Mips32 {
    Mips32_wrpgpr(): Mips32(Release2,
                            sOP(020)|sR0(016)|shift_to<0, 10>(0),
                            mOP()   |mR0()   |mask_for<0, 10>()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_wrpgpr, "wrpgpr",
                                  d->makeShadowRegister(gR2(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_wrpgpr;

// WSBH -- word swap bytes within halfwords
static struct Mips32_wsbh: Mips32 {
    Mips32_wsbh(): Mips32(Release2,
                          sOP(037)|sR0(000)|sR3(002)|sFN(040),
                          mOP()   |mR0()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_wsbh, "wsbh",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_wsbh;

// XOR -- exclusive OR
static struct Mips32_xor: Mips32 {
    Mips32_xor(): Mips32(Release1,
                         sOP(000)|sR3(000)|sFN(046),
                         mOP()   |mR3()   |mFN()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_xor, "xor",
                                  d->makeRegister(gR2(ib)), d->makeRegister(gR0(ib)), d->makeRegister(gR1(ib)));
    }
} mips32_xor;

// XORI -- exclusive OR immediate
static struct Mips32_xori: Mips32 {
    Mips32_xori(): Mips32(Release1, sOP(016), mOP()) {}
    SgAsmMipsInstruction *operator()(D *d, unsigned ib) {
        return d->makeInstruction(mips_xori, "xori",
                                  d->makeRegister(gR1(ib)), d->makeRegister(gR0(ib)), d->makeImmediate16(gIM(ib), 0, 16));
    }
} mips32_xori;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
DisassemblerMips::init(ByteOrder::Endianness sex)
{
    switch (sex) {
        case ByteOrder::ORDER_MSB:
            name("mips-be");
            break;
        case ByteOrder::ORDER_LSB:
            name("mips-le");
            break;
        default:
            ASSERT_not_reachable("invalid MIPS disassembler byte order");
    }

    registerDictionary(RegisterDictionary::dictionary_mips32()); // only a default
    REG_IP = *registerDictionary()->lookup("pc");
    REG_SP = *registerDictionary()->lookup("sp");

    wordSizeBytes(4);
    byteOrder(sex);
    callingConventions(CallingConvention::dictionaryMips());

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
    insert_idis(&mips32_ehb);
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
