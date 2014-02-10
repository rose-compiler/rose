#include "sage3basic.h"

#include "DisassemblerM68k.h"
#include "integerOps.h"
#include "stringify.h"
#include "sageBuilderAsm.h"

using namespace IntegerOps;

typedef BitPattern<uint16_t> Pattern;

// Bit pattern for an opcode in the high-order 4 bits of the instruction word
static Pattern
OP(unsigned val) {
    return Pattern(12, 15, val, 0);
}

// Bit pattern for an effective address mode in the low-order 6 bits of the instruction word. Only the specified modes are
// matched.
static Pattern
EAM(unsigned eamodes, size_t lobit=0)
{
    assert(eamodes!=0);
    Pattern retval;
    for (size_t bitnum=0; eamodes!=0; ++bitnum) {
        unsigned bit = IntegerOps::shl1<unsigned>(bitnum);
        if (0 != (eamodes & bit)) {
            eamodes &= ~bit;
            M68kEffectiveAddressMode eam = (M68kEffectiveAddressMode)bit;
            switch (eam) {
                case m68k_eam_drd:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, lobit+6, reg, 0);
                    break;
                case m68k_eam_ard:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, lobit+6, 0x08|reg, 0);
                    break;
                case m68k_eam_ari:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, lobit+6, 0x10|reg, 0);
                    break;
                case m68k_eam_inc:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, lobit+6, 0x18|reg, 0);
                    break;
                case m68k_eam_dec:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, lobit+6, 0x20|reg, 0);
                    break;
                case m68k_eam_dsp:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, lobit+6, 0x28|reg, 0);
                    break;
                case m68k_eam_idx:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, lobit+6, 0x30|reg, 0);
                    break;
                case m68k_eam_pcdsp:
                    retval |= Pattern(lobit, lobit+6, 0x3a, 0);
                    break;
                case m68k_eam_pcidx:
                    retval |= Pattern(lobit, lobit+6, 0x3b, 0);
                    break;
                case m68k_eam_absw:
                    retval |= Pattern(lobit, lobit+6, 0x38, 0);
                    break;
                case m68k_eam_absl:
                    retval |= Pattern(lobit, lobit+6, 0x39, 0);
                    break;
                case m68k_eam_imm:
                    retval |= Pattern(lobit, lobit+6, 0x3c, 0);
                    break;
                default:
                    assert(!"invalid effective address mode");
                    abort();
            }
        }
    }
    return retval;
}

template<size_t lo, size_t hi>
Pattern BITS(unsigned val)
{
    return Pattern(lo, hi, val, 0);
}

template<size_t lo>
Pattern BIT(unsigned val)
{
    return Pattern(lo, lo, val);
}

