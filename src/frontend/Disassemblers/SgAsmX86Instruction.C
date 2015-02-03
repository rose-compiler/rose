/* SgAsmX86Instruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "SymbolicSemantics.h"
#include "SymbolicSemantics2.h"
#include "PartialSymbolicSemantics.h"
#include "DispatcherX86.h"
#include "YicesSolver.h"
#include "Disassembler.h"
#include "Diagnostics.h"

using namespace rose;                                   // temporary until this lives in "rose"
using namespace rose::Diagnostics;

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
    // function call.  FIXME: The implementation here assumes a 32-bit machine. [Robb P. Matzke 2013-09-06]
    if (interp && insns.size()<=EXECUTION_LIMIT) {
        using namespace rose::BinaryAnalysis;
        using namespace rose::BinaryAnalysis::InstructionSemantics2;
        using namespace rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
        const InstructionMap &imap = interp->get_instruction_map();
        const RegisterDictionary *regdict = RegisterDictionary::dictionary_for_isa(interp);
        SMTSolver *solver = NULL; // using a solver would be more accurate, but slower
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        DispatcherX86Ptr dispatcher = DispatcherX86::instance(ops);
        SValuePtr orig_esp = SValue::promote(ops->readRegister(dispatcher->REG_ESP));
        try {
            for (size_t i=0; i<insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // If the next instruction address is concrete but does not point to a function entry point, then this is not a call.
        SValuePtr eip = SValue::promote(ops->readRegister(dispatcher->REG_EIP));
        if (eip->is_number()) {
            rose_addr_t target_va = eip->get_number();
            SgAsmFunction *target_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(target_va, NULL));
            if (!target_func || target_va!=target_func->get_entry_va())
                return false;
        }

        // If nothing was pushed onto the stack, then this isn't a function call.
        SValuePtr esp = SValue::promote(ops->readRegister(dispatcher->REG_ESP));
        SValuePtr stack_delta = SValue::promote(ops->add(esp, ops->negate(orig_esp)));
        SValuePtr stack_delta_sign = SValue::promote(ops->extract(stack_delta, 31, 32));
        if (stack_delta_sign->is_number() && 0==stack_delta_sign->get_number())
            return false;

        // If the top of the stack does not contain a concrete value or the top of the stack does not point to an instruction
        // in this basic block's function, then this is not a function call.
        SValuePtr top = SValue::promote(ops->readMemory(dispatcher->REG_SS, esp, esp->undefined_(32), esp->boolean_(true)));
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
        using namespace rose::BinaryAnalysis;
        using namespace rose::BinaryAnalysis::InstructionSemantics2;
        using namespace rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics;
        const RegisterDictionary *regdict = RegisterDictionary::dictionary_pentium4();
        SMTSolver *solver = NULL; // using a solver would be more accurate, but slower
        BaseSemantics::RiscOperatorsPtr ops = RiscOperators::instance(regdict, solver);
        DispatcherX86Ptr dispatcher = DispatcherX86::instance(ops);
        try {
            for (size_t i=0; i<insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // Look at the top of the stack
        SValuePtr top = SValue::promote(ops->readMemory(dispatcher->REG_SS, ops->readRegister(dispatcher->REG_ESP),
                                                        ops->get_protoval()->undefined_(32),
                                                        ops->get_protoval()->boolean_(true)));
        if (top->is_number() && top->get_number() == last->get_address()+last->get_size()) {
            if (target) {
                SValuePtr eip = SValue::promote(ops->readRegister(dispatcher->REG_EIP));
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

/** Determines whether this instruction is the special x86 "unknown" instruction. */
bool
SgAsmX86Instruction::isUnknown() const
{
    return x86_unknown_instruction == get_kind();
}

/** Return control flow successors. See base class for full documentation. */
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

        case x86_ret:
        case x86_iret:
        case x86_int1:
        case x86_int3:
        case x86_into:
        case x86_rsm:
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

