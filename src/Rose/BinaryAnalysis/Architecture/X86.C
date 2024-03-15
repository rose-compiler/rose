#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/X86.h>

#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/BinaryAnalysis/InstructionMap.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/PartialSymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/EngineBinary.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesX86.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>
#include <Rose/CommandLine/Parser.h>

#include <SgAsmX86Instruction.h>
#include <Cxx_GrammarDowncast.h>
#include <sageInterface.h>

#include <Sawyer/Message.h>
#include <boost/lexical_cast.hpp>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::string
registerPrefix(size_t bitsPerWord) {
    switch (bitsPerWord) {
        case 16:
            return "";
        case 32:
            return "e";
        case 64:
            return "r";
    }
    ASSERT_not_reachable("invalid number of bits per word");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

X86::X86(const std::string &name, size_t bytesPerWord)
    : Base(name, bytesPerWord, ByteOrder::ORDER_LSB) {}

X86::~X86() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction characteristics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
X86::isUnknown(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmX86Instruction(insn_);
    ASSERT_not_null(insn);
    return x86_unknown_instruction == insn->get_kind();
}

bool
X86::terminatesBasicBlock(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmX86Instruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
        case x86_unknown_instruction:
        case x86_call:
        case x86_ret:
        case x86_iret:
        case x86_farcall:
        case x86_farjmp:
        case x86_hlt:
        case x86_jmp:
        case x86_int:
        case x86_int1:
        case x86_int3:
        case x86_into:
        case x86_ja:
        case x86_jae:
        case x86_jb:
        case x86_jbe:
        case x86_jcxz:
        case x86_je:
        case x86_jecxz:
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
        case x86_jrcxz:
        case x86_js:
        case x86_loop:
        case x86_loopnz:
        case x86_loopz:
        case x86_retf:
        case x86_rsm:
        case x86_syscall:
        case x86_sysret:
        case x86_ud2:
            return true;
        default: return false;
    }
}

bool
X86::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) const {
    if (insns.empty())
        return false;
    auto last = isSgAsmX86Instruction(insns.back());
    ASSERT_not_null(last);

    // Quick method based only on the kind of instruction
    if (x86_call == last->get_kind() || x86_farcall == last->get_kind()) {
        if (target)
            branchTarget(last).assignTo(*target);
        if (return_va)
            *return_va = last->get_address() + last->get_size();
        return true;
    }

    return false;
}

bool
X86::isFunctionCallSlow(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) const {
    if (isFunctionCallFast(insns, target, return_va))
        return true;

    // The following stuff works only if we have a relatively complete AST.
    static const size_t EXECUTION_LIMIT = 10; // max size of basic blocks for expensive analyses
    if (insns.empty())
        return false;
    auto last = isSgAsmX86Instruction(insns.back());
    ASSERT_not_null(last);
    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(last);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(func);

    // Slow method: Emulate the instructions and then look at the EIP and stack.  If the EIP points outside the current function and
    // the top of the stack holds an address of an instruction within the current function, then this must be a function call.
    if (interp && insns.size() <= EXECUTION_LIMIT) {
        using namespace Rose::BinaryAnalysis;
        using namespace Rose::BinaryAnalysis::InstructionSemantics;
        using namespace Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;
        const InstructionMap &imap = interp->get_instructionMap();
        RegisterDictionary::Ptr regdict = registerDictionary();
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        BaseSemantics::RiscOperators::Ptr ops = RiscOperators::instanceFromRegisters(regdict, solver);
        ASSERT_not_null(ops);
        DispatcherX86::Ptr dispatcher = DispatcherX86::promote(newInstructionDispatcher(ops));
        SValue::Ptr orig_esp = SValue::promote(ops->peekRegister(dispatcher->REG_anySP));
        try {
            for (size_t i = 0; i < insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // If the next instruction address is concrete but does not point to a function entry point, then this is not a call.
        SValue::Ptr eip = SValue::promote(ops->peekRegister(dispatcher->REG_anyIP));
        if (auto target_va = eip->toUnsigned()) {
            SgAsmFunction *target_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(*target_va, NULL));
            if (!target_func || *target_va != target_func->get_entryVa())
                return false;
        }

        // If nothing was pushed onto the stack, then this isn't a function call.
        const size_t spWidth = dispatcher->REG_anySP.nBits();
        SValue::Ptr esp = SValue::promote(ops->peekRegister(dispatcher->REG_anySP));
        SValue::Ptr stack_delta = SValue::promote(ops->add(esp, ops->negate(orig_esp)));
        SValue::Ptr stack_delta_sign = SValue::promote(ops->extract(stack_delta, spWidth-1, spWidth));
        if (stack_delta_sign->isFalse())
            return false;

        // If the top of the stack does not contain a concrete value or the top of the stack does not point to an instruction in
        // this basic block's function, then this is not a function call.
        const size_t ipWidth = dispatcher->REG_anyIP.nBits();
        SValue::Ptr top = SValue::promote(ops->peekMemory(dispatcher->REG_SS, esp, esp->undefined_(ipWidth)));
        if (auto va = top->toUnsigned()) {
            SgAsmFunction *return_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(*va, NULL));
            if (!return_func || return_func!=func) {
                return false;
            }
        } else {
            return false;
        }

        // Since EIP might point to a function entry address and since the top of the stack contains a pointer to an instruction in
        // this function, we assume that this is a function call.
        if (target)
            eip->toUnsigned().assignTo(*target);
        if (return_va)
            top->toUnsigned().assignTo(*return_va);
        return true;
    }

    // Similar to the above method, but works when all we have is the basic block (e.g., this case gets hit quite a bit from the
    // Partitioner).  Returns true if, after executing the basic block, the top of the stack contains the fall-through address of
    // the basic block. We depend on our caller to figure out if EIP is reasonably a function entry address.
    if (!interp && insns.size() <= EXECUTION_LIMIT) {
        using namespace Rose::BinaryAnalysis;
        using namespace Rose::BinaryAnalysis::InstructionSemantics;
        using namespace Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        SgAsmX86Instruction *x86insn = isSgAsmX86Instruction(insns.front());
        ASSERT_not_null(x86insn);
#if 1 // [Robb P. Matzke 2015-03-03]: FIXME[Robb P. Matzke 2015-03-03]: not ready yet; x86-64 semantics still under construction
        if (x86insn->get_addressSize() != x86_insnsize_32)
            return false;
#endif
        RegisterDictionary::Ptr regdict = registerDictionary();
        const RegisterDescriptor SP = regdict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
        BaseSemantics::RiscOperators::Ptr ops = RiscOperators::instanceFromRegisters(regdict, solver);
        DispatcherX86::Ptr dispatcher = DispatcherX86::promote(newInstructionDispatcher(ops));
        try {
            for (size_t i = 0; i < insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // Look at the top of the stack
        const size_t ipWidth = dispatcher->REG_anyIP.nBits();
        SValue::Ptr top = SValue::promote(ops->peekMemory(dispatcher->REG_SS, ops->peekRegister(SP),
                                                        ops->protoval()->undefined_(ipWidth)));
        if (top->toUnsigned().orElse(0) == last->get_address() + last->get_size()) {
            if (target) {
                SValue::Ptr eip = SValue::promote(ops->peekRegister(dispatcher->REG_anyIP));
                eip->toUnsigned().assignTo(*target);
            }
            if (return_va)
                top->toUnsigned().assignTo(*return_va);
            return true;
        }
    }

    return false;
}

bool
X86::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) const {
    if (insns.empty())
        return false;
    auto last = isSgAsmX86Instruction(insns.back());
    ASSERT_not_null(last);

    return last->get_kind() == x86_ret || last->get_kind() == x86_retf;
}

Sawyer::Optional<rose_addr_t>
X86::branchTarget(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmX86Instruction(insn_);
    ASSERT_not_null(insn);

    // Treats far destinations as "unknown"
    switch (insn->get_kind()) {
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
        case x86_loopz:
            if (insn->nOperands() == 1) {
                if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(insn->operand(0)))
                    return ival->get_absoluteValue();
            }
            return Sawyer::Nothing();

        default:
            return Sawyer::Nothing();
    }
}

AddressSet
X86::getSuccessors(SgAsmInstruction *insn_, bool &complete) const {
    auto insn = isSgAsmX86Instruction(insn_);
    ASSERT_not_null(insn);

    AddressSet retval;
    complete = true; /*assume true and prove otherwise*/

    switch (insn->get_kind()) {
        case x86_call:
        case x86_farcall:
        case x86_jmp:
        case x86_farjmp:
            /* Unconditional branch to operand-specified address. We cannot assume that a CALL instruction returns to the
             * fall-through address. */
            if (Sawyer::Optional<rose_addr_t> va = branchTarget(insn)) {
                retval.insert(*va);
            } else {
                complete = false;
            }
            break;

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
        case x86_loopz:
            /* Conditional branches to operand-specified address */
            if (Sawyer::Optional<rose_addr_t> va = branchTarget(insn)) {
                retval.insert(*va);
            } else {
                complete = false;
            }
            retval.insert(insn->get_address() + insn->get_size());
            break;

        case x86_int:                                   // assumes interrupts return
        case x86_int1:
        case x86_int3:
        case x86_into:
        case x86_syscall: {
            retval.insert(insn->get_address() + insn->get_size());  // probable return point
            complete = false;
            break;
        }

        case x86_ret:
        case x86_iret:
        case x86_rsm:
        case x86_sysret:
        case x86_ud2:
        case x86_retf: {
            /* Unconditional branch to run-time specified address */
            complete = false;
            break;
        }

        case x86_hlt: {
            /* Instructions having no successor. */
            break;
        }

        case x86_unknown_instruction: {
            /* Instructions having unknown successors */
            complete = false;
            break;
        }

        default: {
            /* Instructions that always fall through to the next instruction */
            retval.insert(insn->get_address() + insn->get_size());
            break;
        }
    }
    return retval;
}

