#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* See header file for full documentation. */

/* References:
 *
 * [1] "Programming Environments Manual for 32-Bit Implementations of the PowerPC Architecture", MPCFPE32B Rev. 3, 9/2005, by
 * Freescale Semiconductor (www.freescale.com). Freescale Semiconductor Technical Information Center, 1300 N. Alma School Road,
 * Chandler, AZ 85224. 1-800-521-6274.  A copy can be found on the web at
 * http://www-106.ibm.com/developerworks/linux/library/l-powarch/
 */


Disassembler::AddressSet
SgAsmPowerpcInstruction::get_successors() {
    Disassembler::AddressSet retval;
    switch (get_kind()) {
        case powerpc_bc:
        case powerpc_bca:
        case powerpc_bcl:
        case powerpc_bcla: {
            /* Conditional branches: bcX BO,BI,TARGET */
            const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()==3);
            ROSE_ASSERT(isSgAsmValueExpression(exprs[2]));
            rose_addr_t target = SageInterface::getAsmConstant(isSgAsmValueExpression(exprs[2]));
            retval.insert(target);
            retval.insert(get_address()+get_raw_bytes().size());
            break;
        }

        case powerpc_bcctr:
        case powerpc_bcctrl:
        case powerpc_bclr:
        case powerpc_bclrl:
            /* Conditional branches to count register; target is unknown */
            retval.insert(get_address()+get_raw_bytes().size());
            break;

        case powerpc_b:
        case powerpc_ba:
        case powerpc_bl:
        case powerpc_bla: {
            /* Unconditional branches */
            const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()==1);
            ROSE_ASSERT(isSgAsmValueExpression(exprs[0]));
            rose_addr_t target = SageInterface::getAsmConstant(isSgAsmValueExpression(exprs[0]));
            retval.insert(target);
            break;
        }

        case powerpc_unknown_instruction:
        case powerpc_tw:
        case powerpc_twi:
        case powerpc_rfi:
        case powerpc_sc:
            /* No known successors */
            break;

        default:
            /* All others fall through to next instruction */
            retval.insert(get_address()+get_raw_bytes().size());
            break;
    }
    return retval;
}

bool
SgAsmPowerpcInstruction::terminatesBasicBlock() {
    switch (get_kind()) {
        case powerpc_unknown_instruction:
        case powerpc_b:         /* branch instructions... */
        case powerpc_ba:
        case powerpc_bl:
        case powerpc_bla:
        case powerpc_bc:
        case powerpc_bca:
        case powerpc_bcl:
        case powerpc_bcla:
        case powerpc_bcctr:
        case powerpc_bcctrl:
        case powerpc_bclr:
        case powerpc_bclrl:
        case powerpc_tw:        /* trap instructions... */
        case powerpc_twi:
        case powerpc_sc:        /* system call */
        case powerpc_rfi:       /* return from interrupt */
            return true;

        default:
            return false;
    }
}

Disassembler *
DisassemblerPowerpc::can_disassemble(SgAsmGenericHeader *header) const
{
    SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    if (isa == SgAsmExecutableFileFormat::ISA_PowerPC)
        return new DisassemblerPowerpc(header);
    return NULL;
}

void
DisassemblerPowerpc::init(SgAsmGenericHeader *header)
{
    set_wordsize(4);
    set_alignment(4);
    set_sex(SgAsmExecutableFileFormat::ORDER_LSB);
}

/* This is a bit of a kludge for now because we're trying to use an unmodified version of the PowerpcDisassembler name space. */
SgAsmInstruction *
DisassemblerPowerpc::disassembleOne(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va, 
                                    AddressSet *successors)
{
    /* Starting virtual address was not available when the DisassemblerPowerpc class was instantiated, so set it now. */
    params.ip = start_va;

    /* The old PowerpcDisassembler::disassemble() function doesn't understand RvaFileMap mappings. Therefore, remap the next
     * few bytes (enough for at least one instruction) into a temporary buffer. */
    unsigned char temp[4];
    size_t tempsz = map.read(temp, buf, start_va, sizeof temp);

    /* Disassemble the instruction */
    SgAsmPowerpcInstruction *insn = NULL;
    AddressSet suc1;
    try {
        insn = PowerpcDisassembler::disassemble(params, temp, tempsz, 0, &suc1);
    } catch(const PowerpcDisassembler::OverflowOfInstructionVector&) {
        throw Exception("short read");
    } catch(const PowerpcDisassembler::BadInstruction&) {
        throw Exception("bad instruction");
    } catch(...) {
        throw Exception("cannot disassemble");
    }
    if (successors)
        successors->insert(suc1.begin(), suc1.end());

#if 0 /*DEBUG*/
    /* Make sure the successors returned by the disassembler match the successors returned by the instruction since they are
     * built two different ways. When this code is removed, we can optimize the decoding above to not need the temporary suc1. */
    AddressSet suc2 = insn->get_successors();
    if (suc1!=suc2) {
        fprintf(stderr, "Successors do not match for instruction: %s\n", unparseInstructionWithAddress(insn).c_str());
        fprintf(stderr, "  Old method:\n");
        for (AddressSet::iterator si=suc1.begin(); si!=suc1.end(); si++)
            fprintf(stderr, "    0x%08"PRIx64"\n", *si);
        fprintf(stderr, "  New method:\n");
        for (AddressSet::iterator si=suc2.begin(); si!=suc2.end(); si++)
            fprintf(stderr, "    0x%08"PRIx64"\n", *si);
        ROSE_ASSERT(suc1==suc2);
    }
#endif

    ROSE_ASSERT(insn);
    return insn;
}

SgAsmInstruction *
DisassemblerPowerpc::make_unknown_instruction(const Exception &e) 
{
    SgAsmPowerpcInstruction *insn = new SgAsmPowerpcInstruction(e.ip, "unknown", powerpc_unknown_instruction);
    SgAsmOperandList *operands = new SgAsmOperandList();
    insn->set_operandList(operands);
    operands->set_parent(insn);
    insn->set_raw_bytes(e.bytes);
    return insn;
}
