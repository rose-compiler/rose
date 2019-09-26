#include <sage3basic.h>
#include <BinaryUnparserX86.h>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <SageBuilderAsm.h>
#include <stringify.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

void
X86::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn_, State&) const {
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(insn_);
    ASSERT_not_null2(insn, "not an x86 instruction");
    if (insn->get_lockPrefix())
        out <<"lock ";
    out <<insn->get_mnemonic();
    switch (insn->get_branchPrediction()) {
        case x86_branch_prediction_none: break;
        case x86_branch_prediction_taken: out <<",pt"; break;
        case x86_branch_prediction_not_taken: out <<",pn"; break;
        default:
            ASSERT_not_reachable("bad x86 branch prediction: " +
                                 stringifyBinaryAnalysisX86BranchPrediction(insn->get_branchPrediction()));
    }
}

void
X86::emitTypeName(std::ostream &out, SgAsmType *type, State &state) const {
    if (!type)
        return;

    if (SgAsmIntegerType *it = isSgAsmIntegerType(type)) {
        switch (it->get_nBits()) {
            case 8: out <<"byte"; break;
            case 16: out <<"word"; break;
            case 32: out <<"dword"; break;
            case 64: out <<"qword"; break;
        }
    } else if (SgAsmFloatType *ft = isSgAsmFloatType(type)) {
        switch (ft->get_nBits()) {
            case 32: out <<"float"; break;
            case 64: out <<"double"; break;
            case 80: out <<"ldouble"; break;
        }
    } else if (type == SageBuilderAsm::buildTypeVector(2, SageBuilderAsm::buildTypeU64())) {
        out <<"dqword";
    } else if (SgAsmVectorType *vt = isSgAsmVectorType(type)) {
        out <<"v" <<vt->get_nElmts();
        state.frontUnparser().emitTypeName(out, vt->get_elmtType(), state);
    } else {
        ASSERT_not_reachable("unhandled type: " + type->toString());
    }
}

void
X86::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryExpression *bin = isSgAsmBinaryExpression(expr)) {
        outputExpr(out, bin->get_lhs(), state);
        switch (expr->variantT()) {
            case V_SgAsmBinaryAdd:      out <<" + "; break;
            case V_SgAsmBinarySubtract: out <<" - "; break;
            case V_SgAsmBinaryMultiply: out <<" * "; break;
            default:
                ASSERT_not_implemented(expr->class_name());
        }
        outputExpr(out, bin->get_rhs(), state);

    } else if (SgAsmMemoryReferenceExpression *mr = isSgAsmMemoryReferenceExpression(expr)) {
        SgAsmX86Instruction *insn = SageInterface::getEnclosingNode<SgAsmX86Instruction>(expr);
        bool isLea = insn != NULL && insn->get_kind() == x86_lea;
        if (!isLea) {
            state.frontUnparser().emitTypeName(out, mr->get_type(), state);
            out <<" ";
        }
        if (mr->get_segment()) {
            outputExpr(out, mr->get_segment(), state);
            out <<":";
        }
        out <<"[";
        outputExpr(out, mr->get_address(), state);
        out <<"]";

    } else if (SgAsmDirectRegisterExpression *rr = isSgAsmDirectRegisterExpression(expr)) {
        state.frontUnparser().emitRegister(out, rr->get_descriptor(), state);

    } else if (SgAsmIndirectRegisterExpression *rr = isSgAsmIndirectRegisterExpression(expr)) {
        std::ostringstream ss;
        state.frontUnparser().emitRegister(ss, rr->get_descriptor(), state);
        std::string s = ss.str();
        if (boost::ends_with(s, "0"))
            boost::erase_tail(s, 1);
        out <<s <<"(" <<rr->get_index() <<")";

    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, ival->get_bitVector(), state);

    } else {
        ASSERT_not_implemented("invalid x86 expression: " + expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

void
X86::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace
