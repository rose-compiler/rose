/* SgAsmM68kInstruction member definitions. Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any other *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include "CommandLine.h"
#include "Diagnostics.h"
#include "Disassembler.h"
#include "DispatcherM68k.h"
#include "PartialSymbolicSemantics2.h"
#include "SymbolicSemantics2.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

unsigned
SgAsmM68kInstruction::get_anyKind() const {
    return p_kind;
}

// see base class
bool
SgAsmM68kInstruction::terminatesBasicBlock()
{
    switch (get_kind()) {
        case m68k_unknown_instruction:
        case m68k_bcc:
        case m68k_bcs:
        case m68k_beq:
        case m68k_bge:
        case m68k_bgt:
        case m68k_bhi:
        case m68k_ble:
        case m68k_bls:
        case m68k_blt:
        case m68k_bmi:
        case m68k_bne:
        case m68k_bpl:
        case m68k_bvc:
        case m68k_bvs:
        case m68k_bra:
        case m68k_bsr:
        case m68k_callm:
        case m68k_dbcc:
        case m68k_dbcs:
        case m68k_dbeq:
        case m68k_dbf:
        case m68k_dbge:
        case m68k_dbgt:
        case m68k_dbhi:
        case m68k_dble:
        case m68k_dbls:
        case m68k_dblt:
        case m68k_dbmi:
        case m68k_dbne:
        case m68k_dbpl:
        case m68k_dbt:
        case m68k_dbvc:
        case m68k_dbvs:
        case m68k_illegal:
        case m68k_jmp:
        case m68k_jsr:
        case m68k_rtd:
        case m68k_rtm:
        case m68k_rtr:
        case m68k_rts:
        case m68k_trap:
        case m68k_trapcc:
        case m68k_trapcs:
        case m68k_trapeq:
        case m68k_trapf:
        case m68k_trapge:
        case m68k_trapgt:
        case m68k_traphi:
        case m68k_traple:
        case m68k_trapls:
        case m68k_traplt:
        case m68k_trapmi:
        case m68k_trapne:
        case m68k_trappl:
        case m68k_trapt:
        case m68k_trapvc:
        case m68k_trapvs:
        case m68k_trapv:
        case m68k_fbeq:
        case m68k_fbne:
        case m68k_fbgt:
        case m68k_fbngt:
        case m68k_fbge:
        case m68k_fbnge:
        case m68k_fblt:
        case m68k_fbnlt:
        case m68k_fble:
        case m68k_fbnle:
        case m68k_fbgl:
        case m68k_fbngl:
        case m68k_fbgle:
        case m68k_fbngle:
        case m68k_fbogt:
        case m68k_fbule:
        case m68k_fboge:
        case m68k_fbult:
        case m68k_fbolt:
        case m68k_fbuge:
        case m68k_fbole:
        case m68k_fbugt:
        case m68k_fbogl:
        case m68k_fbueq:
        case m68k_fbor:
        case m68k_fbun:
        case m68k_fbf:
        case m68k_fbt:
        case m68k_fbsf:
        case m68k_fbst:
        case m68k_fbseq:
        case m68k_fbsne:
            return true;
        default:
            return false;
    }
}

// see base class; don't modify target_va or return_va if they are not known
bool
SgAsmM68kInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target_va,
                                         rose_addr_t *return_va)
{
    if (insns.empty())
        return false;
    SgAsmM68kInstruction *last = isSgAsmM68kInstruction(insns.back());
    if (!last)
        return false;

    // Quick method based only on the kind of instruction
    if (m68k_bsr==last->get_kind() || m68k_jsr==last->get_kind() || m68k_callm==last->get_kind()) {
        last->getBranchTarget(target_va); // only modifies target_va if it can be determined
        if (return_va)
            *return_va = last->get_address() + last->get_size();
        return true;
    }

    return false;
}

// see base class; don't modify target_va or return_va if they are not known
bool
SgAsmM68kInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target_va,
                                         rose_addr_t *return_va)
{
    if (isFunctionCallFast(insns, target_va, return_va))
        return true;

    static const size_t EXECUTION_LIMIT = 25; // max size of basic blocks for expensive analyses
    if (insns.empty())
        return false;
    SgAsmM68kInstruction *last = isSgAsmM68kInstruction(insns.back());
    if (!last)
        return false;
    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(last);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(func);

    // Slow method: Emulate the instructions and then look at the program counter (PC) and stack (A7).  If the PC points
    // outside the current function and the top of the stack holds an address of an instruction within the current function,
    // then this must be a function call.
    if (interp && insns.size()<=EXECUTION_LIMIT) {
        using namespace Rose::BinaryAnalysis;
        using namespace Rose::BinaryAnalysis::InstructionSemantics2;
        using namespace Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
        const InstructionMap &imap = interp->get_instruction_map();
        const RegisterDictionary *regdict = RegisterDictionary::dictionary_for_isa(interp);
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        ASSERT_not_null(ops);
        DispatcherM68kPtr dispatcher = DispatcherM68k::instance(ops, 32);
        SValuePtr orig_sp = SValue::promote(ops->peekRegister(dispatcher->REG_A[7]));
        try {
            for (size_t i=0; i<insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // If the next instruction address is concrete but does not point to a function entry point, then this is not a call.
        SValuePtr ip = SValue::promote(ops->peekRegister(dispatcher->REG_PC));
        if (ip->is_number()) {
            rose_addr_t target_va = ip->get_number();
            SgAsmFunction *target_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(target_va, NULL));
            if (!target_func || target_va!=target_func->get_entry_va())
                return false;
        }

        // If nothing was pushed onto the stack, then this isn't a function call.
        SValuePtr sp = SValue::promote(ops->peekRegister(dispatcher->REG_A[7]));
        SValuePtr stack_delta = SValue::promote(ops->add(sp, ops->negate(orig_sp)));
        SValuePtr stack_delta_sign = SValue::promote(ops->extract(stack_delta, 31, 32));
        if (stack_delta_sign->is_number() && 0==stack_delta_sign->get_number())
            return false;

        // If the top of the stack does not contain a concrete value or the top of the stack does not point to an instruction
        // in this basic block's function, then this is not a function call.
        SValuePtr top = SValue::promote(ops->peekMemory(RegisterDescriptor(), sp, sp->undefined_(32)));
        if (top->is_number()) {
            rose_addr_t va = top->get_number();
            SgAsmFunction *return_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(va, NULL));
            if (!return_func || return_func!=func) {
                return false;
            }
        } else {
            return false;
        }

        // Since the instruction pointer might point to a function entry address and since the top of the stack contains a
        // pointer to an instruction in this function, we assume that this is a function call.
        if (target_va && ip->is_number())
            *target_va = ip->get_number();
        if (return_va && top->is_number())
            *return_va = top->get_number();
        return true;
    }

    // Similar to the above method, but works when all we have is the basic block (e.g., this case gets hit quite a bit from
    // the Partitioner).  Returns true if, after executing the basic block, the top of the stack contains the fall-through
    // address of the basic block. We depend on our caller to figure out if the instruction pointer is reasonably a function
    // entry address.
    if (!interp && insns.size()<=EXECUTION_LIMIT) {
        using namespace Rose::BinaryAnalysis;
        using namespace Rose::BinaryAnalysis::InstructionSemantics2;
        using namespace Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
        const RegisterDictionary *regdict = RegisterDictionary::dictionary_coldfire_emac();
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        DispatcherM68kPtr dispatcher = DispatcherM68k::instance(ops, 32);
        try {
            for (size_t i=0; i<insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // Look at the top of the stack
        SValuePtr top = SValue::promote(ops->peekMemory(RegisterDescriptor(), ops->peekRegister(dispatcher->REG_A[7]),
                                                        ops->protoval()->undefined_(32)));
        if (top->is_number() && top->get_number() == last->get_address()+last->get_size()) {
            if (target_va) {
                SValuePtr ip = SValue::promote(ops->peekRegister(dispatcher->REG_PC));
                if (ip->is_number())
                    *target_va = ip->get_number();
            }
            if (return_va)
                *return_va = top->get_number();
            return true;
        }
    }

    return false;
}

bool
SgAsmM68kInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>& insns)
{
    if (insns.empty())
        return false;
    SgAsmM68kInstruction *last = isSgAsmM68kInstruction(insns.back());
    if (!last)
        return false;
    if (last->get_kind()==m68k_rtd || last->get_kind()==m68k_rtm || last->get_kind()==m68k_rtr || last->get_kind()==m68k_rts)
        return true;
    return false;
}

bool 
SgAsmM68kInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>& insns)
{
    return isFunctionReturnFast(insns);
}

bool
SgAsmM68kInstruction::isUnknown() const
{
    return m68k_unknown_instruction == get_kind();
}

AddressSet
SgAsmM68kInstruction::getSuccessors(bool *complete)
{
    AddressSet retval;
    *complete = true;

    switch (get_kind()) {
        //case m68k_halt: {
        //    // Instructions having no successors
        //    break;
        //}

        case m68k_unknown_instruction:
        case m68k_illegal:
        case m68k_trap: {
            // Instructions having unknown successors
            *complete = false;
            break;
        }

        case m68k_rtd:
        case m68k_rtm:
        case m68k_rtr:
        case m68k_rts: {
            // Instructions that have a single successor that is unknown
            *complete = false;
            break;
        }
            
        case m68k_bcc:
        case m68k_bcs:
        case m68k_beq:
        case m68k_bge:
        case m68k_bgt:
        case m68k_bhi:
        case m68k_ble:
        case m68k_bls:
        case m68k_blt:
        case m68k_bmi:
        case m68k_bne:
        case m68k_bpl:
        case m68k_bvc:
        case m68k_bvs:
        case m68k_bkpt:
        case m68k_chk:
        case m68k_chk2:
        case m68k_dbhi:
        case m68k_dbls:
        case m68k_dbcc:
        case m68k_dbcs:
        case m68k_dbne:
        case m68k_dbeq:
        case m68k_dbf:
        case m68k_dbvc:
        case m68k_dbvs:
        case m68k_dbpl:
        case m68k_dbmi:
        case m68k_dbge:
        case m68k_dblt:
        case m68k_dbgt:
        case m68k_dble:
        case m68k_fbeq:
        case m68k_fbne:
        case m68k_fbgt:
        case m68k_fbngt:
        case m68k_fbge:
        case m68k_fbnge:
        case m68k_fblt:
        case m68k_fbnlt:
        case m68k_fble:
        case m68k_fbnle:
        case m68k_fbgl:
        case m68k_fbngl:
        case m68k_fbgle:
        case m68k_fbngle:
        case m68k_fbogt:
        case m68k_fbule:
        case m68k_fboge:
        case m68k_fbult:
        case m68k_fbolt:
        case m68k_fbuge:
        case m68k_fbole:
        case m68k_fbugt:
        case m68k_fbogl:
        case m68k_fbueq:
        case m68k_fbor:
        case m68k_fbun:
        case m68k_fbf:
        case m68k_fbt:
        case m68k_fbsf:
        case m68k_fbst:
        case m68k_fbseq:
        case m68k_fbsne:
        case m68k_trapt:
        case m68k_traphi:
        case m68k_trapls:
        case m68k_trapcc:
        case m68k_trapcs:
        case m68k_trapne:
        case m68k_trapeq:
        case m68k_trapvc:
        case m68k_trapvs:
        case m68k_trappl:
        case m68k_trapmi:
        case m68k_trapge:
        case m68k_traplt:
        case m68k_trapgt:
        case m68k_traple:
        case m68k_trapv: {
            // Fall-through address and another (known or unknown) address
            rose_addr_t target_va;
            if (getBranchTarget(&target_va)) {
                retval.insert(target_va);
            } else {
                *complete = false;
            }
            retval.insert(get_address() + get_size());
            break;
        }
            
        case m68k_bra:
        case m68k_bsr:
        case m68k_callm:
        case m68k_jmp:
        case m68k_jsr: {
            // Unconditional branches
            rose_addr_t target_va;
            if (getBranchTarget(&target_va)) {
                retval.insert(target_va);
            } else {
                *complete = false;
            }
            break;
        }

        case m68k_dbt:                                  // no-op
        case m68k_trapf:                                // no-op
        default: {
            // Instructions that always only fall through
            retval.insert(get_address() + get_size());
            break;
        }
    }
    return retval;
}

AddressSet
SgAsmM68kInstruction::getSuccessors(const std::vector<SgAsmInstruction*>& insns, bool *complete,
                                    const BinaryAnalysis::MemoryMap::Ptr &initial_memory)
{
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;
    Stream debug(mlog[DEBUG]);

    if (debug) {
        debug <<"SgAsmM68kInstruction::getSuccessors(" <<StringUtility::addrToString(insns.front()->get_address())
              <<" for " <<insns.size() <<" instruction" <<(1==insns.size()?"":"s") <<"):" <<"\n";
    }

    AddressSet successors = SgAsmInstruction::getSuccessors(insns, complete);

    // If we couldn't determine all the successors, or a cursory analysis couldn't narrow it down to a single successor then
    // we'll do a more thorough analysis now. In the case where the cursory analysis returned a complete set containing two
    // successors, a thorough analysis might be able to narrow it down to a single successor. We should not make special
    // assumptions about function call instructions -- their only successor is the specified address operand. */
    if (!*complete || successors.size()>1) {
        using namespace Rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics;

        const RegisterDictionary *regdict = RegisterDictionary::dictionary_coldfire_emac();
        RiscOperatorsPtr ops = RiscOperators::instance(regdict);
        ops->set_memory_map(initial_memory);
        DispatcherM68kPtr dispatcher = DispatcherM68k::instance(ops, 32);
        
        try {
            for (size_t i=0; i<insns.size(); ++i) {
                dispatcher->processInstruction(insns[i]);
                if (debug)
                    debug << "  state after " <<insns[i]->toString() <<"\n" <<*ops;
            }
            SValuePtr ip = SValue::promote(ops->peekRegister(dispatcher->REG_PC));
            if (ip->is_number()) {
                successors.clear();
                successors.insert(ip->get_number());
                *complete = true; /*this is the complete set of successors*/
            }
        } catch(const BaseSemantics::Exception& e) {
            /* Abandon entire basic block if we hit an instruction that's not implemented. */
            debug <<e <<"\n";
        }
    }

    if (debug) {
        debug <<"  successors:";
        BOOST_FOREACH (rose_addr_t va, successors.values())
            debug <<" " <<StringUtility::addrToString(va);
        debug <<(*complete?"":"...") <<"\n";
    }

    return successors;
}