// see base class
bool
DisassemblerM68k::can_disassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_M68K_Family;
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeDataRegister(unsigned regnum, size_t nbits)
{
    assert(regnum <= 7);
    assert(8==nbits || 16==nbits || 32==nbits);
    RegisterDescriptor desc(m68k_regclass_data, regnum, 0, nbits);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeAddressRegister(unsigned regnum, size_t nbits)
{
    assert(regnum <= 7);
    assert(16==nbits || 32==nbits);
    RegisterDescriptor desc(m68k_regclass_addr, regnum, 0, nbits);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeConditionCodeRegister()
{
    RegisterDescriptor desc(m68k_regclass_spr, m68k_spr_ccr, 0, 8);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeProgramCounter()
{
    RegisterDescriptor desc(m68k_regclass_spr, m68k_spr_pc, 0, 32);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

char
DisassemblerM68k::sizeToLetter(size_t nbits)
{
    switch (nbits) {
        case  8: return 'b';
        case 16: return 'w';
        case 32: return 'l';
    }
    assert(!"invalid operand size");
    abort();
}

SgAsmIntegerValueExpression *
DisassemblerM68k::makeImmediate32(unsigned low16, unsigned hi16)
{
    uint32_t val = low16 | shiftLeft<32>(hi16, 16);
    return new SgAsmIntegerValueExpression(val, 32);
}

SgAsmIntegerValueExpression *
DisassemblerM68k::makeImmediate16(unsigned val)
{
    return new SgAsmIntegerValueExpression(val & 0xffff, 16);
}

SgAsmIntegerValueExpression *
DisassemblerM68k::makeImmediate8(unsigned val)
{
    return new SgAsmIntegerValueExpression(val & 0xff, 8);
}

M68kEffectiveAddressMode
DisassemblerM68k::effectiveAddressMode(unsigned modreg, size_t nbits)
{
    unsigned mode = modreg >> 3;
    unsigned reg  = modreg & 7;
    switch (mode) {
        case 0: return m68k_eam_drd;
        case 1: return m68k_eam_ard;
        case 2: return m68k_eam_ari;
        case 3: return m68k_eam_inc;
        case 4: return m68k_eam_dec;
        case 5: return m68k_eam_dsp;
        case 6: return m68k_eam_idx;
        case 7:
            switch (reg) {
                case 0: return m68k_eam_absw;
                case 1: return m68k_eam_absl;
                case 2: return m68k_eam_pcdsp;
                case 4: return m68k_eam_imm;
                default: return m68k_eam_unknown;
            }
    }
    assert(!"invalid effective address mode");
    abort();
}

size_t
DisassemblerM68k::nExtensionWords(M68kEffectiveAddressMode eam, size_t nbits)
{
    assert(8==nbits || 16==nbits || 32==nbits);
    switch (eam) {
        case m68k_eam_drd:
        case m68k_eam_ard:
        case m68k_eam_ari:
        case m68k_eam_inc:
        case m68k_eam_dec:
            return 0;
        case m68k_eam_dsp:
        case m68k_eam_idx:
        case m68k_eam_pcdsp:
        case m68k_eam_pcidx:
        case m68k_eam_absw:
            return 1;
        case m68k_eam_absl:
            return 2;
        case m68k_eam_imm:
            return 32==nbits ? 2 : 1;
        default:
            assert(!"invalid effective address mode");
            abort();
    }
}
            
// The modreg should be 6 bits: upper three bits are the mode, lower three bits are usually a register number.
SgAsmExpression *
DisassemblerM68k::makeEffectiveAddress(unsigned modreg, size_t nbits, size_t ext_offset)
{
    assert(8==nbits || 16==nbits || 32==nbits);
    assert(ext_offset<=2);
    unsigned reg  = modreg & 7;
    M68kEffectiveAddressMode eam = effectiveAddressMode(modreg, nbits);
    SgAsmType *type = NULL;
    switch (nbits) {
        case 8: type = SgAsmTypeByte::createType(); break;
        case 16: type = SgAsmTypeWord::createType(); break;
        case 32: type = SgAsmTypeDoubleWord::createType(); break;
    }
    switch (eam) {
        case m68k_eam_drd:
            return makeDataRegister(reg, nbits);
        case m68k_eam_ard:
            return makeAddressRegister(reg, nbits);
        case m68k_eam_ari: {
            SgAsmM68kRegisterReferenceExpression *rre = makeAddressRegister(reg, 32);
            return SageBuilderAsm::makeMemoryReference(rre, NULL/*segment*/, type);
        }
        case m68k_eam_inc: {
            SgAsmM68kRegisterReferenceExpression *rre = makeAddressRegister(reg, 32);
            rre->set_adjustment(nbits/8); // post increment number of bytes
            return SageBuilderAsm::makeMemoryReference(rre, NULL/*segment*/, type);
        }
        case m68k_eam_dec: {
            SgAsmM68kRegisterReferenceExpression *rre = makeAddressRegister(reg, 32);
            rre->set_adjustment(-(nbits/8)); // pre decrement number of bytes
            return SageBuilderAsm::makeMemoryReference(rre, NULL/*segment*/, type);
        }
        case m68k_eam_dsp:
        case m68k_eam_pcdsp: {
            SgAsmM68kRegisterReferenceExpression *rre = eam==m68k_eam_dsp ? makeAddressRegister(reg, 32) : makeProgramCounter();
            uint64_t val = signExtend<16, 32>(instruction_word(ext_offset+1));
            SgAsmIntegerValueExpression *dsp = new SgAsmIntegerValueExpression(val, 32);
            SgAsmExpression *addr = SageBuilderAsm::makeAdd(rre, dsp);
            return SageBuilderAsm::makeMemoryReference(addr, NULL/*segment*/, type);
        }
        case m68k_eam_idx:
        case m68k_eam_pcidx: {
            unsigned ext1 = instruction_word(ext_offset+1);
            SgAsmM68kRegisterReferenceExpression *areg = eam==m68k_eam_idx ? makeAddressRegister(reg, 32) : makeProgramCounter();
            uint64_t val = signExtend<8, 32>(extract<0, 7>(ext1));
            SgAsmIntegerValueExpression *dsp = new SgAsmIntegerValueExpression(val, 32);
            SgAsmExpression *addr = SageBuilderAsm::makeAdd(areg, dsp);
            bool da_bit = 0!=extract<15, 15>(ext1);
            bool wl_bit = 0!=extract<11, 11>(ext1);
            if (!wl_bit)
                throw Exception("word index registers not supported by this architecture");
            SgAsmM68kRegisterReferenceExpression *ireg = da_bit ?
                                                         makeAddressRegister(extract<12, 14>(ext1), 32) :
                                                         makeDataRegister(extract<12, 14>(ext1), 32);
            switch (extract<9, 10>(ext1)) {
                case 0: val=1; break;
                case 1: val=2; break;
                case 2: val=4; break;
                case 3: val=8; break;
            }
            SgAsmIntegerValueExpression *scale = new SgAsmIntegerValueExpression(val, 32);
            SgAsmExpression *product = SageBuilderAsm::makeMul(ireg, scale);
            addr = SageBuilderAsm::makeAdd(addr, product);
            return SageBuilderAsm::makeMemoryReference(addr, NULL/*segment*/, type);
        }
        case m68k_eam_absw: {
            uint64_t val = signExtend<16, 32>(instruction_word(ext_offset+1));
            SgAsmIntegerValueExpression *addr = new SgAsmIntegerValueExpression(val, 32);
            return SageBuilderAsm::makeMemoryReference(addr, NULL/*segment*/, type);
        }
        case m68k_eam_absl: {
            uint64_t val = shiftLeft<32>(instruction_word(ext_offset+1), 16) | instruction_word(ext_offset+2);
            SgAsmIntegerValueExpression *addr = new SgAsmIntegerValueExpression(val, 32);
            return SageBuilderAsm::makeMemoryReference(addr, NULL/*segment*/, type);
        }
        case m68k_eam_imm: {
            switch (nbits) {
                case 8:
                    return makeImmediate8(instruction_word(ext_offset+1) & 0xff);
                case 16:
                    return makeImmediate16(instruction_word(ext_offset+1));
                case 32:
                    return makeImmediate32(instruction_word(ext_offset+2), instruction_word(ext_offset+1));
            }
        }
        default:
            throw Exception("invalid effective address mode");
    }
}

SgAsmInstruction *
DisassemblerM68k::make_unknown_instruction(const Disassembler::Exception &e)
{
    return makeInstruction(m68k_unknown_instruction, "unknown");
}

SgAsmM68kInstruction *
DisassemblerM68k::makeInstruction(M68kInstructionKind kind, const std::string &mnemonic,
                                  SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3)
{
    SgAsmM68kInstruction *insn = new SgAsmM68kInstruction(get_insn_va(), mnemonic, kind);

    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    /* If any operand is null, then the following operands must also be null because analysis assumes that the operand vector
     * indices correspond to argument positions and don't expect null-padding in the vector. */
    ROSE_ASSERT((!op1 && !op2 && !op3) ||
                ( op1 && !op2 && !op3) ||
                ( op1 &&  op2 && !op3) ||
                ( op1 &&  op2 &&  op3));

    if (op1)
        SageBuilderAsm::appendOperand(insn, op1);
    if (op2)
        SageBuilderAsm::appendOperand(insn, op2);
    if (op3)
        SageBuilderAsm::appendOperand(insn, op3);

    return insn;
}


// Returns the Nth 16-bit word of an instruction, reading it from memory if necessary.  Instructions contain one, two, or
// three 16-bit words.
uint16_t
DisassemblerM68k::instruction_word(size_t n)
{
    if (n>2)
        throw Exception("instruction word " + StringUtility::numberToString(n) + " is out of range");
    niwords_used = std::max(niwords_used, n+1);
    return iwords[n];
}

// see base class
SgAsmInstruction *
DisassemblerM68k::disassembleOne(const MemoryMap *map, rose_addr_t start_va, AddressSet *successors)
{
    start_instruction(map, start_va);
    niwords = map->read(iwords, start_va, 3, get_protection());
    SgAsmM68kInstruction *insn = NULL;
    if (M68k *idis = find_idis(iwords, niwords))
        insn = (*idis)(this, instruction_word(0));
    if (!insn)
        throw Exception("cannot disassemble m68k instruction: "+StringUtility::addrToString(instruction_word(0), 16), start_va);

    assert(niwords_used>0);
    SgUnsignedCharList raw_bytes(iwords+0, iwords+niwords_used);
    insn->set_raw_bytes(raw_bytes);

    if (successors) {
        bool complete;
        AddressSet suc2 = insn->get_successors(&complete);
        successors->insert(suc2.begin(), suc2.end());
    }

    update_progress(insn);
    return insn;
}

void
DisassemblerM68k::insert_idis(M68k *idis)
{
    // Check whether this instruction disassembler's bit pattern is ambiguous with an existing pattern
    assert(idis!=NULL);
    for (IdisTable::iterator ti=idis_table.begin(); ti!=idis_table.end(); ++ti) {
        std::pair<size_t, size_t> alternatives;
        if (idis->pattern.any_same((*ti)->pattern, &alternatives)) {
            // Inserting this instruction-specific disassembler would cause an ambiguity in the table
            std::cerr <<"DisassemblerM68k::insert_idis: insertion key ";
            idis->pattern.print(std::cerr, alternatives.first);
            std::cerr <<" conflicts with existing key ";
            (*ti)->pattern.print(std::cerr, alternatives.second);
            std::cerr <<"\n";
            throw Exception("insert_idis() would cause an ambiguity in the M68k instruction disassembly table");
        }
    }

    // Insert the new pattern in the list which is sorted by descending number of significant bits.
    IdisTable::iterator ti = idis_table.begin();
    while (ti!=idis_table.end() && (*ti)->pattern.nsignificant()>=idis->pattern.nsignificant()) ++ti;
    idis_table.insert(ti, idis);
}

DisassemblerM68k::M68k *
DisassemblerM68k::find_idis(uint16_t *insn_bytes, size_t nbytes) const
{
    for (IdisTable::const_iterator ti=idis_table.begin(); ti!=idis_table.end(); ++ti) {
        if ((*ti)->pattern.matches(insn_bytes, nbytes))
            return *ti;
    }
    return NULL;
}

/*******************************************************************************************************************************
 *                                      Instruction-specific disassemblers
 *******************************************************************************************************************************/

typedef DisassemblerM68k::M68k M68k;

// ADD.L <ea>y, Dx
static struct M68k_add_1: M68k {
    M68k_add_1(): M68k(OP(13) & BITS<6, 8>(2) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_add, "add.l",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0),
                                  d->makeDataRegister(extract<9, 11>(w0), 32));
    }
} add_1;

// ADD.L Dy, <ea>x
static struct M68k_add_2: M68k {
    M68k_add_2(): M68k(OP(13) & BITS<6, 8>(6) &
                       EAM(m68k_eam_all & ~(m68k_eam_direct|m68k_eam_imm|m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_add, "add.l",
                                  d->makeDataRegister(extract<9, 11>(w0), 32),
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0));
    }
} add_2;

// ADDA.L <ea>y, Ax
static struct M68k_adda: M68k {
    M68k_adda(): M68k(OP(13) & BITS<6, 8>(7) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_adda, "adda.l",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0),
                                  d->makeAddressRegister(extract<9, 11>(w0), 32));
    }
} adda;

