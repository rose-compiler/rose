#include "sage3basic.h"

#include "DisassemblerM68k.h"
#include "integerOps.h"
#include "stringify.h"
#include "sageBuilderAsm.h"

using namespace rose;

#if 1 /*DEBUGGING [Robb P. Matzke 2013-10-02]*/
#include "AsmUnparser_compat.h"
#endif

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
    size_t hibit = lobit + 6 - 1; // inclusive
    for (size_t bitnum=0; eamodes!=0; ++bitnum) {
        unsigned bit = IntegerOps::shl1<unsigned>(bitnum);
        if (0 != (eamodes & bit)) {
            eamodes &= ~bit;
            M68kEffectiveAddressMode eam = (M68kEffectiveAddressMode)bit;
            switch (eam) {
                case m68k_eam_drd:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, reg, 0);
                    break;
                case m68k_eam_ard:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x08|reg, 0);
                    break;
                case m68k_eam_ari:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x10|reg, 0);
                    break;
                case m68k_eam_inc:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x18|reg, 0);
                    break;
                case m68k_eam_dec:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x20|reg, 0);
                    break;
                case m68k_eam_dsp:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x28|reg, 0);
                    break;
                case m68k_eam_idx8:
                    for (size_t reg=0; reg<8; ++reg)
                        retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    break;
                case m68k_eam_idxbd:
                    if (0 == (eamodes & m68k_eam_idx8)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case m68k_eam_mpost:
                    if (0 == (eamodes & m68k_eam_idx)) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case m68k_eam_mpre:
                    if (0 == (eamodes & (m68k_eam_idx | m68k_eam_mpost))) {
                        for (size_t reg=0; reg<8; ++reg)
                            retval |= Pattern(lobit, hibit, 0x30|reg, 0);
                    }
                    break;
                case m68k_eam_pcdsp:
                    retval |= Pattern(lobit, hibit, 0x3a, 0);
                    break;
                case m68k_eam_pcidx8:
                    retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case m68k_eam_pcidxbd:
                    if (0 == (eamodes & m68k_eam_pcidx8))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case m68k_eam_pcmpost:
                    if (0 == (eamodes & m68k_eam_pcidx))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case m68k_eam_pcmpre:
                    if (0 == (eamodes & (m68k_eam_pcidx | m68k_eam_pcmpost)))
                        retval |= Pattern(lobit, hibit, 0x3b, 0);
                    break;
                case m68k_eam_absw:
                    retval |= Pattern(lobit, hibit, 0x38, 0);
                    break;
                case m68k_eam_absl:
                    retval |= Pattern(lobit, hibit, 0x39, 0);
                    break;
                case m68k_eam_imm:
                    retval |= Pattern(lobit, hibit, 0x3c, 0);
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

template<size_t wordnum>
Pattern WORD(const Pattern &sub_pattern)
{
    return sub_pattern.shift_left(16*wordnum);
}

template<size_t lo>
Pattern BIT(unsigned val)
{
    return Pattern(lo, lo, val, 0);
}

static std::string
sizeToLetter(size_t nbits)
{
    switch (nbits) {
        case  8: return "b";
        case 16: return "w";
        case 32: return "l";
    }
    assert(!"invalid operand size");
    abort();
}

// see base class
bool
DisassemblerM68k::can_disassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_M68K_Family;
}

SgAsmType *
DisassemblerM68k::makeIntegerType(size_t nbits)
{
    switch (nbits) {
        case 8:  return SgAsmTypeByte::createType();
        case 16: return SgAsmTypeWord::createType();
        case 32: return SgAsmTypeDoubleWord::createType();
        default:
            assert(!"data width not handled yet");
            abort(); // FIXME [Robb P. Matzke 2013-10-07]
    }
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeDataRegister(unsigned regnum, size_t nbits, size_t bit_offset)
{
    assert(regnum <= 7);
    assert(8==nbits || 16==nbits || 32==nbits);
    assert(0==bit_offset || 16==bit_offset);
    assert(bit_offset+nbits <= 32);
    RegisterDescriptor desc(m68k_regclass_data, regnum, bit_offset, nbits);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeAddressRegister(unsigned regnum, size_t nbits, size_t bit_offset)
{
    assert(regnum <= 7);
    assert(0==bit_offset || 16==bit_offset);
    assert(bit_offset+nbits <= 32);
    if (16!=nbits && 32!=nbits) // in particular, 8-bit access is not possible
        throw Exception("invalid address register size: " + StringUtility::numberToString(nbits));
    RegisterDescriptor desc(m68k_regclass_addr, regnum, bit_offset, nbits);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmMemoryReferenceExpression *
DisassemblerM68k::makeAddressRegisterPreDecrement(unsigned regnum, size_t nbits)
{
    SgAsmType *type = makeIntegerType(nbits);
    SgAsmM68kRegisterReferenceExpression *rre = makeAddressRegister(regnum, 32);
    rre->set_adjustment(-(nbits/8)); // pre decrement number of bytes
    return SageBuilderAsm::makeMemoryReference(rre, NULL/*segment*/, type);
}

SgAsmMemoryReferenceExpression *
DisassemblerM68k::makeAddressRegisterPostIncrement(unsigned regnum, size_t nbits)
{
    SgAsmType *type = makeIntegerType(nbits);
    SgAsmM68kRegisterReferenceExpression *rre = makeAddressRegister(regnum, 32);
    rre->set_adjustment(nbits/8); // post increment number of bytes
    return SageBuilderAsm::makeMemoryReference(rre, NULL/*segment*/, type);
}
    

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeDataAddressRegister(unsigned regnum, size_t nbits, size_t bit_offset)
{
    assert(regnum <= 15);
    return regnum <= 7 ? makeDataRegister(regnum, nbits, bit_offset) : makeAddressRegister(regnum-8, nbits, bit_offset);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeStatusRegister()
{
    RegisterDescriptor desc(m68k_regclass_spr, m68k_spr_sr, 0, 16);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeConditionCodeRegister()
{
    RegisterDescriptor desc(m68k_regclass_spr, m68k_spr_sr, 0, 8); // CCR is the low byte of the status register
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeProgramCounter()
{
    RegisterDescriptor desc(m68k_regclass_spr, m68k_spr_pc, 0, 32);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeMacRegister(M68kMacRegister minor)
{
    RegisterDescriptor desc(m68k_regclass_mac, minor, 0, 32);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeEmacRegister(M68kEmacRegister minor)
{
    size_t nbits = 0;
    switch (minor) {
        case m68k_emac_macsr:
        case m68k_emac_mask:
            nbits = 32;
            break;
        case m68k_emac_acc0:
        case m68k_emac_acc1:
        case m68k_emac_acc2:
        case m68k_emac_acc3:
            nbits = 48;
            break;
        // no default: we want the compiler warning!
    }
    assert(0!=nbits);
    RegisterDescriptor desc(m68k_regclass_emac, minor, 0, nbits);
    return new SgAsmM68kRegisterReferenceExpression(desc);
}

SgAsmM68kRegisterReferenceExpression *
DisassemblerM68k::makeRegister(const RegisterDescriptor &reg)
{
    return new SgAsmM68kRegisterReferenceExpression(reg);
}

SgAsmIntegerValueExpression *
DisassemblerM68k::makeImmediateValue(size_t nbits, unsigned value)
{
    SgAsmIntegerValueExpression *retval = new SgAsmIntegerValueExpression(value, nbits);
    retval->set_type(makeIntegerType(nbits));
    return retval;
}

SgAsmIntegerValueExpression *
DisassemblerM68k::makeImmediateExtension(size_t nbits, size_t ext_word_number)
{
    uint64_t value = 0;
    switch (nbits) {
        case 8:
            value = extract<0, 7>(instruction_word(ext_word_number+1));
            break;
        case 16:
            value = instruction_word(ext_word_number+1);
            break;
        case 32: {
            uint32_t hi_part = instruction_word(ext_word_number+1);
            uint32_t lo_part = instruction_word(ext_word_number+2);
            value = IntegerOps::shiftLeft<32>(hi_part, 16) | lo_part;
            break;
        }
        default:
            assert(!"word size not handled yet");
            abort(); // [Robb P. Matzke 2013-10-08]
    }

    SgAsmIntegerValueExpression *retval = new SgAsmIntegerValueExpression(value, nbits);
    retval->set_type(makeIntegerType(nbits));
    return retval;
}

#if 0
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
        case 6: return m68k_eam_idx | m68k_eam_mi;
        case 7:
            switch (reg) {
                case 0: return m68k_eam_absw;
                case 1: return m68k_eam_absl;
                case 2: return m68k_eam_pcdsp;
                case 3: return m68k_eam_pcidx | m68k_eam_pcmi;
                case 4: return m68k_eam_imm;
            }
    }
    assert(!"invalid effective address mode");
    abort();
}
#endif

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
        case m68k_eam_idx8:
        case m68k_eam_pcdsp:
        case m68k_eam_pcidx8:
        case m68k_eam_absw:
            return 1;

        case m68k_eam_idxbd:
        case m68k_eam_pcidxbd:
            ROSE_ASSERT(!"FIXME"); // 1, 2, or 3 extension words, but documentation is not clear [Robb P. Matzke 2013-10-07]

        case m68k_eam_mpost:
        case m68k_eam_mpre:
        case m68k_eam_pcmpost:
        case m68k_eam_pcmpre:
            ROSE_ASSERT(!"FIXME"); // 1-5 extension words, but documentation is not clear [Robb P. Matzke 2013-10-07]
            
        case m68k_eam_absl:
            return 2;

        case m68k_eam_imm:
            switch (nbits) {
                case 8: return 1;       // operand is in low-order byte of the single extension word
                case 16: return 1;      // operand is the single extension word
                case 32: return 2;      // high order bits in first extension word; low order bits in second word
                case 64: return 4;      // double precision is in four words
                case 80: return 6;      // extended precision and packed decimal real are in six words
                default:
                    assert(!"invalid width for m68k_eam_imm");
                    abort();
            }

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
#if 1 /*DEBUGGING [Robb P. Matzke 2013-10-08]*/
    M68kEffectiveAddressMode eam = m68k_eam_unknown;            // FIXME
#else
    M68kEffectiveAddressMode eam = effectiveAddressMode(modreg, nbits);
#endif
    SgAsmType *type = makeIntegerType(nbits);
    switch (eam) {
        case m68k_eam_drd:
            return makeDataRegister(reg, nbits);

        case m68k_eam_ard:
            return makeAddressRegister(reg, nbits);

        case m68k_eam_ari: {
            SgAsmM68kRegisterReferenceExpression *rre = makeAddressRegister(reg, 32);
            return SageBuilderAsm::makeMemoryReference(rre, NULL/*segment*/, type);
        }

        case m68k_eam_inc:
            return makeAddressRegisterPreDecrement(reg, nbits);

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

        case m68k_eam_idx8:
        case m68k_eam_pcidx8: {
            unsigned ext1 = instruction_word(ext_offset+1);
            SgAsmM68kRegisterReferenceExpression *areg = eam==m68k_eam_idx ? makeAddressRegister(reg, 32) : makeProgramCounter();
            uint64_t val = signExtend<8, 32>(extract<0, 7>(ext1));
            SgAsmIntegerValueExpression *dsp = new SgAsmIntegerValueExpression(val, 32);
            SgAsmExpression *addr = SageBuilderAsm::makeAdd(areg, dsp);
            bool da_bit = 0!=extract<15, 15>(ext1);
            bool wl_bit = 0!=extract<11, 11>(ext1);
            if (!wl_bit)
                throw Exception("16-bit index registers not supported by this architecture");
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

        case m68k_eam_idxbd:
        case m68k_eam_pcidxbd:
        case m68k_eam_mpost:
        case m68k_eam_mpre:
        case m68k_eam_pcmpost:
        case m68k_eam_pcmpre:
            // FIXME: Documentation isn't clear about which extension words hold which parts of this address. [RPM 2013-10-07]
            assert(!"address mode not handled yet");
            abort();

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

        case m68k_eam_imm:
            return makeImmediateExtension(nbits, ext_offset);

        default: {
            unsigned mode = modreg >> 3;
            throw Exception(std::string("invalid effective address mode:") +
                            " mode=" + StringUtility::numberToString(mode) +
                            " reg="  + StringUtility::numberToString(reg));
        }
    }
}

DisassemblerM68k::ExpressionPair
DisassemblerM68k::makeOffsetWidthPair(unsigned w1)
{
    SgAsmExpression *offset=NULL, *width=NULL;
    if (extract<11, 11>(w1)) {
        if (extract<9, 11>(w1))
            throw Exception("bits 9 and 10 of extension word 1 should be zero when bit 11 is set");
        offset = makeDataRegister(extract<6, 8>(w1), 32);
    } else {
        offset = makeImmediateValue(8, extract<6, 10>(w1));
    }
    if (extract<5, 5>(w1)) {
        if (extract<3, 4>(w1))
            throw Exception("bits 3 and 4 of extension word 1 should be zero when bit 5 is set");
        width = makeDataRegister(extract<0, 2>(w1), 32);
    } else {
        width = makeImmediateValue(8, extract<0, 4>(w1));
    }
    return ExpressionPair(offset, width);
}

SgAsmInstruction *
DisassemblerM68k::make_unknown_instruction(const Disassembler::Exception &e)
{
    return makeInstruction(m68k_unknown_instruction, "unknown");
}

SgAsmM68kInstruction *
DisassemblerM68k::makeInstruction(M68kInstructionKind kind, const std::string &mnemonic,
                                  SgAsmExpression *op1, SgAsmExpression *op2, SgAsmExpression *op3,
                                  SgAsmExpression *op4, SgAsmExpression *op5, SgAsmExpression *op6)
{
    SgAsmM68kInstruction *insn = new SgAsmM68kInstruction(get_insn_va(), mnemonic, kind);

    SgAsmOperandList *operands = new SgAsmOperandList;
    insn->set_operandList(operands);
    operands->set_parent(insn);

    /* If any operand is null, then the following operands must also be null because analysis assumes that the operand vector
     * indices correspond to argument positions and don't expect null-padding in the vector. */
    ROSE_ASSERT((!op1 && !op2 && !op3 && !op4 && !op5 && !op6) ||
                ( op1 && !op2 && !op3 && !op4 && !op5 && !op6) ||
                ( op1 &&  op2 && !op3 && !op4 && !op5 && !op6) ||
                ( op1 &&  op2 &&  op3 && !op4 && !op5 && !op6) ||
                ( op1 &&  op2 &&  op3 &&  op4 && !op5 && !op6) ||
                ( op1 &&  op2 &&  op3 &&  op4 &&  op5 && !op6) ||
                ( op1 &&  op2 &&  op3 &&  op4 &&  op5 &&  op6));

    if (op1)
        SageBuilderAsm::appendOperand(insn, op1);
    if (op2)
        SageBuilderAsm::appendOperand(insn, op2);
    if (op3)
        SageBuilderAsm::appendOperand(insn, op3);
    if (op4)
        SageBuilderAsm::appendOperand(insn, op4);
    if (op5)
        SageBuilderAsm::appendOperand(insn, op5);
    if (op6)
        SageBuilderAsm::appendOperand(insn, op6);

    return insn;
}


// Returns the Nth 16-bit word of an instruction, reading it from memory if necessary.  Instructions contain one, two, or
// three 16-bit words.
uint16_t
DisassemblerM68k::instruction_word(size_t n)
{
    if (n>2)
        throw Exception("malformed instruction uses more than two extension words");
    if (n>=niwords)
        throw Exception("short read for instruction word " + StringUtility::numberToString(n));
    niwords_used = std::max(niwords_used, n+1);
    return iwords[n];
}

// see base class
SgAsmInstruction *
DisassemblerM68k::disassembleOne(const MemoryMap *map, rose_addr_t start_va, AddressSet *successors)
{
    start_instruction(map, start_va);
    uint8_t buf[sizeof(iwords)]; // largest possible instruction
    size_t nbytes = map->read(buf, start_va, sizeof buf, get_protection());
    niwords = nbytes / sizeof(iwords[0]);
    if (0==niwords)
        throw Exception("short read from memory map", start_va);
    for (size_t i=0; i<niwords; ++i)
        iwords[i] = ByteOrder::be_to_host(*(uint16_t*)(buf+2*i));
    niwords_used = 1;

    SgAsmM68kInstruction *insn = NULL;
    if (M68k *idis = find_idis(iwords, niwords))
        insn = (*idis)(this, instruction_word(0));
    if (!insn)
        throw Exception("cannot disassemble m68k instruction: "+StringUtility::addrToString(instruction_word(0), 16), start_va);

    assert(niwords_used>0);
    SgUnsignedCharList raw_bytes(buf+0, buf+2*niwords_used);
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
            std::cerr <<" for '" <<idis->name <<"' conflicts with existing key ";
            (*ti)->pattern.print(std::cerr, alternatives.second);
            std::cerr <<" for '" <<(*ti)->name <<"'\n";
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
 *                                      Integer instructions
 *******************************************************************************************************************************/

typedef DisassemblerM68k::M68k M68k;

// ABCD.B Dy, Dx
struct M68k_abcd_1: M68k {
    M68k_abcd_1(): M68k("abcd_1", m68k_family,
                        OP(12) & BITS<3, 8>(0x20)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 8;
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), nbits);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_abcd, "abcd."+sizeToLetter(nbits), src, dst);
    }
};

// ABCD.B -(Ay), -(Ax)
struct M68k_abcd_2: M68k {
    M68k_abcd_2(): M68k("abcd_2", m68k_family,
                        OP(12) & BITS<3, 8>(0x21)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 8;
        SgAsmExpression *src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), nbits);
        SgAsmExpression *dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_abcd, "abcd."+sizeToLetter(nbits), src, dst);
    }
};

// ADD.B <ea>y, Dx
struct M68k_add_1: M68k {
    M68k_add_1(): M68k("add_1", m68k_family,
                       OP(13) & BIT<8>(0) & BITS<6, 7>(0) & EAM(m68k_eam_all & ~m68k_eam_ard)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_add, "add."+sizeToLetter(nbits), src, dst);
    }
};

