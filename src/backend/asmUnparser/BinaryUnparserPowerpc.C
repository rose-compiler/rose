#include <sage3basic.h>
#include <BinaryUnparserPowerpc.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

void
UnparserPowerpc::emitInstruction(std::ostream &out, SgAsmInstruction *insn_, State &state) const {
    SgAsmPowerpcInstruction *insn = isSgAsmPowerpcInstruction(insn_);
    ASSERT_not_null2(insn, "not a PowerPC instruction");
    UnparserBase::emitInstruction(out, insn_, state);
}

void
UnparserPowerpc::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        emitOperandBody(out, add->get_lhs(), state);
        out <<" + ";
        emitOperandBody(out, add->get_rhs(), state);

    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
        emitTypeName(out, mre->get_type(), state);
        out <<" [";
        emitOperandBody(out, mre->get_address(), state);
        out <<"]";

    } else if (SgAsmDirectRegisterExpression *dre = isSgAsmDirectRegisterExpression(expr)) {
        emitRegister(out, dre->get_descriptor(), state);

    } else if (SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(expr)) {
        comments = emitSignedInteger(out, ive->get_bitVector(), state);
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
    
} // namespace
} // namespace
} // namespace