// ADDI #<data>.L, Dx
static struct M68k_addi: M68k {
    M68k_addi(): M68k(OP(0) & BITS<3, 11>(0xd0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_addi, "addi.l",
                                  d->makeImmediate32(d->instruction_word(2), d->instruction_word(1)),
                                  d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} addi;

// ADDQ.L #<data>, <ea>x
static struct M68k_addq: M68k {
    M68k_addq(): M68k(OP(5) & BITS<6, 8>(2) &
                      EAM(m68k_eam_all & ~(m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned imm = extract<9, 11>(w0);
        if (0==imm)
            imm = 8;
        return d->makeInstruction(m68k_addq, "addq.l",
                                  d->makeImmediate32(imm),
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0));
    }
} addq;

// ADDX.L Dy, Dx
static struct M68k_addx: M68k {
    M68k_addx(): M68k(OP(13) & BITS<3, 8>(0x30)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_addx, "addx.l",
                                  d->makeDataRegister(extract<0, 2>(w0), 32),
                                  d->makeDataRegister(extract<9, 11>(w0), 32));
    }
} addx;

// AND.L <ea>y, Dx
static struct M68k_and_1: M68k {
    M68k_and_1(): M68k(OP(12) & BITS<6, 8>(2) &
                       EAM(m68k_eam_all & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_and, "and.l",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0),
                                  d->makeDataRegister(extract<9, 11>(w0), 32));
    }
} and_1;

// AND.L Dy, <ea>x
static struct M68k_and_2: M68k {
    M68k_and_2(): M68k(OP(12) & BITS<6, 8>(6) &
                       EAM(m68k_eam_all & ~(m68k_eam_direct | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_and, "and.l",
                                  d->makeDataRegister(extract<9, 11>(w0), 32),
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0));
    }
} and_2;

// ANDI.L #<data>, Dx
static struct M68k_andi: M68k {
    M68k_andi(): M68k(OP(0) & BITS<3, 11>(0x50)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_andi, "andi.l",
                                  d->makeImmediate32(d->instruction_word(2), d->instruction_word(1)),
                                  d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} andi;

// ASL.L Dy, Dx
// ASR.L Dy, Dx
// ASL.L #<nbits>, Dx
// ASR.L #<nbits>, Dx
static struct M68k_ashift: M68k {
    M68k_ashift(): M68k(OP(14) & BITS<6, 7>(2) & BITS<3, 4>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned direction = w0 & 0x0100;
        M68kInstructionKind kind = direction ? m68k_asl : m68k_asr;
        const char *mnemonic = direction ? "asl.l" : "asr.l";
        SgAsmExpression *count = NULL;
        if (0==(w0 & 0x0020)) {
            unsigned n = extract<9, 11>(w0);
            if (0==n) n = 8;
            count = d->makeImmediate8(n);
        } else {
            count = d->makeDataRegister(extract<9, 11>(w0), 32);
        }
        return d->makeInstruction(kind, mnemonic, count, d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} ashift;

// Bcc.sz <label>
// BRA.sz <label>
// BSR.sz <label>
static struct M68k_branch: M68k {
    M68k_branch(): M68k(OP(6)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned cc = extract<8, 11>(w0);
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (cc) {
            case  0: kind = m68k_bra; break;
            case  1: kind = m68k_bsr; break;
            case  2: kind = m68k_bhi; break;
            case  3: kind = m68k_bls; break;
            case  4: kind = m68k_bcc; break;
            case  5: kind = m68k_bcs; break;
            case  6: kind = m68k_bne; break;
            case  7: kind = m68k_beq; break;
            case  8: kind = m68k_bvc; break;
            case  9: kind = m68k_bvs; break;
            case 10: kind = m68k_bpl; break;
            case 11: kind = m68k_bmi; break;
            case 12: kind = m68k_bge; break;
            case 13: kind = m68k_blt; break;
            case 14: kind = m68k_bgt; break;
            case 15: kind = m68k_ble; break;
        }
        if (m68k_unknown_instruction==kind)
            throw Disassembler::Exception("invalid branch condition code");
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_");
        rose_addr_t base = d->get_insn_va() + 2;
        int32_t offset = signExtend<8, 32>(extract<0, 7>(w0));
        if (0==offset) {
            offset = signExtend<16, 32>(d->instruction_word(1));
            mnemonic += ".w";
        } else if (-1==offset) {
            // FIXME: documentation is not clear about order of the two 16-bit words. I'm assuming that, like other cases
            // of 32-bit values, the high 16 bits are in the first word following the instruction word, and the low-order
            // 16 bits are in the second word after the instruction word. [Robb P. Matzke 2013-08-06]
            offset = shiftLeft<32>(d->instruction_word(1), 16) | d->instruction_word(2);
            mnemonic += ".l";
        } else {
            mnemonic += ".b";
        }
        rose_addr_t target_va = (base + offset) & GenMask<rose_addr_t, 32>::value;
        SgAsmIntegerValueExpression *target = d->makeImmediate32(target_va);
        return d->makeInstruction(kind, mnemonic, target);
    }
} branch;

// BCHG.B #<bitnum>, <ea>x
// BCHG.L #<bitnum>, <ea>x
static struct M68k_bchg_1: M68k {
    M68k_bchg_1(): M68k(OP(0) & BITS<6, 11>(0x21) &
                        EAM(m68k_eam_drd | m68k_eam_ari | m68k_eam_inc | m68k_eam_dec | m68k_eam_dsp)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediate8(d->instruction_word(1) & 0xff);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        std::string mnemonic = "bchg";
        if (isSgAsmRegisterReferenceExpression(dst)) {
            mnemonic += ".l";
        } else {
            dst->set_type(SgAsmTypeByte::createType());
            mnemonic += ".b";
        }
        return d->makeInstruction(m68k_bchg, mnemonic, src, dst);
    }
} bchg_1;

// BCHG.B Dy, <ea>x
// BCHG.L Dy, <ea>x
static struct M68k_bchg_2: M68k {
    M68k_bchg_2(): M68k(OP(0) & BITS<6, 8>(5) &
                        EAM(m68k_eam_all & ~(m68k_eam_ard | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kEffectiveAddressMode eam = d->effectiveAddressMode(extract<0, 5>(w0), 32);
        size_t nbits = (eam & m68k_eam_direct) ? 32 : 8;
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_bchg, "bchg."+d->sizeToLetter(nbits), src, dst);
    }
} bchg_2;

// BCLR.B #<bitnum>, <ea>x
// BCLR.L #<bitnum>, <ea>x
static struct M68k_bclr_1: M68k {
    M68k_bclr_1(): M68k(OP(0) & BITS<6, 11>(0x22) &
                        EAM(m68k_eam_drd | m68k_eam_ari | m68k_eam_inc | m68k_eam_dec | m68k_eam_dsp)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kEffectiveAddressMode eam = d->effectiveAddressMode(extract<0, 5>(w0), 32);
        size_t nbits = (eam & m68k_eam_direct) ? 32 : 8;
        SgAsmExpression *src = d->makeImmediate8(d->instruction_word(1) & 0xff);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 1);
        return d->makeInstruction(m68k_bclr, "bclr."+d->sizeToLetter(nbits), src, dst);
    }
} bclr_1;

// BCLR.B Dy, <ea>x
// BCLR.L Dy, <ea>x
static struct M68k_bclr_2: M68k {
    M68k_bclr_2(): M68k(OP(0) & BITS<6, 8>(6) &
                        EAM(m68k_eam_all & ~(m68k_eam_ard | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kEffectiveAddressMode eam = d->effectiveAddressMode(extract<0, 5>(w0), 32);
        size_t nbits = (eam & m68k_eam_direct) ? 32 : 8;
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_bclr, "bclr."+d->sizeToLetter(nbits), src, dst);
    }
} bclr_2;

// BITREV.L Dx
static struct M68k_bitrev: M68k {
    M68k_bitrev(): M68k(OP(0) & BITS<3, 11>(0x18)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_bitrev, "bitrev.l", d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} bitrev;

// BSET.B #<bitnum>, <ea>x
// BSET.L #<bitnum>, <ea>x
static struct M68k_bset_1: M68k {
    M68k_bset_1(): M68k(OP(0) & BITS<6, 11>(0x23) &
                        EAM(m68k_eam_drd | m68k_eam_ari | m68k_eam_inc | m68k_eam_dec | m68k_eam_dsp)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediate8(d->instruction_word(1) & 0xff);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        std::string mnemonic = "bset";
        if (isSgAsmRegisterReferenceExpression(dst)) {
            mnemonic += ".l";
        } else {
            dst->set_type(SgAsmTypeByte::createType());
            mnemonic += ".b";
        }
        return d->makeInstruction(m68k_bset, mnemonic, src, dst);
    }
} bset_1;

// BSET.B Dy, <ea>x
// BSET.L Dy, <ea>x
static struct M68k_bset_2: M68k {
    M68k_bset_2(): M68k(OP(0) & BITS<6, 8>(7) &
                        EAM(m68k_eam_all & ~(m68k_eam_ard | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        std::string mnemonic = "bset";
        if (isSgAsmRegisterReferenceExpression(dst)) {
            mnemonic += ".l";
        } else {
            dst->set_type(SgAsmTypeByte::createType());
            mnemonic += ".b";
        }
        return d->makeInstruction(m68k_bset, mnemonic, src, dst);
    }
} bset_2;

// BTST.B #<bitnum>, <ea>x
// BTST.L #<bitnum>, <ea>x
static struct M68k_btst_1: M68k {
    M68k_btst_1(): M68k(OP(0) & BITS<6, 11>(0x20) &
                        EAM(m68k_eam_drd | m68k_eam_ari | m68k_eam_inc | m68k_eam_dec | m68k_eam_dsp)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediate8(d->instruction_word(1) & 0xff);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        std::string mnemonic = "btst";
        if (isSgAsmRegisterReferenceExpression(dst)) {
            mnemonic += ".l";
        } else {
            dst->set_type(SgAsmTypeByte::createType());
            mnemonic += ".b";
        }
        return d->makeInstruction(m68k_btst, mnemonic, src, dst);
    }
} btst_1;

// BTST.B Dy, <ea>x
// BTST.L Dy, <ea>x
static struct M68k_btst_2: M68k {
    M68k_btst_2(): M68k(OP(0) & BITS<6, 8>(4) &
                        EAM(m68k_eam_all & ~(m68k_eam_ard | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        std::string mnemonic = "btst";
        if (isSgAsmRegisterReferenceExpression(dst)) {
            mnemonic += ".l";
        } else {
            dst->set_type(SgAsmTypeByte::createType());
            mnemonic += ".b";
        }
        return d->makeInstruction(m68k_btst, mnemonic, src, dst);
    }
} btst_2;

// BYTEREV.L Dx
static struct M68k_byterev: M68k {
    M68k_byterev(): M68k(OP(0) & BITS<3, 11>(0x58)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_byterev, "byterev.l", d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} byterev;

// CLR.B <ea>x
// CLR.W <ea>x
// CLR.L <ea>x
static struct M68k_clr: M68k {
    M68k_clr(): M68k(OP(4) & BITS<8, 11>(2) &
                     EAM(m68k_eam_all & ~(m68k_eam_ard | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        std::string mnemonic = "clr";
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0:
                nbits = 8;
                mnemonic += ".b";
                break;
            case 1:
                nbits = 16;
                mnemonic += ".w";
                break;
            case 2:
                nbits = 32;
                mnemonic += ".l";
                break;
            default:
                throw Disassembler::Exception("invalid operand size");
        }
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_clr, mnemonic, dst);
    }
} clr;

// CMP.B <ea>y, Dx
// CMP.W <ea>y, Dx
// CMP.L <ea>y, Dx
// CMPA.W <ea>y, Ax
// CMPA.L <ea>y, Ax
static struct M68k_cmp: M68k {
    M68k_cmp(): M68k(OP(11) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = m68k_unknown_instruction;
        std::string mnemonic;
        SgAsmExpression *src=NULL, *dst=NULL;
        switch (extract<6, 8>(w0)) {
            case 0:
                kind = m68k_cmp;
                mnemonic = "cmp.b";
                src = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
                dst = d->makeDataRegister(extract<9, 11>(w0), 8);
                break;
            case 1:
                kind = m68k_cmp;
                mnemonic = "cmp.w";
                src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
                dst = d->makeDataRegister(extract<9, 11>(w0), 16);
                break;
            case 2:
                kind = m68k_cmp;
                mnemonic = "cmp.l";
                src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
                dst = d->makeDataRegister(extract<9, 11>(w0), 32);
                break;
            case 3:
                kind = m68k_cmpa;
                mnemonic = "cmpa.w";
                src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
                dst = d->makeAddressRegister(extract<9, 11>(w0), 16);
                break;
            case 7:
                kind = m68k_cmpa;
                mnemonic = "cmpa.l";
                src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
                dst = d->makeAddressRegister(extract<9, 11>(w0), 32);
                break;
            default:
                throw Disassembler::Exception("invalid CMP/CMPA opmode field");
        }
        return d->makeInstruction(kind, mnemonic, src, dst);
    }
} cmp;

// CMPI.B #<data>, Dx
// CMPI.W #<data>, Dx
// CMPI.L #<data>, Dx
static struct M68k_cmpi: M68k {
    M68k_cmpi(): M68k(OP(0) & BITS<8, 11>(6) & BITS<3, 5>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = NULL;
        std::string mnemonic = "cmpi";
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0:
                nbits = 8;
                mnemonic += ".b";
                src = d->makeImmediate8(d->instruction_word(1) & 0xff);
                break;
            case 1:
                nbits = 16;
                mnemonic += ".w";
                src = d->makeImmediate16(d->instruction_word(1));
                break;
            case 2:
                nbits = 32;
                mnemonic += ".l";
                src = d->makeImmediate32(d->instruction_word(2), d->instruction_word(1));
                break;
            default:
                throw Disassembler::Exception("invalid CMPI size field");
        }
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), nbits);
        return d->makeInstruction(m68k_cmpi, mnemonic, src, dst);
    }
} cmpi;

// DIVS.W <ea>y, Dx
static struct M68k_divs_w: M68k {
    M68k_divs_w(): M68k(OP(8) & BITS<6, 8>(7) &
                        EAM(m68k_eam_all & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_divs, "divs.w",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0),
                                  d->makeDataRegister(extract<9, 11>(w0), 32));
    }
} divs_w;

// DIVU.W
static struct M68k_divu_w: M68k {
    M68k_divu_w(): M68k(OP(8) & BITS<6, 8>(3) &
                        EAM(m68k_eam_all & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_divu, "divu.w",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0),
                                  d->makeDataRegister(extract<9, 11>(w0), 32));
    }
} divu_w;

// DIVS.L <ea>y, Dx
// DIVU.L <ea>y, Dx
static struct M68k_divide_l: M68k {
    M68k_divide_l(): M68k(OP(4) & BITS<6, 11>(0x31) &
                          EAM(m68k_eam_drd | m68k_eam_ari | m68k_eam_inc | m68k_eam_dec | m68k_eam_dsp)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        if (extract<12, 14>(d->instruction_word(1)) != extract<0, 2>(d->instruction_word(1)))
            throw Disassembler::Exception("inconsistent Dx register in DIVS extension word");
        M68kInstructionKind kind = extract<11, 11>(d->instruction_word(1)) ? m68k_divs : m68k_divu;
        return d->makeInstruction(kind, stringifyM68kInstructionKind(kind, "m68k_")+".l",
                                  d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1),
                                  d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32));
    }
} divide_l;

// EOR.L Dy, <ea>x
static struct M68k_eor: M68k {
    M68k_eor(): M68k(OP(11) & BITS<6, 8>(6) &
                     EAM(m68k_eam_all & ~(m68k_eam_ard | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_eor, "eor.l", src, dst);
    }
} eor;

// EORI.L #<data>, Dx
static struct M68k_eori: M68k {
    M68k_eori(): M68k(OP(0) & BITS<3, 11>(0x150)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediate32(d->instruction_word(2), d->instruction_word(1));
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), 32);
        return d->makeInstruction(m68k_eori, "eori.l", src, dst);
    }
} eori;

// EXT.W Dx
static struct M68k_ext_w: M68k {
    M68k_ext_w(): M68k(OP(4) & BITS<3, 11>(0x110)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_ext, "ext.w", d->makeDataRegister(extract<0, 2>(w0), 16));
    }
} ext_w;

// EXT.L Dx
static struct M68k_ext_l: M68k {
    M68k_ext_l(): M68k(OP(4) & BITS<3, 11>(0x118)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_ext, "ext.l", d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} ext_l;

// EXTB.L Dx
static struct M68k_extb_l: M68k {
    M68k_extb_l(): M68k(OP(4) & BITS<3, 11>(0x138)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_extb, "extb.l", d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} extb_l;

// FF1.L Dx
static struct M68k_ff1: M68k {
    M68k_ff1(): M68k(BITS<3, 15>(0x98)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_ff1, "ff1.l", d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} ff1;

// ILLEGAL
static struct M68k_illegal: M68k {
    M68k_illegal(): M68k(BITS<0, 15>(0x4afc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_illegal, "illegal");
    }
} illegal;

// JMP <ea>x
static struct M68k_jmp: M68k {
    M68k_jmp(): M68k(BITS<6, 15>(0x13b) &
                     EAM(m68k_eam_ari | m68k_eam_dsp | m68k_eam_idx | m68k_eam_absw | m68k_eam_absl | m68k_eam_pc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_jmp, "jmp", src);
    }
} jmp;

// JSR <ea>x
static struct M68k_jsr: M68k {
    M68k_jsr(): M68k(BITS<6, 15>(0x13a) &
                     EAM(m68k_eam_ari | m68k_eam_dsp | m68k_eam_idx | m68k_eam_absw | m68k_eam_absl | m68k_eam_pc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_jsr, "jsr", src);
    }
} jsr;

// LEA.L <ea>y, Ax
static struct M68k_lea: M68k {
    M68k_lea(): M68k(OP(4) & BITS<6, 8>(7) &
                     EAM(m68k_eam_ari | m68k_eam_dsp | m68k_eam_idx | m68k_eam_absw | m68k_eam_absl | m68k_eam_pc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_lea, "lea.l", src, dst);
    }
} lea;

// LINK.W Ay, #<displacement>
static struct M68k_link: M68k {
    M68k_link(): M68k(BITS<3, 15>(0x9ca)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeAddressRegister(extract<0, 2>(w0), 32);
        SgAsmExpression *dsp = d->makeImmediate16(d->instruction_word(1));
        return d->makeInstruction(m68k_link, "link.w", src, dsp);
    }
} link_1;

// LSL.L Dy, Dx
// LSR.L Dy, Dx
// LSL.L #<nbits>, Dx
// LSR.L #<nbits>, Dx
static struct M68k_lshift: M68k {
    M68k_lshift(): M68k(OP(15) & BITS<6, 7>(2) & BITS<3, 4>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned direction = w0 & 0x0100;
        M68kInstructionKind kind = direction ? m68k_lsl : m68k_lsr;
        const char *mnemonic = direction ? "lsl.l" : "lsr.l";
        SgAsmExpression *count = NULL;
        if (0==(w0 & 0x0020)) {
            unsigned n = extract<9, 11>(w0);
            if (0==n) n = 8;
            count = d->makeImmediate8(n);
        } else {
            count = d->makeDataRegister(extract<9, 11>(w0), 32);
        }
        return d->makeInstruction(kind, mnemonic, count, d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} lshift;

// MOV3Q.L #<data>, <ea>x
static struct M68k_mov3q: M68k {
    M68k_mov3q(): M68k(OP(10) & BITS<6, 8>(5) &
                       EAM(m68k_eam_all & ~(m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        int32_t val = signExtend<3, 32>(extract<9, 11>(w0));
        if (0==val)
            val = -1;
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_mov3q, "mov3q.l", d->makeImmediate32(val), dst);
    }
} mov3q;

// MOVE.B <ea>y, <ea>x
static struct M68k_move: M68k {
    M68k_move(): M68k((OP(1)|OP(2)|OP(3)) & // 8-, 32-, or 16-bit operation
                      ((EAM(m68k_eam_drd|m68k_eam_ard|m68k_eam_ari|m68k_eam_inc|m68k_eam_dec) &
                        EAM(m68k_eam_all & ~(m68k_eam_ard | m68k_eam_imm | m68k_eam_pc), 6)) |
                       (EAM(m68k_eam_dsp|m68k_eam_pcdsp) &
                        EAM(m68k_eam_drd|m68k_eam_ari|m68k_eam_inc|m68k_eam_dec|m68k_eam_dsp, 6)) |
                       (EAM(m68k_eam_idx|m68k_eam_pcidx|m68k_eam_absw|m68k_eam_absl|m68k_eam_imm) &
                        EAM(m68k_eam_drd|m68k_eam_ari|m68k_eam_inc|m68k_eam_dec, 6)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<12, 13>(w0)) {
            case 0: assert(!"invalid match"); abort();
            case 1: nbits = 8; break;
            case 2: nbits = 32; break;
            case 3: nbits = 16; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        M68kEffectiveAddressMode src_eam = d->effectiveAddressMode(extract<0, 5>(w0), nbits);
        size_t ext_nwords = d->nExtensionWords(src_eam, 8);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<6, 11>(w0), 8, ext_nwords);
        return d->makeInstruction(m68k_move, "move.b", src, dst);
    }
} move;

// MOVEA.W <ea>y, Ax
// MOVEA.L <ea>y, Ax
static struct M68k_movea: M68k {
    M68k_movea(): M68k((OP(2)|OP(3)) & // 32- or 16-bit operation
                        EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 2==extract<12, 13>(w0) ? 32 : 16;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_movea, "movea."+d->sizeToLetter(nbits), src, dst);
    }
} movea;

// MOVEM.L #list, <ea>x
// MOVEM.L <ea>y, #list
static struct M68k_movem: M68k {
    M68k_movem(): M68k(OP(4) & BIT<11>(1) & BITS<6, 9>(3) &
                       EAM(m68k_eam_ari | m68k_eam_dsp)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *e1 = d->makeImmediate16(d->instruction_word(1));
        SgAsmExpression *e2 = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        if (extract<10, 10>(w0))
            std::swap(e1, e2);
        return d->makeInstruction(m68k_movem, "movem.l", e1, e2);
    }
} movem;

// MOVEQ.L #<data>, Dx
static struct M68k_moveq: M68k {
    M68k_moveq(): M68k(OP(7) & BIT<8>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediate32(signExtend<8, 32>(extract<0, 7>(w0)));
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_moveq, "moveq.l", src, dst);
    }
} moveq;

// MOVE.W CCR, Dx
static struct M68k_move_from_ccr: M68k {
    M68k_move_from_ccr(): M68k(OP(4) & BITS<3, 11>(0x58)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeConditionCodeRegister();
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), 16);
        return d->makeInstruction(m68k_move, "move.w", src, dst);
    }
} move_from_ccr;

// MOVE.B Dy, CCR
// MOVE.B #<data>, CCR
static struct M68k_move_to_ccr: M68k {
    M68k_move_to_ccr(): M68k(OP(4) & BITS<6, 11>(0x13) &
                             EAM(m68k_eam_drd | m68k_eam_imm)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_move, "move.w", src, dst);
    }
} move_to_ccr;

// MULS.W <ea>y, Dx             16 x 16 -> 32
static struct M68k_muls_w: M68k {
    M68k_muls_w(): M68k(OP(12) & BITS<6, 8>(7) &
                        EAM(m68k_eam_all & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_muls, "muls.w", src, dst);
    }
} muls_w;

// MULU.W <ea>y, Dx             16 x 16 -> 32
static struct M68k_mulu_w: M68k {
    M68k_mulu_w(): M68k(OP(12) & BITS<6, 8>(3) &
                        EAM(m68k_eam_all & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_mulu, "mulu.w", src, dst);
    }
} mulu_w;

// MULS.L <ea>y, Dx             32 x 32 -> 32
// MULU.L <ea>y, Dx             32 x 32 -> 32
static struct M68k_multiply_l: M68k {
    M68k_multiply_l(): M68k(OP(4) & BITS<6, 11>(0x30) &
                            EAM(m68k_eam_drd | m68k_eam_ari | m68k_eam_inc | m68k_eam_dec | m68k_eam_dsp)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = extract<11, 11>(d->instruction_word(1)) ? m68k_muls : m68k_mulu;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
        return d->makeInstruction(kind, stringifyM68kInstructionKind(kind, "m68k_")+".l", src, dst);
    }
} multiply_l;

// MVS.B <ea>y, Dx
// MVS.W <ea>y, Dx
static struct M68k_mvs: M68k {
    M68k_mvs(): M68k(OP(7) & BITS<7, 8>(2) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = extract<6, 6>(w0) ? 16 : 8;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_mvs, "mvs."+d->sizeToLetter(nbits), src, dst);
    }
} mvs;

// MVZ.B <ea>y, Dx
// MVZ.W <ea>y, Dx
static struct M68k_mvz: M68k {
    M68k_mvz(): M68k(OP(7) & BITS<7, 8>(3) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = extract<6, 6>(w0) ? 16 : 8;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_mvz, "mvz."+d->sizeToLetter(nbits), src, dst);
    }
} mvz;

// NEG.L Dx
static struct M68k_neg: M68k {
    M68k_neg(): M68k(OP(4) & BITS<3, 11>(0x90)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_neg, "neg.l", d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} neg;

// NEGX.L Dx
static struct M68k_negx: M68k {
    M68k_negx(): M68k(OP(4) & BITS<3, 11>(0x10)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_negx, "negx.l", d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} negx;

// NOP
static struct M68k_nop: M68k {
    M68k_nop(): M68k(BITS<0, 15>(0x4e71)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_nop, "nop");
    }
} nop;

// NOT.L Dx
static struct M68k_not: M68k {
    M68k_not(): M68k(OP(4) & BITS<3, 11>(0xd0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_not, "not.l", d->makeDataRegister(extract<0, 2>(w0), 32));
    }
} not_1;

// OR.L <ea>y, Dx
static struct M68k_or_1: M68k {
    M68k_or_1(): M68k(OP(8) & BITS<6, 8>(2) &
                      EAM(m68k_eam_all & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_or, "or.l", src, dst);
    }
} or_1;

// OR.L Dy, <ea>x
static struct M68k_or_2: M68k {
    M68k_or_2(): M68k(OP(8) & BITS<6, 8>(6) &
                      EAM(m68k_eam_all & ~(m68k_eam_direct | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_or, "or.l", src, dst);
    }
} or_2;

// ORI.L #<data>, Dx
static struct M68k_ori: M68k {
    M68k_ori(): M68k(OP(0) & BITS<3, 11>(0x10)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediate32(d->instruction_word(2), d->instruction_word(1));
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), 32);
        return d->makeInstruction(m68k_ori, "ori.l", src, dst);
    }
} ori;

// PEA.L <ea>y
static struct M68k_pea: M68k {
    M68k_pea(): M68k(OP(4) & BITS<6, 11>(0x21) &
                     EAM(m68k_eam_ari | m68k_eam_dsp | m68k_eam_idx | m68k_eam_absolute | m68k_eam_pc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_pea, "pea.l", src);
    }
} pea;

// PULSE
static struct M68k_pulse: M68k {
    M68k_pulse(): M68k(BITS<0, 15>(0x4acc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_pulse, "pulse");
    }
} pulse;

// REMS.L <ea>y, Dw:Dx                  32-bit Dx/32-bit <ea>y  :> 32-bit remainder in Dw
// REMU.L <ea>y, Dw:Dx                  32-bit Dx/32-bit <ea>y  :> 32-bit remainder in Dw
static struct M68k_remainder_l: M68k {
    M68k_remainder_l(): M68k(OP(4) & BITS<6, 11>(0x31) &
                             EAM(m68k_eam_drd | m68k_eam_ari | m68k_eam_inc |m68k_eam_dec |m68k_eam_dsp)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = extract<11, 11>(d->instruction_word(1)) ? m68k_rems : m68k_remu;
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dw = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
        SgAsmExpression *dx  = d->makeDataRegister(extract<0, 2>(d->instruction_word(1)), 32);
        return d->makeInstruction(kind, stringifyM68kInstructionKind(kind, "m68k_")+".l", ea, dw, dx);
    }
} remainder_l;

// RTS
static struct M68k_rts: M68k {
    M68k_rts(): M68k(BITS<0, 15>(0x4e75)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_rts, "rts");
    }
} rts;

// SATS.L Dx
static struct M68k_sats: M68k {
    M68k_sats(): M68k(OP(4) & BITS<3, 11>(0x190)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *dx = d->makeDataRegister(extract<0, 2>(w0), 32);
        return d->makeInstruction(m68k_sats, "sats.l", dx);
    }
} sats;

// Scc.B Dx
static struct M68k_scc: M68k {
    M68k_scc(): M68k(OP(5) & BITS<3, 7>(0x18)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<8, 11>(w0)) {
            case 0x0: kind = m68k_st;  break;
            case 0x1: kind = m68k_sf;  break;
            case 0x2: kind = m68k_shi; break;
            case 0x3: kind = m68k_sls; break;
            case 0x4: kind = m68k_scc; break;
            case 0x5: kind = m68k_scs; break;
            case 0x6: kind = m68k_sne; break;
            case 0x7: kind = m68k_seq; break;
            case 0x8: kind = m68k_svc; break;
            case 0x9: kind = m68k_svs; break;
            case 0xa: kind = m68k_spl; break;
            case 0xb: kind = m68k_smi; break;
            case 0xc: kind = m68k_sge; break;
            case 0xd: kind = m68k_slt; break;
            case 0xe: kind = m68k_sgt; break;
            case 0xf: kind = m68k_sle; break;
        }
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), 8);
        return d->makeInstruction(kind, stringifyM68kInstructionKind(kind, "m68k_")+".b", dst);
    }
} scc;
                