bool
SgAsmM68kInstruction::getBranchTarget(rose_addr_t *target)
{
    size_t labelArg = 999;                              // which argument is the target?
    bool useEffectiveAddress = false;                   // use the effective address as the target

    switch (get_kind()) {
        case m68k_bcc:
        case m68k_bcs:
        case m68k_beq:
        case m68k_bge:
        case m68k_bgt:
        case m68k_bhi:
        case m68k_ble:
        case m68k_bls:
        case m68k_blt:
        case m68k_bmi:
        case m68k_bne:
        case m68k_bpl:
        case m68k_bvc:
        case m68k_bvs:
        case m68k_bsr:
        case m68k_bra:
        case m68k_fbeq:
        case m68k_fbne:
        case m68k_fbgt:
        case m68k_fbngt:
        case m68k_fbge:
        case m68k_fbnge:
        case m68k_fblt:
        case m68k_fbnlt:
        case m68k_fble:
        case m68k_fbnle:
        case m68k_fbgl:
        case m68k_fbngl:
        case m68k_fbgle:
        case m68k_fbngle:
        case m68k_fbogt:
        case m68k_fbule:
        case m68k_fboge:
        case m68k_fbult:
        case m68k_fbolt:
        case m68k_fbuge:
        case m68k_fbole:
        case m68k_fbugt:
        case m68k_fbogl:
        case m68k_fbueq:
        case m68k_fbor:
        case m68k_fbun:
        case m68k_fbf:
        case m68k_fbt:
        case m68k_fbsf:
        case m68k_fbst:
        case m68k_fbseq:
        case m68k_fbsne:
        case m68k_jmp:
        case m68k_jsr:
            labelArg = 0;
            break;

        case m68k_bkpt:
        case m68k_chk:
        case m68k_chk2:
        case m68k_trapt:
        case m68k_traphi:
        case m68k_trapls:
        case m68k_trapcc:
        case m68k_trapcs:
        case m68k_trapne:
        case m68k_trapeq:
        case m68k_trapvc:
        case m68k_trapvs:
        case m68k_trappl:
        case m68k_trapmi:
        case m68k_trapge:
        case m68k_traplt:
        case m68k_trapgt:
        case m68k_traple:
        case m68k_trapv:
            // branch, but we know not to where
            return false;

        case m68k_callm:
            labelArg = 1;
            useEffectiveAddress = true;
            break;

        case m68k_dbf:
        case m68k_dbhi:
        case m68k_dbls:
        case m68k_dbcc:
        case m68k_dbcs:
        case m68k_dbne:
        case m68k_dbeq:
        case m68k_dbvc:
        case m68k_dbvs:
        case m68k_dbpl:
        case m68k_dbmi:
        case m68k_dbge:
        case m68k_dblt:
        case m68k_dbgt:
        case m68k_dble:
            labelArg = 1;
            break;

        case m68k_dbt:                                  // no-op
        case m68k_trapf:                                // no-op
        default:
            // Not a branching instruction; do not modify target
            return false;
    }

    const SgAsmExpressionPtrList &args = get_operandList()->get_operands();
    ASSERT_require(labelArg < args.size());
    SgAsmIntegerValueExpression *target_expr = NULL;
    if (useEffectiveAddress) {
        if (SgAsmMemoryReferenceExpression *memref = isSgAsmMemoryReferenceExpression(args[labelArg]))
            target_expr = isSgAsmIntegerValueExpression(memref->get_address());
    }
    if (!target_expr)
        target_expr = isSgAsmIntegerValueExpression(args[labelArg]);
    if (!target_expr)
        return false;
    if (target)
        *target = target_expr->get_absoluteValue();
    return true;
}