// ADD.W <ea>y, Dx
// ADD.L <ea>y, Dx
struct M68k_add_2: M68k {
    M68k_add_2(): M68k("add_2", m68k_family,
                       OP(13) & BIT<8>(0) & (BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_add, "add."+sizeToLetter(nbits), src, dst);
    }
};

// ADD.B Dy, <ea>x
// ADD.W Dy, <ea>x
// ADD.L Dy, <ea>x
struct M68k_add_3: M68k {
    M68k_add_3(): M68k("add_3", m68k_family,
                       OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(m68k_eam_memory & m68k_eam_alter)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), nbits);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_add, "add."+sizeToLetter(nbits), src, dst);
    }
};

// ADDA.W <ea>y, Ax
// ADDA.L <ea>y, Ax
struct M68k_adda: M68k {
    M68k_adda(): M68k("adda", m68k_family,
                      OP(13) & (BITS<6, 8>(3) | BITS<6, 8>(7)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 8>(w0)) {
            case 3: nbits=16; break;
            case 7: nbits=32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_adda, "adda."+sizeToLetter(nbits), src, dst);
    }
};

// ADDI.B #<data>, Dx
// ADDI.W #<data>, Dx
// ADDI.L #<data>, Dx
//
// Note: Reference manual text says all "data alterable" modes allowed, but the table lacks program counter memory
// indirect modes. I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct M68k_addi: M68k {
    M68k_addi(): M68k("addi", m68k_family,
                      OP(0) & BITS<8, 11>(0x6) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeImmediateExtension(nbits, 0); // immediate argument is in extension word(s)
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), nbits);
        return d->makeInstruction(m68k_addi, "addi."+sizeToLetter(nbits), src, dst);
    }
};

// ADDQ.B #<data>, <ea>x
// ADDQ.W #<data>, <ea>x
// ADDQ.L #<data>, <ea>x
//
// Note: Reference manual text says all "alterable modes" allowed, but the table lacks program counter memory indirect modes.
// I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct M68k_addq: M68k {
    M68k_addq(): M68k("addq", m68k_family,
                      OP(5) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        unsigned imm = extract<9, 11>(w0);
        if (0==imm)
            imm = 8;
        SgAsmExpression *src = d->makeImmediateValue(nbits, imm);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_addq, "addq."+sizeToLetter(nbits), src, dst);
    }
};

// ADDX.B Dy, Dx
// ADDX.W Dy, Dx
// ADDX.L Dy, Dx
struct M68k_addx_1: M68k {
    M68k_addx_1(): M68k("addx_1", m68k_family,
                        OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), nbits);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_addx, "addx."+sizeToLetter(nbits), src, dst);
    }
};

// ADDX.B -(Ay), -(Ax)
// ADDX.W -(Ay), -(Ax)
// ADDX.L -(Ay), -(Ax)
struct M68k_addx_2: M68k {
    M68k_addx_2(): M68k("addx_1", m68k_family,
                        OP(13) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), nbits);
        SgAsmExpression *dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_addx, "addx."+sizeToLetter(nbits), src, dst);

    }
};

// AND.B <ea>y, Dx
// AND.W <ea>y, Dx
// AND.L <ea>y, Dx
struct M68k_and_1: M68k {
    M68k_and_1(): M68k("and_1", m68k_family,
                       OP(12) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_and, "and."+sizeToLetter(nbits), src, dst);
    }
};

// AND.B Dy, <ea>x
// AND.W Dy, <ea>x
// AND.L Dy, <ea>x
//
// Note: Reference manual text says "memory alterable modes" are allowed, but the table lacks program counter memory indirect
// modes.  I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct M68k_and_2: M68k {
    M68k_and_2(): M68k("and_2", m68k_family,
                       OP(12) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), nbits);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_and, "and."+sizeToLetter(nbits), src, dst);
    }
};

// ANDI.B #<data>, <ea>x
// ANDI.W #<data>, <ea>x
// ANDI.L #<data>, <ea>x
//
// Note: Reference manual text says "data alterable modes" are allowed, but the table lacks program counter memory indirect
// modes.  I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct M68k_andi: M68k {
    M68k_andi(): M68k("andi", m68k_family,
                      OP(0) & BITS<8, 12>(2) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeImmediateExtension(nbits, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 32==nbits?2:1);
        return d->makeInstruction(m68k_andi, "andi."+sizeToLetter(nbits), src, dst);
    }
};

// ANDI.B #<data>, CCR
struct M68k_andi_to_ccr: M68k {
    M68k_andi_to_ccr(): M68k("andi_to_ccr", m68k_family,
                             OP(0) & BITS<0, 11>(0x23c) & WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 8;
        SgAsmExpression *src = d->makeImmediateValue(nbits, extract<0, 7>(d->instruction_word(1)));
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_andi, "andi."+sizeToLetter(nbits), src, dst);
    }
};

// ASL.B Dy, Dx         (Dy is the shift count)
// ASL.W Dy, Dx
// ASL.L Dy, Dx
// ASR.B Dy, Dx
// ASR.W Dy, Dx
// ASR.L Dy, Dx
struct M68k_ashift_1: M68k {
    M68k_ashift_1(): M68k("ashift_1", m68k_family,
                          OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(4)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32); // full register, mod 64
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), nbits);
        return d->makeInstruction(shift_left ? m68k_asl : m68k_asr,
                                  (shift_left ? "asl." : "asr.")+sizeToLetter(nbits),
                                  src, dst);
    }
};

// ASL.B #<nbits>, Dx        
// ASL.W #<nbits>, Dx        
// ASL.L #<nbits>, Dx        
// ASR.B #<nbits>, Dx        
// ASR.W #<nbits>, Dx        
// ASR.L #<nbits>, Dx        
struct M68k_ashift_2: M68k {
    M68k_ashift_2(): M68k("ashift_2", m68k_family,
                          OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        unsigned count = extract<9, 11>(w0);
        if (0==count)
            count = 8;
        SgAsmExpression *src = d->makeImmediateValue(3, count);
        SgAsmExpression *dst = d->makeDataRegister(extract<0, 2>(w0), nbits);
        return d->makeInstruction(shift_left ? m68k_asl : m68k_asr,
                                  (shift_left ? "asl." : "asr.")+sizeToLetter(nbits),
                                  src, dst);
    }
};

// ASL.W <ea>x
// ASR.W <ea>x
//
// Note: Reference manual text says "memory alterable modes" are allowed, but the table lacks program counter memory indirect
// modes.  I am favoring the table over the text. [Robb P. Matzke 2013-10-07]
struct M68k_ashift_3: M68k {
    M68k_ashift_3(): M68k("ashift_3", m68k_family,
                          OP(14) & BITS<9, 11>(0) & BITS<6, 7>(3) &
                          EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        bool shift_left = extract<8, 8>(w0) ? true : false;
        size_t nbits = 16;
        // src is implied #<1>
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(shift_left ? m68k_asl : m68k_asr,
                                  (shift_left ? "asl." : "asr.")+sizeToLetter(nbits),
                                  dst);
    }
};

// Bcc.B <label>
// Bcc.W <label>
// Bcc.L <label>
//      Mnemonic Condition              Value (table 3.19)
//               true                   0000    0               (BRA instruction)
//               false                  0001    1               (BSR instruction)
//      hi       hi                     0010    2
//      ls       lo or same             0011    3
//      cc(hi)   carry clear            0100    4
//      cs(lo)   carry set              0101    5
//      ne       not equal              0110    6
//      eq       equal                  0111    7
//      vc       overflow clear         1000    8
//      vs       overflow set           1001    9
//      pl       plus                   1010    a
//      mi       minus                  1011    b
//      ge       greater or equal       1100    c
//      lt       less than              1101    d
//      gt       greater than           1110    e
//      le       less or equal          1111    f
//
// BRA.B <label>
// BRA.W <label>
// BRA.L <label>
// BSR.B <label>
// BSR.W <label>
// BSR.L <label>
struct M68k_branch: M68k {
    M68k_branch(): M68k("branch", m68k_family, OP(6)) {}
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
        SgAsmIntegerValueExpression *target = d->makeImmediateValue(32, target_va);
        return d->makeInstruction(kind, mnemonic, target);
    }
};

// BCHG.L #<bitnum>, <ea>x
struct M68k_bchg_1: M68k {
    M68k_bchg_1(): M68k("bchg_1", m68k_family,
                        OP(0) & BITS<6, 11>(0x21) & EAM(m68k_eam_drd) & WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bchg, "bchg.l", src, dst);
    }
};

// BCHG.B #<bitnum>, <ea>x
struct M68k_bchg_2: M68k {
    M68k_bchg_2(): M68k("bchg_2", m68k_family,
                        OP(0) & BITS<6, 11>(0x21) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd) &
                        WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 1);
        return d->makeInstruction(m68k_bchg, "bchg.b", src, dst);
    }
};

// BCHG.L Dy, <ea>x
struct M68k_bchg_3: M68k {
    M68k_bchg_3(): M68k("bchg_3", m68k_family,
                        OP(0) & BITS<6, 8>(5) & EAM(m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_bchg, "bchg.l", src, dst);
    }
};

