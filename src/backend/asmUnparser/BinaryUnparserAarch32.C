#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>
#include <BinaryUnparserAarch32.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

void
Aarch32::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn_, State&) const {
    auto insn = isSgAsmAarch32Instruction(insn_);
    out <<insn->get_mnemonic();
}

void
Aarch32::outputRegister(std::ostream &out, SgAsmRegisterReferenceExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    ASSERT_not_null(expr->get_type());
    std::string name = state.registerNames()(expr->get_descriptor());

    ASSERT_require2(isSgAsmIntegerType(expr->get_type()), "not implemented yet");
    out <<name;

    if (settings().insn.operands.showingWidth)
        out <<"[" <<expr->get_descriptor().nBits() <<"]";
}

void
Aarch32::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryAdd *op = isSgAsmBinaryAdd(expr)) {
        outputExpr(out, op->get_lhs(), state);

        // Print the "+" and RHS only if RHS is non-zero
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(op->get_rhs());
        if (!ival || !ival->get_bitVector().isAllClear()) {
            out <<" + ";
            outputExpr(out, op->get_rhs(), state);
        }

    } else if (SgAsmBinaryAddPreupdate *op = isSgAsmBinaryAddPreupdate(expr)) {
        outputExpr(out, op->get_lhs(), state);
        out <<" += ";
        outputExpr(out, op->get_rhs(), state);

    } else if (SgAsmBinaryAddPostupdate *op = isSgAsmBinaryAddPostupdate(expr)) {
            outputExpr(out, op->get_lhs(), state);
            out <<" then ";
            outputExpr(out, op->get_lhs(), state);
            out <<" += ";
            outputExpr(out, op->get_rhs(), state);

    } else if (SgAsmMemoryReferenceExpression *op = isSgAsmMemoryReferenceExpression(expr)) {
        state.frontUnparser().emitTypeName(out, op->get_type(), state);
        out <<" [";
        outputExpr(out, op->get_address(), state);
        out <<"]";

    } else if (SgAsmDirectRegisterExpression *op = isSgAsmDirectRegisterExpression(expr)) {
        outputRegister(out, op, state);

    } else if (SgAsmIntegerValueExpression *op = isSgAsmIntegerValueExpression(expr)) {
        comments = state.frontUnparser().emitSignedInteger(out, op->get_bitVector(), state);

    } else if (SgAsmFloatValueExpression *op = isSgAsmFloatValueExpression(expr)) {
        out <<op->get_nativeValue();

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

    } else if (SgAsmAarch32Coprocessor *op = isSgAsmAarch32Coprocessor(expr)) {
        out <<"p" <<op->coprocessor();

    } else if (SgAsmRegisterNames *op = isSgAsmRegisterNames(expr)) {
        for (size_t i = 0; i < op->get_registers().size(); ++i) {
            out <<(0 == i ? "{" : ", ");
            outputExpr(out, op->get_registers()[i], state);
        }
        out <<"}";

    } else {
        ASSERT_not_implemented(expr->class_name());
    }

    if (!expr->get_comment().empty())
        comments.push_back(expr->get_comment());
    if (!comments.empty())
        out <<"<" + boost::join(comments, ",") <<">";
}

void
Aarch32::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    outputExpr(out, expr, state);
}

} // namespace
} // namespace
} // namespace

#endif