/** Return control flow successors. See base class for full documentation. */
BinaryAnalysis::Disassembler::AddressSet
SgAsmX86Instruction::getSuccessors(const std::vector<SgAsmInstruction*>& insns, bool *complete, const MemoryMap *initial_memory)
{
    using namespace rose::BinaryAnalysis::InstructionSemantics;
    Stream debug(mlog[DEBUG]);

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

#if 0
        /* Use the most robust semantic analysis available.  Warning: this can be very slow, especially when an SMT solver is
         * involved! */
# if defined(ROSE_YICES) || defined(ROSE_HAVE_LIBYICES)
        YicesSolver yices;
        if (yices.available_linkage() & YicesSolver::LM_LIBRARY) {
            yices.set_linkage(YicesSolver::LM_LIBRARY);
        } else {
            yices.set_linkage(YicesSolver::LM_EXECUTABLE);
        }
        SMTSolver *solver = &yices;
# else
        SMTSolver *solver = NULL;
# endif
        if (debug && solver)
            solver->set_debug(stderr);
        typedef SymbolicSemantics::Policy<> Policy;
        typedef SymbolicSemantics::ValueType<32> RegisterType;
        typedef X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> Semantics;
        Policy policy(solver);
#else
        typedef PartialSymbolicSemantics::Policy<> Policy;
        typedef PartialSymbolicSemantics::ValueType<32> RegisterType;
        typedef X86InstructionSemantics<Policy, PartialSymbolicSemantics::ValueType> Semantics;
        Policy policy;
        policy.set_map(initial_memory);
#endif
        try {
            Semantics semantics(policy);
            for (size_t i=0; i<insns.size(); i++) {
                SgAsmX86Instruction* insn = isSgAsmX86Instruction(insns[i]);
                semantics.processInstruction(insn);
                if (debug) {
                    debug << "  state after " <<unparseInstructionWithAddress(insn) <<"\n"
                          <<policy.get_state();
                }
            }
            const RegisterType &newip = policy.get_ip();
            if (newip.is_known()) {
                successors.clear();
                successors.insert(newip.known_value());
                *complete = true; /*this is the complete set of successors*/
            }
        } catch(const Semantics::Exception& e) {
            /* Abandon entire basic block if we hit an instruction that's not implemented. */
            debug <<e <<"\n";
        } catch(const Policy::Exception& e) {
            /* Abandon entire basic block if the semantics policy cannot handle the instruction. */
            debug <<e <<"\n";
        }
    }

    if (debug) {
        debug <<"  successors:";
        for (BinaryAnalysis::Disassembler::AddressSet::const_iterator si=successors.begin(); si!=successors.end(); ++si)
            debug <<" " <<StringUtility::addrToString(*si);
        debug <<(*complete?"":"...") <<"\n";
    }

    return successors;
}