// SUB.L <ea>y, Dx
static struct M68k_sub_1: M68k {
    M68k_sub_1(): M68k(OP(9) & BITS<6, 8>(2) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_sub, "sub.l", src, dst);
    }
} sub_1;

// SUB.L Dy, <ea>x
static struct M68k_sub_2: M68k {
    M68k_sub_2(): M68k(OP(9) & BITS<6, 8>(6) &
                       EAM(m68k_eam_all & ~(m68k_eam_direct|m68k_eam_imm|m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_sub, "sub.l", src, dst);
    }
} sub_2;

// SUBA.L <ea>y, Ax
static struct M68k_suba: M68k {
    M68k_suba(): M68k(OP(9) & BITS<6, 8>(7) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_suba, "suba.l", src, dst);
    }
} suba;

// SUBI.L #<data>, Dx
static struct M68k_subi: M68k {
    M68k_subi(): M68k(OP(0) & BITS<3, 11>(0x90)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediate32(d->instruction_word(2), d->instruction_word(1));
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), 32);
        return d->makeInstruction(m68k_subi, "subi.l", src, dst);
    }
} subi;

// SUBQ.L #<data>, <ea>x
static struct M68k_subq: M68k {
    M68k_subq(): M68k(OP(5) & BITS<6, 8>(6) &
                      EAM(m68k_eam_all & ~(m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        int val = extract<9, 11>(w0);
        if (0==val)
            val = 8;
        SgAsmExpression *src = d->makeImmediate32(val);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_subq, "subq.l", src, dst);
    }
} subq;

// SUBX.L Dy, Dx
static struct M68k_subx: M68k {
    M68k_subx(): M68k(OP(9) & BITS<3, 8>(0x30)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), 32);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_subx, "subx.l", src, dst);
    }
} subx;

