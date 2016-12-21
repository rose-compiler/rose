#include <sage3basic.h>
#include <BinaryUnparserM68k.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

void
UnparserM68k::emitInstruction(std::ostream &out, SgAsmInstruction *insn_, State &state) const {
    SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(insn_);
    ASSERT_not_null2(insn, "not an m68k instruction");
    UnparserBase::emitInstruction(out, insn_, state);
}

void
UnparserM68k::emitOperandBody(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(expr)) {
        int adjustment = rre->get_adjustment();
        if (adjustment < 0)
            out <<"--";
        emitRegister(out, rre->get_descriptor(), state);
        if (adjustment > 0)
            out <<"++";

    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(expr)) {
        emitTypeName(out, expr->get_type(), state);
        out <<" [";
        emitOperandBody(out, mre->get_address(), state);
        out <<"]";

    } else if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(expr)) {
        emitOperandBody(out, add->get_lhs(), state);
        out <<"+";
        emitOperandBody(out, add->get_rhs(), state);

    } else if (SgAsmBinaryMultiply *mul = isSgAsmBinaryMultiply(expr)) {
        emitOperandBody(out, mul->get_lhs(), state);
        out <<"*";
        emitOperandBody(out, mul->get_rhs(), state);

    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(expr)) {
        comments = emitSignedInteger(out, ival->get_bitVector(), state);

    } else if (SgAsmRegisterNames *regs = isSgAsmRegisterNames(expr)) {
        // The usual assembly is to show only an integer register mask.  That's not very friendly, especially since the meaning
        // of the bits is dependent on the addressing mode of the other instruction.  So we show the register names instead in
        // curly braces.
        int nregs = 0;
        out <<"{";
        BOOST_FOREACH (SgAsmRegisterReferenceExpression *rre, regs->get_registers()) {
            if (++nregs > 1)
                out <<", ";
            emitOperandBody(out, rre, state);
        }
        out <<"}";
        if (regs->get_mask()!=0)
            comments.push_back(StringUtility::toHex2(regs->get_mask(), 16, false, false));

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
