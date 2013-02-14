/* SgAsmMipsInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"

// see base class
bool
SgAsmMipsInstruction::terminates_basic_block()
{
    switch (get_kind()) {
        case mips_beq:
        case mips_beql:
        case mips_bgez:
        case mips_bgezal:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtz:
        case mips_bgtzl:
        case mips_blez:
        case mips_blezl:
        case mips_bltz:
        case mips_bltzal:
        case mips_bltzall:
        case mips_bltzl:
        case mips_bne:
        case mips_bnel:
        case mips_break: // ???
        case mips_j:
        case mips_jal:
        case mips_jalr:
        case mips_jalx:
        case mips_jr:
        case mips_jr_hb:
        case mips_syscall:
        case mips_teq:
        case mips_teqi:
        case mips_tge:
        case mips_tgei:
        case mips_tgeiu:
        case mips_tgeu:
        case mips_tlt:
        case mips_tlti:
        case mips_tltiu:
        case mips_tltu:
        case mips_tne:
        case mips_tnei:
            return true;
        default:
            return false;
    }
}

// see base class
bool
SgAsmMipsInstruction::is_function_call(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target)
{
    if (insns.size()==0)
        return false;
    SgAsmMipsInstruction *last = isSgAsmMipsInstruction(insns.back());
    if (!last)
        return false;
    switch (last->get_kind()) {
        case mips_bgezal:
        case mips_bgezall:
        case mips_bltzal:
        case mips_bltzall:
        case mips_jal:
        case mips_jalr:
        case mips_jalr_hb:
        case mips_jalx: {
            (void) last->get_branch_target(target); // target will not be changed if unknown
            return true;
        }
        default:
            return false;
    }
}

// see base class
bool
SgAsmMipsInstruction::is_function_return(const std::vector<SgAsmInstruction*> &insns)
{
    if (insns.empty())
        return false;
    SgAsmMipsInstruction *last = isSgAsmMipsInstruction(insns.back());
    if (!last)
        return false;
    if (last->get_kind()!=mips_jr)
        return false;
    const SgAsmExpressionPtrList &args = last->get_operandList()->get_operands();
    if (args.size()<1)
        return false;
    SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(args[0]);
    if (!rre)
        return false;
    if (rre->get_descriptor().get_major()!=mips_regclass_gpr || rre->get_descriptor().get_minor()!=31)
        return false;
    return true; // this is a "JR ra" instruction.
}

// see base class
std::set<rose_addr_t>
SgAsmMipsInstruction::get_successors(bool *complete_)
{
    bool complete = false;
    rose_addr_t target_va = 0;
    std::set<rose_addr_t> successors;
    switch (get_kind()) {
        case mips_break:
        case mips_j:
        case mips_jal:
        case mips_jalr:
        case mips_jalx:
        case mips_jr:
        case mips_jr_hb:
        case mips_syscall:
            // unconditional branch
            if ((complete=get_branch_target(&target_va)))
                successors.insert(target_va);
            break;

        case mips_beq:
        case mips_beql:
        case mips_bgez:
        case mips_bgezal:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtz:
        case mips_bgtzl:
        case mips_blez:
        case mips_blezl:
        case mips_bltz:
        case mips_bltzal:
        case mips_bltzall:
        case mips_bltzl:
        case mips_bne:
        case mips_bnel:
        case mips_teq:
        case mips_teqi:
        case mips_tge:
        case mips_tgei:
        case mips_tgeiu:
        case mips_tgeu:
        case mips_tlt:
        case mips_tlti:
        case mips_tltiu:
        case mips_tltu:
        case mips_tne:
        case mips_tnei:
            // conditional branch
            if ((complete=get_branch_target(&target_va)))
                successors.insert(target_va);
            successors.insert(get_address() + get_size()); // fall through address
            break;

        default:
            // fall through
            successors.insert(get_address() + get_size());
            complete = true;
    }
    if (complete_)
        *complete_ = complete;
    return successors;
}

// see base class
bool
SgAsmMipsInstruction::is_unknown() const
{
    return mips_unknown_instruction == get_kind();
}

bool
SgAsmMipsInstruction::get_branch_target(rose_addr_t *target)
{
    SgAsmExpressionPtrList &args = get_operandList()->get_operands();
    switch (get_kind()) {
        case mips_j:
        case mips_jal:
        case mips_jalx: {
            // target address stored in first argument
            assert(args.size()>=1);
            if (target) {
                SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[0]);
                assert(ival!=NULL);
                *target = ival->get_absolute_value();
            }
            return true;
        }

        case mips_bgez:
        case mips_bgezal:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtz:
        case mips_bgtzl:
        case mips_blez:
        case mips_blezl:
        case mips_bltz:
        case mips_bltzal:
        case mips_bltzall:
        case mips_bltzl: {
            // target address stored in the second argument
            assert(args.size()>=2);
            if (target) {
                SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[1]);
                assert(ival!=NULL);
                *target = ival->get_absolute_value();
            }
            return true;
        }

        case mips_beq:
        case mips_beql:
        case mips_bne:
        case mips_bnel: {
            // target address stored in the third argument
            assert(args.size()>=3);
            if (target) {
                SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[2]);
                assert(ival!=NULL);
                *target = ival->get_absolute_value();
            }
            return true;
        }

        default:
            // no known target; do not modify *target
            return false;
    }
}




            
