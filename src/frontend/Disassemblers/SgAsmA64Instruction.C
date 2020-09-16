/* SgAsmA64Instruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64
#include "sage3basic.h"

#include "Disassembler.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;                   // temporary

unsigned
SgAsmA64Instruction::get_anyKind() const {
    return p_kind;
}

// Returns true if the instruction modifies the instruction pointer.  Most instructions modify the instruction pointer to
// advancing it to the fall-through address (the instruction at the following memory address), in which this function returns
// false. This function returns true if @c this instruction can cause the instruction pointer to point somewhere other than the
// following instruction.
static bool modifies_ip(SgAsmA64Instruction *insn) {
    using Kind = ::Rose::BinaryAnalysis::A64InstructionKind;

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
SgAsmA64Instruction::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) {
    if (insns.empty())
        return false;
    SgAsmA64Instruction *last = isSgAsmA64Instruction(insns.back());
    if (!last)
        return false;

    // Quick method based only on the kind of instruction
    using Kind = ::Rose::BinaryAnalysis::A64InstructionKind;
    switch (last->get_kind()) {
        case Kind::ARM64_INS_BL:
        case Kind::ARM64_INS_BLR:
        //case Kind::ARM64_INS_BLRAA: -- not in capstone
        //case Kind::ARM64_INS_BLRAAZ: -- not in capstone
        //case Kind::ARM64_INS_BLRAB: -- not in capstone
        //case Kind::ARM64_INS_BLRABZ: -- not in capstone
            last->getBranchTarget(target);
            if (return_va)
                *return_va = last->get_address() + last->get_size();
            return true;
        default:
            return false;
    }
}

// see base class
bool
SgAsmA64Instruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) {
    return isFunctionCallFast(insns, target, return_va);
}

// see base class
bool
SgAsmA64Instruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) {
    if (insns.empty())
        return false;
    SgAsmA64Instruction *last_insn = isSgAsmA64Instruction(insns.back());
    if (!last_insn)
        return false;

    using Kind = ::Rose::BinaryAnalysis::A64InstructionKind;
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
SgAsmA64Instruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) {
    return isFunctionReturnFast(insns);
}

AddressSet
SgAsmA64Instruction::getSuccessors(bool &complete) {
    using Kind = ::Rose::BinaryAnalysis::A64InstructionKind;
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

bool
SgAsmA64Instruction::getBranchTarget(rose_addr_t *target) {
    using Kind = ::Rose::BinaryAnalysis::A64InstructionKind;
    const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
    switch (get_kind()) {
        case Kind::ARM64_INS_B:
        case Kind::ARM64_INS_BL:
        case Kind::ARM64_INS_BLR:
        case Kind::ARM64_INS_BR:
            // First argument is the branch target
            ASSERT_require(exprs.size() == 1);
            if (auto ival = isSgAsmIntegerValueExpression(exprs[0])) {
                if (target)
                    *target = ival->get_absoluteValue();
                return true;
            }
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
            if (auto ival = isSgAsmIntegerValueExpression(exprs[1])) {
                if (target)
                    *target = ival->get_absoluteValue();
                return true;
            }
            break;

        case Kind::ARM64_INS_HLT:
            // not considered a branching instruction (no successors)
            break;

        case Kind::ARM64_INS_TBNZ:
        case Kind::ARM64_INS_TBZ:
            ASSERT_require(exprs.size() == 3);
            if (auto ival = isSgAsmIntegerValueExpression(exprs[1])) {
                if (target)
                    *target = ival->get_absoluteValue();
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

// Does instruction terminate basic block? See base class for full documentation.
bool
SgAsmA64Instruction::terminatesBasicBlock() {
    if (get_kind()==A64InstructionKind::ARM64_INS_INVALID)
        return true;
    return modifies_ip(this);
}

// Determines whether this is the special ARM "unkown" instruction. See base class for documentation.
bool
SgAsmA64Instruction::isUnknown() const {
    return A64InstructionKind::ARM64_INS_INVALID == get_kind();
}

std::string
SgAsmA64Instruction::description() const {
    // The commented out cases are not present in Capstone at this time. Parenthesized reasons are given when available.
    using Kind = ::Rose::BinaryAnalysis::A64InstructionKind;
    switch (get_kind()) {
        case Kind::ARM64_INS_INVALID:           return "";
        case Kind::ARM64_INS_ABS:               return "absolute value";
        case Kind::ARM64_INS_ADC:               return "add with carry";
        //case Kind::ARM64_INS_ADCS: add with carry, setting flags
        case Kind::ARM64_INS_ADD:               return "add";
        // case Kind::ARM64_INS_ADDG: add with tag (ARMv8.5)
        case Kind::ARM64_INS_ADDHN:             return "add returning high narrow to lower";
        case Kind::ARM64_INS_ADDHN2:            return "add returning high narrow to upper";
        case Kind::ARM64_INS_ADDP:              return "add pair of elements";
        // case Kind::ARM64_INS_ADDS: add, setting flags
        case Kind::ARM64_INS_ADDV:              return "add across vector";
        case Kind::ARM64_INS_ADR:               return "form pc-relative address";
        case Kind::ARM64_INS_ADRP:              return "form pc-relative address to 4kB page";
        case Kind::ARM64_INS_AESD:              return "AES single round decryption";
        case Kind::ARM64_INS_AESE:              return "AES single round encryption";
        case Kind::ARM64_INS_AESIMC:            return "AES inverse mix columns";
        case Kind::ARM64_INS_AESMC:             return "AES mix columns";
        case Kind::ARM64_INS_AND:               return "bitwise AND";
        // case Kind::ARM64_INS_ANDS: bitwise AND, setting flags
        case Kind::ARM64_INS_ASR:               return "arithmetic shift right";
        // case Kind::ARM64_INS_ASRV: arithmetic shift right variable (used by the alias ASR)
        case Kind::ARM64_INS_AT:                return "address translate";
        // case Kind::ARM64_INS_AUTDA: authenticate data address using key A (ARMv8.3)
        // case Kind::ARM64_INS_AUTDZA: authenticate data address using key A (ARMv8.3)
        // case Kind::ARM64_INS_AUTDB: authenticate data address using key B (ARMv8.3)
        // case Kind::ARM64_INS_AUTDZB: authenticate data address using key B (ARMv8.3)
        // case Kind::ARM64_INS_AUTIA: authenticate instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_AUTIA1716: authenticate instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_AUTIASP: authenticate instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_AUTIAZ: authenticate instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_AUTIZA: authenticate instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_AUTIB: authenticate instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_AUTIB1716: authenticate instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_AUTIBSP: authenticate instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_AUTIBZ: authenticate instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_AUTIZB: authenticate instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_AXFlag: convert floating-point condition flags from ARM to external format (ARMv8.5)
        case Kind::ARM64_INS_B:                 return "branch";
        // case Kind::ARM64_INS_BCAX: bit clear and exclusive OR (ARMv8.2)
        // case Kind::ARM64_INS_BFC: bitfield clear (ARMv8.2)
        case Kind::ARM64_INS_BFI:               return "bitfield insert"; // alias of BFM
        case Kind::ARM64_INS_BFM:               return "bitfield move";
        case Kind::ARM64_INS_BFXIL:             return "bifield extract and insert low";
        case Kind::ARM64_INS_BIC:               return "bitwise bit clear";
        // case Kind::ARM64_INS_BICS: bitwise bit clear, setting flags
        case Kind::ARM64_INS_BIF:               return "bitwise insert if false";
        case Kind::ARM64_INS_BIT:               return "bitwise insert if true";
        case Kind::ARM64_INS_BL:                return "branch with link";
        case Kind::ARM64_INS_BLR:               return "branch with link to register";
        // case Kind::ARM64_INS_BLRAA: branch with link to register with pointer authentication (ARMv8.3)
        // case Kind::ARM64_INS_BLRAAZ: branch with link to register with pointer authentication (ARMv8.3)
        // case Kind::ARM64_INS_BLRAB: branch with link to register with pointer authentication (ARMv8.3)
        // case Kind::ARM64_INS_BLRABZ: branch with link to register with pointer authentication (ARMv8.3)
        case Kind::ARM64_INS_BR:                return "branch to register";
        // case Kind::ARM64_INS_BRAA: branch to register with pointer authentication (ARMv8.3)
        // case Kind::ARM64_INS_BRAAZ: branch to register with pointer authentication (ARMv8.3)
        // case Kind::ARM64_INS_BRAB: branch to register with pointer authentication (ARMv8.3)
        // case Kind::ARM64_INS_BRABZ: branch to register with pointer authentication (ARMv8.3)
        case Kind::ARM64_INS_BRK:               return "breakpoint";
        case Kind::ARM64_INS_BSL:               return "bitwise select";
        // case Kind::ARM64_INS_BTI: branch target identification (ARMv8.5)
        // case Kind::ARM64_INS_CAS: compare and swap (ARMv8.1)
        // case Kind::ARM64_INS_CASA: compare and swap (ARMv8.1)
        // case Kind::ARM64_INS_CASAL: compare and swap (ARMv8.1)
        // case Kind::ARM64_INS_CASL: compare and swap word or doubleword (ARMv8.1)
        // case Kind::ARM64_INS_CASB: compare and swap byte (ARMv8.1)
        // case Kind::ARM64_INS_CASAB: compare and swap byte (ARMv8.1)
        // case Kind::ARM64_INS_CASALB: compare and swap byte (ARMv8.1)
        // case Kind::ARM64_INS_CASLB: compare and swap byte (ARMv8.1)
        // case Kind::ARM64_INS_CASH: compare and swap halfword (ARMv8.1)
        // case Kind::ARM64_INS_CASAH: compare and swap halfword (ARMv8.1)
        // case Kind::ARM64_INS_CASALH: compare and swap halfword (ARMv8.1)
        // case Kind::ARM64_INS_CASLH: compare and swap halfword (ARMv8.1)
        // case Kind::ARM64_INS_CASP: compare and swap apir of words or doublewords (ARMv8.1)
        // case Kind::ARM64_INS_CASPA: compare and swap apir of words or doublewords (ARMv8.1)
        // case Kind::ARM64_INS_CASPAL: compare and swap apir of words or doublewords (ARMv8.1)
        // case Kind::ARM64_INS_CASPL: compare and swap apir of words or doublewords (ARMv8.1)
        case Kind::ARM64_INS_CBNZ:              return "compare and branch on nonzero";
        case Kind::ARM64_INS_CBZ:               return "compare and branch on zero";
        case Kind::ARM64_INS_CCMN:              return "conditional compare negative";
        case Kind::ARM64_INS_CCMP:              return "conditional compare";
        // case Kind::ARM64_INS_CFINV: invert carry flag (ARMv8.4)
        // case Kind::ARM64_INS_CFP: control flow prediction restriction by context (alias of SYS)
        case Kind::ARM64_INS_CINC:              return "conditional increment"; // alias of CSINC
        case Kind::ARM64_INS_CINV:              return "conditonal invert"; // alias of CSINV
        case Kind::ARM64_INS_CLREX:             return "clear exclusive";
        case Kind::ARM64_INS_CLS:               return "count leading sign bits";
        case Kind::ARM64_INS_CLZ:               return "count leading zero bits";
        case Kind::ARM64_INS_CMEQ:              return "compare bitwise equal";
        case Kind::ARM64_INS_CMGE:              return "compare signed greater than or equal";
        case Kind::ARM64_INS_CMGT:              return "compare signed greater than";
        case Kind::ARM64_INS_CMHI:              return "compare unsigned higher";
        case Kind::ARM64_INS_CMHS:              return "compare unsigned higher or same";
        case Kind::ARM64_INS_CMLE:              return "compare signed less than or equal";
        case Kind::ARM64_INS_CMLT:              return "compare signed less than";
        case Kind::ARM64_INS_CMN:               return "compare negative"; // alias of ADDS
        case Kind::ARM64_INS_CMP:               return "compare"; // alias of SUBS
        // case Kind::ARM64_INS_CMPP: compare with tag (ARMv8.5)
        case Kind::ARM64_INS_CMTST:             return "cmpare bitwise test";
        case Kind::ARM64_INS_CNEG:              return "conditional negate"; // alias of CSNEG
        case Kind::ARM64_INS_CNT:               return "population count per byte";
        // case Kind::ARM64_INS_CPP: cache prefetch prediction restriction by context (alias of SYS)
        case Kind::ARM64_INS_CRC32B:            return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32CB:           return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32CH:           return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32CW:           return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32CX:           return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32H:            return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32W:            return "cyclic redundancy check";
        case Kind::ARM64_INS_CRC32X:            return "cyclic redundancy check";
        // case Kind::ARM64_INS_CSDB: consumption of speculative data barrier
        case Kind::ARM64_INS_CSEL:              return "conditional select";
        case Kind::ARM64_INS_CSET:              return "conditional set"; // alias of CSINC
        case Kind::ARM64_INS_CSETM:             return "conditional set mask"; // alias of CSINV
        case Kind::ARM64_INS_CSINC:             return "conditional select increment";
        case Kind::ARM64_INS_CSINV:             return "conditional select invert";
        case Kind::ARM64_INS_CSNEG:             return "conditional select negation";
        case Kind::ARM64_INS_DC:                return "data cache"; // alias of SYS
        case Kind::ARM64_INS_DCPS1:             return "debug change PE state to EL1";
        case Kind::ARM64_INS_DCPS2:             return "debug change PE state to EL2";
        case Kind::ARM64_INS_DCPS3:             return "debug change PE state to EL3";
        case Kind::ARM64_INS_DMB:               return "data memory barrier";
        case Kind::ARM64_INS_DRPS:              return "debug restore process state";
        case Kind::ARM64_INS_DSB:               return "data synchronization barrier";
        case Kind::ARM64_INS_DUP:               return "duplicate";
        // case Kind::ARM64_INS_DVP: data value prediction restriction by context (alias of SYS)
        case Kind::ARM64_INS_EON:               return "bitwise exclusive OR NOT";
        case Kind::ARM64_INS_EOR:               return "bitwise exclusive OR";
        // case Kind::ARM64_INS_EOR3: three-way exclusive OR (ARMv8.2)
        case Kind::ARM64_INS_ERET:              return "return from exception";
        // case Kind::ARM64_INS_ERETAA: exception return (ARMv8.3)
        // case Kind::ARM64_INS_ERETAB: exception return (ARMv8.3)
        // case Kind::ARM64_INS_ESB: error synchronization barrier (ARMv8.2)
        case Kind::ARM64_INS_EXT:               return "extract vector from pair of vectors";
        case Kind::ARM64_INS_EXTR:              return "extract register";
        case Kind::ARM64_INS_FABD:              return "floating-point absolute difference";
        case Kind::ARM64_INS_FABS:              return "floating-point absolute value";
        case Kind::ARM64_INS_FACGE:             return "floating-point absolute compare greater than or equal";
        case Kind::ARM64_INS_FACGT:             return "floating-point absolute compare greater than";
        case Kind::ARM64_INS_FADD:              return "floating-point add";
        case Kind::ARM64_INS_FADDP:             return "floating-point add pair";
        case Kind::ARM64_INS_FCCMP:             return "floating-point conditional quiet compare";
        case Kind::ARM64_INS_FCCMPE:            return "floating-point conditional signaling compare";
        case Kind::ARM64_INS_FCMEQ:             return "floating-point compare equal";
        case Kind::ARM64_INS_FCMGE:             return "floating-point compare greater than or equal";
        case Kind::ARM64_INS_FCMGT:             return "floating-point compare greater than";
        // case Kind::ARM64_INS_FCMLA: floating-point complex multiply accumulate (ARMv8.3)
        case Kind::ARM64_INS_FCMLE:             return "floating-point compare less than or equal";
        case Kind::ARM64_INS_FCMLT:             return "floating-point compare less than";
        case Kind::ARM64_INS_FCMP:              return "floating-point quiet compare";
        case Kind::ARM64_INS_FCMPE:             return "floating-point signaling compare";
        case Kind::ARM64_INS_FCSEL:             return "floating-point conditional select";
        case Kind::ARM64_INS_FCVT:              return "floating-point convert";
        case Kind::ARM64_INS_FCVTAS:            return "floating-point convert to signed integer";
        case Kind::ARM64_INS_FCVTAU:            return "floating-point convert to unsigned integer";
        case Kind::ARM64_INS_FCVTL:             return "floating-point convert to higher precision long";
        case Kind::ARM64_INS_FCVTL2:            return "floating-point convert to higher precision long";
        case Kind::ARM64_INS_FCVTMS:            return "floating-point convert to signed integer";
        case Kind::ARM64_INS_FCVTMU:            return "floaging-point convert to unsigned integer";
        case Kind::ARM64_INS_FCVTN:             return "floating-point convert to lower precision narrow";
        case Kind::ARM64_INS_FCVTN2:            return "floating-point convert to lower precision narrow";
        case Kind::ARM64_INS_FCVTNS:            return "floating-point convert to signed integer";
        case Kind::ARM64_INS_FCVTNU:            return "floating-point convert to unsigned integer";
        case Kind::ARM64_INS_FCVTPS:            return "floating-point convert to signed integer";
        case Kind::ARM64_INS_FCVTPU:            return "floating-point convert to unsigned integer";
        case Kind::ARM64_INS_FCVTXN:            return "floating-point convert to lower precision narrow";
        case Kind::ARM64_INS_FCVTXN2:           return "floating-point convert to lower precision narrow";
        case Kind::ARM64_INS_FCVTZS:            return "floating-point convert to signed fixed-point";
        case Kind::ARM64_INS_FCVTZU:            return "floating-point convert to unsigned fixed-point";
        case Kind::ARM64_INS_FDIV:              return "floating-point divide";
        // case Kind::ARM64_INS_FJCVTZS: floating-point Javascript convert to signed fixed-point (ARMv8.3)
        case Kind::ARM64_INS_FMADD:             return "floating-point fused multiply-add";
        case Kind::ARM64_INS_FMAX:              return "floating-point maximum";
        case Kind::ARM64_INS_FMAXNM:            return "floating-point maximum number";
        case Kind::ARM64_INS_FMAXNMP:           return "floating-point maximum number pairwise";
        case Kind::ARM64_INS_FMAXNMV:           return "floating-point maximum number across vector";
        case Kind::ARM64_INS_FMAXP:             return "floating-point maximum of pair of elements";
        case Kind::ARM64_INS_FMAXV:             return "floating-point maximum across vector";
        case Kind::ARM64_INS_FMIN:              return "floating-point minimum";
        case Kind::ARM64_INS_FMINNM:            return "floating-point minimum number";
        case Kind::ARM64_INS_FMINNMP:           return "floating-point minimum number pairwise";
        case Kind::ARM64_INS_FMINNMV:           return "floating-point minimum number across vector";
        case Kind::ARM64_INS_FMINP:             return "floating-point minimum of pair of elements";
        case Kind::ARM64_INS_FMINV:             return "floating-point minimum across vector";
        case Kind::ARM64_INS_FMLA:              return "floating-point fused multiply-add";
        // case Kind::ARM64_INS_FMLAL: floating-point fused multiply-add long (ARMv8.2)
        // case Kind::ARM64_INS_FMLAL2: floating-point fused multiply-add long (ARMv8.2)
        case Kind::ARM64_INS_FMLS:              return "floating-point fused multiply-subtract";
        // case Kind::ARM64_INS_FMLSL: floating-point fused multiply-subtract long (ARMv8.2)
        // case Kind::ARM64_INS_FMLSL2: floating-point fused multiply-subtract long (ARMv8.2)
        case Kind::ARM64_INS_FMOV:              return "floating-point move";
        case Kind::ARM64_INS_FMSUB:             return "floating-point fused multiply-subtract";
        case Kind::ARM64_INS_FMUL:              return "floating-point multiply";
        case Kind::ARM64_INS_FMULX:             return "floating-point multiply extended";
        case Kind::ARM64_INS_FNEG:              return "floating-point negate";
        case Kind::ARM64_INS_FNMADD:            return "floating-point negated fused multiply-add";
        case Kind::ARM64_INS_FNMSUB:            return "floating-point negated fused multiply-subtract";
        case Kind::ARM64_INS_FNMUL:             return "floating-point multiply-negate";
        case Kind::ARM64_INS_FRECPE:            return "floating-point reciprocal estimate";
        case Kind::ARM64_INS_FRECPS:            return "floating-point reciprocal step";
        case Kind::ARM64_INS_FRECPX:            return "floating-point reciprocal exponent";
        // case Kind::ARM64_INS_FRINT32X: floating-point round to 32-integer (ARMv8.5)
        // case Kind::ARM64_INS_FRINT32Z: floating-point round to 32-bit integer toward zero
        // case Kind::ARM64_INS_FRINT64X: floating-point round to 64-bit integer
        // case Kind::ARM64_INS_FRINT64Z: floating-point round to 64-bit integer toward zero
        case Kind::ARM64_INS_FRINTA:            return "floating-point round to integral, ties to away";
        case Kind::ARM64_INS_FRINTI:            return "floating-point round to integral, current rounding mode";
        case Kind::ARM64_INS_FRINTM:            return "floating-point round to integral, toward minus infinity";
        case Kind::ARM64_INS_FRINTN:            return "floating-point round to integral, ties to even";
        case Kind::ARM64_INS_FRINTP:            return "floating-point round to integral, toward plus infinity";
        case Kind::ARM64_INS_FRINTX:            return "floating-point round to integral exact, current rounding mode";
        case Kind::ARM64_INS_FRINTZ:            return "floating-point round to integral, toward zero";
        case Kind::ARM64_INS_FRSQRTE:           return "floating-point reciprocal square root estimate";
        case Kind::ARM64_INS_FRSQRTS:           return "floating-point reciprocal square root step";
        case Kind::ARM64_INS_FSQRT:             return "floating-point square root";
        case Kind::ARM64_INS_FSUB:              return "floating-point subtract";
        // case Kind::ARM64_INS_GMI: tag mask insert (ARMv8.5)
        case Kind::ARM64_INS_HINT:              return "hint";
        case Kind::ARM64_INS_HLT:               return "halt";
        case Kind::ARM64_INS_HVC:               return "hypervisor call";
        case Kind::ARM64_INS_IC:                return "instruction cache operation"; // alias of SYS
        case Kind::ARM64_INS_INS:               return "insert vector element";
        // case Kind::ARM64_INS_IRG: insert random tag (ARMv8.5)
        case Kind::ARM64_INS_ISB:               return "instruction synchronization barrier";
        case Kind::ARM64_INS_LD1:               return "load single-element structure(s)";
        case Kind::ARM64_INS_LD1R:              return "load one single-element structure and replicate to all lanes";
        case Kind::ARM64_INS_LD2:               return "load 2-element structure(s)";
        case Kind::ARM64_INS_LD2R:              return "load 2-element structure and replicate to all lanes";
        case Kind::ARM64_INS_LD3:               return "load 3-element structure(s)";
        case Kind::ARM64_INS_LD3R:              return "load 3-element structure and replicate to all lanes";
        case Kind::ARM64_INS_LD4:               return "load 4-element structure(s)";
        case Kind::ARM64_INS_LD4R:              return "load 4-element structure and replicate to all lanes";
        // case Kind::ARM64_INS_LDADD: atomic add on word or doubleword (ARMv8.1, used by STADD, STADDL)
        // case Kind::ARM64_INS_LDADDA: atomic add on word or doubleword (ARMv8.1, used by STADD, STADDL)
        // case Kind::ARM64_INS_LDADDAL: atomic add on word or doubleword (ARMv8.1, used by STADD, STADDL)
        // case Kind::ARM64_INS_LDADDL: atomic add on word or doubleword (ARMv8.1, used by STADD, STADDL)
        // case Kind::ARM64_INS_LDADDB: atomic add on word or doubleword (ARMv8.1, used by STADDB, STADDLB)
        // case Kind::ARM64_INS_LDADDAB: atomic add on word or doubleword (ARMv8.1, used by STADDB, STADDLB)
        // case Kind::ARM64_INS_LDADDALB: atomic add on word or doubleword (ARMv8.1, used by STADDB, STADDLB)
        // case Kind::ARM64_INS_LDADDLB: atomic add on word or doubleword (ARMv8.1, used by STADDB, STADDLB)
        // case Kind::ARM64_INS_LDADDH: atomic add on halfword (ARMv8.1, used by STADDH, STADDLH)
        // case Kind::ARM64_INS_LDADDAH: atomic add on halfword (ARMv8.1, used by STADDH, STADDLH)
        // case Kind::ARM64_INS_LDADDALH: atomic add on halfword (ARMv8.1, used by STADDH, STADDLH)
        // case Kind::ARM64_INS_LDADDLH: atomic add on halfword (ARMv8.1, used by STADDH, STADDLH)
        // case Kind::ARM64_INS_LDAPR: load-acquire RCpc register (ARMv8.3)
        // case Kind::ARM64_INS_LDAPRB: load-acquire RCpc register byte (ARMv8.3)
        // case Kind::ARM64_INS_LDAPRH: load-acquire RCpc register halfword (ARMv8.3)
        // case Kind::ARM64_INS_LDAPUR: load-acquire RCpc register
        // case Kind::ARM64_INS_LDAPURB: load-acquire RCpc register byte
        // case Kind::ARM64_INS_LDAPURH: load-acquire RCpc regsiter halfword
        // case Kind::ARM64_INS_LDAPURSB: load-acquire RCpc regsiter signed byte
        // case Kind::ARM64_INS_LDAPURSH: load-acquire RCpc register signed halfword
        // case Kind::ARM64_INS_LDAPURSW: load-acquire RCpc register signed word
        case Kind::ARM64_INS_LDAR:              return "load-acquire register";
        case Kind::ARM64_INS_LDARB:             return "load-acquire register byte";
        case Kind::ARM64_INS_LDARH:             return "load-acquire register halfword";
        case Kind::ARM64_INS_LDAXP:             return "load-aquire exclusive pair of registers";
        case Kind::ARM64_INS_LDAXR:             return "load-acquire exclusive register";
        case Kind::ARM64_INS_LDAXRB:            return "load-acquire exclusive register byte";
        case Kind::ARM64_INS_LDAXRH:            return "load-acquire exclusive register halfword";
        // case Kind::ARM64_INS_LDCLR: atomic bit clear on word or doubleword (ARMv8.1, used by STCLR, STCLRL)
        // case Kind::ARM64_INS_LDCLRA: atomic bit clear on word or doubleword (ARMv8.1, used by STCLR, STCLRL)
        // case Kind::ARM64_INS_LDCLRAL: atomic bit clear on word or doubleword (ARMv8.1, used by STCLR, STCLRL)
        // case Kind::ARM64_INS_LDCLRL: atomic bit clear on word or doubleword (ARMv8.1, used by STCLR, STCLRL)
        // case Kind::ARM64_INS_LDCLRB: atomic bit clear on byte (ARMv8.1, used by STCLRB, STCLRLB)
        // case Kind::ARM64_INS_LDCLRAB: atomic bit clear on byte (ARMv8.1, used by STCLRB, STCLRLB)
        // case Kind::ARM64_INS_LDCLRALB: atomic bit clear on byte (ARMv8.1, used by STCLRB, STCLRLB)
        // case Kind::ARM64_INS_LDCLRLB: atomic bit clear on byte (ARMv8.1, used by STCLRB, STCLRLB)
        // case Kind::ARM64_INS_LDCLRH: atomic bit clear on halfword (ARMv8.1, used by STCLRH, STCLRLH)
        // case Kind::ARM64_INS_LDCLRAH: atomic bit clear on halfword (ARMv8.1, used by STCLRH, STCLRLH)
        // case Kind::ARM64_INS_LDCLRALH: atomic bit clear on halfword (ARMv8.1, used by STCLRH, STCLRLH)
        // case Kind::ARM64_INS_LDCLRLH: atomic bit clear on halfword (ARMv8.1, used by STCLRH, STCLRLH)
        // case Kind::ARM64_INS_LDEOR: atomic exlusive OR on word or doubleword (ARMv8.1, used by STEOR, STEORL)
        // case Kind::ARM64_INS_LDEORA: atomic exlusive OR on word or doubleword (ARMv8.1, used by STEOR, STEORL)
        // case Kind::ARM64_INS_LDEORAL: atomic exlusive OR on word or doubleword (ARMv8.1, used by STEOR, STEORL)
        // case Kind::ARM64_INS_LDEORL: atomic exlusive OR on word or doubleword (ARMv8.1, used by STEOR, STEORL)
        // case Kind::ARM64_INS_LDEORB: atomic ex8sive OR on byte (ARMv8.1, used by STEORB, STEORLB)
        // case Kind::ARM64_INS_LDEORAB: atomic ex8sive OR on byte (ARMv8.1, used by STEORB, STEORLB)
        // case Kind::ARM64_INS_LDEORALB: atomic ex8sive OR on byte (ARMv8.1, used by STEORB, STEORLB)
        // case Kind::ARM64_INS_LDEORLB: atomic ex8sive OR on byte (ARMv8.1, used by STEORB, STEORLB)
        // case Kind::ARM64_INS_LDEORH: atomic exlusive OR on halfword (ARMv8.1, used by STEORH, STEORLH)
        // case Kind::ARM64_INS_LDEORAH: atomic exlusive OR on halfword (ARMv8.1, used by STEORH, STEORLH)
        // case Kind::ARM64_INS_LDEORALH: atomic exlusive OR on halfword (ARMv8.1, used by STEORH, STEORLH)
        // case Kind::ARM64_INS_LDEORLH: atomic exlusive OR on halfword (ARMv8.1, used by STEORH, STEORLH)
        // case Kind::ARM64_INS_LDG: load allocation tag (ARMv8.5)
        // case Kind::ARM64_INS_LDGV: load tag vector (ARMv8.5)
        // case Kind::ARM64_INS_LDLAR: load LOAcquire register (ARMv8.1)
        // case Kind::ARM64_INS_LDLARB: load LOAcquire register byte (ARMv8.1)
        // case Kind::ARM64_INS_LDLARH: load LOAcquire register halfword (ARMv8.1)
        case Kind::ARM64_INS_LDNP:              return "load pair of registers with non-temporal hint";
        case Kind::ARM64_INS_LDP:               return "load pair of registers";
        case Kind::ARM64_INS_LDPSW:             return "load pair of registers with signed word";
        case Kind::ARM64_INS_LDR:               return "load register";
        // case Kind::ARM64_INS_LDRAA: load register with pointer authentication (ARMv8.3)
        // case Kind::ARM64_INS_LDRAB: load register with pointer authentication (ARMv8.3)
        case Kind::ARM64_INS_LDRB:              return "load register byte";
        case Kind::ARM64_INS_LDRH:              return "load register halfword";
        case Kind::ARM64_INS_LDRSB:             return "load signed byte";
        case Kind::ARM64_INS_LDRSH:             return "load signed halfword";
        case Kind::ARM64_INS_LDRSW:             return "load register signed word";
        // case Kind::ARM64_INS_LDSET: atomic bit set on word or doubleword (ARMv8.1, used by STSET, STSETL)
        // case Kind::ARM64_INS_LDSETA: atomic bit set on word or doubleword (ARMv8.1, used by STSET, STSETL)
        // case Kind::ARM64_INS_LDSETAL: atomic bit set on word or doubleword (ARMv8.1, used by STSET, STSETL)
        // case Kind::ARM64_INS_LDSETL: atomic bit set on word or doubleword (ARMv8.1, used by STSET, STSETL)
        // case Kind::ARM64_INS_LDSETB: atomic bit set on byte (ARMv8.1, used by STSETB, STSETLB)
        // case Kind::ARM64_INS_LDSETAB: atomic bit set on byte (ARMv8.1, used by STSETB, STSETLB)
        // case Kind::ARM64_INS_LDSETALB: atomic bit set on byte (ARMv8.1, used by STSETB, STSETLB)
        // case Kind::ARM64_INS_LDSETLB: atomic bit set on byte (ARMv8.1, used by STSETB, STSETLB)
        // case Kind::ARM64_INS_LDSETH: atomic bit set on halfword (ARMv8.1, used by STSETH, STSETLH)
        // case Kind::ARM64_INS_LDSETAH: atomic bit set on halfword (ARMv8.1, used by STSETH, STSETLH)
        // case Kind::ARM64_INS_LDSETALH: atomic bit set on halfword (ARMv8.1, used by STSETH, STSETLH)
        // case Kind::ARM64_INS_LDSETLH: atomic bit set on halfword (ARMv8.1, used by STSETH, STSETLH)
        // case Kind::ARM64_INS_LDSMAX: atomic signed maximum on word or doubleword (ARMv8.1, used by STSMAX, STSMAXL)
        // case Kind::ARM64_INS_LDSMAXA: atomic signed maximum on word or doubleword (ARMv8.1, used by STSMAX, STSMAXL)
        // case Kind::ARM64_INS_LDSMAXAL: atomic signed maximum on word or doubleword (ARMv8.1, used by STSMAX, STSMAXL)
        // case Kind::ARM64_INS_LDSMAXL: atomic signed maximum on word or doubleword (ARMv8.1, used by STSMAX, STSMAXL)
        // case Kind::ARM64_INS_LDSMAXB: atomic signed maximum on byte (ARMv8.1, used by STSMAXB, STSMAXLB)
        // case Kind::ARM64_INS_LDSMAXAB: atomic signed maximum on byte (ARMv8.1, used by STSMAXB, STSMAXLB)
        // case Kind::ARM64_INS_LDSMAXALB: atomic signed maximum on byte (ARMv8.1, used by STSMAXB, STSMAXLB)
        // case Kind::ARM64_INS_LDSMAXLB: atomic signed maximum on byte (ARMv8.1, used by STSMAXB, STSMAXLB)
        // case Kind::ARM64_INS_LDSMAXH: atomic signed maximum on halfword (ARMv8.1, used by STSMAXH, STSMAXLH)
        // case Kind::ARM64_INS_LDSMAXAH: atomic signed maximum on halfword (ARMv8.1, used by STSMAXH, STSMAXLH)
        // case Kind::ARM64_INS_LDSMAXALH: atomic signed maximum on halfword (ARMv8.1, used by STSMAXH, STSMAXLH)
        // case Kind::ARM64_INS_LDSMAXLH: atomic signed maximum on halfword (ARMv8.1, used by STSMAXH, STSMAXLH)
        // case Kind::ARM64_INS_LDSMIN: atomic signed minimum on word or doubleword (ARMv8.1, used by STSMIN, STSMINL)
        // case Kind::ARM64_INS_LDSMINA: atomic signed minimum on word or doubleword (ARMv8.1, used by STSMIN, STSMINL)
        // case Kind::ARM64_INS_LDSMINAL: atomic signed minimum on word or doubleword (ARMv8.1, used by STSMIN, STSMINL)
        // case Kind::ARM64_INS_LDSMINL: atomic signed minimum on word or doubleword (ARMv8.1, used by STSMIN, STSMINL)
        // case Kind::ARM64_INS_LDSMINB: atomic signed minimum on byte (ARMv8.1, used by STSMINB, STSMINLB)
        // case Kind::ARM64_INS_LDSMINAB: atomic signed minimum on byte (ARMv8.1, used by STSMINB, STSMINLB)
        // case Kind::ARM64_INS_LDSMINALB: atomic signed minimum on byte (ARMv8.1, used by STSMINB, STSMINLB)
        // case Kind::ARM64_INS_LDSMINLB: atomic signed minimum on byte (ARMv8.1, used by STSMINB, STSMINLB)
        // case Kind::ARM64_INS_LDSMINH: atomic signed minimum on halfword (ARMv8.1, used by STSMINH, STSMINLH)
        // case Kind::ARM64_INS_LDSMINAH: atomic signed minimum on halfword (ARMv8.1, used by STSMINH, STSMINLH)
        // case Kind::ARM64_INS_LDSMINALH: atomic signed minimum on halfword (ARMv8.1, used by STSMINH, STSMINLH)
        // case Kind::ARM64_INS_LDSMINLH: atomic signed minimum on halfword (ARMv8.1, used by STSMINH, STSMINLH)
        case Kind::ARM64_INS_LDTR:              return "load register";
        case Kind::ARM64_INS_LDTRB:             return "load register byte";
        case Kind::ARM64_INS_LDTRH:             return "load register halfword";
        case Kind::ARM64_INS_LDTRSB:            return "load register signed byte";
        case Kind::ARM64_INS_LDTRSH:            return "load register signed halfword";
        case Kind::ARM64_INS_LDTRSW:            return "load register signed word";
        // case Kind::ARM64_INS_LDUMAX: atomic unsigned maximum on word or doubleword (ARMv8.1, used by STUMAX, STUMAXL)
        // case Kind::ARM64_INS_LDUMAXA: atomic unsigned maximum on word or doubleword (ARMv8.1, used by STUMAX, STUMAXL)
        // case Kind::ARM64_INS_LDUMAXAL: atomic unsigned maximum on word or doubleword (ARMv8.1, used by STUMAX, STUMAXL)
        // case Kind::ARM64_INS_LDUMAXL: atomic unsigned maximum on word or doubleword (ARMv8.1, used by STUMAX, STUMAXL)
        // case Kind::ARM64_INS_LDUMAXB: atomic unsigned maximum on byte (ARMv8.1, used by STUMAXB, STUMAXLB)
        // case Kind::ARM64_INS_LDUMAXAB: atomic unsigned maximum on byte (ARMv8.1, used by STUMAXB, STUMAXLB)
        // case Kind::ARM64_INS_LDUMAXALB: atomic unsigned maximum on byte (ARMv8.1, used by STUMAXB, STUMAXLB)
        // case Kind::ARM64_INS_LDUMAXLB: atomic unsigned maximum on byte (ARMv8.1, used by STUMAXB, STUMAXLB)
        // case Kind::ARM64_INS_LDUMAXH: atomic unsigned maximum on halfword (ARMv8.1, used by STUMAXH, STUMAXLH)
        // case Kind::ARM64_INS_LDUMAXAH: atomic unsigned maximum on halfword (ARMv8.1, used by STUMAXH, STUMAXLH)
        // case Kind::ARM64_INS_LDUMAXALH: atomic unsigned maximum on halfword (ARMv8.1, used by STUMAXH, STUMAXLH)
        // case Kind::ARM64_INS_LDUMAXLH: atomic unsigned maximum on halfword (ARMv8.1, used by STUMAXH, STUMAXLH)
        // case Kind::ARM64_INS_LDUMIN: atomic unsigned minimum on word or doubleword (ARMv8.1, used by STUMIN, STUMINL)
        // case Kind::ARM64_INS_LDUMINA: atomic unsigned minimum on word or doubleword (ARMv8.1, used by STUMIN, STUMINL)
        // case Kind::ARM64_INS_LDUMINAL: atomic unsigned minimum on word or doubleword (ARMv8.1, used by STUMIN, STUMINL)
        // case Kind::ARM64_INS_LDUMINL: atomic unsigned minimum on word or doubleword (ARMv8.1, used by STUMIN, STUMINL)
        // case Kind::ARM64_INS_LDUMINB: atomic unsigned minimum on byte (ARMv8.1, used by STUMINB, STUMINLB)
        // case Kind::ARM64_INS_LDUMINAB: atomic unsigned minimum on byte (ARMv8.1, used by STUMINB, STUMINLB)
        // case Kind::ARM64_INS_LDUMINALB: atomic unsigned minimum on byte (ARMv8.1, used by STUMINB, STUMINLB)
        // case Kind::ARM64_INS_LDUMINLB: atomic unsigned minimum on byte (ARMv8.1, used by STUMINB, STUMINLB)
        // case Kind::ARM64_INS_LDUMINH: atomic unsigned minimum on halfword (ARMv8.1, used by STUMINH, STUMINLH)
        // case Kind::ARM64_INS_LDUMINAH: atomic unsigned minimum on halfword (ARMv8.1, used by STUMINH, STUMINLH)
        // case Kind::ARM64_INS_LDUMINALH: atomic unsigned minimum on halfword (ARMv8.1, used by STUMINH, STUMINLH)
        // case Kind::ARM64_INS_LDUMINLH: atomic unsigned minimum on halfword (ARMv8.1, used by STUMINH, STUMINLH)
        case Kind::ARM64_INS_LDUR:              return "load register";
        case Kind::ARM64_INS_LDURB:             return "load register byte";
        case Kind::ARM64_INS_LDURH:             return "load register halfword";
        case Kind::ARM64_INS_LDURSB:            return "load register signed byte";
        case Kind::ARM64_INS_LDURSH:            return "load register signed halfword";
        case Kind::ARM64_INS_LDURSW:            return "load register signed word";
        case Kind::ARM64_INS_LDXP:              return "load exclusive pair of registers";
        case Kind::ARM64_INS_LDXR:              return "load exclusive register";
        case Kind::ARM64_INS_LDXRB:             return "load exclusive register byte";
        case Kind::ARM64_INS_LDXRH:             return "load exclusive register halfword";
        case Kind::ARM64_INS_LSL:               return "logical shift left";
        // case Kind::ARM64_INS_LSLV: logical shift left variable (used by LSL)
        case Kind::ARM64_INS_LSR:               return "logical shift right";
        // case Kind::ARM64_INS_LSRV: logical shift right variable (used by LSR)
        case Kind::ARM64_INS_MADD:              return "multiply-add";
        case Kind::ARM64_INS_MLA:               return "multiply-add";
        case Kind::ARM64_INS_MLS:               return "multiply-subtract";
        case Kind::ARM64_INS_MNEG:              return "multiply-negate"; // alias of MSUB
        case Kind::ARM64_INS_MOV:               return "move";
        case Kind::ARM64_INS_MOVI:              return "move immediate";
        case Kind::ARM64_INS_MOVK:              return "move wide with keep";
        case Kind::ARM64_INS_MOVN:              return "move wide with NOT";
        case Kind::ARM64_INS_MOVZ:              return "move wide with zero";
        case Kind::ARM64_INS_MRS:               return "move from system register";
        case Kind::ARM64_INS_MSR:               return "move to system register";
        case Kind::ARM64_INS_MSUB:              return "multiply-subtract";
        case Kind::ARM64_INS_MUL:               return "multiply";
        case Kind::ARM64_INS_MVN:               return "bitwise NOT"; // alias of NOT
        case Kind::ARM64_INS_MVNI:              return "move inverted immediate";
        case Kind::ARM64_INS_NEG:               return "negate";
        case Kind::ARM64_INS_NEGS:              return "negate, setting flags";
        case Kind::ARM64_INS_NGC:               return "negate with carry"; // alias of SBC
        case Kind::ARM64_INS_NGCS:              return "negate with carry, setting flags"; // alias of SBCS
        case Kind::ARM64_INS_NOP:               return "no operation";
        case Kind::ARM64_INS_NOT:               return "bitwise NOT";
        case Kind::ARM64_INS_ORN:               return "bitwise OR NOT";
        case Kind::ARM64_INS_ORR:               return "bitwise OR";
        // case Kind::ARM64_INS_PACDA: pointer authentication code for data addresses using key A (ARMv8.3)
        // case Kind::ARM64_INS_PACDZA: pointer authentication code for data addresses using key A (ARMv8.3)
        // case Kind::ARM64_INS_PACDB: pointer authentication code for data address using key B (ARMv8.3)
        // case Kind::ARM64_INS_PACDZB: pointer authentication code for data address using key B (ARMv8.3)
        // case Kind::ARM64_INS_PACGA: pointer authentication code using generic key (ARMv8.3)
        // case Kind::ARM64_INS_PACIA: pointer authentication code for instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_PACIA1716: pointer authentication code for instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_PACIASP: pointer authentication code for instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_PACIAZ: pointer authentication code for instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_PACIZA: pointer authentication code for instruction address using key A (ARMv8.3)
        // case Kind::ARM64_INS_PACIB: pointer authentication code for instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_PACIB1716: pointer authentication code for instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_PACIBSP: pointer authentication code for instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_PACIBZ: pointer authentication code for instruction address using key B (ARMv8.3)
        // case Kind::ARM64_INS_PACIZB: pointer authentication code for instruction address using key B (ARMv8.3)
        case Kind::ARM64_INS_PMUL:              return "polynomial multiply";
        case Kind::ARM64_INS_PMULL:             return "polynomial multiply long";
        case Kind::ARM64_INS_PMULL2:            return "polynomial multiply long";
        case Kind::ARM64_INS_PRFM:              return "prefetch memory";
        case Kind::ARM64_INS_PRFUM:             return "prefetch memory";
        // case Kind::ARM64_INS_PSB_CSYNC: profiling synchronization barrier (ARMv8.2)
        // case Kind::ARM64_INS_PSSBB: physical speculative store bypass barrier
        case Kind::ARM64_INS_RADDHN:            return "rounding add returning high narrow";
        case Kind::ARM64_INS_RADDHN2:           return "rounding add returning high narrow";
        // case Kind::ARM64_INS_RAX1: rotate and exclusive OR (ARMv8.2)
        case Kind::ARM64_INS_RBIT:              return "reverse bits";
        case Kind::ARM64_INS_RET:               return "return from subroutine";
        // case Kind::ARM64_INS_RETAA: return from subroutine with pointer authentication (ARMv8.3)
        // case Kind::ARM64_INS_RETAB: return from subroutine with pointer authentication (ARMv8.3)
        case Kind::ARM64_INS_REV:               return "reverse bytes";
        case Kind::ARM64_INS_REV16:             return "reverse bytes in 16-bit halfwords";
        case Kind::ARM64_INS_REV32:             return "reverse bytes in 32-bit words";
        case Kind::ARM64_INS_REV64:             return "reverse bytes";
        // case Kind::ARM64_INS_RMIF: rotation right and masked save (ARMv8.4)
        case Kind::ARM64_INS_ROR:               return "rotate right";
        // case Kind::ARM64_INS_RORV: rotate right variable (used by ROR)
        case Kind::ARM64_INS_RSHRN:             return "rounding shift right narrow";
        case Kind::ARM64_INS_RSHRN2:            return "rounding shift right narrow";
        case Kind::ARM64_INS_RSUBHN:            return "rounding subtract returning high narrow";
        case Kind::ARM64_INS_RSUBHN2:           return "rounding subtract returning high narrow";
        case Kind::ARM64_INS_SABA:              return "signed absolute difference and accumulate";
        case Kind::ARM64_INS_SABAL:             return "signed absolute difference and accumulate long";
        case Kind::ARM64_INS_SABAL2:            return "signed absolute difference and accumulate long";
        case Kind::ARM64_INS_SABD:              return "signed absolute difference";
        case Kind::ARM64_INS_SABDL:             return "signed absolute difference long";
        case Kind::ARM64_INS_SABDL2:            return "signed absolute difference long";
        case Kind::ARM64_INS_SADALP:            return "signed add and accumulate long pairwise";
        case Kind::ARM64_INS_SADDL:             return "signed add long";
        case Kind::ARM64_INS_SADDL2:            return "signed add long";
        case Kind::ARM64_INS_SADDLP:            return "signed add long pairwise";
        case Kind::ARM64_INS_SADDLV:            return "signed add long across vector";
        case Kind::ARM64_INS_SADDW:             return "signed add wide";
        case Kind::ARM64_INS_SADDW2:            return "signed add wide";
        // case Kind::ARM64_INS_SB: speculation barrier
        case Kind::ARM64_INS_SBC:               return "subtract with carry";
        // case Kind::ARM64_INS_SBCS: subtract with carry, setting flags
        case Kind::ARM64_INS_SBFIZ:             return "signed bitfield insert in zeros"; // alias of SBFM
        case Kind::ARM64_INS_SBFM:              return "signed bitfield move";
        case Kind::ARM64_INS_SBFX:              return "signed bitfield extract"; // alias of SBFM
        case Kind::ARM64_INS_SCVTF:             return "signed fixed-point convert fo floating-point";
        case Kind::ARM64_INS_SDIV:              return "signed divide";
        // case Kind::ARM64_INS_SDOT: signed dot product (ARMv8.2)
        // case Kind::ARM64_INS_SETF8: set the PSTATE.NZV flags (ARMv8.4)
        // case Kind::ARM64_INS_SETF16: set the PSTATE.NZV flags (ARMv8.4)
        case Kind::ARM64_INS_SEV:               return "send event";
        case Kind::ARM64_INS_SEVL:              return "send event local";
        case Kind::ARM64_INS_SHA1C:             return "SHA1 hash update, choose";
        case Kind::ARM64_INS_SHA1H:             return "SHA1 fixed rotate";
        case Kind::ARM64_INS_SHA1M:             return "SHA1 hash update, majority";
        case Kind::ARM64_INS_SHA1P:             return "SHA1 hash update, parity";
        case Kind::ARM64_INS_SHA1SU0:           return "SHA1 schedule update 0";
        case Kind::ARM64_INS_SHA1SU1:           return "SHA1 schedule update 1";
        case Kind::ARM64_INS_SHA256H:           return "SHA256 hash update, part 1";
        case Kind::ARM64_INS_SHA256H2:          return "SHA256 hash update, part 2";
        case Kind::ARM64_INS_SHA256SU0:         return "SHA256 schedule update 0";
        case Kind::ARM64_INS_SHA256SU1:         return "SHA256 schedule update 1";
        // case Kind::ARM64_INS_SHA512H: SHA512 hash update, part 1 (ARMv8.2)
        // case Kind::ARM64_INS_SHA512H2: SHA512 hash update part 2 (ARMv8.2)
        // case Kind::ARM64_INS_SHA512SU0: SHA512 schedule update 0
        // case Kind::ARM64_INS_SHA512SU1: SHA512 schedule update 1
        case Kind::ARM64_INS_SHADD:             return "signed halving add";
        case Kind::ARM64_INS_SHL:               return "shift left";
        case Kind::ARM64_INS_SHLL:              return "shift left long";
        case Kind::ARM64_INS_SHLL2:             return "shift left long";
        case Kind::ARM64_INS_SHRN:              return "shift right narrow";
        case Kind::ARM64_INS_SHRN2:             return "shift right narrow";
        case Kind::ARM64_INS_SHSUB:             return "signed halving subtract";
        case Kind::ARM64_INS_SLI:               return "shift left and insert";
        // case Kind::ARM64_INS_SM3PARTW1: no succinct description (ARMv8.2)
        // case Kind::ARM64_INS_SM3PARTW2: no succinct description (ARMv8.2)
        // case Kind::ARM64_INS_SM3SS1: no succinct description (ARMv8.2)
        // case Kind::ARM64_INS_SM3TT1A: no succinct description (ARMv8.2)
        // case Kind::ARM64_INS_SM3TT1B: no succinct description (ARMv8.2)
        // case Kind::ARM64_INS_SM3TT2A: no succinct description (ARMv8.2)
        // case Kind::ARM64_INS_SM3TT2B: no succinct description (ARMv8.2)
        // case Kind::ARM64_INS_SM4E: no succinct description (ARMv8.2)
        // case Kind::ARM64_INS_SM4EKEY: no succinct description (ARMv8.2)
        case Kind::ARM64_INS_SMADDL:            return "signed multiply-add long";
        case Kind::ARM64_INS_SMAX:              return "signed maximum";
        case Kind::ARM64_INS_SMAXP:             return "signed maximum pairwise";
        case Kind::ARM64_INS_SMAXV:             return "signed maximum across vector";
        case Kind::ARM64_INS_SMC:               return "secure monitor call";
        case Kind::ARM64_INS_SMIN:              return "signed minimum";
        case Kind::ARM64_INS_SMINP:             return "signed minimum pairwise";
        case Kind::ARM64_INS_SMINV:             return "signed minimum across vector";
        case Kind::ARM64_INS_SMLAL:             return "signed multiply-add long";
        case Kind::ARM64_INS_SMLAL2:            return "signed multiply-add long";
        case Kind::ARM64_INS_SMLSL:             return "singed multiply-subtract long";
        case Kind::ARM64_INS_SMLSL2:            return "signed multiply-subtract long";
        case Kind::ARM64_INS_SMNEGL:            return "signed multiply-negate long"; // alias of SMSUBL
        case Kind::ARM64_INS_SMOV:              return "signed move";
        case Kind::ARM64_INS_SMSUBL:            return "signed multiply-subtract long";
        case Kind::ARM64_INS_SMULH:             return "signed multiply high";
        case Kind::ARM64_INS_SMULL:             return "signed multiply long";
        case Kind::ARM64_INS_SMULL2:            return "signed multiply long";
        case Kind::ARM64_INS_SQABS:             return "signed saturating absolute value";
        case Kind::ARM64_INS_SQADD:             return "signed saturating add";
        case Kind::ARM64_INS_SQDMLAL:           return "signed saturating doubling multiply-add long";
        case Kind::ARM64_INS_SQDMLAL2:          return "signed saturating doubling multiply-add long";
        case Kind::ARM64_INS_SQDMLSL:           return "signed saturating doubling multiply-subtract long";
        case Kind::ARM64_INS_SQDMLSL2:          return "signed saturating doubling multiply-subtract long";
        case Kind::ARM64_INS_SQDMULH:           return "signed saturating doubling multiply returning high half";
        case Kind::ARM64_INS_SQDMULL:           return "signed saturating doubling multiply long";
        case Kind::ARM64_INS_SQDMULL2:          return "signed saturating doubling multiply long";
        case Kind::ARM64_INS_SQNEG:             return "signed saturating negate";
        // case Kind::ARM64_INS_SQRDMLAH: signed saturating rounding doubling multiply accumulate returning high half (ARMv8.1)
        // case Kind::ARM64_INS_SQRDMLSH: signed saturating rounding doubling multiply subtract returning high half (ARMv8.1)
        case Kind::ARM64_INS_SQRDMULH:          return "signed saturating rounding doubling multiply";
        case Kind::ARM64_INS_SQRSHL:            return "signed saturating rounding shift left";
        case Kind::ARM64_INS_SQRSHRN:           return "signed saturating rounded shift right narrow";
        case Kind::ARM64_INS_SQRSHRN2:          return "signed saturating rounded shift right narrow";
        case Kind::ARM64_INS_SQRSHRUN:          return "signed saturating rounded shift right unsigned narrow";
        case Kind::ARM64_INS_SQRSHRUN2:         return "signed saturating rounded shift right unsigned narrow";
        case Kind::ARM64_INS_SQSHL:             return "signed saturating shift left";
        case Kind::ARM64_INS_SQSHLU:            return "signed saturating shift left unsigned";
        case Kind::ARM64_INS_SQSHRN:            return "signed saturating shift right narrow";
        case Kind::ARM64_INS_SQSHRN2:           return "signed saturating shift right narrow";
        case Kind::ARM64_INS_SQSHRUN:           return "signed saturating shift right unsigned narrow";
        case Kind::ARM64_INS_SQSHRUN2:          return "signed saturating shift right unsigned narrow";
        case Kind::ARM64_INS_SQSUB:             return "signed saturating subtract";
        case Kind::ARM64_INS_SQXTN:             return "signed saturating extract narrow";
        case Kind::ARM64_INS_SQXTN2:            return "signed saturating extract narrow";
        case Kind::ARM64_INS_SQXTUN:            return "signed saturating extract unsigned narrow";
        case Kind::ARM64_INS_SQXTUN2:           return "signed saturating extract unsigned narrow";
        case Kind::ARM64_INS_SRHADD:            return "signed roudning halving add";
        case Kind::ARM64_INS_SRI:               return "shift right and insert";
        case Kind::ARM64_INS_SRSHL:             return "signed rounding shift left";
        case Kind::ARM64_INS_SRSHR:             return "signed rounding shift right";
        case Kind::ARM64_INS_SRSRA:             return "signed rounding shift right and accumulate";
        // case Kind::ARM64_INS_SSBB: speculative store bypass barrier
        case Kind::ARM64_INS_SSHL:              return "signed shift left";
        case Kind::ARM64_INS_SSHLL:             return "signed shift left long";
        case Kind::ARM64_INS_SSHLL2:            return "signed shift left long";
        case Kind::ARM64_INS_SSHR:              return "signed shift right";
        case Kind::ARM64_INS_SSRA:              return "signed shift right and accumulate";
        case Kind::ARM64_INS_SSUBL:             return "signed subtract long";
        case Kind::ARM64_INS_SSUBL2:            return "signed subtract long";
        case Kind::ARM64_INS_SSUBW:             return "signed subtract wide";
        case Kind::ARM64_INS_SSUBW2:            return "signed subtract wide";
        case Kind::ARM64_INS_ST1:               return "store single-element structure(s)";
        case Kind::ARM64_INS_ST2:               return "store 2-element structure(s)";
        // case Kind::ARM64_INS_ST2G: store allocation tags (ARMv8.5)
        case Kind::ARM64_INS_ST3:               return "store 3-element structure(s)";
        case Kind::ARM64_INS_ST4:               return "store 4-element structure(s)";
        // case Kind::ARM64_INS_STADD: atomic add on word or doubleword (ARMv8.1, alias of LDADD, LDADDA, LDADDAL, LDADDL)
        // case Kind::ARM64_INS_STADDL: atomic add on word or doubleword (ARMv8.1, alias of LDADD, LDADDA, LDADDAL, LDADDL)
        // case Kind::ARM64_INS_STADDB: atomic add on byte (ARMv8.1, alias of LDADDB, LDADDAB, LDADDALB, LDADDLB)
        // case Kind::ARM64_INS_STADDLB: atomic add on byte (ARMv8.1, alias of LDADDB, LDADDAB, LDADDALB, LDADDLB)
        // case Kind::ARM64_INS_STADDH: atomic add on halfword (ARMv8.1, alias of LDADDH, LDADDAH, LDADDALH, LDADDLH)
        // case Kind::ARM64_INS_STADDLH: atomic add on halfword (ARMv8.1, alias of LDADDH, LDADDAH, LDADDALH, LDADDLH)
        // case Kind::ARM64_INS_STCRL: atomic bit clear on word or doubleword (ARMv8.1, alias of LDCLR, LDCLRA, LDCLRAL, LDCLRL)
        // case Kind::ARM64_INS_STCLRL: atomic bit clear on word or doubleword (ARMv8.1, alias of LDCLR, LDCLRA, LDCLRAL, LDCLRL)
        // case Kind::ARM64_INS_STCLRB: atomic bit clear on byte (ARMv8.1, alias of LDCLRB, LDCLRAB, LDCLRALB, LDCLRLB)
        // case Kind::ARM64_INS_STCLRLB: atomic bit clear on byte (ARMv8.1, alias of LDCLRB, LDCLRAB, LDCLRALB, LDCLRLB)
        // case Kind::ARM64_INS_STCLRH: atomic bit clear on halfword (ARMv8.1, alias of LDCLRH, LDCLRAH, LDCLRALH, LDCLRLH)
        // case Kind::ARM64_INS_STCLRLH: atomic bit clear on halfword (ARMv8.1, alias of LDCLRH, LDCLRAH, LDCLRALH, LDCLRLH)
        // case Kind::ARM64_INS_STEOR: atomic exlusive OR on word or doubleword (ARMv8.1, alias of LDEOR, LDEORA, LDEORAL, LDEORL)
        // case Kind::ARM64_INS_STEORL: atomic exlusive OR on word or doubleword (ARMv8.1, alias of LDEOR, LDEORA, LDEORAL, LDEORL)
        // case Kind::ARM64_INS_STEORB: atomic exlusive OR on byte (ARMv8.1, alias of LDEORB, LDEORAB, LDEORALB, LDEORLB)
        // case Kind::ARM64_INS_STEORLB: atomic exlusive OR on byte (ARMv8.1, alias of LDEORB, LDEORAB, LDEORALB, LDEORLB)
        // case Kind::ARM64_INS_STEORH: atomic exlusive OR on halfword (ARMv8.1, alias of LDEORH, LDEORAH, LDEORALH, LDEORLH)
        // case Kind::ARM64_INS_STG: store allocatio tag (ARMv8.5)
        // case Kind::ARM64_INS_STGP: store allocation tag and pair (ARMv8.5)
        // case Kind::ARM64_INS_STGV: store tag vector (ARMv8.5)
        // case Kind::ARM64_INS_STLLR: store LO release register (ARMv8.1)
        // case Kind::ARM64_INS_STLLRB: store LO release register byte (ARMv8.1)
        // case Kind::ARM64_INS_STLLRH: store LO release register halfword (ARMv8.1)
        case Kind::ARM64_INS_STLR:              return "store-release register";
        case Kind::ARM64_INS_STLRB:             return "store-release register byte";
        case Kind::ARM64_INS_STLRH:             return "store-release register halfword";
        // case Kind::ARM64_INS_STLUR: store-release register unscaled
        // case Kind::ARM64_INS_STLURB: store-release register byte
        // case Kind::ARM64_INS_STLURH: store-release register halfword
        case Kind::ARM64_INS_STLXP:             return "store-release exclusive pair of registers";
        case Kind::ARM64_INS_STLXR:             return "store-release exclusive register";
        case Kind::ARM64_INS_STLXRB:            return "store-release exclusive register byte";
        case Kind::ARM64_INS_STLXRH:            return "store-release exclusive register halfword";
        case Kind::ARM64_INS_STNP:              return "store pair of registers with non-temporal hint";
        case Kind::ARM64_INS_STP:               return "store pair of registers";
        case Kind::ARM64_INS_STR:               return "store register";
        case Kind::ARM64_INS_STRB:              return "store register byte";
        case Kind::ARM64_INS_STRH:              return "store register halfword";
        // case Kind::ARM64_INS_STSET: atomic bit set on word or doubleword (ARMv8.1, alias of LDSET, LDSETA, LDSETAL, LDSETL)
        // case Kind::ARM64_INS_STSETL: atomic bit set on word or doubleword (ARMv8.1, alias of LDSET, LDSETA, LDSETAL, LDSETL)
        // case Kind::ARM64_INS_STSETB: atomic bit set on byte (ARMv8.1, alias of LDSETB, LDSETAB, LDSETALB, LDSETLB)
        // case Kind::ARM64_INS_STSETLB: atomic bit set on byte (ARMv8.1, alias of LDSETB, LDSETAB, LDSETALB, LDSETLB)
        // case Kind::ARM64_INS_STSETH: atomic bit set on halfword (ARMv8.1, alias of LDSETH, LDSETAH, LDSETALH, LDSETLH)
        // case Kind::ARM64_INS_STSETLH: atomic bit set on halfword (ARMv8.1, alias of LDSETH, LDSETAH, LDSETALH, LDSETLH)
        // case Kind::ARM64_INS_STSMAX: atomic signed maximum on word or doubleword (ARMv8.1, alias of LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL)
        // case Kind::ARM64_INS_STSMAXL: atomic signed maximum on word or doubleword (ARMv8.1, alias of LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL)
        // case Kind::ARM64_INS_STSMAXB: atomic signed maximum on byte (ARMv8.1, alias of LDSMAXB, LDSAMXAB, LDSMAXALB, LDSMAXLB)
        // case Kind::ARM64_INS_STSMAXLB: atomic signed maximum on byte (ARMv8.1, alias of LDSMAXB, LDSAMXAB, LDSMAXALB, LDSMAXLB)
        // case Kind::ARM64_INS_STSMAXH: atomic signed maximum on halfword (ARMv8.1, alias of LDSMAXH, LDSMAXAH, LDSMAXALH, LDSMAXLH)
        // case Kind::ARM64_INS_STSMAXLH: atomic signed maximum on halfword (ARMv8.1, alias of LDSMAXH, LDSMAXAH, LDSMAXALH, LDSMAXLH)
        // case Kind::ARM64_INS_STSMIN: atomic signed minimum on word or doubleword (ARMv8.1, alias of LDSMIN, LDSMINA, LDSMINAL, LDSMINL)
        // case Kind::ARM64_INS_STSMINL: atomic signed minimum on word or doubleword (ARMv8.1, alias of LDSMIN, LDSMINA, LDSMINAL, LDSMINL)
        // case Kind::ARM64_INS_STSMINB: atomic signed minimum on byte (ARMv8.1, alias of LDSMINB, LDSMINAB, LDSMINALB, LDSMINLB)
        // case Kind::ARM64_INS_STSMINLB: atomic signed minimum on byte (ARMv8.1, alias of LDSMINB, LDSMINAB, LDSMINALB, LDSMINLB)
        // case Kind::ARM64_INS_STSMINH: atomic signed minimum on halfword (ARMv8.1, alias of LDSMINH, LDSMINAH, LDSMINALH, LDSMINLH)
        // case Kind::ARM64_INS_STSMINLH: atomic signed minimum on halfword (ARMv8.1, alias of LDSMINH, LDSMINAH, LDSMINALH, LDSMINLH)
        case Kind::ARM64_INS_STTR:              return "store register";
        case Kind::ARM64_INS_STTRB:             return "store register byte";
        case Kind::ARM64_INS_STTRH:             return "store register halfword";
        // case Kind::ARM64_INS_STUMAX: atomic unsigned maximum on word or doubleword (ARMv8.1, alias of LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL)
        // case Kind::ARM64_INS_STUMAXL: atomic unsigned maximum on word or doubleword (ARMv8.1, alias of LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL)
        // case Kind::ARM64_INS_STUMAXB: atomic unsigned maximum o byte (ARMv8.1, alias of LDUMAXB, LDUMAXAB, LDUMAXALB, LDUMAXLB)
        // case Kind::ARM64_INS_STUMAXLB: atomic unsigned maximum o byte (ARMv8.1, alias of LDUMAXB, LDUMAXAB, LDUMAXALB, LDUMAXLB)
        // case Kind::ARM64_INS_STUMAXH: atomic unsigned maximum on halfword (ARMv8.1, alias of LDUMAXH, LDUMAXAH, LDUMAXALH, LDUMAXLH)
        // case Kind::ARM64_INS_STUMAXLH: atomic unsigned maximum on halfword (ARMv8.1, alias of LDUMAXH, LDUMAXAH, LDUMAXALH, LDUMAXLH)
        // case Kind::ARM64_INS_STUMIN: atomic unsigend minmum on word or doubleword (ARMv8.1, alias of LDUMIN, LDUMINA, LDUMINAL, LDUMINL)
        // case Kind::ARM64_INS_STUMINL: atomic unsigend minmum on word or doubleword (ARMv8.1, alias of LDUMIN, LDUMINA, LDUMINAL, LDUMINL)
        // case Kind::ARM64_INS_STUMINB: atomic unsigned minimum on byte (ARMv8.1, alias of LDUMINB, LDUMINAB, LDUMINALB, LDUMINLB)
        // case Kind::ARM64_INS_STUMINLB: atomic unsigned minimum on byte (ARMv8.1, alias of LDUMINB, LDUMINAB, LDUMINALB, LDUMINLB)
        // case Kind::ARM64_INS_STUMINH: atomic unsigned minum on halfword (ARMv8.1, alias of LDUMINH, LDUMINAH, LDUMINALH, LDUMINLH)
        // case Kind::ARM64_INS_STUMINLH: atomic unsigned minum on halfword (ARMv8.1, alias of LDUMINH, LDUMINAH, LDUMINALH, LDUMINLH)
        case Kind::ARM64_INS_STUR:              return "store register";
        case Kind::ARM64_INS_STURB:             return "store register byte";
        case Kind::ARM64_INS_STURH:             return "store register halfword";
        case Kind::ARM64_INS_STXP:              return "store exclusive pair of registers";
        case Kind::ARM64_INS_STXR:              return "store exclusive register";
        case Kind::ARM64_INS_STXRB:             return "store exclusive register byte";
        case Kind::ARM64_INS_STXRH:             return "store exclusive register halfword";
        // case Kind::ARM64_INS_STZ2G: no succinct description (ARMv8.5)
        // case Kind::ARM64_INS_STZG: no succinct description (ARMv8.5)
        case Kind::ARM64_INS_SUB:               return "subtract";
        // case Kind::ARM64_INS_SUBG: subtract with tag (ARMv8.5)
        case Kind::ARM64_INS_SUBHN:             return "subtract returning high narrow";
        case Kind::ARM64_INS_SUBHN2:            return "subtract returning high narrow";
        // case Kind::ARM64_INS_SUBP: subtract pointer (ARMv8.5)
        // case Kind::ARM64_INS_SUBPS: subtract pointer, setting flags (ARMv8.5)
        // case Kind::ARM64_INS_SUBS: subtact, setting flags
        case Kind::ARM64_INS_SUQADD:            return "signed saturating accumulate of unsigned value";
        case Kind::ARM64_INS_SVC:               return "supervisor call app to OS";
        // case Kind::ARM64_INS_SWP: swap word or doubleword (ARMv8.1)
        // case Kind::ARM64_INS_SWPA: swap word or doubleword (ARMv8.1)
        // case Kind::ARM64_INS_SWPAL: swap word or doubleword (ARMv8.1)
        // case Kind::ARM64_INS_SWPL: swap word or doubleword (ARMv8.1)
        // case Kind::ARM64_INS_SWPB: swap byte (ARMv8.1)
        // case Kind::ARM64_INS_SWPAB: swap byte (ARMv8.1)
        // case Kind::ARM64_INS_SWPALB: swap byte (ARMv8.1)
        // case Kind::ARM64_INS_SWPLB: swap byte (ARMv8.1)
        // case Kind::ARM64_INS_SWPH: swap halfword (ARMv8.1)
        // case Kind::ARM64_INS_SWPAH: swap halfword (ARMv8.1)
        // case Kind::ARM64_INS_SWPALH: swap halfword (ARMv8.1)
        // case Kind::ARM64_INS_SWPLH: swap halfword (ARMv8.1)
        case Kind::ARM64_INS_SXTB:              return "signed extend byte"; // alias of SBFM
        case Kind::ARM64_INS_SXTH:              return "signed extend halfword"; // alias of SBFM
        // case Kind::ARM64_INS_SXTL: signed extend long (alias of SSHLL, SSHLL2)
        // case Kind::ARM64_INS_SXTL2: signed extend long (alias of SSHLL, SSHLL2)
        case Kind::ARM64_INS_SXTW:              return "sign extend word"; // alias of SBFM
        case Kind::ARM64_INS_SYS:               return "system instruction";
        case Kind::ARM64_INS_SYSL:              return "system instruction with result";
        case Kind::ARM64_INS_TBL:               return "table vector lookup";
        case Kind::ARM64_INS_TBNZ:              return "test bit and branch if nonzero";
        case Kind::ARM64_INS_TBX:               return "table vector lookup extension";
        case Kind::ARM64_INS_TBZ:               return "test bit and branch if zero";
        case Kind::ARM64_INS_TLBI:              return "TLB invalidate operation"; // alias of SYS
        case Kind::ARM64_INS_TRN1:              return "transpose vectors, primary";
        case Kind::ARM64_INS_TRN2:              return "transpose vectors, secondary";
        // case Kind::ARM64_INS_TSB_CSYNC: trace synchronizatio barrier (ARMv8.4)
        case Kind::ARM64_INS_TST:               return "test bits"; // alias of ANDS
        case Kind::ARM64_INS_UABA:              return "unsigned absolute difference and accumulate";
        case Kind::ARM64_INS_UABAL:             return "unsigned absolute difference and accumulate long";
        case Kind::ARM64_INS_UABAL2:            return "unsigned absolute difference and accumulate long";
        case Kind::ARM64_INS_UABD:              return "unsigned absolute difference";
        case Kind::ARM64_INS_UABDL:             return "unsigned absolute difference long";
        case Kind::ARM64_INS_UABDL2:            return "unsigned absolute difference long";
        case Kind::ARM64_INS_UADALP:            return "unsigned add and accumulate long pairwise";
        case Kind::ARM64_INS_UADDL:             return "unsigned add long";
        case Kind::ARM64_INS_UADDL2:            return "unsigned add long";
        case Kind::ARM64_INS_UADDLP:            return "unsigned add long pairwise";
        case Kind::ARM64_INS_UADDLV:            return "unsigned sum long across vector";
        case Kind::ARM64_INS_UADDW:             return "unsigned add wide";
        case Kind::ARM64_INS_UADDW2:            return "unsigned add wide";
        case Kind::ARM64_INS_UBFIZ:             return "unsigned bitfield insert in zeros"; // alias of UBFM
        case Kind::ARM64_INS_UBFM:              return "unsigned bitfield move";
        case Kind::ARM64_INS_UBFX:              return "unsigned bitfield extract";
        case Kind::ARM64_INS_UCVTF:             return "unsigned fixed-point convert to floating-point";
        // case Kind::ARM64_INS_UDF: permanently undefined
        case Kind::ARM64_INS_UDIV:              return "unsigned divide";
        // case Kind::ARM64_INS_UDOT: unsigned dot product (ARMv8.2)
        case Kind::ARM64_INS_UHADD:             return "unsigned halving add";
        case Kind::ARM64_INS_UHSUB:             return "unsigned halving subtract";
        case Kind::ARM64_INS_UMADDL:            return "unsigned multiply-add long";
        case Kind::ARM64_INS_UMAX:              return "unsigned maximum";
        case Kind::ARM64_INS_UMAXP:             return "unsigned maximum pairwise";
        case Kind::ARM64_INS_UMAXV:             return "unsigned maximum across vector";
        case Kind::ARM64_INS_UMIN:              return "unsigned minimum";
        case Kind::ARM64_INS_UMINP:             return "unsigned minimum pairwise";
        case Kind::ARM64_INS_UMINV:             return "unsigned minimum across vector";
        case Kind::ARM64_INS_UMLAL:             return "unsigned multiply-add long";
        case Kind::ARM64_INS_UMLAL2:            return "unsigned multiply-add long";
        case Kind::ARM64_INS_UMLSL:             return "unsigned multiply-subtract long";
        case Kind::ARM64_INS_UMLSL2:            return "unsigned multiply-subtract long";
        case Kind::ARM64_INS_UMNEGL:            return "unsigned multiply-negate"; // alias of UMSUBL
        case Kind::ARM64_INS_UMOV:              return "unsigned move";
        case Kind::ARM64_INS_UMSUBL:            return "unsigned multiply-subtract long";
        case Kind::ARM64_INS_UMULH:             return "unsigned multiply high";
        case Kind::ARM64_INS_UMULL:             return "unsigned multiply long";
        case Kind::ARM64_INS_UMULL2:            return "unsigned multiply long";
        case Kind::ARM64_INS_UQADD:             return "unsigned saturating add";
        case Kind::ARM64_INS_UQRSHL:            return "unsigned saturating rounding shift left";
        case Kind::ARM64_INS_UQRSHRN:           return "unsigned saturating rounded shift right narrow";
        case Kind::ARM64_INS_UQRSHRN2:          return "unsigned saturating rounded shift right narrow";
        case Kind::ARM64_INS_UQSHL:             return "unsigned saturating shift left";
        case Kind::ARM64_INS_UQSHRN:            return "unsigned saturating shift right narrow";
        case Kind::ARM64_INS_UQSHRN2:           return "unsigned saturating shift right narrow";
        case Kind::ARM64_INS_UQSUB:             return "unsigned saturating subtract";
        case Kind::ARM64_INS_UQXTN:             return "unsigned saturating extract narrow";
        case Kind::ARM64_INS_UQXTN2:            return "unsigned saturating extract narrow";
        case Kind::ARM64_INS_URECPE:            return "unsigned reciprocal estimate";
        case Kind::ARM64_INS_URHADD:            return "unsigned rounding halving add";
        case Kind::ARM64_INS_URSHL:             return "unsigned rounding shift left";
        case Kind::ARM64_INS_URSHR:             return "unsigned rounding shift right";
        case Kind::ARM64_INS_URSQRTE:           return "unsigned reciprocal square root estimate";
        case Kind::ARM64_INS_URSRA:             return "unsigned rounding shift right and accumulate";
        case Kind::ARM64_INS_USHL:              return "unsigned shift left";
        case Kind::ARM64_INS_USHLL:             return "unsigned shift left long";
        case Kind::ARM64_INS_USHLL2:            return "unsigned shift left long";
        case Kind::ARM64_INS_USHR:              return "unsigned shift right";
        case Kind::ARM64_INS_USQADD:            return "unsigned saturating accumulate of signed value";
        case Kind::ARM64_INS_USRA:              return "unsigned shift right and accumulate";
        case Kind::ARM64_INS_USUBL:             return "unsigned subtract long";
        case Kind::ARM64_INS_USUBL2:            return "unsigned subtract long";
        case Kind::ARM64_INS_USUBW:             return "unsigned subtract wide";
        case Kind::ARM64_INS_USUBW2:            return "unsigned subtract wide";
        case Kind::ARM64_INS_UXTB:              return "unsigned extend byte"; // alias of UBFM
        case Kind::ARM64_INS_UXTH:              return "unsigned extend halfword"; // alias of UBFM
        // case Kind::ARM64_INS_UXTL: unsigned extend long (alias of USHLL, USHLL2)
        // case Kind::ARM64_INS_UXTL2: unsigned extend long (alias of USHLL, USHLL2)
        case Kind::ARM64_INS_UXTW:              return "undocumented"; //FIXME: present in capstone but not ARM documentation
        case Kind::ARM64_INS_UZP1:              return "unzip vectors, primary";
        case Kind::ARM64_INS_UZP2:              return "unzip vectors, secondary";
        case Kind::ARM64_INS_WFE:               return "wait for event";
        case Kind::ARM64_INS_WFI:               return "wait for interrupt";
        // case Kind::ARM64_INS_XAFlag: convert floating-point condition flags from external to ARM (ARMv8.5)
        // case Kind::ARM64_INS_XAR: exclusive OR and rotate (ARMv8.2)
        // case Kind::ARM64_INS_XPACD: strip pointer authenticate code (ARMv8.3)
        // case Kind::ARM64_INS_XPACI: strip pointer authenticate code (ARMv8.3)
        // case Kind::ARM64_INS_XPACLRI: strip pointer authenticate code (ARMv8.3)
        case Kind::ARM64_INS_XTN:               return "extract narrow";
        case Kind::ARM64_INS_XTN2:              return "extract narrow";
        case Kind::ARM64_INS_YIELD:             return "thread yield hint";
        case Kind::ARM64_INS_ZIP1:              return "zip vectors, primary";
        case Kind::ARM64_INS_ZIP2:              return "zip vectors, secondary";
        case Kind::ARM64_INS_ENDING:            ASSERT_not_reachable("invalid ARM instruction kind");
    }
    ASSERT_not_reachable("invalid AArch64 A64 instruction kind: " + StringUtility::numberToString(get_kind()));
}

#endif
