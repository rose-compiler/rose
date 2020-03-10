/* SgAsmX86Instruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include "SymbolicSemantics2.h"
#include "PartialSymbolicSemantics2.h"
#include "CommandLine.h"
#include "DispatcherX86.h"
#include "Disassembler.h"
#include "Diagnostics.h"
#include "x86InstructionProperties.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmX86Instruction::get_anyKind() const {
    return p_kind;
}

// see base class
bool
SgAsmX86Instruction::terminatesBasicBlock() {
    if (get_kind()==x86_unknown_instruction)
        return true;
    return x86InstructionIsControlTransfer(this);
}

// class method
X86InstructionSize
SgAsmX86Instruction::instructionSizeForWidth(size_t nbits) {
    switch (nbits) {
        case 16: return x86_insnsize_16;
        case 32: return x86_insnsize_32;
        case 64: return x86_insnsize_64;
    }
    ASSERT_not_reachable("invalid width: " + StringUtility::numberToString(nbits));
}

// class method
size_t
SgAsmX86Instruction::widthForInstructionSize(X86InstructionSize isize) {
    switch (isize) {
        case x86_insnsize_16: return 16;
        case x86_insnsize_32: return 32;
        case x86_insnsize_64: return 64;
        default: ASSERT_not_reachable("invalid x86 instruction size");
    }
}

// class method
const RegisterDictionary*
SgAsmX86Instruction::registersForInstructionSize(X86InstructionSize isize) {
    switch (isize) {
        case x86_insnsize_16: return RegisterDictionary::dictionary_i286();
        case x86_insnsize_32: return RegisterDictionary::dictionary_pentium4();
        case x86_insnsize_64: return RegisterDictionary::dictionary_amd64();
        default: ASSERT_not_reachable("invalid x86 instruction size");
    }
}

// class method
const RegisterDictionary*
SgAsmX86Instruction::registersForWidth(size_t nbits) {
    return registersForInstructionSize(instructionSizeForWidth(nbits));
}

// see base class
bool
SgAsmX86Instruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target, rose_addr_t *return_va)
{
    if (insns.empty())
        return false;
    SgAsmX86Instruction *last = isSgAsmX86Instruction(insns.back());
    if (!last)
        return false;

    // Quick method based only on the kind of instruction
    if (x86_call==last->get_kind() || x86_farcall==last->get_kind()) {
        last->getBranchTarget(target);
        if (return_va)
            *return_va = last->get_address() + last->get_size();
        return true;
    }

    return false;
}

// see base class
bool
SgAsmX86Instruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target, rose_addr_t *return_va)
{
    if (isFunctionCallFast(insns, target, return_va))
        return true;

    // The following stuff works only if we have a relatively complete AST.
    static const size_t EXECUTION_LIMIT = 10; // max size of basic blocks for expensive analyses
    if (insns.empty())
        return false;
    SgAsmX86Instruction *last = isSgAsmX86Instruction(insns.back());
    if (!last)
        return false;
    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(last);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(func);

    // Slow method: Emulate the instructions and then look at the EIP and stack.  If the EIP points outside the current
    // function and the top of the stack holds an address of an instruction within the current function, then this must be a
    // function call.
    if (interp && insns.size()<=EXECUTION_LIMIT) {
        using namespace Rose::BinaryAnalysis;
        using namespace Rose::BinaryAnalysis::InstructionSemantics2;
        using namespace Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
        const InstructionMap &imap = interp->get_instruction_map();
        const RegisterDictionary *regdict = RegisterDictionary::dictionary_for_isa(interp);
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        ASSERT_not_null(ops);
        const RegisterDescriptor SP = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
        DispatcherX86Ptr dispatcher = DispatcherX86::instance(ops, SP.nBits());
        SValuePtr orig_esp = SValue::promote(ops->peekRegister(dispatcher->REG_anySP));
        try {
            for (size_t i=0; i<insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // If the next instruction address is concrete but does not point to a function entry point, then this is not a call.
        SValuePtr eip = SValue::promote(ops->peekRegister(dispatcher->REG_anyIP));
        if (eip->is_number()) {
            rose_addr_t target_va = eip->get_number();
            SgAsmFunction *target_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(target_va, NULL));
            if (!target_func || target_va!=target_func->get_entry_va())
                return false;
        }

        // If nothing was pushed onto the stack, then this isn't a function call.
        const size_t spWidth = dispatcher->REG_anySP.nBits();
        SValuePtr esp = SValue::promote(ops->peekRegister(dispatcher->REG_anySP));
        SValuePtr stack_delta = SValue::promote(ops->add(esp, ops->negate(orig_esp)));
        SValuePtr stack_delta_sign = SValue::promote(ops->extract(stack_delta, spWidth-1, spWidth));
        if (stack_delta_sign->is_number() && 0==stack_delta_sign->get_number())
            return false;

        // If the top of the stack does not contain a concrete value or the top of the stack does not point to an instruction
        // in this basic block's function, then this is not a function call.
        const size_t ipWidth = dispatcher->REG_anyIP.nBits();
        SValuePtr top = SValue::promote(ops->peekMemory(dispatcher->REG_SS, esp, esp->undefined_(ipWidth)));
        if (top->is_number()) {
            rose_addr_t va = top->get_number();
            SgAsmFunction *return_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(va, NULL));
            if (!return_func || return_func!=func) {
                return false;
            }
        } else {
            return false;
        }

        // Since EIP might point to a function entry address and since the top of the stack contains a pointer to an
        // instruction in this function, we assume that this is a function call.
        if (target && eip->is_number())
            *target = eip->get_number();
        if (return_va && top->is_number())
            *return_va = top->get_number();
        return true;
    }

    // Similar to the above method, but works when all we have is the basic block (e.g., this case gets hit quite a bit from
    // the Partitioner).  Returns true if, after executing the basic block, the top of the stack contains the fall-through
    // address of the basic block. We depend on our caller to figure out if EIP is reasonably a function entry address.
    if (!interp && insns.size()<=EXECUTION_LIMIT) {
        using namespace Rose::BinaryAnalysis;
        using namespace Rose::BinaryAnalysis::InstructionSemantics2;
        using namespace Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        SgAsmX86Instruction *x86insn = isSgAsmX86Instruction(insns.front());
        ASSERT_not_null(x86insn);
#if 1 // [Robb P. Matzke 2015-03-03]: FIXME[Robb P. Matzke 2015-03-03]: not ready yet; x86-64 semantics still under construction
        if (x86insn->get_addressSize() != x86_insnsize_32)
            return false;
#endif
        const RegisterDictionary *regdict = registersForInstructionSize(x86insn->get_addressSize());
        const RegisterDescriptor SP = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        DispatcherX86Ptr dispatcher = DispatcherX86::instance(ops, SP.nBits());
        try {
            for (size_t i=0; i<insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // Look at the top of the stack
        const size_t ipWidth = dispatcher->REG_anyIP.nBits();
        SValuePtr top = SValue::promote(ops->peekMemory(dispatcher->REG_SS, ops->peekRegister(SP),
                                                        ops->protoval()->undefined_(ipWidth)));
        if (top->is_number() && top->get_number() == last->get_address()+last->get_size()) {
            if (target) {
                SValuePtr eip = SValue::promote(ops->peekRegister(dispatcher->REG_anyIP));
                if (eip->is_number())
                    *target = eip->get_number();
            }
            if (return_va)
                *return_va = top->get_number();
            return true;
        }
    }

    return false;
}

// See base class.
bool
SgAsmX86Instruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) {
    if (insns.empty())
        return false;
    SgAsmX86Instruction *last_insn = isSgAsmX86Instruction(insns.back());
    if (!last_insn)
        return false;
    if (last_insn->get_kind()==x86_ret || last_insn->get_kind()==x86_retf)
        return true;
    return false;
}

// See base class.
bool
SgAsmX86Instruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) {
    return isFunctionReturnFast(insns);
}

bool
SgAsmX86Instruction::isUnknown() const
{
    return x86_unknown_instruction == get_kind();
}

BinaryAnalysis::Disassembler::AddressSet
SgAsmX86Instruction::getSuccessors(bool *complete) {
    BinaryAnalysis::Disassembler::AddressSet retval;
    *complete = true; /*assume true and prove otherwise*/

    switch (get_kind()) {
        case x86_call:
        case x86_farcall:
        case x86_jmp:
        case x86_farjmp: {
            /* Unconditional branch to operand-specified address. We cannot assume that a CALL instruction returns to the
             * fall-through address. */
            rose_addr_t va;
            if (getBranchTarget(&va)) {
                retval.insert(va);
            } else {
                *complete = false;
            }
            break;
        }

        case x86_ja:
        case x86_jae:
        case x86_jb:
        case x86_jbe:
        case x86_jcxz:
        case x86_jecxz:
        case x86_jrcxz:
        case x86_je:
        case x86_jg:
        case x86_jge:
        case x86_jl:
        case x86_jle:
        case x86_jne:
        case x86_jno:
        case x86_jns:
        case x86_jo:
        case x86_jpe:
        case x86_jpo:
        case x86_js:
        case x86_loop:
        case x86_loopnz:
        case x86_loopz: {
            /* Conditional branches to operand-specified address */
            rose_addr_t va;
            if (getBranchTarget(&va)) {
                retval.insert(va);
            } else {
                *complete = false;
            }
            retval.insert(get_address() + get_size());
            break;
        }

        case x86_int:                                   // assumes interrupts return
        case x86_int1:
        case x86_int3:
        case x86_into:
        case x86_syscall: {
            retval.insert(get_address() + get_size());  // probable return point
            *complete = false;
            break;
        }
            
        case x86_ret:
        case x86_iret:
        case x86_rsm:
        case x86_sysret:
        case x86_ud2:
        case x86_retf: {
            /* Unconditional branch to run-time specified address */
            *complete = false;
            break;
        }

        case x86_hlt: {
            /* Instructions having no successor. */
            break;
        }

        case x86_unknown_instruction: {
            /* Instructions having unknown successors */
            *complete = false;
            break;
        }

        default: {
            /* Instructions that always fall through to the next instruction */
            retval.insert(get_address() + get_size());
            break;
        }
    }
    return retval;
}