/** Determines whether a single instruction has an effect other than advancing the instruction pointer. Instructions that have
 *  no effect are called "no-ops".  The NOP instruction is an example of a no-op, but there are others also. The following
 *  information is largely from Cory Cohen at CERT. In the discussion that follows, we are careful to distinguish between NOP
 *  (the mneumonic for instructions 90, and 0f1f) and "no-op" (any instruction whose only effect is to advance the instruction
 *  pointer).
 *
 *  \code
 *  Opcode bytes         Intel assembly syntax
 *  -------------------- ---------------------- 
 *  90                   nop
 *
 *  89c0                 mov eax,eax            Intel's old recommended two-byte no-op was to
 *  89c9                 mov ecx,ecx            move a register to itself...  The second byte of these are mod/rm
 *  89d2                 mov edx,edx            bytes, and can generally be substituded wherever you see 0xc0 in
 *  89db                 mov ebx,ebx            subsequent examples.
 *  89e4                 mov esp,esp
 *  89ed                 mov ebp,ebp
 *  89f6                 mov esi,esi
 *  89ff                 mov edi,edi
 *
 *  88c0                 mov al,al              The above are also available in 8-bit form with a leading byte of 0x88
 *  6689c0               mov ax,ax              and with an operand size prefix (0x66).
 *
 *  66666689c0           mov ax,ax              The prefixes can be repeated. One source seemed to imply that up to
 *                                              three are reliably supported by the actual Intel processors. ROSE supports
 *                                              any number up to the maximum instruction size (varies by mode).
 *
 *  6688c0               mov al,al              The operand size prefix can even be nonsensical.
 *
 *  8ac0                 mov al,al              These are also presumabely no-ops.  As with most instructions, these will
 *  8bc0                 mov eax,eax            accept operand size prefixes as well.
 *
 *  f090                 lock nop               Most of these instructions will accept a lock prefix as well, which does
 *  f0f090               lock nop               not materially affect the result. As before, they can occur repeatedly, and
 *  f066f090             lock nop               even in wacky combinations.
 *  f066f06666f0f066f090 lock nop
 *  
 *  f290                 repne nop              Cory Cohen strongly suspects that the other instruction prefixes are
 *  f390                 rep nop                ignored as well, although to be complete, we might want to conduct a few
 *  2690                 es nop                 tests into the behavior of common processors.
 *  2e90                 cs nop
 *  3690                 ss nop
 *  3e90                 ds nop
 *  6490                 fs nop
 *  6590                 gs nop
 *  6790                 nop
 *  
 *  8d00                 lea eax,[eax]          Intel's old recommendation for larger no-ops was to use the LEA
 *  8d09                 lea ecx,[ecx]          instruction in various dereferencing modes.
 *  8d12                 lea edx,[edx]
 *  8d1b                 lea ebx,[ebx]
 *  8d36                 lea esi,[esi]
 *  8d3f                 lea edi,[edi]
 *  
 *  8d4000               lea eax,[eax+0x0]
 *  8d4900               lea ecx,[ecx+0x0]
 *  8d5200               lea edx,[edx+0x0]
 *  8d5b00               lea ebx,[ebx+0x0]
 *  8d7600               lea esi,[esi+0x0]
 *  8d7f00               lea edi,[edi+0x0]
 *  
 *  8d8000000000         lea eax,[eax+0x0]      This last block is really the [reg*0x1+0x0] dereferencing mode.
 *  8d8900000000         lea ecx,[ecx+0x0]
 *  8d9200000000         lea edx,[edx+0x0]
 *  8d9b00000000         lea ebx,[ebx+0x0]
 *  8db600000000         lea esi,[esi+0x0]
 *  8dbf00000000         lea edi,[edi+0x0]
 *
 *  8d0420               lea eax,[eax]          Then there's funky equivalents involving SIB bytes.
 *  8d0c21               lea ecx,[ecx]
 *  8d1422               lea edx,[edx]
 *  8d1c23               lea ebx,[ebx]
 *  8d2424               lea esp,[esp]
 *  8d3426               lea esi,[esi]
 *  8d3c27               lea edi,[edi]
 *  
 *  8d442000             lea eax,[eax+0x0]
 *  8d4c2100             lea ecx,[ecx+0x0]
 *  8d542200             lea edx,[edx+0x0]
 *  8d5c2300             lea ebx,[ebx+0x0]
 *  8d642400             lea esp,[esp+0x0]
 *  8d742600             lea esi,[esi+0x0]
 *  8d7c2700             lea edi,[edi+0x0]
 *  
 *  8d842000000000       lea eax,[eax+0x0]
 *  8d8c2100000000       lea ecx,[ecx+0x0]
 *  8d942200000000       lea edx,[edx+0x0]
 *  8d9c2300000000       lea ebx,[ebx+0x0]
 *  8da42400000000       lea esp,[esp+0x0]
 *  8db42600000000       lea esi,[esi+0x0]
 *  8dbc2700000000       lea edi,[edi+0x0]
 *  
 *  8d2c2d00000000       lea ebp,[ebp+0x0]      The EBP variants don't exactly follow the pattern above.
 *  8d6c2500             lea ebp,[ebp+0x0]
 *  8dac2500000000       lea ebp,[ebp+0x0]
 *
 *  0f1f00               nop [eax]              P4+ adds the 0f1f instruction. Each of these can be prefixed with the
 *  0f1f4000             nop [eax+0x0]          0x66 operand size prefix. In fact, Intel recommends doing this now for the
 *  0f1f440000           nop [eax+0x0]          optimally efficient 6- and 9-byte sequences.
 *  0f1f8000000000       nop [eax+0x0]
 *  0f1f840000000000     nop [eax+0x0]
 *
 *  0f0dxx               nop [xxx]              The latest version of the manual implies that this sequence is also
 *                                              reserved for NOP, although I can find almost no references to it except
 *                                              in the latest instruction manual on page A-13 of volume 2B. It's also mentioned
 *                                              on x86asm.net. [CORY 2010-04]
 *                                              
 *  d9d0                 fnop                   These aren't really no-ops on the chip, but are no-ops from the program's
 *  9b                   wait                   perspective. Most of these instructions are related to improving cache
 *  0f08                 invd                   efficiency and performance, but otherwise do not affect the program
 *  0f09                 wbinvd                 behavior.
 *  0f01c9               mwait
 *  0f0138               invlpg [eax]
 *  0f01bf00000000       invlpg [edi+0x0]       and more...
 *  0f18 /0              prefetchnta [xxx]
 *  0f18 /1              prefetch0 [xxx]
 *  0f18 /2              prefetch1 [xxx]
 *  0f18 /3              prefetch2 [xxx]
 *  0fae /5              lfence [xxx]
 *  0fae /6              mfence [xxx]
 *  0fae /7              sfence [xxx]
 *
 *  0f18xx through 0f1exx                       This opcode rante is officially undefined but is probably reserved for
 *                                              no-ops as well.  Any instructions encountered in this range are probably
 *                                              consequences of bad code and should be ingored.
 *                                              
 *  JMP, Jcc, PUSH/RET, etc.                    Branches are considered no-ops if they can be proven to always branch
 *                                              to the fall-through address.
 *  \endcode
 */
