// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "Disassembler.h"
#include "sageBuilderAsm.h"
#include "DisassemblerArm.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* See header file for full documentation. */

/* Returns true if the instruction modifies the instruction pointer (r15). */
static bool modifies_ip(SgAsmArmInstruction *insn) 
{
    switch (insn->get_kind()) {

        /* Branch instructions */
        case arm_b:
        case arm_bl:
        case arm_blx:
        case arm_bx:
        case arm_bxj:
            return true;

        /* Comparison instructions */
        case arm_cmn:
        case arm_cmp:
        case arm_teq:
        case arm_tst:
            return false;

        /* Load multiple registers instructions. Second argument is the set of registers to load.  If the instruction
         * pointer (r15) can be one of them. */
        case arm_ldm:
        case arm_ldmda:
        case arm_ldmdb:
        case arm_ldmia:
        case arm_ldmib: {
            const std::vector<SgAsmExpression*> &exprs = insn->get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()>=2);
            SgAsmExprListExp *elist = isSgAsmExprListExp(exprs[1]);
            if (!elist) {
                SgAsmUnaryArmSpecialRegisterList *rlist = isSgAsmUnaryArmSpecialRegisterList(exprs[1]);
                ROSE_ASSERT(rlist);
                elist = isSgAsmExprListExp(rlist->get_operand());
                ROSE_ASSERT(elist);
            }
            for (size_t i=0; i<elist->get_expressions().size(); i++) {
                SgAsmArmRegisterReferenceExpression *reg = isSgAsmArmRegisterReferenceExpression(elist->get_expressions()[i]);
                ROSE_ASSERT(reg);
                if (reg->get_descriptor().get_major()==arm_regclass_gpr && reg->get_descriptor().get_minor()==15) {
                    return true;
                }
            }
            return false;
        }

        /* Interrupt-causing instructions */
        case arm_bkpt:
        case arm_swi:
        case arm_undefined:
            return true;

        /* Other instructions modify the instruction pointer if it's the first (destination) argument. */
        default: {
            const std::vector<SgAsmExpression*> &exprs = insn->get_operandList()->get_operands();
            if (exprs.size()>=1) {
                SgAsmArmRegisterReferenceExpression *rre = isSgAsmArmRegisterReferenceExpression(exprs[0]);
                if (rre &&
                    rre->get_descriptor().get_major()==arm_regclass_gpr && rre->get_descriptor().get_minor()==15) {
                    return true;
                }
            }
        }
    }
    return false;
}

Disassembler::AddressSet
SgAsmArmInstruction::get_successors(bool *complete) {
    Disassembler::AddressSet retval;
    const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
    *complete = true; /*assume retval is the complete set of successors for now*/

    switch (get_kind()) {
        case arm_b:
        case arm_bl:
        case arm_blx:
        case arm_bx: {
            /* Branch target */
            ROSE_ASSERT(exprs.size()==1);
            SgAsmExpression *dest = exprs[0];
            if (isSgAsmValueExpression(dest)) {
                rose_addr_t target_va = SageInterface::getAsmConstant(isSgAsmValueExpression(dest));
                retval.insert(target_va);
            } else {
                /* Could also be a register reference expression, but we don't know the successor in that case. */
                *complete = false;
            }
            
            /* Fall-through address */
            if (get_condition()!=arm_cond_al)
                retval.insert(get_address()+4);
            break;
        }

        case arm_bxj: {
            /* First argument is the register that holds the next instruction pointer value to use in the case that Jazelle is
             * not available. We only know the successor if the register is the instruction pointer, in which case the
             * successor is the fall-through address. */
            ROSE_ASSERT(exprs.size()==1);
            SgAsmArmRegisterReferenceExpression *rre = isSgAsmArmRegisterReferenceExpression(exprs[0]);
            ROSE_ASSERT(rre);
            if (rre->get_descriptor().get_major()==arm_regclass_gpr && rre->get_descriptor().get_minor()==15) {
                retval.insert(get_address()+4);
            } else {
                *complete = false;
            }
            break;
        }
            
        case arm_cmn:
        case arm_cmp:
        case arm_teq:
        case arm_tst:
            /* Comparison and test instructions don't ever affect the instruction pointer; they only fall through */
            retval.insert(get_address()+4);
            break;

        case arm_bkpt:
        case arm_swi:
        case arm_undefined:
        case arm_unknown_instruction:
            /* No known successors for interrupt-generating instructions */
            break;

        default:
            if (!modifies_ip(this) || get_condition()!=arm_cond_al) {
                retval.insert(get_address()+4);
            } else {
                *complete = false;
            }
            break;
    }
    return retval;
}