// SWAP.W Dx
static struct M68k_swap: M68k {
    M68k_swap(): M68k(OP(4) & BITS<3, 11>(0x108)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), 32);
        return d->makeInstruction(m68k_swap, "swap.w", src);
    }
} swap_1;

// TAS.B <ea>x
static struct M68k_tas: M68k {
    M68k_tas(): M68k(OP(4) & BITS<6, 11>(0x2b) &
                     EAM(m68k_eam_all & ~(m68k_eam_direct | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        return d->makeInstruction(m68k_tas, "tas.b", src);
    }
} tas;

// TPF
// TPF.W #<data>
// TPF.L #<data>
static struct M68k_tpf: M68k {
    M68k_tpf(): M68k(OP(5) & BITS<3, 11>(0x3f)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = NULL;
        unsigned opmode = extract<0, 2>(w0);
        switch (opmode) {
            case 2:
                src = d->makeImmediate16(d->instruction_word(1));
                return d->makeInstruction(m68k_tpf, "tpf.w", src);
            case 3:
                src = d->makeImmediate32(d->instruction_word(2), d->instruction_word(1));
                return d->makeInstruction(m68k_tpf, "tpf.l", src);
            case 4:
                return d->makeInstruction(m68k_tpf, "tpf");
            default:
                throw Disassembler::Exception("invalid opmode "+StringUtility::numberToString(opmode)+" for TPF instruction");
        }
    }
} tpf;

// TRAP #<vector>
static struct M68k_trap: M68k {
    M68k_trap(): M68k(OP(4) & BITS<4, 11>(0xe4)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *vector = d->makeImmediate8(extract<0, 3>(w0));
        return d->makeInstruction(m68k_trap, "trap", vector);
    }
} trap;

// TST.B <ea>y
// TST.W <ea>y
// TST.L <ea>y
static struct M68k_tst_b: M68k {
    M68k_tst_b(): M68k(OP(4) & BITS<8, 11>(0xa) &
                       (((BITS<6, 7>(1) | BITS<6, 7>(2) | BITS<6, 7>(3)) & EAM(m68k_eam_all)) |
                        (BITS<6, 7>(0) & EAM(m68k_eam_all & ~m68k_eam_ard)))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0:
                nbits = 8;
                break;
            case 1:
            case 3:
                nbits = 16;
                break;
            case 2:
                nbits = 32;
                break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_tst, "tst."+d->sizeToLetter(nbits), src);
    }
} tst;

// UNLK Ax
static struct M68k_unlk: M68k {
    M68k_unlk(): M68k(OP(4) & BITS<3, 11>(0x1cb)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *dst = d->makeAddressRegister(extract<0, 2>(w0), 32);
        return d->makeInstruction(m68k_unlk, "unlk", dst);
    }
} unlk;

// WDDATA.B <ea>y
// WDDATA.W <ea>y
// WDDATA.L <ea>y
static struct M68k_wddata: M68k {
    M68k_wddata(): M68k(OP(15) & BITS<8, 11>(0xb) &
                        EAM(m68k_eam_all & ~(m68k_eam_direct | m68k_eam_imm | m68k_eam_pc))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
            case 3: throw Disassembler::Exception("invalid size for WDDATA instruction");
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_wddata, "wddata."+d->sizeToLetter(nbits), src);
    }
} wddata;

        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