// BCHG.B Dy, <ea>x
struct M68k_bchg_4: M68k {
    M68k_bchg_4(): M68k("bchg_4", m68k_family,
                        OP(0) & BITS<6, 8>(5) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        return d->makeInstruction(m68k_bchg, "bchg.b", src, dst);
    }
};

// BCLR.L #<bitnum>, <ea>x
struct M68k_bclr_1: M68k {
    M68k_bclr_1(): M68k("bclr_1", m68k_family,
                        OP(0) & BITS<6, 11>(0x22) & EAM(m68k_eam_drd) & WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bclr, "bclr.l", src, dst);
    }
};

// BCLR.B #<bitnum>, <ea>x
struct M68k_bclr_2: M68k {
    M68k_bclr_2(): M68k("bclr_2", m68k_family,
                        OP(0) & BITS<6, 11>(0x22) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd) &
                        WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 1);
        return d->makeInstruction(m68k_bclr, "bclr.b", src, dst);
    }
};

// BCLR.L Dy, <ea>x
struct M68k_bclr_3: M68k {
    M68k_bclr_3(): M68k("bclr_3", m68k_family,
                        OP(0) & BITS<6, 8>(6) & EAM(m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(20), 32, 0);
        return d->makeInstruction(m68k_bclr, "bclr.l", src, dst);
    }
};

// BCLR.B Dy, <ea>x
struct M68k_bclr_4: M68k {
    M68k_bclr_4(): M68k("bclr_4", m68k_family,
                        OP(0) & BITS<6, 8>(6) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(20), 8, 0);
        return d->makeInstruction(m68k_bclr, "bclr.b", src, dst);
    }
};

// BFCHG <ea>x {offset:width}
struct M68k_bfchg: M68k {
    M68k_bfchg(): M68k("bfchg", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x2b) & EAM(m68k_eam_drd | (m68k_eam_control & m68k_eam_alter)) &
                       WORD<1>(BITS<12, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instruction_word(1));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bfchg, "bfchg", dst, offset_width.first, offset_width.second);
    }
};

// BFCLR <ea>x {offset:width}
struct M68k_bfclr: M68k {
    M68k_bfclr(): M68k("bfclr", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x33) & EAM(m68k_eam_drd | (m68k_eam_control & m68k_eam_alter)) &
                       WORD<1>(BITS<12, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instruction_word(1));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bfclr, "bfclr", dst, offset_width.first, offset_width.second);
    }
};

// BFEXTS <ea>y {offset:width}, Dx
struct M68k_bfexts: M68k {
    M68k_bfexts(): M68k("bfexts", m68k_68020|m68k_68030|m68k_68040,
                        OP(0xe) & BITS<6, 11>(0x2f) & EAM(m68k_eam_drd | m68k_eam_control) &
                        WORD<1>(BIT<15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instruction_word(1));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
        return d->makeInstruction(m68k_bfexts, "bfexts", src, offset_width.first, offset_width.second, dst);
    }
};

// BFEXTU <ea>y {offset:width}, Dx
struct M68k_bfextu: M68k {
    M68k_bfextu(): M68k("bfextu", m68k_68020|m68k_68030|m68k_68040,
                        OP(0xe) & BITS<6, 11>(0x27) & EAM(m68k_eam_drd | m68k_eam_control) &
                        WORD<1>(BIT<15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instruction_word(1));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
        return d->makeInstruction(m68k_bfextu, "bfextu", src, offset_width.first, offset_width.second, dst);
    }
};

// BFFFO <ea>y {offset:width}, Dx
//
// Disabled because the documentation indicates that BFEXTU and BFFFO have the same bit pattern. [Robb P. Matzke 2013-10-28]
#if 0
struct M68k_bfffo: M68k {
    M68k_bfffo(): M68k("bfffo", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x27) & EAM(m68k_eam_drd | m68k_eam_control) &
                       WORD<1>(BIT<15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        ExpressionPair offset_width d->makeOffsetWidthPair(d->instruction_word(1));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
        return d->makeInstruction(m68k_bfffo, "bfffo", src, offset_width.first, offset_width.second, dst);
    }
};
#endif

// BFINS Dy, <ea>x {offset:width}
struct M68k_bfins: M68k {
    M68k_bfins(): M68k("bfins", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x3f) & EAM(m68k_eam_drd | (m68k_eam_control & m68k_eam_alter)) &
                       WORD<1>(BIT<15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instruction_word(1));
        SgAsmExpression *src = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bfins, "bfins", src, dst, offset_width.first, offset_width.second);
    }
};

// BFSET <ea>x {offset:width}
struct M68k_bfset: M68k {
    M68k_bfset(): M68k("bfset", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x33) & EAM(m68k_eam_drd | (m68k_eam_control & m68k_eam_alter)) &
                       WORD<1>(BITS<12, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instruction_word(1));
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bfset, "bfset", dst, offset_width.first, offset_width.second);
    }
};

// BFTST <ea>y {offset:width}
struct M68k_bftst: M68k {
    M68k_bftst(): M68k("bftst", m68k_68020|m68k_68030|m68k_68040,
                       OP(0xe) & BITS<6, 11>(0x23) & EAM(m68k_eam_drd | m68k_eam_control) &
                       WORD<1>(BITS<12, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        DisassemblerM68k::ExpressionPair offset_width = d->makeOffsetWidthPair(d->instruction_word(1));
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bftst, "bftst", src, offset_width.first, offset_width.second);
    }
};

// BKPT #<data>
struct M68k_bkpt: M68k {
    M68k_bkpt(): M68k("bkpt", m68k_68ec000|m68k_68010|m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                      OP(4) & BITS<3, 11>(0x109)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *data = d->makeImmediateValue(3, extract<0, 2>(w0));
        return d->makeInstruction(m68k_bkpt, "bkpt", data);
    }
};

// BSET.L #<bitnum>, <ea>x
struct M68k_bset_1: M68k {
    M68k_bset_1(): M68k("bset_1", m68k_family,
                        OP(0) & BITS<6, 11>(0x23) & EAM(m68k_eam_drd) & WORD<1>(BITS<9, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bset, "bset.l", src, dst);
    }
};

// BSET.B #<bitnum>, <ea>x
struct M68k_bset_2: M68k {
    M68k_bset_2(): M68k("bset_2", m68k_family,
                        OP(0) & BITS<6, 11>(0x23) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd) &
                        WORD<1>(BITS<9, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_bset, "bset.b", src, dst);
    }
};

// BSET.L Dy, <ea>x
struct M68k_bset_3: M68k {
    M68k_bset_3(): M68k("bset_3", m68k_family,
                        OP(0) & BITS<6, 8>(7) & EAM(m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_bset, "bset.l", src, dst);
    }
};

// BSET.B Dy, <ea>x
struct M68k_bset_4: M68k {
    M68k_bset_4(): M68k("bset_4", m68k_family,
                        OP(0) & BITS<6, 8>(7) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        return d->makeInstruction(m68k_bset, "bset.b", src, dst);
    }
};

// BTST.L #<bitnum>, <ea>x
struct M68k_btst_1: M68k {
    M68k_btst_1(): M68k("btst_1", m68k_family,
                        OP(0) & BITS<6, 11>(0x20) & EAM(m68k_eam_drd) &
                        WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_btst, "btst.l", src, dst);
    }
};

// BTST.B #<bitnum>, <ea>x
//
// Note: Text says data addressing modes can be used, but the table excludes immediate mode. [Robb P. Matzke 2013-10-28]
struct M68k_btst_2: M68k {
    M68k_btst_2(): M68k("btst_2", m68k_family,
                        OP(0) & BITS<6, 11>(0x20) & EAM(m68k_eam_data & ~m68k_eam_imm & ~m68k_eam_drd) &
                        WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 1);
        return d->makeInstruction(m68k_btst, "btst.b", src, dst);
    }
};

// BTST.L Dy, <ea>x
struct M68k_btst_3: M68k {
    M68k_btst_3(): M68k("btst_3", m68k_family,
                        OP(0) & BITS<6, 8>(4) & EAM(m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_btst, "btst.l", src, dst);
    }
};

// BTST.B Dy, <ea>x
struct M68k_btst_4: M68k {
    M68k_btst_4(): M68k("btst_4", m68k_family,
                        OP(0) & BITS<6, 8>(4) & EAM(m68k_eam_data & ~m68k_eam_drd)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        return d->makeInstruction(m68k_btst, "btst.b", src, dst);
    }
};

// CALLM #<data>, <ea>y
struct M68k_callm: M68k {
    M68k_callm(): M68k("callm", m68k_68020,
                       OP(0) & BITS<6, 11>(0x1b) & EAM(m68k_eam_control) &
                       WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *argcount = d->makeImmediateExtension(8, 0);
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_callm, "callm", argcount, src);
    }
};

// CAS Dc,Du <ea>x
//
// Note: The text say memory alterable addressing modes can be used, but the table excludes program counter memory indirect
// modes. I am preferring the table over the text. [Robb P. Matzke 2013-10-28]
struct M68k_cas: M68k {
    M68k_cas(): M68k("cas", m68k_68020|m68k_68030|m68k_68040,
                     OP(0) & BIT<11>(1) & BITS<6, 8>(3) & EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi) &
                     WORD<1>(BITS<9, 15>(0) & BITS<3, 5>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned w1 = d->instruction_word(1);
        SgAsmExpression *du = d->makeDataRegister(extract<6, 8>(w1), 32);
        SgAsmExpression *dc = d->makeDataRegister(extract<0, 2>(w1), 32);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
        return d->makeInstruction(m68k_cas, "cas", dc, du, dst);
    }
};

// CAS2 Dc1:Dc2, Du1:Du2, (Rx1):(Rx2)
struct M68k_cas2: M68k {
    M68k_cas2(): M68k("cas2", m68k_68020|m68k_68030|m68k_68040,
                      OP(0) & BIT<11>(1) & BITS<0, 8>(0x0fc) &
                      WORD<1>(BITS<9, 11>(0) & BITS<3, 5>(0)) &
                      WORD<2>(BITS<9, 11>(0) & BITS<3, 5>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned w1 = d->instruction_word(1);
        unsigned w2 = d->instruction_word(2);
        SgAsmExpression *rx1 = d->makeDataAddressRegister(extract<12, 15>(w1), 32);
        SgAsmExpression *rx2 = d->makeDataAddressRegister(extract<12, 15>(w2), 32);
        SgAsmExpression *du1 = d->makeDataRegister(extract<6, 8>(w1), 32);
        SgAsmExpression *du2 = d->makeDataRegister(extract<6, 8>(w2), 32);
        SgAsmExpression *dc1 = d->makeDataRegister(extract<0, 2>(w1), 32);
        SgAsmExpression *dc2 = d->makeDataRegister(extract<0, 2>(w2), 32);
        return d->makeInstruction(m68k_cas2, "cas2", dc1, dc2, du1, du2, rx1, rx2);
    }
};

// CHK.W <ea>, Dn
// CHK.L <ea>, Dn
struct M68k_chk: M68k {
    M68k_chk(): M68k("chk", m68k_family,
                     OP(4) & (BITS<7, 8>(2) | BITS<7, 8>(3)) & BIT<6>(0) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 3==extract<7, 8>(w0) ? 16 : 32;
        SgAsmExpression *bound = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *reg = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_chk, "chk."+sizeToLetter(nbits), bound, reg);
    }
};

// CHK2.B <ea>, Rn
// CHK2.W <ea>, Rn
// CHK2.L <ea>, Rn
struct M68k_chk2: M68k {
    M68k_chk2(): M68k("chk2", m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                      OP(0) & BIT<11>(0) & (BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(2)) & BITS<6, 8>(3) &
                      EAM(m68k_eam_control) &
                      WORD<1>(BITS<0, 11>(0x800))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits=0;
        switch (extract<9, 10>(w0)) {
            case 0: nbits=8; break;
            case 1: nbits=16; break;
            case 2: nbits=32; break;
        }
        SgAsmExpression *bounds = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 1);
        SgAsmExpression *reg = d->makeDataAddressRegister(extract<12, 15>(w0), nbits);
        return d->makeInstruction(m68k_chk2, "chk2."+sizeToLetter(nbits), bounds, reg);
    }
};

// CLR.B <ea>x
// CLR.W <ea>x
// CLR.L <ea>x
struct M68k_clr: M68k {
    M68k_clr(): M68k("clr", m68k_family,
                     OP(4) & BITS<8, 11>(2) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_clr, "clr."+sizeToLetter(nbits), dst);
    }
};

// CMP.B <ea>y, Dx
// CMP.W <ea>y, Dx
// CMP.L <ea>y, Dx
struct M68k_cmp: M68k {
    M68k_cmp(): M68k("cmp", m68k_family,
                     OP(11) & (BITS<6, 8>(0) | BITS<6, 8>(1) | BITS<6, 8>(2)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 8>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_cmp, "cmp."+sizeToLetter(nbits), src, dst);
    }
};

// CMPA.W <ea>y, Ax
// CMPA.L <ea>y, Ax
struct M68k_cmpa: M68k {
    M68k_cmpa(): M68k("cmpa", m68k_family,
                      OP(11) & (BITS<6, 8>(3) | BITS<6, 8>(7)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 8>(w0)) {
            case 3: nbits = 16; break;
            case 7: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_cmpa, "cmpa."+sizeToLetter(nbits), src, dst);
    }
};

