#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Motorola.h>

#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/InstructionMap.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/PartialSymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesM68k.h>
#include <Rose/BinaryAnalysis/Unparser/M68k.h>
#include <Rose/CommandLine/Parser.h>

#include <SgAsmInstruction.h>
#include <SgAsmM68kInstruction.h>
#include <Cxx_GrammarDowncast.h>
#include <sageInterface.h>

#include <Sawyer/Message.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Motorola::Motorola(const std::string &name)
    : Base(name, 4, ByteOrder::ORDER_MSB) {}

Motorola::~Motorola() {}

const CallingConvention::Dictionary&
Motorola::callingConventions() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!callingConventions_.isCached()) {
        CallingConvention::Dictionary dict;

        // https://m680x0.github.io/doc/abi.html
        const RegisterDictionary::Ptr regdict = registerDictionary();
        const RegisterDescriptor SP = regdict->stackPointerRegister();

        auto cc = CallingConvention::Definition::instance("sysv", "m68k-sysv", constPtr());

        //==== Address locations ====
        cc->instructionPointerRegister(regdict->instructionPointerRegister());
        cc->returnAddressLocation(ConcreteLocation(SP, 0));

        //==== Stack characteristics ====
        cc->stackPointerRegister(SP);
        cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
        cc->nonParameterStackSize(bytesPerWord()); // return address

        //====  Function parameters ====
        // All parameters are passed on the stack.
        cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
        cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLER);

        //==== Other inputs ====


        //==== Return values ====
        cc->appendOutputParameter(regdict->findOrThrow("d0"));
        cc->appendOutputParameter(regdict->findOrThrow("a0"));
        cc->appendOutputParameter(regdict->findOrThrow("fp0"));
        cc->appendOutputParameter(SP);                  // final value is usually one word greater than initial value

        //====  Scratch registers ====
        // (i.e., modified, not callee-saved, not return registers
        cc->scratchRegisters().insert(regdict->findOrThrow("d0"));
        cc->scratchRegisters().insert(regdict->findOrThrow("d1"));
        cc->scratchRegisters().insert(regdict->findOrThrow("a0"));
        cc->scratchRegisters().insert(regdict->findOrThrow("a1"));
        cc->scratchRegisters().insert(regdict->findOrThrow("pc"));
        cc->scratchRegisters().insert(regdict->findOrThrow("ccr"));
        cc->scratchRegisters().insert(regdict->findOrThrow("fp0"));
        cc->scratchRegisters().insert(regdict->findOrThrow("fp1"));


        //==== Callee-saved registers
        // Everything else
        RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
        std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
        cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

        dict.push_back(cc);

        callingConventions_ = dict;
    }

    return callingConventions_.get();
}

std::string
Motorola::instructionDescription(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmM68kInstruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
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
    ASSERT_not_reachable("invalid m68k instruction kind: " + StringUtility::numberToString(insn->get_kind()));
}

bool
Motorola::isUnknown(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmM68kInstruction(insn_);
    ASSERT_not_null(insn);
    return m68k_unknown_instruction == insn->get_kind();
}