AddressSet
X86::getSuccessors(const std::vector<SgAsmInstruction*>& insns, bool &complete, const MemoryMap::Ptr &initial_memory) const {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    using namespace Rose::BinaryAnalysis::InstructionSemantics;

    if (debug) {
        debug <<"X86::getSuccessors(" <<StringUtility::addrToString(insns.front()->get_address())
              <<" for " <<insns.size() <<" instruction" <<(1==insns.size()?"":"s") <<"):" <<"\n";
    }

    AddressSet successors = Base::getSuccessors(insns, complete/*out*/);

    // If we couldn't determine all the successors, or a cursory analysis couldn't narrow it down to a single successor then we'll
    // do a more thorough analysis now. In the case where the cursory analysis returned a complete set containing two successors, a
    // thorough analysis might be able to narrow it down to a single successor. We should not make special assumptions about CALL
    // and FARCALL instructions -- their only successor is the specified address operand.
    if (!complete || successors.size() > 1) {
        RegisterDictionary::Ptr regdict = registerDictionary();
        PartialSymbolicSemantics::RiscOperators::Ptr ops = PartialSymbolicSemantics::RiscOperators::instanceFromRegisters(regdict);
        ops->set_memory_map(initial_memory);
        BaseSemantics::Dispatcher::Ptr cpu = newInstructionDispatcher(ops);

        try {
            for (SgAsmInstruction *insn: insns) {
                cpu->processInstruction(insn);
                SAWYER_MESG(debug) <<"  state after " <<insn->toString() <<"\n" <<*ops;
            }
            BaseSemantics::SValue::Ptr ip = ops->peekRegister(regdict->instructionPointerRegister());
            if (auto ipval = ip->toUnsigned()) {
                successors.clear();
                successors.insert(*ipval);
                complete = true;
            }
        } catch(const BaseSemantics::Exception &e) {
            /* Abandon entire basic block if we hit an instruction that's not implemented. */
            debug <<e <<"\n";
        }
    }

    if (debug) {
        debug <<"  successors:";
        for (rose_addr_t va: successors.values())
            debug <<" " <<StringUtility::addrToString(va);
        debug <<(complete?"":"...") <<"\n";
    }

    return successors;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Factories
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Disassembler::Base::Ptr
X86::newInstructionDecoder() const {
    return Disassembler::X86::instance(shared_from_this());
}

Unparser::Base::Ptr
X86::newUnparser() const {
    return Unparser::X86::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
X86::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherX86::instance(shared_from_this(), ops);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner matchers and callbacks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
X86::functionPrologueMatchers(const Partitioner2::Engine::Ptr &engine) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesX86::MatchHotPatchPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchStandardPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchAbbreviatedPrologue::instance());
    retval.push_back(Partitioner2::ModulesX86::MatchEnterPrologue::instance());
    if (engine->settings().partitioner.findingThunks) {
        if (auto engineBinary = engine.dynamicCast<Partitioner2::EngineBinary>())
            retval.push_back(Partitioner2::Modules::MatchThunk::instance(engineBinary->functionMatcherThunks()));
    }
    retval.push_back(Partitioner2::ModulesX86::MatchRetPadPush::instance());
    return retval;
}

std::vector<Partitioner2::BasicBlockCallback::Ptr>
X86::basicBlockCreationHooks(const Partitioner2::Engine::Ptr&) const {
    std::vector<Partitioner2::BasicBlockCallback::Ptr> retval;
    retval.push_back(Partitioner2::ModulesX86::FunctionReturnDetector::instance());
    retval.push_back(Partitioner2::ModulesX86::SwitchSuccessors::instance());
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calling conventions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CallingConvention::Definition::Ptr
X86::cc_cdecl(size_t bitsPerWord) const {
    // Register prefix letter for things like "ax", "eax", "rax"
    const std::string prefix = registerPrefix(bitsPerWord);
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CallingConvention::Definition::instance(bitsPerWord, "cdecl",
                                                      "x86-" + boost::lexical_cast<std::string>(bitsPerWord) + " cdecl",
                                                      regdict);
    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(SP, 0));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(bitsPerWord >> 3);        // return address

    //==== Function parameters ====
    // All parameters are passed on the stack.
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLER);

    //==== Other inputs ====
    // direction flag is always assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("df"), regdict));
    // code segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("cs"), regdict));
    // data segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ds"), regdict));
    // stack segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ss"), regdict));

    //==== Return values ====
    cc->appendOutputParameter(regdict->findOrThrow(prefix + "ax"));
    if (const RegisterDescriptor ST0 = regdict->findLargestRegister(x86_regclass_st, x86_st_0))
        cc->appendOutputParameter(ST0);
    cc->appendOutputParameter(SP);                      // final value is usually one word greater than initial value

    //==== Scratch registers ====
    // (i.e., modified, not callee-saved, not return registers)
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "cx"));
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "dx"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "flags"));
    if (const RegisterDescriptor FPSTATUS = regdict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus))
        cc->scratchRegisters().insert(FPSTATUS);

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

