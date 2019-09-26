#include <sage3basic.h>
#include <BinaryUnparserArm.h>
#include <stringify.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

// class method
std::string
Arm::unparseArmCondition(ArmInstructionCondition cond) {
    std::string retval = stringifyBinaryAnalysisArmInstructionCondition(cond, "arm_cond_");
    ASSERT_require(!retval.empty() && retval[0]!='(');
    return retval;
}

void
Arm::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state, std::string sign, std::string *suffix) const {
    std::ostringstream extra;
    SgAsmUnaryExpression *uno = isSgAsmUnaryExpression(expr);
    SgAsmBinaryExpression *dos = isSgAsmBinaryExpression(expr);
    std::vector<std::string> comments;

    if (!isSgAsmValueExpression(expr)) {
        out <<sign;
        sign = "+";
    }
    
    switch (expr->variantT()) {
        case V_SgAsmBinarySubtractPreupdate:
            sign = "-";
            // fall through
        case V_SgAsmBinaryAddPreupdate: {
            outputExpr(out, dos->get_lhs(), state, "");
            out <<", ";
            outputExpr(out, dos->get_rhs(), state, sign);
            extra <<"!";
            break;
        }

        case V_SgAsmBinarySubtractPostupdate:
            sign = "-";
            // fall through
        case V_SgAsmBinaryAddPostupdate:
            if (suffix) {
                // Appears inside a memory reference expression
                outputExpr(out, dos->get_lhs(), state, "");
                extra <<", ";
                outputExpr(extra, dos->get_rhs(), state, sign);
            } else {
                // Used by LDM* and STM* instructions outside memory reference expressions. RHS is unused.
                outputExpr(out, dos->get_lhs(), state, "");
                out <<"!";
            }
            break;

        case V_SgAsmBinaryAdd:
            outputExpr(out, dos->get_lhs(), state, "");
            out <<", ";
            outputExpr(out, dos->get_rhs(), state, "+");
            break;

        case V_SgAsmBinarySubtract:
            outputExpr(out, dos->get_lhs(), state, "");
            out <<", ";
            outputExpr(out, dos->get_rhs(), state, "+");
            break;

        case V_SgAsmBinaryMultiply:
            outputExpr(out, dos->get_lhs(), state, "");
            out <<"*";
            outputExpr(out, dos->get_rhs(), state, "");
            break;

        case V_SgAsmBinaryLsl:
            outputExpr(out, dos->get_lhs(), state, "");
            out <<", lsl ";
            outputExpr(out, dos->get_rhs(), state, "");
            break;

        case V_SgAsmBinaryLsr:
            outputExpr(out, dos->get_lhs(), state, "");
            out <<", lsr ";
            outputExpr(out, dos->get_rhs(), state, "");
            break;

        case V_SgAsmBinaryAsr:
            outputExpr(out, dos->get_lhs(), state, "");
            out <<", asr ";
            outputExpr(out, dos->get_rhs(), state, "");
            break;

        case V_SgAsmBinaryRor:
            outputExpr(out, dos->get_lhs(), state, "");
            out <<", ror ";
            outputExpr(out, dos->get_rhs(), state, "");
            break;

        case V_SgAsmUnaryRrx:
            outputExpr(out, uno->get_operand(), state, "");
            out <<", rrx";
            break;

        case V_SgAsmUnaryArmSpecialRegisterList:
            outputExpr(out, uno->get_operand(), state, "");
            out <<"^";
            break;

        case V_SgAsmExprListExp: {
            const std::vector<SgAsmExpression*> &exprs = isSgAsmExprListExp(expr)->get_expressions();
            out <<"{";
            for (size_t i = 0; i < exprs.size(); ++i) {
                if (i != 0)
                    out <<", ";
                outputExpr(out, exprs[i], state, "");
            }
            out <<"}";
            break;
        }
            
        case V_SgAsmMemoryReferenceExpression: {
            SgAsmMemoryReferenceExpression *mr = isSgAsmMemoryReferenceExpression(expr);
            SgAsmExpression *addr = mr->get_address();
            switch (addr->variantT()) {
                case V_SgAsmDirectRegisterExpression:
                case V_SgAsmIndirectRegisterExpression:
                case V_SgAsmBinaryAdd:
                case V_SgAsmBinarySubtract:
                case V_SgAsmBinaryAddPreupdate:
                case V_SgAsmBinarySubtractPreupdate:
                case V_SgAsmBinaryAddPostupdate:
                case V_SgAsmBinarySubtractPostupdate:
                    break;
                default: ASSERT_not_reachable("bad addressing mode: " + stringifyVariantT(addr->variantT()));
            }

            std::string suffix;
            state.frontUnparser().emitTypeName(out, expr->get_type(), state);
            out <<" [";
            outputExpr(out, addr, state, "", &suffix);
            out <<"]" <<suffix;
            break;
        }

        case V_SgAsmDirectRegisterExpression: {
            SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(expr);
            state.frontUnparser().emitRegister(out, rre->get_descriptor(), state);
            if (rre->get_descriptor().majorNumber() == arm_regclass_psr && rre->get_psr_mask() !=0) {
                out <<"_";
                if (rre->get_psr_mask() & 1)
                    out <<"c";
                if (rre->get_psr_mask() & 2)
                    out <<"x";
                if (rre->get_psr_mask() & 4)
                    out <<"s";
                if (rre->get_psr_mask() & 8)
                    out <<"f";
            }
            break;
        }

        case V_SgAsmIntegerValueExpression: {
            SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(expr);
            out <<"#" <<sign;
            comments = state.frontUnparser().emitUnsignedInteger(out, ive->get_bitVector(), state);
            break;
        }

        default:
            ASSERT_not_reachable("unhandled expression kind: " + expr->class_name());
    }

    // The extra string should be passed back up the call stack so it can be inserted into the ultimate return string. We can't
    // insert it here because the string can't be generated strictly left-to-right. If "suffix" is the null pointer then the
    // caller isn't expecting a suffix and we'll have to just do our best -- the result will not be valid ARM assembly.
    if (!extra.str().empty()) {
        if (suffix) {
            *suffix = extra.str();
        } else {
            out <<" and \"" <<StringUtility::cEscape(extra.str()) <<"\"";
        }
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

void
Arm::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn_, State&) const {
    SgAsmArmInstruction *insn = isSgAsmArmInstruction(insn_);
    ASSERT_not_null2(insn, "not an ARM instruction");
    std::string result = insn->get_mnemonic();
    std::string cond = unparseArmCondition(insn->get_condition());
    ASSERT_require(insn->get_positionOfConditionInMnemonic() >= 0);
    ASSERT_require((size_t)insn->get_positionOfConditionInMnemonic() <= result.size());
    result.insert(result.begin() + insn->get_positionOfConditionInMnemonic(), cond.begin(), cond.end());
    out <<result;
}

void
Arm::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    SgAsmArmInstruction *insn = SageInterface::getEnclosingNode<SgAsmArmInstruction>(expr);
    ASSERT_not_null(insn);

    if (insn->get_kind() == arm_b || insn->get_kind() == arm_bl) {
        ASSERT_require(insn->nOperands() == 1);
        ASSERT_require(insn->operand(0) == expr);
        SgAsmIntegerValueExpression *tgt = isSgAsmIntegerValueExpression(expr);
        ASSERT_not_null(tgt);
        state.frontUnparser().emitAddress(out, tgt->get_bitVector(), state);
    } else {
        outputExpr(out, expr, state, "");
    }
}

} // namespace
} // namespace
} // namespace