bool
SgAsmX86Instruction::hasEffect()
{
    std::vector<SgAsmInstruction*> sequence;
    sequence.push_back(this);
    return hasEffect(sequence, false);
}

/** Determines whether a sequence of instructions has an effect besides advancing the flow of control.
 *
 *  The specified list of instructions should be (part of) a basic block and the instructions are given in the order they would
 *  be executed. This function does not check that the instructions are actualy executed sequentially as specified, it just
 *  evaluates the machine state as if they had been executed sequentially.  This can be useful when a basic block was built
 *  from control-flow information that is not available to this function.
 *
 *  An empty sequence of instructions has no effect (i.e., return value is false).
 *
 *  If the final instruction of the sequence results in an undetermined instruction pointer then the sequence is considered to
 *  have an effect (this situation usually results from a conditional jump).  If the final instruction results in a known
 *  value for the instruction pointer, and the known value is the fall-through address then the final instruction is
 *  considered to have no effect.  If the final instruction results in a known instruction pointer that is not the
 *  fall-through address then the final instruction has an effect only if allow_branch is false.
 *
 *  If relax_stack_semantics is true then each time the stack pointer is increased the memory locations below
 *  the new stack value are discarded.  Typically, well behaved programs do not read stack data that is below the stack
 *  pointer.
 *
 *  "this" is only used to select the virtual function; the operation is performed on the specified instruction vector.
 */
bool
SgAsmX86Instruction::hasEffect(const std::vector<SgAsmInstruction*>& insns, bool allow_branch/*false*/, 
                               bool relax_stack_semantics/*false*/)
{
    using namespace rose::BinaryAnalysis::InstructionSemantics;

    if (insns.empty()) return false;

    typedef PartialSymbolicSemantics::Policy<> Policy;
    typedef X86InstructionSemantics<Policy, PartialSymbolicSemantics::ValueType> Semantics;
    Policy policy;
    Semantics semantics(policy);
    if (relax_stack_semantics) policy.set_discard_popped_memory(true);
    try {
        for (std::vector<SgAsmInstruction*>::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(*ii);
            if (!insn) return true;
            semantics.processInstruction(insn);
            if (!policy.get_ip().is_known()) return true;
        }
    } catch (const Semantics::Exception&) {
        return true;
    } catch (const Policy::Exception&) {
        return true;
    }

    /* If the final instruction pointer is not the fall-through address of the final instruction then return true. In other
     * words, a sequence ending with a JMP (for instance) has an effect, but an internal JMP has no effect.  This is to
     * support instruction sequences from non-contiguous basic blocks. */
    ROSE_ASSERT(policy.get_ip().is_known());
    if (!allow_branch && policy.get_ip().known_value()!=insns.back()->get_address() + insns.back()->get_size())
        return true;

    /* Instructions have an effect if the state changed.  We want the comparison to be independent of the instruction pointer,
     * so we'll set the IP of both the initial and final states to the same (unknown) value. */ 
    policy.get_orig_state().registers.ip = policy.get_state().registers.ip = PartialSymbolicSemantics::ValueType<32>();
    return !policy.equal_states(policy.get_orig_state(), policy.get_state());
}