bool
SgAsmX86Instruction::getBranchTarget(rose_addr_t *target) {
    // Treats far destinations as "unknown"
    switch (get_kind()) {
        case x86_call:
        case x86_farcall:
        case x86_jmp:
        case x86_ja:
        case x86_jae:
        case x86_jb:
        case x86_jbe:
        case x86_jcxz:
        case x86_jecxz:
        case x86_jrcxz:
        case x86_je:
        case x86_jg:
        case x86_jge:
        case x86_jl:
        case x86_jle:
        case x86_jne:
        case x86_jno:
        case x86_jns:
        case x86_jo:
        case x86_jpe:
        case x86_jpo:
        case x86_js:
        case x86_loop:
        case x86_loopnz:
        case x86_loopz: {
            const SgAsmExpressionPtrList &args = get_operandList()->get_operands();
            if (args.size()!=1)
                return false;
            SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[0]);
            if (!ival)
                return false;
            if (target)
                *target = ival->get_absoluteValue();
            return true;
        }
        default:
            return false; // do not modify *target
    }
}

BinaryAnalysis::Disassembler::AddressSet
SgAsmX86Instruction::getSuccessors(const std::vector<SgAsmInstruction*>& insns, bool *complete,
                                   const MemoryMap::Ptr &initial_memory)
{
    Stream debug(mlog[DEBUG]);
    using namespace Rose::BinaryAnalysis::InstructionSemantics2;

    if (debug) {
        debug <<"SgAsmX86Instruction::getSuccessors(" <<StringUtility::addrToString(insns.front()->get_address())
              <<" for " <<insns.size() <<" instruction" <<(1==insns.size()?"":"s") <<"):" <<"\n";
    }

    BinaryAnalysis::Disassembler::AddressSet successors = SgAsmInstruction::getSuccessors(insns, complete);

    /* If we couldn't determine all the successors, or a cursory analysis couldn't narrow it down to a single successor then
     * we'll do a more thorough analysis now. In the case where the cursory analysis returned a complete set containing two
     * successors, a thorough analysis might be able to narrow it down to a single successor. We should not make special
     * assumptions about CALL and FARCALL instructions -- their only successor is the specified address operand. */
    if (!*complete || successors.size()>1) {
        const RegisterDictionary *regdict;
        if (SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(this)) {
            regdict = RegisterDictionary::dictionary_for_isa(interp);
        } else {
            switch (get_baseSize()) {
                case x86_insnsize_16:
                    regdict = RegisterDictionary::dictionary_i286();
                    break;
                case x86_insnsize_32:
                    regdict = RegisterDictionary::dictionary_pentium4();
                    break;
                case x86_insnsize_64:
                    regdict = RegisterDictionary::dictionary_amd64();
                    break;
                default:
                    ASSERT_not_reachable("invalid x86 instruction size");
            }
        }
        const RegisterDescriptor IP = regdict->findLargestRegister(x86_regclass_ip, 0);
        PartialSymbolicSemantics::RiscOperatorsPtr ops = PartialSymbolicSemantics::RiscOperators::instance(regdict);
        ops->set_memory_map(initial_memory);
        BaseSemantics::DispatcherPtr cpu = DispatcherX86::instance(ops, IP.nBits(), regdict);

        try {
            BOOST_FOREACH (SgAsmInstruction *insn, insns) {
                cpu->processInstruction(insn);
                SAWYER_MESG(debug) <<"  state after " <<insn->toString() <<"\n" <<*ops;
            }
            BaseSemantics::SValuePtr ip = ops->peekRegister(IP);
            if (ip->is_number()) {
                successors.clear();
                successors.insert(ip->get_number());
                *complete = true;
            }
        } catch(const BaseSemantics::Exception &e) {
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

#endif