bool
SgAsmArmInstruction::terminatesBasicBlock() {
    if (get_kind()==arm_unknown_instruction)
        return true;
    return modifies_ip(this);
}

bool
DisassemblerArm::can_disassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_ARM_Family;
}

void
DisassemblerArm::init()
{
    decodeUnconditionalInstructions = true;
    set_wordsize(4);
    set_alignment(4);
    set_sex(SgAsmExecutableFileFormat::ORDER_LSB);
    set_registers(RegisterDictionary::arm7());
}

/* This is a bit of a kludge for now because we're trying to use an unmodified version of the ArmDisassembler name space. */
SgAsmInstruction *
DisassemblerArm::disassembleOne(const MemoryMap *map, rose_addr_t start_va, AddressSet *successors)
{
    if (start_va & 0x3)
        throw Exception("instruction pointer not word aligned", start_va);
    if (start_va >= 0xffffffff)
        throw Exception("instruction pointer out of range", start_va);

    /* The old ArmDisassembler::disassemble() function doesn't understand MemoryMap mappings. Therefore, remap the next
     * few bytes (enough for at least one instruction) into a temporary buffer. */
    unsigned char temp[4]; /* all ARM instructions are 32 bits */
    size_t tempsz = map->read(temp, start_va, sizeof temp, get_protection());

    /* Treat the bytes as a little-endian instruction. FIXME: This assumes a little-endian ARM system. */
    if (tempsz<4)
        throw Exception("short read", start_va);
    uint32_t c = temp[0] | (temp[1]<<8) | (temp[2]<<16) | (temp[3]<<24);

    /* Disassemble the instruction */
    startInstruction(start_va, c);
    SgAsmArmInstruction *insn = disassemble(); /*throws an exception on error*/
    ROSE_ASSERT(insn);
    
    /* Note successors if necessary */
    if (successors) {
        bool complete;
        AddressSet suc2 = insn->get_successors(&complete);
        successors->insert(suc2.begin(), suc2.end());
    }

    update_progress(insn);
    return insn;
}

SgAsmInstruction *
DisassemblerArm::make_unknown_instruction(const Exception &e) 
{
    SgAsmArmInstruction *insn = new SgAsmArmInstruction(e.ip, "unknown", arm_unknown_instruction, arm_cond_unknown, 0);
    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_raw_bytes(e.bytes);
    return insn;
}



SgAsmArmInstruction *
DisassemblerArm::makeInstructionWithoutOperands(uint32_t address, const std::string& mnemonic, int condPos,
                                                ArmInstructionKind kind, ArmInstructionCondition cond, uint32_t insn)
{
    SgAsmArmInstruction* instruction = new SgAsmArmInstruction(address, mnemonic, kind, cond, condPos);
    ROSE_ASSERT (instruction);

    SgAsmOperandList* operands = new SgAsmOperandList();
    instruction->set_operandList(operands);
    operands->set_parent(instruction);

    SgUnsignedCharList bytes(4, '\0');
    for (int i = 0; i < 4; ++i) {
        bytes[i] = (insn >> (8 * i)) & 0xFF; /* Force little-endian */
    }   

    instruction->set_raw_bytes(bytes);

    return instruction;
}

/** Creates a general-purpose register reference expression. */
SgAsmArmRegisterReferenceExpression *
DisassemblerArm::makeRegister(uint8_t reg) const
{
    ROSE_ASSERT(get_registers()!=NULL);
    ROSE_ASSERT(reg<16);
    std::string name = "r" + StringUtility::numberToString(reg);
    const RegisterDescriptor *rdesc = get_registers()->lookup(name);
    ROSE_ASSERT(rdesc!=NULL);
    SgAsmArmRegisterReferenceExpression* r = new SgAsmArmRegisterReferenceExpression(*rdesc);
    return r;
}

/** Create a reference to one of the program status registers. If @p useSPSR is true then create a reference to the saved
 *  program status register, otherwise create a reference to the current program status register. The @p fields is a 4-bit mask
 *  indicating which individual fields of the register are selected.
 *
 *  The field bits are
 *     0x01 => c
 *     0x02 => x
 *     0x04 => s
 *     0x08 => f
 */
SgAsmArmRegisterReferenceExpression *
DisassemblerArm::makePsrFields(bool useSPSR, uint8_t fields) const
{
    ROSE_ASSERT(get_registers()!=NULL);
    std::string name = useSPSR ? "spsr" : "cpsr";
    const RegisterDescriptor *rdesc = get_registers()->lookup(name);
    ROSE_ASSERT(rdesc!=NULL);
    SgAsmArmRegisterReferenceExpression *r = new SgAsmArmRegisterReferenceExpression(*rdesc);
    if (fields!=0)
        r->set_psr_mask(fields);
    return r;
}