// CMPI.B #<data>, <ea>
// CMPI.W #<data>, <ea>
// CMPI.L #<data>, <ea>
struct M68k_cmpi: M68k {
    M68k_cmpi(): M68k("cmpi", m68k_family,
                      OP(0) & BITS<8, 11>(0xc) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & ~m68k_eam_imm)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeImmediateExtension(nbits, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 32==nbits?2:1);
        return d->makeInstruction(m68k_cmpi, "cmpi."+sizeToLetter(nbits), src, dst);
    }
};

// CMPM.B (Ay)+, (Ax)+
// CMPM.W (Ay)+, (Ax)+
// CMPM.L (Ay)+, (Ax)+
struct M68k_cmpm: M68k {
    M68k_cmpm(): M68k("cmpm", m68k_family,
                      OP(11) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeAddressRegisterPostIncrement(extract<0, 2>(w0), nbits);
        SgAsmExpression *dst = d->makeAddressRegisterPostIncrement(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_cmpa, "cmpa."+sizeToLetter(nbits), src, dst);
    }
};

// CMP2.B <ea>, Rn
// CMP2.W <ea>, Rn
// CMP2.L <ea>, Rn
struct M68k_cmp2: M68k {
    M68k_cmp2(): M68k("cmp2", m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                      OP(0) & BIT<11>(0) & (BITS<9, 10>(0) | BITS<9, 10>(1) | BITS<9, 10>(2)) &
                      BITS<6, 8>(3) & EAM(m68k_eam_control) &
                      WORD<1>(BITS<0, 11>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *bounds = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *src2 = d->makeDataAddressRegister(extract<12, 15>(d->instruction_word(1)), nbits);
        return d->makeInstruction(m68k_cmp2, "cmp2."+sizeToLetter(nbits), bounds, src2);
    }
};

// CPBcc.W <label>
// CPBcc.L <label>
struct M68k_cpbcc: M68k {
    M68k_cpbcc(): M68k("cpbcc", m68k_68020|m68k_68030,
                       OP(15) & BITS<7, 8>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPBcc is not implemented");
    }
};

// CPDBcc.W Dn, <label>
struct M68k_cpdbcc: M68k {
    M68k_cpdbcc(): M68k("cpdbcc", m68k_68020|m68k_68030,
                        OP(15) & BITS<3, 8>(0x09) &
                        WORD<1>(BITS<6, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPDBcc is not implemented");
    }
};

// CPGEN <...>
struct M68k_cpgen: M68k {
    M68k_cpgen(): M68k("cpgen", m68k_68020|m68k_68030,
                       OP(15) & BITS<6, 8>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPGEN is not implemented");
    }
};

// CPSCC.B <ea>
struct M68k_cpscc: M68k {
    M68k_cpscc(): M68k("cpscc", m68k_68020|m68k_68030,
                       OP(15) & BITS<6, 8>(1) & EAM(m68k_eam_alter) &
                       WORD<1>(BITS<6, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPScc is not implemented");
    }
};

// CPTRAPcc.W #<data>
// CPTRAPcc.L #<data>
struct M68k_cptrapcc: M68k {
    M68k_cptrapcc(): M68k("cptrapcc", m68k_68020|m68k_68030,
                          OP(15) & BITS<3, 8>(0x0f) & (BITS<0, 2>(2) | BITS<0, 2>(3) | BITS<0, 2>(4)) &
                          WORD<1>(BITS<6, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        // Not implemented because we don't know what co-processors are present, and the co-processor determines the
        // size of this instruction. [Robb P. Matzke 2014-02-20]
        throw DisassemblerM68k::Exception("M68k CPTRAPcc is not implemented");
    }
};

// DBcc.W Dn, <label>
//      Mnemonic Condition              Value (table 3.19)
//               true                   0000    0               (BRA instruction)
//               false                  0001    1               (BSR instruction)
//      hi       hi                     0010    2
//      ls       lo or same             0011    3
//      cc(hi)   carry clear            0100    4
//      cs(lo)   carry set              0101    5
//      ne       not equal              0110    6
//      eq       equal                  0111    7
//      vc       overflow clear         1000    8
//      vs       overflow set           1001    9
//      pl       plus                   1010    a
//      mi       minus                  1011    b
//      ge       greater or equal       1100    c
//      lt       less than              1101    d
//      gt       greater than           1110    e
//      le       less or equal          1111    f
struct M68k_dbcc: M68k {
    M68k_dbcc(): M68k("dbcc", m68k_family,
                      OP(5) & BITS<3, 7>(0x19)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        unsigned cc = extract<8, 11>(w0);
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (cc) {
            case  0: kind = m68k_dbt; break;
            case  1: kind = m68k_dbf; break;
            case  2: kind = m68k_dbhi; break;
            case  3: kind = m68k_dbls; break;
            case  4: kind = m68k_dbcc; break;
            case  5: kind = m68k_dbcs; break;
            case  6: kind = m68k_dbne; break;
            case  7: kind = m68k_dbeq; break;
            case  8: kind = m68k_dbvc; break;
            case  9: kind = m68k_dbvs; break;
            case 10: kind = m68k_dbpl; break;
            case 11: kind = m68k_dbmi; break;
            case 12: kind = m68k_dbge; break;
            case 13: kind = m68k_dblt; break;
            case 14: kind = m68k_dbgt; break;
            case 15: kind = m68k_dble; break;
        }
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_");
        rose_addr_t target_va = d->get_insn_va() + 2 + signExtend<16, 32>(d->instruction_word(1));
        target_va &= GenMask<rose_addr_t, 32>::value;
        SgAsmIntegerValueExpression *target = d->makeImmediateValue(32, target_va);
        return d->makeInstruction(kind, mnemonic+".w", target);
    }
};

// DIVS.W <ea>y, Dx
struct M68k_divs_w: M68k {
    M68k_divs_w(): M68k("divs_w", m68k_family,
                        OP(8) & BITS<6, 8>(7) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_divs, "divs.w", src, dst);
    }
};

// DIVS.L <ea>y, Dx
// DIVU.L <ea>y, Dx
// REMS.L <ea>y, Dw:Dx                  32-bit Dx/32-bit <ea>y  :> 32-bit remainder in Dw
// REMU.L <ea>y, Dw:Dx                  32-bit Dx/32-bit <ea>y  :> 32-bit remainder in Dw
struct M68k_divrem_l: M68k {
    M68k_divrem_l(): M68k("divrem_l", m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                          OP(4) & BITS<6, 11>(0x31) & EAM(m68k_eam_data) &
                          WORD<1>(BIT<15>(0) & BITS<3, 9>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        if (extract<12, 14>(d->instruction_word(1)) == extract<0, 2>(d->instruction_word(1))) {
            // division
            M68kInstructionKind kind = extract<11, 11>(d->instruction_word(1)) ? m68k_divs : m68k_divu;
            SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
            SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
            return d->makeInstruction(kind, stringifyM68kInstructionKind(kind, "m68k_")+".l", src, dst);
        } else {
            // remainder
            M68kInstructionKind kind = extract<11, 11>(d->instruction_word(1)) ? m68k_rems : m68k_remu;
            SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
            SgAsmExpression *dw = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
            SgAsmExpression *dx  = d->makeDataRegister(extract<0, 2>(d->instruction_word(1)), 32);
            return d->makeInstruction(kind, stringifyM68kInstructionKind(kind, "m68k_")+".l", ea, dw, dx);
        }
    }
};
        
// DIVU.W
struct M68k_divu_w: M68k {
    M68k_divu_w(): M68k("divu_w", m68k_family,
                        OP(8) & BITS<6, 8>(3) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_divu, "divu.w", src, dst);
    }
};
    
// EOR.B Dy, <ea>x
// EOR.W Dy, <ea>x
// EOR.L Dy, <ea>x
struct M68k_eor: M68k {
    M68k_eor(): M68k("eor", m68k_family,
                     OP(11) & (BITS<6, 8>(4) | BITS<6, 8>(5) | BITS<6, 8>(6)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 8>(w0)) {
            case 4: nbits = 8;  break;
            case 5: nbits = 16; break;
            case 6: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), nbits);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_eor, "eor."+sizeToLetter(nbits), src, dst);
    }
};

// EORI.B #<data>, <ea>
// EORI.W #<data>, <ea>
// EORI.L #<data>, <ea>
struct M68k_eori: M68k {
    M68k_eori(): M68k("eori", m68k_family,
                      OP(0) & BITS<8, 11>(6) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8;  break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeImmediateExtension(nbits, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 32==nbits?2:1);
        return d->makeInstruction(m68k_eori, "eori."+sizeToLetter(nbits), src, dst);
    }
};

// EORI.B #<data>, CCR
struct M68k_eori_to_ccr: M68k {
    M68k_eori_to_ccr(): M68k("eori_to_ccr", m68k_family,
                             OP(0) & BITS<0, 11>(0xa3c) & WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateExtension(8, 0);
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_eori, "eori.b", src, dst);
    }
};

// EXG.L Dx, Dy
// EXG.L Ax, Ay
// EXG.L Dx, Ay
struct M68k_exg: M68k {
    M68k_exg(): M68k("exg", m68k_family,
                     OP(12) & BIT<8>(1) & (BITS<3, 7>(0x08) | BITS<3, 7>(0x09) | BITS<3, 7>(0x11))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx=NULL, *ry=NULL;
        switch (extract<3, 7>(w0)) {
            case 0x08:
                rx = d->makeDataRegister(extract<9, 11>(w0), 32);
                ry = d->makeDataRegister(extract<0, 2>(w0), 32);
                break;
            case 0x09:
                rx = d->makeAddressRegister(extract<9, 11>(w0), 32);
                ry = d->makeAddressRegister(extract<0, 2>(w0), 32);
                break;
            case 0x11:
                rx = d->makeDataRegister(extract<9, 11>(w0), 32);
                ry = d->makeAddressRegister(extract<0, 2>(w0), 32);
                break;
        }
        return d->makeInstruction(m68k_exg, "exg.l", rx, ry);
    }
};

// EXT.W Dx
struct M68k_ext_w: M68k {
    M68k_ext_w(): M68k("ext_w", m68k_family,
                       OP(4) & BITS<3, 11>(0x110)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(extract<0, 2>(w0), 16);
        return d->makeInstruction(m68k_ext, "ext.w", rx);
    }
};

// EXT.L Dx
struct M68k_ext_l: M68k {
    M68k_ext_l(): M68k("ext_l", m68k_family,
                       OP(4) & BITS<3, 11>(0x118)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(extract<0, 2>(w0), 32);
        return d->makeInstruction(m68k_ext, "ext.l", rx);
    }
};

// EXTB.L Dx
struct M68k_extb_l: M68k {
    M68k_extb_l(): M68k("extb_l", m68k_family,
                        OP(4) & BITS<3, 11>(0x138)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeDataRegister(extract<0, 2>(w0), 32);
        return d->makeInstruction(m68k_extb, "extb.l", rx);
    }
};

// ILLEGAL
struct M68k_illegal: M68k {
    M68k_illegal(): M68k("illegal", m68k_family,
                         OP(4) & BITS<0, 11>(0xafc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_illegal, "illegal");
    }
};

// JMP <ea>x
struct M68k_jmp: M68k {
    M68k_jmp(): M68k("jmp", m68k_family,
                     OP(4) & BITS<6, 11>(0x3b) & EAM(m68k_eam_control)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_jmp, "jmp", src);
    }
};
                
// JSR <ea>x
struct M68k_jsr: M68k {
    M68k_jsr(): M68k("jsr", m68k_family,
                     OP(4) & BITS<6, 11>(0x3a) & EAM(m68k_eam_control)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_jsr, "jsr", src);
    }
};

// LEA.L <ea>y, Ax
struct M68k_lea: M68k {
    M68k_lea(): M68k("lea", m68k_family,
                     OP(4) & BITS<6, 8>(7) & EAM(m68k_eam_control)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_lea, "lea.l", src, dst);
    }
};

// LINK.W An, #<displacement>
struct M68k_link_w: M68k {
    M68k_link_w(): M68k("link_w", m68k_family,
                        OP(4) & BITS<3, 11>(0x1ca)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeAddressRegister(extract<0, 2>(w0), 32);
        SgAsmExpression *dsp = d->makeImmediateExtension(16, 0);
        return d->makeInstruction(m68k_link, "link.w", rx, dsp);
    }
};

// LINK.L An, #<displacement>
struct M68k_link_l: M68k {
    M68k_link_l(): M68k("link_l", m68k_family,
                        OP(4) & BITS<3, 11>(0x101)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rx = d->makeAddressRegister(extract<0, 2>(w0), 32);
        SgAsmExpression *dsp = d->makeImmediateExtension(32, 0);
        return d->makeInstruction(m68k_link, "link.l", rx, dsp);
    }
};

