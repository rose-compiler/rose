// SgAsmX86Instruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/PartialSymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include "x86InstructionProperties.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmX86Instruction::get_anyKind() const {
    return p_kind;
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
RegisterDictionary::Ptr
SgAsmX86Instruction::registersForInstructionSize(X86InstructionSize isize) {
    switch (isize) {
        case x86_insnsize_16: return Architecture::findByName("intel-80286").orThrow()->registerDictionary();
        case x86_insnsize_32: return Architecture::findByName("intel-pentium4").orThrow()->registerDictionary();
        case x86_insnsize_64: return Architecture::findByName("amd64").orThrow()->registerDictionary();
        default: ASSERT_not_reachable("invalid x86 instruction size");
    }
}

// class method
RegisterDictionary::Ptr
SgAsmX86Instruction::registersForWidth(size_t nbits) {
    return registersForInstructionSize(instructionSizeForWidth(nbits));
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

AddressSet
SgAsmX86Instruction::getSuccessors(bool &complete) {
    AddressSet retval;
    complete = true; /*assume true and prove otherwise*/

    switch (get_kind()) {
        case x86_call:
        case x86_farcall:
        case x86_jmp:
        case x86_farjmp:
            /* Unconditional branch to operand-specified address. We cannot assume that a CALL instruction returns to the
             * fall-through address. */
            if (Sawyer::Optional<rose_addr_t> va = branchTarget()) {
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
            if (Sawyer::Optional<rose_addr_t> va = branchTarget()) {
                retval.insert(*va);
            } else {
                complete = false;
            }
            retval.insert(get_address() + get_size());
            break;

        case x86_int:                                   // assumes interrupts return
        case x86_int1:
        case x86_int3:
        case x86_into:
        case x86_syscall: {
            retval.insert(get_address() + get_size());  // probable return point
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
            retval.insert(get_address() + get_size());
            break;
        }
    }
    return retval;
}

Sawyer::Optional<rose_addr_t>
SgAsmX86Instruction::branchTarget() {
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
        case x86_loopz:
            if (nOperands() == 1) {
                if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(operand(0)))
                    return ival->get_absoluteValue();
            }
            return Sawyer::Nothing();

        default:
            return Sawyer::Nothing();
    }
}

AddressSet
SgAsmX86Instruction::getSuccessors(const std::vector<SgAsmInstruction*>& insns, bool &complete,
                                   const MemoryMap::Ptr &initial_memory)
{
    Stream debug(mlog[DEBUG]);
    using namespace Rose::BinaryAnalysis::InstructionSemantics;

    if (debug) {
        debug <<"SgAsmX86Instruction::getSuccessors(" <<StringUtility::addrToString(insns.front()->get_address())
              <<" for " <<insns.size() <<" instruction" <<(1==insns.size()?"":"s") <<"):" <<"\n";
    }

    AddressSet successors = SgAsmInstruction::getSuccessors(insns, complete/*out*/);

    /* If we couldn't determine all the successors, or a cursory analysis couldn't narrow it down to a single successor then
     * we'll do a more thorough analysis now. In the case where the cursory analysis returned a complete set containing two
     * successors, a thorough analysis might be able to narrow it down to a single successor. We should not make special
     * assumptions about CALL and FARCALL instructions -- their only successor is the specified address operand. */
    if (!complete || successors.size()>1) {

        Architecture::Base::ConstPtr arch;
        if (SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(this)) {
            arch = Architecture::findByInterpretation(interp).orThrow();
        } else {
            switch (get_baseSize()) {
                case x86_insnsize_16:
                    arch = Architecture::findByName("intel-80286").orThrow();
                    break;
                case x86_insnsize_32:
                    arch = Architecture::findByName("intel-pentium4").orThrow();
                    break;
                case x86_insnsize_64:
                    arch = Architecture::findByName("amd64").orThrow();
                    break;
                default:
                    ASSERT_not_reachable("invalid x86 instruction size");
            }
        }
        RegisterDictionary::Ptr regdict = arch->registerDictionary();
        const RegisterDescriptor IP = regdict->findLargestRegister(x86_regclass_ip, 0);
        PartialSymbolicSemantics::RiscOperators::Ptr ops = PartialSymbolicSemantics::RiscOperators::instanceFromRegisters(regdict);
        ops->set_memory_map(initial_memory);
        BaseSemantics::Dispatcher::Ptr cpu = arch->newInstructionDispatcher(ops);

        try {
            BOOST_FOREACH (SgAsmInstruction *insn, insns) {
                cpu->processInstruction(insn);
                SAWYER_MESG(debug) <<"  state after " <<insn->toString() <<"\n" <<*ops;
            }
            BaseSemantics::SValue::Ptr ip = ops->peekRegister(IP);
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
        BOOST_FOREACH (rose_addr_t va, successors.values())
            debug <<" " <<StringUtility::addrToString(va);
        debug <<(complete?"":"...") <<"\n";
    }

    return successors;
}

#endif