SgAsmArmRegisterReferenceExpression *
DisassemblerArm::makePsr(bool useSPSR) const
{
    return makePsrFields(useSPSR, 0);
}


SgAsmExpression *
DisassemblerArm::makeRotatedImmediate() const
{
    uint8_t rsField = (insn >> 8) & 15;
    uint8_t rotateCount = rsField * 2;
    uint32_t immRaw = insn & 0xFF;
    if (rotateCount == 0) {
        return SageBuilderAsm::makeDWordValue(immRaw);
    } else {
        return SageBuilderAsm::makeDWordValue((immRaw >> rotateCount) | (immRaw << (32 - rotateCount)));
    }
}

SgAsmExpression *
DisassemblerArm::makeShifterField() const
{
    bool i = (insn >> 25) & 1;
    uint8_t rsField = (insn >> 8) & 15;
    uint8_t rmField = insn & 15;
    uint8_t shiftCount = (insn >> 7) & 31;
    uint8_t shiftCountOr32 = shiftCount == 0 ? 32 : shiftCount;
    if (i) {
        return makeRotatedImmediate();
    } else if ((insn & 0xFF0) == 0) {
        return makeRegister(rmField);
    } else if ((insn & 0x070) == 0) {
        return SageBuilderAsm::makeLsl(makeRegister(rmField), SageBuilderAsm::makeByteValue(shiftCount));
    } else if ((insn & 0x0F0) == 0x010) {
        return SageBuilderAsm::makeLsl(makeRegister(rmField), makeRegister(rsField));
    } else if ((insn & 0x070) == 0x020) {
        return SageBuilderAsm::makeLsr(makeRegister(rmField), SageBuilderAsm::makeByteValue(shiftCountOr32));
    } else if ((insn & 0x0F0) == 0x030) {
        return SageBuilderAsm::makeLsr(makeRegister(rmField), makeRegister(rsField));
    } else if ((insn & 0x070) == 0x040) {
        return SageBuilderAsm::makeAsr(makeRegister(rmField), SageBuilderAsm::makeByteValue(shiftCountOr32));
    } else if ((insn & 0x0F0) == 0x050) {
        return SageBuilderAsm::makeAsr(makeRegister(rmField), makeRegister(rsField));
    } else if ((insn & 0xFF0) == 0x060) {
        return SageBuilderAsm::makeRrx(makeRegister(rmField));
    } else if ((insn & 0x070) == 0x060) {
        return SageBuilderAsm::makeRor(makeRegister(rmField), SageBuilderAsm::makeByteValue(shiftCount));
    } else if ((insn & 0x0F0) == 0x070) {
        return SageBuilderAsm::makeRor(makeRegister(rmField), makeRegister(rsField));
    } else {
        throw ExceptionArm("bad shifter field", this, 25);
    }
}

#define MAKE_INSN0(Mne, CondPos) (makeInstructionWithoutOperands(ip, #Mne, (CondPos), arm_##Mne, cond, insn))
#define MAKE_INSN1(Mne, CondPos, Op1) (SageBuilderAsm::appendOperand(MAKE_INSN0(Mne, CondPos), (Op1)))
#define MAKE_INSN2(Mne, CondPos, Op1, Op2) (SageBuilderAsm::appendOperand(MAKE_INSN1(Mne, CondPos, Op1), (Op2)))
#define MAKE_INSN3(Mne, CondPos, Op1, Op2, Op3) (SageBuilderAsm::appendOperand(MAKE_INSN2(Mne, CondPos, Op1, Op2), (Op3)))
#define MAKE_INSN4(Mne, CondPos, Op1, Op2, Op3, Op4) (SageBuilderAsm::appendOperand(MAKE_INSN3(Mne, CondPos, Op1, Op2, Op3), (Op4)))