// LSL.B Dy, Dx         as in (L)ogical (S)hift (L)eft
// LSL.W Dy, Dx
// LSL.L Dy, Dx
// LSR.B Dy, Dx
// LSR.W Dy, Dx
// LSR.L Dy, Dx
// Only register shifts, not memory
struct M68k_lshift_rr: M68k {
    M68k_lshift_rr(): M68k("lshift_rr", m68k_family,
                           OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(5)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *dy = d->makeDataRegister(extract<9, 11>(w0), 32, 0);
        SgAsmExpression *dx = d->makeDataRegister(extract<0, 2>(w0), nbits, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_lsl : m68k_lsr;
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + "." + sizeToLetter(nbits);
        return d->makeInstruction(kind, mnemonic, dy, dx);
    }
};

// LSL.B #<nbits>, Dx   as in (L)ogical (S)hift (L)eft
// LSL.W #<nbits>, Dx
// LSL.L #<nbits>, Dx
// LSR.B #<nbits>, Dx
// LSR.W #<nbits>, Dx
// LSR.L #<nbits>, Dx
// Only register shifts, not memory
struct M68k_lshift_ir: M68k {
    M68k_lshift_ir(): M68k("lshift_ir", m68k_family,
                           OP(14) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<3, 5>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        unsigned n = extract<9, 11>(w0);
        SgAsmExpression *sa = d->makeImmediateValue(8, 0==n ? 8 : n);
        SgAsmExpression *dx = d->makeDataRegister(extract<0, 2>(w0), nbits, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_lsl : m68k_lsr;
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + "." + sizeToLetter(nbits);
        return d->makeInstruction(kind, mnemonic, sa, dx);
    }
};

// LSL.W <ea>         as in (L)ogical (S)hift (L)eft
// LSR.W <ea>
// Only memory shifts, not registers
struct M68k_lshift_mem: M68k {
    M68k_lshift_mem(): M68k("lshift_mem", m68k_family,
                            OP(14) & BITS<9, 11>(1) & BITS<6, 7>(3) &
                            EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_lsl : m68k_lsr;
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + ".w";
        return d->makeInstruction(kind, mnemonic, ea);
    }
};

// MOVE.B <ea>y, <ea>x
// MOVE.W <ea>y, <ea>x
// MOVE.L <ea>y, <ea>x
struct M68k_move: M68k {
    M68k_move(): M68k("move", m68k_family,
                      (OP(1) | OP(2) | OP(3)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi, 6) &
                      EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<12, 13>(w0)) {
            case 1: nbits = 8; break;
            case 2: nbits = 32; break;                  // yes, not the usual order
            case 3: nbits = 16; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<6, 11>(w0), nbits, 0);
        return d->makeInstruction(m68k_move, "move."+sizeToLetter(nbits), src, dst);
    }
};

// MOVEA.W <ea>y, Ax
// MOVEA.L <ea>y, Ax
struct M68k_movea: M68k {
    M68k_movea(): M68k("movea", m68k_family,
                       (OP(2)|OP(3)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 2==extract<12, 13>(w0) ? 32 : 16;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_movea, "movea."+sizeToLetter(nbits), src, dst);
    }
};

// MOVE.W CCR, Dx
struct M68k_move_from_ccr: M68k {
    M68k_move_from_ccr(): M68k("move_from_ccr", m68k_68010|m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                               OP(4) & BITS<6, 11>(0x0b) &
                               EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeConditionCodeRegister();
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        return d->makeInstruction(m68k_move, "move.w", src, dst);
    }
};

// MOVE.W <ea>y, CCR
struct M68k_move_to_ccr: M68k {
    M68k_move_to_ccr(): M68k("move_to_ccr", m68k_family,
                             OP(4) & BITS<6, 11>(0x13) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_move, "move.w", src, dst);
    }
};

// MOVE.W SR, <ea>x
struct M68k_move_from_sr: M68k {
    M68k_move_from_sr(): M68k("move_from_sr", m68k_68000|m68k_68008,
                              OP(4) & BITS<6, 11>(0x03) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeStatusRegister();
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        return d->makeInstruction(m68k_move, "move.w", src, dst);
    }
};

// MOVE16 (Ax)+, (Ay)+
struct M68k_move16_pp: M68k {
    M68k_move16_pp(): M68k("move16_pp", m68k_68040,
                           OP(15) & BITS<3, 11>(0xc4) & WORD<1>(BIT<15>(1) & BITS<0, 11>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeAddressRegisterPostIncrement(extract<0, 2>(w0), 32);
        SgAsmExpression *dst = d->makeAddressRegisterPostIncrement(extract<12, 14>(d->instruction_word(1)), 32);
        return d->makeInstruction(m68k_move16, "move16", src, dst);
    }
};

// MOVE16 (xxx).L, (An)+
// MOVE16 (An)+, (xxx).L
// MOVE16 (xxx).L, (An)
// MOVE16 (An), (xxx).L
struct M68k_move16_a: M68k {
    M68k_move16_a(): M68k("move16_a", m68k_68040,
                          OP(15) & BITS<5, 11>(0x30)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        switch (extract<3, 4>(w0)) {
            case 0:                                     // MOVE16 (An)+, (xxx).L
                src = d->makeEffectiveAddress(030|extract<0, 2>(w0), 32, 0);
                dst = d->makeEffectiveAddress(071, 32, 0);
                break;
            case 1:                                     // MOVE16 (xxx).L, (An)+
                src = d->makeEffectiveAddress(071, 32, 0);
                dst = d->makeEffectiveAddress(030|extract<0, 2>(w0), 32, 0);
                break;
            case 2:                                     // MOVE16 (An), (xxx).L
                src = d->makeEffectiveAddress(020|extract<0, 2>(w0), 32, 0);
                dst = d->makeEffectiveAddress(071, 32, 0);
                break;
            case 3:                                     // MOVE16 (xxx).L, (An)
                src = d->makeEffectiveAddress(071, 32, 0);
                dst = d->makeEffectiveAddress(020|extract<0, 2>(w0), 32, 0);
                break;
        }
        return d->makeInstruction(m68k_move16, "move16", src, dst);
    }
};

// MOVEM.W #list, <ea>x
// MOVEM.L #list, <ea>x
struct M68k_movem_rm: M68k {
    M68k_movem_rm(): M68k("movem_rm", m68k_family,
                          OP(4) & BITS<7, 11>(0x11) &
                          EAM(m68k_eam_ari|m68k_eam_dec|m68k_eam_dsp|m68k_eam_idx|m68k_eam_mi|m68k_eam_abs)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = extract<6, 6>(w0) ? 32 : 16;
        SgAsmExpression *src = d->makeImmediateExtension(16, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 1);
        return d->makeInstruction(m68k_movem, "movem."+sizeToLetter(nbits), src, dst);
    }
};
                
// MOVEM.W <ea>y, #list
// MOVEM.L <ea>y, #list
struct M68k_movem_mr: M68k {
    M68k_movem_mr(): M68k("movem_mr", m68k_family,
                          OP(4) & BITS<7, 11>(0x19) & EAM(m68k_eam_control | m68k_eam_inc)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = extract<6, 6>(w0) ? 32 : 16;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 1);
        SgAsmExpression *dst = d->makeImmediateExtension(16, 0);
        return d->makeInstruction(m68k_movem, "movem."+sizeToLetter(nbits), src, dst);
    }
};

// MOVEP.W Dx, (d16,Ay)
// MOVEP.L Dx, (d16,Ay)
// MOVEP.W (d16,Ay), Dx
// MOVEP.L (d16,Ay), Dx
struct M68k_movep: M68k {
    M68k_movep(): M68k("movep", m68k_family,
                       OP(0) & BIT<8>(1) & BITS<3, 5>(1)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = extract<6, 6>(w0) ? 32 : 16;
        SgAsmExpression *src = d->makeEffectiveAddress(050|extract<0, 2>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        if (extract<7, 7>(w0))
            std::swap(src, dst);                        // register-to-memory
        return d->makeInstruction(m68k_movep, "movep."+sizeToLetter(nbits), src, dst);
    }
};

// MOVEQ.L #<data>, Dx
struct M68k_moveq: M68k {
    M68k_moveq(): M68k("moveq", m68k_family,
                       OP(7) & BIT<8>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateValue(32, signExtend<8, 32>(extract<0, 7>(w0)));
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_moveq, "moveq.l", src, dst);
    }
};

// MULS.W <ea>y, Dx             16 x 16 -> 32
struct M68k_muls_w: M68k {
    M68k_muls_w(): M68k("muls_w", m68k_family,
                        OP(12) & BITS<6, 8>(7) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_muls, "muls.w", src, dst);
    }
};

// MULS.L <ea>y, Dx             32 x 32 -> 32
// MULU.L <ea>y, Dx             32 x 32 -> 32
struct M68k_multiply_l: M68k {
    M68k_multiply_l(): M68k("multiply_l", m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                            OP(4) & BITS<6, 11>(0x30) & EAM(m68k_eam_data) &
                            WORD<1>(BIT<15>(0) & BITS<3, 10>(0))) {} // BITS<0,2> are unused and unspecified
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = extract<11, 11>(d->instruction_word(1)) ? m68k_muls : m68k_mulu;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
        return d->makeInstruction(kind, stringifyM68kInstructionKind(kind, "m68k_")+".l", src, dst);
    }
};

// MULS.L <ea>y, Dh, Dl         32 x 32 -> 64
// MULU.L <ea>y, Dh, DL         32 x 32 -> 64
struct M68k_multiply_64: M68k {
    M68k_multiply_64(): M68k("multiply_64", m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                             OP(4) & BITS<6, 11>(0x30) & EAM(m68k_eam_data) &
                             WORD<1>(BIT<15>(0) & BITS<3, 10>(0x80))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        M68kInstructionKind kind = extract<11, 11>(d->instruction_word(1)) ? m68k_muls : m68k_mulu;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        SgAsmExpression *dh = d->makeDataRegister(extract<0, 2>(d->instruction_word(1)), 32);
        SgAsmExpression *dl = d->makeDataRegister(extract<12, 14>(d->instruction_word(1)), 32);
        return d->makeInstruction(m68k_muls, stringifyM68kInstructionKind(kind, "m68k_")+".l", src, dh, dl);
    }
};

// MULU.W <ea>y, Dx             16 x 16 -> 32
struct M68k_mulu_w: M68k {
    M68k_mulu_w(): M68k("mulu_w", m68k_family,
                        OP(12) & BITS<6, 8>(3) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
        return d->makeInstruction(m68k_mulu, "mulu.w", src, dst);
    }
};

// NBCD.B <ea>
struct M68k_nbcd: M68k {
    M68k_nbcd(): M68k("nbcd", m68k_family,
                      OP(8) & BITS<6, 11>(0x20) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        return d->makeInstruction(m68k_nbcd, "nbcd.b", ea);
    }
};

// NEG.B <ea>
// NEG.W <ea>
// NEG.L <ea>
struct M68k_neg: M68k {
    M68k_neg(): M68k("neg", m68k_family,
                     OP(4) & BITS<8, 11>(4) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_neg, "neg."+sizeToLetter(nbits), ea);
    }
};

// NEGX.B <ea>
// NEGX.W <ea>
// NEGX.L <ea>
struct M68k_negx: M68k {
    M68k_negx(): M68k("negx", m68k_family,
                      OP(4) & BITS<8, 11>(0) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_negx, "negx."+sizeToLetter(nbits), ea);
    }
};

// NOP
struct M68k_nop: M68k {
    M68k_nop(): M68k("nop", m68k_family,
                     OP(4) & BITS<0, 11>(0xe71)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_nop, "nop");
    }
};

// NOT.B <ea>
// NOT.W <ea>
// NOT.L <ea>
struct M68k_not: M68k {
    M68k_not(): M68k("not", m68k_family,
                     OP(4) & BITS<8, 11>(6) & (BITS<6, 7>(0) |BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_not, "not."+sizeToLetter(nbits), ea);
    }
};

// OR.B <ea>y, Dx
// OR.W <ea>y, Dx
// OR.L <ea>y, Dx
struct M68k_or_1: M68k {
    M68k_or_1(): M68k("or_1", m68k_family,
                      OP(8) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_data)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_or, "or."+sizeToLetter(nbits), src, dst);
    }
};

// OR.B Dy, <ea>x
// OR.W Dy, <ea>x
// OR.L Dy, <ea>x
struct M68k_or_2: M68k {
    M68k_or_2(): M68k("or_2", m68k_family,
                      OP(8) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), nbits);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_or, "or."+sizeToLetter(nbits), src, dst);
    }
};

// ORI.B #<data>, <ea>
// ORI.W #<data>, <ea>
// ORI.L #<data>, <ea>
struct M68k_ori: M68k {
    M68k_ori(): M68k("ori", m68k_family,
                     OP(0) & BITS<8, 11>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                     EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeImmediateExtension(nbits, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 2>(w0), nbits, 32==nbits?2:1);
        return d->makeInstruction(m68k_ori, "ori."+sizeToLetter(nbits), src, dst);
    }
};

// ORI.B #<data>, CCR
struct M68k_ori_to_ccr: M68k {
    M68k_ori_to_ccr(): M68k("ori_to_ccr", m68k_family,
                            OP(0) & BITS<0, 11>(0x03c) & WORD<1>(BITS<8, 15>(0))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeImmediateValue(8, extract<0, 7>(d->instruction_word(1)));
        SgAsmExpression *dst = d->makeConditionCodeRegister();
        return d->makeInstruction(m68k_ori, "ori.b", src, dst);
    }
};

// PACK -(Ax), -(Ay), #<adjustment>
// PACK Dx, Dy, #<adjustment>
struct M68k_pack: M68k {
    M68k_pack(): M68k("pack", m68k_68020|m68k_68030|m68k_68040,
                      OP(8) & BITS<4, 8>(0x14)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // PACK -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), 16);
            dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), 16);
        } else {
            // PACK Dx, Dy
            src = d->makeDataRegister(extract<0, 2>(w0), 16, 0);
            dst = d->makeDataRegister(extract<9, 11>(w0), 16, 0);
        }
        SgAsmExpression *adj = d->makeImmediateExtension(16, 0);
        return d->makeInstruction(m68k_pack, "pack", src, dst, adj);
    }
};

// PEA.L <ea>y
struct M68k_pea: M68k {
    M68k_pea(): M68k("pea", m68k_family,
                     OP(4) & BITS<6, 11>(0x21) & EAM(m68k_eam_control)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
        return d->makeInstruction(m68k_pea, "pea.l", src);
    }
};