CallingConvention::Definition::Ptr
X86::cc_stdcall(size_t bitsPerWord) const {
    const std::string prefix = registerPrefix(bitsPerWord);
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CallingConvention::Definition::instance(bitsPerWord, "stdcall",
                                                      "x86-" + boost::lexical_cast<std::string>(bitsPerWord) + " stdcall",
                                                      regdict);
    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(SP, 0));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(bitsPerWord >> 3);        // return address

    //==== Function parameters ====
    // All parameters are passed on the stack
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLEE);

    //==== Other inputs ====
    // direction flag is always assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("df"), regdict));
    // code segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("cs"), regdict));
    // data segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ds"), regdict));
    // stack segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ss"), regdict));

    //==== Return values ====
    cc->appendOutputParameter(regdict->findOrThrow(prefix + "ax"));
    if (const RegisterDescriptor ST0 = regdict->findLargestRegister(x86_regclass_st, x86_st_0))
        cc->appendOutputParameter(ST0);
    cc->appendOutputParameter(SP);

    //==== Scratch registers ====
    // Modified, not callee-saved, not return registers
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "cx"));
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "dx"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "flags"));
    if (const RegisterDescriptor FPSTATUS = regdict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus))
        cc->scratchRegisters().insert(FPSTATUS);

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

CallingConvention::Definition::Ptr
X86::cc_fastcall(size_t bitsPerWord) const {
    const std::string prefix = registerPrefix(bitsPerWord);
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CallingConvention::Definition::instance(bitsPerWord, "fastcall",
                                                      "x86-" + boost::lexical_cast<std::string>(bitsPerWord) + " fastcall",
                                                      regdict);
    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(SP, 0));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(bitsPerWord >> 3);        // return address

    //==== Function parameters ====
    // Uses ECX and EDX for first args that fit; all other parameters are passed on the stack.
    cc->appendInputParameter(regdict->findOrThrow(prefix + "cx"));
    cc->appendInputParameter(regdict->findOrThrow(prefix + "dx"));
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLEE);

    //==== Other inputs ====
    // direction flag is always assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("df"), regdict));
    // code segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("cs"), regdict));
    // data segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ds"), regdict));
    // stack segment register is assumed to be valid
    cc->nonParameterInputs().push_back(ConcreteLocation(regdict->findOrThrow("ss"), regdict));

    //==== Return values ====
    cc->appendOutputParameter(regdict->findOrThrow(prefix + "ax"));
    if (const auto ST0 = regdict->findLargestRegister(x86_regclass_st, x86_st_0))
        cc->appendOutputParameter(ST0);
    cc->appendOutputParameter(SP);

    //==== Scratch registers ====
    // I.e., modified, not callee-saved, not return registers
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "cx"));
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "dx"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());
    cc->scratchRegisters().insert(regdict->findOrThrow(prefix + "flags"));
    if (const auto FPSTATUS = regdict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus))
        cc->scratchRegisters().insert(FPSTATUS);

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

const CallingConvention::Dictionary&
X86::callingConventions() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!callingConventions_.isCached()) {
        CallingConvention::Dictionary dict;

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        {
            auto cc = cc_cdecl(bitsPerWord());
            cc->comment(cc.comment() + " 4-byte alignment");
            cc->stackAlignment(4);
            dict.push_back(cc);
        }

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        {
            auto cc = cc_cdecl(bitsPerWord());
            cc->comment(cc.comment() + " 16-byte alignment");
            cc->stackAlignment(16);
            dict.push_back(cc);
        }
#else
        dict.push_back(cc_cdecl(bitsPerWord()));
#endif

        // other conventions
        dict.push_back(cc_stdcall(bitsPerWord()));
        dict.push_back(cc_fastcall(bitsPerWord()));

        callingConventions_ = dict;
    }

    return callingConventions_.get();
}

} // namespace
} // namespace
} // namespace

#endif
