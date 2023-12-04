// SgAsmAarch64Instruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;                   // temporary

unsigned
SgAsmAarch64Instruction::get_anyKind() const {
    return p_kind;
}

// Returns true if the instruction modifies the instruction pointer.  Most instructions modify the instruction pointer to
// advancing it to the fall-through address (the instruction at the following memory address), in which this function returns
// false. This function returns true if @c this instruction can cause the instruction pointer to point somewhere other than the
// following instruction.
static bool modifies_ip(SgAsmAarch64Instruction *insn) {
    using Kind = ::Rose::BinaryAnalysis::Aarch64InstructionKind;

    switch (insn->get_kind()) {
        case Kind::ARM64_INS_INVALID:
        case Kind::ARM64_INS_B:                         // branch, branc conditionally
        case Kind::ARM64_INS_BL:                        // branch with link
        case Kind::ARM64_INS_BLR:                       // branch with link to register
        case Kind::ARM64_INS_BR:                        // branch to register
        case Kind::ARM64_INS_BRK:                       // breakpoint
        case Kind::ARM64_INS_CBNZ:                      // compare and branch on nonzero
        case Kind::ARM64_INS_CBZ:                       // compare and branch on zero
        case Kind::ARM64_INS_ERET:                      // return from exception
        case Kind::ARM64_INS_HLT:                       // halt
        case Kind::ARM64_INS_HVC:                       // hypervisor call
        case Kind::ARM64_INS_RET:                       // return from subroutine
        case Kind::ARM64_INS_SMC:                       // secure monitor call
        case Kind::ARM64_INS_SVC:                       // supervisor call
        case Kind::ARM64_INS_TBNZ:                      // test bit and branch if nonzero
        case Kind::ARM64_INS_TBZ:                       // test bit and branch if zero
            return true;

        default:
            return false;
    }
}

// see base class
bool
SgAsmAarch64Instruction::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target,
                                            rose_addr_t *return_va) {
    if (insns.empty())
        return false;
    SgAsmAarch64Instruction *last = isSgAsmAarch64Instruction(insns.back());
    if (!last)
        return false;

    // Quick method based only on the kind of instruction
    using Kind = ::Rose::BinaryAnalysis::Aarch64InstructionKind;
    switch (last->get_kind()) {
        case Kind::ARM64_INS_BL:
        case Kind::ARM64_INS_BLR:
        //case Kind::ARM64_INS_BLRAA: -- not in capstone
        //case Kind::ARM64_INS_BLRAAZ: -- not in capstone
        //case Kind::ARM64_INS_BLRAB: -- not in capstone
        //case Kind::ARM64_INS_BLRABZ: -- not in capstone
            if (target)
                last->branchTarget().assignTo(*target);
            if (return_va)
                *return_va = last->get_address() + last->get_size();
            return true;
        default:
            return false;
    }
}

// see base class
bool
SgAsmAarch64Instruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target,
                                            rose_addr_t *return_va) {
    return isFunctionCallFast(insns, target, return_va);
}

// see base class
bool
SgAsmAarch64Instruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) {
    if (insns.empty())
        return false;
    SgAsmAarch64Instruction *last_insn = isSgAsmAarch64Instruction(insns.back());
    if (!last_insn)
        return false;

    using Kind = ::Rose::BinaryAnalysis::Aarch64InstructionKind;
    switch (last_insn->get_kind()) {
        case Kind::ARM64_INS_RET:
        //case Kind::ARM64_INS_RETAA: -- not present in capstone
        //case Kind::ARM64_INS_RETAB: -- not present in capstone
            return true;
        default:
            return false;
    }
}

// see base class
bool
SgAsmAarch64Instruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) {
    return isFunctionReturnFast(insns);
}

AddressSet
SgAsmAarch64Instruction::getSuccessors(bool &complete) {
    using Kind = ::Rose::BinaryAnalysis::Aarch64InstructionKind;
    complete = true;           // set to true for now, change below if necessary

    AddressSet retval;
    const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();

    switch (get_kind()) {
        case Kind::ARM64_INS_B:
            if (get_condition() != ARM64_CC_AL && get_condition() != ARM64_CC_NV) {
                // This is a conditional branch, so the fall through address is a possible successor.
                retval.insert(get_address() + get_size());
            }
            [[fallthrough]];
        case Kind::ARM64_INS_BL:
        case Kind::ARM64_INS_BLR:
        case Kind::ARM64_INS_BR:
            // First argument is the branch target
            ASSERT_require(exprs.size() == 1);
            if (auto ival = isSgAsmIntegerValueExpression(exprs[0])) {
                retval.insert(ival->get_absoluteValue());
            } else {
                complete = false;
            }
            break;
            
        case Kind::ARM64_INS_BRK:
        case Kind::ARM64_INS_ERET:
        case Kind::ARM64_INS_HVC:
        case Kind::ARM64_INS_RET:
        case Kind::ARM64_INS_SMC:
        case Kind::ARM64_INS_SVC:
            complete = false;
            break;

        case Kind::ARM64_INS_CBNZ:
        case Kind::ARM64_INS_CBZ:
            ASSERT_require(exprs.size() == 2);
            retval.insert(get_address() + get_size());
            if (auto ival = isSgAsmIntegerValueExpression(exprs[1])) {
                retval.insert(ival->get_absoluteValue());
            } else {
                complete = false;
            }
            break;

        case Kind::ARM64_INS_HLT:
            // no successors
            break;

        case Kind::ARM64_INS_TBNZ:
        case Kind::ARM64_INS_TBZ:
            ASSERT_require(exprs.size() == 3);
            retval.insert(get_address() + get_size());
            if (auto ival = isSgAsmIntegerValueExpression(exprs[1])) {
                retval.insert(ival->get_absoluteValue());
            } else {
                complete = false;
            }
            break;

        default:
            // all other instructions only ever fall through to the next address
            retval.insert(get_address() + get_size());
            break;
    }
    return retval;
}

Sawyer::Optional<rose_addr_t>
SgAsmAarch64Instruction::branchTarget() {
    using Kind = ::Rose::BinaryAnalysis::Aarch64InstructionKind;
    const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
    switch (get_kind()) {
        case Kind::ARM64_INS_B:
        case Kind::ARM64_INS_BL:
        case Kind::ARM64_INS_BLR:
        case Kind::ARM64_INS_BR:
            // First argument is the branch target
            ASSERT_require(exprs.size() == 1);
            if (auto ival = isSgAsmIntegerValueExpression(exprs[0]))
                return ival->get_absoluteValue();
            break;

        case Kind::ARM64_INS_BRK:
        case Kind::ARM64_INS_ERET:
        case Kind::ARM64_INS_HVC:
        case Kind::ARM64_INS_RET:
        case Kind::ARM64_INS_SMC:
        case Kind::ARM64_INS_SVC:
            // branch instruction, but target is not known
            break;

        case Kind::ARM64_INS_CBNZ:
        case Kind::ARM64_INS_CBZ:
            ASSERT_require(exprs.size() == 2);
            if (auto ival = isSgAsmIntegerValueExpression(exprs[1]))
                return ival->get_absoluteValue();
            break;

        case Kind::ARM64_INS_HLT:
            // not considered a branching instruction (no successors)
            break;

        case Kind::ARM64_INS_TBNZ:
        case Kind::ARM64_INS_TBZ:
            ASSERT_require(exprs.size() == 3);
            if (auto ival = isSgAsmIntegerValueExpression(exprs[1]))
                return ival->get_absoluteValue();
            break;
        default:
            break;
    }
    return false;
}

// Does instruction terminate basic block? See base class for full documentation.
bool
SgAsmAarch64Instruction::terminatesBasicBlock() {
    if (get_kind()==Aarch64InstructionKind::ARM64_INS_INVALID)
        return true;
    return modifies_ip(this);
}

// Determines whether this is the special ARM "unkown" instruction. See base class for documentation.
bool
SgAsmAarch64Instruction::isUnknown() const {
    return Aarch64InstructionKind::ARM64_INS_INVALID == get_kind();
}


#endif
