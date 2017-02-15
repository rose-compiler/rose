#include <sage3basic.h>
#include <BinaryUnparserMips.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

void
Mips::emitInstruction(std::ostream &out, SgAsmInstruction *insn_, State &state) const {
    SgAsmMipsInstruction *insn = isSgAsmMipsInstruction(insn_);
    ASSERT_not_null2(insn, "not a MIPS instruction");
    Base::emitInstruction(out, insn_, state);
}

void
Mips::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        outputExpr(out, add->get_lhs(), state);
        out <<" + ";
        outputExpr(out, add->get_rhs(), state);

    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
        state.frontUnparser().emitTypeName(out, mre->get_type(), state);
        out <<" [";
        outputExpr(out, mre->get_address(), state);
        out <<"]";

    } else if (SgAsmDirectRegisterExpression *dre = isSgAsmDirectRegisterExpression(expr)) {
        state.frontUnparser().emitRegister(out, dre->get_descriptor(), state);

    } else if (SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, ive->get_bitVector(), state);

    } else {
        ASSERT_not_implemented(expr->class_name());
    }
    

    if (!expr->get_replacement().empty())
        comments.push_back(expr->get_replacement());
    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

void
Mips::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace
