#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64
#include <sage3basic.h>
#include <BinaryUnparserArm.h>

#include <boost/regex.hpp>
#include <stringify.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

// class method
std::string
Arm::unparseArmCondition(Arm64InstructionCondition cond) {
    // These constants come from capstone, so we don't have any easy stringification mechanism for them like we do for ROSE
    // enums.
    switch (cond) {
        case ARM64_CC_INVALID: return "INVALID";
        case ARM64_CC_EQ: return "eq";
        case ARM64_CC_NE: return "ne";
        case ARM64_CC_HS: return "hs";
        case ARM64_CC_LO: return "lo";
        case ARM64_CC_MI: return "mi";
        case ARM64_CC_PL: return "pl";
        case ARM64_CC_VS: return "vs";
        case ARM64_CC_VC: return "vc";
        case ARM64_CC_HI: return "hi";
        case ARM64_CC_LS: return "ls";
        case ARM64_CC_GE: return "ge";
        case ARM64_CC_LT: return "lt";
        case ARM64_CC_GT: return "gt";
        case ARM64_CC_LE: return "le";
        case ARM64_CC_AL: return "al";
        case ARM64_CC_NV: return "nv";
    }
    ASSERT_not_reachable("invalid condition code");
}

void
Arm::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn_, State&) const {
    SgAsmArm64Instruction *insn = isSgAsmArm64Instruction(insn_);
    ASSERT_not_null2(insn, "not an ARM instruction");
    std::string result = insn->get_mnemonic();
    out <<result;
}

void
Arm::outputRegister(std::ostream &out, SgAsmRegisterReferenceExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    ASSERT_not_null(expr->get_type());
    std::string name = state.registerNames()(expr->get_descriptor());

    if (SgAsmVectorType *vector = isSgAsmVectorType(expr->get_type())) {
        ASSERT_not_null(vector->get_elmtType());

        // The qN and vN are different names for the same registers and appear in ARM assembly listings depending on whether
        // their value is being interpreted as a vector. ROSE will try to choose the same name as what is customary in the ARM
        // assembly listings even though this might be confusing when the same physical register appears in two or more
        // instructions and is interpretted different ways.
        boost::regex re("q[0-9]+");
        if (boost::regex_match(name, re))
            name[0] = 'v';

        name += "." + boost::lexical_cast<std::string>(vector->get_nElmts());
        switch (vector->get_elmtType()->get_nBits()) {
            case 8:
                name += "b";
                break;
            case 16:
                name += "h";
                break;
            case 32:
                name += "s";
                break;
            case 64:
                name += "d";
                break;
            case 128:
                name += "v";
                break;
            default:
                ASSERT_not_reachable("invalid vector element size: " +
                                     boost::lexical_cast<std::string>(vector->get_elmtType()->get_nBits()) + " bits");
        }
    }

    out <<name;

    if (settings().insn.operands.showingWidth)
        out <<"[" <<expr->get_descriptor().nBits() <<"]";
}