SgAsmArmInstruction *
DisassemblerArm::makeDataProcInstruction(uint8_t opcode, bool s, SgAsmExpression* rn, SgAsmExpression* rd,
                                         SgAsmExpression* rhsOperand)
{
    switch ((s ? 16 : 0) | opcode) {
        case 0x00: return MAKE_INSN3(and, 3, rd, rn, rhsOperand);
        case 0x01: return MAKE_INSN3(eor, 3, rd, rn, rhsOperand);
        case 0x02: return MAKE_INSN3(sub, 3, rd, rn, rhsOperand);
        case 0x03: return MAKE_INSN3(rsb, 3, rd, rn, rhsOperand);
        case 0x04: return MAKE_INSN3(add, 3, rd, rn, rhsOperand);
        case 0x05: return MAKE_INSN3(adc, 3, rd, rn, rhsOperand);
        case 0x06: return MAKE_INSN3(sbc, 3, rd, rn, rhsOperand);
        case 0x07: return MAKE_INSN3(rsc, 3, rd, rn, rhsOperand);
        case 0x08: ROSE_ASSERT (!"Not a data processing insn");
        case 0x09: ROSE_ASSERT (!"Not a data processing insn");
        case 0x0A: ROSE_ASSERT (!"Not a data processing insn");
        case 0x0B: ROSE_ASSERT (!"Not a data processing insn");
        case 0x0C: return MAKE_INSN3(orr, 3, rd, rn, rhsOperand);
        case 0x0D: return MAKE_INSN2(mov, 3, rd, rhsOperand);
        case 0x0E: return MAKE_INSN3(bic, 3, rd, rn, rhsOperand);
        case 0x0F: return MAKE_INSN2(mvn, 3, rd, rhsOperand);
        case 0x10: return MAKE_INSN3(ands, 3, rd, rn, rhsOperand);
        case 0x11: return MAKE_INSN3(eors, 3, rd, rn, rhsOperand);
        case 0x12: return MAKE_INSN3(subs, 3, rd, rn, rhsOperand);
        case 0x13: return MAKE_INSN3(rsbs, 3, rd, rn, rhsOperand);
        case 0x14: return MAKE_INSN3(adds, 3, rd, rn, rhsOperand);
        case 0x15: return MAKE_INSN3(adcs, 3, rd, rn, rhsOperand);
        case 0x16: return MAKE_INSN3(sbcs, 3, rd, rn, rhsOperand);
        case 0x17: return MAKE_INSN3(rscs, 3, rd, rn, rhsOperand);
        case 0x18: return MAKE_INSN2(tst, 3, rn, rhsOperand);
        case 0x19: return MAKE_INSN2(teq, 3, rn, rhsOperand);
        case 0x1A: return MAKE_INSN2(cmp, 3, rn, rhsOperand);
        case 0x1B: return MAKE_INSN2(cmn, 3, rn, rhsOperand);
        case 0x1C: return MAKE_INSN3(orrs, 3, rd, rn, rhsOperand);
        case 0x1D: return MAKE_INSN2(movs, 3, rd, rhsOperand);
        case 0x1E: return MAKE_INSN3(bics, 3, rd, rn, rhsOperand);
        case 0x1F: return MAKE_INSN2(mvns, 3, rd, rhsOperand);
        default: ROSE_ASSERT (false);
    }
// DQ (11/29/2009): Avoid MSVC warning.
   return NULL;
}

SgAsmDoubleWordValueExpression *
DisassemblerArm::makeSplit8bitOffset() const
{
    int32_t val = ((insn >> 4) & 0xF0) | (insn & 0xF);
    val <<= 24;
    val >>= 24; // Arithmetic shift to copy highest bit of immediate
    return SageBuilderAsm::makeDWordValue((uint32_t)val);
}

SgAsmDoubleWordValueExpression *
DisassemblerArm::makeBranchTarget() const
{
    int32_t val = insn & 0xFFFFFF;
    val <<= 8;
    val >>= 6; // Arithmetic shift to copy highest bit of immediate
    uint32_t targetAddr = ip + 8 + val;
    return SageBuilderAsm::makeDWordValue(targetAddr);
}

SgAsmExpression *
DisassemblerArm::decodeMemoryAddress(SgAsmExpression* rn) const
{
    bool bit25 = (insn >> 25) & 1;
    bool p = (insn >> 24) & 1;
    bool u = (insn >> 23) & 1;
    bool w = (insn >> 21) & 1;
    SgAsmExpression* offset = bit25 ? makeShifterField() : SageBuilderAsm::makeDWordValue(insn & 0xFFFU);
    switch ((p ? 4 : 0) | (u ? 2 : 0) | (w ? 1 : 0)) {
      case 0: return SageBuilderAsm::makeSubtractPostupdate(rn, offset);
      case 1: return SageBuilderAsm::makeSubtractPostupdate(rn, offset); // T suffix
      case 2: return SageBuilderAsm::makeAddPostupdate(rn, offset);
      case 3: return SageBuilderAsm::makeAddPostupdate(rn, offset); // T suffix
      case 4: return SageBuilderAsm::makeSubtract(rn, offset);
      case 5: return SageBuilderAsm::makeSubtractPreupdate(rn, offset);
      case 6: return SageBuilderAsm::makeAdd(rn, offset);
      case 7: return SageBuilderAsm::makeAddPreupdate(rn, offset);
      default: {
        ROSE_ASSERT (false);
        // DQ (11/29/2009): Avoid MSVC warning.
        return NULL;
      }
    }
}

