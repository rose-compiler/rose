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

// Determines whether this is the special ARM "unkown" instruction. See base class for documentation.
bool
SgAsmAarch64Instruction::isUnknown() const {
    return Aarch64InstructionKind::ARM64_INS_INVALID == get_kind();
}


#endif