// ROL.B Dx, Dy         as in (RO)tate (L)eft
// ROL.W Dx, Dy
// ROL.L Dx, Dy
// ROL.B #<data>, Dy
// ROL.W #<data>, Dy
// ROL.L #<data>, Dy
// ROR.B Dx, Dy         as in (RO)tate (R)ight
// ROR.W Dx, Dy
// ROR.L Dx, Dy
// ROR.B #<data>, Dy
// ROR.W #<data>, Dy
// ROR.L #<data>, Dy
struct M68k_rotate_reg: M68k {
    M68k_rotate_reg(): M68k("rotate_reg", m68k_family,
                            OP(14) & BITS<3, 4>(3)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        unsigned n = extract<9, 11>(w0);                // "count/regisger"
        SgAsmExpression *sa = NULL;
        if (extract<5, 5>(w0)) {                        // "i/r"
            sa = d->makeDataRegister(extract<9, 11>(w0), 32, 0);
        } else {
            sa = d->makeImmediateValue(8, 0==n ? 8 : n);
        }
        SgAsmExpression *dy = d->makeDataRegister(extract<0, 2>(w0), nbits, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_rol : m68k_ror; // "dr"
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + "." + sizeToLetter(nbits);
        return d->makeInstruction(kind, mnemonic, sa, dy);
    }
};

// ROL.W <ea>y          memory rotate left
// ROR.W <ea>y          memory rotate right
struct M68k_rotate_mem: M68k {
    M68k_rotate_mem(): M68k("rotate_mem", m68k_family,
                            OP(14) & BITS<9, 11>(3) & BITS<6, 7>(3)
                            & EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_rol : m68k_ror; // "dr"
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + ".w";
        return d->makeInstruction(kind, mnemonic, eay);
    }
};

// ROXL.B Dx, Dy         as in (RO)tate with e(X)tend (L)eft
// ROXL.W Dx, Dy
// ROXL.L Dx, Dy
// ROXL.B #<data>, Dy
// ROXL.W #<data>, Dy
// ROXL.L #<data>, Dy
// ROXR.B Dx, Dy
// ROXR.W Dx, Dy
// ROXR.L Dx, Dy
// ROXR.B #<data>, Dy
// ROXR.W #<data>, Dy
// ROXR.L #<data>, Dy
struct M68k_rotate_extend_reg: M68k {
    M68k_rotate_extend_reg(): M68k("rotate_extend_reg", m68k_family,
                                   OP(14) & BITS<3, 4>(2)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        unsigned n = extract<9, 11>(w0);                // "count/regisger"
        SgAsmExpression *sa = NULL;
        if (extract<5, 5>(w0)) {                        // "i/r"
            sa = d->makeDataRegister(extract<9, 11>(w0), 32, 0);
        } else {
            sa = d->makeImmediateValue(8, 0==n ? 8 : n);
        }
        SgAsmExpression *dy = d->makeDataRegister(extract<0, 2>(w0), nbits, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_roxl : m68k_roxr; // "dr"
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + "." + sizeToLetter(nbits);
        return d->makeInstruction(kind, mnemonic, sa, dy);
    }
};

// ROXL.W <ea>y          memory rotate left
// ROXR.W <ea>y          memory rotate right
struct M68k_rotate_extend_mem: M68k {
    M68k_rotate_extend_mem(): M68k("rotate_extend_mem", m68k_family,
                                   OP(14) & BITS<9, 11>(2) & BITS<6, 7>(3)
                                   & EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), 16, 0);
        M68kInstructionKind kind = extract<8, 8>(w0) ? m68k_roxl : m68k_roxr; // "dr"
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + ".w";
        return d->makeInstruction(kind, mnemonic, eay);
    }
};

// RTD #<displacement>
struct M68k_rtd: M68k {
    M68k_rtd(): M68k("rtd", m68k_68010|m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                     OP(4) & BITS<0, 11>(0xe74)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *disp = d->makeImmediateExtension(16, 0);
        return d->makeInstruction(m68k_rtd, "rtd", disp);
    }
};

// RTM Rn
struct M68k_rtm: M68k {
    M68k_rtm(): M68k("rtm", m68k_68020,
                     OP(0) & BITS<4, 11>(0x6c)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *rn = d->makeDataAddressRegister(extract<0, 3>(w0), 32, 0);
        return d->makeInstruction(m68k_rtm, "rtm", rn);
    }
};

// RTR
struct M68k_rtr: M68k {
    M68k_rtr(): M68k("rtr", m68k_family,
                     OP(4) & BITS<0, 11>(0xe77)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_rtr, "rtr");
    }
};

// RTS
struct M68k_rts: M68k {
    M68k_rts(): M68k("rts", m68k_family,
                     OP(4) & BITS<0, 11>(0xe75)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_rts, "rts");
    }
};

// SBCD.B Dx, Dy
// SBCD.B -(Ax), -(Ay)
struct M68k_sbcd: M68k {
    M68k_sbcd(): M68k("sbcd", m68k_family,
                      OP(8) & BITS<4, 8>(0x10)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // SBCD.B -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), 8);
            dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), 8);
        } else {
            // SBCD.B Dx, Dy
            src = d->makeDataRegister(extract<0, 2>(w0), 8, 0);
            dst = d->makeDataRegister(extract<9, 11>(w0), 8, 0);
        }
        return d->makeInstruction(m68k_sbcd, "sbcd.b", src, dst);
    }
};

// Scc.B <ea>
//      Mnemonic Condition              Value (table 3.19)
//      t        true                   0000    0
//      f        false                  0001    1
//      hi       hi                     0010    2
//      ls       lo or same             0011    3
//      cc(hi)   carry clear            0100    4
//      cs(lo)   carry set              0101    5
//      ne       not equal              0110    6
//      eq       equal                  0111    7
//      vc       overflow clear         1000    8
//      vs       overflow set           1001    9
//      pl       plus                   1010    a
//      mi       minus                  1011    b
//      ge       greater or equal       1100    c
//      lt       less than              1101    d
//      gt       greater than           1110    e
//      le       less or equal          1111    f
struct M68k_scc: M68k {
    M68k_scc(): M68k("scc", m68k_family,
                     OP(5) & BITS<6, 7>(3) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
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
        std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + ".b";
        SgAsmExpression *ea = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        return d->makeInstruction(kind, mnemonic, ea);
    }
};

// SUB.B <ea>y, Dx
// SUB.W <ea>y, Dx
// SUB.L <ea>y, Dx
struct M68k_sub_1: M68k {
    M68k_sub_1(): M68k("sub_1", m68k_family,
                       OP(9) & BIT<8>(0) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), nbits);
        return d->makeInstruction(m68k_sub, "sub."+sizeToLetter(nbits), src, dst);
    }
};

// SUB.B Dy, <ea>x
// SUB.W Dy, <ea>x
// SUB.L Dy, <ea>x
struct M68k_sub_2: M68k {
    M68k_sub_2(): M68k("sub_2", m68k_family,
                       OP(9) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                       EAM(m68k_eam_memory & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeDataRegister(extract<9, 11>(w0), nbits);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_sub, "sub."+sizeToLetter(nbits), src, dst);
    }
};

// SUBA.W <ea>y, Ax
// SUBA.L <ea>y, Ax
struct M68k_suba: M68k {
    M68k_suba(): M68k("suba", m68k_family,
                      OP(9) & BITS<9, 10>(3) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = extract<8, 8>(w0) ? 32 : 16;
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        SgAsmExpression *dst = d->makeAddressRegister(extract<9, 11>(w0), nbits, 0);
        return d->makeInstruction(m68k_suba, "suba."+sizeToLetter(nbits), src, dst);
    }
};

// SUBI.B #<data>, Dx
// SUBI.W #<data>, Dx
// SUBI.L #<data>, Dx
struct M68k_subi: M68k {
    M68k_subi(): M68k("subi", m68k_family,
                      OP(0) & BITS<8, 10>(4) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeImmediateExtension(nbits, 0);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_subi, "subi."+sizeToLetter(nbits), src, dst);
    }
};

// SUBQ.B #<data>, <ea>x
// SUBQ.W #<data>, <ea>x
// SUBQ.L #<data>, <ea>x
struct M68k_subq: M68k {
    M68k_subq(): M68k("subq", m68k_family,
                      OP(5) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) &
                      EAM(m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        unsigned n = extract<9, 11>(w0);
        SgAsmExpression *src = d->makeImmediateValue(nbits, n?n:8);
        SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_subq, "subq."+sizeToLetter(nbits), src, dst);
    }
};

// SUBX.B Dx, Dy
// SUBX.W Dx, Dy
// SUBX.L Dx, Dy
// SUBX.B -(Ax), -(Ay)
// SUBX.W -(Ax), -(Ay)
// SUBX.L -(Ax), -(Ay)
struct M68k_subx: M68k {
    M68k_subx(): M68k("subx", m68k_family,
                      OP(9) & BIT<8>(1) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & BITS<4, 5>(0)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // SUBX.? -(Ax), -(Ay)
            src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), nbits);
            dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), nbits);
        } else {
            // SUBX.? Dx, Dy
            src = d->makeDataRegister(extract<0, 2>(w0), nbits, 0);
            dst = d->makeDataRegister(extract<9, 11>(w0), nbits, 0);
        }
        return d->makeInstruction(m68k_subx, "subx."+sizeToLetter(nbits), src, dst);
    }
};

// SWAP.W Dx
struct M68k_swap: M68k {
    M68k_swap(): M68k("swap", m68k_family,
                      OP(4) & BITS<3, 11>(0x108)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeDataRegister(extract<0, 2>(w0), 32, 0);
        return d->makeInstruction(m68k_swap, "swap.w", src); // mnemonic is "w" because it swaps words of a 32-bit reg
    }
};

// TAS.B <ea>x
struct M68k_tas: M68k {
    M68k_tas(): M68k("tas", m68k_family,
                     OP(4) & BITS<6, 11>(0x2b) & EAM(m68k_eam_data & m68k_eam_alter & ~m68k_eam_pcmi)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), 8, 0);
        return d->makeInstruction(m68k_tas, "tas.b", src);
    }
};

// TRAP #<vector>
struct M68k_trap: M68k {
    M68k_trap(): M68k("trap", m68k_family,
                      OP(4) & BITS<4, 11>(0x0e4)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *vector = d->makeImmediateValue(8, extract<0, 3>(w0));
        return d->makeInstruction(m68k_trap, "trap", vector);
    }
};

// TRAPcc
// TRAPcc.W #<data>
// TRAPcc.L #<data>
//      Mnemonic Condition              Value (table 3.19)
//      t        true                   0000    0
//      f        false                  0001    1
//      hi       hi                     0010    2
//      ls       lo or same             0011    3
//      cc(hi)   carry clear            0100    4
//      cs(lo)   carry set              0101    5
//      ne       not equal              0110    6
//      eq       equal                  0111    7
//      vc       overflow clear         1000    8
//      vs       overflow set           1001    9
//      pl       plus                   1010    a
//      mi       minus                  1011    b
//      ge       greater or equal       1100    c
//      lt       less than              1101    d
//      gt       greater than           1110    e
//      le       less or equal          1111    f
struct M68k_trapcc: M68k {
    M68k_trapcc(): M68k("trapcc", m68k_68020|m68k_68030|m68k_68040|m68k_cpu32,
                        OP(5) & BITS<3, 7>(0x1f) & (BITS<0, 2>(2) | BITS<0, 2>(3) | BITS<0, 2>(4))) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<0, 2>(w0)) {
            case 2: nbits = 16; break;
            case 3: nbits = 32; break;
            case 4: nbits = 0; break;                   // no operand
        }
        M68kInstructionKind kind = m68k_unknown_instruction;
        switch (extract<8, 11>(w0)) {
            case 0x0: kind = m68k_trapt;  break;
            case 0x1: kind = m68k_trapf;  break;
            case 0x2: kind = m68k_traphi; break;
            case 0x3: kind = m68k_trapls; break;
            case 0x4: kind = m68k_trapcc; break;
            case 0x5: kind = m68k_trapcs; break;
            case 0x6: kind = m68k_trapne; break;
            case 0x7: kind = m68k_trapeq; break;
            case 0x8: kind = m68k_trapvc; break;
            case 0x9: kind = m68k_trapvs; break;
            case 0xa: kind = m68k_trappl; break;
            case 0xb: kind = m68k_trapmi; break;
            case 0xc: kind = m68k_trapge; break;
            case 0xd: kind = m68k_traplt; break;
            case 0xe: kind = m68k_trapgt; break;
            case 0xf: kind = m68k_traple; break;
        }
        if (0==nbits) {
            std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_");
            return d->makeInstruction(kind, mnemonic);
        } else {
            SgAsmExpression *data = d->makeImmediateExtension(nbits, 0);
            std::string mnemonic = stringifyM68kInstructionKind(kind, "m68k_") + "." + sizeToLetter(nbits);
            return d->makeInstruction(kind, mnemonic, data);
        }
    }
};
        
// TRAPV
struct M68k_trapv: M68k {
    M68k_trapv(): M68k("trapv", m68k_family,
                       OP(4) & BITS<0, 11>(0xe76)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        return d->makeInstruction(m68k_trapv, "trapv");
    }
};

// TST.B <ea>y
// TST.W <ea>y
// TST.L <ea>y
struct M68k_tst: M68k {
    M68k_tst(): M68k("tst", m68k_family,
                     OP(4) & BITS<8, 11>(0xa) & (BITS<6, 7>(0) | BITS<6, 7>(1) | BITS<6, 7>(2)) & EAM(m68k_eam_all)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        size_t nbits = 0;
        switch (extract<6, 7>(w0)) {
            case 0: nbits = 8; break;
            case 1: nbits = 16; break;
            case 2: nbits = 32; break;
        }
        SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
        return d->makeInstruction(m68k_tst, "tst."+sizeToLetter(nbits), src);
    }
};