std::string
SgAsmM68kInstruction::description() const {
    switch (get_kind()) {
        case m68k_unknown_instruction:  return "";
        case m68k_abcd:                 return "add decimal with extended";
        case m68k_add:                  return "add";
        case m68k_adda:                 return "add address";
        case m68k_addi:                 return "add immediate";
        case m68k_addq:                 return "add quick";
        case m68k_addx:                 return "add extended";
        case m68k_and:                  return "logical AND";
        case m68k_andi:                 return "immediate AND";
        case m68k_asl:                  return "arithmetic shift left";
        case m68k_asr:                  return "arithmetic shift right";
        case m68k_bcc:                  return "branch carry clear";
        case m68k_bcs:                  return "branch carry set";
        case m68k_beq:                  return "branch equal";
        case m68k_bge:                  return "branch greater or equal";
        case m68k_bgt:                  return "branch greater than";
        case m68k_bhi:                  return "branch high";
        case m68k_ble:                  return "branch less or equal";
        case m68k_bls:                  return "branch lower or same";
        case m68k_blt:                  return "branch less than";
        case m68k_bmi:                  return "branch minus";
        case m68k_bne:                  return "branch not equal";
        case m68k_bpl:                  return "branch plus";
        case m68k_bvc:                  return "branch overflow clear";
        case m68k_bvs:                  return "branch overflow set";
        case m68k_bchg:                 return "test bit and change";
        case m68k_bclr:                 return "test bit and clear";
        case m68k_bfchg:                return "test bit field and change";
        case m68k_bfclr:                return "test bit field and clear";
        case m68k_bfexts:               return "extract bit field signed";
        case m68k_bfextu:               return "extract bit field unsigned";
        // m68k_bfffo,                  find first one in bit field
        case m68k_bfins:                return "insert bit field";
        case m68k_bfset:                return "test bit field and set";
        case m68k_bftst:                return "test bit field";
        // m68k_bitrev,                 bit reverse register
        case m68k_bkpt:                 return "breakpoint";
        case m68k_bra:                  return "branch always";
        case m68k_bset:                 return "test bit and set";
        case m68k_bsr:                  return "branch to subroutine";
        case m68k_btst:                 return "test bit";
        // m68k_byterev,                byte reverse register
        case m68k_callm:                return "call module";
        case m68k_cas:                  return "compare and swap with operand";
        case m68k_cas2:                 return "compare and swap with operands";
        case m68k_chk:                  return "check register against bounds";
        case m68k_chk2:                 return "check register against bounds";
        case m68k_clr:                  return "clear";
        case m68k_cmp:                  return "compare";
        case m68k_cmp2:                 return "compare register against bounds";
        case m68k_cmpa:                 return "compare address";
        case m68k_cmpi:                 return "compare immediate";
        case m68k_cmpm:                 return "compare memory";
        case m68k_cpusha:               return "push and invalidate all";
        case m68k_cpushl:               return "push and invalidate cache lines";
        case m68k_cpushp:               return "push and invalidate cache pages";
        case m68k_dbt:                  return "decrement and branch if true";
        case m68k_dbf:                  return "decrement and branch if false";
        case m68k_dbhi:                 return "decrement and branch if high";
        case m68k_dbls:                 return "decrement and branch if lower or same";
        case m68k_dbcc:                 return "decrement and branch if cary clear";
        case m68k_dbcs:                 return "decrement and branch if carry set";
        case m68k_dbne:                 return "decrement and branch if not equal";
        case m68k_dbeq:                 return "decrement and branch if equal";
        case m68k_dbvc:                 return "decrement and branch if overflow clear";
        case m68k_dbvs:                 return "decrement and branch if overflow set";
        case m68k_dbpl:                 return "decrement and branch if plus";
        case m68k_dbmi:                 return "decrement and branch if minus";
        case m68k_dbge:                 return "decrement and branch if greater or equal";
        case m68k_dblt:                 return "decrement and branch if less than";
        case m68k_dbgt:                 return "decrement and branch if greater than";
        case m68k_dble:                 return "decrement and branch if less than or equal";
        case m68k_divs:                 return "signed divide with optional remainder";
        case m68k_divsl:                return "signed divide 32-bit quotient with remainder";
        case m68k_divu:                 return "unsigned divide with optional remainder";
        case m68k_divul:                return "unsigned divide 32-bit quotient with remainder";
        case m68k_eor:                  return "exclusive-OR logical";
        case m68k_eori:                 return "exclusive-OR immediate";
        case m68k_exg:                  return "exchange registers";
        case m68k_ext:                  return "sign extend";
        case m68k_extb:                 return "sign extend byte to longword";
        case m68k_fabs:                 return "fp absolute value with FPCR rounding";
        case m68k_fadd:                 return "fp add with FPCR rounding";
        case m68k_fbeq:                 return "fp branch if equal";
        case m68k_fbne:                 return "fp branch if not equal";
        case m68k_fbgt:                 return "fp branch if greater than";
        case m68k_fbngt:                return "fp branch if not greater than";
        case m68k_fbge:                 return "fp branch if greater than or equal";
        case m68k_fbnge:                return "fp branch if not greater than or equal";
        case m68k_fblt:                 return "fp branch if less than";
        case m68k_fbnlt:                return "fp branch if not less than";
        case m68k_fble:                 return "fp branch if less than or equal";
        case m68k_fbnle:                return "fp branch if not less than or equal";
        case m68k_fbgl:                 return "fp branch if greater or less than";
        case m68k_fbngl:                return "fp branch if not greater or less than";
        case m68k_fbgle:                return "fp branch if greater, less, or equal";
        case m68k_fbngle:               return "fp branch if not greater, less, or equal";
        case m68k_fbogt:                return "fp branch if ordered greater than";
        case m68k_fbule:                return "fp branch if unordered or less or equal";
        case m68k_fboge:                return "fp branch if ordered greater than or equal";
        case m68k_fbult:                return "fp branch if unordered less than";
        case m68k_fbolt:                return "fp branch if ordered less than";
        case m68k_fbuge:                return "fp branch if unordered or greater than or equal";
        case m68k_fbole:                return "fp branch if ordered less than or equal";
        case m68k_fbugt:                return "fp branch if unordered or greater than";
        case m68k_fbogl:                return "fp branch if ordered greater or less than";
        case m68k_fbueq:                return "fp branch if unordered or equal";
        case m68k_fbor:                 return "fp branch if ordered";
        case m68k_fbun:                 return "fp branch if unordered";
        case m68k_fbf:                  return "fp branch if false";
        case m68k_fbt:                  return "fp branch if true";
        case m68k_fbsf:                 return "fp branch if signaling false";
        case m68k_fbst:                 return "fp branch if signaling true";
        case m68k_fbseq:                return "fp branch if signaling equal";
        case m68k_fbsne:                return "fp branch if signaling not equal";
        case m68k_fcmp:                 return "fp compare";
        case m68k_fdabs:                return "fp absolute value with double-precision rounding";
        case m68k_fdadd:                return "fp add with double-precision rounding";
        case m68k_fddiv:                return "fp divide with double-precision rounding";
        case m68k_fdiv:                 return "fp divide with FPCR rounding";
        case m68k_fdmove:               return "copy fp data with double-precision rounding";
        case m68k_fdmul:                return "fp multiple with double-precision rounding";
        case m68k_fdneg:                return "fp negation with double-precision rounding";
        case m68k_fdsqrt:               return "fp square root with double-precision rounding";
        case m68k_fdsub:                return "fp subtract with double-precision rounding";
        // m68k_ff1,                    find first one in register
        case m68k_fint:                 return "fp integer part";
        case m68k_fintrz:               return "fp integer part rounded-to-zero";
        case m68k_fmove:                return "copy fp data with FPCR rounding";
        case m68k_fmovem:               return "copy multiple fp data registers";
        case m68k_fmul:                 return "fp multiply with FPCR rounding";
        case m68k_fneg:                 return "fp negate with FPCR rounding";
        case m68k_fnop:                 return "fp no operation";
        // m68k_frestore,
        case m68k_fsabs:                return "fp absolute value with single-precision rounding";
        case m68k_fsadd:                return "fp add with single-precision rounding";
        // m68k_fsave,
        case m68k_fsdiv:                return "fp divide with single-precision rounding";
        case m68k_fsmove:               return "copy fp data with single-precision rounding";
        case m68k_fsmul:                return "fp multiply with single-precision rounding";
        case m68k_fsneg:                return "fp negation with single-precision rounding";
        case m68k_fsqrt:                return "fp square root with FPCR rounding";
        case m68k_fssqrt:               return "fp square root with single-precision rounding";
        case m68k_fssub:                return "fp subtract with single-precision rounding";
        case m68k_fsub:                 return "fp subtract with FPCR rounding";
        case m68k_ftst:                 return "fp test";
        // m68k_halt,                   halt the CPU
        case m68k_illegal:              return "take illegal instruction trap";
        // m68k_intouch,
        case m68k_jmp:                  return "jump";
        case m68k_jsr:                  return "jump to subroutine";
        case m68k_lea:                  return "load effective address";
        case m68k_link:                 return "link and allocate";
        case m68k_lsl:                  return "logical shift left";
        case m68k_lsr:                  return "logical shift right";
        case m68k_mac:                  return "multiply accumulate";
        case m68k_mov3q:                return "copy 3-bit data quick";
        case m68k_movclr:               return "copy from MAC ACC register and clear";
        case m68k_move:                 return "copy data";
        case m68k_move_acc:             return "copy MAC ACC register";
        case m68k_move_accext:          return "copy MAC ACCext register";
        case m68k_move_ccr:             return "copy condition code register";
        case m68k_move_macsr:           return "copy MAC status register";
        case m68k_move_mask:            return "copy MAC MASK register";
        case m68k_move_sr:              return "copy status register";
        case m68k_move16:               return "copy 16-byte block";
        case m68k_movea:                return "copy address";
        case m68k_movec:                return "copy control register";
        case m68k_movem:                return "copy multiple registers";
        case m68k_movep:                return "copy peripheral data";
        case m68k_moveq:                return "copy quick";
        case m68k_msac:                 return "multiply subtract";
        case m68k_muls:                 return "signed multiply";
        case m68k_mulu:                 return "unsigned multiply";
        case m68k_mvs:                  return "copy with sign extend";
        case m68k_mvz:                  return "copy with zero fill";
        case m68k_nbcd:                 return "negate decimal with extend";
        case m68k_neg:                  return "negate";
        case m68k_negx:                 return "negate with extend";
        case m68k_nop:                  return "no operation";
        case m68k_not:                  return "logical complement";
        case m68k_or:                   return "inclusive-OR logical";
        case m68k_ori:                  return "inclusive-OR immediate";
        case m68k_pack:                 return "pack";
        case m68k_pea:                  return "push effective address";
        // m68k_pulse,                  generate unique processor status
        // m68k_rems,                   signed divide remainder -- see divs instead
        // m68k_remu,                   unsigned divide remainder -- see divu instead
        case m68k_rol:                  return "rotate left without extend";
        case m68k_ror:                  return "rotate right without extend";
        case m68k_roxl:                 return "rotate left with extend";
        case m68k_roxr:                 return "rotate right with extend";
        case m68k_rtd:                  return "return and deallocate";
        case m68k_rtm:                  return "return from module";
        // m68k_rte,                    return from exception
        case m68k_rtr:                  return "return and restore condition codes";
        case m68k_rts:                  return "return from subroutine";
        // m68k_sats,                   signed saturate
        case m68k_sbcd:                 return "subtract decimal with extend";
        case m68k_st:                   return "set if true";
        case m68k_sf:                   return "set if false";
        case m68k_shi:                  return "set if high";
        case m68k_sls:                  return "set if lower or same";
        case m68k_scc:                  return "set if carry clear (HS)";
        case m68k_scs:                  return "set if carry set (LO)";
        case m68k_sne:                  return "set if not equal";
        case m68k_seq:                  return "set if equal";
        case m68k_svc:                  return "set if overflow clear";
        case m68k_svs:                  return "set if overflow set";
        case m68k_spl:                  return "set if plus";
        case m68k_smi:                  return "set if minus";
        case m68k_sge:                  return "set if greater or equal";
        case m68k_slt:                  return "set if less than";
        case m68k_sgt:                  return "set if greater than";
        case m68k_sle:                  return "set if less or equal";
        // m68k_stop,
        case m68k_sub:                  return "subtract";
        case m68k_suba:                 return "subtract address";
        case m68k_subi:                 return "subtract immediate";
        case m68k_subq:                 return "subtract quick";
        case m68k_subx:                 return "subtract extended";
        case m68k_swap:                 return "swap register halves";
        case m68k_tas:                  return "test and set";
        // m68k_tpf,                    trap false (no operation)
        case m68k_trap:                 return "trap";
        case m68k_trapt:                return "trap if true";
        case m68k_trapf:                return "trap if false";
        case m68k_traphi:               return "trap if high";
        case m68k_trapls:               return "trap if lower or same";
        case m68k_trapcc:               return "trap if carry clear (HS)";
        case m68k_trapcs:               return "trap if carry set (LO)";
        case m68k_trapne:               return "trap if not equal";
        case m68k_trapeq:               return "trap if equal";
        case m68k_trapvc:               return "trap if overflow clear";
        case m68k_trapvs:               return "trap if overflow set";
        case m68k_trappl:               return "trap if plus";
        case m68k_trapmi:               return "trap if minus";
        case m68k_trapge:               return "trap if greater or equal";
        case m68k_traplt:               return "trap if less than";
        case m68k_trapgt:               return "trap if greater than";
        case m68k_traple:               return "trap if less or equal";
        case m68k_trapv:                return "trap on overflow";
        case m68k_tst:                  return "test";
        case m68k_unlk:                 return "unlink";
        case m68k_unpk:                 return "unpack binary coded decimal";
        // m68k_wddata,                 write to debug data
        // m68k_wdebug,
        case m68k_last_instruction:     ASSERT_not_reachable("not a valid m68k instruction kind");
    }
    ASSERT_not_reachable("invalid m68k instruction kind: " + StringUtility::numberToString(get_kind()));
}

#endif