bool
Motorola::terminatesBasicBlock(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmM68kInstruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
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

bool
Motorola::isFunctionCallFast(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target_va, rose_addr_t *return_va) const {
    if (insns.empty())
        return false;
    auto last = isSgAsmM68kInstruction(insns.back());
    ASSERT_not_null(last);

    // Quick method based only on the kind of instruction
    if (m68k_bsr == last->get_kind() || m68k_jsr == last->get_kind() || m68k_callm == last->get_kind()) {
        if (target_va)
            branchTarget(last).assignTo(*target_va);  // only modifies target_va if it can be determined
        if (return_va)
            *return_va = last->get_address() + last->get_size();
        return true;
    }

    return false;
}

bool
Motorola::isFunctionCallSlow(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target_va,
                                         rose_addr_t *return_va) const {
    if (isFunctionCallFast(insns, target_va, return_va))
        return true;

    static const size_t EXECUTION_LIMIT = 25; // max size of basic blocks for expensive analyses
    if (insns.empty())
        return false;
    auto last = isSgAsmM68kInstruction(insns.back());
    ASSERT_not_null(last);
    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(last);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(func);

    // Slow method: Emulate the instructions and then look at the program counter (PC) and stack (A7).  If the PC points outside the
    // current function and the top of the stack holds an address of an instruction within the current function, then this must be a
    // function call.
    if (interp && insns.size() <= EXECUTION_LIMIT) {
        using namespace Rose::BinaryAnalysis;
        using namespace Rose::BinaryAnalysis::InstructionSemantics;
        using namespace Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;
        const InstructionMap &imap = interp->get_instructionMap();
        RegisterDictionary::Ptr regdict = registerDictionary();
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        BaseSemantics::RiscOperators::Ptr ops = RiscOperators::instanceFromRegisters(regdict, solver);
        ASSERT_not_null(ops);
        DispatcherM68kPtr dispatcher = DispatcherM68k::promote(newInstructionDispatcher(ops));
        SValue::Ptr orig_sp = SValue::promote(ops->peekRegister(dispatcher->REG_A[7]));
        try {
            for (size_t i = 0; i < insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // If the next instruction address is concrete but does not point to a function entry point, then this is not a call.
        SValue::Ptr ip = SValue::promote(ops->peekRegister(regdict->instructionPointerRegister()));
        if (auto target_va = ip->toUnsigned()) {
            SgAsmFunction *target_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(*target_va, NULL));
            if (!target_func || *target_va != target_func->get_entryVa())
                return false;
        }

        // If nothing was pushed onto the stack, then this isn't a function call.
        SValue::Ptr sp = SValue::promote(ops->peekRegister(regdict->stackPointerRegister()));
        SValue::Ptr stack_delta = SValue::promote(ops->add(sp, ops->negate(orig_sp)));
        SValue::Ptr stack_delta_sign = SValue::promote(ops->extract(stack_delta, 31, 32));
        if (stack_delta_sign->isFalse())
            return false;

        // If the top of the stack does not contain a concrete value or the top of the stack does not point to an instruction in
        // this basic block's function, then this is not a function call.
        SValue::Ptr top = SValue::promote(ops->peekMemory(RegisterDescriptor(), sp, sp->undefined_(32)));
        if (auto va = top->toUnsigned()) {
            SgAsmFunction *return_func = SageInterface::getEnclosingNode<SgAsmFunction>(imap.get_value_or(*va, NULL));
            if (!return_func || return_func!=func) {
                return false;
            }
        } else {
            return false;
        }

        // Since the instruction pointer might point to a function entry address and since the top of the stack contains a pointer
        // to an instruction in this function, we assume that this is a function call.
        if (target_va)
            ip->toUnsigned().assignTo(*target_va);
        if (return_va)
            top->toUnsigned().assignTo(*return_va);
        return true;
    }

    // Similar to the above method, but works when all we have is the basic block (e.g., this case gets hit quite a bit from the
    // Partitioner).  Returns true if, after executing the basic block, the top of the stack contains the fall-through address of
    // the basic block. We depend on our caller to figure out if the instruction pointer is reasonably a function entry address.
    if (!interp && insns.size() <= EXECUTION_LIMIT) {
        using namespace Rose::BinaryAnalysis;
        using namespace Rose::BinaryAnalysis::InstructionSemantics;
        using namespace Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;
        RegisterDictionary::Ptr regdict = registerDictionary();
        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        BaseSemantics::RiscOperators::Ptr ops = RiscOperators::instanceFromRegisters(regdict, solver);
        DispatcherM68kPtr dispatcher = DispatcherM68k::promote(newInstructionDispatcher(ops));
        try {
            for (size_t i = 0; i < insns.size(); ++i)
                dispatcher->processInstruction(insns[i]);
        } catch (const BaseSemantics::Exception &e) {
            return false;
        }

        // Look at the top of the stack
        const RegisterDescriptor SP = regdict->stackPointerRegister();
        const RegisterDescriptor IP = regdict->instructionPointerRegister();
        SValue::Ptr top = SValue::promote(ops->peekMemory(RegisterDescriptor(), ops->peekRegister(SP),
                                                          ops->protoval()->undefined_(32)));
        if (top->toUnsigned().orElse(0) == last->get_address() + last->get_size()) {
            if (target_va) {
                SValue::Ptr ip = SValue::promote(ops->peekRegister(IP));
                ip->toUnsigned().assignTo(*target_va);
            }
            if (return_va)
                top->toUnsigned().assignTo(*return_va);
            return true;
        }
    }

    return false;
}

bool
Motorola::isFunctionReturnFast(const std::vector<SgAsmInstruction*>& insns) const {
    if (insns.empty())
        return false;
    auto last = isSgAsmM68kInstruction(insns.back());
    ASSERT_not_null(last);

    switch (last->get_kind()) {
        case m68k_rtd:
        case m68k_rtm:
        case m68k_rtr:
        case m68k_rts:
            return true;
        default:
            return false;
    }
}

Sawyer::Optional<rose_addr_t>
Motorola::branchTarget(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmM68kInstruction(insn_);
    ASSERT_not_null(insn);

    size_t labelArg = 999;                              // which argument is the target?
    bool useEffectiveAddress = false;                   // use the effective address as the target

    switch (insn->get_kind()) {
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
            return Sawyer::Nothing();
    }

    const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
    ASSERT_require(labelArg < args.size());
    SgAsmIntegerValueExpression *target_expr = nullptr;
    if (useEffectiveAddress) {
        if (SgAsmMemoryReferenceExpression *memref = isSgAsmMemoryReferenceExpression(args[labelArg]))
            target_expr = isSgAsmIntegerValueExpression(memref->get_address());
    }
    if (!target_expr)
        target_expr = isSgAsmIntegerValueExpression(args[labelArg]);
    if (!target_expr)
        return Sawyer::Nothing();
    return target_expr->get_absoluteValue();
}

AddressSet
Motorola::getSuccessors(SgAsmInstruction *insn_, bool &complete) const {
    auto insn = isSgAsmM68kInstruction(insn_);
    ASSERT_not_null(insn);

    AddressSet retval;
    complete = true;

    switch (insn->get_kind()) {
        //case m68k_halt: {
        //    // Instructions having no successors
        //    break;
        //}

        case m68k_unknown_instruction:
        case m68k_illegal:
        case m68k_trap: {
            // Instructions having unknown successors
            complete = false;
            break;
        }

        case m68k_rtd:
        case m68k_rtm:
        case m68k_rtr:
        case m68k_rts: {
            // Instructions that have a single successor that is unknown
            complete = false;
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
        case m68k_trapv:
            // Fall-through address and another (known or unknown) address
            if (Sawyer::Optional<rose_addr_t> target_va = branchTarget(insn)) {
                retval.insert(*target_va);
            } else {
                complete = false;
            }
            retval.insert(insn->get_address() + insn->get_size());
            break;

        case m68k_bra:
        case m68k_bsr:
        case m68k_callm:
        case m68k_jmp:
        case m68k_jsr:
            // Unconditional branches
            if (Sawyer::Optional<rose_addr_t> target_va = branchTarget(insn)) {
                retval.insert(*target_va);
            } else {
                complete = false;
            }
            break;

        case m68k_dbt:                                  // no-op
        case m68k_trapf:                                // no-op
        default: {
            // Instructions that always only fall through
            retval.insert(insn->get_address() + insn->get_size());
            break;
        }
    }
    return retval;
}

AddressSet
Motorola::getSuccessors(const std::vector<SgAsmInstruction*>& insns, bool &complete,
                                    const BinaryAnalysis::MemoryMap::Ptr &initial_memory) const {
    using namespace Rose::BinaryAnalysis::InstructionSemantics;
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    if (debug) {
        debug <<"Motorola::getSuccessors(" <<StringUtility::addrToString(insns.front()->get_address())
              <<" for " <<insns.size() <<" instruction" <<(1==insns.size()?"":"s") <<"):" <<"\n";
    }

    AddressSet successors = Base::getSuccessors(insns, complete/*out*/);

    // If we couldn't determine all the successors, or a cursory analysis couldn't narrow it down to a single successor then we'll
    // do a more thorough analysis now. In the case where the cursory analysis returned a complete set containing two successors, a
    // thorough analysis might be able to narrow it down to a single successor. We should not make special assumptions about
    // function call instructions -- their only successor is the specified address operand.
    if (!complete || successors.size() > 1) {
        using namespace Rose::BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics;

        RegisterDictionary::Ptr regdict = registerDictionary();
        RiscOperators::Ptr ops = RiscOperators::instanceFromRegisters(regdict);
        ops->set_memory_map(initial_memory);
        InstructionSemantics::BaseSemantics::Dispatcher::Ptr dispatcher = newInstructionDispatcher(ops);

        try {
            for (size_t i = 0; i < insns.size(); ++i) {
                dispatcher->processInstruction(insns[i]);
                if (debug)
                    debug << "  state after " <<insns[i]->toString() <<"\n" <<*ops;
            }
            SValue::Ptr ip = SValue::promote(ops->peekRegister(regdict->instructionPointerRegister()));
            if (auto number = ip->toUnsigned()) {
                successors.clear();
                successors.insert(*number);
                complete = true; /*this is the complete set of successors*/
            }
        } catch(const BaseSemantics::Exception& e) {
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

Unparser::Base::Ptr
Motorola::newUnparser() const {
    return Unparser::M68k::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
Motorola::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherM68k::instance(shared_from_this(), ops);
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Motorola::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesM68k::MatchLink::instance());
    return retval;
}

std::vector<Partitioner2::BasicBlockCallback::Ptr>
Motorola::basicBlockCreationHooks(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::BasicBlockCallback::Ptr> retval;
    retval.push_back(Partitioner2::ModulesM68k::SwitchSuccessors::instance());
    return retval;
}

} // namespace
} // namespace
} // namespace

#endif