DisassemblerM68k::init()
{
    set_registers(RegisterDictionary::dictionary_mcf5484()); // only a default
    set_wordsize(2);
    set_alignment(2);
    set_sex(ByteOrder::ORDER_MSB);

    insert_idis(&add_1);
    insert_idis(&add_2);
    insert_idis(&adda);
    insert_idis(&addi);
    insert_idis(&addq);
    insert_idis(&addx);
    insert_idis(&and_1);
    insert_idis(&and_2);
    insert_idis(&andi);
    insert_idis(&ashift);
    insert_idis(&branch);
    insert_idis(&bchg_1);
    insert_idis(&bchg_2);
    insert_idis(&bclr_1);
    insert_idis(&bclr_2);
    insert_idis(&bitrev);
    insert_idis(&bset_1);
    insert_idis(&bset_2);
    insert_idis(&btst_1);
    insert_idis(&btst_2);
    insert_idis(&clr);
    insert_idis(&cmp);
    insert_idis(&cmpi);
    insert_idis(&divs_w);
    insert_idis(&divu_w);
    insert_idis(&divide_l);
    insert_idis(&eor);
    insert_idis(&eori);
    insert_idis(&ext_w);
    insert_idis(&ext_l);
    insert_idis(&extb_l);
    insert_idis(&ff1);
    insert_idis(&illegal);
    insert_idis(&jmp);
    insert_idis(&jsr);
    insert_idis(&lea);
    insert_idis(&link_1);
    insert_idis(&lshift);
    insert_idis(&mov3q);
    insert_idis(&move);
    insert_idis(&movea);
    insert_idis(&movem);
    insert_idis(&moveq);
    insert_idis(&move_from_ccr);
    insert_idis(&move_to_ccr);
    insert_idis(&muls_w);
    insert_idis(&mulu_w);
    insert_idis(&multiply_l);
    insert_idis(&mvs);
    insert_idis(&mvz);
    insert_idis(&neg);
    insert_idis(&negx);
    insert_idis(&nop);
    insert_idis(&not_1);
    insert_idis(&or_1);
    insert_idis(&or_2);
    insert_idis(&ori);
    insert_idis(&pea);
    insert_idis(&pulse);
    insert_idis(&remainder_l);
    insert_idis(&rts);
    insert_idis(&sats);
    insert_idis(&scc);
    insert_idis(&sub_1);
    insert_idis(&sub_2);
    insert_idis(&suba);
    insert_idis(&subi);
    insert_idis(&subq);
    insert_idis(&subx);
    insert_idis(&swap_1);
    insert_idis(&tas);
    insert_idis(&tpf);
    insert_idis(&trap);
    insert_idis(&tst);
    insert_idis(&unlk);
    insert_idis(&wddata);
}