void
Arm::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        outputExpr(out, add->get_lhs(), state);

        // Print the "+" and RHS only if RHS is non-zero
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(add->get_rhs());
        if (!ival || !ival->get_bitVector().isAllClear()) {
            out <<" + ";
            outputExpr(out, add->get_rhs(), state);
        }

    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
        state.frontUnparser().emitTypeName(out, mre->get_type(), state);
        out <<" [";
        outputExpr(out, mre->get_address(), state);
        out <<"]";

    } else if (SgAsmDirectRegisterExpression *dre = isSgAsmDirectRegisterExpression(expr)) {
        outputRegister(out, dre, state);

    } else if (SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, ive->get_bitVector(), state);

    } else if (SgAsmFloatValueExpression *fp = isSgAsmFloatValueExpression(expr)) {
        out <<fp->get_nativeValue();

    } else if (SgAsmUnaryUnsignedExtend *op = isSgAsmUnaryUnsignedExtend(expr)) {
        out <<"uext(";
        outputExpr(out, op->get_operand(), state);
        out <<", " <<op->get_nBits() <<")";

    } else if (SgAsmUnarySignedExtend *op = isSgAsmUnarySignedExtend(expr)) {
        out <<"sext(";
        outputExpr(out, op->get_operand(), state);
        out <<", " <<op->get_nBits() <<")";

    } else if (SgAsmUnaryTruncate *op = isSgAsmUnaryTruncate(expr)) {
        out <<"trunc(";
        outputExpr(out, op->get_operand(), state);
        out <<", " <<op->get_nBits() <<")";

    } else if (SgAsmBinaryAsr *op = isSgAsmBinaryAsr(expr)) {
        out <<"asr(";
        outputExpr(out, op->get_lhs(), state);
        out <<", ";
        outputExpr(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmBinaryRor *op = isSgAsmBinaryRor(expr)) {
        out <<"ror(";
        outputExpr(out, op->get_lhs(), state);
        out <<", ";
        outputExpr(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmBinaryLsr *op = isSgAsmBinaryLsr(expr)) {
        outputExpr(out, op->get_lhs(), state);
        out <<" >> ";
        outputExpr(out, op->get_rhs(), state);

    } else if (SgAsmBinaryLsl *op = isSgAsmBinaryLsl(expr)) {
        outputExpr(out, op->get_lhs(), state);
        out <<" << ";
        outputExpr(out, op->get_rhs(), state);

    } else if (SgAsmBinaryMsl *op = isSgAsmBinaryMsl(expr)) {
        out <<"msl(";
        outputExpr(out, op->get_lhs(), state);
        out <<", ";
        outputExpr(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmArm64AtOperand *op = isSgAsmArm64AtOperand(expr)) {
        switch (op->operation()) {
            case ARM64_AT_S1E1R:
                out <<"s1e1r";
                break;
            case ARM64_AT_S1E1W:
                out <<"s1e1w";
                break;
            case ARM64_AT_S1E0R:
                out <<"s1e0r";
                break;
            case ARM64_AT_S1E0W:
                out <<"s1e0w";
                break;
            case ARM64_AT_S1E2R:
                out <<"s1e2r";
                break;
            case ARM64_AT_S1E2W:
                out <<"s1e2w";
                break;
            case ARM64_AT_S12E1R:
                out <<"s12e1r";
                break;
            case ARM64_AT_S12E1W:
                out <<"s12e1w";
                break;
            case ARM64_AT_S12E0R:
                out <<"s12e0r";
                break;
            case ARM64_AT_S12E0W:
                out <<"s12e0w";
                break;
            case ARM64_AT_S1E3R:
                out <<"s1e3r";
                break;
            case ARM64_AT_S1E3W:
                out <<"s1e3w";
                break;
            default:
                ASSERT_not_reachable("invalid AT operand");
        }

    } else if (SgAsmArm64PrefetchOperand *op = isSgAsmArm64PrefetchOperand(expr)) {
        switch (op->operation()) {
            case ARM64_PRFM_PLDL1KEEP:  out <<"pldl1keep"; break;
            case ARM64_PRFM_PLDL1STRM:  out <<"pldl1strm"; break;
            case ARM64_PRFM_PLDL2KEEP:  out <<"pldl2keep"; break;
            case ARM64_PRFM_PLDL2STRM:  out <<"pldl2strm"; break;
            case ARM64_PRFM_PLDL3KEEP:  out <<"pldl3keep"; break;
            case ARM64_PRFM_PLDL3STRM:  out <<"pldl3strm"; break;
            case ARM64_PRFM_PLIL1KEEP:  out <<"plil1keep"; break;
            case ARM64_PRFM_PLIL1STRM:  out <<"plil1strm"; break;
            case ARM64_PRFM_PLIL2KEEP:  out <<"plil2keep"; break;
            case ARM64_PRFM_PLIL2STRM:  out <<"plil2strm"; break;
            case ARM64_PRFM_PLIL3KEEP:  out <<"plil3keep"; break;
            case ARM64_PRFM_PLIL3STRM:  out <<"plil3strm"; break;
            case ARM64_PRFM_PSTL1KEEP:  out <<"pstl1keep"; break;
            case ARM64_PRFM_PSTL1STRM:  out <<"pstl1strm"; break;
            case ARM64_PRFM_PSTL2KEEP:  out <<"pstl2keep"; break;
            case ARM64_PRFM_PSTL2STRM:  out <<"pstl2strm"; break;
            case ARM64_PRFM_PSTL3KEEP:  out <<"pstl3keep"; break;
            case ARM64_PRFM_PSTL3STRM:  out <<"pstl3strm"; break;
            default:
                ASSERT_not_reachable("invalid prefetch command");
        }

    } else if (SgAsmArm64SysMoveOperand *op = isSgAsmArm64SysMoveOperand(expr)) {
        unsigned op0 = (op->access() >> 14) & 1;
        unsigned op1 = (op->access() >> 11) & 7;
        unsigned crn = (op->access() >>  7) & 0xf;
        unsigned crm = (op->access() >>  3) & 0xf;
        unsigned op2 = op->access() & 7;
        out <<"s" <<(op0 + 2) <<"_" <<op1 <<"_c" <<crn <<"_c" <<crm <<"_" <<op2;

    } else if (SgAsmArm64CImmediateOperand *op = isSgAsmArm64CImmediateOperand(expr)) {
        out <<"c" <<op->immediate();

    } else {
        ASSERT_not_implemented(expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

void
Arm::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace

#endif
