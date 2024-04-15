#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <Rose/BinaryAnalysis/Unparser/Aarch32.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <SgAsmAarch32Coprocessor.h>
#include <SgAsmAarch32Instruction.h>
#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryAsr.h>
#include <SgAsmBinaryConcat.h>
#include <SgAsmBinaryLsl.h>
#include <SgAsmBinaryLsr.h>
#include <SgAsmBinaryMsl.h>
#include <SgAsmBinaryPostupdate.h>
#include <SgAsmBinaryPreupdate.h>
#include <SgAsmBinaryRor.h>
#include <SgAsmBinarySubtract.h>
#include <SgAsmByteOrder.h>
#include <SgAsmFloatValueExpression.h>
#include <SgAsmIntegerType.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmRegisterNames.h>
#include <SgAsmRegisterReferenceExpression.h>
#include <SgAsmUnarySignedExtend.h>
#include <SgAsmUnaryTruncate.h>
#include <SgAsmUnaryUnsignedExtend.h>

#include <Cxx_GrammarDowncast.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
unparseAarch32Mnemonic(SgAsmAarch32Instruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

std::string
unparseAarch32Expression(SgAsmExpression *expr, const LabelMap*, RegisterDictionary::Ptr registers) {
    auto arch = Architecture::findByName("arm-a32").orThrow();

    if (!registers)
        registers = arch->registerDictionary();
    auto unparser = arch->newUnparser();
    std::ostringstream ss;
    auto p = Partitioner2::Partitioner::instance(arch);
    State state(p, registers, unparser->settings(), *unparser);
    unparser->emitOperand(ss, expr, state);
    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Aarch32
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Aarch32::~Aarch32() {}

Aarch32::Aarch32(const Architecture::Base::ConstPtr &arch, const Aarch32Settings &settings)
    : Base(arch), settings_(settings) {}

Aarch32::Ptr
Aarch32::instance(const Architecture::Base::ConstPtr &arch, const Aarch32Settings &settings) {
    return Ptr(new Aarch32(arch, settings));
}

Base::Ptr
Aarch32::copy() const {
    return instance(architecture(), settings());
}

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

int
Aarch32::operatorPrecedence(SgAsmExpression *expr) {
    ASSERT_not_null(expr);
    if (isSgAsmBinaryPostupdate(expr) ||
        isSgAsmBinaryPreupdate(expr)) {
        return 0;                                       // these look like "X then Y" or "X after Y"

    } else if (isSgAsmBinaryLsr(expr) ||
               isSgAsmBinaryLsl(expr)) {
        return 1;                                       // shift/rotate


    } else if (isSgAsmBinaryAdd(expr) ||
               isSgAsmBinarySubtract(expr)) {
        return 2;

    } else if (isSgAsmBinaryConcat(expr)) {
        return 3;

    } else if (isSgAsmMemoryReferenceExpression(expr) ||
               isSgAsmDirectRegisterExpression(expr) ||
               isSgAsmIntegerValueExpression(expr) ||
               isSgAsmFloatValueExpression(expr) ||
               isSgAsmUnaryUnsignedExtend(expr) ||
               isSgAsmUnarySignedExtend(expr) ||
               isSgAsmUnaryTruncate(expr) ||
               isSgAsmBinaryAsr(expr) ||
               isSgAsmBinaryRor(expr) ||
               isSgAsmBinaryMsl(expr) ||
               isSgAsmAarch32Coprocessor(expr) ||
               isSgAsmByteOrder(expr) ||
               isSgAsmRegisterNames(expr)) {
        return 4;

    } else {
        ASSERT_not_reachable("invalid operator for AArch32 disassembly");
    }
}

Aarch32::Parens
Aarch32::parensForPrecedence(int rootPrec, SgAsmExpression *subexpr) {
    ASSERT_not_null(subexpr);
    int subPrec = operatorPrecedence(subexpr);
    if (subPrec < rootPrec) {
        return Parens("(", ")");
    } else {
        return Parens();
    }
}

void
Aarch32::outputExpr(std::ostream &out, SgAsmExpression *expr, State &state) const {
    ASSERT_not_null(expr);
    std::vector<std::string> comments;

    if (SgAsmBinaryAdd *op = isSgAsmBinaryAdd(expr)) {
        // Print the "+" and RHS only if RHS is non-zero
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(op->get_rhs());
        if (!ival || !ival->get_bitVector().isAllClear()) {
            int prec = operatorPrecedence(expr);
            Parens parens = parensForPrecedence(prec, op->get_lhs());
            out <<parens.left;
            outputExpr(out, op->get_lhs(), state);
            out <<parens.right;

            out <<" + ";

            parens = parensForPrecedence(prec, op->get_rhs());
            out <<parens.left;
            outputExpr(out, op->get_rhs(), state);
            out <<parens.right;
        } else {
            outputExpr(out, op->get_lhs(), state);
        }

    } else if (SgAsmBinarySubtract *op = isSgAsmBinarySubtract(expr)) {
        // Print the "-" and RHS only if RHS is non-zero
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(op->get_rhs());
        if (!ival || !ival->get_bitVector().isAllClear()) {
            int prec = operatorPrecedence(expr);
            Parens parens = parensForPrecedence(prec, op->get_lhs());
            out <<parens.left;
            outputExpr(out, op->get_lhs(), state);
            out <<parens.right;

            out <<" - ";

            parens = parensForPrecedence(prec, op->get_rhs());
            out <<parens.left;
            outputExpr(out, op->get_rhs(), state);
            out <<parens.right;
        } else {
            outputExpr(out, op->get_lhs(), state);
        }

    } else if (SgAsmBinaryPreupdate *op = isSgAsmBinaryPreupdate(expr)) {
        outputExpr(out, op->get_lhs(), state);
        out <<" (after ";
        outputExpr(out, op->get_lhs(), state);
        out <<" = ";
        outputExpr(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmBinaryPostupdate *op = isSgAsmBinaryPostupdate(expr)) {
        outputExpr(out, op->get_lhs(), state);
        out <<" (then ";
        outputExpr(out, op->get_lhs(), state);
        out <<" = ";
        outputExpr(out, op->get_rhs(), state);
        out <<")";

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
        int prec = operatorPrecedence(expr);
        Parens parens = parensForPrecedence(prec, op->get_lhs());
        out <<parens.left;
        outputExpr(out, op->get_lhs(), state);
        out <<parens.right;

        out <<" >> ";

        parens = parensForPrecedence(prec, op->get_rhs());
        out <<parens.left;
        outputExpr(out, op->get_rhs(), state);
        out <<parens.right;

    } else if (SgAsmBinaryLsl *op = isSgAsmBinaryLsl(expr)) {
        int prec = operatorPrecedence(expr);
        Parens parens = parensForPrecedence(prec, op->get_lhs());
        out <<parens.left;
        outputExpr(out, op->get_lhs(), state);
        out <<parens.right;

        out <<" << ";

        parens = parensForPrecedence(prec, op->get_rhs());
        out <<parens.left;
        outputExpr(out, op->get_rhs(), state);
        out <<parens.right;

    } else if (SgAsmBinaryMsl *op = isSgAsmBinaryMsl(expr)) {
        out <<"msl(";
        outputExpr(out, op->get_lhs(), state);
        out <<", ";
        outputExpr(out, op->get_rhs(), state);
        out <<")";

    } else if (SgAsmAarch32Coprocessor *op = isSgAsmAarch32Coprocessor(expr)) {
        out <<"p" <<op->coprocessor();

    } else if (SgAsmByteOrder *op = isSgAsmByteOrder(expr)) {
        switch (op->byteOrder()) {
            case ByteOrder::ORDER_MSB:
                out <<"be";
                break;
            case ByteOrder::ORDER_LSB:
                out <<"le";
                break;
            default:
                ASSERT_not_reachable("invalid byte order " + boost::lexical_cast<std::string>(op->byteOrder()));
        }

    } else if (SgAsmRegisterNames *op = isSgAsmRegisterNames(expr)) {
        for (size_t i = 0; i < op->get_registers().size(); ++i) {
            out <<(0 == i ? "{" : ", ");
            outputExpr(out, op->get_registers()[i], state);
        }
        out <<"}";

    } else if (SgAsmBinaryConcat *op = isSgAsmBinaryConcat(expr)) {
        int prec = operatorPrecedence(expr);
        Parens parens = parensForPrecedence(prec, op->get_lhs());
        out <<parens.left;
        outputExpr(out, op->get_lhs(), state);
        out <<parens.right;

        out <<":";
        parens = parensForPrecedence(prec, op->get_rhs());
        out <<parens.left;
        outputExpr(out, op->get_rhs(), state);
        out <<parens.right;

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