// UNLK Ax
struct M68k_unlk: M68k {
    M68k_unlk(): M68k("unlk", m68k_family,
                      OP(4) & BITS<3, 11>(0x1cb)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *dst = d->makeAddressRegister(extract<0, 2>(w0), 32, 0);
        return d->makeInstruction(m68k_unlk, "unlk", dst);
    }
};

// UNPK Dx, Dy, #<adjustment>
// UNPK -(Ax), -(Ay), #<adjustment>
struct M68k_unpk: M68k {
    M68k_unpk(): M68k("unpk", m68k_68020|m68k_68030|m68k_68040,
                      OP(8) & BITS<4, 8>(0x18)) {}
    SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
        SgAsmExpression *src=NULL, *dst=NULL;
        if (extract<3, 3>(w0)) {
            // UNPK -(Ax), -(Ay), #<adjustment>
            src = d->makeAddressRegisterPreDecrement(extract<0, 2>(w0), 16);
            dst = d->makeAddressRegisterPreDecrement(extract<9, 11>(w0), 16);
        } else {
            // UNPK Dx, Dy, #<adjustment>
            src = d->makeDataRegister(extract<0, 2>(w0), 16, 0);
            dst = d->makeDataRegister(extract<0, 2>(w0), 16, 0);
        }
        SgAsmExpression *adj = d->makeImmediateExtension(16, 0);
        return d->makeInstruction(m68k_unpk, "unpk", src, dst, adj);
    }
};



    
//                              Instructions from MFC5484 that we don't use anymore
//  
//  
//  
//  // BITREV.L Dx
//  struct M68k_bitrev: M68k {
//      M68k_bitrev(): M68k("bitrev", OP(0) & BITS<3, 11>(0x18)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_bitrev, "bitrev.l", d->makeDataRegister(extract<0, 2>(w0), 32));
//      }
//  };
//  
//  // BYTEREV.L Dx
//  struct M68k_byterev: M68k {
//      M68k_byterev(): M68k("byterev", OP(0) & BITS<3, 11>(0x58)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_byterev, "byterev.l", d->makeDataRegister(extract<0, 2>(w0), 32));
//      }
//  };
//  
//  // FF1.L Dx
//  struct M68k_ff1: M68k {
//      M68k_ff1(): M68k("ff1", OP(0) & BITS<3, 11>(0x98)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_ff1, "ff1.l", d->makeDataRegister(extract<0, 2>(w0), 32));
//      }
//  };
//  
//  
//  
//  // MAC.W Ry.{U,L}, Rx.{U,L} SF
//  // bits of w1 are unspecified: 0-5, 12-15
//  struct M68k_mac_w: M68k {
//      M68k_mac_w(): M68k("mac_w", OP(10) & BITS<7, 8>(0) & BITS<4, 5>(0) &
//                         WORD<1>(BIT<11>(0) & BIT<8>(0))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          bool rx_is_areg = extract<6, 6>(w0) == 1; // is Rx an address register vs. data register?
//          size_t rx_offset = extract<7, 7>(d->instruction_word(1)) ? 16 : 0;
//          size_t ry_offset = extract<6, 6>(d->instruction_word(1)) ? 16 : 0;
//          unsigned rx_regnum = extract<9, 11>(w0);
//          SgAsmExpression *rx = rx_is_areg ?
//                                d->makeAddressRegister(rx_regnum, 16, rx_offset) :
//                                d->makeDataRegister(rx_regnum, 16, rx_offset);
//          SgAsmExpression *ry = d->makeDataAddressRegister(extract<0, 3>(w0), 16, ry_offset);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<9, 10>(d->instruction_word(1)));
//          return d->makeInstruction(m68k_mac, "mac.w", ry, rx, sf);
//      }
//  };
//  
//  // MAC.W Ry.{U,L}, Rx.{U,L} SF, <ea>y&, Rw
//  struct M68k_mac_w2: M68k {
//      M68k_mac_w2(): M68k("mac_w2", OP(10) & BITS<7, 8>(1) & EAM(m68k_eam_ari|m68k_eam_inc|m68k_eam_dec|m68k_eam_dsp) &
//                          WORD<1>(BIT<11>(0) & BIT<8>(0) & BIT<4>(0))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          size_t rx_offset = extract<7, 7>(d->instruction_word(1)) ? 16 : 0;
//          size_t ry_offset = extract<6, 6>(d->instruction_word(1)) ? 16 : 0;
//          SgAsmExpression *rx = d->makeDataAddressRegister(extract<12, 15>(d->instruction_word(1)), 16, rx_offset);
//          SgAsmExpression *ry = d->makeDataAddressRegister(extract<0, 3>(d->instruction_word(1)), 16, ry_offset);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<9, 10>(d->instruction_word(1)));
//          SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
//          SgAsmExpression *use_mask = d->makeImmediateValue(1, extract<5, 5>(d->instruction_word(1)));
//          SgAsmExpression *rw = d->makeDataAddressRegister(extract<0, 3>(d->instruction_word(1)), 32);
//          return d->makeInstruction(m68k_mac, "mac.w", ry, rx, sf, eay, use_mask, rw);
//      }
//  };
//  
//  // MAC.L Ry, Rx, SF
//  // bits of w1 are unspecified: 0-7, 12-15
//  // bits w1.6 and w1.7 are only used for the upper/lower register-part indication and apply only to MAC.W
//  struct M68k_mac_l: M68k {
//      M68k_mac_l(): M68k("mac_l", OP(10) & BITS<7, 8>(0) & BITS<4, 5>(0) &
//                         WORD<1>(BIT<11>(1) & BIT<8>(0))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          bool rx_is_areg = extract<6, 6>(w0) == 1; // is Rx an address register vs. data register?
//          bool ry_is_areg = extract<3, 3>(w0) == 1; // is Ry an address register vs. data register?
//          unsigned rx_regnum = extract<9, 11>(w0);
//          unsigned ry_regnum = extract<0, 2>(w0);
//          SgAsmExpression *rx = rx_is_areg ?
//                                d->makeAddressRegister(rx_regnum, 32) :
//                                d->makeDataRegister(rx_regnum, 32);
//          SgAsmExpression *ry = ry_is_areg ?
//                                d->makeAddressRegister(ry_regnum, 32) :
//                                d->makeDataRegister(ry_regnum, 32);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<9, 10>(d->instruction_word(1)));
//          return d->makeInstruction(m68k_mac, "mac.l", ry, rx, sf);
//      }
//  };
//  
//  // MAC.L Ry, Rx, SF, <ea>y&, Rw
//  // The U/Lx (bit w1.7) and U/Ly (bit w1.6) are not used for this 32-bit instruction
//  struct M68k_mac_l2: M68k {
//      M68k_mac_l2(): M68k("mac_l2", OP(10) & BITS<7, 8>(1) & EAM(m68k_eam_ari|m68k_eam_inc|m68k_eam_dec|m68k_eam_dsp) &
//                          WORD<1>(BIT<11>(1) & BIT<8>(0) & BIT<4>(0))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          SgAsmExpression *rx = d->makeDataAddressRegister(extract<12, 15>(d->instruction_word(1)), 32, 0);
//          SgAsmExpression *ry = d->makeDataAddressRegister(extract<0, 3>(d->instruction_word(1)), 32, 0);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<9, 10>(d->instruction_word(1)));
//          SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
//          SgAsmExpression *use_mask = d->makeImmediateValue(1, extract<5, 5>(d->instruction_word(1)));
//          SgAsmExpression *rw = d->makeDataAddressRegister(extract<0, 3>(d->instruction_word(1)), 32);
//          return d->makeInstruction(m68k_mac, "mac.l", ry, rx, sf, eay, use_mask, rw);
//      }
//  };
//  
//  // MOV3Q.L #<data>, <ea>x
//  //
//  // Note: The reference manual addressing mode table lists address register direct as a valid addressing mode but the
//  // text says "use only data addressing modes listed in the following table."  According to 2.2.13, the address register direct
//  // mode is not a data addressing mode.  I am assuming that the table is correct rather than the text is correct, since the
//  // table omits the non-alterable modes but the text includes them (and it doesn't make sense for the destination to be
//  // non-alterable). [Robb P. Matzke 2013-10-02]
//  struct M68k_mov3q: M68k {
//      M68k_mov3q(): M68k("mov3q", OP(10) & BITS<6, 8>(5) &
//                         EAM(m68k_eam_all & ~(m68k_eam_imm | m68k_eam_pc))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          int32_t val = signExtend<3, 32>(extract<9, 11>(w0));
//          if (0==val)
//              val = -1;
//          SgAsmExpression *dst = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0);
//          return d->makeInstruction(m68k_mov3q, "mov3q.l", d->makeImmediateValue(32, val), dst);
//      }
//  };
//  
//  
//  // MOVE.L ACC, Rx
//  struct M68k_move_from_acc: M68k {
//      M68k_move_from_acc(): M68k("move_from_acc", OP(10) & BITS<4, 11>(0x18)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_move, "move.l",
//                                    d->makeMacRegister(m68k_mac_acc),
//                                    d->makeDataAddressRegister(extract<0, 3>(w0), 32));
//      }
//  };
//  
//  // MOVE.L MACSR, Rx
//  struct M68k_move_from_macsr: M68k {
//      M68k_move_from_macsr(): M68k("move_from_macsr", OP(10) & BITS<4, 11>(0x98)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_move, "move.l",
//                                    d->makeRegister(RegisterDescriptor(m68k_regclass_mac, m68k_mac_macsr, 0, 32)),
//                                    d->makeDataAddressRegister(extract<0, 3>(w0), 32));
//      }
//  };
//  
//  // MOVE.L MASK, Rx
//  struct M68k_move_from_mask: M68k {
//      M68k_move_from_mask(): M68k("move_from_mask", OP(10) & BITS<4, 11>(0xd8)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_move, "move.l",
//                                    d->makeRegister(RegisterDescriptor(m68k_regclass_mac, m68k_mac_mask, 0, 32)),
//                                    d->makeDataAddressRegister(extract<0, 3>(w0), 32));
//      }
//  };
//  
//  // MOVE.L MACSR, CCR
//  struct M68k_move_macsr2ccr: M68k {
//      M68k_move_macsr2ccr(): M68k("move_macsr2ccr", OP(10) & BITS<0, 11>(0x9c0)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_move, "move.l",
//                                    d->makeRegister(RegisterDescriptor(m68k_regclass_mac, m68k_mac_macsr, 0, 32)),
//                                    d->makeConditionCodeRegister());
//      }
//  };
//  
//  // MOVE.L Ry, ACC
//  // MOVE.L #<data>, ACC
//  struct M68k_move_to_acc: M68k {
//      M68k_move_to_acc(): M68k("move_to_acc", OP(10) & BITS<6, 11>(0x04) & EAM(m68k_eam_direct|m68k_eam_imm)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_move, "move.l",
//                                    d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0),
//                                    d->makeMacRegister(m68k_mac_acc));
//      }
//  };
//  
//  // MOVE.L Ry, MACSR
//  // MOVE.L #<data>, MACSR
//  struct M68k_move_to_macsr: M68k {
//      M68k_move_to_macsr(): M68k("move_to_macsr", OP(10) & BITS<6, 11>(0x24) & EAM(m68k_eam_direct|m68k_eam_imm)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_move, "move.l",
//                                    d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0),
//                                    d->makeRegister(RegisterDescriptor(m68k_regclass_mac, m68k_mac_macsr, 0, 32)));
//      }
//  };
//  
//  // MOVE.L Ry, MASK
//  // MOVE.L #<data>, MASK
//  struct M68k_move_to_mask: M68k {
//      M68k_move_to_mask(): M68k("move_to_mask", OP(10) & BITS<6, 11>(0x34) & EAM(m68k_eam_direct|m68k_eam_imm)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_move, "move.l",
//                                    d->makeEffectiveAddress(extract<0, 5>(w0), 32, 0),
//                                    d->makeRegister(RegisterDescriptor(m68k_regclass_mac, m68k_mac_mask, 0, 32)));
//      }
//  };
//  
//  // MSAC.W Ry.{U,L}, Rx.{U,L}SF
//  // word 1 bits 0-5 and 12-15 are unused
//  struct M68k_msac_w: M68k {
//      M68k_msac_w(): M68k("msac_w", OP(10) & BITS<7, 8>(0) & BITS<4, 5>(0) &
//                          WORD<1>(BIT<11>(0) & BIT<8>(1))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          size_t ry_offset = extract<6, 6>(d->instruction_word(1)) ? 16 : 0;
//          size_t rx_offset = extract<7, 7>(d->instruction_word(1)) ? 16 : 0;
//          SgAsmExpression *ry = d->makeDataAddressRegister(extract<0, 3>(w0), 16, ry_offset);
//          SgAsmExpression *rx = extract<6, 6>(w0) ?
//                                d->makeAddressRegister(extract<9, 11>(w0), 16, rx_offset) :
//                                d->makeDataRegister(extract<9, 11>(w0), 16, rx_offset);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<8, 10>(d->instruction_word(1)));
//          return d->makeInstruction(m68k_msac, "msac.w", ry, rx, sf);
//      }
//  };
//  
//  // MSAC.W Ry.{U,L}, Rx.{U,L} SF, <ea>y&, Rw
//  struct M68k_msac_w2: M68k {
//      M68k_msac_w2(): M68k("msac_w2", OP(10) & BITS<7, 8>(1) & EAM(m68k_eam_ari|m68k_eam_inc|m68k_eam_dec|m68k_eam_dsp) &
//                           WORD<1>(BIT<11>(0) & BIT<8>(1) & BIT<4>(0))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          size_t ry_offset = extract<6, 6>(d->instruction_word(1)) ? 16 : 0;
//          size_t rx_offset = extract<7, 7>(d->instruction_word(1)) ? 16 : 0;
//          SgAsmExpression *ry = d->makeDataAddressRegister(extract<0, 3>(d->instruction_word(1)), 16, ry_offset);
//          SgAsmExpression *rx = d->makeDataAddressRegister(extract<12, 15>(d->instruction_word(1)), 16, rx_offset);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<9, 10>(d->instruction_word(1)));
//          SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
//          SgAsmExpression *use_mask = d->makeImmediateValue(1, extract<5, 5>(d->instruction_word(1)));
//          SgAsmExpression *rw = extract<6, 6>(w0) ?
//                                d->makeAddressRegister(extract<9, 11>(w0), 32) :
//                                d->makeDataRegister(extract<9, 11>(w0), 32);
//          return d->makeInstruction(m68k_msac, "msac.w", ry, rx, sf, eay, use_mask, rw);
//      }
//  };
//  
//  // MSAC.L Ry, Rx SF
//  // word 1 bits 0-5 and 12-15 are unused
//  // word 1 bits 6-7 are unused because they only apply to 16-bit operands
//  struct M68k_msac_l: M68k {
//      M68k_msac_l(): M68k("msac_l", OP(10) & BITS<7, 8>(0) & BITS<4, 5>(0) &
//                          WORD<1>(BIT<11>(1) & BIT<8>(1))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          SgAsmExpression *ry = d->makeDataAddressRegister(extract<0, 3>(w0), 32);
//          SgAsmExpression *rx = extract<6, 6>(w0) ?
//                                d->makeAddressRegister(extract<9, 11>(w0), 32) :
//                                d->makeDataRegister(extract<9, 11>(w0), 32);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<8, 10>(d->instruction_word(1)));
//          return d->makeInstruction(m68k_msac, "msac.l", ry, rx, sf);
//      }
//  };
//  
//  // MSAC.W Ry.{U,L}, Rx.{U,L} SF, <ea>y&, Rw
//  // word 1 bits 6 and 7 are unused because they only apply to 16-bit operands
//  struct M68k_msac_l2: M68k {
//      M68k_msac_l2(): M68k("msac_l2", OP(10) & BITS<7, 8>(1) & EAM(m68k_eam_ari|m68k_eam_inc|m68k_eam_dec|m68k_eam_dsp) &
//                           WORD<1>(BIT<11>(1) & BIT<8>(1) & BIT<4>(0))) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          SgAsmExpression *ry = d->makeDataAddressRegister(extract<0, 3>(d->instruction_word(1)), 32);
//          SgAsmExpression *rx = d->makeDataAddressRegister(extract<12, 15>(d->instruction_word(1)), 32);
//          SgAsmExpression *sf = d->makeImmediateValue(8, extract<9, 10>(d->instruction_word(1)));
//          SgAsmExpression *eay = d->makeEffectiveAddress(extract<0, 5>(w0), 32, 1);
//          SgAsmExpression *use_mask = d->makeImmediateValue(1, extract<5, 5>(d->instruction_word(1)));
//          SgAsmExpression *rw = extract<6, 6>(w0) ?
//                                d->makeAddressRegister(extract<9, 11>(w0), 32) :
//                                d->makeDataRegister(extract<9, 11>(w0), 32);
//          return d->makeInstruction(m68k_msac, "msac.l", ry, rx, sf, eay, use_mask, rw);
//      }
//  };
//  
//  // MVS.B <ea>y, Dx
//  // MVS.W <ea>y, Dx
//  //
//  // Note: Reference manual table shows all effective addressing modes are possible, but the text near the table says "use only
//  // data addressing modes from the following table", which according to the definition in section 2.2.13 excludes the address
//  // register direct mode. [Robb P. Matzke 2013-10-02]
//  struct M68k_mvs: M68k {
//      M68k_mvs(): M68k("mvs", OP(7) & BITS<7, 8>(2) & EAM(m68k_eam_data)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          size_t nbits = extract<6, 6>(w0) ? 16 : 8;
//          SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
//          SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
//          return d->makeInstruction(m68k_mvs, "mvs."+sizeToLetter(nbits), src, dst);
//      }
//  };
//  
//  // MVZ.B <ea>y, Dx
//  // MVZ.W <ea>y, Dx
//  //
//  // Note: Reference manual table shows all effective addressing modes are possible, but the text above the table says "use
//  // the following data addressing modes", which according to the definition in section 2.2.13 excludes the address register
//  // direct mode. [Robb P. Matzke 2013-10-02]
//  struct M68k_mvz: M68k {
//      M68k_mvz(): M68k("mvz", OP(7) & BITS<7, 8>(3) & EAM(m68k_eam_data)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          size_t nbits = extract<6, 6>(w0) ? 16 : 8;
//          SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
//          SgAsmExpression *dst = d->makeDataRegister(extract<9, 11>(w0), 32);
//          return d->makeInstruction(m68k_mvz, "mvz."+sizeToLetter(nbits), src, dst);
//      }
//  };
//  
//  // PULSE
//  struct M68k_pulse: M68k {
//      M68k_pulse(): M68k("pulse", OP(4) & BITS<0, 11>(0xacc)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          return d->makeInstruction(m68k_pulse, "pulse");
//      }
//  };
//  
//  // SATS.L Dx
//  struct M68k_sats: M68k {
//      M68k_sats(): M68k("sats", OP(4) & BITS<3, 11>(0x190)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          SgAsmExpression *dx = d->makeDataRegister(extract<0, 2>(w0), 32);
//          return d->makeInstruction(m68k_sats, "sats.l", dx);
//      }
//  };
//  
//                  
//  // TPF
//  // TPF.W #<data>
//  // TPF.L #<data>
//  struct M68k_tpf: M68k {
//      M68k_tpf(): M68k("tpf", OP(5) & BITS<3, 11>(0x3f)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          SgAsmExpression *src = NULL;
//          unsigned opmode = extract<0, 2>(w0);
//          switch (opmode) {
//              case 2:
//                  src = d->makeImmediateExtension(16, 0);
//                  return d->makeInstruction(m68k_tpf, "tpf.w", src);
//              case 3:
//                  src = d->makeImmediateExtension(32, 0);
//                  return d->makeInstruction(m68k_tpf, "tpf.l", src);
//              case 4:
//                  return d->makeInstruction(m68k_tpf, "tpf");
//              default:
//                  throw Disassembler::Exception("invalid opmode "+StringUtility::numberToString(opmode)+" for TPF instruction");
//          }
//      }
//  };
//  
//  // WDDATA.B <ea>y
//  // WDDATA.W <ea>y
//  // WDDATA.L <ea>y
//  //
//  // Note: The reference manual has a table which indicates that word and long absolute addressing modes are permitted, but the
//  // text states "use only those memory alterable addressing modes listed in the following table". According to section 2.2.13,
//  // the absolute addressing modes are not in the set of alterable addressing modes. [Robb P. Matzke 2013-10-02]
//  struct M68k_wddata: M68k {
//      M68k_wddata(): M68k("wddata", OP(15) & BITS<8, 11>(0xb) & EAM(m68k_eam_memory & m68k_eam_alter)) {}
//      SgAsmM68kInstruction *operator()(D *d, unsigned w0) {
//          size_t nbits = 0;
//          switch (extract<6, 7>(w0)) {
//              case 0: nbits = 8; break;
//              case 1: nbits = 16; break;
//              case 2: nbits = 32; break;
//              case 3: throw Disassembler::Exception("invalid size for WDDATA instruction");
//          }
//          SgAsmExpression *src = d->makeEffectiveAddress(extract<0, 5>(w0), nbits, 0);
//          return d->makeInstruction(m68k_wddata, "wddata."+sizeToLetter(nbits), src);
//      }
//  };

        
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
DisassemblerM68k::init()
{
    set_registers(RegisterDictionary::dictionary_m68000()); // only a default
    set_wordsize(2);
    set_alignment(2);
    set_sex(ByteOrder::ORDER_MSB);

#define M68k_DECODER(NAME)                                                                                                     \
    static M68k_##NAME *v_##NAME=NULL;                                                                                         \
    if (NULL==v_##NAME)                                                                                                        \
        v_##NAME = new M68k_##NAME;                                                                                            \
    insert_idis(v_##NAME);

    // Integer instructions from pages 4-1 through 4-198
    M68k_DECODER(abcd_1);
    M68k_DECODER(abcd_2);
    M68k_DECODER(add_1);
    M68k_DECODER(add_2);
    M68k_DECODER(add_3);
    M68k_DECODER(adda);
    M68k_DECODER(addi);
    M68k_DECODER(addq);
    M68k_DECODER(addx_1);
    M68k_DECODER(addx_2);
    M68k_DECODER(and_1);
    M68k_DECODER(and_2);
    M68k_DECODER(andi);
    M68k_DECODER(andi_to_ccr);
    M68k_DECODER(ashift_1);
    M68k_DECODER(ashift_2);
    M68k_DECODER(ashift_3);
    M68k_DECODER(branch);
    M68k_DECODER(bchg_1);
    M68k_DECODER(bchg_2);
    M68k_DECODER(bchg_3);
    M68k_DECODER(bchg_4);
    M68k_DECODER(bclr_1);
    M68k_DECODER(bclr_2);
    M68k_DECODER(bclr_3);
    M68k_DECODER(bclr_4);
    M68k_DECODER(bfchg);
    M68k_DECODER(bfclr);
    M68k_DECODER(bfexts);
    M68k_DECODER(bfextu);
    M68k_DECODER(bfins);
    M68k_DECODER(bfset);
    M68k_DECODER(bftst);
    M68k_DECODER(bkpt);
    M68k_DECODER(bset_1);
    M68k_DECODER(bset_2);
    M68k_DECODER(bset_3);
    M68k_DECODER(bset_4);
    M68k_DECODER(btst_1);
    M68k_DECODER(btst_2);
    M68k_DECODER(btst_3);
    M68k_DECODER(btst_4);
    M68k_DECODER(callm);
    M68k_DECODER(cas);
    M68k_DECODER(cas2);
    M68k_DECODER(chk);
    M68k_DECODER(chk2);
    M68k_DECODER(clr);
    M68k_DECODER(cmp);
    M68k_DECODER(cmpa);
    M68k_DECODER(cmpi);
    M68k_DECODER(cmpm);
    M68k_DECODER(cmp2);
    M68k_DECODER(cpbcc);
    M68k_DECODER(cpdbcc);
    M68k_DECODER(cpscc);
    M68k_DECODER(cptrapcc);
    M68k_DECODER(dbcc);
    M68k_DECODER(divs_w);
    M68k_DECODER(divrem_l);
    M68k_DECODER(divu_w);
    M68k_DECODER(eor);
    M68k_DECODER(eori);
    M68k_DECODER(eori_to_ccr);
    M68k_DECODER(exg);
    M68k_DECODER(ext_w);
    M68k_DECODER(ext_l);
    M68k_DECODER(extb_l);
    M68k_DECODER(illegal);
    M68k_DECODER(jmp);
    M68k_DECODER(jsr);
    M68k_DECODER(lea);
    M68k_DECODER(link_w);
    M68k_DECODER(link_l);
    M68k_DECODER(lshift_rr);
    M68k_DECODER(lshift_ir);
    M68k_DECODER(lshift_mem);
    M68k_DECODER(move);
    M68k_DECODER(movea);
    M68k_DECODER(move_from_ccr);
    M68k_DECODER(move_to_ccr);
    M68k_DECODER(move_from_sr);
    M68k_DECODER(move16_pp);
    M68k_DECODER(move16_a);
    M68k_DECODER(movem_rm);
    M68k_DECODER(movem_mr);
    M68k_DECODER(movep);
    M68k_DECODER(moveq);
    M68k_DECODER(muls_w);
    M68k_DECODER(multiply_l);
    M68k_DECODER(multiply_64);
    M68k_DECODER(mulu_w);
    M68k_DECODER(nbcd);
    M68k_DECODER(neg);
    M68k_DECODER(negx);
    M68k_DECODER(nop);
    M68k_DECODER(not);
    M68k_DECODER(or_1);
    M68k_DECODER(or_2);
    M68k_DECODER(ori);
    M68k_DECODER(ori_to_ccr);
    M68k_DECODER(pack);
    M68k_DECODER(pea);
    M68k_DECODER(rotate_reg);
    M68k_DECODER(rotate_mem);
    M68k_DECODER(rotate_extend_reg);
    M68k_DECODER(rotate_extend_mem);
    M68k_DECODER(rtd);
    M68k_DECODER(rtm);
    M68k_DECODER(rtr);
    M68k_DECODER(rts);
    M68k_DECODER(sbcd);
    M68k_DECODER(scc);
    M68k_DECODER(sub_1);
    M68k_DECODER(sub_2);
    M68k_DECODER(suba);
    M68k_DECODER(subi);
    M68k_DECODER(subq);
    M68k_DECODER(subx);
    M68k_DECODER(swap);
    M68k_DECODER(tas);
    M68k_DECODER(trap);
    M68k_DECODER(trapcc);
    M68k_DECODER(trapv);
    M68k_DECODER(tst);
    M68k_DECODER(unlk);
    M68k_DECODER(unpk);
}
