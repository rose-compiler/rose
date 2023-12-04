// SgAsmM68kInstruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/PartialSymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

unsigned
SgAsmM68kInstruction::get_anyKind() const {
    return p_kind;
}

bool
SgAsmM68kInstruction::isUnknown() const
{
    return m68k_unknown_instruction == get_kind();
}

AddressSet
SgAsmM68kInstruction::getSuccessors(bool &complete)
{
    AddressSet retval;
    complete = true;

    switch (get_kind()) {
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
            if (Sawyer::Optional<rose_addr_t> target_va = branchTarget()) {
                retval.insert(*target_va);
            } else {
                complete = false;
            }
            retval.insert(get_address() + get_size());
            break;

        case m68k_bra:
        case m68k_bsr:
        case m68k_callm:
        case m68k_jmp:
        case m68k_jsr:
            // Unconditional branches
            if (Sawyer::Optional<rose_addr_t> target_va = branchTarget()) {
                retval.insert(*target_va);
            } else {
                complete = false;
            }
            break;

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
SgAsmM68kInstruction::getSuccessors(const std::vector<SgAsmInstruction*>& insns, bool &complete,
                                    const BinaryAnalysis::MemoryMap::Ptr &initial_memory)
{
    using namespace Rose::BinaryAnalysis::InstructionSemantics;
    Stream debug(mlog[DEBUG]);

    if (debug) {
        debug <<"SgAsmM68kInstruction::getSuccessors(" <<StringUtility::addrToString(insns.front()->get_address())
              <<" for " <<insns.size() <<" instruction" <<(1==insns.size()?"":"s") <<"):" <<"\n";
    }

    AddressSet successors = SgAsmInstruction::getSuccessors(insns, complete/*out*/);

    // If we couldn't determine all the successors, or a cursory analysis couldn't narrow it down to a single successor then
    // we'll do a more thorough analysis now. In the case where the cursory analysis returned a complete set containing two
    // successors, a thorough analysis might be able to narrow it down to a single successor. We should not make special
    // assumptions about function call instructions -- their only successor is the specified address operand. */
    if (!complete || successors.size()>1) {
        using namespace Rose::BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics;

        auto arch = Architecture::findByName("nxp-coldfire").orThrow();
        RegisterDictionary::Ptr regdict = arch->registerDictionary();
        RiscOperators::Ptr ops = RiscOperators::instanceFromRegisters(regdict);
        ops->set_memory_map(initial_memory);
        DispatcherM68kPtr dispatcher = DispatcherM68k::promote(arch->newInstructionDispatcher(ops));
        
        try {
            for (size_t i=0; i<insns.size(); ++i) {
                dispatcher->processInstruction(insns[i]);
                if (debug)
                    debug << "  state after " <<insns[i]->toString() <<"\n" <<*ops;
            }
            SValue::Ptr ip = SValue::promote(ops->peekRegister(dispatcher->REG_PC));
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
        BOOST_FOREACH (rose_addr_t va, successors.values())
            debug <<" " <<StringUtility::addrToString(va);
        debug <<(complete?"":"...") <<"\n";
    }

    return successors;
}

Sawyer::Optional<rose_addr_t>
SgAsmM68kInstruction::branchTarget() {
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
            return Sawyer::Nothing();
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
        return Sawyer::Nothing();
    return target_expr->get_absoluteValue();
}


#endif
