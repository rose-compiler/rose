/* SgAsmM68kInstruction member definitions. Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any other *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "AsmUnparser_compat.h"
#include "Diagnostics.h"
#include "Disassembler.h"
#include "DispatcherM68k.h"
#include "PartialSymbolicSemantics2.h"
#include "SymbolicSemantics2.h"

using namespace rose;                                   // temporary until this lives in "rose"
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
        using namespace rose::BinaryAnalysis;
        using namespace rose::BinaryAnalysis::InstructionSemantics2;
        using namespace rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
        const InstructionMap &imap = interp->get_instruction_map();
        const RegisterDictionary *regdict = RegisterDictionary::dictionary_for_isa(interp);
        SMTSolver *solver = NULL; // using a solver would be more accurate, but slower
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        DispatcherM68kPtr dispatcher = DispatcherM68k::instance(ops, 32);
        SValuePtr orig_sp = SValue::promote(ops->readRegister(dispatcher->REG_A[7]));
        try {
            for (size_t i=0; i<insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // If the next instruction address is concrete but does not point to a function entry point, then this is not a call.
        SValuePtr ip = SValue::promote(ops->readRegister(dispatcher->REG_PC));
        if (ip->is_number()) {
            rose_addr_t target_va = ip->get_number();
            SgAsmFunction *target_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(target_va, NULL));
            if (!target_func || target_va!=target_func->get_entry_va())
                return false;
        }

        // If nothing was pushed onto the stack, then this isn't a function call.
        SValuePtr sp = SValue::promote(ops->readRegister(dispatcher->REG_A[7]));
        SValuePtr stack_delta = SValue::promote(ops->add(sp, ops->negate(orig_sp)));
        SValuePtr stack_delta_sign = SValue::promote(ops->extract(stack_delta, 31, 32));
        if (stack_delta_sign->is_number() && 0==stack_delta_sign->get_number())
            return false;

        // If the top of the stack does not contain a concrete value or the top of the stack does not point to an instruction
        // in this basic block's function, then this is not a function call.
        SValuePtr top = SValue::promote(ops->readMemory(RegisterDescriptor(), sp, sp->undefined_(32), sp->boolean_(true)));
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
        using namespace rose::BinaryAnalysis;
        using namespace rose::BinaryAnalysis::InstructionSemantics2;
        using namespace rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
        const RegisterDictionary *regdict = RegisterDictionary::dictionary_coldfire_emac();
        SMTSolver *solver = NULL; // using a solver would be more accurate, but slower
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        DispatcherM68kPtr dispatcher = DispatcherM68k::instance(ops, 32);
        try {
            for (size_t i=0; i<insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // Look at the top of the stack
        SValuePtr top = SValue::promote(ops->readMemory(RegisterDescriptor(), ops->readRegister(dispatcher->REG_A[7]),
                                                        ops->get_protoval()->undefined_(32),
                                                        ops->get_protoval()->boolean_(true)));
        if (top->is_number() && top->get_number() == last->get_address()+last->get_size()) {
            if (target_va) {
                SValuePtr ip = SValue::promote(ops->readRegister(dispatcher->REG_PC));
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

BinaryAnalysis::Disassembler::AddressSet
SgAsmM68kInstruction::getSuccessors(bool *complete)
{
    BinaryAnalysis::Disassembler::AddressSet retval;
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

BinaryAnalysis::Disassembler::AddressSet
SgAsmM68kInstruction::getSuccessors(const std::vector<SgAsmInstruction*>& insns, bool *complete, const MemoryMap *initial_memory)
{
    using namespace rose::BinaryAnalysis::InstructionSemantics2;
    Stream debug(mlog[DEBUG]);

    if (debug) {
        debug <<"SgAsmM68kInstruction::getSuccessors(" <<StringUtility::addrToString(insns.front()->get_address())
              <<" for " <<insns.size() <<" instruction" <<(1==insns.size()?"":"s") <<"):" <<"\n";
    }

    BinaryAnalysis::Disassembler::AddressSet successors = SgAsmInstruction::getSuccessors(insns, complete);

    // If we couldn't determine all the successors, or a cursory analysis couldn't narrow it down to a single successor then
    // we'll do a more thorough analysis now. In the case where the cursory analysis returned a complete set containing two
    // successors, a thorough analysis might be able to narrow it down to a single successor. We should not make special
    // assumptions about function call instructions -- their only successor is the specified address operand. */
    if (!*complete || successors.size()>1) {
        using namespace rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics;

        const RegisterDictionary *regdict = RegisterDictionary::dictionary_coldfire_emac();
        RiscOperatorsPtr ops = RiscOperators::instance(regdict);
        ops->set_memory_map(initial_memory);
        DispatcherM68kPtr dispatcher = DispatcherM68k::instance(ops, 32);
        
        try {
            for (size_t i=0; i<insns.size(); ++i) {
                dispatcher->processInstruction(insns[i]);
                if (debug)
                    debug << "  state after " <<unparseInstructionWithAddress(insns[i]) <<"\n" <<*ops;
            }
            SValuePtr ip = SValue::promote(ops->readRegister(dispatcher->REG_PC));
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
        BOOST_FOREACH (rose_addr_t va, successors)
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
