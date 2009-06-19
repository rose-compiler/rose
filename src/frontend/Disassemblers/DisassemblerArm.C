#include "rose.h"

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
            ROSE_ASSERT(elist);
            for (size_t i=0; i<elist->get_expressions().size(); i++) {
                SgAsmArmRegisterReferenceExpression *reg = isSgAsmArmRegisterReferenceExpression(elist->get_expressions()[i]);
                ROSE_ASSERT(reg);
                if (reg->get_arm_register_code()==SgAsmArmRegisterReferenceExpression::reg15) {
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
                if (rre && rre->get_arm_register_code()==SgAsmArmRegisterReferenceExpression::reg15) {
                    return true;
                }
            }
        }
    }
    return false;
}

Disassembler::AddressSet
SgAsmArmInstruction::get_successors() {
    Disassembler::AddressSet retval;
    const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();

    switch (get_kind()) {
        case arm_b:
        case arm_bl:
        case arm_blx:
        case arm_bx: {
            /* Branch target */
            ROSE_ASSERT(exprs.size()==1);
            SgAsmExpression *dest = exprs[0];
            ROSE_ASSERT(isSgAsmValueExpression(dest));
            rose_addr_t target_va = SageInterface::getAsmConstant(isSgAsmValueExpression(dest));
            retval.insert(target_va);

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
            if (rre->get_arm_register_code()==SgAsmArmRegisterReferenceExpression::reg15) {
                retval.insert(get_address()+4);
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
            if (!modifies_ip(this) || get_condition()!=arm_cond_al)
                retval.insert(get_address()+4);
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

Disassembler *
DisassemblerArm::can_disassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    if ((isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_ARM_Family)
        return new DisassemblerArm(header);
    return NULL;
}

void
DisassemblerArm::init(SgAsmGenericHeader *header)
{
    params = ArmDisassembler::Parameters(0, true);
    set_wordsize(4);
    set_alignment(4);
    set_sex(SgAsmExecutableFileFormat::ORDER_LSB);
}

/* This is a bit of a kludge for now because we're trying to use an unmodified version of the ArmDisassembler name space. */
SgAsmInstruction *
DisassemblerArm::disassembleOne(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va, 
                                AddressSet *successors)
{
    /* Starting virtual address was not available when the DisassemblerArm class was instantiated, so set it now. */
    if (start_va & 0x3)
        throw Exception("instruction pointer not word aligned");
    if (start_va > 0xffffffff)
        throw Exception("instruction pointer out of range");
    params.ip = start_va;

    /* The old ArmDisassembler::disassemble() function doesn't understand RvaFileMap mappings. Therefore, remap the next
     * few bytes (enough for at least one instruction) into a temporary buffer. */
    unsigned char temp[4]; /* all ARM instructions are 32 bits */
    size_t tempsz = map.read(temp, buf, start_va, sizeof temp);

    /* Disassemble the instruction */
    SgAsmArmInstruction *insn = NULL;
    AddressSet suc1;
    try {
        insn = ArmDisassembler::disassemble(params, temp, tempsz, 0, &suc1);
    } catch(const ArmDisassembler::OverflowOfInstructionVector&) {
        throw Exception("short read");
    } catch(const ArmDisassembler::BadInstruction&) {
        throw Exception("bad instruction");
    } catch(...) {
        throw Exception("cannot disassemble");
    }
    if (successors)
        successors->insert(suc1.begin(), suc1.end());

#if 0 /*DEBUG*/
    /* Make sure the successors returned by the disassembler match the successors returned by the instruction since they are
     * built two different ways.  The biggest difference is that the old disassembler includes the fall-through instruction as
     * a known successor even for unconditional branches.  When this code is removed, we can optimize the decoding above to
     * not need the temporary suc1. */
    AddressSet suc2 = insn->get_successors();
    if (suc1!=suc2) {
        bool skip_assertion = false;
        switch (insn->get_kind()) {
            case arm_bl:
                skip_assertion = insn->get_condition()==arm_cond_al;
                break;
            case arm_ldmia:
            case arm_ldr:
            case arm_mov:
                skip_assertion = modifies_ip(insn) && insn->get_condition()==arm_cond_al;
                break;
            default:
                break;
        }
        if (!skip_assertion) {
            fprintf(stderr, "Successors do not match for instruction: %s\n", unparseInstructionWithAddress(insn).c_str());
            fprintf(stderr, "  Old method:\n");
            for (AddressSet::iterator si=suc1.begin(); si!=suc1.end(); si++)
                fprintf(stderr, "    0x%08"PRIx64"\n", *si);
            fprintf(stderr, "  New method:\n");
            for (AddressSet::iterator si=suc2.begin(); si!=suc2.end(); si++)
                fprintf(stderr, "    0x%08"PRIx64"\n", *si);
            ROSE_ASSERT(suc1==suc2);
        }
    }
#endif

    ROSE_ASSERT(insn);
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