/** Determines what subsequences of an instruction sequence have no cumulative effect.  The return value is a vector of pairs
 *  where each pair is the starting index and length of subsequence.  The algorithm we use is to compute the machine state
 *  after each instruction and then look for pairs of states that are identical except for the instruction pointer.  Like the
 *  vector version of hasEffect(), the control-flow from the final instruction is treated specially depending on the
 *  allow_branch value, which defaults to false.
 *
 *  It is more efficient to call this function to find sequences than to call the vector version of hasEffect() with various
 *  vectors. First, one doesn't have to construct all the different subsequences; second, the semantic analysis is performed
 *  only one time.
 *
 *  "this" is only used to select the virtual function; the operation is performed over the specified instruction vector. */
std::vector< std::pair< size_t, size_t > >
SgAsmX86Instruction::findNoopSubsequences(const std::vector<SgAsmInstruction*>& insns, bool allow_branch/*false*/, 
                                          bool relax_stack_semantics/*false*/)
{
    using namespace rose::BinaryAnalysis::InstructionSemantics;

    static const bool verbose = false;

    if (verbose) std::cerr <<"findNoopSubsequences:\n";
    std::vector< std::pair <size_t/*starting insn index*/, size_t/*num. insns*/> > retval;

    typedef PartialSymbolicSemantics::Policy<> Policy;
    typedef X86InstructionSemantics<Policy, PartialSymbolicSemantics::ValueType> Semantics;
    Policy policy;
    if (relax_stack_semantics) policy.set_discard_popped_memory(true);
    Semantics semantics(policy);

    /* When comparing states, we don't want to compare the instruction pointers. Therefore, we'll change the IP value of
     * each state to be the same. */
    const PartialSymbolicSemantics::ValueType<32> common_ip;
    
    /* Save the state before and after each instruction.  states[i] is the state before insn[i] and states[i+1] is the state
     * after insn[i]. */
    std::vector<PartialSymbolicSemantics::State<PartialSymbolicSemantics::ValueType> > state;
    state.push_back(policy.get_state());
    state.back().registers.ip = common_ip;
    try {
        for (std::vector<SgAsmInstruction*>::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(*ii);
            if (verbose)
                std::cerr <<"  insn #" <<(state.size()-1)
                          <<" " <<(insn ? unparseInstructionWithAddress(insn) : "<none>") <<"\n";
            if (!insn) return retval;
            semantics.processInstruction(insn);
            state.push_back(policy.get_state());
            if (verbose) std::cerr <<"  state:\n" <<policy.get_state();
        }
    } catch (const Semantics::Exception&) {
        /* Perhaps we can find at least a few no-op subsequences... */
    } catch (const Policy::Exception&) {
        /* Perhaps we can find at least a few no-op subsequences... */
    }

    /* If the last instruction resulted in indeterminant instruction pointer then discard it from the list of states because
     * it has an effect (it's probably a conditional jump).  It's up to the caller whether a final instruction that
     * unconditionally branches has an effect. */
    if (!policy.get_ip().is_known()) {
        state.pop_back();
    } else if (!allow_branch &&
               policy.get_ip().known_value()!=insns.back()->get_address() + insns.back()->get_size()) {
        state.pop_back();
    }

    /* Change the IP register so its the same for all states so it doesn't contribute to state differences. */
    const size_t nstates = state.size();
    for (size_t i=0; i<nstates; i++)
        state[i].registers.ip = common_ip;

    /* Find pairs of equivalent states. */
    if (verbose) std::cerr <<"  number of states: " <<nstates <<"\n";
    for (size_t i=0; i<nstates-1; i++) {
        for (size_t j=i+1; j<nstates; j++) {
            if (policy.equal_states(state[i], state[j])) {
                if (verbose) std::cerr <<"  at instruction #"<<i <<": no-op of length " <<(j-i) <<"\n";
                retval.push_back(std::make_pair(i, j-i));
            }
        }
    }

    return retval;
}