SgAsmArmInstruction *
DisassemblerArm::decodeMediaInstruction() const
{
    std::cerr << "ARM media instructions not supported: " << StringUtility::intToHex(insn) << std::endl;
    throw ExceptionArm("media instruction not supported", this);
}

SgAsmArmInstruction *
DisassemblerArm::decodeMultiplyInstruction() const
{
    SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
    SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
    SgAsmExpression* rs = makeRegister((insn >> 8) & 15);
    SgAsmExpression* rm = makeRegister(insn & 15);
    switch ((insn >> 20) & 15) {
        case 0x0: return MAKE_INSN3(mul, 3, rn, rm, rs);
        case 0x1: return MAKE_INSN3(muls, 3, rn, rm, rs);
        case 0x2: return MAKE_INSN4(mla, 3, rn, rm, rs, rd);
        case 0x3: return MAKE_INSN4(mlas, 3, rn, rm, rs, rd);
        case 0x4: return MAKE_INSN4(umaal, 3, rd, rn, rm, rs);
        case 0x5: throw ExceptionArm("bad bits in decodeMultiplyInstruction (0x5)", this, 20);
        case 0x6: throw ExceptionArm("bad bits in decodeMultiplyInstruction (0x6)", this, 20);
        case 0x7: throw ExceptionArm("bad bits in decodeMultiplyInstruction (0x7)", this, 20);
        case 0x8: return MAKE_INSN4(umull, 5, rd, rn, rm, rs);
        case 0x9: return MAKE_INSN4(umulls, 5, rd, rn, rm, rs);
        case 0xA: return MAKE_INSN4(umlal, 5, rd, rn, rm, rs);
        case 0xB: return MAKE_INSN4(umlals, 5, rd, rn, rm, rs);
        case 0xC: return MAKE_INSN4(smull, 5, rd, rn, rm, rs);
        case 0xD: return MAKE_INSN4(smulls, 5, rd, rn, rm, rs);
        case 0xE: return MAKE_INSN4(smlal, 5, rd, rn, rm, rs);
        case 0xF: return MAKE_INSN4(smlals, 5, rd, rn, rm, rs);
        default: ROSE_ASSERT (false);
    }
// DQ (11/29/2009): Avoid MSVC warning.
   return NULL;
}

SgAsmArmInstruction *
DisassemblerArm::decodeExtraLoadStores() const
{
    bool bit5 = (insn >> 5) & 1;
    bool bit6 = (insn >> 6) & 1;
    bool bit20 = (insn >> 20) & 1;
    bool bit21 = (insn >> 21) & 1;
    bool bit22 = (insn >> 22) & 1;
    bool bit23 = (insn >> 23) & 1;
    bool bit24 = (insn >> 24) & 1;
    SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
    SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
    SgAsmExpression* offset = bit22 ? (SgAsmExpression*)makeSplit8bitOffset() : makeRegister(insn & 15);
    SgAsmExpression* addr = NULL;
    switch ((bit24 ? 4 : 0) | (bit23 ? 2 : 0) | (bit21 ? 1 : 0)) {
        case 0: addr = SageBuilderAsm::makeSubtractPostupdate(rn, offset); break;
        case 1: throw ExceptionArm("bad bits in decodeExtraLoadStores (1)", this, 21);
        case 2: addr = SageBuilderAsm::makeAddPostupdate(rn, offset); break;
        case 3: throw ExceptionArm("bad bits in decodeExtraLoadStores (3)", this, 21);
        case 4: addr = SageBuilderAsm::makeSubtract(rn, offset); break;
        case 5: addr = SageBuilderAsm::makeSubtractPreupdate(rn, offset); break;
        case 6: addr = SageBuilderAsm::makeAdd(rn, offset); break;
        case 7: addr = SageBuilderAsm::makeAddPreupdate(rn, offset); break;
        default: ROSE_ASSERT (false);
    }
    SgAsmExpression* memref = SageBuilderAsm::makeMemoryReference(addr);
    uint8_t lsh = (bit20 ? 4 : 0) | (bit6 ? 2 : 0) | (bit5 ? 1 : 0);
    switch (lsh) {
        case 0:
            /* Should have been handled in multiply code above */
            throw ExceptionArm("bad bits in decodeExtraLoadStores (0)", this, 5);
        case 1: return MAKE_INSN2(strh, 3, rd, memref);
        case 2: return MAKE_INSN2(ldrd, 3, rd, memref);
        case 3: return MAKE_INSN2(strd, 3, rd, memref);
        case 4:
            /* Should have been handled in multiply code above */
            throw ExceptionArm("bad bits in decodeExtraLoadStores (4)", this, 5);
        case 5: return MAKE_INSN2(ldruh, 3, rd, memref);
        case 6: return MAKE_INSN2(ldrsb, 3, rd, memref);
        case 7: return MAKE_INSN2(ldrsh, 3, rd, memref);
        default: ROSE_ASSERT (false);
    }
// DQ (11/29/2009): Avoid MSVC warning.
   return NULL;
}

SgAsmArmInstruction *
DisassemblerArm::decodeMiscInstruction() const
{
    bool bit7 = (insn >> 7) & 1;
    bool bit21 = (insn >> 21) & 1;
    bool bit22 = (insn >> 22) & 1;
    if (!bit7) {
      switch ((insn >> 4) & 7) {
        case 0: {
          if (bit21) {
            SgAsmArmRegisterReferenceExpression* rm = makeRegister(insn & 15);
            bool useSPSR = bit22;
            uint8_t mask = (insn >> 16) & 15;
            SgAsmArmRegisterReferenceExpression* psr = makePsrFields(useSPSR, mask);
            return MAKE_INSN2(msr, 3, psr, rm);
          } else {
            bool useSPSR = bit22;
            SgAsmArmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
            SgAsmArmRegisterReferenceExpression* psr = makePsr(useSPSR);
            return MAKE_INSN2(mrs, 3, rd, psr);
          }
        }
        case 1: {
          if (bit22) {
            SgAsmArmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
            SgAsmArmRegisterReferenceExpression* rm = makeRegister(insn & 15);
            return MAKE_INSN2(clz, 3, rd, rm);
          } else {
            return MAKE_INSN1(bx, 2, makeRegister(insn & 15));
          }
        }
        case 2: return MAKE_INSN1(bxj, 3, makeRegister(insn & 15));
        case 3: return MAKE_INSN1(blx, 3, makeRegister(insn & 15));
        case 4: throw ExceptionArm("bad bits in decodeMiscInstruction (4)", this, 4);
        case 5: {
          SgAsmArmRegisterReferenceExpression* rd = makeRegister((insn >> 12) & 15);
          SgAsmArmRegisterReferenceExpression* rn = makeRegister((insn >> 16) & 15);
          SgAsmArmRegisterReferenceExpression* rm = makeRegister(insn & 15);
          uint8_t op = (insn >> 21) & 3;
          switch (op) {
            case 0: return MAKE_INSN3(qadd, 4, rd, rm, rn);
            case 1: return MAKE_INSN3(qsub, 4, rd, rm, rn);
            case 2: return MAKE_INSN3(qdadd, 5, rd, rm, rn);
            case 3: return MAKE_INSN3(qdsub, 5, rd, rm, rn);
            default: ROSE_ASSERT (false);
          }
        }
        case 6: throw ExceptionArm("bad bits in decodeMiscInstruction (6)", this, 4);
        case 7: {
          uint16_t imm1 = (insn >> 8) & 0xFFF;
          uint16_t imm2 = insn & 0xF;
          uint16_t imm = (imm1 << 4) | imm2;
          return MAKE_INSN1(bkpt, 4, SageBuilderAsm::makeWordValue(imm));
        }
        default: ROSE_ASSERT (false);
      }
    } else { // bit 7 set -- signed mul
      SgAsmArmRegisterReferenceExpression* rd = makeRegister((insn >> 16) & 15);
      SgAsmArmRegisterReferenceExpression* rn = makeRegister((insn >> 12) & 15);
      SgAsmArmRegisterReferenceExpression* rs = makeRegister((insn >> 8) & 15);
      SgAsmArmRegisterReferenceExpression* rm = makeRegister(insn & 15);
      uint8_t op = (insn >> 21) & 3;
      bool y = (insn >> 6) & 1;
      bool x = (insn >> 5) & 1;
      switch ((op << 2) | (x ? 2 : 0) | (y ? 1 : 0)) {
        case 0x0: return MAKE_INSN4(smlabb, 6, rd, rm, rs, rn);
        case 0x1: return MAKE_INSN4(smlabt, 6, rd, rm, rs, rn);
        case 0x2: return MAKE_INSN4(smlatb, 6, rd, rm, rs, rn);
        case 0x3: return MAKE_INSN4(smlatt, 6, rd, rm, rs, rn);
        case 0x4: return MAKE_INSN4(smlawb, 6, rd, rm, rs, rn);
        case 0x5: return MAKE_INSN4(smlawt, 6, rd, rm, rs, rn);
        case 0x6: return MAKE_INSN4(smluwb, 6, rd, rm, rs, rn);
        case 0x7: return MAKE_INSN4(smluwt, 6, rd, rm, rs, rn);
        case 0x8: return MAKE_INSN4(smlalbb, 7, rn, rd, rm, rs);
        case 0x9: return MAKE_INSN4(smlalbt, 7, rn, rd, rm, rs);
        case 0xA: return MAKE_INSN4(smlaltb, 7, rn, rd, rm, rs);
        case 0xB: return MAKE_INSN4(smlaltt, 7, rn, rd, rm, rs);
        case 0xC: return MAKE_INSN3(smulbb, 6, rd, rm, rs);
        case 0xD: return MAKE_INSN3(smulbt, 6, rd, rm, rs);
        case 0xE: return MAKE_INSN3(smultb, 6, rd, rm, rs);
        case 0xF: return MAKE_INSN3(smultt, 6, rd, rm, rs);
        default: ROSE_ASSERT (false);
      }
    }
// DQ (11/29/2009): Avoid MSVC warning.
   return NULL;
}

