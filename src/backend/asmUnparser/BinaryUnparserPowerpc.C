#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryUnparserPowerpc.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

void
Powerpc::emitInstruction(std::ostream &out, SgAsmInstruction *insn_, State &state) const {
    ASSERT_not_null(isSgAsmPowerpcInstruction(insn_));
    Base::emitInstruction(out, insn_, state);
}

void
Powerpc::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
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
        state.frontUnparser().emitRegister(out, dre->get_descriptor(), state);

    } else if (SgAsmIntegerValueExpression *ive = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, ive->get_bitVector(), state);
    } else {
        ASSERT_not_implemented(expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

void
Powerpc::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace

#endif
