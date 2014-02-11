/* SgAsmM68kInstruction member definitions. Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any other *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "Disassembler.h"

// see base class
bool
SgAsmM68kInstruction::terminates_basic_block()
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
        case m68k_halt:
        case m68k_illegal:
        case m68k_jmp:
        case m68k_jsr:
        case m68k_rte:
        case m68k_rts:
        case m68k_trap:
            return true;
        default:
            return false;
    }
}

// see base class
bool
SgAsmM68kInstruction::is_function_call(const std::vector<SgAsmInstruction*>& insns, rose_addr_t *target_va,
                                       rose_addr_t *return_va)
{
    if (insns.empty())
        return false;
    SgAsmM68kInstruction *last = isSgAsmM68kInstruction(insns.back());
    if (!last)
        return false;

    // Quick method based only on the kind of instruction
    if (m68k_bsr==last->get_kind() || m68k_jsr==last->get_kind()) {
        bool is_branch __attribute__((unused)) = last->get_branch_target(target_va);
        assert(is_branch);
        if (return_va)
            *return_va = last->get_address() + last->get_size();
        return true;
    }

    // Slow method: emulate the instructions and then look at the instruction and stack pointers.
    // FIXME: Not implemented yet--no m68k semantics. See SgAsmX86Instruction::is_function_call(). [Robb P. Matzke 2013-10-01]
    return false;
}

bool
SgAsmM68kInstruction::is_function_return(const std::vector<SgAsmInstruction*>& insns)
{
    if (insns.empty())
        return false;
    SgAsmM68kInstruction *last = isSgAsmM68kInstruction(insns.back());
    if (!last)
        return false;
    if (last->get_kind()==m68k_rts || last->get_kind()==m68k_rte)
        return true;
    return false;
}

bool
SgAsmM68kInstruction::is_unknown() const
{
    return m68k_unknown_instruction == get_kind();
}

Disassembler::AddressSet
SgAsmM68kInstruction::get_successors(bool *complete)
{
    Disassembler::AddressSet retval;
    *complete = true;

    switch (get_kind()) {
        case m68k_halt: {
            // Instructions having no successors
            break;
        }

        case m68k_unknown_instruction:
        case m68k_illegal:
        case m68k_trap: {
            // Instructions having unknown successors
            *complete = false;
            break;
        }

        case m68k_rte:
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
        case m68k_bsr: {
            // Conditional branches
            rose_addr_t target_va;
            bool is_branch __attribute__((unused)) = get_branch_target(&target_va);
            assert(is_branch);
            retval.insert(target_va);
            retval.insert(get_address() + get_size());
            break;
        }
            
        case m68k_bra:
        case m68k_jmp:
        case m68k_jsr: {
            // Unconditional branches
            rose_addr_t target_va;
            bool is_branch __attribute__((unused)) = get_branch_target(&target_va);
            assert(is_branch);
            retval.insert(target_va);
            break;
        }

        default: {
            // Instructions that always only fall through
            retval.insert(get_address() + get_size());
            break;
        }
    }
    return retval;
}

bool
SgAsmM68kInstruction::get_branch_target(rose_addr_t *target)
{
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
        case m68k_bra:
        case m68k_bsr:
        case m68k_bvc:
        case m68k_bvs:
        case m68k_jmp:
        case m68k_jsr: {
            const SgAsmExpressionPtrList &args = get_operandList()->get_operands();
            assert(1==args.size());
            SgAsmIntegerValueExpression *target_expr = isSgAsmIntegerValueExpression(args[0]);
            assert(target_expr!=NULL);
            if (target)
                *target = target_expr->get_absolute_value();
            return true;
        }

        default:
            // Not a branching instruction; do not modify target
            return false;
    }
}