SgAsmArmInstruction *
DisassemblerArm::disassemble()
{
      // fprintf(stderr, "Disassembling insn 0x%08" PRIx32 " at addr 0x%08" PRIx32 "\n", insn, p.ip);
      uint8_t condField = (insn >> 28) & 0xF;
      bool bit4 = (insn >> 4) & 1;
      bool bit7 = (insn >> 7) & 1;
      bool bit9 = (insn >> 9) & 1;
      bool bit16 = (insn >> 16) & 1;
      bool bit20 = (insn >> 20) & 1;
      bool bit21 = (insn >> 21) & 1;
      bool bit22 = (insn >> 22) & 1;
      bool bit23 = (insn >> 23) & 1;
      bool bit24 = (insn >> 24) & 1;
      bool bit25 = (insn >> 25) & 1;
      bool bit4_and_bit7 = bit4 && bit7;
      if (condField != 15 || !decodeUnconditionalInstructions) { // Normal instructions (or arm_cond_nv instructions if they are not treated specially)
        cond = (ArmInstructionCondition)(condField + 1);
        uint8_t dataProcOpcode = (insn >> 21) & 15;
        bool dpIsSpecial = (insn & 0x01900000) == 0x01000000;
        switch ((insn >> 26) & 3) {
          case 0: { // Data processing, misc, multiplies, undefined, move imm to status reg
            if ((insn & 0x0F0000F0U) == 0x00000090U) { // Multiplies
              return decodeMultiplyInstruction();
            } else if (bit4_and_bit7 && !bit25) {
              return decodeExtraLoadStores();
            } else if (dpIsSpecial && bit25) {
              if (bit21) {
                SgAsmExpression* imm = makeRotatedImmediate();
                bool useSPSR = bit22;
                uint8_t mask = (insn >> 16) & 15;
                SgAsmArmRegisterReferenceExpression* psr = makePsrFields(useSPSR, mask);
                return MAKE_INSN2(msr, 3, psr, imm);
              } else {
                  throw ExceptionArm("bad bit21", this, 26);
              }
            } else if (dpIsSpecial && !bit25) {
              return decodeMiscInstruction();
            } else { // !dpIsSpecial && !bit4_and_bit7
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
              SgAsmExpression* shifterField = makeShifterField();
              return makeDataProcInstruction(dataProcOpcode, bit20, rn, rd, shifterField);
            }
          }
          case 1: { // Load-store, media, undefined
            if (!bit4 || !bit25) {
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExpression* memref = SageBuilderAsm::makeMemoryReference(decodeMemoryAddress(rn));
              SgAsmExpression* rd = makeRegister((insn >> 12) & 15);
              bool isLoad = bit20;
              bool isByte = bit22;
              bool isTranslated = !bit24 && bit21;
              switch ((isTranslated ? 4 : 0) | (isLoad ? 2 : 0) | (isByte ? 1 : 0)) {
                case 0: return MAKE_INSN2(str, 3, rd, memref);
                case 1: return MAKE_INSN2(strb, 3, rd, memref);
                case 2: return MAKE_INSN2(ldr, 3, rd, memref);
                case 3: return MAKE_INSN2(ldrb, 3, rd, memref);
                case 4: return MAKE_INSN2(strt, 3, rd, memref);
                case 5: return MAKE_INSN2(strbt, 3, rd, memref);
                case 6: return MAKE_INSN2(ldrt, 3, rd, memref);
                case 7: return MAKE_INSN2(ldrbt, 3, rd, memref);
                default: ROSE_ASSERT (false);
              }
            } else if ((insn & 0x0FF000F0U) == 0x07F000F0U) {
              return MAKE_INSN0(undefined, 9);
            } else {
              return decodeMediaInstruction();
            }
          }
          case 2: { // Load-store multiple, branches
            if (!bit25) {
              SgAsmExpression* rn = makeRegister((insn >> 16) & 15);
              SgAsmExprListExp* regs = SageBuilderAsm::makeExprListExp();
              for (int i = 0; i < 16; ++i) {
                if ((insn >> i) & 1) {
                  SgAsmArmRegisterReferenceExpression* reg = makeRegister(i);
                  regs->get_expressions().push_back(reg);
                  reg->set_parent(regs);
                }
              }
              SgAsmExpression* base = rn;
              if (bit21) { // w
                SgAsmExpression* offset = SageBuilderAsm::makeByteValue(regs->get_expressions().size() * 4);
                if (bit23) { // u
                    base = SageBuilderAsm::makeAddPostupdate(rn, offset);
                } else {
                    base = SageBuilderAsm::makeSubtractPostupdate(rn, offset);
                }
              }

           // DQ (11/29/2009): This is a MSVC warning: warning C4805: '|' : unsafe mix of type 'uint32_t' and type 'bool' in operation
              switch (((insn >> 21) & 62) | bit20) { // p, u, s, l
                case 0x0: return MAKE_INSN2(stmda, 3, rn, regs);
                case 0x1: return MAKE_INSN2(ldmda, 3, rn, regs);
                case 0x2: return MAKE_INSN2(stmda, 3, rn, SageBuilderAsm::makeArmSpecialRegisterList(regs));
                case 0x3: return MAKE_INSN2(ldmda, 3, rn, SageBuilderAsm::makeArmSpecialRegisterList(regs));
                case 0x4: return MAKE_INSN2(stmia, 3, rn, regs);
                case 0x5: return MAKE_INSN2(ldmia, 3, rn, regs);
                case 0x6: return MAKE_INSN2(stmia, 3, rn, SageBuilderAsm::makeArmSpecialRegisterList(regs));
                case 0x7: return MAKE_INSN2(ldmia, 3, rn, SageBuilderAsm::makeArmSpecialRegisterList(regs));
                case 0x8: return MAKE_INSN2(stmdb, 3, rn, regs);
                case 0x9: return MAKE_INSN2(ldmdb, 3, rn, regs);
                case 0xA: return MAKE_INSN2(stmdb, 3, rn, SageBuilderAsm::makeArmSpecialRegisterList(regs));
                case 0xB: return MAKE_INSN2(ldmdb, 3, rn, SageBuilderAsm::makeArmSpecialRegisterList(regs));
                case 0xC: return MAKE_INSN2(stmib, 3, rn, regs);
                case 0xD: return MAKE_INSN2(ldmib, 3, rn, regs);
                case 0xE: return MAKE_INSN2(stmib, 3, rn, SageBuilderAsm::makeArmSpecialRegisterList(regs));
                case 0xF: return MAKE_INSN2(ldmib, 3, rn, SageBuilderAsm::makeArmSpecialRegisterList(regs));
                default: ROSE_ASSERT (false);
              }
            } else {
              SgAsmExpression* target = makeBranchTarget();
              if ((insn >> 24) & 1) {
                return MAKE_INSN1(bl, 2, target);
              } else {
                return MAKE_INSN1(b, 1, target);
              }
            }
          }
          case 3: {
            if ((insn & 0x0F000000U) == 0x0F000000U) {
              return MAKE_INSN1(swi, 3, SageBuilderAsm::makeDWordValue(insn & 0x00FFFFFFU));
            } else {
                std::cerr << "Coprocessor not supported 0x" << StringUtility::intToHex(insn) << std::endl;
                throw ExceptionArm("coprocessor not supported", this, 26);
            }
          }
          default: ROSE_ASSERT (!"Can't happen");
        }
      } else { // Unconditional instructions
        cond = arm_cond_al;
        uint16_t opcode1 = (insn >> 20) & 0xFF;

     // DQ (8/30/2008): Unused value removed to avoid compiler warning.
     // uint16_t opcode2 = (insn >> 4) & 0xF;

        switch (opcode1) {
          case 0x10: {
            if (bit16) {
              return MAKE_INSN1(setend, 6, SageBuilderAsm::makeByteValue(bit9));
            } else {
              ROSE_ASSERT (!"CPS not supported");
            }
          }
          default: {
              std::cerr << "Cannot handle too many unconditional instructions: " << StringUtility::intToHex(insn) << std::endl;
              throw ExceptionArm("too many unconditional instructions", this, 32);
          }
        }
      }

   ROSE_ASSERT (!"Fell off end of disassemble");
// DQ (11/29/2009): Avoid MSVC warning.
   return NULL;
}
